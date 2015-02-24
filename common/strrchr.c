/**MOD+************************************************************************/
/* Module:  strrchr.c                                                         */
/*                                                                            */
/* Purpose: strrchr implementation for MiniOS                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

char *strrchr(register const char *s, int c)
{
    register char ch = c;
    register const char *l = 0;

    for (;;) {
        if (ch == *s) l = s;
        if (!*s) return (char *)l;
        ++s;
    }

    return (char *)l;
}

/******************************************************************************/
