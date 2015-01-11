/**MOD+************************************************************************/
/* Module:  head.S                                                            */
/*                                                                            */
/* Purpose: ARM7&9 start                                                      */
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

    .global _start
_start:
    b _init
    b _undef_instr
    b _swi
    b _abort_addr
    b _abort_data
    b _reserved
    ldr pc, _LCirq_handler
    b _fiq

_init:
    // clear bss
    ldr r1, _LCbss_start
    ldr r2, _LCbss_end
    mov r0, #0
    cmp r1, r2
    beq 2f
1:
    str r0, [r1], #4
    cmp r2, r1
    bhi 1b
2:
    // set irq sp
    msr cpsr_c, #(ARM_MODE_IRQ | ARM_IRQ_BIT | ARM_FIQ_BIT)
    ldr sp, _LCinterrupt_stack

    // set svc sp
    msr cpsr_c, #(ARM_MODE_SVC | ARM_IRQ_BIT | ARM_FIQ_BIT)
#ifdef HAL_USE_INTERRUPT_STACK
    ldr sp, _LCinterrupt_stack
#else
    ldr sp, _LCstartup_stack
#endif

    // start minios
    bl os_start

_start_halt:
    b _start_halt

/******************************************************************************/

_undef_instr:
    b _undef_instr
_swi:
    b _swi
_abort_addr:
    b _abort_addr
_abort_data:
    b _abort_data
_reserved:
    b _reserved
_fiq:
    b _fiq
_LCirq_handler:
    .long asm_do_interrupt
#ifndef HAL_USE_INTERRUPT_STACK
_LCstartup_stack:
    .long __startup_stack
#endif
_LCinterrupt_stack:
     .long __interrupt_stack
_LCbss_start:
    .long _bss_start
_LCbss_end:
    .long _bss_end
_LCdeadbe01:
    .long 0xdeadbe01

/******************************************************************************/

    .section ".bss"
#ifndef HAL_INTERRUPTS_STACK_SIZE
#define HAL_INTERRUPTS_STACK_SIZE 4096
#endif
    .balign 16
__interupt_stack_base:
    .rept HAL_INTERRUPTS_STACK_SIZE
    .byte 0
    .endr
    .balign 16
__interrupt_stack:

#ifndef HAL_USE_INTERRUPT_STACK

#ifndef HAL_STARTUP_STACK_SIZE
#define HAL_STARTUP_STACK_SIZE 1024
#endif

    .balign 16
__startup_stack_base:
    .rept HAL_STARTUP_STACK_SIZE
    .byte 0
    .endr
    .balign 16
__startup_stack:

#endif /* HAL_USE_INTERRUPT_STACK */

/******************************************************************************/
