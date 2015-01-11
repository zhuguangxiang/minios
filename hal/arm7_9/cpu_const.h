/**INC+************************************************************************/
/* Header:  cpu.h                                                             */
/*                                                                            */
/* Purpose: ARM7&9 const                                                      */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_ARM7_9_CONST_H_
#define _HAL_ARM7_9_CONST_H_

#define ARM_MODE_SVC    0x13
#define ARM_MODE_IRQ    0x12
#define ARM_IRQ_BIT     (1 << 7)
#define ARM_FIQ_BIT     (1 << 6)

#define S_FRAME_SIZE    64
#define S_PSR           60
#define S_PC            56

#endif /* _HAL_ARM7_9_CONST_H_ */

/******************************************************************************/
