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
 * rtdemo3.c
 *          This demo programs RT 5 with subaddress 1 to receive
 *          a single message of 32 words; subaddress 2 to receive
 *          a single message of 32 words; and subadress 3 to
 *          transmit 32 words containing the most current output
 *          from an operation performed with the data recieved by
 *          subadress 1 and 2.
 *
 *          Note: This program uses a polling technique to determine
 *          if a new buffer has been received (and the transmit buffer
 *          needs to be updated). If a transmit is sent back to back
 *          after a receive, the transmit buffer will not have been
 *          updated if the loop is relative slow.
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

/* the following give us access to macros defined in the
 * header files which indirectly access the BuConf structure
 */
//extern BuConf_p BuConf;

#define RTaddr   5

#define COMMAND_FIELDS(wrd) (*((CmdWordType*)&wrd))

void update_transmit_buffer(void);
void display_data(RTBlkHandle blkhdl);

MsgType msg;

/*block_handle holds info about an allocated memory blk*/

RTBlkHandle r1,r2,t1=NULL,temp_handle;

/*
 * This is called when data is recieved by the RT. It takes the
 * data mapped to Subaddr 1 and Subaddr 2 and adds them together
 * putting it in the transmit buffer of Subaddr 3.
 */

void update_transmit_buffer() {

 /* Note that this function simply adds the data gathered from
  * from Subaddr 1 & 2. This is where the bulk of the data
  * processing would occur
  */

  U16BIT buffer1[32],buffer2[32],buffer3[32],x=0;

  BuReadBlk(r1,buffer1,32);
  BuReadBlk(r2,buffer2,32);

  for(x=0;x<32;x++) {
	  buffer3[x]=buffer1[x]+buffer2[x];
  }

  temp_handle=BuRTAllocBlk(SINGLE_MESSAGE);
  BuWriteBlk(temp_handle,buffer3,32);
  BuRTMapBlk(3, TRANSMIT, temp_handle,0);
  if (t1)
	BuRTFreeBlk(t1);
  t1=temp_handle;
}

/*
 * The data from the block handle passed in is display on the screen. The
 * size is determined from the "size" field in the handle. This field is
 * the number of words allocated for that block. (32,128,256 etc..). A more
 * efficient way of retrieving data from a data block is to do a
 * read_data_block(blkhdl, dataptr, size) where dataptr is a pointer to
 * u16bit (where data will be copied to) and size which is the number of
 * 16-bit words to read.
 */

void display_data(RTBlkHandle blkhdl)
{
  int x;
	for(x=0;x<blkhdl->size;x++)
		printf("%4x ",BuReadRam((U16BIT)(blkhdl->absaddr+x)));
	printf("\n\n");
}


int main(void)
{
  SubAddrCtrlWrd sub_addr_config;
  BuError_t Err;  /* ACE library error status type  */
  U16BIT buffer1[32]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  U16BIT buffer2[32]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1&2 to receive 32 words and RT 5 SA 3 to\n");
  printf("transmit 32 words based on the latest received data from SA 1&2.\n\n");

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
  BuRTAddress(RTaddr);

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

  BuRTDefSA(1, &sub_addr_config);
  BuRTDefSA(2, &sub_addr_config);
  BuRTDefSA(3, &sub_addr_config);

  /*allocate data blocks*/
  r1 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
  r2 = BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
//  t1 = BuRTAllocBlk((U8BIT)sub_addr_config.TxMm);

  /*map the data blocks to the lookup table*/
  BuRTMapBlk(1, RECEIVE, r1,0);
  BuRTMapBlk(2, RECEIVE, r2,0);
//  BuRTMapBlk(3, TRANSMIT, t1,0);

  /*fill memory*/
  BuBlockFill(r1->absaddr,1,32);
  BuBlockFill(r2->absaddr,2,32);
  BuWriteBlk(r1,buffer1,32);
  BuWriteBlk(r2,buffer2,32);

  update_transmit_buffer();

  /*legalize subaddresses */
  BuRTDefMsgLegal(RECEIVE, 01, ALL);
  BuRTDefMsgLegal(RECEIVE, 02, ALL);
  BuRTDefMsgLegal(TRANSMIT, 03, ALL);

  BuRTRun();

  printf("\nRT address %d\nrx->SA 1&2 32 Words\ntx->SA 3 32 Words\n",RTaddr);
  printf(" transmit buffer contains:\n");

  display_data(t1);

  printf("\nPress any key to exit program.\n");
  while(!kbhit())
  {
	  if(BuRTReadMsg(LAST_MESSAGE,&msg)==BU_SUCCESS)
	  {
		  if(COMMAND_FIELDS(msg.CmdWord1).TR)
			  update_transmit_buffer();

		  printf("\nMESSAGE -> BSW=%4x  TTAG=%8u  WC=%2d  CWRD=%4x\n",
			  msg.BlockStatus,
			  msg.TimeTag*2,
			  msg.WordCount,
			  msg.CmdWord1);

		  printf("\nRT address %d\nrx->SA 1&2 32 Words\ntx->SA 3 32 Words\n",RTaddr);
          printf(" transmit buffer contains:\n");
  		  display_data(t1);
		  printf("\nPress any key to exit program.\n");
	  }
  }/*end while*/

  /* Must close out of library properly */
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
