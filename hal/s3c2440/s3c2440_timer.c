/**MOD+************************************************************************/
/* Module:  s3c2440_timer.c                                                   */
/*                                                                            */
/* Purpose: s3c2440 hardware timer configuration                              */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "hal/s3c2440/s3c2440_regs.h"
#include "hal/s3c2440/s3c2440_io.h"
#include "hal/s3c2440/s3c2440_interrupt.h"
#include "config/config.h"

/******************************************************************************/
/* External functions & variables                                             */
/******************************************************************************/
VOID tick_increase(VOID);

/**PROC+***********************************************************************/
/* Name:     s3c2440_do_timer_interrupt                                       */
/*                                                                            */
/* Purpose:  Hardware timer handler                                           */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN irq  - hardware timer interrupt number                        */
/*           IN data - user data                                              */
/*                                                                            */
/**PROC-***********************************************************************/
VOID s3c2440_do_timer_interrupt(INT irq, VOID *data)
{
    s3c2440_disable_irq(irq);
    s3c2440_clear_irq(irq);
    tick_increase();
    s3c2440_enable_irq(irq);
}

/**PROC+***********************************************************************/
/* Name:     s3c2440_init_timer                                               */
/*                                                                            */
/* Purpose:  Hardware timer initialization, called by Software timer module   */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID s3c2440_init_timer(VOID)
{
    ULONG tmp;

    register_irq(S3C2440_IRQ_TIMER0, s3c2440_do_timer_interrupt, NULL);

    tmp = READ_REG(TCFG0);
    tmp |= 199; /* prescaler=199 */
    WRITE_REG(TCFG0, tmp);

    tmp = READ_REG(TCFG1);
    tmp |= 1; /* divider = 1/4 */
    WRITE_REG(TCFG1, tmp);

    tmp = (62500 / CLOCK_HZ); /* timer count */
    WRITE_REG(TCNTB0, tmp);

    /* manual update bit */
    tmp = READ_REG(TCON);
    tmp |= 2;
    WRITE_REG(TCON, tmp);

    /* start timer0, auto-reload and clear manual update bit */
    tmp = READ_REG(TCON);
    tmp |= 9;
    tmp &= ~2;
    WRITE_REG(TCON, tmp);

    /* enable irq */
    s3c2440_enable_irq(S3C2440_IRQ_TIMER0);
}

/******************************************************************************/
