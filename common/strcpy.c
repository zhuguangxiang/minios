/**MOD+************************************************************************/
/* Module:  strcpy.c                                                          */
/*                                                                            */
/* Purpose: strcpy implementation for MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

char *strcpy(char *s1, const char *s2)
{
    char *res = s1;

    while ((*s1++ = *s2++));

    return res;
}

/******************************************************************************/
// EOF strcpy.c
