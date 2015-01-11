/**MOD+************************************************************************/
/* Module:  memcpy.c                                                          */
/*                                                                            */
/* Purpose: memcpy implementation for MiniOS (maybe slow)                     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

void *memcpy(void *dst, void *src, int size)
{
    char *d = (char *)dst;
    char *s = (char *)src;

    while (size--) *d++ = *s++;

    return dst;
}

/******************************************************************************/
