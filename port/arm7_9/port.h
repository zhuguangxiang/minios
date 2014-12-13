/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#ifndef _ARM7_9_PORT_H_
#define _ARM7_9_PORT_H_

#include "os/minios_type.h"
#include "port/arm7_9/cpu_const.h"

/* find first set, return 0 - 32 */
static inline int __ffs(uint32_t bits)
{
    /* ffs(0) => 0, ffs(1) => 1, ffs(2) => 2, ffs(256) => 9, ffs(-1) = 1 */
    int plus = 0;

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

static inline void arm7_9_disable_irq(void)
{
    register cpu_flags_t flags;
    asm volatile (
        "mrs %0, cpsr\n\t"
        "orr %0, %0, #0x80\n\t"
        "msr cpsr_c, %0\n\t"
        :"=r"(flags)
        :
        :"memory", "cc");
}

static inline void arm7_9_enable_irq(void)
{
    register cpu_flags_t flags;
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

void task_entry_wrapper(void);

static inline void arm7_9_buid_stack(address_t *stack_addr)
{
    uint32_t *stack = *(uint32_t **)stack_addr;

    *--stack = (uint32_t)(ARM_MODE_SVC | ARM_FIQ_BIT); /* CPSR */
    *--stack = (uint32_t)task_entry_wrapper;           /* PC */
    *--stack = (uint32_t)0;    /* LR */
    *--stack = (uint32_t)0;    /* R12 = IP(intra-procedure scratch register) */
    *--stack = (uint32_t)0;    /* R11 = FP(frame pointer) */
    *--stack = (uint32_t)0;    /* R10  */
    *--stack = (uint32_t)0;    /* R9   */
    *--stack = (uint32_t)0;    /* R8   */
    *--stack = (uint32_t)0;    /* R7   */
    *--stack = (uint32_t)0;    /* R6   */
    *--stack = (uint32_t)0;    /* R5   */
    *--stack = (uint32_t)0;    /* R4   */
    *--stack = (uint32_t)0;    /* R3   */
    *--stack = (uint32_t)0;    /* R2   */
    *--stack = (uint32_t)0;    /* R1   */
    *--stack = (uint32_t)0;    /* R0   */

    *stack_addr = (address_t)stack;
}

#define HAL_TASK_BUILD_STACK arm7_9_buid_stack

void arm7_9_switch_context(address_t *to, address_t *from);
void arm7_9_load_context(address_t *to);

#define HAL_TASK_SWITCH_CONTEXT arm7_9_switch_context
#define HAL_LOAD_TASK_CONTEXT arm7_9_load_context

#endif // _ARM7_9_PORT_H_
// EOF port.h
