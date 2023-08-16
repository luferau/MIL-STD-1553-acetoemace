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
 * rtdemo12.c
 *           This software sets up 32*3 (tx,rx,bcst) word data blocks for
 *           each subaddress. The programs uses an end of message interrupt
 *           to store stack information to the disk. The BuRTReadMsg()
 *           function is used to read message information from the ACE. The
 *           ACE.RSF file created can be decoded with the DECODRSF.C sample
 *           program.
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

BuConf_t Conf; /* ACE library configuration type */

#define  RTaddress   5
#define  active_sa   16

#define COMMAND_FIELDS(wrd) (*((CmdWordType*)&wrd))

/* max read size is 256 + 1024 */
#define MAX_STK_READ_SIZE (2*1024)
#define BUF_SIZE (6*1024)
#define NUM_BUFS 4
#define BUF_FULL (BUF_SIZE - MAX_STK_READ_SIZE - 100 )

/* array to keep counters on messages */
U32BIT  TotalMessages[active_sa];
/* structure to hold message information */
MsgType msg;
/* keeps track of total bytes read from ACE */
U32BIT  TotalBytesMonitored = 0;

int volatile DisplayInfo=0;

/* the ISR is setup to interrupt at the end of a message. at this
 * point, the BuRTReadMsg() function is used to read any new message
 * from the stack. The timeout is to prevent a race condition if
 * messages are comming in faster than they can be read out. A write
 * buffer is pulled and updated with message information. TotalByesMonitored
 * is updated with the size of the message.
 */

void __BUDECL MyIsr(U16BIT status)
{
  U16BIT *BufPtr;
  int timeout=0,x;
  BuIrqDisable(IRQ_END_OF_MESSAGE);

  if(status&IRQ_END_OF_MESSAGE)
  {
	  BufPtr=BuGetWriteBufPtr();

	  if(BufPtr)
	  {
		  while((BuRTReadMsg(LAST_MESSAGE,&msg)==BU_SUCCESS)&&(timeout<25))
	      {
			TotalMessages[COMMAND_FIELDS(msg.CmdWord1).SubAddr]++;
			*(BufPtr++)=msg.BlockStatus;
			*(BufPtr++)=msg.TimeTag;
			*(BufPtr++)=msg.CmdWord1;
			for(x=0;x<msg.WordCount;x++)
				*(BufPtr++)=msg.Data[x];
			BuUpdateWriteBuf(msg.WordCount+3);
			TotalBytesMonitored+=(msg.WordCount+3)*sizeof(U16BIT);
			DisplayInfo++;
		  }
	  }

	  BuIrqEnable(IRQ_END_OF_MESSAGE);
  }
}/*end MyIsr*/

