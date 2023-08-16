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
 * bcdemo2.c
 *          This program demonstrates the use of the minor frame with
 *          bus controller operation. Also, the 10 possible message types
 *          allowed are demonstrated here in the frame that is created.
 *          After the frame is run, the results are read back and
 *          displayed to the screen.
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
  
  BCMsgHandle msgs[10];      /* each message has a handle */
  BCMinorFrmHandle myframe;  /* each minor frame has a handle */
  MsgType readmsg;           /* holds message result read from frame */

  U16BIT data[10]={ 0x1111,0x2222,0x3333,0x4444,0x5555,
                    0x6666,0x7777,0x8888,0x9999,0x0000};
  U16BIT data1[1]={0xabcd};

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo demonstrates the use of Minor frames and the 10 msg types.\n\n");
  
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
       msgs[0]=BuBCXBCtoRT(3,4,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);

       /* create bc<-rt rt 1 sa 5 transmit 10 words to bus controller */
       msgs[1]=BuBCXRTtoBC(1,5,10,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* create rt<-rt rt 2 sa 3 receive 5 words from rt 6 sa 7 */
       msgs[2]=BuBCXRTtoRT(2,3,5,6,2,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode (00011 initiate self-test) rt 8 */
       msgs[3]=BuBCXMode(8,CW_CHANNELA,0x03,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode tx with data (10010 tx last cmd) rt 6 */
       msgs[4]=BuBCXModeTx(6,CW_CHANNELA,0x02,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode rx with data (10001 synchronize) rt 9 */
       msgs[5]=BuBCXModeRx(9,CW_CHANNELA,0x01,data1,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast rx sa 3 10 words */
       msgs[6]=BuBCXBrdcst(3,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);

       /* rt(s)<-rt broadcast rx sa 3, tx rt 7 sa 4 6 words */
       msgs[7]=BuBCXBrdcstRTtoRT(3,6,7,4,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast mode rx data (10001 synchronize) */
       msgs[8]=BuBCXBrdcstModeRx(CW_CHANNELA,0x01,data1,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast mode no data (00100 transmitter shutdown) */
       msgs[9]=BuBCXBrdcstMode(CW_CHANNELA,0x01,BU_BCNOGAP,BU_BCALWAYS);


       /* create minor frame */
       myframe=BuBCXMinorFrm(30000l,10,msgs);
       /* load minor frame into ACE stack */
       BuBCLoadMinor(BU_BCFRMBUFA,myframe);

       /* run frame */
       BuBCRunMinor(BU_BCFRMBUFA,BU_BCSINGLE);

       while(BuBCIsFrmActive());

       for(i=0;i<10;i++) {
	          if(BuBCReadMsgNum(myframe,i,&readmsg))
                 printf("error reading message result\n");
		  else 
			  DisplayMsg(&readmsg);
		  
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
