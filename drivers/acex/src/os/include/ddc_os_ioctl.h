/*******************************************************************************
 * FILE: ddc_os_ioctl.h
 *
 * DESCRIPTION:
 *
 *  TODO
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
 * Copyright (c) 2014 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef _DDC_OS_IOCTL_H_
#define _DDC_OS_IOCTL_H_


/* ========================================================================== */
/* IOCTL Definitions                                                          */
/* ========================================================================== */
#define IOCTL_INDEX                     0x0800
#define IOCTL_DEVICE_TYPE               0x8000
#define IOCTL_ACEX_TYPE                 0x4000

#define DDC_IOCTL_OFFSET	0

#define DDC_IOCTL__GROUP_MASK      0xFFFF0000  /* bits 15 - 7 */
#define DDC_IOCTL__GROUP_OFFSET    16
#define DDC_IOCTL__GROUP_SET_VAL(x) ((x << DDC_IOCTL__GROUP_OFFSET) & DDC_IOCTL__GROUP_MASK)

#define DDC_IOCTL__TYPE_MASK       0x0000FFFF   /* bits 6 - 0 */
#define DDC_IOCTL__TYPE_OFFSET     2    
#define DDC_IOCTL__TYPE_SET_VAL(x)   ((x << DDC_IOCTL__TYPE_OFFSET) & DDC_IOCTL__TYPE_MASK)

#define DDC_IOCTL(ioctl_group, ioctl_type, ioctl_method) \
   ((DDC_IOCTL__GROUP_SET_VAL((IOCTL_DEVICE_TYPE + IOCTL_ACEX_TYPE + ioctl_group)) + DDC_IOCTL__TYPE_SET_VAL((IOCTL_INDEX + ioctl_type))) + ioctl_method)

/*#define DDC_IOCTL(ioctl_group, ioctl_type, ioctl_method) \
   ((DDC_IOCTL__GROUP_SET_VAL((IOCTL_DEVICE_TYPE + ioctl_group)) + DDC_IOCTL__TYPE_SET_VAL((IOCTL_INDEX + ioctl_type))) + ioctl_method)
*/

/* used for driver debugging */
#define DDC_IOCTL__GROUP_GET_VAL(ioctlVal) ((ioctlVal & DDC_IOCTL__GROUP_MASK) >> DDC_IOCTL__GROUP_OFFSET)
#define DDC_IOCTL__TYPE_GET_VAL(ioctlVal) ((ioctlVal & DDC_IOCTL__TYPE_MASK) >> DDC_IOCTL__TYPE_OFFSET)

#endif /* _DDC_OS_IOCTL_H_ */
