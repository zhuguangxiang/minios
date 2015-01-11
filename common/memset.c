/**MOD+************************************************************************/
/* Module:  memset.c                                                          */
/*                                                                            */
/* Purpose: memset implementation for MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

void *memset(void *dst, int s, int count)
{
    char *d = dst;

    while (count--) *d++ = s;

    return dst;
}

/******************************************************************************/
