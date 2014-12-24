/**INC+************************************************************************/
/* Header:  crc.h                                                             */
/*                                                                            */
/* Purpose: crc functions                                                     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_CRC_H_
#define _MINIOS_CRC_H_

/* 16 bit CRC with polynomial x^16+x^12+x^5+1 */
unsigned short crc16(unsigned char *s, int len);

#endif /* _MINIOS_CRC_H_ */

/******************************************************************************/
// EOF crc.h
