/**INC+************************************************************************/
/* Header:  s3c2440_platform.h                                                */
/*                                                                            */
/* Purpose: S3C2440 SOC functions                                             */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_S3C2440_PLATFORM_H_
#define _HAL_S3C2440_PLATFORM_H_

#include "common/types.h"

VOID s3c2440_init_timer(VOID);

#define HAL_TIMER_INIT s3c2440_init_timer

#endif /* _HAL_S3C2440_PLATFORM_H_ */

/******************************************************************************/
// EOF s3c2440_platform.h
