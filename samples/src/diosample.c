/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        Copyright (c) 1995-2004 by Data Device Corp.
 *        All Rights Reserved.
 *
 * DIOSample.c
 *      This program allows the user to control DIO funcions on an EBR PC/104 card
 *
 */

#define ADVACE

/* Remove warnings for deprecated functions */
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#include <stdace.h>

/* These defines are for the calls that operate on individual ports. */
/* The individual port references. */
#define DIO_0		0
#define DIO_1		1
#define DIO_2		2
#define DIO_3		3
#define DIO_4		4
#define DIO_5		5
#define DIO_6		6
#define DIO_7		7
#define DIO_8		8
#define DIO_9		9
#define DIO_10		10
#define DIO_11		11

/* Direction options */
#define DIO_IN		0x0000
#define DIO_OUT		0x8000

/* State options */
#define DIO_OFF		0
#define DIO_ON		1

/* Directive to read the direction or state of a port */
#define DIO_READ	0xFFFF

/* Mode Functions */
#define DIO_ONLY	0x0000
#define DIO_HUB		0x4000

int main (void)
{
	BuConf_t	Conf;	// ACE library configuration
	BuError_t	Err;	// ACE library stderr
	U16BIT		wFunc, wCmd, wDirection;
	U8BIT		bMask, bPort;

	/* display ACE library version information */
	printf("%s\n\n",BuRev());

	/* setup configuration for device X and open ACE library */
	printf("Choose the logical device # of your device:> ");
	scanf("%d",(int*)&Conf.ConfDev);
#ifdef _ACELINUX
	/* Open device for access */
	Err = BuOpenLinux(&Conf);
#else
	/* open device for access */
	Err = BuOpen32(&Conf);
#endif
	/* Turn on Discrete (DIO) function, (vs. HUB function) */
	Err = advBuSetDioFunction(&Conf, DIO_ONLY);

	/* verify DIO function active */
	Err = advBuGetDioFunction(&Conf, &wFunc);
	printf("\nDiscrete Banks configured as: ");
	if (wFunc == DIO_HUB)
		printf("External Hub & DIO\n");
	else
		printf("DIO only\n");

/*-------------------------------------------------------*/
	/* set direction of bank1 to outputs */
	wDirection = DIO_OUT;
	bMask = 0;
	bPort = DIO_1;  /* DIO_1 selected because it's in bank 1 */
	Err = advBuDioDirBank(&Conf
							, &wDirection	/* DIO_OUT */
							, bMask		/* write bank direction */
							, bPort		/* DIO_1: 1st bank */
							);


	/* demonstrate directional read of bank1 to confirm its configururation */
	bMask = 1;
	bPort = DIO_1;	/* DIO_1 selected because it's in bank 1 */
	Err = advBuDioDirBank(&Conf
							, &wDirection	/* U16BIT *bState */
							, bMask		/* read bank direction */
							, bPort		/* DIO_1: 1st bank */
							);


	printf("\nDiscrete Bank1 direction configured as: ");
	if (wDirection == DIO_IN)
		printf("INPUTS\n");
	else
		printf("OUTPUTS\n");

/*-------------------------------------------------------*/
	/* set direction of bank2 to inputs */
	wDirection = DIO_IN;
	bMask = 0;
	bPort = DIO_9;	/* DIO_9 selected because it's in bank 2 */
	Err = advBuDioDirBank(&Conf
							, &wDirection	/* DIO_IN */
							, bMask		/* write bank direction */
							, bPort		/* DIO_9: 2nd bank */
							);


	/* demonstrate directional read of bank2 to confirm its configururation */
	bMask = 1;
	bPort = DIO_9;	/* DIO_9 selected because it's in bank 2 */
	Err = advBuDioDirBank(&Conf
							, &wDirection	/* U16BIT *bState */
							, bMask		/* read bank direction */
							, bPort		/* DIO_9: 2nd bank */
							);

	printf("\nDiscrete Bank2 direction configured as: ");
	if (wDirection == DIO_IN)
		printf("INPUTS\n");
	else
		printf("OUTPUTS\n");

/*-------------------------------------------------------*/
	printf("\nWriting values to Bank 1...");

	/* set DIO_1 high (DIO_ON) : bank1  */
	bPort = DIO_1;
	wCmd = DIO_ON;
	Err = advBuDioWrite(&Conf, bPort, wCmd);

	if (!Err)
		printf("\n Discrete Bank 1: DIO_1 set High");

/*-------------------------------------------------------*/
	/* set DIO_2 low (DIO_OFF) : bank1  */
	bPort = DIO_2;
	wCmd = DIO_OFF;
	Err = advBuDioWrite(&Conf, bPort, wCmd);

	if (!Err)
		printf("\n Discrete Bank 1: DIO_2 set Low");

/*-------------------------------------------------------*/
	/* set DIO_5 high (DIO_ON) : bank1  */
	bPort = DIO_5;
	wCmd = DIO_ON;
	Err = advBuDioWrite(&Conf, bPort, wCmd);

	if (!Err)
		printf("\n Discrete Bank 1: DIO_5 set High");

/*-------------------------------------------------------*/
	printf("\n\nReading values from Bank 2...");

	/* read DIO_9 : bank2 */
	Err = advBuDioRead(&Conf, DIO_9, &wCmd);
	if (wCmd == DIO_OFF)
		printf("\n DIO_9 reads as OFF");
	else
		printf("\n DIO_9 reads as ON");

/*-------------------------------------------------------*/
	/* read DIO_11 : bank2 */
	Err = advBuDioRead(&Conf, DIO_11, &wCmd);
	if (wCmd == DIO_OFF)
		printf("\n DIO_11 reads as OFF");
	else
		printf("\n DIO_11 reads as ON");

/*-------------------------------------------------------*/
	printf("\n\nPress Any Key to Exit.\n");

#ifndef _ACELINUX
	_getch();
	do {	} while(!kbhit());
#endif

  BuClose();
  return 0;
}
