/**INC+************************************************************************/
/* Header:  assert.h                                                          */
/*                                                                            */
/* Purpose: Unexpected things happen, assert()                                */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_ASSERT_H_
#define _MINIOS_ASSERT_H_

#ifdef NDEBUG
#define assert(e)   ((void)0)
#else
static inline void __assert_fail(const char *expr, const char *file, int line)
{
    printf("Assertion failed: \'%s\', file:%s, line:%d.\n\r", exp, file, line);
    panic(0);
}
#define assert(exp) \
  ((void)((exp) || (__assert_fail(#exp, __FILE__, __LINE__),0)))
#endif

#endif /* _MINIOS_ASSERT_H_ */

/******************************************************************************/
