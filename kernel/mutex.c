/**MOD+************************************************************************/
/* Module:  mutex.c                                                           */
/*                                                                            */
/* Purpose: Mutex implementation of MiniOS                                    */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/mutex.h"
#include "common/bug.h"

/**PROC+***********************************************************************/
/* Name:     mutex_lock                                                       */
/*                                                                            */
/* Purpose:  Lock and/or wait the mutex                                       */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN mutex - the mutex to be locked                                */
/*                                                                            */
/**PROC-***********************************************************************/
VOID mutex_lock(MUTEX *mutex)
{
    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    /**************************************************************************/
    /* Loop while the mutex is locked, sleeping each time around the loop.    */
    /* This copes with the possibility of a higher priority task grabbing the */
    /* mutex between the wakeup in unlock() and this task actually starting.  */
    /**************************************************************************/
    while (TRUE == mutex->locked) {
        BUG_ON(NULL == mutex->owner);
        BUG_ON(current == mutex->owner);
        BUG_ON(mutex->owner->state != TASK_STATE_RUNNING);
        BUG_ON(mutex->owner->priority < current->priority);
        if (current->priority < mutex->owner->priority) {
            /******************************************************************/
            /* Priority inversion                                             */
            /******************************************************************/
            if (mutex->protocol == MUTEX_PROTOCOL_INHERIT)
                task_set_inherit_priority(current->priority, mutex->owner);
            else if (mutex->protocol == MUTEX_PROTOCOL_CEILING)
                task_set_inherit_priority(0, mutex->owner);
            else
                BUG_ON(mutex->protocol != MUTEX_PROTOCOL_NONE);
        }
        sleep_on(&mutex->queue, -1, current);
    }

    /**************************************************************************/
    /* Got the mutex and set its owner to current task.                       */
    /**************************************************************************/
    BUG_ON(NULL != mutex->owner);
    mutex->locked = TRUE;
    mutex->owner = current;

    /**************************************************************************/
    /* Increase the number of mutexes held by current task.                   */
    /* Allow a task to hold multi-mutexes, and restore its priority to        */
    /* orignal priority only if all mutexes held by this task are unlocked.   */
    /**************************************************************************/
    if (mutex->protocol != MUTEX_PROTOCOL_NONE)
        ++current->mutex_count;

    /**************************************************************************/
    /* Unlock scheduler and maybe switch tasks                                */
    /**************************************************************************/
    sched_unlock();
}

/**PROC+***********************************************************************/
/* Name:     mutex_relay_priority                                             */
/*                                                                            */
/* Purpose:  Relay task's priority to next task which will grab the mutex     */
/*                                                                            */
/* Returns:  0, wait queue will continue to resume other tasks                */
/*           1, stop wake up current task and others behind it                */
/*                                                                            */
/* Params:   IN data1 - wait queue callback data1                             */
/*           IN data2 - wait queue callback data2                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INT mutex_relay_priority(VOID *data1, VOID *data2)
{
    TASK *task = (TASK *)data1;
    WAIT_QUEUE *queue = (WAIT_QUEUE *)data2;

    BUG_ON(TASK_STATE_SUSPEND != task->state);

    if (WAIT_QUEUE_COUNT(queue) > 1) {
        task_set_inherit_priority(current->priority, task);
    }

    return 0;
}

/**PROC+***********************************************************************/
/* Name:     mutex_unlock                                                     */
/*                                                                            */
/* Purpose:  Unlock the mutex                                                 */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN mutex - the mutex to be unlocked                              */
/*                                                                            */
/**PROC-***********************************************************************/
VOID mutex_unlock(MUTEX *mutex)
{
    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    BUG_ON(mutex->locked == FALSE);
    BUG_ON(mutex->owner != current);

    /**************************************************************************/
    /* Wake up one task if there are waiting tasks.                           */
    /**************************************************************************/
    if (0 != WAIT_QUEUE_COUNT(&mutex->queue)) {
        if (mutex->protocol != MUTEX_PROTOCOL_NONE)
            wake_up_one(&mutex->queue, mutex_relay_priority, &mutex->queue);
        else
            wake_up_one(&mutex->queue, NULL, NULL);
    }

    /**************************************************************************/
    /* decrease the number of mutexes held by current task.                   */
    /**************************************************************************/
    if (mutex->protocol != MUTEX_PROTOCOL_NONE)
        --current->mutex_count;

    /**************************************************************************/
    /* Restore task's priority if priority inversion happen.                  */
    /**************************************************************************/
    if ((mutex->protocol == MUTEX_PROTOCOL_INHERIT) ||
        (mutex->protocol == MUTEX_PROTOCOL_CEILING))
        task_clear_inherit_priority(current);
    else
        BUG_ON(mutex->protocol != MUTEX_PROTOCOL_NONE);

    /**************************************************************************/
    /* Release the mutex and set its owner to NULL.                           */
    /**************************************************************************/
    mutex->locked = FALSE;
    mutex->owner = NULL;

    /**************************************************************************/
    /* Unlock scheduler and maybe switch tasks                                */
    /**************************************************************************/
    sched_unlock();
}

/******************************************************************************/
