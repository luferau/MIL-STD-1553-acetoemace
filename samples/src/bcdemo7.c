/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) sample  rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * bcdemo7.c
 *          This example loads minor frames created with BCDEMO6, and
 *          executes them in a major frame format. The BC end-of-frame
 *          interrupt is used in conjuction with the ACE internal double
 *          buffering to enable the bus controller to be transmitting
 *          a minor frame with the active stack while the inactive stack can
 *          be captured (and stored to disk as in this program) and then
 *          loaded with the next minor frame. The bus controller is
 *          used in auto-repeat mode. This allows the automatic swapping of
 *          the active and inactive stack areas and loading of the frame
 *          timer.
 *
 *          The stack is output to 'ace.bsf' this can be decoded with the
 *          'decodbsf.c' sample program.
 */

/* Remove warnings for deprecated functions */
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/* Common headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdace.h>

#ifdef _ACELINUX
/* Linux Headers */
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "aceutils.h"
#define getch() readch()
#define Sleep   sleep
#else
/* DOS/Windows Headers */
#include <sys\stat.h>
#include <fcntl.h>
#include <conio.h>
#include <io.h>
#endif

// 100 bytes max per message and 64 messages per minor frame
// max read size is 100 * 64 = 6400 bytes

#define MAX_STK_READ_SIZE 3200

#ifdef _Windows
#define BUF_SIZE (12*1024)
#define NUM_BUFS 8
#else
#define BUF_SIZE (12*1024)
#define NUM_BUFS 2
#endif

#define BUF_FULL (BUF_SIZE - MAX_STK_READ_SIZE - 100 )

void __BUDECL BcHandler(U16BIT IntStatus);

long totaltm=0;           // total time of run
int  frmnum=5;            // number of minor frames in major
int  frmdex=0;            // index (keeps track of last loaded minor frame)
int  frmrun=100;//20000;        // number of times to run major frame
long bytesread=0;         // number of bytes read with BuBCReadStk
long bytesstored=0;       // number of bytes written to disk
int  errorreadingstack=0; // number of BuBCReadStk errors
long frmcount=0;          // current major frame number

BCMinorFrmHandle frame[10];  /* each minor frame has a handle */

