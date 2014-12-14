/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _HAL_ARM7_9_CONST_H_
#define _HAL_ARM7_9_CONST_H_

#define ARM_MODE_SVC    0x13
#define ARM_MODE_IRQ    0x12
#define ARM_IRQ_BIT     (1 << 7)
#define ARM_FIQ_BIT     (1 << 6)

#define S_FRAME_SIZE    64
#define S_PSR           60
#define S_PC            56

#define ARM_IRQ_STACK_SIZE  8192

#define HAL_USE_INTERRUPT_STACK

#define HAL_COMMON_INTERRUPTS_STACK_SIZE 4096

#define HAL_STARTUP_STACK_SIZE 1024

#endif // _HAL_ARM7_9_CONST_H_

/*--------------------------------------------------------------------------*/
// EOF cpu_const.h
