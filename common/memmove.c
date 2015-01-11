/**MOD+************************************************************************/
/* Module:  memmove.c                                                         */
/*                                                                            */
/* Purpose: memmove implementation for MiniOS                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

void *memmove(void *dst, const void *src, int count)
{
    char *a = dst;
    const char *b = src;

    if (src != dst) {
        if (src > dst) {
            while (count--) *a++ = *b++;
        } else {
            a+=count-1;
            b+=count-1;
            while (count--) *a-- = *b--;
        }
    }

    return dst;
}

/******************************************************************************/
