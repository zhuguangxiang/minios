/**INC+************************************************************************/
/* Header:  s3c2440_io.h                                                      */
/*                                                                            */
/* Purpose: s3c2440 registers read/write                                      */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_S3C2440_IO_H_
#define _HAL_S3C2440_IO_H_

#include "common/types.h"

#define READ_REG(addr)  *(volatile UINT32 *)(addr)
#define WRITE_REG(addr, val) *(volatile UINT32 *)(addr) = (val)

#endif /* _HAL_S3C2440_IO_H_ */

/******************************************************************************/
