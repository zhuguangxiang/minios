/**MOD+************************************************************************/
/* Module:  interrupt.S                                                       */
/*                                                                            */
/* Purpose: ARM7&9 interrupt                                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "hal/arm7_9/cpu_const.h"
#include "config/config.h"

    .global asm_do_interrupt
asm_do_interrupt:
    sub lr, lr, #4

    stmfd sp!, {r0-r3, ip, lr}

    mrs lr, spsr
    stmfd sp!, {lr}

    ldr r1, _LCirq_level
    ldr r0, [r1]
    add r0, r0, #1
    str r0, [r1]

    /* call platform_do_interrupt */
    mov lr, pc
    ldr pc, _LCplat_irq

    ldr r1, _LCirq_level
    ldr r0, [r1]
    sub r0, r0, #1
    str r0, [r1]

    cmp r0, #0
    ldmnefd sp!, {lr}
    msrne spsr, lr
    swine #0
    ldmnefd sp!, {r0-r3, ip, pc}^   /* ^, restore spsr_irq to cpsr */

    /* enable irq */
    mrs r0, cpsr
    bic r0, r0, #ARM_IRQ_BIT
    msr cpsr, r0

    bl isr_handle_pending_hsrs

    /* diable irq */
    mrs r0, cpsr
    orr r0, r0, #ARM_IRQ_BIT
    msr cpsr, r0

    bl rq_need_sched

    ldmfd sp!, {lr}
    msr spsr, lr

    cmp r0, #0
    ldmeqfd sp!, {r0-r3, ip, pc}^   /* ^, restore spsr_irq to cpsr */

    /* need schedule, ldmeqfd isn't executed */

    ldmfd sp!, {r0-r3, ip, lr}

    /* mov irq to svc mode */
    msr cpsr_c, #(ARM_MODE_SVC | ARM_IRQ_BIT | ARM_FIQ_BIT)
    sub sp, sp, #S_FRAME_SIZE
    stmia sp, {r0-r12, lr}

    mov r2, sp

    /* mov to irq mode, get irq_lr & irq_spsr */
    msr cpsr_c, #(ARM_MODE_IRQ | ARM_IRQ_BIT | ARM_FIQ_BIT)
    mov r0, lr
    mrs r1, spsr

    /* save lr & spsr */
    str r0, [r2, #S_PC]
    str r1, [r2, #S_PSR]

    /* save sp to current->stack */
    ldr r1, _LCcurrent
    ldr r0, [r1]
    str r2, [r0]

    /* current = rq_pick_task */
    bl rq_pick_task
    ldr r1, _LCcurrent
    str r0, [r1]

    /* moe to svc mode and load new task */
    msr cpsr_c, #(ARM_MODE_SVC | ARM_IRQ_BIT | ARM_FIQ_BIT)

    /* inc task_siwthces count */
    ldr r2, _LCtask_switches
    ldr r1, [r2]
    add r1, r1, #1
    str r1, [r2]

#ifdef TASK_STACK_CHECK
    bl task_stack_check
    ldr r1, _LCcurrent
    ldr r0, [r1]
#endif

    b arm7_9_load_context

_LCirq_level:
    .long irq_level
_LCtask_switches:
    .long task_switches
_LCplat_irq:
    .long platform_do_interrupt
_LCcurrent:
    .long current

    .section ".bss"
irq_level:
    .long 0

/******************************************************************************/
