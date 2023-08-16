/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integerated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) sample  rel 4.0     29-NOV-1997
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * selftst4.c
 *           performs selftest of ace using ace test vectors in
 *           external file "test.vec"
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
  BuConf_t  Conf;
  BuTest_t  Test;
  BuError_t Err;
  char buf[80]="test.vec";

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("This selftest uses Ace test vectors in file test.vec to check device.\n\n");

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

  /* Set the Interrupt Service Routine to disabled */
  Conf.BuIsrEnabled=FALSE;

  Test.code=0; /* initialize test structure */

  /* Run Test */
  printf("\nRunning Test Vectors...\n");
  BuVectorTest(buf,&Test);

  if(Test.code==TEST_FINISHED)
  {
	printf("\nPassed... no failures detected.\n");
	printf("Test vector end of file reached...at line #%d\n",Test.count);
  }
  else
  {
	printf("\nFAILURES DETECTED!!\n");
	printf("Failure...at line #%d\n",Test.count);
    printf("          location=%s\n",(Test.code==TEST_FAILED_MVECTOR)?"memory":"register");
    printf("          address=%04x\n",Test.addr);
    printf("          expected=%04x\n",Test.tdata);
    printf("          actual=%04x\n",Test.data);
  }


  /*Must close out properly*/
  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  return 0;
}
