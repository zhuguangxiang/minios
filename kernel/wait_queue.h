/**INC+************************************************************************/
/* Header:  wait_queue.h                                                      */
/*                                                                            */
/* Purpose: task wait queue used prio to task_suspend/task_resume             */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_WAIT_QUEUE_H_
#define _MINIOS_WAIT_QUEUE_H_

#include "common/types.h"
#include "common/list.h"

/******************************************************************************/
/* wait queue type, fifo or priority                                          */
/******************************************************************************/
#define WQ_TYPE_FIFO 1
#define WQ_TYPE_PRIO 2

/**STRUCT+*********************************************************************/
/* Structure: WAIT_QUEUE                                                      */
/*                                                                            */
/* Description: wait queue which is fifo type or priority type                */
/**STRUCT-*********************************************************************/
typedef struct wait_queue {
    /**************************************************************************/
    /* WAIT_NODE list, its sequenece is based on type field.                  */
    /**************************************************************************/
    LQE wq_list;

    /**************************************************************************/
    /* one of WQ_TYPE_(FIFO, PRIO)                                            */
    /**************************************************************************/
    INT wq_type;
} WAIT_QUEUE;

/******************************************************************************/
/* Macros to intialize wait queue                                             */
/******************************************************************************/
#define WAIT_QUEUE_INIT(wq, type) {LIST_INIT((wq).wq_list), type}

STATIC INLINE VOID init_wait_queue(WAIT_QUEUE *wq, INT type)
{
    init_list(&wq->wq_list);
    wq->wq_type = type;
}

/**API+************************************************************************/

/******************************************************************************/
/* if return value is not 0, wake_up_all will stop handle next wait node      */
/* data1 - passed to sleep_on                                                 */
/* data2 - passed to wake_up                                                  */
/******************************************************************************/
typedef INT (*WAKEUP_FUNC)(VOID *data1, VOID *data2);

INT sleep_on(WAIT_QUEUE *wq, LONG ticks, VOID *data);
VOID wake_up(WAIT_QUEUE *wq, INT nr_exclusive, WAKEUP_FUNC func, VOID *data);

STATIC INLINE VOID wake_up_one(WAIT_QUEUE *wq, WAKEUP_FUNC func, VOID *data)
{
    wake_up(wq, 0, func, data);
}

STATIC INLINE VOID wake_up_all(WAIT_QUEUE *wq, WAKEUP_FUNC func, VOID *data)
{
     wake_up(wq, -1, func, data);
}

/**API-************************************************************************/

#endif /* _MINIOS_WAIT_QUEUE_H_ */

/******************************************************************************/
// EOF wait_queue.h
