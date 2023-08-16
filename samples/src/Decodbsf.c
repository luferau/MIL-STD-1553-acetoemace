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
 * decodbsf.c
 *           Bus Controller demo program that decodes a binary ACE stack
 *           file (ace.bsf) and displays the messages contained
 *           within the file. This program works in conjunction with
 *           bcdemo7.c which transmits major frames on the 1553 bus and
 *           stores the bc stack in an ACE bus controller stack file.
 *
 */

//#ifdef _MSC_VER
//#define gotoxy(x,y) _settextposition(y,x)
//#include <sys\types.h>
//#include <io.h>
//#include <errno.h>
//#endif

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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "aceutils.h"
#define getch() readch()
#else
/* DOS/Windows Headers */
#include <sys\stat.h>
#include <fcntl.h>
#include <conio.h>
#include <io.h>
#endif

void DisplayMsg(MsgType*Msg)
{
  U16BIT i;

  printf("Message Type = %s",BuMsgTypeStr(Msg->Type));
  if(Msg->BlockStatus&MT_ERR)printf(" (EXCEPTION)\n");else printf("\n");


  printf(" Cmd1 %04X %s\n",Msg->CmdWord1,BuCmdStr(Msg->CmdWord1));

  if(Msg->CmdWord2flag) {
       printf(" Cmd2 %04X %s\n",Msg->CmdWord2,BuCmdStr(Msg->CmdWord2));
  }

  printf(" Time %u uS\n",Msg->TimeTag*2); /*2us resolution*/
  printf(" GapT %u uS\n",Msg->GapTime); /*1us resolution*/
  printf(" BSW  %04X %s\n",Msg->BlockStatus,BuBCBSWErrorStr(Msg->BlockStatus));
  printf(" Ctrl %04X \n",Msg->ControlWord);

  for(i=0;i<Msg->DataLength;++i) {
     if(i==0)printf(" Data ");
     printf("%04X  ",Msg->Data[i]);
     if((i%6)==5)printf("\n      ");
  }
  if(Msg->Status1flag)printf("\n Sta1 %04X",Msg->Status1);
  if(Msg->Status2flag)printf("\n Sta2 %04X",Msg->Status2);
  if(Msg->LoopBack1flag)printf("\n Lpb1 %04X",Msg->LoopBack1);
  if(Msg->LoopBack2flag)printf("\n Lpb2 %04X",Msg->LoopBack2);
  printf("\n\n");
}

#define BUFFER_SIZE (4*1024)
U16BIT Buffer[BUFFER_SIZE];

void main()
{
  int in_handle;
  BuError_t error;
  MsgType msg;
  int BufLen;
  U16BIT TempStkLen;
  U16BIT *TempStkPtr;
  int long TotalBytes = 0;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  /* open input file (Ace Stack File) */
  /* change the default file mode from text to binary */
#ifndef _ACELINUX
  _fmode=O_BINARY;
#endif
  in_handle=open("ACE.BSF",S_IREAD);
      if(in_handle==-1) {
       printf("error opening output file\n" );
       exit(1);
  }
#ifdef _ACELINUX
  BufLen=read(in_handle,(char *)Buffer,BUFFER_SIZE*sizeof(U16BIT));
#else
  BufLen=read(in_handle,(char far*)Buffer,BUFFER_SIZE*sizeof(U16BIT));
#endif
  TotalBytes+=BufLen;
  TempStkLen=BufLen/2; /* convert to words */
  TempStkPtr=Buffer;
  while(TempStkLen&&(!kbhit())) {
       /* if we are close to the end of the stack and the file is not */
       /* empty then read in more stack data. Max message length is 4 */
       /* words header + 1 cmd + 2 status + 32 data words = 39 */
       if((TempStkLen<39)&&!eof(in_handle)) {
        int i;
          /* copy data from end of stack to beginning of stack */
        /* and append with data read from file */
        for(i=0;i<(TempStkLen);i++)
         Buffer[i]=*(TempStkPtr+i);
#ifdef _ACELINUX
         BufLen=read(in_handle,((char *)Buffer)+
                (TempStkLen*sizeof(U16BIT)),
                (BUFFER_SIZE-TempStkLen)*sizeof(U16BIT));
#else
         BufLen=read(in_handle,((char far*)Buffer)+
                (TempStkLen*sizeof(U16BIT)),
                (BUFFER_SIZE-TempStkLen)*sizeof(U16BIT));
#endif
         TotalBytes+=BufLen;
         TempStkPtr=Buffer;
         TempStkLen+=BufLen/2;
       }
       error=BuBCDecodeStk(&TempStkPtr,&TempStkLen,&msg);
       if(!error)DisplayMsg(&msg);
       else {
        printf("\nError decoding stack (Error #0x%x)\n", error);
        TempStkLen=0; /* exit */
       }
  }
  /* make sure that we clear keyboard buffer if we are dropping out */
  /* becuase of keypress */
  while(kbhit())getch();

  printf("\nA total of %ld bytes were decoded\n",
       TotalBytes-(TempStkLen*sizeof(U16BIT)));

  close(in_handle);

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif

  return;
}
