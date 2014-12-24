/**MOD+************************************************************************/
/* Module:  printf.c                                                          */
/*                                                                            */
/* Purpose: standard printf implementation                                    */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/stdio.h"
#include "common/stdarg.h"
#include "config/config.h"

int vsnprintf(char *buf, int size, const char *fmt, va_list args);

VOID printf(CONST CHAR *fmt, ...)
{
    int count;
    char buf[PRINT_BUF_SIZE];
    va_list args;

    va_start(args, fmt);
    count = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    buf[count] = 0;
    puts(buf);
}

/******************************************************************************/
// EOF stdio.c
