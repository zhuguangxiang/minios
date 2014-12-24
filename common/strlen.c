/**MOD+************************************************************************/
/* Module:  strlen.c                                                          */
/*                                                                            */
/* Purpose: strlen implementation for MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

int strlen(const char *s)
{
    int i = 0;

    if (!s) return 0;

    while (*s++) ++i;

    return i;
}

/******************************************************************************/
// EOF strlen.c
