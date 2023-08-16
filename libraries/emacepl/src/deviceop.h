/*******************************************************************************
 * FILE: deviceop.h
 *
 * DESCRIPTION:
 *
 *   This is the include file for deviceop.c
 *
 ******************************************************************************
 * Non-Disclosure Statement
 * ------------------------
 * This software is the sole property of Data Device Corporation.  All
 * rights, title, ownership, or other interests in the software remain
 * the property of Data Device Corporation.  This software may be used
 * in accordance with applicable licenses.  Any unauthorized use,
 * duplication, transmission, distribution, or disclosure is expressly
 * forbidden.
 *
 * This non-disclosure statement may not be removed or modified without
 * prior written consent of Data Device Corporation.
 ******************************************************************************
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2018 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __DEVICEOP_H__
#define __DEVICEOP_H__

//#include <INTEGRITY.h>
//#include <pthread.h>
//#include <unistd.h>
#include "os/include/ddc_os_types.h"

#define PCI_MEM_WIN_LEN 0x20000 /* EMACE PCI Boards Channel RAM window size (128Kb) */
#define PCI_REG_WIN_LEN 0x4000  /* EMACE PCI Boards REG window size (16Kb) */

#define USB_MAX_CARD            8
#define USB_1553_CHANNELS       2
#define DDC_SYMBOLIC_LINK_SIZE 128

/* DDC Registry Key Base */
#define DDC_REG_KEY_BASE "System\\CurrentControlSet\\Control\\DDC1553Devices\\%d"
#define DDC_REG_KEY_SIZE 128
#define DDC_REG_ITEM_DEVICE  "DeviceID"
#define DDC_REG_ITEM_CHANNEL "Channel"
#define DDC_REG_ITEM_FAMILY  "Family"

/* Single Processor Affinity Mask */
#define SINGLE_PROC_AFFINTIY_MASK   0x1L

/* ========================================================================== */
/*                     Register/Memory Access Functions                       */
/* ========================================================================== */

extern U16BIT _DECL _dvRegRead
(
    S16BIT s16DevNum,
    U16BIT u16RegAddr,
    U16BIT u16Mask
);

extern U32BIT _DECL _dvRegRead32
(
    S16BIT s16DevNum,
    U32BIT u16RegAddr,
    U32BIT u16Mask
);

extern VOID _DECL _dvRegWrite
(   S16BIT s16DevNum,
    U16BIT u16RegAddr,
    U16BIT u16Value,
    U16BIT u16Mask
);

extern VOID _DECL _dvRegWrite32
(   
    S16BIT s16DevNum,
    U32BIT u16RegAddr,
    U32BIT u16Value,
    U32BIT u16Mask
);

extern U16BIT _DECL _dvMemRead
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr
);

extern S16BIT _DECL _dvMemWrite
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U16BIT u16Value
);

extern S16BIT _DECL _dvMemWrite32
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U32BIT u32Value
);

extern S16BIT _DECL _dvClearMemory
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U32BIT u32MemLength
);


/* ========================================================================== */
/*                General Purpose Board Level Access Functions                */
/* ========================================================================== */

extern U32BIT _DECL _dvGpRegRead
(
    S16BIT s16DevNum,
    U32BIT du16RegAddr
);

extern VOID _DECL _dvGpRegWrite
(
    S16BIT s16DevNum,
    U32BIT du16RegAddr,
    U32BIT u32Value,
    U32BIT du16Mask
);


/* ========================================================================== */
/*                      General Command Stack Functions                       */
/* ========================================================================== */

extern VOID _DECL _dvCmdStackRead
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U16BIT *pu16Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvCmdStackRead32
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U32BIT *pu32Buffer,
    U16BIT u16Length
);

extern U16BIT _DECL _DECL _dvGetCommandStkArea
(
    S16BIT s16DevNum
);


/* ========================================================================== */
/*                         MT Command Stack Functions                         */
/* ========================================================================== */

extern VOID _DECL _dvSetMtCommandStkSize
(
    S16BIT s16DevNum,
    U16BIT u16CmdStkSize,
    U16BIT u16Mask

);

extern U32BIT _DECL _dvGetMtCommandStkPointer
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation
);

extern VOID _DECL _dvSetMtCommandStkPointer
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation,
    U32BIT u32StackPtr
);

extern VOID _DECL _dvGetMtCommandStkNext
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U16BIT *p16Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvGetMtCommandStkNext32
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U32BIT *p32Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvGetMtCommandStkLatest
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U16BIT *p16Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvGetMtCommandStkData
(
    S16BIT s16DevNum,
    U32BIT u32Address,
    U32BIT u32DataStkBase,
    U16BIT *p16Buffer,
    U16BIT u16Length,
    U16BIT u16RolloverLength
);

extern VOID _DECL _dvGetMtCommandStkData32
(
    S16BIT s16DevNum,
    U32BIT u32Address,
    U32BIT u32DataStkBase,
    U32BIT *p32Buffer,
    U16BIT u16Length,
    U16BIT u16RolloverLength
);

