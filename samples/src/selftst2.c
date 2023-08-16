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
 * selftst2.c
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

static BuTest_t Test;

void RamTest(U16BIT value)
{
  printf("Testing...");
  BuRamTest(value,&Test);
  if(Test.code==TEST_PASSED)printf("Ram Passed %04x test...\n",value);
  else {
       printf("Ram Failed %04x test, data read = %04x addr = %d!!!\n",
                   value,
                   Test.data,
                   Test.addr);
  }
}

void RegTest(void)
{
  printf("Testing...");
  BuRegisterTest(&Test);
  if(Test.code==TEST_PASSED)printf("Registers Passed test...\n");
  else {
       printf("Register Failed test, expected=%04x read=%04x!!!\n",
                   Test.tdata,
                   Test.data);
  }

}

void ProtocolTest(void)
{
  printf("Testing...");
  BuProtocolTest(&Test);
  if(Test.code==TEST_PASSED)printf("Bus Controller Protocol Unit Passed test...\n");
  else {
       printf("Bus Controller Protocol Unit Failed test, expected=%04x read=%04x addr=%04x!!!\n",
                   Test.tdata,
                   Test.data,
                   Test.addr);
  }
}

void InterruptTest(int irqLevel)
{
  printf("Testing...");
  BuIrqTest(&Test);
   if(Test.code==TEST_PASSED)printf("Interrupt Occurred on %d, test passed...\n",irqLevel);
  else {
       printf("Interrupt Test Failure, %s %s!!!\n",
                   (Test.data&1)?"NO TimeTag Rollover":"",
                   (Test.data&2)?"NO IRQ":"");
  }

}

int main (int argc, char *argv[])
{
  BuConf_p Conf;
  BuError_t Err;
  char buf[300];
  unsigned char devnum;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("\n%s\n\n",BuRev());

  if(argc!=2)	// Program Input
  {
	  /* setup configuration for device X and open ACE library */
	  printf("What logical device would you like to test?> ");
	  scanf("%d",(int*)&devnum);

  }
  else	// Parameter Input
  {
		devnum=atoi(argv[1]);

		if(devnum>31)
		{
			printf("\nBad input parameter - Device number should be in 0-31 range.\n\n");

			sprintf(buf,"\n%s\n\n%s\n\n%s\n%s\n%s\n",
					" Proper Command Line Usage:",
					"\t\tC:\\> selftst2 <device #>",
					" The <device #> parameter is the logical number of the device to be tested.",
					" This number is associated with your device through the Ace Manager Control",
					" Panel Applet.");

			printf("%s",buf);

			#ifdef _ACELINUX
				AceUtilsExit();  /* Exit Ace Utilities */
			#endif

			return 0;
		}
  }

  /* Have the library allocate space for Conf structure and return
   * pointer.
   */
  Conf=BuAllocConf();

  /* setup logical device #'s from Ace Manager */
 Conf->ConfDev=devnum;

#ifdef _ACELINUX
  Err=BuOpenLinux(Conf);
#else
  Err=BuOpen32(Conf);
#endif

 if(Err) {
    printf("BuError %d %s\n",Err,BuErrorStr(Err));

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif
    return 0;
  }


  printf("\n*****Testing Device Number %d*****\n",Conf->ConfDev);
  RegTest();
  RamTest(0xaaaa);
  RamTest(0xaa55);
  RamTest(0x55aa);
  RamTest(0x5555);
  RamTest(0xffff);
  RamTest(0x1111);
  RamTest(0x8888);
  RamTest(0x0000);
  ProtocolTest();
  InterruptTest(Conf->BuIrq);


  /* Must close out properly and deallocate storage for conf structure */
  BuClose();
  BuDeallocConf(Conf);

#ifndef _ACELINUX
  printf("\nPress Any Key to Exit.\n");
  do {	} while(!kbhit());
#endif

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif

  return 0;
}
