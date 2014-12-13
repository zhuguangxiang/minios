/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "cpu_const.h"

/* void arm7_9_switch_context(address_t *to, address_t *from) */
    .global arm7_9_switch_context
arm7_9_switch_context:
    sub sp, sp, #S_FRAME_SIZE

    stmia sp, {r0-r12, lr}
    str lr, [sp, #S_PC]
    mrs lr, cpsr
    str lr, [sp, #S_PSR]

    str sp, [r1]

    /* fall through */

/* void arm7_9_load_context(address_t *to) */
    .global arm7_9_load_context
arm7_9_load_context:

    /* mov psr to spsr reg */
    ldr sp, [r0]
    ldr r1, [sp, #S_PSR]
    msr spsr, r1

    /* set sp reg */
    mov r1, sp
    add sp, sp, #S_FRAME_SIZE

    /* load task context */
    ldmia r1, {r0-r12, lr, pc}^
/*--------------------------------------------------------------------------*/
// EOF context_switch.S
