/**INC+************************************************************************/
/* Header:  cond_var.h                                                        */
/*                                                                            */
/* Purpose: condition variable implementation of MiniOS                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_COND_VAR_H_
#define _MINIOS_COND_VAR_H_

#include "kernel/mutex.h"

/**STRUCT+*********************************************************************/
/* Structure: COND_VAR                                                        */
/*                                                                            */
/* Description: condition variable control block                              */
/*              The difference of condition variable and binary semaphore     */
/*              is that the signal of cond_var is lost when there is not any  */
/*              tasks waiting for it and that the signal of binary_sem is not */
/*              lost at first time.                                           */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* Queue of waiting tasks                                                 */
    /**************************************************************************/
    WAIT_QUEUE cv_queue;
} COND_VAR;

#define COND_INITIALIZER(name) \
    {WAIT_QUEUE_INITIALIZER((name).cv_queue, WQ_TYPE_FIFO)}

STATIC INLINE VOID init_cond(COND_VAR *cond)
{
    init_wait_queue(&cond->cv_queue);
}

/**API+************************************************************************/

STATUS cond_wait(COND_VAR *cond, MUTEX *mutex, TICK_COUNT timeout);
VOID cond_signal(COND_VAR *cond);
VOID cond_broadcast(COND_VAR *cond);

/**API-************************************************************************/
#endif /* _MINIOS_COND_VAR_H_ */

/******************************************************************************/
// EOF cond_var.h
