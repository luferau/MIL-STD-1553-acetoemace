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
 * decodrsf.c
 *           Decodes .rsf file create by RTDEMO12.C and displays
 *           message information to the screen. Total bytes decoded
 *           is displayed at the end of the run. The user can abort
 *           the decode at any time with a keypress.
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

/* get a 16 bit word from the file */

unsigned int getword(FILE *fp)
{
  char buffer1,buffer2;
  fscanf(fp,"%c%c",&buffer1,&buffer2);
  return( ((unsigned int)(buffer2&0xff)<<8)|((unsigned int)(buffer1&0x00ff)));
}

/* display the message/data to the screen */

void DisplayMsg(MsgType*Msg)
{
  U16BIT i;
  printf(" Cmd1 %04X %s\n",Msg->CmdWord1,BuCmdStr(Msg->CmdWord1));
  printf(" WdCt %d\n",Msg->WordCount);
  printf(" Time %u uS\n",Msg->TimeTag*2); /*2us resolution*/
  printf(" BSW  %04X\n", Msg->BlockStatus);
  for(i=0;i<Msg->WordCount;++i) {
         if(i==0)printf(" Data ");
         printf("%04X  ",Msg->Data[i]);
         if((i%6)==5)printf("\n      ");
  }
  printf("\n\n");
}

void main()
{
  FILE *in;
  int count=0,x=0;
  MsgType msg;

#ifdef _ACELINUX
  AceUtilsStart();  /* Start Ace Utilities */
#endif

  if((in=fopen("ace.rsf","rb"))==NULL) {
    fprintf(stderr,"Cannot open input file.\n");

#ifdef _ACELINUX
    AceUtilsExit();  /* Exit Ace Utilities */
#endif

    return;
  }
  while(!feof(in)&&(!kbhit())) {
    msg.BlockStatus=getword(in);
    msg.TimeTag=getword(in);
    msg.CmdWord1=getword(in);
    msg.WordCount=msg.CmdWord1&0x1f;
    /* convert 0 to 32 words */
    if(msg.WordCount==0)msg.WordCount=32;
    if(!feof(in)) {
      for(x=0;x<msg.WordCount;x++)msg.Data[x]=getword(in);
      DisplayMsg(&msg);
      count+=msg.WordCount+3;
    }
  }
  fclose(in);
  printf("\n%d bytes decoded",count<<1);

#ifdef _ACELINUX
  AceUtilsExit();  /* Exit Ace Utilities */
#endif
}


