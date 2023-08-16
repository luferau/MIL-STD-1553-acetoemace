/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3)  sample  rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * rtdemo9.c
 *          The ACE allows for an internal or external BIT word. Internal
 *          bit words are readable by the host processor (the BuRTBITRead()
 *          function can be used for this) but not writable. External bit
 *          words are readable with the above function and also writable with
 *          the BuRTExtBITWrite() function.
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

#define RTaddress 5

int main(void)
{
  BuError_t Err;  /* ACE library error status type  */
  MsgType   msg;  /* structure to hold message information */
  int x;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 to demonstrate use of BIT words\n");
  printf("and the functions BuRTBITRead() and BuRTExtBITWrite().\n\n");

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

  /* utilize ACE enhanced mode code handling and external BIT word */
  BuRTEnhModeCode(ON);
  BuRTExtBITWord(ON);

  BuRTDefMsgLegal(TRANSMIT,31,RT_MODE_TX_BIT_WORD);

  BuRTExtBITWrite(   RtBitwd_RT_RT_GP_SYNC_ADR_ER
                   | RtBitwd_INCORRECT_SYNC_RXD
                   | RtBitwd_HIGH_WORD_COUNT
                   | RtBitwd_LOOP_TEST_FAILURE_A);

  /*RUN RT*/
  BuRTRun();

  printf("\nRT %d active... Press any key to exit.\n\n",RTaddress);

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
