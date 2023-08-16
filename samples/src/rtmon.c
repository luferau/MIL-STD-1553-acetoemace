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
 * rtmon.c
 *        Demonstrates use of ACE Library in dual RT/MT mode.
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

/* Remove warnings for deprecated functions */
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#define RTaddress 5

#define MONSTKSIZE 8*1024
U16BIT MonStk[MONSTKSIZE];

void DisplayMsg(MsgType*Msg);

int main()
{
  MsgType   msg;  /* structure to hold message information */
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */
 // BuError_t error,temp;
  U16BIT MonStkLen;
  BuError_t error_code;
 // U16BIT CTable[128];

  /* each subaddress has a dynamically allocated memory block */
  RTBlkHandle   r1;

  /* the variables in this structure are modified to configure a subaddress */
  SubAddrCtrlWrd sub_addr_config;

  /* tx buffer, tx value (to fill buffer) and active buffer value */
  //U16BIT	txbuf[32];
  int       x;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT\\MT Demo sets up RT 5 SA 1 to demonstrate the RT and MT dual\n");
  printf("functionality of the ace product.\n\n");

  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # for your RT\\MT:> ");
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
    BuRTMONOpen();

//#ifdef lori
  BuMTStkType(MT_DOUBLE,CMD256,DATA1024);

  /* def stack A to have a command stack at address 0, initially */
  /* pointing to address 0x40 and a length of 256 words. And define */
  /* the data stack to be at address 0x400, with the pointer initially */
  /* starting at 0x500 and a length of 0x400. */

  BuMTDefStk(MT_STACKA,0x0000,0x0040,0x0400,0x100);

  /* def stack B to have a command stack at address 0x100, initially */
  /* pointing to address 0x140 and a length of 256 words. And define the */
  /* data stack to be at address 0x800, with the pointer initially */
  /* starting at 0x900 and a length of 0x400. */

  BuMTDefStk(MT_STACKB,0x0100,0x0040,0x0800,0x100);
//#endif

  /* enable monitoring all rts, transmit/receive and subaddresses */
  BuMTEnableRT(MT_ALL,MT_ALL,MT_ALL);

    /* Call to BuMTEnableRT, above, will cause an RT configuration with default values,
     * therefore must due further RT configuration after the above function completes. */

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

	/*allocate data blocks*/
	r1 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);

	/*map the data block to the lookup table*/
	BuRTMapBlk(1, RECEIVE, r1, 0);

	/*legalize subaddresses */
	BuRTDefMsgLegal(RECEIVE, 01, ALL);


  BuRTMONRun();

  printf("\nRT address %d\nrx subaddr 1\n\n",RTaddress);
  printf("Press any key to stop.\n\n");

  while(!kbhit())
  {
	if(BuRTReadMsg(LAST_MESSAGE,&msg)==BU_SUCCESS)
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

  BuMTHalt();
  getch();
  MonStkLen=MONSTKSIZE;
  error_code=BuMTReadStk(MonStk,&MonStkLen);

  if(!error_code)
  {
	  U16BIT TempStkLen=MonStkLen;
	  U16BIT *TempStkPtr=MonStk;
	  while(TempStkLen&&(!kbhit()))
	  {
		  error_code=BuMTDecodeStk(&TempStkPtr,&TempStkLen,&msg);
		  if(!error_code)
			  DisplayMsg(&msg);
		  else
			  printf("Error decoding stack (Error #0x%x)\n",error_code);
	  }
  }else
  {
	  if(error_code==BU_ERROR_MTBUFEMPTY)
		  printf("Stack empty\n");
      else
		  printf("*** Error reading stack (Error #0x%x)\n", error_code);
  }

	BuRTMONClose();

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
  printf(" BSW  %04X %s\n", Msg->BlockStatus,
                            BuMTBSWErrorStr(Msg->BlockStatus));

  for(i=0;i<Msg->DataLength;++i) {
         if(i==0)printf(" Data ");
         printf("%04X  ",Msg->Data[i]);
         if((i%6)==5)printf("\n      ");
  }

  if(Msg->Status1flag)printf("\n Sta1 %04X",Msg->Status1);
  if(Msg->Status2flag)printf("\n Sta2 %04X",Msg->Status2);
  printf("\n\n");
}
