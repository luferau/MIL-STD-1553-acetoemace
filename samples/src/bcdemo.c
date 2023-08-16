/* Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) sample  rel 4.0     29-NOV-2000
 *
 *        Copyright (c) 1995,1996,1997,2000 by Data Device Corp.
 *        All Rights Reserved.
 *
 * bcdemo1.c
 *          Uses BuBCSendData routine for creation and execution of
 *          a BC to RT message and BuBCGetData routine for creation and
 *          execution of a RT to BC message. These integrated routines
 *          are an alternative to setting up major and minor frame
 *          sequences.
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

int main (void)
{
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */
  int rt=0,sa=0,wc=0,tr=0;
  BCMsgHandle msg;           /* message handle */
  BCMinorFrmHandle myframe;  /* minor frame handle */
  MsgType readmsg;           /* holds message result read from frame */
  
  U16BIT data[32]={ 0X0000,0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,
                    0x8888,0x9999,0xAAAA,0xBBBB,0xCCCC,0xDDDD,0xEEEE,0xFFFF,
                    0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
                    0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo allows the user to transmit or receive a single message\n");
  printf("based on user parameters.\n\n");
  
  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # of your BC?> ");
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

  /* opens bus controller mode */
  BuBCOpen();

  printf("\nChoose the RT Address> ");
  scanf("%d",&rt);
  printf("\nChoose the Subaddress> ");
  scanf("%d",&sa);
  printf("\nChoose the Word Count> ");
  scanf("%d",&wc);
  printf("\nTransmit(0) or Receive(1)> "); 
  scanf("%d",&tr);
  

  printf("\nRT=%d,SA=%d,WC=%d,T\\R*=%d\n\n",rt,sa,wc,tr);

  /* set response timeout to 50.5 us */
  BuTimeout(RESPONSE_505);

  if(!tr)
  {
       /* create bc->rt */
       msg=BuBCXBCtoRT((U8BIT)rt,(U8BIT)sa,(U8BIT)wc,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
  }else 
  {
       /* create bc<-rt */
       msg=BuBCXRTtoBC((U8BIT)rt,(U8BIT)sa,(U8BIT)wc,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
  }

  /* create minor frame */
  myframe=BuBCXMinorFrm(30000l,1,&msg);

  /* load minor frame into ACE stack */
  BuBCLoadMinor(BU_BCFRMBUFA,myframe);

  /* run frame */
  BuBCRunMinor(BU_BCFRMBUFA,BU_BCSINGLE);

  while(BuBCIsFrmActive());

  /* display data */
  if(BuBCReadMsgNum(myframe,0,&readmsg))
{
	printf("error reading message result\n");
} 
 else 
  {
	  U16BIT i;

	  printf("Message Type = %s",BuMsgTypeStr(0xFF));//Freadmsg.Type));
	  
	  if(readmsg.BlockStatus&MT_ERR)
		  printf(" (EXCEPTION)\n");
	  else
		  printf("\n");

	  printf(" Cmd1 %04X %s\n",readmsg.CmdWord1,BuCmdStr(readmsg.CmdWord1));

	  if(readmsg.CmdWord2flag)
	  {
		  printf(" Cmd2 %04X %s\n",readmsg.CmdWord2,BuCmdStr(readmsg.CmdWord2));
	  }

	  printf(" Time %u uS\n",readmsg.TimeTag*2); /*2us resolution*/
	  printf(" GapT %u uS\n",readmsg.GapTime); /*1us resolution*/
	  printf(" BSW  %04X %s\n",readmsg.BlockStatus,BuBCBSWErrorStr(readmsg.BlockStatus));
	  printf(" Ctrl %04X \n",readmsg.ControlWord);

	  for(i=0;i<readmsg.DataLength;++i)
	  {
		  if(i==0)
			  printf(" Data ");

		  printf("%04X  ",readmsg.Data[i]);

		  if((i%6)==5)
			  printf("\n      ");
	  }

	  if(readmsg.Status1flag)
		  printf("\n Sta1 %04X",readmsg.Status1);

	  if(readmsg.Status2flag)
		  printf("\n Sta2 %04X",readmsg.Status2);

	  if(readmsg.LoopBack1flag)
		  printf("\n Lpb1 %04X",readmsg.LoopBack1);
	  if(readmsg.LoopBack2flag)
		  printf("\n Lpb2 %04X",readmsg.LoopBack2);

	  printf("\n\n");
  }
  
  /* closes bus controller mode */
  BuBCClose();

  /* must call at end of ACE library use */
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
