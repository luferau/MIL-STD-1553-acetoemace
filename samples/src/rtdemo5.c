/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3)  rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * rtdemo5.c
 *          This program demonstrates the use of subaddress
 *          double buffering for receive commands. This demonstrates
 *          the asynchronous reading of an RT data buffer.
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
#include "aceutils.h"
#define getch() readch()
#else
/* DOS/Windows Headers */
#include <conio.h>
#endif

#define RTaddress 5

RTBlkHandle RxBuff;
int chksum=0;

int main(void)
{

  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */

  SubAddrCtrlWrd SubAddrConfig;

  U16BIT DataBuf[32];
  int i;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to have double buffering for receive\n");
  printf("commands and demonstrates asynchronous reading of the RT buffer.\n\n");

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

  /* open rt mode for access */
  BuRTOpen();

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
  SubAddrConfig.RxEomInt      = FALSE;
  SubAddrConfig.TxEomInt      = FALSE;
  SubAddrConfig.BcstBuffInt   = FALSE;
  SubAddrConfig.RxBuffInt     = FALSE;
  SubAddrConfig.TxBuffInt     = FALSE;
  SubAddrConfig.BcstMm        = SINGLE_MESSAGE;
  SubAddrConfig.RxMm          = SINGLE_MESSAGE;
  SubAddrConfig.TxMm          = SINGLE_MESSAGE;

  /* setting DOUBLEBUFFER will enable the RT rx double buffering
   * for this subadress only.
   */
  SubAddrConfig.RcvBufferType = DOUBLEBUFFER;

  /* we must use DOUBLE_MESSAGE to allocate the 32*2 words */
  RxBuff = BuRTAllocBlk(DOUBLE_MESSAGE);

  /* initialize subaddresses #1 */
  BuRTDefSA(1, &SubAddrConfig);

  /* map data block */
  BuRTMapBlk(1, RECEIVE, RxBuff,0);

  /* initialize both data blocks (to 0) ... they are consecuitive
   * since we allocate memory on 64 word boundaries in this case
   */
  BuBlockFill(RxBuff->absaddr,0,64);

  /* make receive to subaddres 1 legal */
  BuRTDefMsgLegal(RECEIVE, 01, ALL);

  /*RUN RT*/
  BuRTRun();

  while(!kbhit())
  {
	  BuRTReadInactive(RxBuff,DataBuf);
	  chksum=0;
	  for(i=0;i<4;i++)
	  {
		  printf("%4d:%5x",i,DataBuf[i]);
		  chksum+=DataBuf[i];
	  }
	  printf("\r\rchksum = %10x",chksum);
  }

  /*Must close out of library properly*/
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}

