/**INC+************************************************************************/
/* Header:  string.h                                                          */
/*                                                                            */
/* Purpose: string functions for MiniOS                                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_STRING_H_
#define _MINIOS_STRING_H_

void *memcpy(void *dst, void *src, int size);
void *memset(void *dst, int s, int count);
int memcmp(const void *dst, const void *src, int count);
void *memmove(void *dst, const void *src, int count);

char *strcpy(char *s1, const char *s2);
int strlen(const char *s);
int strcmp(const char *s1, const char *s2);

char *itoa(int value, char *s, int base);
unsigned int strtoul(const char *cp, char **endp, unsigned int base);
int strtol(const char *cp, char **endp, unsigned int base);

#endif /* _MINIOS_STRING_H_ */

/******************************************************************************/
// EOF string.h
