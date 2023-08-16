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
 * rtdemo4.c
 *          This demo sets up two 32 word buffers. When the RT transfers
 *          the data in the first buffer the active one becomes the second buffer.
 *          This second buffer is used when the next transfer occurs. At
 *          this time control is transfered back to the first. Thus a
 *          ping-pong effect is done at each transmit.
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

#define COMMAND_FIELDS(wrd) (*((CmdWordType*)&wrd))

/* CommandFields is a macro that is used for typecasting an
 * unsinged int to CommandWord type which has elements
 * WordCount, SubAddr, Tr, RtAdd which represent the bit
 * fields of a command word
 */

MsgType Message;  /* structure to hold message information */
RTBlkHandle TxBuffNum[2],BcstBuff,RxBuff;
int ActiveBuff = 0;

void __BUDECL MyIsr(U16BIT status)
{
	if(status&IRQ_RT_BC_MESSAGE_INT)
	{
		while(BuRTReadMsg(LAST_MESSAGE,&Message)==BU_SUCCESS)
		{/* if message was a transmit command to subaddress #1,
		  * flip transmit data buffers
		  */
			if((COMMAND_FIELDS(Message.CmdWord1).TR==TRANSMIT)&&
				(COMMAND_FIELDS(Message.CmdWord1).SubAddr==01))
				BuRTMapBlk(1,TRANSMIT,TxBuffNum[(ActiveBuff=((ActiveBuff+1)%2))],0);
		}
	}/*end if*/
}

int main(void)
{

  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */
  SubAddrCtrlWrd SubAddrConfig;
  U16BIT TxData[32];
  int i;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to have two buffers which ping-pong\n");
  printf("after each transmit command.\n\n");

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
  SubAddrConfig.BcstEomInt = FALSE;
  SubAddrConfig.RxEomInt   = FALSE;
  SubAddrConfig.TxEomInt   = TRUE;
  SubAddrConfig.BcstBuffInt = FALSE;
  SubAddrConfig.RxBuffInt   = FALSE;
  SubAddrConfig.TxBuffInt   = FALSE;
  SubAddrConfig.BcstMm = SINGLE_MESSAGE;
  SubAddrConfig.RxMm   = SINGLE_MESSAGE;
  SubAddrConfig.TxMm   = SINGLE_MESSAGE;

  /*allocate data blocks*/
  BcstBuff = BuRTAllocBlk((U8BIT)SubAddrConfig.BcstMm);
  RxBuff = BuRTAllocBlk((U8BIT)SubAddrConfig.RxMm);
  TxBuffNum[0] = BuRTAllocBlk((U8BIT)SubAddrConfig.TxMm);
  TxBuffNum[1] = BuRTAllocBlk((U8BIT)SubAddrConfig.TxMm);

  /* initialize subaddresses #1 */
  BuRTDefSA(1, &SubAddrConfig);

  BuRTMapBlk(1, BROADCAST, BcstBuff,0);
  BuRTMapBlk(1, RECEIVE, RxBuff,0);

  /* make transmit 32 words from subaddres 1 legal */
  BuRTDefMsgLegal(TRANSMIT, 01, ALL);

  for(i=0;i<32;i++)TxData[i]=i;
  BuWriteBlk(TxBuffNum[ActiveBuff],TxData,32);
  BuRTMapBlk(1,TRANSMIT,TxBuffNum[ActiveBuff],0);
  for (i=0;i<32;i++)TxData[i]=0xff00|i;
  BuWriteBlk(TxBuffNum[((ActiveBuff+1)%2)],TxData,32);

  /*setup interrupt handler*/
  BuInstallUsrHandler(MyIsr);

  /*enable end of message interrupts*/
  BuIrqEnable(IRQ_RT_BC_MESSAGE_INT);

  BuRTRun();

  printf("\nRT %d, SA 1 Active \n",RTaddress);
  printf("Tx command to SA 1 will pinpong buffers\n\n");
  printf("Press any key to exit.\n");
  while (!kbhit());


  /*Must close out of library properly*/
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}

