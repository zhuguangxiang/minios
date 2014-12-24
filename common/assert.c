/**MOD+************************************************************************/
/* Module:  assert.c                                                          */
/*                                                                            */
/* Purpose:  Unexpected things happen, __assert_fail                          */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/stdio.h"
#include "common/panic.h"

void __assert_fail(const char *exp, const char *file, int line)
{
    printf("Assertion failed: \'%s\', file:%s, line:%d.\n\r", exp, file, line);
    panic(0);
}

/******************************************************************************/
// EOF assert.c
