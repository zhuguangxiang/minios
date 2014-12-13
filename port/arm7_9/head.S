/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "cpu_const.h"

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
    /* irq mode */
    msr cpsr_c, #ARM_MODE_IRQ + ARM_IRQ_BIT + ARM_FIQ_BIT
    /* set sp_irq */
    ldr sp, _LCirq_sp

    /* svc mode */
    msr cpsr_c, #ARM_MODE_SVC + ARM_IRQ_BIT + ARM_FIQ_BIT

    /* clear bss */
    ldr r0, _LCbss_start
    ldr r1, _LCbss_end
    mov r2, #0
    cmp r1, r0
    bls _out
_loop:
    str r2, [r0], #4
    cmp r1, r0
    bhi _loop
_out:
    /* set sp_svc */
    ldr sp, _LCstart_sp

    /* jump to main */
    b os_start

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
_LCstart_sp_base:
    .space 2048, 0
_LCstart_sp:
    .long _LCstart_sp
    .long 0xdeadbeaf
_LCirq_sp:
    .long irq_stack_addr + ARM_IRQ_STACK_SIZE
_LCbss_start:
    .long _bss_start
_LCbss_end:
    .long _bss_end
/*--------------------------------------------------------------------------*/
// EOF head.S
