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
 * rtdemo2.c
 *          Programs RT 5 with subadress 1 to receive
 *          a circular buffer of 128 words, subaddress 2 to
 *          transmit a single message of 32 words. This uses
 *          an interrupt service routine to store the received
 *          messages from STACK A to a circular queue.
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
#define  txval 0xABCD

int volatile BufferHead = 0, BufferTail = 0;
long total_messages=0;
MsgType Buffer[100];

/* If an interrupt occurs, read the message(s) received and store it in
 * the "Buffer". The "BufferHead" increments to index to the next available
 * buffer position.
 */

void __BUDECL MyIsr(U16BIT status)
{
	if(status&(IRQ_RT_CIRC_BUFFR_ROLLOVR|IRQ_RT_BC_MESSAGE_INT)) {
	  while(((BufferHead+1)%100)!=BufferTail)
	  {
			if(BuRTReadMsg(LAST_MESSAGE,&Buffer[BufferHead])==BU_SUCCESS) {
				BufferHead=(BufferHead+1)%100;
				total_messages++;
			} else {
				break;
			}
	  }
	}
}

int main(void)
{
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type  */
  int x=0;

  /* handles to the memory blocks allocated for a subaddress */
  RTBlkHandle   r1,t1;

  SubAddrCtrlWrd sub_addr_config;

  U16BIT txbuf[32];

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to receive a circular buffer of 128 words\n");
  printf("and SA 2 to transmit a single message of 32 words. An ISR is used to store\n");
  printf("messages from STACK A to circular buffer.\n\n");

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
  sub_addr_config.BcstEomInt      = FALSE;
  sub_addr_config.RxEomInt        = FALSE;
  sub_addr_config.TxEomInt        = TRUE;
  sub_addr_config.BcstBuffInt     = FALSE;
  sub_addr_config.RxBuffInt       = TRUE;
  sub_addr_config.TxBuffInt       = FALSE;
  sub_addr_config.BcstMm          = SINGLE_MESSAGE;
  sub_addr_config.RxMm            = RTBUFFER128;
  sub_addr_config.TxMm            = SINGLE_MESSAGE;

  /*initialize subaddresses*/
  BuRTDefSA(1, &sub_addr_config);
  BuRTDefSA(2, &sub_addr_config);

  /*allocate data blocks*/
  r1 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
  t1 = BuRTAllocBlk((U8BIT)sub_addr_config.TxMm);

  /*setup transit data*/
  for (x=0;x<32;x++)
		txbuf[x]=txval;

  /*write block to card memory*/
  BuWriteBlk(t1, txbuf, 32);

  /*map the data blocks to the lookup table*/
  printf( "map rx blk\n" );
  BuRTMapBlk(1, RECEIVE, r1,0);

  printf( "map tx blk\n" );
  BuRTMapBlk(2, TRANSMIT, t1,0);

  /*legalize subaddresses */
  BuRTDefMsgLegal(RECEIVE, 01, ALL);
  BuRTDefMsgLegal(TRANSMIT, 02, ALL);

  /*setup interrupt handler*/
  BuInstallUsrHandler(MyIsr);

  /*enable end of message interrupts*/
  BuIrqEnable(IRQ_RT_BC_MESSAGE_INT|IRQ_RT_CIRC_BUFFR_ROLLOVR);

  BuRTRun();

  /* display RT info */
  printf("RT address %d\n\n",RTaddress);
  printf("rx 128 words subaddr 1\ntx 32 words subaddr 2\n");
  printf("tx buffer contains 32 words of %4x\n\nPress any key to stop.\n\n",txval);

	while(!kbhit())
	{
	  if (BufferHead!=BufferTail)
	  {
	    /***************************************************************/
	    /* This section of this program is where the bulk of the data  */
	    /* processing would take place. For the purpose of this        */
	    /* example, the four word descriptor stack entry is printed    */
	    /* for each 1553 message that is processed.                    */
	    /***************************************************************/
		  printf("Bsw %4x TimeTag %8u uS WordCount %3d Cmd %4x \n",
			  Buffer[BufferTail].BlockStatus,
			  Buffer[BufferTail].TimeTag*2,
			  Buffer[BufferTail].WordCount,
			  Buffer[BufferTail].CmdWord1);

		  if(Buffer[BufferTail].WordCount)
		  {
			  for(x=0;x<Buffer[BufferTail].WordCount;x++)
				  printf("%04x ",Buffer[BufferTail].Data[x]);printf("\n");
		  }
	   BufferTail = (BufferTail+1)%100;
	   }
	}/*end while*/

  /* cleanup */
  MyIsr(IRQ_RT_BC_MESSAGE_INT|IRQ_RT_CIRC_BUFFR_ROLLOVR);

  while(BufferHead!=BufferTail)
  {
	  printf("Bsw %4x TimeTag %8u uS WordCount %3d Cmd %4x \n",
		  Buffer[BufferTail].BlockStatus,Buffer[BufferTail].TimeTag*2,
		  Buffer[BufferTail].WordCount,Buffer[BufferTail].CmdWord1);

	  if(Buffer[BufferTail].WordCount)
	  {
		  for(x=0;x<Buffer[BufferTail].WordCount;x++)
			  printf("%04x ",Buffer[BufferTail].Data[x]);printf("\n");
	  }
	  BufferTail = (BufferTail+1)%100;
  }

  /* Close out of the library properly*/
  BuRTClose();
  BuClose();

  printf("\n\nTotal messages=%ld\n",total_messages);

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}



