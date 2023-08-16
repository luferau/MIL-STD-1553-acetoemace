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

  U16BIT data[32]={ 0X0000,0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,
                    0x8888,0x9999,0xAAAA,0xBBBB,0xCCCC,0xDDDD,0xEEEE,0xFFFF,
                    0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
                    0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis BC Demo sets the Ace card in BC mode and sends a few messages.\n\n");
  
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

  /* opens bus controller mode */
  BuBCOpen();

  /* send 10 words to rt 5 sa 1 on channel A */
  Err=BuBCSendData(CW_CHANNELA,5,1,data,10);

  printf("%s\n",BuErrorStr(Err));

  /* send 32 words to rt 6 sa 1 on channel A
     added this [01-SEP-1995] to test 32 word case */
  Err=BuBCSendData(CW_CHANNELA,6,1,data,32);
  printf("%s\n",BuErrorStr(Err));

  /* receive 5 words from rt 8 sa 4 on channel A */
  Err=BuBCGetData(CW_CHANNELA,8,4,data,5);
  printf("%s\n",BuErrorStr(Err));

  /* display data */
  if(!Err){int x;for(x=0;x<5;x++)printf("%04x ",data[x]);printf("\n");}

  /* receive 32 words from rt 7 sa 2 on channel A
     added this [01-SEP-1995] to test 32 word case */
  Err=BuBCGetData(CW_CHANNELA,7,2,data,32);
  printf("%s\n",BuErrorStr(Err));

  /* display data */
  if(!Err){int x;for(x=0;x<32;x++)printf("%04x ",data[x]);}

  /* closes bus controller mode */
  BuBCClose();

  /* must call at end of ACE library use */
  BuClose();

#ifdef _ACELINUX
	AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
