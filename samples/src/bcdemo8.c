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
 * bcdemo8.c
 *          This program demonstrates the use of the minor frame with
 *          bus controller operation. The BuBCReadMsgNum and BuBCWriteMsgNum
 *          routines are used to edit message information between frame
 *          runs.
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
  int i;

  BCMsgHandle msg;           /* the message has a handle */
  BCMinorFrmHandle myframe;  /* the minor frame has a handle */
  MsgType readmsg;           /* holds message result read from frame */

  U16BIT data1[10]={ 0x0000,0x1111,0x2222,0x3333,0x4444,
                     0x5555,0x6666,0x7777,0x8888,0x9999};

  U16BIT data2[10]={ 0xaaaa,0xbbbb,0xcccc,0xdddd,0xeeee,
                     0xffff,0xaa55,0x55aa,0x5555,0xa5a5};

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo demonstrates the use of BuBCReadMsgNum and BuBCWriteMsgNum\n");
  printf("to edit messages between frame runs.\n\n");
    
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

       /* set response timeout to 50.5 us */
       BuTimeout(RESPONSE_505);

       /* create bc->rt rt 3 sa 4 receive 10 words from bus controller */
       msg=BuBCXBCtoRT(3,4,10,CW_CHANNELA,data1,BU_BCNOGAP,BU_BCALWAYS);

       /* create minor frame */
       myframe=BuBCXMinorFrm(30000l,1,&msg);

       /* load the FIRST minor frame, run, and display the message result */
       BuBCLoadMinor(BU_BCFRMBUFA,myframe);
       BuBCRunMinor(BU_BCFRMBUFA,BU_BCSINGLE);
       while(BuBCIsFrmActive());

       if(BuBCReadMsgNum(myframe,0,&readmsg))
         printf("error reading message result\n");
       else DisplayMsg(&readmsg);

       /* here we keep the msg structure the same but
        * modify the data and write the data back
        * to the ACE
        */
       for(i=0;i<10;i++)
		   readmsg.Data[i]=data2[i];

       BuBCWriteMsgNum(myframe,0,&readmsg);

       /* load the SECOND minor frame, run, and display the message result
        * the data has been modified
        */
       BuBCLoadMinor(BU_BCFRMBUFA,myframe);
       BuBCRunMinor(BU_BCFRMBUFA,BU_BCSINGLE);
       while(BuBCIsFrmActive());
       if(BuBCReadMsgNum(myframe,0,&readmsg))
         printf("error reading message result\n");
       else DisplayMsg(&readmsg);

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
