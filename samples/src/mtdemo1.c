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
 * mtdemo1.c
 *          Demonstrates use of monitor mode using polling of the
 *          ACE stack pointer to determine if messages have been rcvd
 *          off-loading monitor stack on a message by message basis.
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

void DisplayMsg(MsgType*Msg);

int main (void)
{

  MsgType msg;
  BuConf_t Conf;
  BuError_t Err;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis MT Demo polls the ACE stack pointer to determine if msgs have\n");
  printf("been received.\n\n");

  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # for your MT:> ");
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


  if(BuValid())
  {
	  BuMTOpen();

	  BuMTStkType(MT_DOUBLE,CMD256,DATA1024);

  /* def stack A to have a command stack at address 0, initially */
  /* pointing to address 0x40 and a length of 256 words. and define */
  /* the data stack to be at address 0x400, with the pointer initially */
  /* starting at 0x500 and a length of 0x400 */

	  BuMTDefStk(MT_STACKA,0X0000,0X0040,0X0400,0x100);

  /* def stack B to have a command stack at address 0x100, initially */
  /* pointing to address 0x140 and a length of 256 words. and define */
  /* the data stack to be at address 0x800, with the pointer initially */
  /* starting at 0x900 and a length of 0x400 */

	  BuMTDefStk(MT_STACKB,0x0100,0x40,0x0800,0x100);

  /* enable all subaddress's */
	  BuMTEnableRT(MT_ALL,MT_ALL,MT_ALL);

	  BuMTRun(); /* start the monitor up */

	  printf("\nMonitor Running -- waiting for messages\n\n");
	  printf("...Press any key to stop\n\n");

	  do{
		  if(BuMTReadMsg(&msg))
			  DisplayMsg(&msg);
	  }while(!kbhit());

	  BuMTClose();
  }else
	  printf("Error with card!\n");

  /*Must close out of library properly*/
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}


void DisplayMsg(MsgType*Msg)
{
  U16BIT i;

  printf("Message Type = %s",BuMsgTypeStr(Msg->Type));

  if(Msg->BlockStatus&MT_ERR)
	  printf(" (EXCEPTION)\n");
  else
	  printf("\n");

  printf(" Cmd1 %04X %s\n",Msg->CmdWord1,BuCmdStr(Msg->CmdWord1));

  if(Msg->CmdWord2flag)
  {
       printf(" Cmd2 %04X %s\n",Msg->CmdWord2,BuCmdStr(Msg->CmdWord2));
  }

  printf(" wc %d\n",Msg->WordCount);
  printf(" dl %d\n",Msg->DataLength);
  printf(" Time %u uS\n",Msg->TimeTag*2); /*2us resolution*/
  printf(" BSW  %04X %s\n", Msg->BlockStatus,BuMTBSWErrorStr(Msg->BlockStatus));

  for(i=0;i<Msg->DataLength;++i)
  {
	  if(i==0)
		  printf(" Data ");

	  printf("%04X  ",Msg->Data[i]);

	  if((i%6)==5)
		  printf("\n      ");
  }

  if(Msg->Status1flag)printf("\n Sta1 %04X",Msg->Status1);
  if(Msg->Status2flag)printf("\n Sta2 %04X",Msg->Status2);
  printf("\n\n");
}



