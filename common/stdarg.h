/**INC+************************************************************************/
/* Header:  stdarg.h                                                          */
/*                                                                            */
/* Purpose: va_list implementation for MiniOS                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_STDARG_H_
#define _MINIOS_STDARG_H_

#include "common/types.h"

#if ((__GNUC__ > 2) || (__GNUC__ == 2) && (__GNUC_MINOR__ >= 96))

/******************************************************************************/
/* using gcc builtin implementation.                                          */
/******************************************************************************/
typedef __builtin_va_list va_list;

#define va_start(v, l)  __builtin_va_start((v), (l))
#define va_end          __builtin_va_end
#define va_arg          __builtin_va_arg

#else

/******************************************************************************/
/* using user defined                                                         */
/******************************************************************************/
typedef CHAR *va_list;

/******************************************************************************/
/* __va_size(n) is aligned with pointer size                                  */
/******************************************************************************/
#define __va_size(n) \
    ((sizeof(n) + sizeof(void *) - 1) & ~(sizeof(void *) - 1))

#define va_start(v, l)  (v = (va_list)&l + __va_size(l))
#define va_end(v)       (v = (va_list)0)
#define va_arg(v, t)    (*(t *)((v += __va_size(t)) - __va_size(t)))

#endif /* __GUNC__ */


#endif /* _MINIOS_STDARG_H_ */

/******************************************************************************/
// EOF stdarg.h
