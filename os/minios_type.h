/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _MINIOS_TYPES_H_
#define _MINIOS_TYPES_H_

typedef signed   char  int8_t;
typedef unsigned char  uint8_t;
typedef signed   short int6_t;
typedef unsigned short uint6_t;
typedef signed   int   int32_t;
typedef unsigned int   uint32_t;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

#define NULL ((void *)0)

typedef uint32_t size_t;
typedef uint32_t cpu_flags_t;
typedef int32_t  bool_t;
typedef int32_t  status_t;

typedef void  *handle_t;

typedef unsigned long address_t;

#define PTR_TO_INT(ptr) ((address_t)ptr)

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
// EOF minios_type.h