extern U16BIT _DECL _dvGetMtCommandStkDataCount
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U32BIT u32DataStkAddr,
    U32BIT u32DataStkAddrNext,
    U16BIT u16DataStkSize,
    U16BIT u16TotalDataLength
);

extern VOID _DECL _dvSetMtCommandStkPurge
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation,
    U16BIT u16PurgeData
);


/* ========================================================================== */
/*                         RT Command Stack Functions                         */
/* ========================================================================== */

extern VOID _DECL _dvSetRtCommandStkSize
(
    S16BIT s16DevNum,
    U16BIT u16CmdStkSize,
    U16BIT u16Mask

);

extern U32BIT _DECL _dvGetRtCommandStkPointer
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation
);

extern VOID _DECL _dvSetRtCommandStkPointer
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation,
    U32BIT u32StackPtr
);

extern VOID _DECL _dvGetRtCommandStkNext
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U16BIT *p16Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvGetRtCommandStkNext32
(
    S16BIT s16DevNum,
    U32BIT u32CmdStkAddr,
    U32BIT *p32Buffer,
    U16BIT u16Length
);

extern VOID _DECL _dvGetRtCommandStkData
(
    S16BIT s16DevNum,
    U32BIT u32Address,
    U32BIT u32DataStkBase,
    U16BIT *p16Buffer,
    U16BIT u16Length,
    U16BIT u16RolloverLength
);

extern VOID _DECL _dvGetRtCommandStkData32
(
    S16BIT s16DevNum,
    U32BIT u32Address,
    U32BIT u32DataStkBase,
    U32BIT *p32Buffer,
    U16BIT u16Length,
    U16BIT u16RolloverLength
);

extern VOID _DECL _dvSetRtCommandStkPurge
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation,
    U16BIT u16PurgeData
);


/* ========================================================================== */
/*                            Data Stack Functions                            */
/* ========================================================================== */

extern VOID _DECL _dvDataStackRead
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U16BIT *pu16Buffer,
    U16BIT u16Length,
    U16BIT u16Offset
);

extern VOID _DECL _dvDataStackRead32
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U32BIT *pu32Buffer,
    U16BIT u16Length,
    U16BIT u16Offset
);

extern VOID _DECL _dvDataStackWrite
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    U16BIT *pu16Buffer,
    U16BIT u16Length,
    U16BIT u16Offset
);

extern VOID _DECL _dvSetMtDataStkPointer
(
    S16BIT s16DevNum,
    U32BIT u32StackLocation,
    U32BIT u32StackPtr

);

extern VOID _DECL _dvSetMtDataStkSize
(
    S16BIT s16DevNum,
    U16BIT u16CmdStkSize,
    U16BIT u16Mask

);


/* ========================================================================== */
/*                           Data Block Functions                             */
/* ========================================================================== */

extern U32BIT _DECL _dvGetRtDataBlkOffset
(
    S16BIT s16DevNum,
    U16BIT u16LookupTableAddr,
    U32BIT u32DataBlkBaseAddr
);

extern U32BIT _DECL _dvGetRtDataBlkPointer
(
    S16BIT s16DevNum,
    U16BIT u16LookupTableAddr
);


/* ========================================================================== */
/*                               BC Functions                                 */
/* ========================================================================== */

extern VOID _DECL _dvSetBcMessageDataPtr
(
    S16BIT s16DevNum,
    U32BIT u32MsgAddress,
    U32BIT u32DataBlkPtr
);

extern VOID _DECL _dvSetBcMessageBlk
(
    S16BIT s16DevNum,
    U32BIT u32MsgAddress,
    U16BIT *p16Buffer,
    U16BIT u16Length
);

extern S16BIT _DECL _dvGetBcExtTriggerAvail
(
    S16BIT s16DevNum,
    U32BIT *pu32Avail
);


/* ========================================================================== */
/*                               RT Functions                                 */
/* ========================================================================== */

extern U16BIT _DECL _dvGetRtSubAddrCtrlWord
(
    S16BIT s16DevNum,
    U16BIT u16SubAddress
);

extern VOID _DECL _dvSetRtSubAddrCtrlWord
(
    S16BIT s16DevNum,
    U16BIT u16SubAddress,
    U16BIT u16SubAddressCtrlWord
);

extern VOID _DECL _dvSetRtDataBlkPointer
(
    S16BIT s16DevNum,
    U16BIT u16LookupTableAddr,
    U32BIT u32DataBlkPtr
);


/* ========================================================================== */
/*                               MTI Functions                                */
/* ========================================================================== */

extern S16BIT _DECL _dvSetMtiConfig
(
    S16BIT s16DevNum,
    PMTI_CONFIG pMtiConfig
);

extern VOID _DECL _dvSetMtiState
(
    S16BIT s16DevNum,
    U16BIT u16State
);

