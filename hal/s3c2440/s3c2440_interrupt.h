/**INC+************************************************************************/
/* Header:  s3c2440_interrupt.h                                               */
/*                                                                            */
/* Purpose: s3c2440 interrupt functions                                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_S3C2440_INTERRUPT_H_
#define _HAL_S3C2440_INTERRUPT_H_

#include "common/types.h"

VOID s3c2440_enable_irq(INT irq);
VOID s3c2440_disable_irq(INT irq);
VOID s3c2440_clear_irq(INT irq);
INT s3c2440_get_irq(VOID);
VOID s3c2440_enable_subirq(INT subirq);
VOID s3c2440_disable_subirq(INT subirq);
VOID s3c2440_clear_subirq(INT subirq);
INT s3c2440_get_subirq(VOID);

typedef VOID (*INT_HANDLE)(INT, VOID *);
VOID register_irq(INT irq, INT_HANDLE handler, VOID *data);

#define S3C2440_IRQS_NR     32
#define S3C2440_IRQ_TIMER0  10
#define S3C2440_IRQ_UART0   28
#define S3C2440_IRQ_ADC		31

#define S3C2440_SUBIRQ_ADC_S	10
#define S3C2440_SUBIRQ_TC		9

#endif /* _HAL_S3C2440_INTERRUPT_H_ */

/******************************************************************************/
