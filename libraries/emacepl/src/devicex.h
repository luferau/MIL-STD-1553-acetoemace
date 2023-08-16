/*******************************************************************************
 * FILE: devicex.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide ACEX driver access for the RTL.
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

#ifndef __DEVICEX_H__
#define __DEVICEX_H__

/* ========================================================================== */
/*                                GENERAL DEFINITIONS                         */
/* ========================================================================== */

/* MRT Improvements IOCTL Command Code */
#define IOCTL_IMP_MRT_CMD                 0

/* 1553 IOCTL ACCESS TYPES */
#define REG_TYPE_ABSOLUTE_ADDR   0
#define REG_TYPE_BD_OFFSET       1
#define REG_TYPE_BC_OFFSET       2
#define REG_TYPE_MRT_OFFSET      3
#define REG_TYPE_MTI_OFFSET      4
#define REG_TYPE_1553_OFFSET     5
#define REG_TYPE_EI_OFFSET       6
#define REG_TYPE_REPLAY_OFFSET   7
#define REG_TYPE_TRG_OFFSET      8

#define ACEX_16_BIT_ACCESS 0
#define ACEX_32_BIT_ACCESS -1

#define IOCTL_16_BIT_ACCESS 0
#define IOCTL_32_BIT_ACCESS 1

#if DDC_PPC
    #define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -2
#else
    #define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -1
#endif

#define ACEX_HANDLE         DDC_HANDLE


/* DMA Cache Line Size Padding Macro - 32 bytes accommodates all possible bus widths */
#define DMA_CACHE_LINE_PAD    32


/* ========================================================================== */
/*                         BUS CONTROLLER DEFINITIONS                         */
/* ========================================================================== */

/*  dwIrqMask parameter can be the following OR'd together */
#define ACE_IMR1_EOM                    0x00000001
#define ACE_IMR1_BC_STATUS_SET          0x00000002
#define ACE_IMR1_RT_MODE_CODE           0x00000002
#define ACE_IMR1_MT_PATTERN_TRIG        0x00000002
#define ACE_IMR1_FORMAT_ERR             0x00000004
#define ACE_IMR1_BC_END_OF_FRM          0x00000008
#define ACE_IMR1_BC_MSG_EOM             0x00000010
#define ACE_IMR1_RT_SUBADDR_EOM         0x00000010
#define ACE_IMR1_RT_CIRCBUF_ROVER       0x00000020
#define ACE_IMR1_TT_ROVER               0x00000040
#define ACE_IMR1_RT_ADDR_PAR_ERR        0x00000080
#define ACE_IMR1_BC_RETRY               0x00000100
#define ACE_IMR1_HSHAKE_FAIL            0x00000200
#define ACE_IMR1_MT_DATASTK_ROVER       0x00000400
#define ACE_IMR1_MT_CMDSTK_ROVER        0x00000800
#define ACE_IMR1_BCRT_CMDSTK_ROVER      0x00001000
#define ACE_IMR1_BCRT_TX_TIMEOUT        0x00002000
#define ACE_IMR1_RAM_PAR_ERR            0x00004000
#define ACE_IMR2_IRIG_INTERRUPT         0x00008000
#define ACE_IMR2_BIT_TRIGGER            0x00010000
#define ACE_IMR2_BIT_COMPLETE           0x00020000
#define ACE_IMR2_FLEX_DMA_ENABLE        0x00020000
#define ACE_IMR2_BC_UIRQ0               0x00040000
#define ACE_IMR2_BC_UIRQ1               0x00080000
#define ACE_IMR2_BC_UIRQ2               0x00100000
#define ACE_IMR2_BC_UIRQ3               0x00200000
#define ACE_IMR2_MT_DSTK_50P_ROVER      0x00400000
#define ACE_IMR2_MT_CSTK_50P_ROVER      0x00800000
#define ACE_IMR2_RT_CIRC_50P_ROVER      0x01000000
#define ACE_IMR2_RT_CSTK_50P_ROVER      0x02000000
#define ACE_IMR2_BC_TRAP                0x04000000
#define ACE_IMR2_BC_CALLSTK_ERR         0x08000000
#define ACE_IMR2_GPQ_ISQ_ROVER          0x10000000
#define ACE_IMR2_RT_ILL_CMD             0x20000000
#define ACE_IMR2_BC_OPCODE_PARITY       0x40000000
#define ACE_IMR2_MTI_INTERRUPT          0x80000000


/* ========================================================================== */
/*                        Configuration/Init Functions                        */
/* ========================================================================== */

extern S16BIT _dvxGetApiType
(
    S16BIT s16DevNum
);

extern S16BIT _dvxDriverResourceAllocation
(
    S16BIT s16DevNum
);

extern S16BIT _dvxDriverResourceRelease
(
    S16BIT s16DevNum
);

extern S16BIT _dvxIODriverResourceAllocation
(
    S16BIT s16DevNum
);

extern S16BIT _dvxIODriverResourceRelease
(
    S16BIT s16DevNum
);

extern S16BIT _DECL _dvxSetIRQCond
(
    S16BIT s16DevNum,
    U16BIT u16Enable,
    U32BIT dwIrqMask
);

extern S16BIT _dvxBcReplayThreadCreate
(
    S16BIT s16DevNum
);

extern VOID _dvxBcReplayIrqRelease
(
    S16BIT s16DevNum
);

void *  _dvxBcReplayDispatcher
(
    void *pwDevNum
);


#endif /* __DEVICEX_H__ */