extern VOID _DECL _dvSetMtiFree
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvGetMtiCh10DataPkt
(
    S16BIT s16DevNum,
    PMTI_CH10_DATA_PKT *ppCh10Pkt,
    S16BIT s16Timeout
);

extern S16BIT _DECL _dvGetMtiCh10TimePkt
(
    S16BIT s16DevNum,
    PMTI_CH10_TIME_PKT *ppCh10Pkt,
    S16BIT s16Timeout
);

extern S16BIT _DECL _dvMTISetCh10TimePktId
(
    S16BIT s16DevNum,
    U16BIT u16ChannelId
);


/* ========================================================================== */
/*                          Discrete I/O Functions                            */
/* ========================================================================== */

extern DISC_LEVEL _DECL _dvGetDiscOut
(
    S16BIT s16DevNum,
    S16BIT s16Discrete
);

extern S16BIT _DECL _dvGetDiscDir
(
    S16BIT s16DevNum,
    S16BIT s16Discrete
);

extern DISC_LEVEL _DECL _dvGetDiscIn
(
    S16BIT s16DevNum,
    S16BIT s16Discrete
);

extern S16BIT _DECL _dvSetDiscOut
(
    S16BIT s16DevNum,
    S16BIT s16Discrete,
    DISC_LEVEL eDiscreteLevel
);

extern S16BIT _DECL _dvSetDiscDir
(
    S16BIT s16DevNum,
    S16BIT s16Discrete,
    DISC_DIR eDiscreteDirection
);

extern S16BIT _DECL _dvGetDiscAll
(
    S16BIT s16DevNum,
    U16BIT *pDiscreteDirections,
    U16BIT *pDiscreteLevels
);

extern S16BIT _DECL _dvSetDiscAll
(
    S16BIT s16DevNum,
    U16BIT u16DisceteDirections,
    U16BIT u16DiscreteLevels
);


/* ========================================================================== */
/*                          Avionic I/O Functions                             */
/* ========================================================================== */


extern AVIONIC_LEVEL _DECL _dvGetAioOutput
(
    S16BIT s16DevNum,
    S16BIT s16Avionic
);

extern AVIONIC_DIRECTION _DECL _dvGetAioDir
(
    S16BIT s16DevNum,
    S16BIT s16Avionic
);

extern AVIONIC_LEVEL _DECL _dvGetAioInput
(
    S16BIT s16DevNum,
    S16BIT s16Avionic
);

extern S16BIT _DECL _dvSetAioOutput
(
    S16BIT s16DevNum,
    S16BIT s16Avionic,
    AVIONIC_LEVEL eAvionicLevel
);

extern S16BIT _DECL _dvSetAioDir
(
    S16BIT s16DevNum,
    S16BIT s16Avionic,
    AVIONIC_DIRECTION eAvionicDirection
);

extern S16BIT _DECL _dvGetAioAll
(
    S16BIT s16DevNum,
    U16BIT *pu16AvionicDirections,
    U16BIT *pu16AvionicLevels
);

extern S16BIT _DECL _dvSetAioAll
(
    S16BIT s16DevNum,
    U16BIT u16AvionicDirections, 
    U16BIT u16AvionicLevels
);


/* ========================================================================== */
/*                    Hardware Access/Info Functions                          */
/* ========================================================================== */

extern VOID _DECL _dvDeviceInit
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvGetHwVersionInfo
(
    S16BIT s16DevNum,
    PHWVERSIONINFO  pHwVersionInfo
);

extern S16BIT _DECL _dvGetChannelCount
(
    S16BIT s16DevNum,
    CHANCOUNT_t *pChanCount
);

extern VOID _DECL _dvGetTimeTagValue
(
    S16BIT s16DevNum,
    U64BIT  *pu64TimeTag
);

extern VOID _DECL _dvSetTimeTagValue
(
    S16BIT s16DevNum,
    U64BIT  u64TimeTag
);

extern S16BIT _DECL _dvGetIRIGTx
(
    S16BIT s16DevNum,
    PACE_IRIG_TX psIrigTx
);

extern S16BIT _DECL _dvSetIRIGTx
(
    S16BIT s16DevNum,
    ACE_IRIG_TX sIrigTx
);


/* ========================================================================== */
/*                             Misc. Functions                                */
/* ========================================================================== */

extern S16BIT _DECL _dvDriverResourceAllocation
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvDriverResourceRelease
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvIODriverResourceAllocation
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvIODriverResourceRelease
(
    S16BIT s16DevNum
);

extern VOID _dvInitializeCriticalSection
(
    S16BIT s16DevNum
);

extern VOID _dvDeleteCriticalSection
(
    S16BIT s16DevNum
);

extern VOID _dvEnterCriticalSection
(
    S16BIT s16DevNum
);

extern VOID _dvLeaveCriticalSection
(
    S16BIT s16DevNum
);

extern S16BIT _aceSetTaskPriority
(
    S16BIT s16DevNum,
    U16BIT u16Type,
    U32BIT u32Priority
);


#endif /* __DEVICEOP_H_ */
