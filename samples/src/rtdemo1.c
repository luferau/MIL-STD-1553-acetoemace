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
 * rtdemo1.c
 *          This programs RT 5 with subadress 1 to receive 32 words
 *          and subadress 2 to transmit 32 words. Status of RECIEVE
 *          messages are displayed and value of 32 transmitted words
 *          can be changed from 1 to 9. This does not use interrupts
 *          but polls STACK A to retrieve messages.
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

#define  RTaddress   5

int main(void)
{
  MsgType   msg;  /* structure to hold message information */
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */

  /* each subaddress has a dynamically allocated memory block */
  RTBlkHandle   r1,t1,t2;

  /* the variables in this structure are modified to configure a subaddress */
  SubAddrCtrlWrd sub_addr_config;

  /* tx buffer, tx value (to fill buffer) and active buffer value */
  U16BIT	txbuf[32];
  U16BIT    txvalue=1;
  U16BIT    actbuf=0;
  int       x,keyin;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to receive 32 words and SA 2 to transmit\n");
  printf("32 words. The values of the transmitted words can be changed from 1-9.\n\n");

  /* setup configuration for device X and open ACE library */
  printf("Choose the logical device # of your RT?> ");
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
	t1 = BuRTAllocBlk((U8BIT)sub_addr_config.TxMm);
	t2 = BuRTAllocBlk((U8BIT)sub_addr_config.TxMm);

	/*map the data block to the lookup table*/
	BuRTMapBlk(1, RECEIVE, r1, 0);

	/*legalize subaddresses */
	BuRTDefMsgLegal(RECEIVE, 01, ALL);
	BuRTDefMsgLegal(TRANSMIT, 02, ALL);

	/*setup transit data*/
	for(x=0;x<32;x++)txbuf[x]=txvalue;

	/*write block to card memory*/
	BuWriteBlk(actbuf?t1:t2,txbuf,32);
	BuRTMapBlk(2,TRANSMIT,actbuf?t1:t2,0);

    BuRTRun();

	do{

		printf("\nRT address %d\n\nrx subaddr 1\n",RTaddress);
		printf("tx subaddr 2\n");
		printf("tx buffer contains 32 words of %d\n\n",txvalue);
		printf("1-9 reassign transmit data new values\n0 exit\n");

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
				  for(x=0;x<msg.WordCount;x++)printf("%04x ",msg.Data[x]);printf("\n");
			  }
			}
		}

	  /*key has been hit here*/
	  keyin=getch()-48;

	  /*check keyboad data*/
	  if((keyin>0)&&(keyin<=9))
	  {
		  txvalue=keyin;
		  for(x=0;x<32;x++)txbuf[x]=txvalue;
		  /*buffer that is currently NOT being used has data written*/
		  /*to it to ensure data integrity*/
		  actbuf=~actbuf;
		  BuWriteBlk(actbuf?t1:t2, txbuf, 32);
		  BuRTMapBlk(2,TRANSMIT,actbuf?t1:t2,0);
	  }

	}while(keyin!=0);

	/*Must close out of library properly*/
	BuRTClose();
	BuClose();

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
