/**INC+************************************************************************/
/* Header:  types.h                                                           */
/*                                                                            */
/* Purpose: Common data types for the MiniOS                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_TYPES_H_
#define _MINIOS_TYPES_H_

/******************************************************************************/
/* Common Types                                                               */
/******************************************************************************/
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

/******************************************************************************/
/* Boolean Define                                                             */
/******************************************************************************/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/******************************************************************************/
/* NULL Define                                                                */
/******************************************************************************/
#define NULL ((void *)0)

/******************************************************************************/
/* Kernel Types                                                               */
/******************************************************************************/
typedef ULONG    CPU_FLAGS;
typedef INT      STATUS;
typedef ULONG    HANDLE;
typedef ULONG    ADDRESS;

/******************************************************************************/
/* MIN()/MAX() Macros                                                         */
/******************************************************************************/
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/******************************************************************************/
/* Cast a member of a structure out to the containing structure               */
/******************************************************************************/
#undef OFFSETOF
#ifdef __compiler_offsetof
#define OFFSETOF(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define OFFSETOF(TYPE, MEMBER) ((UINT) &((TYPE *)0)->MEMBER)
#endif

#define CONTAINER_OF(ptr, type, member) ({ \
    CONST typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((CHAR *)__mptr - OFFSETOF(type, member));})

/******************************************************************************/
/* Section used for linked script                                             */
/******************************************************************************/
#if !defined(USER_SECTION)
#define USER_SECTION(__sect__) __attribute__ ((used, section (__sect__)))
#endif

/******************************************************************************/
/* init module table for linked script                                        */
/******************************************************************************/
typedef VOID (*INIT_CALL)(VOID);
#define MOD_LVL(lvl) "_"#lvl
#define MOD_INIT_CALL(fn, lvl)  \
    STATIC INIT_CALL __##fn##__ \
    USER_SECTION(".mod_init_lvl" MOD_LVL(lvl) ".init") = fn

/******************************************************************************/
/* Alignment Macros                                                           */
/******************************************************************************/
#define ALIGN_DOWN(val, size) ((val) & (~((size)-1)))
#define ALIGN_UP(val, size)   (((val)+(size)-1) & ~((size)-1))

/******************************************************************************/
/* ROUND/BITS_TO_BYTES Macros                                                 */
/******************************************************************************/
#define DIV_ROUND_UP(n,d)    (((n) + (d) - 1) / (d))
#define BITS_TO_BYTES(nr)    DIV_ROUND_UP(nr, 8)
#define BITS_TO_ULONG(nr)    DIV_ROUND_UP(nr, 32)

#endif /* _MINIOS_TYPES_H_ */

/******************************************************************************/
// EOF types.h
