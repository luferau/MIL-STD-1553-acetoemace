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
 * bcdemo4.c
 *          This program demonstrates the use of BuBCRecallMinor. This
 *          function loads a minor frame from disk and does all the
 *          necessary message and minor frame creation. The program
 *          continues with the execution of the minor frame and the
 *          reading of the message results with BuBCReadMsgNum.
 */

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
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type */
  int NumOfMsgs,j;

  BCMinorFrmHandle myframe;  /* each minor frame has a handle */
  MsgType readmsg;

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo demonstrates the use of BuBCRecallMinor.\n\n");
    
  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # of your BC:> ");
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

       /* enter bus controller mode of operation */
       BuBCOpen();
	   
       /* recall minor frame to file */
       if((NumOfMsgs=BuBCRecallMinor("test.frm",&myframe))==0) {
         printf("error loading .frm file\n");
         BuBCClose();
         BuClose();
#ifdef _ACELINUX
	     AceUtilsExit();  /* Exit Ace Utilities */
#endif
		 return 0;
       }

       /* set response timeout to 50.5 us */
       BuTimeout(RESPONSE_505);

       /* load minor frame into ACE stack */
       BuBCLoadMinor(BU_BCFRMBUFA,myframe);

       /* run frame */
       BuBCRunMinor(BU_BCFRMBUFA,BU_BCSINGLE);

       while(BuBCIsFrmActive());

       for(j=0;j<NumOfMsgs;j++) {
	          if(BuBCReadMsgNum(myframe,(U16BIT)j,&readmsg))
                 printf("error reading message result\n");
	          else DisplayMsg(&readmsg);
	     }

       /* free minor frame and associated messages */
       BuBCFreeMinor(myframe,TRUE);

       /* should be called to close bus controller mode */
       BuBCClose();

  } else printf("error communicating with pc card\n");

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
  if(Msg->BlockStatus&MT_ERR)printf(" (EXCEPTION)\n");else printf("\n");


  printf(" Cmd1 %04X %s\n",Msg->CmdWord1,BuCmdStr(Msg->CmdWord1));

  if(Msg->CmdWord2flag) {
       printf(" Cmd2 %04X %s\n",Msg->CmdWord2,BuCmdStr(Msg->CmdWord2));
  }

  printf(" Time %u uS\n",Msg->TimeTag*2); /*2us resolution*/
  printf(" GapT %u uS\n",Msg->GapTime); /*1us resolution*/
  printf(" BSW  %04X %s\n",Msg->BlockStatus,BuBCBSWErrorStr(Msg->BlockStatus));
  printf(" Ctrl %04X \n",Msg->ControlWord);

  for(i=0;i<Msg->DataLength;++i) {
         if(i==0)printf(" Data ");
         printf("%04X  ",Msg->Data[i]);
         if((i%6)==5)printf("\n      ");
  }
  if(Msg->Status1flag)printf("\n Sta1 %04X",Msg->Status1);
  if(Msg->Status2flag)printf("\n Sta2 %04X",Msg->Status2);
  if(Msg->LoopBack1flag)printf("\n Lpb1 %04X",Msg->LoopBack1);
  if(Msg->LoopBack2flag)printf("\n Lpb2 %04X",Msg->LoopBack2);
  printf("\n\n");
}
