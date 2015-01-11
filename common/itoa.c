/**MOD+************************************************************************/
/* Module:  itoa.c                                                            */
/*                                                                            */
/* Purpose: integer to string                                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

char *itoa(int val, char *str, int base)
{
    static char map[] = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z',
    };

    int count = -1;
    int i, tmp;
    unsigned int value;

    if ((base < 2) || (base > 36))
        return str;

    if ((val < 0) && (base == 10)) {
        *str++ = '-';
        val = -val;
    }

    value = *(unsigned int *)&val;

    do {
        str[++count] = map[value % base];
        value /= base;
    } while (value > 0);

    i = (count + 1)/2;
    while (i-- > 0) {
        tmp = str[i];
        str[i] = str[count - i];
        str[count - i] = tmp;
    }

    str[count+1] = 0;

    return str;
}

/******************************************************************************/
