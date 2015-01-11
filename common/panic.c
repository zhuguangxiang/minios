/**MOD+************************************************************************/
/* Module:  panic.c                                                           */
/*                                                                            */
/* Purpose: Errors happen, panic()                                            */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/stdio.h"
#include "hal/port.h"

void panic(const char *msg)
{
    if (msg) puts(msg);
    HAL_DISABLE_INTERRUPTS();
    while (1);
}

/******************************************************************************/
