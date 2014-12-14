/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "hal/s3c2440/s3c2440_io.h"
#include "hal/s3c2440/s3c2440_regs.h"
#include "hal/s3c2440/s3c2440_interrupt.h"

void s3c2440_enable_irq(int irq)
{
    uint32_t tmp = READ_REG(INTMASK);
    tmp &= ~(1 << irq);
    WRITE_REG(INTMASK, tmp);
}

void s3c2440_disable_irq(int irq)
{
    uint32_t tmp = READ_REG(INTMASK);
    tmp |= (1 << irq);
    WRITE_REG(INTMASK, tmp);
}

void s3c2440_clear_irq(int irq)
{
    uint32_t tmp = READ_REG(SRCPND);
    tmp |= (1 << irq);
    WRITE_REG(SRCPND, tmp);

    tmp = READ_REG(INTPND);
    tmp |= (1 << irq);
    WRITE_REG(INTPND, tmp);
}

uint32_t s3c2440_get_irq(void)
{
    return READ_REG(INTOFFSET);
}

/*--------------------------------------------------------------------------*/

uint32_t s3c2440_get_subirq(void)
{
    return READ_REG(SUBSRCPND);
}

void s3c2440_enable_subirq(int subirq)
{
    uint32_t tmp = READ_REG(INTSUBMASK);
    tmp &= ~(1 << subirq);
    WRITE_REG(INTSUBMASK, tmp);
}

void s3c2440_disable_subirq(int subirq)
{
    uint32_t tmp = READ_REG(INTSUBMASK);
    tmp |= (1 << subirq);
    WRITE_REG(INTSUBMASK, tmp);
}

void s3c2440_clear_subirq(int subirq)
{
    uint32_t tmp = READ_REG(SUBSRCPND);
    tmp |= (1 << subirq);
    WRITE_REG(SUBSRCPND, tmp);
}

/*--------------------------------------------------------------------------*/

typedef struct {
    int_handle_t handler;
    void *data;
} int_desc_t;

static int_desc_t int_descs[INT_MAX_NR];

static void int_bad_handler(int irq, void *data)
{
    BUG_ON(1);
}

static int_desc_t bad_int_desc = {
    .handler = int_bad_handler,
    .data = NULL,
};

void platform_do_interrupt(void)
{
    int_desc_t *desc = &bad_int_desc;
    uint32_t irq = s3c2440_get_irq();
    if ((irq >= 0) && (irq < INT_MAX_NR))
        desc = int_descs + irq;
    desc->handler(irq, desc->data);
}

void register_irq(int irq, int_handle_t handler, void *data)
{
    int_desc_t *desc = int_descs + irq;

    if ((irq >= 0) && (irq < INT_MAX_NR)) {
        desc->handler = handler;
        desc->data = data;
    } else {
        BUG_ON(1);
    }
}

/*--------------------------------------------------------------------------*/
// EOF s3c2440_interrupt.c
