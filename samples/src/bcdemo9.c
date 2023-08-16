/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *        BUS-69082(3) sample  rel 4.6     05-MAY-1999
 *
 *        Copyright (c) 1995,1996,1997 by ILC Data Device Corp.
 *        All Rights Reserved.
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

/* Enumerated values are in words not bytes */
enum{
	MAX_READ_SIZE = 43 * 64,	/* 38(RT-RT)+3(DESC)+2(LEN/BYTE) * MsgCnt */
	BUF_NUM		= 8,							/* 8 buffers total */
	BUF_SIZE	= 24 * 1024,					/* 24K words */
	BUF_FULL	= BUF_SIZE - MAX_READ_SIZE,		/* Buf full condition */
};

BCMinorFrmHandle hMinorFrms[10];  /* each minor frame has a handle */

/* Asynchronous message variables */
int nCount = 0;
BCMsgHandle hAsyncMsg = NULL;
U16BIT data[10]={0x1111,0x2222,0x3333,0x4444,0x5555,
                 0x6666,0x7777,0x8888,0x9999,0x0000};


void DisplayMsg(MsgType *Msg);



/*--------------------------------------------------------------------------
 Name			irqBcHandler

 Description	Not used anymore!
 --------------------------------------------------------------------------*/
void __BUDECL irqBcHandler(U16BIT wIrqStatus)
{
//	BuBCGetMajorFrmsRemaining(&nCount);
//	BuBCReplaceMsg(hMinorFrms[0],2,hAsyncMsg);
}

