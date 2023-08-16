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
 * bcdemo3.c
 *          This creates a sample frame (test.frm) file that is used in
 *          BCDEMO 4 and 5. The messages are first created with the BuBCX
 *          message constructors. Their handles are stored in the array
 *          (msgs). A ptr to this array and a minor frame time is passed
 *          into BuBCXMinorFrm. The MinorFrm handle and a filename is passed
 *          into BuBCStoreMinor (saving the minor frame to a file).
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

  BCMsgHandle msgs[11];      /* each message has a handle */
  BCMinorFrmHandle myframe;  /* each minor frame has a handle */

  U16BIT data[32]={ 0x1111,0x2222,0x3333,0x4444,0x5555,
                    0x6666,0x7777,0x8888,0x9999,0x0000,
                    0x0001,0x0002,0x0004,0x0008,0x0010,
                    0x0020,0x0040,0x0080,0x0100,0x0200,
                    0x0400,0x0800,0x1000,0x2000,0x4000,
                    0x8000,0x55AA,0xAA55,0xA5A5,0xAAAA,
                    0xA555,0x555A};
  U16BIT data1[1]={0xabcd};

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo creates a sample frame (test.frm) file that is used in BCDEMO 4&5.\n\n");
    
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

       /* create bc->rt rt 3 sa 4 receive 32 words from bus controller */
       msgs[0]=BuBCXBCtoRT(3,4,32,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);

       /* create bc<-rt rt 1 sa 5 transmit 32 words to bus controller */
       msgs[1]=BuBCXRTtoBC(1,5,32,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* create bc<-rt rt 1 sa 5 transmit 7 words to bus controller */
       msgs[2]=BuBCXRTtoBC(1,5,7,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* create rt<-rt rt 2 sa 3 receive 5 words from rt 6 sa 7 */
       msgs[3]=BuBCXRTtoRT(2,3,5,6,2,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode (00011 initiate self-test) rt 8 */
       msgs[4]=BuBCXMode(8,CW_CHANNELA,0x03,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode tx with data (10010 tx last cmd) rt 6 */
       msgs[5]=BuBCXModeTx(6,CW_CHANNELA,0x02,BU_BCNOGAP,BU_BCALWAYS);

       /* create mode rx with data (10001 synchronize) rt 9 */
       msgs[6]=BuBCXModeRx(9,CW_CHANNELA,0x01,data1,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast rx sa 3 10 words */
       msgs[7]=BuBCXBrdcst(3,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);

       /* rt(s)<-rt broadcast rx sa 3, tx rt 7 sa 4 6 words */
       msgs[8]=BuBCXBrdcstRTtoRT(3,6,7,4,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast mode rx data (10001 synchronize) */
       msgs[9]=BuBCXBrdcstModeRx(CW_CHANNELA,0x01,data1,BU_BCNOGAP,BU_BCALWAYS);

       /* bc->rt(s) broadcast mode no data (00100 transmitter shutdown) */
       msgs[10]=BuBCXBrdcstMode(CW_CHANNELA,0x01,BU_BCNOGAP,BU_BCALWAYS);

       /* create minor frame */
       myframe=BuBCXMinorFrm(30000l,11,msgs);

       /* store minor frame to file */
       i=BuBCStoreMinor("test.frm",myframe);

       printf("%d Messges Stored\n",i);

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

