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
 * bcdemo6.c
 *          This creates 5 sample frame (.frm) files that are used in
 *          BCDEMO7. The messages are first created with the BuBCX message
 *          constructors. Their handles are stored in the array (msgs).
 *          A ptr to this array and a minor frame time is passed into
 *          BuBCXMinorFrm. The MinorFrm handle and a filename is passed
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

int main (void)
{
  BuConf_t  Conf; /* ACE library configuration type */
  BuError_t Err;  /* ACE library error status type */
  int i=0;

  BCMsgHandle msgs[10];      /* each message has a handle */
  BCMinorFrmHandle myframe;  /* each minor frame has a handle */

  U16BIT data[10]={ 0x1111,0x2222,0x3333,0x4444,0x5555,
                    0x6666,0x7777,0x8888,0x9999,0x0000};
  U16BIT data1[1]={0xabcd};

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo creates 5 sample frame (.frm) files to be used in BCDEMO7.\n\n");
  
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

       BuBCOpen();

       /* minor frame 1 */

       {
       U16BIT cnt=0;
       msgs[cnt++]=BuBCXBCtoRT(1,2,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXModeRx(9,CW_CHANNELA,0x01,data1,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(1,1,5,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       myframe=BuBCXMinorFrm(10000,cnt,msgs);
       }

       i+=BuBCStoreMinor("frame1.frm",myframe);
       BuBCFreeMinor(myframe,TRUE);

       /* minor frame 2 */

       {
       U16BIT cnt=0;
       msgs[cnt++]=BuBCXBCtoRT(2,2,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(1,1,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(2,2,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(3,1,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(4,2,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXBCtoRT(5,1,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       myframe=BuBCXMinorFrm(10000,cnt,msgs);
       }

       i+=BuBCStoreMinor("frame2.frm",myframe);
       BuBCFreeMinor(myframe,TRUE);

       /* minor frame 3 */

       {
       U16BIT cnt=0;
       msgs[cnt++]=BuBCXBCtoRT(3,2,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXRTtoRT(1,1,2,3,7,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXRTtoRT(3,3,4,2,1,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXRTtoRT(2,2,5,3,3,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       myframe=BuBCXMinorFrm(20000,cnt,msgs);
       }

       i+=BuBCStoreMinor("frame3.frm",myframe);
       BuBCFreeMinor(myframe,TRUE);

       /* minor frame 4 */

       {
       U16BIT cnt=0;
       msgs[cnt++]=BuBCXBrdcst(3,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS);
       myframe=BuBCXMinorFrm(10000,cnt,msgs);
       }

       i+=BuBCStoreMinor("frame4.frm",myframe);
       BuBCFreeMinor(myframe,TRUE);

       /* minor frame 5 */

       {
       U16BIT cnt=0;
       msgs[cnt++]=BuBCXRTtoBC(1,1,10,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXRTtoBC(1,2,10,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       msgs[cnt++]=BuBCXRTtoBC(1,3,10,CW_CHANNELA,BU_BCNOGAP,BU_BCALWAYS);
       myframe=BuBCXMinorFrm(20000,cnt,msgs);
       }

       i+=BuBCStoreMinor("frame5.frm",myframe);
       BuBCFreeMinor(myframe,TRUE);

       printf("%d = Total Messges Stored\n",i);

       /* should be called to close bus controller mode */
       BuBCClose();

  } else printf("error communicating with pc card\n");

  BuClose();

#ifdef _ACELINUX
	AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}

