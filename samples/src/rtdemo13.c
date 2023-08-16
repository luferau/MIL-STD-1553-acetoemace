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
 * rtdemo13.c
 *        The BuRTReadMsg() routine can be passed the LAST_MESSAGE
 *        defined constant. This mode utilizes an internal counter to
 *        return messages in a FIFO format. Many applications may
 *        require a periodic update of the latest RT data and message
 *        status. The following sample program demonstrates a LIFO
 *        reading of the RT stack utilizing the BuRTReadMsg() absolute
 *        addressing mode.
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include "aceutils.h"
#define getch() readch()
#else
/* DOS/Windows Headers */
#include <sys\stat.h>
#include <fcntl.h>
#include <conio.h>
#include <time.h>
#include <io.h>
#endif

BuConf_t  Conf;

#define delay(x) {int v;for(v=0;v<10000*x;v++);}

#define  RTaddr   5

int main(void)
{
	BuError_t Err;
	RTBlkHandle rx;
	SubAddrCtrlWrd sub_addr_config;
#if 0
	time_t t;
#endif

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis RT Demo sets up RT 5 SA 1 to demonstrate reading of the RT stack\n");
  printf("utilizing the BuRTReadMsg() absolute addressing mode.\n\n");

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

  {/*initialize RT mode*/
	  BuRTOpen();
	  BuRTAddress(RTaddr);
	  BuRTDefMsgIllegal(ALL,ALL,ALL);

	  sub_addr_config.BcstEomInt      = FALSE;
	  sub_addr_config.RxEomInt        = FALSE;
	  sub_addr_config.TxEomInt        = FALSE;
	  sub_addr_config.BcstBuffInt     = FALSE;
	  sub_addr_config.RxBuffInt       = FALSE;
	  sub_addr_config.TxBuffInt       = FALSE;
	  sub_addr_config.BcstMm          = SINGLE_MESSAGE;
	  sub_addr_config.RxMm            = RTBUFFER128;
	  sub_addr_config.TxMm            = SINGLE_MESSAGE;

	  BuRTDefSA(1,&sub_addr_config);
	  rx=BuRTAllocBlk((U8BIT)sub_addr_config.RxMm);
	  BuRTMapBlk(1, RECEIVE, rx,0);
	  BuRTDefMsgLegal(RECEIVE, 01, ALL);

      /*clearing out stack*/
      BuBlockFill(STACK_A,0,256);
  }/*initialize RT mode*/

  {/*activate RT*/
	  printf("\nRT address %d\n",RTaddr);
	  printf("rx 128 word buffer subaddr 1\n\n");
	  printf("Press any key to start and stop...\n\n");
	  while(!kbhit());
	  getch();
	  BuRTRun();
  }/*activate RT*/

  while(!kbhit())
  {/*loop till user abort*/

    /*display time extra*/
//    t=time(NULL);printf("[%ld] ",t);

    {/*update with latest data*/
    U16BIT Hptr;/*where we start looking*/
    U16BIT Sptr;/*index to scan stack*/
    int Hup=1;/*0 is no update, 1 is updated, 2 is error*/
    MsgType m;/*store message here*/

	  {/*start at previous entry from current STACK_POINTER_A*/
        Sptr=BuReadRam(STACK_POINTER_A);
        Hptr=Sptr=(Sptr-4)%256; /*previous entry*/
      }/*start at previous entry from current STACK_POINTER_A*/

Sptr%= 0xff;

      while(BuReadRam(Sptr)&0x4000) {/*som bit set*/
        Sptr=(Sptr-4)%256; /*previous entry*/
        if(Sptr==Hptr) {/*complete circle, som bit set for all?*/
          Hup=2;break;
        }/*error, terminate loop*/

      }/*adjust Sptr to previous entry*/

      /*may have found an entry where som bit is not set*/
      /*check to see if eom bit is set then*/
      if(!(BuReadRam(Sptr)&0x8000)) {/*eom bit not set*/
        Hup=0;
      }/*no updated messages*/

	  if(Hup==1)
	  {/*valid updated message*/
        int tt=BuReadRam((U16BIT)(Sptr+1));
        BuRTReadMsg(Sptr,&m);
        /*the following line will effectively clear out a message
         *after it is read, this is a way of determining if a message
         *was updated or not, take the following line out and the
         *(m) message will always be the latest
		 */
         BuWriteRam(Sptr,0);
		 if (m.WordCount)
			printf("SPTR 0x%x BSW %4x TT %8u uS CMD %4x DW01 %04x DW%02d %04x\n", Sptr,
			 m.BlockStatus,
			 m.TimeTag*2,
			 m.CmdWord1,
			 m.Data[0],
			 m.WordCount,
			 m.Data[m.WordCount-1]);
		 else
			printf("SPTR 0x%x BSW %4x TT %8u uS CMD %4x\n", Sptr,
			 m.BlockStatus,
			 m.TimeTag*2,
			 m.CmdWord1);

		 if(tt!=m.TimeTag){printf("error in BuRTReadMsg()\n");break;}
	  }/*read message info and display*/

      else
      if(Hup==0) {/*no updated messages*/
        ; //printf("no update\n");
      }/*indicate no update*/
      else {/*error occured*/
        printf("internal stack error occurred\n");
        break;
      }/*indicate no update*/

    }/*update with latest data*/


    delay(10);/* simulate 100 Hz rate*/

  }

  /*Must close out of library properly*/
  BuRTClose();
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
