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
 * rtdemo10.c
 *           This demonstrates the use of the ACE's subaddress BUSY
 *           bit lookup tables. The BuRTBusyTableEna() function enables
 *           the lookup table (alternate rt status words should be disabled
 *           this is the default setup from BuRTOpen()). The busy bit for
 *           each subaddress can be controlled using the BuRTBusyBitEnable()
 *           and BuRTBusyBitDisable() functions.
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

BuConf_t Conf; /* ACE library configuration type */

#define  RTaddress   5

int main(void)
{
  MsgType   msg;  /* structure to hold message information */
  BuError_t Err;  /* ACE library error status type  */
  RTBlkHandle   r1,r2;
  SubAddrCtrlWrd sub_addr_config;
  int x;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1&2 to demonstrate use of ACE's BUSY bit\n");
  printf("lookup tables.\n\n");

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

  /*initialize subaddresses*/
  BuRTDefSA(1, &sub_addr_config);
  BuRTDefSA(2, &sub_addr_config);

  /*allocate data blocks*/
  r1 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
  r2 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);

  /*map the data blocks to the lookup table*/
  BuRTMapBlk(1, RECEIVE, r1, 0);
  BuRTMapBlk(2, RECEIVE, r2, 0);

  /*legalize subaddresses */
  BuRTDefMsgLegal(RECEIVE, 01, ALL);
  BuRTDefMsgLegal(RECEIVE, 02, ALL);

  /* the ACE allows an optional Busy BIT lookup table. It
   * is enabled and subadress 1 is set to busy, subaddress 2 is
   * unaffected. The parameters are broadcast=FALSE,t_r=RECIEVE,
   * and subaddress 1
   */

  BuRTBusyTableEna(ON);

  BuRTBusyBitEnable(FALSE,RECEIVE,01);

  /*RUN RT*/
  BuRTRun();

  printf("\nRT address %d\n\n rx 32 words subaddr 1 (BUSY)\n",RTaddress);
  printf(" rx 32 words subaddr 2\n\nPress any key to exit\n\n");
  while(!kbhit())
  {
	  while(BuRTReadMsg(LAST_MESSAGE,&msg)==BU_SUCCESS)
	  {
		  printf("Bsw %4x TimeTag %8u uS WordCount %3d Cmd %4x \n",
			  msg.BlockStatus,
			  msg.TimeTag*2,
			  msg.WordCount,
			  msg.CmdWord1);
		  if(msg.WordCount)
		  {
			  for(x=0;x<msg.WordCount;x++)
				  printf("%04x ",msg.Data[x]);
			  printf("\n");
		  }
	  }
  }

  /*Must close out of library properly*/
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