int main (void)
{
	BuConf_t  Conf; // ACE library configuration
	BuError_t err;  // ACE library stderr
	int x;
	int result;
	int outhandle;
	U16BIT* Ptr;
	U16BIT WordCount;
	U32BIT ByteCount;

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

	/* display revision info */
	printf("%s\n\n",BuRev());
	printf("\nThis BC Demo loads minor frames created with BCDEMO6 and outputs stack to \n");
	printf("a .bsf file\n\n");

	/* setup configuration for device X and open ACE library */
	printf("Choose the logical device # of your BC:> ");
	scanf("%d",(int*)&Conf.ConfDev);

#ifdef _ACELINUX
	err = BuOpenLinux(&Conf);
#else
	err = BuOpen32(&Conf);
#endif

	if(err)
	{
		printf("BuError %d %s\n",err,BuErrorStr(err));
		getch();

#ifdef _ACELINUX
		AceUtilsExit();  /* Exit Ace Utilities */
#endif
		return 0;
	}

	/* check for valid ACE then enter mainline code */
	if(BuValid())
	{
		/* create output file (ace bc stack file) */
#ifndef _ACELINUX
		_fmode=O_BINARY;
#endif
		outhandle=creat("ACE.BSF",S_IWRITE|S_IREAD);
		if(outhandle==-1) {
			printf("error opening output file\n");
			BuClose();

#ifdef _ACELINUX
			AceUtilsExit();  /* Exit Ace Utilities */
#endif
			return 0;
		}

		/* enter bus controller mode of operation */
		BuBCOpen();
		BuBufOpen();

		result=BuCreateRWBuffers(NUM_BUFS,BUF_SIZE,BUF_FULL);

		if(result!=NUM_BUFS)
		{
			printf("unable to allocate monitor buffers\n");
			printf("    (%d buffers requested, only %d allocated)\n",NUM_BUFS,result);
			BuFreeRWBuffers();
			BuBufClose();
			BuBCClose();
			BuClose();

#ifdef _ACELINUX
		    AceUtilsExit();  /* Exit Ace Utilities */
#endif
			return 0;
		}

		/* set response timeout to 50.5 us */
		BuTimeout(RESPONSE_505);

		/* install my interrupt handler */
		BuInstallUsrHandler(BcHandler);

		/* enable bc end of frame and end of message interrupts */
		BuIrqEnable(IRQ_BC_END_OF_FRAME);

		/* recall minor frames from file */
		if( (!BuBCRecallMinor("frame1.frm",&frame[0]))|
			(!BuBCRecallMinor("frame2.frm",&frame[1]))|
			(!BuBCRecallMinor("frame3.frm",&frame[2]))|
			(!BuBCRecallMinor("frame4.frm",&frame[3]))|
			(!BuBCRecallMinor("frame5.frm",&frame[4])))
		{
			printf("error loading .frm file\n");
			BuFreeRWBuffers();
			BuBufClose();
			BuBCClose();
			BuClose();

#ifdef _ACELINUX
			AceUtilsExit();  /* Exit Ace Utilities */
#endif
			return 0;
		}

		/* load minor frame into ACE stack A */
		BuBCLoadMinor(BU_BCFRMBUFA,frame[frmdex]);

		/* load minor frame into ACE stack B */
		frmdex=((frmdex+1)%frmnum);
		BuBCLoadMinor(BU_BCFRMBUFB,frame[frmdex]);

		/* run frame */
		BuBCRunMinor(BU_BCFRMBUFA,BU_BCREPEAT);

		// if there are buffers store them to disk, otherwise we
		// have enough idle time to worry about screen refresh
		while((!kbhit())&&(frmcount<10))
		{
			if((BuGetReadBufPtr(&Ptr,&WordCount)==BUF_BUFFERFULL))
			{
				ByteCount=write(outhandle,(char*)Ptr,WordCount*sizeof(U16BIT));
				BuResetReadBuf();
				bytesstored+=ByteCount;
			} else 
			{
				printf("Time=%06dms  ",(int)totaltm/1000);
				printf("MjrFrms=%06d  ",(int)frmcount);
				printf("BytesRead=%06d  ",(int)bytesread);
				printf("BytesStored=%06d\r",(int)bytesstored);
			}
		}	
		
		// wait for current major to end (if we aborted with keypress
		if(kbhit()){
			int t=frmcount;
			while((t==frmcount));
		}
		
		// wait for last frame to flip
		
		{int tp=BuBCActiveStk();while(tp==BuBCActiveStk());}
		
		// last frame, stop on this one
		BuStopOnFrame();
		
		// wait till last frame ends
		while (BuBCIsFrmActive());
		
		// write remaining buffers to disk
		do {
			BuGetReadBufPtr(&Ptr, &WordCount);
			if(WordCount);
			{
				int long ByteCount=write(outhandle,(char*)Ptr,WordCount*sizeof(U16BIT));
				BuResetReadBuf();
				bytesstored+=ByteCount;
			}
		}
		while(WordCount);
		
		// update with latest and last counters
		printf("Time=%06dms  ",(int)totaltm/1000);
		printf("MjrFrms=%06d  ",(int)frmcount);
		printf("BytesRead=%06d  ",(int)bytesread);
		printf("BytesStored=%06d\n",(int)bytesstored);
		
		// allow time for interrupts to complete before continuing
		Sleep(1000);        
		
		// free minor frame and associated messages
		{for(x=0;x<frmnum;x++)BuBCFreeMinor(frame[x],TRUE);}
		
		// close the .bsf file, free the buffers, close the buffer module
		// and close the bus cotroller mode
		close(outhandle);
		BuFreeRWBuffers();
		BuBufClose();
		BuBCClose();
		
  } else printf("error communicating with pc card\n");
  
  BuClose();

#ifdef _ACELINUX
	AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}

// BcHandler is a user defined hook into the interrupt service routine
// in this example, the BC end-of-frame interrupt is used to read the
// inactive stack (store to a buffer), load the next minor frame, and
// check for major frame completion
void __BUDECL BcHandler(U16BIT IntStatus)
{
	// buflen is intially set the the largest readable stack size
	U16BIT buflen=MAX_STK_READ_SIZE;
	U16BIT *bufptr;

	if(IntStatus&IRQ_BC_END_OF_FRAME)
	{
		// frames have swapped so load the minor frame time of the
		// active frame and swap buffers (the swap buffers do not take
		// effect now, but at the end of the current frame). total time
		// is incremented by the time of the inactive (last) minor frame
		BuBCSetFrameTime(frame[frmdex]->FrameTime);
		BuBCSetActiveStk((U16BIT)(BuBCActiveStk()^BU_BCFRMBUFB));
		totaltm+=frame[((frmdex==0)?(frmnum-1):(frmdex-1))]->FrameTime;
		
		// get write buffer pointer
		bufptr=BuGetWriteBufPtr();
		
		if(bufptr)
		{
			// read inactive bus controller stack into buffer
			// update the total bytes read
			if(BuBCReadStk(frame[((frmdex==0)?(frmnum-1):(frmdex-1))],bufptr,&buflen,BCmode))
				errorreadingstack++;
			BuUpdateWriteBuf(buflen);
			bytesread+=buflen<<1;
		}
		
		// frmdex is incremented and the next minor frame on the queue
		// is inserted into the inactive stack the MAJOR frame has been
		// totally loaded if we have begun to load the next MAJOR frame
		frmdex=((frmdex+1)%frmnum);
		BuBCLoadMinor((U16BIT)(BuBCActiveStk()^BU_BCFRMBUFB),frame[frmdex]);
		if(frmdex==0)frmcount++;
	}
}
