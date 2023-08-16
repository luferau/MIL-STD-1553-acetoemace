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
 * mt2disk.c
 *          Monitor demo program monitors all messages, and writes
 *          the monitored data into an ACE Stack File (ACE.ASF)
 *          which is a binary dump of the ACE monitor stack. The
 *          program stores data continuously to the file. The file
 *          may be decoded and displayed using the decodasf sample
 *          program.
 *
 *          Note the use of the BuConf.MT data members MTcount and
 *          MTerror which are initialized in BuOpen() to 0, and are
 *          incremented in the BuMTReadStk() when a message is stored
 *          to the local buffer. MTerror is incremented in the event
 *          of a set error bit in the monitor block status word.
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
#else
/* DOS/Windows Headers */
#include <sys\stat.h>
#include <fcntl.h>
#include <conio.h>
#include <io.h>
#endif

/* max read size is 256 + 1024 */
#define MAX_STK_READ_SIZE (2*1024)

#ifdef _Windows
#define BUF_SIZE (12*1024)
#define NUM_BUFS 8
#else
#define BUF_SIZE (12*1024)
#define NUM_BUFS 2
#endif

#define BUF_FULL (BUF_SIZE - MAX_STK_READ_SIZE - 200 )

#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif

void __BUDECL MyHandler(U16BIT IntStatus);

U16BIT tp = 0;
U16BIT DataOverRun = 0;

U16BIT ErrorReadingStack = 0;
U32BIT TotalBytesMonitored = 0;

int main(void)
{
  int DisplayTimeOut;
  int out_handle;
  U16BIT result;
  U32BIT TotalBytesStored = 0;
  U16BIT* Ptr;
  U16BIT WordCount;

  BuConf_t Conf;
  BuError_t Err;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis MT demo monitors all messages, and writes the monitored data\n");
  printf("into an ACE Stack File (ACE.ASF).\n\n");

  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # of your MT:> ");
  scanf("%d",(int*)&Conf.ConfDev);

#ifdef _ACELINUX
  Err=BuOpenLinux(&Conf);
#else
  Err=BuOpen32(&Conf);
#endif

  if(Err) {
    printf("BuError %d %s\n",Err,BuErrorStr(Err));

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif

    return 0;
  }

  if(BuValid()) {

    /* create output file (ace stack file) */
#ifndef _ACELINUX
	_fmode=O_BINARY;
#endif

    out_handle=creat("ACE.ASF",S_IWRITE|S_IREAD);
    if(out_handle==-1) {
      printf("error opening output file\n");
      BuClose();

#ifdef _ACELINUX
      AceUtilsExit();  /* Exit Ace Utilities */
#endif
      return 0;
    }

    BuMTOpen();
    BuBufOpen();

    result=BuCreateRWBuffers(NUM_BUFS,BUF_SIZE,BUF_FULL);

    if(result!=NUM_BUFS) {
      printf("unable to allocate monitor buffers\n");
      printf("    (%d buffers requested, only %d allocated)\n",NUM_BUFS,result);
      BuFreeRWBuffers();
      BuMTClose();
      BuClose();

#ifdef _ACELINUX
      AceUtilsExit();  /* Exit Ace Utilities */
#endif
	  return 0;
    }

    /* install interrupt handler */
    BuInstallUsrHandler(MyHandler);

    /* enable command and data stack rollover interrupts */
    BuIrqEnable( IRQ_MT_CMD_STACK_ROLLOVR|
		 IRQ_MT_DTA_STACK_ROLLOVR);

    /* start the monitor up */
    BuMTRun();

    printf("\nBUS-69082(3) MONITOR TO DISK PROGRAM\n\n");
    printf("Press Any Key to Exit.\n\n");
    DisplayTimeOut=20;

    do {
		U32BIT ByteCount;
		if( (BuGetReadBufPtr(&Ptr,&WordCount)==BUF_BUFFERFULL)&&(DisplayTimeOut--))
		{
			ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
			BuResetReadBuf();
			TotalBytesStored += ByteCount;
		}else
		{
			/* only update the screens if the read buffer */
			/* is empty (give priority to disk storage) */
			printf("TBytesMon  %05u   ",TotalBytesMonitored);
			printf("TBytesStor  %05u   ",TotalBytesStored);
			printf("MTCount  %05u   ",Conf.MT->MTcount); /* added [01-SEP-1995] */
			printf("MTError  %05u\r",Conf.MT->MTerror); /* added [01-SEP-1995] */
			DisplayTimeOut=20;
		}

		if(DataOverRun){
			printf("Data Over Run Occured");
		}

		if(ErrorReadingStack) {
	    	printf("Error Reading Stack Occured, 0x%04x",ErrorReadingStack);
			ErrorReadingStack=0;
		}
	}while(!kbhit());

       BuMTHalt();

       /* dump remaining data buffer to disk to make sure */
       /* that we have room for any new data which may be on stack */

       while(BuGetReadBufPtr(&Ptr,&WordCount)) {
	    int long ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
	    BuResetReadBuf();
	    TotalBytesStored+=ByteCount;
       }

       /* call my handler to read what ever is left */
       /* on stack into buffer */
       MyHandler(IRQ_MT_CMD_STACK_ROLLOVR);

       /* dump remaining data in buffer to disk */
       do {
		   BuGetReadBufPtr(&Ptr, &WordCount);
		   if(WordCount);
		   {
			   int long ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
			   BuResetReadBuf();
			   
			   TotalBytesStored+=ByteCount;
		   }
	   }while(WordCount);


	   printf("TBytesMon  %05u   ",TotalBytesMonitored);
	   printf("TBytesStor  %05u   ",TotalBytesStored);
	   printf("MTCount  %05u   ",Conf.MT->MTcount);
	   printf("MTError  %05u\r",Conf.MT->MTerror);

       close(out_handle);
       BuFreeRWBuffers();
       BuMTClose();
       BuBufClose();
    } else printf("error communicating with hardware\n");

    BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}


void __BUDECL MyHandler(U16BIT IntStatus)
{
  U16BIT MonStkLen=MAX_STK_READ_SIZE;
  U16BIT *BufPtr;

  /* check if the command stack or the data stack have rolled over */
  if( (IntStatus&IRQ_MT_CMD_STACK_ROLLOVR)||
      (IntStatus&IRQ_MT_DTA_STACK_ROLLOVR)) {

    /* get current write buffer pointer */
    BufPtr=BuGetWriteBufPtr();

    if(BufPtr) {

      /* read monitor stack into buffer pointer */
      if(BuMTReadStk(BufPtr,&MonStkLen)==BU_ERROR_MTSTKBUFOVR)
        ErrorReadingStack++;
      BuUpdateWriteBuf(MonStkLen);

      TotalBytesMonitored+=MonStkLen*sizeof(U16BIT);
    }
  }
}
