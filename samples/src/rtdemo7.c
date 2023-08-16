/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * rtdemo7.c
 *
 *          This is a demonstration of a BULK transfer TRANSMIT technique using
 *          the ACE runtime library in RT mode. Two large circular buffers
 *          are allocated in the ACE address space. Each of these buffers has
 *          an offset to prevent data from being repetively sent after a
 *          rollover). The ACE library buffering scheme is used to allocate
 *          a number of buffers the same size as the active area of an
 *          allocated circular buffer. The software buffers are loaded with
 *          data from a file. Initially the active circular buffer is
 *          loaded with the first buffer. The RT mode becomes active and
 *          responds to transmit messages incrementing through the first
 *          circular buffer. A circular buffer rollover interrupt executes
 *          a user defined ISR. The inactive buffer is first loaded with
 *          the offset address space (that area on the top of the buffer
 *          currently being processed). Additionally, the inactive buffer is
 *          loaded with a new local buffer (previously loaded from the file
 *          or subsequentally loaded within the main loop). The buffer is
 *          swapped and the index into the new buffer is calculated so a
 *          continuous issuance of new transmit buffers (or ZEROED buffers if
 *          no more data is available) is ready. A loop in the mainline
 *          program continues to load buffers from disk if they are available.
 *
 *          Note: Transmit messages of 32 words should be sent from the BC.
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

/*   -----   0    <--This is a rollover buffer to allow
 *       |           time till the isr swaps buffers.
 *       |
 *   -----   256  <--This is used as a holding spot for the
 *       |           previous rollover buffer. We calculate
 *       |           the starting offset.
 *   -----   512
 *       |
 *       |
 *       |        <--This is the active data area.
 *       |
 *       |
 *   -----   1024
 */

#define DataBlockSize    1024
#define DataBlockType    RTBUFFER1024
#define DataBlockOffset  256
#define DataBlockAct     (DataBlockSize-DataBlockOffset-256)
#define DataBlockBoff    512

#define RTaddress     5
#define RTsaddr       1

/* number of buffer swap retries */
#define SwapRetry        8

/* each buffer is 512 (active) + 256 (rollover buffer) words */

#define BUF_SIZE          768
#define NUM_BUFS          5

RTBlkHandle  TxBuff[2];

/* this flag is an index to the next available buffer */
int  TxBuffNext = 0;

U16BIT  WordCount,ByteCount;
U16BIT  *Ptr;
U16BIT  *BufPtr;

/* bytes transferred to the ace */
U32BIT  BytesTransferred=0;

/* bytes loaded into the local buffers */
U32BIT  BytesLoaded=0;

U16BIT  DataStackPtr;

void  __BUDECL MyIsr(U16BIT status)
{
  int offset=0;
  int notdone=SwapRetry;

  if(status&IRQ_RT_CIRC_BUFFR_ROLLOVR)
  {

    /* get the last loaded read buffer and determine if it had any
     * carryover, if it didn't we fill the new buffer with zero,
     * otherwise the carryover is written to the next tx buffer
     * and a new read buffer is appended
     */
    BuGetReadBufPtr(&Ptr,&WordCount);

    /* no carryover so we have no new buffer to prepare */
    if(WordCount<=DataBlockAct) {
      BuBlockFill(TxBuff[TxBuffNext]->absaddr,0,DataBlockSize);
    }

    offset = WordCount - DataBlockAct;

    /* copy carryover, read in the next buffer */
    if(WordCount>DataBlockAct) {
      BuBlockWrite( (U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockOffset),
                    Ptr+DataBlockAct,
                    (U16BIT)offset);
      BuResetReadBuf();

      BuGetReadBufPtr(&Ptr,&WordCount);
      BytesTransferred+=WordCount<<1;

      /* determine if buffer has carryover, if not then here we
       * fill the rollover buffer with zero, write in the buffer
       * we do have, and fill the remaining with zero
       */
      if(WordCount<=DataBlockSize-DataBlockOffset-offset)
	  {
		  BuBlockFill(TxBuff[TxBuffNext]->absaddr,0,DataBlockOffset);
		  BuBlockWrite((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockOffset+offset),
			  Ptr,WordCount);
		  BuBlockFill((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockOffset+WordCount+offset),
			  0,(U16BIT)(DataBlockAct-WordCount));
	  }

      /* here we do have carryover so we write the new active buffer,
       * write the rest of the buffer to the rollover buffer, and fill
       * in any remaining data with zero's
       */
      else if(WordCount>(DataBlockSize-DataBlockOffset-offset))
	  {
		  BuBlockWrite((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockOffset+offset),
			  Ptr,(U16BIT)(DataBlockSize-DataBlockOffset-offset));
		  BuBlockWrite(TxBuff[TxBuffNext]->absaddr,Ptr+(DataBlockSize-DataBlockOffset-offset),
			  (U16BIT)(WordCount-(DataBlockSize-DataBlockOffset-offset)));

		  if((WordCount-(DataBlockSize-DataBlockOffset-offset))<DataBlockOffset)
			  BuBlockFill((U16BIT)(TxBuff[TxBuffNext]->absaddr+(WordCount-(DataBlockSize-DataBlockOffset-offset))),
			  0,(U16BIT)(DataBlockOffset-(WordCount-(DataBlockSize-DataBlockOffset-offset))));
	  }
    }
  }

  /* to swap buffers we first determine the offset into the rollover
   * buffer by reading the stack pointer and subtracting the starting
   * address of the circular buffer, if a message is active (we assume
   * the message requires 32 words) and add this to the offset
   *
   * we wait for the end of the message, determine if we sucessfully
   * swapped buffers
   */

  do{
     offset=BuReadRam(LOOKUP_A+(32*TRANSMIT)+RTsaddr)-TxBuff[TxBuffNext^1]->absaddr;

     DataStackPtr=BuRTMapBlk( RTsaddr,TRANSMIT,TxBuff[TxBuffNext],
		 (U16BIT)(DataBlockOffset+offset+((BuReadReg(CONFIG_1)&1)?32:0)));

     while(BuReadReg(CONFIG_1)&1);

	 if((BuReadRam(LOOKUP_A+(32*TRANSMIT)+RTsaddr)&0x1c00)==((TxBuff[TxBuffNext]->absaddr)&0x1c00))
		 notdone=0;else notdone--;
	}while(notdone);

  TxBuffNext^=1;
}