int main(void)
{
  BuError_t Err;  /* ACE library error status type  */
  RTBlkHandle sa_rx_buf[active_sa];
  RTBlkHandle sa_tx_buf[active_sa];
  RTBlkHandle sa_bs_buf[active_sa];
  SubAddrCtrlWrd sub_addr_config;
  int out_handle;
  int x;
  U16BIT result;
  U16BIT* Ptr;
  U16BIT WordCount;
  U32BIT TotalBytesStored = 0;
  U32BIT ByteCount;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 with 32*3 (tx,rx,bcst) word data blocks for\n");
  printf("subaddress's 0-15 and uses an ISR to store stack info to disk.\n\n");

  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # for your RT:> ");
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

  /* create output file (ace stack file) */
#ifndef _ACELINUX
  _fmode=O_BINARY;
#endif
  out_handle=creat("ACE.RSF",S_IWRITE|S_IREAD);
  if(out_handle==-1) {
    printf("error opening output file\n");
    BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
    return 0;
  }

  /* open rt mode for access */
  BuRTOpen();
  BuBufOpen();

  result=BuCreateRWBuffers(NUM_BUFS,BUF_SIZE,BUF_FULL);

  if(result!=NUM_BUFS) {
    printf("unable to allocate monitor buffers\n");
    printf("    (%d buffers requested, only %d allocated)\n",NUM_BUFS,result);
    BuFreeRWBuffers();
    BuRTClose();
    BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
    return 0;
  }

	/* set the rt address */
	BuRTAddress(RTaddress);

	/*define all messages illegal to start with*/
	BuRTDefMsgIllegal(ALL,ALL,ALL);

	/*define subadress*/
	sub_addr_config.BcstEomInt = FALSE;
	sub_addr_config.RxEomInt   = FALSE;
	sub_addr_config.TxEomInt   = FALSE;
	sub_addr_config.BcstBuffInt = FALSE;
	sub_addr_config.RxBuffInt   = FALSE;
	sub_addr_config.TxBuffInt   = FALSE;
	sub_addr_config.BcstMm = SINGLE_MESSAGE;
	sub_addr_config.RxMm   = SINGLE_MESSAGE;
	sub_addr_config.TxMm   = SINGLE_MESSAGE;

	/*allocate data blocks*/
	for(x=0;x<active_sa;x++)
	{
		BuRTDefSA((U16BIT)x,&sub_addr_config);
		sa_rx_buf[x]=BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
		sa_tx_buf[x]=BuRTAllocBlk((U8BIT)sub_addr_config.TxMm);
		sa_bs_buf[x]=BuRTAllocBlk((U8BIT)sub_addr_config.BcstMm);
			
		if((sa_rx_buf[x]==NULL)||(sa_tx_buf[x]==NULL)||(sa_bs_buf[x]==NULL))
		{
			printf("cannot allocate data block for all subaddress's\n");
			BuRTClose();
			BuClose();

#ifdef _ACELINUX
            AceUtilsExit();  /* Exit Ace Utilities */
#endif
			return 0;
		}
		
		BuRTMapBlk((U16BIT)x, RECEIVE, sa_rx_buf[x], 0);
		BuRTMapBlk((U16BIT)x, TRANSMIT, sa_tx_buf[x], 0);
		BuRTMapBlk((U16BIT)x, BROADCAST, sa_bs_buf[x], 0);

		BuRTDefMsgLegal(RECEIVE, (U16BIT)x, ALL);
		BuRTDefMsgLegal(TRANSMIT, (U16BIT)x, ALL);
		BuRTDefMsgLegal(BROADCAST, (U16BIT)x, ALL);
		TotalMessages[x]=0;
	}
  

  /* setup interrupt handler */
	BuInstallUsrHandler(MyIsr);

	BuIrqEnable(IRQ_END_OF_MESSAGE);

	BuRTRun();


    printf("RT address %d active\n\n",RTaddress);

    for(x=0;x<active_sa;x++)
	{
		printf("Subaddr %2d->#Msgs=%5u; ",x,TotalMessages[x]);
		if((x%3)==2)printf("\n");
	}
    printf("\n");
	
    while(!kbhit())
	{
		if(BuGetReadBufPtr(&Ptr,&WordCount)==BUF_BUFFERFULL)
		{
			ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
			BuResetReadBuf();
			TotalBytesStored += ByteCount;
		}else
		{
			for(x=0;x<active_sa;x++)
			{
				if(DisplayInfo)
				{
					for(x=0;x<active_sa;x++)
					{
						printf("Subaddr %2d->#Msgs=%5u; ",x,TotalMessages[x]);
						if((x%3)==2)printf("\n");
					}
				    printf("\n");
					DisplayInfo--;
				}
			}
			
			/* only update the screens if the read buffer */
            /* is empty (give priority to disk storage) */
   		    printf("Total Bytes Monitored = "); 
			printf("%05u\t",TotalBytesMonitored);
			printf("Total Bytes Stored = ");
			printf("%05u\r",TotalBytesStored);
		}
	}
	
  /* cleanup any remaining messages on the stack */
  MyIsr(IRQ_END_OF_MESSAGE);

  do{
	  BuGetReadBufPtr(&Ptr,&WordCount);
	  ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
	  BuResetReadBuf();
	  TotalBytesStored += ByteCount;
  }while(WordCount);

  /* only update the screens if the read buffer */
  /* is empty (give priority to disk storage) */
  printf("Total Bytes Monitored = ");
  printf("%05u\t",TotalBytesMonitored);
  printf("Total Bytes Stored = ");
  printf("%05u\r",TotalBytesStored);
  
  close(out_handle);
  BuFreeRWBuffers();

  /*Must close out of library properly*/
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
