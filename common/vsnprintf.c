/**MOD+************************************************************************/
/* Module:  vsnprintf.c                                                       */
/*                                                                            */
/* Purpose: variable format string functions                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/stdarg.h"
#include "common/string.h"
#include "common/ctype.h"

#define NULL ((void *)0)

unsigned int strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned int result = 0, value;

    if (*cp == '0') {
        cp++;
        if ((*cp == 'x') && isxdigit(cp[1])) {
            base = 16;
            cp++;
        }
        if (!base) {
            base = 8;
        }
    }

    if (!base) {
        base = 10;
    }

    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp - '0'
            : (islower(*cp) ? toupper(*cp) : *cp) - 'A' + 10) < base) {
        result = result * base + value;
        cp++;
    }

    if (endp)
        *endp = (char *) cp;
    return result;
}

int strtol(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -strtoul(cp + 1, endp, base);
    return strtoul(cp, endp, base);
}

/* convert string to integer */
static int skip_atoi(const char **s)
{
    register int i = 0;
    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

/* Find the length of a length-limited string */
static int strnlen(const char * s, int count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

/* format string flags bits */
#define ZEROPAD 1   /* pad with zero                    */
#define SIGN    2   /* unsigned/signed long             */
#define PLUS    4   /* show plus                        */
#define SPACE   8   /* space if plus                    */
#define LEFT    16  /* left justified                   */
#define SPECIAL 32  /* 0x/0X/0                          */
#define LARGE   64  /* use 'ABCDEF' instead of 'abcdef' */

/* divsion, gcc extension grammar ({}) */
#define do_div(n,base)                              \
({                                                  \
int __res;                                          \
__res = ((unsigned long)n) % (unsigned int)base;    \
n = ((unsigned long)n) / (unsigned int)base;        \
__res;                                              \
})

/* Works only for digits and letters, but small and fast */
#define TOLOWER(x) ((x) | 0x20)

static unsigned int simple_guess_base(const char *cp)
{
    if (cp[0] == '0') {
        if (TOLOWER(cp[1]) == 'x' && isxdigit(cp[2]))
            return 16;
        else
            return 8;
    } else {
        return 10;
    }
}

/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned long result = 0;

    if (!base)
        base = simple_guess_base(cp);

    if (base == 16 && cp[0] == '0' && TOLOWER(cp[1]) == 'x')
    cp += 2;

    while (isxdigit(*cp)) {
        unsigned int value;

        value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10;
        if (value >= base)
            break;
        result = result * base + value;
        cp++;
    }

    if (endp)
        *endp = (char *)cp;
    return result;
}

/****************************************************************************
 *  %[flags][width][.precision][length]specifier
 *  Flags include LEFT-JUST(-), SHOWN SIGN(+), SPECIAL(#), PADDING(0/space)
 *  Width is the count of total characters.
 *  Precision = count of characters + zeroes padding.
 *  If Precision > count of characters, output = zeroes padding + chars
 *  Width - Precision uses PADDING in Flags to pad.
 *  LEFT/RIGHT PADDING is controlled by Flag(-).
 ****************************************************************************/

/*  number output format: [spaces1][sign][special][zeroes]number[spaces2]
 *  ("%32.16d", "12345") ===>>> 16 spaces + 11 zeroes + 12345
 *  ("%-32.16d", "12345") ===>>> 11 zeroes + 12345 + 16 spaces
 *  ("%32.3d", "12345") ===>>> 27 spaces + 0 zeroes + 12345
 *  ("%-32.3d", "12345") ===>>> 0 zeroes + 12345 + 27 spaces
 *  spaces1 or spaces2 = width - precision.
 *  spaces1 and spaces2 can output only one, corresponding LEFT flag.
 *  if precision > count of number, using zeroes as precision to output before
 *  number to output.
 *  if precision < count of number, precision count is 0.
 *  LEFT align no spaces to output before precision and number.
 */
static char *
__number(char *start, char *end, int base, int flags, int width, int precision,
         long long num)
{
    const char *digits;
    static const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char c, sign, tmp[66] = {0};
    int i;

    /* uppercase ? */
    digits = (flags & LARGE) ? large_digits : small_digits;

    /* base valid range check */
    if (base < 2 || base > 36)
        return NULL;

    /* left-adjust, space padding and not zero padding */
    if (flags & LEFT)
        flags &= ~ZEROPAD;

    /* padding character */
    c = (flags & ZEROPAD) ? '0' : ' ';

    /* preceding character +/-/(space) */
    sign = 0;
    if (flags & SIGN) {
        if (num < 0) {
            sign = '-'; num = -num; width--;
        } else if (flags & PLUS) {
            sign = '+'; width--;
        } else if (flags & SPACE) {
            sign = ' '; width--;
        }
    }

    /* process SPECIAL 0x/0X/0 */
    if (flags & SPECIAL) {
        if (base == 16)
            width -= 2;
        else if (base == 8)
            width -= 1;
    }

    /* process num */
    i = 0;
    if (num == 0) {
        tmp[i++] = '0';
    } else {    /* save number characters reversely into tmp array */
        while (num != 0)
            tmp[i++] = digits[do_div(num, base)];
    }

    /* precision adjust, if precision is less equal than string length */
    if (i > precision)
        precision = i;

    /* adjust width */
    width -= precision;

    /* print preceding padding characters */
    if (!(flags & (ZEROPAD + LEFT))) {
        /* using space to padding, right-adjust */
        while (width-- > 0) {
            if (start < end)
                *start = ' ';
            ++start;
        }
    }

    /* print preceding sign */
    if (sign) {
        if (start < end)
            *start = sign;
        ++start;
    }

    /* print preceding special characters (0x/0X/0) */
    if (flags & SPECIAL) {
        if (base == 16) {
            if (start < end)
                *start = '0';
            ++start;
            if (start < end)
                *start = digits[33]; /* digits[33] = x/X */
            ++start;
        } else if (base == 8) {
            if (start < end)
                *start = '0';
            ++start;
        }
    }

    /* print leftside padding using spaces or zeroes, no LEFT flag */
    if (!(flags & LEFT)) {
        while (width-- > 0) {
            if (start < end)
                *start = c;
            ++start;
        }
    }

    /* print precision */
    while (i < precision--) {
        if (start < end)
            *start = '0';
        ++start;
    }

    /* print number characters */
    while (i-- > 0) {
        if (start < end)
            *start = tmp[i];
        ++start;
    }

    /* print rightside padding using spaces */
    while (width-- > 0) {
        if (start < end)
            *start = ' ';
        ++start;
    }

    return start;
}

/*  handle specifier "%c", [space1]char[space2], only flags(-), width valid.
 *  ("%32c", 'a') => 31 spaces + 'a'
 *  ("%-32c", 'a') => 'a' + 31 spaces
 *  precision is not a valid option.
 */
static char *
__vsnprintf_c(char *start, char *end, int flags, int width, char c)
{
    /* no minus sign(-) in format string, default right-adjust */
    if (!(flags & LEFT)) {
        /* no minus and width > 0, "%8c" */
        while (--width > 0) {
            if (start < end)
                *start = ' '; /* pad blank space */
            ++start;
        }
    }

    if (start < end)
        *start = c;
    ++start;

    while (--width > 0) {   /* "%-8c" */
        if (start < end)
            *start = ' '; /* pad blank space */
        ++start;
    }

    return start;
}

/*  handle specifier "%s"
 *  ("%32.16sxx", "abcdef")     =>  26spaces + abcdef
 *  ("%-32.16sxx", "abcdef")    =>  abcdef + 26spaces
 *  ("%32.3sxx", "abcdef")      =>  29spaces + abc
 *  ("%-32.3sxx", "abcdef")     =>  abc + 29spaces
 */
static char *
__vsnprintf_s(char *start, char *end, int flags, int width, int precision,
              const char *s)
{
    int i, len = strnlen(s, precision);

    /* no minus sign(-) in format string, default right-adjust */
    if (!(flags & LEFT)) {
        /* no minus and width > 0, "%8s" */
        while (len < width--) {
            if (start < end)
                *start = ' '; /* pad blank space */
            ++start;
        }
    }

    for (i = 0; i < len; ++i) {
        if (start < end)
            *start = *s;
        ++start; ++s;
    }

    while (len < width--) { /* "%-8s" */
        if (start < end)
            *start = ' '; /* pad blank space */
        ++start;
    }

    return start;
}

/*  handle specifier "%p":
 */
static char *
__vsnprintf_p(char *start, char *end, int flags, int width, void *p)
{
    if (width == -1) {
        width = 2 * sizeof(void *) + 2;
        flags |= (ZEROPAD + SPECIAL);   /* print 0x */
    }

    return __number(start, end, 16, flags, width, 0, (long)p);
}

/* handle specifier "%m", output ipv4, ignore all flags, width and precision */
static char *
__vsnprintf_ipv4(char *start, char *end, unsigned long m)
{
    return 0;
}

/**PROC+*********************************************************************/
/*                                                                          */
/* Name:     vsnprintf                                                      */
/*                                                                          */
/* Purpose:  format a string and place it in a buffer.                      */
/*           %[flags][width][.precision][length]specifier                   */
/*           http://www.cplusplus.com/reference/cstdio/printf/              */
/*                                                                          */
/* Returns:  The count of characters in the output buffer                   */
/*                                                                          */
/* Params:   IN/OUT  buf   - The buffer to place the result into            */
/*           IN      size  - The size of the buffer                         */
/*           IN      fmt   - The format string to use                       */
/*           IN      args  - Arguments for the format string                */
/*                                                                          */
/**PROC-*********************************************************************/

int vsnprintf(char *buf, int size, const char *fmt, va_list args)
{
    char *start;
    char *end;

    int flags, width, precision, qualifier;

    char c;
    const char *s;
    void *p;
    unsigned long m;

    int base;
    unsigned long long num;

    start = buf;
    end = buf + size;

    for (; *fmt; ++fmt) {

        /* skip regular characters until at % */
        if (*fmt != '%') {
            if (start < end)
                *start++ = *fmt;
            continue;
        }

        /*
         * +-------+--------------------------------------------------------+
         * | flags |                   description                          |
         * +-------+--------------------------------------------------------+
         * |   -   |Left-justify within the given field with;               |
         * |       |Righ justificaiton is default.                          |
         * |       |"%-5scd" ==>> "ab   cd"  "%5scd" ==>> "   abcd"         |
         * +-------+--------------------------------------------------------+
         * |   +   |Forces to preceed the result with a plus or minus sign  |
         * |       |(+ or -) even for positive numbers. By default, only    |
         * |       |negative numbers are preceeded with a - sign.           |
         * |       |"%+d" ==>> +123 VS "%d" ==>> -123                       |
         * +-------+--------------------------------------------------------+
         * | space |If no sign is to be written, a blank is inserted before |
         * |       |the value.                                              |
         * |       |"% 8d" ==>> "     123"  VS "%08d" ==>> "00000123"       |
         * +-------+--------------------------------------------------------+
         * |   #   |Used with o,x or X specifiers, the value is processed   |
         * |       |with 0,0x or 0X respectively for values differernt than |
         * |       |zero.                                                   |
         * |       |!!FLOAT NOT IMPLEMENTED!!                               |
         * |       |"%#x" ==>> 0xdeafbeaf  VS "%x" ==>> deadbeaf            |
         * +-------+--------------------------------------------------------+
         * |   0   |Left-pads the number with zeroes(0) instead of spaces.  |
         * |       |"% 8d" ==>> "     123"  VS "%08d" ==>> "00000123"       |
         * +-------+--------------------------------------------------------+
         *
         */

        /* process flags */
        flags = 0;
    FLAGES_REPEAT:
        ++fmt; /* this also skips first % */
        switch (*fmt) {
            case '-': flags |= LEFT;    goto FLAGES_REPEAT;
            case '+': flags |= PLUS;    goto FLAGES_REPEAT;
            case ' ': flags |= SPACE;   goto FLAGES_REPEAT;
            case '#': flags |= SPECIAL; goto FLAGES_REPEAT;
            case '0': flags |= ZEROPAD; goto FLAGES_REPEAT;
        }

        /*
         * +--------+-------------------------------------------------------+
         * | width  |                   description                         |
         * +--------+-------------------------------------------------------+
         * | number |Minimum number of characters to be printed.            |
         * |        |If the value to be printed is shorter than this number,|
         * |        |the result is padded with blank spaces. The value is   |
         * |        |not truncated even if this result is larger.           |
         * +--------+-------------------------------------------------------+
         * |   *    |The width is not specified in the format string,       |
         * |        |but as an additional integer value argument preceding  |
         * |        |the argument that has to be formatted.                 |
         * +--------+-------------------------------------------------------+
         *
         */

        /* get field width */
        width = -1;
        if (isdigit(*fmt))
            width = skip_atoi(&fmt);
        else if (*fmt == '*') {  /* used seldomly */
            ++fmt;
            /* get field width from args list */
            width = va_arg(args, int);
            /* ANSI: negative field with means '-' flag and positive width */
            if (width < 0) {
                width = -width;
                flags |= LEFT;
            }
        }

        /*
         * +------------+---------------------------------------------------+
         * | .precision |                   description                     |
         * +------------+---------------------------------------------------+
         * | .number |For integer specifiers(d,i,o,u,x,X), precision        |
         * |         |specifies the minimum number of digits to be written. |
         * |         |If the value to be written is shorter than the number,|
         * |         |the result is padded with leading zeroes.The value is |
         * |         |not truncated even if the result is larger.           |
         * |         |A precision of 0 means no characters is written for   |
         * |         |the value 0. If the value to be printed is shorter    |
         * |         |than this number, the result is padded with blank     |
         * |         |spaces. The value is not truncated even if this result|
         * |         |is larger.                                            |
         * |         |For s: this is the maximum number of characters to be |
         * |         |printed. By default all characters are printed until  |
         * |         |the ending NULL character is encountered.             |
         * |         |If the period is specified without an explicit value  |
         * |         |for precision, 0 is assumed.                          |
         * |         | !!FLOAT NOT IMPLEMENTED!!                            |
         * +---------+------------------------------------------------------+
         * |  .*     |The precision is not specified in the format string,  |
         * |         |but as an additional integer value argument preceding |
         * |         |the argument that has to be formatted.                |
         * +---------+------------------------------------------------------+
         *
         */

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {  /* used seldomly */
                ++fmt;
                /* get precision from args list */
                precision = va_arg(args, int);
            }

            if (precision < 0)
                precision = 0;
        }

        /*
         * The length sub-specifier modifies the length of the data type.
         * +--------+-------------------------------------------------------+
         * |        |                   sub-specifier                       |
         * +--------+-----------+---------------------+-----+-------+-------+
         * | length |      d i  |  u o x X            |  c  |   s   |  p    |
         * +--------+-----------+---------------------+-----+-------+-------+
         * | (none) | int       | unsigned int        | int | char* | void* |
         * +--------+-----------+---------------------+-----+-------+-------+
         * |   h    | short int | unsigned short int  |     |       |       |
         * +--------+-----------+---------------------+-----+-------+-------+
         * |   l    | long int  | unsigned long int   |     |       |       |
         * +--------+-----------+---------------------+-----+-------+-------+
         */

        /* get the conversion qualifier */
        qualifier = -1;
        if ((*fmt == 'h') || (*fmt == 'l')) {
            qualifier = *fmt;
            ++fmt;
            if ((qualifier == 'l') && (*fmt == 'l')) {
                qualifier = 'L';
                ++fmt;
            }
        }

        /* get specifier */

        base = 10;          /* default base */

        switch (*fmt) {
            case 'c':
                c = (unsigned char)va_arg(args, int);
                start = __vsnprintf_c(start, end, flags, width, c);
                continue;
            case 's':
                s = va_arg(args, char *);
                if (!s) s = "(NULL)";
                start = __vsnprintf_s(start, end, flags, width, precision, s);
                continue;
            case 'p':
                p = va_arg(args, void *);
                start = __vsnprintf_p(start, end, flags, width, p);
                continue;
            case 'm':
                /* Extension: IPV4 address */
                m = va_arg(args, unsigned long);
                start = __vsnprintf_ipv4(start, end, m);
                continue;
            case '%':
                if (start < end)
                    *start++ = '%';
                continue;

            /* integer number formats */

            case 'd':
            case 'i':
                flags |= SIGN;
                /* no break */
            case 'u':
                break;

            case 'X':
                flags |= LARGE;
                /* no break */
            case 'x':
                base = 16;
                break;

            case 'o':
                base = 8;
                break;

            /* default branch, output directly */
            default:
                continue;
        } /* END OF switch (*fmt) */

        /* integer conversion */
        if (qualifier == 'L')
            num = va_arg(args, long long);
        else if (qualifier == 'l') {
            num = va_arg(args, unsigned long);
            if (flags & SIGN)
                num = (signed long)num;
        } else if (qualifier == 'h') {
            num = (unsigned short)va_arg(args, int);
            if (flags & SIGN)
                num = (signed short)num;
        } else {
            num = va_arg(args, unsigned int);
            if (flags & SIGN)
                num = (signed int)num;
        }

        start = __number(start, end, base, flags, width, precision, num);
    } /* END OF for (; *fmt; ++fmt) */

    if (start < end)
        *start = '\0';
    else if (size > 0)
        /* don't write out a NULL byte if the buf size is zero */
        *(end-1) = '\0';
    /*
     * the trailing NULL byte doesn't count towards the total
     * ++start;
     */
    return start - buf;
}

/**PROC+*********************************************************************/
/*                                                                          */
/* Name:     snprintf                                                       */
/*                                                                          */
/* Purpose:  format a string and place it in a buffer.                      */
/*                                                                          */
/* Returns:  The count of characters in the output buffer                   */
/*                                                                          */
/* Params:   IN/OUT  buf   - The buffer to place the result into            */
/*           IN      size  - The size of the buffer                         */
/*           IN      fmt   - The format string to use                       */
/*           IN      ...   - Arguments for the format string                */
/*                                                                          */
/**PROC-*********************************************************************/

INT snprintf(CHAR *buf, INT size, CONST CHAR *fmt, ...)
{
    va_list args;
    int count;

    va_start(args, fmt);
    count = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return count;
}

/******************************************************************************/
// EOF vsnprintf.c
