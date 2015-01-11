/**MOD+************************************************************************/
/* Module:  memcmp.c                                                          */
/*                                                                            */
/* Purpose: memcmp implementation for MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

int memcmp(const void *dst, const void *src, int count)
{
    int res;
    const char *d = dst;
    const char *s = src;

    while (count-- > 0)
    {
        if ((res = (*d - *s)))
        {
            return res;
        }

        ++d; ++s;
    }

    return 0;
}

/******************************************************************************/
