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

#define READ_REG(addr)  *(volatile ULONG *)(addr)
#define WRITE_REG(addr, val) *(volatile ULONG *)(addr) = (val)

#endif /* _HAL_S3C2440_IO_H_ */

/******************************************************************************/
