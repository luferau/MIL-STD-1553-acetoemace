/*******************************************************************************
 * FILE: ddc_os_lib_util.h
 *
 * DESCRIPTION:
 *
 *  These routines are Linux substitutes for functions that are not provided
 *  natively.
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

#ifndef _DDC_OS_LIB_UTIL_H_
#define _DDC_OS_LIB_UTIL_H_

#include <time.h>

#include "os/include/ddc_os_types.h"


#define DDC_MSLEEP(u32Milliseconds) \
{ \
    Sleep(u32Milliseconds); \
}

#define DDC_USLEEP(u32Microseconds) \
{ \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = (u32Microseconds * 1000); /* get remainder */ \
    while(nanosleep(&ts,&ts) == -1) \
    { \
        continue; \
    } \
}

void ddcPressAKey
(
    char *prompt
);

#endif /* _DDC_OS_LIB_UTIL_H_ */
