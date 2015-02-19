/**MOD+************************************************************************/
/* Module:  semaphore.c                                                       */
/*                                                                            */
/* Purpose: semaphore implementation of MiniOS                                */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/semaphore.h"
#include "common/error.h"
#include "kernel/task.h"

/**PROC+***********************************************************************/
/* Name:     sem_obtain                                                       */
/*                                                                            */
/* Purpose:  Try to obtain semaphore(P)                                       */
/*                                                                            */
/* Returns:  ENOERR - obtain the semaphore successfully.                      */
/*           others - obtain the semaphore failed.                            */
/*                                                                            */
/* Params:   IN sem - the semaphore to be obtained                            */
/*           IN timeout - time interval of waiting for the semaphore          */
/*                                                                            */
/**PROC-***********************************************************************/
INT sem_obtain(SEM *sem, TICK_COUNT timeout)
{
    INT result;

    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    if (sem->sm_count > 0) {
        /**********************************************************************/
        /* Semaphore is available and decrease the count                      */
        /**********************************************************************/
        --sem->sm_count;
        result = ENOERR;
    } else {
        /**********************************************************************/
        /* Semaphore isn't available and the task maybe go to sleep           */
        /**********************************************************************/
        if (NO_WAIT == timeout)
            result = -ENOAVAIL;
        else
            result = sleep_on(&sem->sm_wait_q, timeout, NULL);
    }

    /**************************************************************************/
    /* Unlock the scheduler and maybe switch tasks                            */
    /**************************************************************************/
    sched_unlock();
    return result;
}

/**PROC+***********************************************************************/
/* Name:     sem_release                                                      */
/*                                                                            */
/* Purpose:  Release semaphore(V)                                             */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN sem - the semaphore to be released                            */
/*                                                                            */
/**PROC-***********************************************************************/
VOID sem_release(SEM *sem)
{
    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    if (WAIT_QUEUE_EMPTY(&sem->sm_wait_q)) {
        /**********************************************************************/
        /* No tasks waiting for the semaphore, increase the count             */
        /**********************************************************************/
        ++sem->sm_count;
    } else {
        /**********************************************************************/
        /* The queue is not empty, so remove the next task from it and wake   */
        /* it up. The waiter won't decrease the count when it is wakened.     */
        /**********************************************************************/
        wake_up_one(&sem->sm_wait_q, NULL, NULL);
    }

    /**************************************************************************/
    /* Unlock the scheduler and maybe switch tasks                            */
    /**************************************************************************/
    sched_unlock();
}

/******************************************************************************/
