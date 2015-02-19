/**INC+************************************************************************/
/* Header:  semaphore.h                                                       */
/*                                                                            */
/* Purpose: semaphore implementation of MiniOS                                */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_SEMAPHORE_H_
#define _MINIOS_SEMAPHORE_H_

#include "kernel/wait_queue.h"
#include "common/bug.h"

/**STRUCT+*********************************************************************/
/* Structure: SEMA                                                            */
/*                                                                            */
/* Description: semaphore control block                                       */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* Semaphore counting                                                     */
    /**************************************************************************/
    INT sm_count;

    /**************************************************************************/
    /* Queue of waiting tasks                                                 */
    /**************************************************************************/
    WAIT_QUEUE sm_wait_q;
} SEM;

/******************************************************************************/
/* Macros to initialize semaphore                                             */
/******************************************************************************/
#define __SEM_INIT(name, count, type) \
    {count, WAIT_QUEUE_INIT((name).sm_wait_q, type)}
#define SEM_FIFO_INIT(name, count)  __SEM_INIT(name, count, WQ_TYPE_FIFO)
#define SEM_PRIO_INIT(name, count)  __SEM_INIT(name, count, WQ_TYPE_PRIO)

STATIC INLINE VOID init_sem(SEM *sem, INT count, INT type)
{
    BUG_ON((type != WQ_TYPE_FIFO) && (type != WQ_TYPE_PRIO));
    sem->sm_count = count;
    init_wait_queue(&sem->sm_wait_q, type);
}

INT sem_obtain(SEM *sem, TICK_COUNT timeout);
VOID sem_release(SEM *sem);

#endif /* _MINIOS_SEMAPHORE_H_ */

/******************************************************************************/
