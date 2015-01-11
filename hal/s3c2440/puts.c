/**MOD+************************************************************************/
/* Module:  puts.c                                                            */
/*                                                                            */
/* Purpose: s3c2440 puts implementation                                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/stdio.h"
#include "common/error.h"
#include "common/string.h"

/******************************************************************************/
/* Default standard character output device handle                            */
/******************************************************************************/
STATIC HANDLE tty_handle;

/**PROC+***********************************************************************/
/* Name:     puts                                                             */
/*                                                                            */
/* Purpose:  Output string to standard outout device                          */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN str  - string to be printed                                   */
/*                                                                            */
/**PROC-***********************************************************************/
VOID __puts(CONST CHAR *str)
{
    UINT count;
    STATUS res;

    if (0 == tty_handle) {
        //res = device_lookup("/dev/tty0", &tty_handle);
        if (ENOERR != res) {
            tty_handle = 0;
            return;
        }
    }

    count = strlen(str);

    //device_write(tty_handle, str, &count);
}

/******************************************************************************/
