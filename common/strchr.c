/**MOD+************************************************************************/
/* Module:  strchr.c                                                          */
/*                                                                            */
/* Purpose: strchr implementation for MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

char *strchr(register const char *s, int c)
{
    register char ch = c;

    for (;;) {
        if (ch == *s) break;
        if (!*s) return (char *)0;
        ++s;
    }

    return (char *)s;
}

/******************************************************************************/
