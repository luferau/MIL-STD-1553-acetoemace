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
 * rtdemo8.c
 *          The ACE library is designed utilize the enhanced mode code
 *          lookup table and interrupt capibility. This enables the
 *          reading and writing of mode code data with the
 *          BuRTWriteEnhMCData() and BuRTReadEnhMCData() functions.
 *          Selective mode code interrupts can be enabled and disabled
 *          with the BuRTModeIrqEnable() and BuRTModeIrqDisable() functions.
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

int VectorCount=0;
int V=0;

void __BUDECL MyIsr(U16BIT status)
{
  /* this example just increments the vector word that is transmitted */
  if(status&(IRQ_STATUS_SET_MODE_INT_TRIG|1)) {
    VectorCount++;
    V=status;
  }
}

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
  printf("\nThis RT Demo sets up RT 5 to demonstrate enhanced mode code\n");
  printf("handling.\n\n");

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

  /* utilize ACE enhanced mode code handling
   * make tx mode code 16 legal (transmit vector word)
   * enable enhanced mode code interrupt, the parameters include
   * broadcast=FALSE,t_r=TRANSMIT,DataWord=TRUE and
   * map=TRANSMIT VECTOR WORD (04h)
   */
  BuRTEnhModeCode(ON);
  BuRTDefMsgLegal(RECEIVE,31,RT_MODE_SYNCHRONIZE_DATA);
  BuRTModeIrqEnable(FALSE,BROADCAST,TRUE,RTMIRQ_SYNCHRONIZE);
  BuRTModeIrqEnable(TRUE,RECEIVE,TRUE,RTMIRQ_SYNCHRONIZE);

  BuRTDefMsgLegal(TRANSMIT,31,RT_MODE_TXS_VECTOR_WORD);
  BuRTModeIrqEnable(FALSE,TRANSMIT,TRUE,RTMIRQ_TRANSMIT_STATUS);

  /* setup interrupt handler */
  BuInstallUsrHandler(MyIsr);

  /* enable mode code interrupt */
  BuIrqEnable(IRQ_STATUS_SET_MODE_INT_TRIG|IRQ_END_OF_MESSAGE);
  BuRTWriteEnhMCData(RtEmod_TRANSMIT_VECTOR_WORD,(U16BIT)VectorCount);

  /*RUN RT*/
  BuRTRun();
  printf("\nRT %d active... Press any key to exit.\n\n",RTaddress);

  while(!kbhit())
  {
	  while((BuRTReadMsg(LAST_MESSAGE,&msg)==BU_SUCCESS)&&(!kbhit()))
	  {
		  printf("Bsw %4x TimeTag %8u uS WordCount %3d Cmd %4x %d %04x\n",
			  msg.BlockStatus,
			  msg.TimeTag*2,
			  msg.WordCount,
			  msg.CmdWord1,VectorCount,V);

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
