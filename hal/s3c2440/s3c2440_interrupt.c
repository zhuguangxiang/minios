/**MOD+************************************************************************/
/* Module:  s3c2440_interrupt.c                                               */
/*                                                                            */
/* Purpose: s3c2440 functions                                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "hal/s3c2440/s3c2440_io.h"
#include "hal/s3c2440/s3c2440_regs.h"
#include "hal/s3c2440/s3c2440_interrupt.h"
#include "common/bug.h"

VOID s3c2440_enable_irq(INT irq)
{
    UINT32 tmp = READ_REG(INTMASK);
    tmp &= ~(1 << irq);
    WRITE_REG(INTMASK, tmp);
}

VOID s3c2440_disable_irq(INT irq)
{
    UINT32 tmp = READ_REG(INTMASK);
    tmp |= (1 << irq);
    WRITE_REG(INTMASK, tmp);
}

VOID s3c2440_clear_irq(INT irq)
{
    UINT32 tmp = READ_REG(SRCPND);
    tmp |= (1 << irq);
    WRITE_REG(SRCPND, tmp);

    tmp = READ_REG(INTPND);
    tmp |= (1 << irq);
    WRITE_REG(INTPND, tmp);
}

INT s3c2440_get_irq(VOID)
{
    return (INT)READ_REG(INTOFFSET);
}

/******************************************************************************/

INT s3c2440_get_subirq(VOID)
{
    return (INT)READ_REG(SUBSRCPND);
}

VOID s3c2440_enable_subirq(INT subirq)
{
    UINT32 tmp = READ_REG(INTSUBMASK);
    tmp &= ~(1 << subirq);
    WRITE_REG(INTSUBMASK, tmp);
}

VOID s3c2440_disable_subirq(INT subirq)
{
    UINT32 tmp = READ_REG(INTSUBMASK);
    tmp |= (1 << subirq);
    WRITE_REG(INTSUBMASK, tmp);
}

VOID s3c2440_clear_subirq(INT subirq)
{
    UINT32 tmp = READ_REG(SUBSRCPND);
    tmp |= (1 << subirq);
    WRITE_REG(SUBSRCPND, tmp);
}

/******************************************************************************/

typedef struct {
    INT_HANDLE handler;
    VOID *data;
} INT_DESC;

STATIC INT_DESC int_descs[S3C2440_IRQS_NR];

STATIC VOID int_bad_handler(INT irq, VOID *data)
{
    BUG_ON(1);
}

STATIC INT_DESC bad_int_desc = {
    .handler = int_bad_handler,
    .data = NULL,
};

VOID platform_do_interrupt(VOID)
{
    INT_DESC *desc = &bad_int_desc;
    INT irq = s3c2440_get_irq();
    if ((irq >= 0) && (irq < S3C2440_IRQS_NR))
        desc = int_descs + irq;
    desc->handler(irq, desc->data);
}

VOID register_irq(INT irq, INT_HANDLE handler, VOID *data)
{
    INT_DESC *desc = int_descs + irq;

    if ((irq >= 0) && (irq < S3C2440_IRQS_NR)) {
        desc->handler = handler;
        desc->data = data;
    } else {
        BUG_ON(1);
    }
}

/******************************************************************************/
