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
 * rtdemo6.c
 *
 *          This is a demonstration of a BULK transfer RECEIVE technique using
 *          the ACE runtime library in RT mode. Two large circular buffers
 *          are allocated in the ACE address space. The starting location of
 *          the buffer is initially offset into the buffer; this allows time
 *          till the circular buffer rollover is serviced. The ISR swaps the
 *          current data pointer to an offset into the second circular buffer.
 *          This ensures no data is lost during the reading of the now
 *          inactive buffer. The full contents from the offset position to
 *          the end of the buffer (where rollover occured) and the data that
 *          is continued allows the area to be stored in host memory.
 *          (The host buffering is built into the ACE runtime library in
 *          the BUF module.) The main execution loop of the program polls
 *          the buffering routines for a full buffer and when collected stores
 *          the full buffers to disk. A final cleanup of the storage buffers
 *          is done at the end of the run.
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

/* number of words in buffer */
#define DataBlockSize    1024
/* this constant is used to define the subaddress */
#define DataBlockType    RTBUFFER1024
/* offset 256 words */
#define DataBlockOffset  512
/* number of buffer swap retries */
#define SwapRetry        50

#define RTaddress        5
#define RTsaddr          1

#define MAX_STK_READ_SIZE DataBlockSize
#define BUF_SIZE          (5*1024)
#define NUM_BUFS          6
#define BUF_FULL          (BUF_SIZE - MAX_STK_READ_SIZE - 100 )

BuConf_t Conf; /* ACE library configuration type */

/* double buffered */
RTBlkHandle RxBuff[2];

/* starting address of the buffer (with offset) */
U16BIT RxBuffInit[2];

/* this flag is an index to the next available buffer */
int RxBuffNext = 0;
int LookForEOM = FALSE;
U32BIT TotalBytesRead;
U32BIT TotalBytesStored;

U16BIT StackPtr;
U16BIT DataStackPtr;
U16BIT DataLen1=0;
U16BIT DataLen2=0;
U16BIT *BufPtr;

void __BUDECL MyIsr(U16BIT status)
{
  int notdone=SwapRetry;

  /* check if the interrupt is due to a circular buffer overflow. If it is,
   * then we use BuRTMapBlk() to read the current data pointer (from the
   * lookup table) and write the new pointer (with an offset 4th parameter).
   * we then wait for any currently active message to end and check the
   * lookup table to see if the we have swapped buffers, we do this by
   * checking the upper bits of the current data pointer against the
   * next buffer to swap, if they are the same we have sucessfully swapped
   */
  if(status&IRQ_RT_CIRC_BUFFR_ROLLOVR)
  {
	  U16BIT NewPtr;
	  do{
		  DataStackPtr=BuRTMapBlk(RTsaddr,RECEIVE,RxBuff[RxBuffNext],DataBlockOffset);
		  while(BuReadReg(CONFIG_1)&1);
		  NewPtr= BuReadRam(LOOKUP_A+(32*RECEIVE)+RTsaddr);
		  if((NewPtr&0x1c00)==((RxBuff[RxBuffNext]->absaddr+DataBlockOffset)&0x1c00))
			  notdone=0;else notdone--;
	  }while(notdone);

	  RxBuffNext^=1;

	  DataLen1=DataBlockSize-DataBlockOffset;
	  DataLen2=DataStackPtr-RxBuff[RxBuffNext]->absaddr;

	  /* if we exceed our offset something is wrong */
	  if(DataLen2>DataBlockOffset){DataLen2=DataBlockOffset;}

	  /* get current write buffer pointer */
	  BufPtr=BuGetWriteBufPtr();

	  if(BufPtr)
	  {
		  /* read data into buffer for FILLED circular buffer*/
		  BuBlockRead(RxBuffInit[RxBuffNext],BufPtr,DataLen1);
		  BuUpdateWriteBuf(DataLen1);

		  /* extra data stored on top of buffer AFTER rollover */
		  BuBlockRead(RxBuff[RxBuffNext]->absaddr,BufPtr,DataLen2);
		  BuUpdateWriteBuf(DataLen2);

		  /* add the full circular buffer back in */
		  TotalBytesRead+=(DataLen1+DataLen2)*sizeof(U16BIT);
	  }
  }
}/*end of MyIsr*/