int main(void)
{
  BuError_t Err;  /* ACE library error status type  */

  SubAddrCtrlWrd SubAddrConfig;
  int result,in_handle;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to transmit BULK data transfers\n");
  printf("using 2 large circular buffers.\n\n");
    
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

  /* open data file (ace rt data file) */
#ifndef _ACELINUX
  _fmode=O_BINARY;
  in_handle=open("ACE.DAT",O_RDONLY|O_BINARY);
#else
  in_handle=open("ACE.DAT",O_RDONLY);
#endif

  if(!in_handle) {
    printf("error opening input file\n");
    BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
    return 0;
  }

  /* open rt mode for access */
  BuRTOpen();

  /* open buffer module */
  BuBufOpen();

  /* create buffers */
  result=BuCreateRWBuffers(NUM_BUFS,BUF_SIZE,0);

  if(result!=NUM_BUFS) {
    printf("unable to allocate buffers\n");
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

  /* define default subaddress control word */
  SubAddrConfig.BcstEomInt    = FALSE;
  SubAddrConfig.RxEomInt      = FALSE;
  SubAddrConfig.TxEomInt      = FALSE;
  SubAddrConfig.BcstBuffInt   = FALSE;
  SubAddrConfig.RxBuffInt     = FALSE;
  SubAddrConfig.TxBuffInt     = TRUE;
  SubAddrConfig.BcstMm        = SINGLE_MESSAGE;
  SubAddrConfig.RxMm          = SINGLE_MESSAGE;
  SubAddrConfig.TxMm          = DataBlockType;
  SubAddrConfig.RcvBufferType = SINGLEBUFFER;

  /* allocate rx buffers */
  TxBuff[0]=BuRTAllocBlk((U8BIT)SubAddrConfig.TxMm);
  TxBuff[1]=BuRTAllocBlk((U8BIT)SubAddrConfig.TxMm);

  /* initialize subaddresses #1 */
  BuRTDefSA(1,&SubAddrConfig);

  /* load in as many buffers as will fit */
  BufPtr=BuGetWriteBufPtr();
  while(BufPtr)
  {
	  ByteCount=read(in_handle,(char*)BufPtr,BUF_SIZE*sizeof(U16BIT));
	  if(ByteCount)
	  {
		  BuUpdateWriteBuf(ByteCount>>1);
		  BytesLoaded+=ByteCount;
		  BufPtr=BuGetWriteBufPtr();
	  }else BufPtr=NULL;
  }

  /* map the new block and load in the first buffer */
  BuRTMapBlk(1,TRANSMIT,TxBuff[TxBuffNext],DataBlockBoff);

  BuGetReadBufPtr(&Ptr,&WordCount);

  if(WordCount<=DataBlockAct) {
    BuBlockFill(TxBuff[TxBuffNext]->absaddr,0,DataBlockBoff);
    BuBlockWrite((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockBoff),Ptr,WordCount);
    BuBlockFill((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockBoff+WordCount),0,(U16BIT)(DataBlockAct-WordCount));  }
  else if(WordCount>DataBlockAct) {
    BuBlockWrite((U16BIT)(TxBuff[TxBuffNext]->absaddr+DataBlockBoff),Ptr,DataBlockAct);
    BuBlockWrite(TxBuff[TxBuffNext]->absaddr,Ptr+DataBlockAct,(U16BIT)(WordCount-DataBlockAct));
    if((WordCount-DataBlockAct)<DataBlockBoff)
    BuBlockFill((U16BIT)(TxBuff[TxBuffNext]->absaddr+(WordCount-DataBlockAct)),0,(U16BIT)(DataBlockBoff-(WordCount-DataBlockAct)));
  }

  BytesTransferred+=WordCount<<1;

  TxBuffNext^=1;

  /* make receive to subaddres 1 legal */
  BuRTDefMsgLegal(TRANSMIT,01,ALL);

  /*setup interrupt handler*/
  BuInstallUsrHandler(MyIsr);

  /*enable circular buffer rollover interrupts*/
  BuIrqEnable(IRQ_RT_CIRC_BUFFR_ROLLOVR);

  BuRTRun();

  printf("\nRT address %d is ready to TRANSMIT bulk data transfer from\n",RTaddress);
  printf("subaddress 1... Press any key to exit.\n\n");
  printf("Note: Transmit messages of 32 words should be sent from the BC.\n\n");

  while(!kbhit())
  {
	  BufPtr=BuGetWriteBufPtr();
	  
	  if(BufPtr)
	  {
		  ByteCount=read(in_handle,(char*)BufPtr,BUF_SIZE*sizeof(U16BIT));
		  if(ByteCount)
		  {
			  BuUpdateWriteBuf(ByteCount>>1);
			  BytesLoaded+=ByteCount;
		  }else BufPtr=NULL;
	  }

	  /*display transmission status info*/
	  printf("Total Bytes Loaded = %10u\t",BytesLoaded);
	  printf("Total Bytes Transferred to ACE  = %10u\r",BytesTransferred);
  }



  BuRTStop();

  close(in_handle);
  BuFreeRWBuffers();

  /*Must close out of library properly*/
  BuRTClose();
  BuBufClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
