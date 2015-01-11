/**INC+************************************************************************/
/* Header:  stdio.h                                                           */
/*                                                                            */
/* Purpose: standard IOs implemention of MiniOS                               */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_STDIO_H_
#define _MINIOS_STDIO_H_

#include "common/types.h"

/* hal needs implemented functions */
VOID puts(CONST CHAR *str);
VOID putc(CHAR ch);
CHAR getc(VOID);
VOID gets(CHAR *buf, INT size);

VOID printf(CONST CHAR *fmt, ...);
INT snprintf(CHAR *buf, INT size, CONST CHAR *fmt, ...);

#endif /* _MINIOS_STDIO_H_ */

/******************************************************************************/