int main(void)
{
  BuError_t Err;  /* ACE library error status type  */

  SubAddrCtrlWrd SubAddrConfig;
  int out_handle,result;
  U16BIT WordCount,ByteCount;
  U16BIT* Ptr;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to receive BULK data transfers\n");
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

  /* create output file (ace rt data file) */
#ifndef _ACELINUX
  _fmode=O_BINARY;
#endif
  out_handle=creat("ACE.DAT",S_IWRITE|S_IREAD);
  if(out_handle==-1) {
    printf("error opening output file\n");
    BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
    return 0;
  }

  if(BuValid())
  {
	  /* open rt mode for access */
	  BuRTOpen();

	  /* open buffer module */
	  BuBufOpen();

	  /* create buffers */
	  result=BuCreateRWBuffers(NUM_BUFS,BUF_SIZE,BUF_FULL);

	  if(result!=NUM_BUFS)
	  {
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

	  /* no flags set in status */
	  BuRTSetBusy(OFF);
	  BuRTSetSSflag(OFF);
	  BuRTSetSvcReq(OFF);
	  BuRTFlag(OFF);

	  /*define all messages illegal to start with*/
	  BuRTDefMsgIllegal(ALL,ALL,ALL);

	  /* define default subaddress control word */
	  SubAddrConfig.BcstEomInt    = FALSE;
	  SubAddrConfig.RxEomInt      = TRUE;
	  SubAddrConfig.TxEomInt      = FALSE;
	  SubAddrConfig.BcstBuffInt   = FALSE;
	  SubAddrConfig.RxBuffInt     = TRUE;
	  SubAddrConfig.TxBuffInt     = FALSE;
	  SubAddrConfig.BcstMm        = SINGLE_MESSAGE;
      SubAddrConfig.RxMm          = RTBUFFER1024;
      SubAddrConfig.TxMm          = SINGLE_MESSAGE;
      SubAddrConfig.RcvBufferType = SINGLEBUFFER;

	  /* allocate rx buffers */
	  RxBuff[0]=BuRTAllocBlk((U8BIT)SubAddrConfig.RxMm);
	  RxBuff[1]=BuRTAllocBlk((U8BIT)SubAddrConfig.RxMm);
	  RxBuffInit[0]=RxBuff[0]->absaddr+DataBlockOffset;
	  RxBuffInit[1]=RxBuff[1]->absaddr+DataBlockOffset;

	  /* initialize subaddresses #1 */
	  BuRTDefSA(1,&SubAddrConfig);

	  /* map data block with word offset into buffer, the Next buffer is
	   * calculated
	   */
	  BuRTMapBlk(1,RECEIVE,RxBuff[RxBuffNext],DataBlockOffset);

	  RxBuffNext^=1;

	  /* make receive to subaddres 1 legal */
	  BuRTDefMsgLegal(RECEIVE,01,ALL);

	  /*setup interrupt handler*/
	  BuInstallUsrHandler(MyIsr);

      /*enable circular buffer rollover interrupts*/
	  BuIrqEnable(IRQ_RT_CIRC_BUFFR_ROLLOVR);

	  /*RUN RT*/
	  BuRTRun();

      printf("\nRT address %d is ready to RECEIVE bulk data transfer at\n",RTaddress);
      printf("subaddress 1... Press any key to exit.\n\n");

	  while(!kbhit())
	  {
		  /* If there is a full read buffer we store this to disk. The read buffer
		   * is then reset to allow it to be used again. The TotalBytesStored is
		   * then updated.
		   */
		  if(BuGetReadBufPtr(&Ptr,&WordCount)==BUF_BUFFERFULL)
		  {
			  ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
			  BuResetReadBuf();
			  TotalBytesStored+=ByteCount;
		  }else
		  {
			  /* only update the screens if the read buffer */
			  /* is empty (give priority to disk storage) */
			  printf("Total Bytes Read   = %10u\t",TotalBytesRead);
			  printf("Total Bytes Stored = %10u\r",TotalBytesStored);
		  }
	  }

	  BuRTStop();

	  /* We have to take the remaining data stack and store this in a buffer.
	   * The DataStackPtr is the last updated pointer to the data stack. A
	   * write buffer pointer is gotten and the DataLen is calculated
       * to be the current data pointer - the start of the circular buffer
       * which is offset from the beginning to allow wraparound. The data is
       * then read into the buffer, the write buffer is updated and the
       * TotalBytesRead counter is updated.
       */

	  DataStackPtr=BuReadRam(LOOKUP_A+1);
	  BufPtr=BuGetWriteBufPtr();

	  if(BufPtr)
	  {
		  /* we want to access the current active buffer not the next one*/
		  DataLen2=(DataStackPtr-RxBuffInit[RxBuffNext^1]);
		  BuBlockRead(RxBuff[RxBuffNext^1]->absaddr,BufPtr,DataLen2);
		  BuUpdateWriteBuf(DataLen2);
		  TotalBytesRead+=DataLen2*sizeof(U16BIT);
	  }

	  /* There may be buffers that have not been stored to disk because the
       * were either not to the top of the queue or not filled (the above loop
       * only dumps the buffer to disk when the buffer is full. The below loop
       * will dump the remaining buffers to disk.
       */

	  do{
		  BuGetReadBufPtr(&Ptr,&WordCount);
		  ByteCount=write(out_handle,(char*)Ptr,WordCount*sizeof(U16BIT));
		  BuResetReadBuf();
		  TotalBytesStored+=ByteCount;
	  }while(WordCount);

	  /* update screen with final outcome */
	  printf("Total Bytes Read   = %10u\t",TotalBytesRead);
	  printf("Total Bytes Stored = %10u\r",TotalBytesStored);

	  close(out_handle);
	  BuFreeRWBuffers();
	  BuRTClose();
	  BuBufClose();
  }

/*Must close out of library properly*/
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
