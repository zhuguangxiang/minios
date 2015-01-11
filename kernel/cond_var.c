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
#include "common/bug.h"

STATUS cond_wait(COND_VAR *cond, MUTEX *mutex, TICK_COUNT timeout)
{
    STATUS status;

    BUG_ON(NULL == cond);
    BUG_ON(NULL == mutex);
    BUG_ON(FALSE == mutex->locked);

    sched_lock();
    mutex_unlock(mutex);
    status = sleep_on(&cond->cv_queue, timeout, NULL);
    sched_unlock();

    /* retry lock mutex */
    mutex_lock(mutex);

    return status;
}

VOID cond_signal(COND_VAR *cond)
{
    sched_lock();
    wake_up_one(&cond->cv_queue, NULL, NULL);
    sched_unlock();
}

VOID cond_broadcast(COND_VAR *cond)
{
    sched_lock();
    wake_up_all(&cond->cv_queue, NULL, NULL);
    sched_unlock();
}

/******************************************************************************/