int main (void)
{
	int i;
	FILE *pFile;			/* Output file handle */
	BuConf_t  Conf;			/* Configuration structure */
	BuError_t Err;			/* Error condition */
	U16BIT *pBuffer;		/* pointer to read buffers */
	U16BIT wWrdCnt=0;			/* Num of words read in buffer */
	U16BIT wResult;			/* place holder for results */
	S32BIT lMjrFrmsRemain=0;	/* # of major frames remaining */
	U32BIT dwMnrFrmIndex=0;	/* index of current minor frame */
	U32BIT dwWrdsRead=0;		/* Num of bytes written to file */
#if 0
	BCMsgHandle msgs[10];      /* each message has a handle */
#endif

#ifdef _ACELINUX
	AceUtilsStart();  /* Start Ace Utilities */
#endif

	/* display revision info */
	printf("%s\n\n",BuRev());
	printf("\nThis BC Demo demonstrates ACE Major Frame functionality using the library.\n");
	printf("A .bsf stack file is used created with message information\n\n");

	/* setup configuration for device X and open ACE library */
	printf("Choose the logical device # of your BC:> ");
	scanf("%d",(int*)&Conf.ConfDev);

	/* Open card and verify */
#ifdef _ACELINUX
	Err = BuOpenLinux(&Conf);
#else
	Err = BuOpen32(&Conf);
#endif

	if(Err)
	{
		printf("BuError %d %s\n",Err,BuErrorStr(Err));
		getch();

#ifdef _ACELINUX
		AceUtilsExit();  /* Exit Ace Utilities */
#endif
		return 0;
	}

	/* Open Ace stack file and verify */
	pFile = fopen("Ace.asf","wb");
	if(!pFile)
	{
		printf("Error opening output file\n");
		BuClose();

#ifdef _ACELINUX
		AceUtilsExit();  /* Exit Ace Utilities */
#endif
		return 0;
	}

	/* Enter bus controller mode of operation */
	Err = BuBCOpen();
	if(Err)
	{
		BuBCClose();
		BuClose();

#ifdef _ACELINUX
		AceUtilsExit();  /* Exit Ace Utilities */
#endif
		return 0;
	}

	/* Create user buffers for reading messages and verify */
	BuBufOpen();
	wResult = BuCreateRWBuffers(BUF_NUM,BUF_SIZE,BUF_FULL);
	if(wResult != BUF_NUM)
	{
		printf("Unable to allocate user buffers\n");
		printf("    (%d buffers requested, only %d allocated)\n",BUF_NUM,wResult);
		BuFreeRWBuffers();
		BuBufClose();
		BuBCClose();
		BuClose();

#ifdef _ACELINUX
		AceUtilsExit();  /* Exit Ace Utilities */
#endif
		return 0;
	}

	/******** is this necessary *********/
	BuTimeout(RESPONSE_505); /* set response timeout to 50.5 us */

	/* Setup asynchronous message */
	hAsyncMsg = BuBCXBCtoRT(15,1,10,CW_CHANNELA,data,BU_BCNOGAP,BU_BCNTIMES|1);
	BuInstallUsrHandler(irqBcHandler);
/*	BuBCSetupIrqMask(TRUE,IRQ_BC_END_OF_FRAME); */

#if 0
	msgs[0]=BuBCXBCtoRT( 1,1,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[1]=BuBCXBCtoRT( 2,2,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[2]=BuBCXBCtoRT( 3,3,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[3]=BuBCXBCtoRT( 4,4,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[4]=BuBCXBCtoRT( 5,5,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[5]=BuBCXBCtoRT( 6,6,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[6]=BuBCXBCtoRT( 7,7,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[7]=BuBCXBCtoRT( 8,8,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );
	msgs[8]=BuBCXBCtoRT( 9,9,1,CW_CHANNELA,data,BU_BCNOGAP,BU_BCALWAYS );

	hMinorFrms[0]=BuBCXMinorFrm( 1000, 3, &msgs[0] );
	hMinorFrms[1]=BuBCXMinorFrm( 1000, 3, &msgs[3] );
	hMinorFrms[2]=BuBCXMinorFrm( 1000, 3, &msgs[6] );

#else

	/* recall minor frames from file */
	if( (!BuBCRecallMinor("frame1.frm",&hMinorFrms[0]))|
		(!BuBCRecallMinor("frame2.frm",&hMinorFrms[1]))|
		(!BuBCRecallMinor("frame3.frm",&hMinorFrms[2]))|
		(!BuBCRecallMinor("frame4.frm",&hMinorFrms[3]))|
		(!BuBCRecallMinor("frame5.frm",&hMinorFrms[4])))
	{
		printf("Error loading .frm file\n");
		BuFreeRWBuffers();
		BuBufClose();
		BuBCClose();
		BuClose();
		return 0;
	}
#endif

	/* Create major frame from minors and run major frame */
	BuBCXMajorFrm(5,hMinorFrms);
	BuBCRunMajor(100,MTmode);

	/* Read user buffers until keypress */
	while(!kbhit())
	{
		if(BuGetReadBufPtr(&pBuffer,&wWrdCnt) == BUF_BUFFERFULL)
		{
			dwWrdsRead += (U32BIT) fwrite(pBuffer,sizeof(U16BIT),wWrdCnt,pFile);
			BuResetReadBuf();
		}

		BuBCGetMajorFrmsRemaining(&lMjrFrmsRemain);
		BuBCGetMinorFrmIndex(&dwMnrFrmIndex);
		
		printf("MjrFrmsRemain = %06d  ",(int)lMjrFrmsRemain);
		printf("MnrFrmIndex = %03d  ",(int)dwMnrFrmIndex);
		printf("BytesStored = %06d\r",(int)((dwWrdsRead+wWrdCnt)*2));

	}
	
	/* Make sure frame is stopped before writing the rest of buffers */
	BuBCHaltMajor();
	while(BuBCIsFrmActive());
	
	/* Write remaining buffers to disk */
	do
	{
		BuGetReadBufPtr(&pBuffer, &wWrdCnt);
		if(wWrdCnt);
		{
			dwWrdsRead += (U32BIT) fwrite(pBuffer,sizeof(U16BIT),wWrdCnt,pFile);
			BuResetReadBuf();

			BuBCGetMajorFrmsRemaining(&lMjrFrmsRemain);
			BuBCGetMinorFrmIndex(&dwMnrFrmIndex);
			printf("MjrFrmsRemain = %06d  ",(int)lMjrFrmsRemain);
			printf("MnrFrmIndex = %03d  ",(int)dwMnrFrmIndex);
			printf("BytesStored = %06d\r",(int)dwWrdsRead*2);
		}
	}while(wWrdCnt);

	printf("\n");


	
	/* Free up minor frame structure and associated messages */
	for(i=0; i<5; i++)
		BuBCFreeMinor(hMinorFrms[i],TRUE);
	
	/* Free up resources and close card */
	fclose(pFile);
	BuFreeRWBuffers();
	BuBufClose();
	BuBCClose();
	BuClose();

#ifdef _ACELINUX
	AceUtilsExit();  /* Start Ace Utilities */
#endif
  return 0;	
}

void DisplayMsg(MsgType*Msg)
{
	U16BIT i;

	printf("Message Type = %s",BuMsgTypeStr(Msg->Type));
	if(Msg->BlockStatus&MT_ERR)
		printf(" (EXCEPTION)\n");
	else
		printf("\n");

	printf(" Cmd1 %04X %s\n",Msg->CmdWord1,BuCmdStr(Msg->CmdWord1));

	if(Msg->CmdWord2flag)
	{
		printf(" Cmd2 %04X %s\n",Msg->CmdWord2,BuCmdStr(Msg->CmdWord2));
	}

	printf(" Time %u uS\n",Msg->TimeTag*2); /*2us resolution*/
	printf(" BSW  %04X %s\n", Msg->BlockStatus,
		BuMTBSWErrorStr(Msg->BlockStatus));

	for(i=0;i<Msg->DataLength;++i)
	{
		if(i==0) printf(" Data ");
		printf("%04X  ",Msg->Data[i]);
		if((i%6)==5) printf("\n      ");
	}

	if(Msg->Status1flag)printf("\n Sta1 %04X",Msg->Status1);
	if(Msg->Status2flag)printf("\n Sta2 %04X",Msg->Status2);
	printf("\n\n");
}
