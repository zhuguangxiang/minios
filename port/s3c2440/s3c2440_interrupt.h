/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#ifndef _MINIOS_S3C2440_INTERRUPT_H_
#define _MINIOS_S3C2440_INTERRUPT_H_

#include "os/minios_type.h"

void s3c2440_enable_irq(int irq);
void s3c2440_disable_irq(int irq);
void s3c2440_clear_irq(int irq);
uint32_t s3c2440_get_irq(void);
void s3c2440_enable_subirq(int subirq);
void s3c2440_disable_subirq(int subirq);
void s3c2440_clear_subirq(int subirq);
uint32_t s3c2440_get_subirq(void);

#define INT_MAX_NR 32

typedef void (*int_handle_t)(int, void *);
void register_irq(int irq, int_handle_t handler, void *data);

#endif // _MINIOS_S3C2440_INTERRUPT_H_
// EOF s3c2440_interrupt.h
