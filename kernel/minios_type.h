/*----------------------------------------------------------------------------*/
/*                                  MINIOS                                    */
/*                        The Embedded Operating System                       */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China         */
/*                           All Rights Reserved                              */
/*----------------------------------------------------------------------------*/

#ifndef _MINIOS_TYPES_H_
#define _MINIOS_TYPES_H_

/*----------------------------------------------------------------------------*/
/* Common Types                                                               */
/*----------------------------------------------------------------------------*/
typedef unsigned char    BYTE;       /* 8 bits                                */
typedef          char    CHAR;       /*                                       */
typedef unsigned short   USHORT;     /* 2 byte unsigned integer               */
typedef          short   SHORT;      /* 2 byte signed integer                 */
typedef unsigned long    ULONG;      /* 4 byte unsigned integer               */
typedef          long    LONG;       /* 4 byte signed integer                 */
typedef unsigned int     UINT;       /* >= 2 byte unsigned integer            */
                                     /* (machine word)                        */
typedef          int     INT;        /* >= 2 byte signed integer              */
                                     /* (machine word)                        */
typedef          int     BOOL;       /* boolean                               */
typedef          void    VOID;       /* void if support, char otherwise       */

#define CONST  const
#define INLINE inline
#define STATIC static

/*----------------------------------------------------------------------------*/
/* Boolean Define                                                             */
/*----------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/*----------------------------------------------------------------------------*/
/* NULL Define                                                                */
/*----------------------------------------------------------------------------*/
#define NULL ((void *)0)

/*----------------------------------------------------------------------------*/
/* Minios Types                                                               */
/*----------------------------------------------------------------------------*/
typedef ULONG    CPU_FLAGS;
typedef INT      STATUS;
typedef ULONG    HANDLE;
typedef ULONG    ADDRESS;


#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef __compiler_offsetof
#define OFFSETOF(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define PARENT_STRUCT(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - OFFSETOF(type,member) );})

#if !defined(USER_SECTION)
#define USER_SECTION(__sect__) __attribute__ ((unused,section (__sect__)))
#endif

#define ALIGN_DOWN(val, size) ((val) & (~((size)-1)))
#define ALIGN_UP(val, size)   (((val)+(size)-1) & ~((size)-1))

#define INVALID_PTR(ptr) ((ptr) == NULL)

#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))
#define BITS_TO_UINT8(nr)   DIV_ROUND_UP(nr, 8)
#define BITS_TO_UINT32(nr)  DIV_ROUND_UP(nr, 32)

#define BUG_ON(condition)  do { if (condition) while (1); } while (0)

#endif // _MINIOS_TYPE_H_

/*--------------------------------------------------------------------------*/
// EOF minios_type.h
