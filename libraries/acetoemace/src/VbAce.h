#ifndef VBACE_H_INCLUDED
#define VBACE_H_INCLUDED
/*
Ace40 conversion
Company: - Choudhary International
Author: - Brijesh Shah
*/
/*AceWin.h: interface for the AceWin class.
/*
////////////////////////////////////////////////////////////////////*/

/* include necessary OLE headers */
#include <stdace.h>

/* String related functions */
__BUEXTERN LONG __BUDECL vbGetAddressForVBString(BSTR *);
__BUEXTERN BSTR __BUDECL vbGetStringFromLPSTR(LPSTR);

/* Address related functions */
__BUEXTERN intptr_t __BUDECL vbGetAddressForObject(LPVOID);
__BUEXTERN VOID __BUDECL vbCopyData(LPVOID source, LPVOID dest, DWORD size);

__BUEXTERN BSTR __BUDECL vbBuMsgTypeStr(U8BIT value);
__BUEXTERN BSTR __BUDECL vbBuCmdStr(U16BIT value);
__BUEXTERN BSTR __BUDECL vbBuErrorStr(BuError_t BuError);
__BUEXTERN BSTR __BUDECL vbBuRev(void);
__BUEXTERN BSTR __BUDECL vbBuBCBSWErrorStr(U16BIT value);
__BUEXTERN BSTR __BUDECL vbBuTestStr(U8BIT Code);
__BUEXTERN BuConf_t * __BUDECL vbBuAllocConf(int DevNum );

__BUEXTERN LONG __BUDECL vbGetInt(U8BIT Byte1, U8BIT Byte2);
__BUEXTERN double __BUDECL vbGetLong(U8BIT Byte1, U8BIT Byte2, U8BIT Byte3, U8BIT Byte4);
__BUEXTERN void __BUDECL vbSetInt(U8BIT * Byte1, U8BIT * Byte2, U16BIT Value);
__BUEXTERN void __BUDECL vbSetLong(U8BIT * Byte1, U8BIT * Byte2, U8BIT * Byte3, U8BIT * Byte4,U32BIT Value);
__BUEXTERN LONG __BUDECL vbRetInt(U16BIT * Intptr);
__BUEXTERN U8BIT __BUDECL vbRetByte(U8BIT * Byteptr);
__BUEXTERN double __BUDECL vbRetLong(U32BIT * Longptr);
__BUEXTERN U8BIT __BUDECL vbRetRTWordElement(U8BIT WordType,U8BIT Element,U16BIT IntValue);
__BUEXTERN BuError_t __BUDECL vbSetRTWordElement(U8BIT WordType,U8BIT Element,U16BIT * DestInt,U8BIT Value);

__BUEXTERN long __BUDECL  vbBuCreateCmdWord( U16BIT rt,U16BIT tr,U16BIT sa,U16BIT wc);
__BUEXTERN long __BUDECL  vbBuGetMsgWordCount( MsgType*msg);
__BUEXTERN long __BUDECL  vbBuGetMsgType( MsgType*msg,U8BIT Mode);
__BUEXTERN long __BUDECL  vbBuBCActiveStk(void);
__BUEXTERN long __BUDECL vbBuBCIsFrmActive(void);
__BUEXTERN long __BUDECL  vbBuBCIsMsgActive(void);
__BUEXTERN long __BUDECL  vbBuBCCalcDataSize( U16BIT CtrlWord,U16BIT CmdWord1);
__BUEXTERN long __BUDECL  vbBuBCCalcSize( U16BIT CtrlWord,U16BIT CmdWord1);
__BUEXTERN long __BUDECL  vbBuBCStoreMinor( char *fn,BCMinorFrmHandle frm);
__BUEXTERN long __BUDECL  vbBuBCRecallMinor( char *fn,BCMinorFrmHandle *frm);
__BUEXTERN long __BUDECL  vbBuCreateRWBuffers( U16BIT NumBuffers,U32BIT BufferSize,U32BIT BufferFull);
__BUEXTERN long __BUDECL  vbBuWriteBufFull( void);
__BUEXTERN long __BUDECL  vbBuUpdateWriteBuf( U32BIT count);
__BUEXTERN long __BUDECL  vbBuGetReadBufPtr( U16BIT **Ptr,U16BIT *Words);
__BUEXTERN long __BUDECL  vbBuResetReadBuf( void);
__BUEXTERN long __BUDECL  vbBuReadReg( U16BIT offset);
__BUEXTERN long __BUDECL  vbBuReadRam( U16BIT offset);
__BUEXTERN long __BUDECL  vbBuGetIrqStatus( void);
__BUEXTERN long __BUDECL  vbBuMTGetActiveStk( void);
__BUEXTERN long __BUDECL  vbBuMTGetInactiveStk( void);
__BUEXTERN long __BUDECL  vbBuMTIsActiveStk( U16BIT stk);
__BUEXTERN long __BUDECL  vbBuMTLookup( U16BIT rt,U16BIT tr, U16BIT sa);
__BUEXTERN long __BUDECL  vbBuMTReadMsg( MsgType *msg);
__BUEXTERN long __BUDECL  vbsacw2word(SubAddrCtrlWrd*  Sacw);
__BUEXTERN long __BUDECL  vbBuRTMapBlk(U16BIT SubAddr,U16BIT t_r,RTBlkHandle BlockHdl, U16BIT Offset);
__BUEXTERN long __BUDECL vbBuRTReadMsg(U16BIT MessageNum, MsgType *Message);
__BUEXTERN long __BUDECL  vbBuRTReadInactive(RTBlkHandle BlockHdl, U16BIT __BUFAR *Buffer);
__BUEXTERN long __BUDECL  vbBuRTReadEnhMCData(U16BIT addr);
__BUEXTERN long __BUDECL  vbBuRTBITRead(void);
__BUEXTERN long __BUDECL  vbBuRTAltStatusRead(void);
__BUEXTERN long __BUDECL  vbBu_selftest( char *error_msg);


#endif //VBACE_H_INCLUDED