/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * selftst.c
 *           Performs detailed selftest of ACE using built in
 *           library test routines. The TestResult structure
 *           returns information on the failure.
 *
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
  U16BIT st_ret;
  char ret_str[80]="This is the initial string\0";

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("This selftest tests Registers, RAM, Protocol, and Interrupts.\n\n");


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

  /* Call self test */
  st_ret=Bu_selftest(ret_str);

  if(st_ret!=BU_ERROR_HIGHBOUND)
  {
	/* print the returned results */
	printf("\nTest results = %s\n",ret_str);
	printf("Test results = %d\n",st_ret);
  }
  else
  {
	 printf("(ERROR) Selftest failed to run.\n");
  }


  /* must be called at end of ACE library use for each device opened*/
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif

  return 0;
}
