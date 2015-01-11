/**INC+************************************************************************/
/* Header:  port.h                                                            */
/*                                                                            */
/* Purpose: arch porting for MiniOS                                           */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_ARM7_9_PORT_H_
#define _HAL_ARM7_9_PORT_H_

#include "common/types.h"
#include "hal/arm7_9/cpu_const.h"
#include "hal/platform.h"

/* find first set, return 0 - 32 */
STATIC INLINE INT __ffs(ULONG bits)
{
    /* ffs(0) => 0, ffs(1) => 1, ffs(2) => 2, ffs(256) => 9, ffs(-1) = 1 */
    INT plus = 0;

    if ((bits & 0xffff) == 0) {
        plus += 16;
        bits >>= 16;
    }

    if ((bits & 0xff) == 0) {
        plus += 8;
        bits >>= 8;
    }

    if ((bits & 0xf) == 0) {
        plus += 4;
        bits >>= 4;
    }

    if (bits & 1) return plus + 1;
    if (bits & 2) return plus + 2;
    if (bits & 4) return plus + 3;
    if (bits & 8) return plus + 4;

    return 0;
}

/* -1 means no any bit set, otherwise 0 to 31 */
#define HAL_FIND_FIRST_SET(bits) (__ffs(bits) - 1)

STATIC INLINE VOID arm7_9_disable_irq(VOID)
{
    register CPU_FLAGS flags;
    asm volatile (
        "mrs %0, cpsr\n\t"
        "orr %0, %0, #0x80\n\t"
        "msr cpsr_c, %0\n\t"
        :"=r"(flags)
        :
        :"memory", "cc");
}

STATIC INLINE VOID arm7_9_enable_irq(VOID)
{
    register CPU_FLAGS flags;
    asm volatile (
        "mrs %0, cpsr\n\t"
        "bic %0, %0, #0x80\n\t"
        "msr cpsr_c, %0\n\t"
        :"=r"(flags)
        :
        :"memory", "cc");
}

#define HAL_DISABLE_INTERRUPTS arm7_9_disable_irq
#define HAL_ENABLE_INTERRUPTS arm7_9_enable_irq

VOID task_entry_wrapper(VOID);

STATIC INLINE VOID arm7_9_buid_stack(ADDRESS *stack_addr)
{
    ULONG *stack = *(ULONG **)stack_addr;

    *--stack = (ULONG)(ARM_MODE_SVC | ARM_FIQ_BIT); /* CPSR */
    *--stack = (ULONG)task_entry_wrapper;           /* PC */
    *--stack = (ULONG)0;    /* LR */
    *--stack = (ULONG)0;    /* R12 = IP(intra-procedure scratch register) */
    *--stack = (ULONG)0;    /* R11 = FP(frame pointer) */
    *--stack = (ULONG)0;    /* R10  */
    *--stack = (ULONG)0;    /* R9   */
    *--stack = (ULONG)0;    /* R8   */
    *--stack = (ULONG)0;    /* R7   */
    *--stack = (ULONG)0;    /* R6   */
    *--stack = (ULONG)0;    /* R5   */
    *--stack = (ULONG)0;    /* R4   */
    *--stack = (ULONG)0;    /* R3   */
    *--stack = (ULONG)0;    /* R2   */
    *--stack = (ULONG)0;    /* R1   */
    *--stack = (ULONG)0;    /* R0   */

    *stack_addr = (ADDRESS)stack;
}

#define HAL_TASK_BUILD_STACK arm7_9_buid_stack

VOID arm7_9_switch_context(ADDRESS *to, ADDRESS *from);
VOID arm7_9_load_context(ADDRESS *to);

#define HAL_TASK_SWITCH_CONTEXT arm7_9_switch_context
#define HAL_LOAD_TASK_CONTEXT arm7_9_load_context

#endif /* _HAL_ARM7_9_PORT_H_ */

/******************************************************************************/
