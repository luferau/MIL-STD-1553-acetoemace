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
 * selftst5.c
 *           Displays reading from ACE timetag register until key is
 *           pressed. Good way to check basic register I/O
 *           functionality.
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
  BuError_t Err;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis selftest reads and displays the timetag register.\n\n");

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
#ifdef _ACELINUX
       AceUtilsExit();  /* Exit Ace Utilities */
#endif
       return 0;
  }

  if(BuValid()) {
       printf("\nPress any key to begin and end.\n");while(!kbhit());getch();
       BuTimeTagResolution(BU_TIMETAG_64);
       while(!kbhit()) printf("timetag=%04x\n",BuReadTimeTag());

  } else printf("error communicating with ACE\n");

  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
