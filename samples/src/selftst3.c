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
 * selftst3.c
 *           Performs simple selftest of ACE using built in library
 *           test routine BuValid.
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

int main (void)
{
  BuConf_t Conf;
  int Err;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("This selftest only tests Registers and RAM (not Protocol or Interrupts).\n\n");


  /* setup configuration for device X and open ACE library */
  printf("What logical device would you like to test?> ");
  scanf("%d",(int*)&Conf.ConfDev);

#ifdef _ACELINUX
  Err=BuOpenLinux(&Conf);
#else
  Err=BuOpen32(&Conf);
#endif

  if(Err) {
       printf("BuError %d %s\n",Err,BuErrorStr(Err));
       return 0;
  }



  if(BuValid()) {
       printf("\nPass\n");
       /*      .
               .
               normal program execution
               .
               .
       */
  } else printf("\nFail\n");

  /* Must close out properly*/
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
