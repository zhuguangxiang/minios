/**MOD+************************************************************************/
/* Module:  cond_var.c                                                        */
/*                                                                            */
/* Purpose: condition variable implementation of MiniOS                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/cond_var.h"
#include "common/error.h"

STATUS cond_wait(COND_VAR *cond, MUTEX *mutex, TICK_COUNT timeout)
{
    STATUS status;
    BUG_ON(NULL == cond);
    BUG_ON(NULL == mutex);

    sched_lock();
    mutex_unlock(mutex);
    status = sleep_on(&cond->cv_queue, timeout, NULL);
    sched_unlock();

    mutex_lock(mutex);

    return status;
}

VOID cond_signal(COND_VAR *cond)
{
}

VOID cond_broadcast(COND_VAR *cond)
{
}

/******************************************************************************/
