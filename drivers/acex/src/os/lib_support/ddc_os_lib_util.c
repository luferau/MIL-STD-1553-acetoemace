/*******************************************************************************
 * FILE: ddc_os_lib_util.c
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

#include <stdio.h>
#include <conio.h>
#include "ddc_os_lib_util.h"

/*******************************************************************************
 * Name:    ddcPressAKey
 *
 * Description:   
 *      Wait for key press with given prompt information. 
 *        
 * In   prompt    prompt information to print to screen
 * Out            none
 ******************************************************************************/
void ddcPressAKey
(
    char *prompt
)
{
    /* flush input buffer */
    while (kbhit())
    {
        getchar();
    }

    if (prompt)
    {
        printf("\n%s\n", prompt);
    }

    /* flush the output buffer */
    fflush(stdout);

    while (!kbhit())
    {
        /* wait for keypress... */
        Sleep(100);
    }

    /* flush input buffer */
    while (kbhit())
    {
        getchar();
    }
}
