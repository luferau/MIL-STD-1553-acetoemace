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
 * loop.c
 *           Performs a loop around selftest of an ACE using internal
 *           test mode registers. The test transmits a word on one
 *           channel and receives the transmitted word on the other
 *           channel. This test requires that a loop back cable be
 *           used which will connect channel A to channel B with the
 *           appropriate termination.
 *
 * Summary of Operation
 *           This test makes use of the ACE's internal test mode
 *           registers. The encoder is controlled through the use
 *           of test register #1. The bit map for the purpose of
 *           this test are as follows:
 *
 *              msb bit 15: Encoder Channel A/B*
 *                      14: set to 0
 *                      13: Transmitter Enable
 *                      12: Encoder Enable
 *                      11: Command/Data* sync
 *                      10: set to 0
 *                       9: set to 0
 *                       8: set to 0
 *                       7: set to 0
 *                       6: set to 0
 *                       5: set to 1
 *                       4: set to 0
 *                       3: set to 0
 *                       2: set to 0
 *                       1: set to 0
 *               lsb bit 0: set to 0
 *
 *           The encoder is activated by asserting a Logic 1 to both
 *           encoder enable and transmitter enable. Once the device
 *           begins transmitting, encoder enable may be removed and
 *           the transmission will stop at the completion of the current
 *           word. Removing the transmitter enable signal once a
 *           transmission has begun will truncate the word immdediately.
 *           Bit 15 should be set to logic 1 to transmit on channel A or
 *           logic 0 to transmit on channel B. Bit 11 should be set to
 *           logic 1 to produce a command sync or a logic 0 to produce a
 *           data sync on the transmitted word. The transmitted word will
 *           be fetched from the BC Frame Time/RT Last Command/MT Trigger
 *           Word registers. Any 16 bit value may be placed into this
 *           register.
 *
 *           Once the transmission is started (by asserting transmitter
 *           enabled and encoder enable, encoder enable is removed
 *           immediately. This will ensure that only one word is transmitted
 *           (assuming that there is less than 20 usec between successive
 *           write transfers). Next the program waits for the word to be
 *           received by the monitor on the opposite channel or for a
 *           software timeout to occur. Note that the monitor will not
 *           store the word on the channel on which it is transmitting.
 *           The program polls the tag word of the first monitor stack
 *           entry. When a word is stored the monitor tag word will contain
 *           a non-zero value. The timeout count of 100 is chosen to
 *           guarantee at least a 20 usec delay to allow the word to be
 *           transfered (ACE access time is 200 ns * 100 transfers = 20 usec).
 *
 *           Once the word is detected by the monitor, then the tag word
 *           for the stored word is verified (ignoring the gap time) and
 *           the actual word received is verified.
 *
 *           This test is repeated for both channels transmitting.
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
  BuConf_t Conf;
  BuError_t Err;
  U16BIT dummy;
  int count = 0;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* display revision info */
  printf("%s\n\n",BuRev());
  printf("\nThis program performs a loopback test across A & B channels.\n");
  printf("It is assumed you have connected a cable between both channels.\n\n");

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

  /* Test A looped into B */
  printf("\nRunning Loop Test A=>B\n");
  BuWriteReg(0x0003,0x0001); // reset
  BuWriteReg(0x0007,0x8000); // config 3: enhanced mode
  BuWriteReg(0x0001,0x4000); // config 1: word monitor mode
  BuWriteRam(0x0100,0x0000); // initialize stack to 0
  BuWriteRam(0x0000,0x0000); // zero first monitor stack entry (rx word)
  BuWriteRam(0x0001,0x0000); // zero first monitor stack entry (tag word)
  BuWriteReg(0x0003,0x0002); // start monitor

  BuWriteReg(0x0008,0x0002); // config 4: encoder test mode
  BuWriteReg(0x000D,0x1234); // place tx word in aux 1

  /* step 1: assert encoder enable and tx enable with desired channel and sync type */
  BuWriteReg(0x0011,0xb820);
  /* step 2: remove encoder enable immediately to ensure only 1 word is transmitted */
  BuWriteReg(0x0011,0xA820);

  /* wait for word to be transfered */
  count = 0;
  do
  {
    dummy = BuReadRam(0x0001);
    count++;
  } while ((!dummy) && (count < 100));

  if((dummy&0x00ff) != 0x00ED)  /* notice that gap time is masked */
    printf("...Failed loopback Test.  Expected Monitor Tag word 0xXXED received 0x%04x\n", dummy);
  else printf("...Passed, correct Tag word.\n");

  dummy = BuReadRam(0x0000);
  if(dummy != 0x1234)
    printf("...Failed loopback Test.  Expected Monitored Word 0x1234 received 0x%04x\n", dummy);
  else printf("...Passed, correct monitored word.\n");

  BuWriteReg(03, 01); /* reset the ACE to return it to a known state */

  /* Now test B looped into A */
  printf("Running Loop Test B=>A\n");
  BuWriteReg(0x0003,0x0001); // reset
  BuWriteReg(0x0007,0x8000); // config 3: enhanced mode
  BuWriteReg(0x0001,0x4000); // config 1: word monitor mode
  BuWriteRam(0x0100,0x0000); // initialize stack to 0
  BuWriteRam(0x0000,0x0000); // zero first monitor stack entry (rx word)
  BuWriteRam(0x0001,0x0000); // zero first monitor stack entry (tag word)
  BuWriteReg(0x0003,0x0002); // start monitor

  BuWriteReg(0x0008,0x0002); // config 4: encoder test mode
  BuWriteReg(0x000D,0x1234); // place tx word in aux 1

  /* step 1: assert encoder enable and tx enable with desired channel and sync type */
  BuWriteReg(0x0011,0x3820);
  /* step 2: remove encoder enable immediately to ensure only 1 word is transmitted */
  BuWriteReg(0x0011,0x2820);

  /* wait for word to be transfered */
  count = 0;
  do
    {
    dummy = BuReadRam(0x0001);
    count++;
    } while ((!dummy) && (count < 100));

  if((dummy&0x00ff) != 0x00E9)  /* notice that gap time is masked */
    printf("...Failed loopback Test.  Expected Monitor Tag word 0xXXED received 0x%04x\n", dummy);
  else printf("...Passed, correct Tag word.\n");

  dummy = BuReadRam(0x0000);
  if(dummy != 0x1234)
    printf("...Failed loopback Test.  Expected Monitored Word 0x1234 received 0x%04x\n", dummy);
  else printf("...Passed, correct monitored word.\n");

  BuWriteReg(03, 01); /* reset the ACE to return it to a known state */

  BuClose();

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
  
  return 0;
}
