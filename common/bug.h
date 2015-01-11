/**INC+************************************************************************/
/* Header:  bug.h                                                             */
/*                                                                            */
/* Purpose: BUG_ON for MiniOS                                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_BUG_H_
#define _MINIOS_BUG_H_

#include "common/stdio.h"
#include "common/panic.h"

/******************************************************************************/
/* Macro to disable interrupts and halt system                                */
/******************************************************************************/
#define BUG_ON(condition) do {                  \
    if (condition) {                            \
        printf("BUG: failure at %s:%d:%s()!\n", \
            __FILE__, __LINE__, __FUNCTION__);  \
        panic("BUG!");                          \
    }                                           \
} while (0)

#endif /* _MINIOS_BUG_H_ */

/******************************************************************************/
