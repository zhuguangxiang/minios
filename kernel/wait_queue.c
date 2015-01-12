/**MOD+************************************************************************/
/* Module:  wait_queue.c                                                      */
/*                                                                            */
/* Purpose: task wait queue implementation                                    */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/bug.h"
#include "common/error.h"
#include "kernel/wait_queue.h"
#include "kernel/task.h"

/**STRUCT+*********************************************************************/
/* Structure: WAIT_NODE                                                       */
/*                                                                            */
/* Description: wait node which will be add to WAIT_QUEUE                     */
/**STRUCT-*********************************************************************/
typedef struct wait_node {
    /**************************************************************************/
    /* List node, added to WAIT_QUEUE.list                                    */
    /**************************************************************************/
    LQE wn_node;

    /**************************************************************************/
    /* Pointer to WAIT_QUEUE                                                  */
    /**************************************************************************/
    WAIT_QUEUE *wn_wq;

    /**************************************************************************/
    /* Task waiting on WAIT_QUEUE                                             */
    /**************************************************************************/
    TASK *wn_task;

    /**************************************************************************/
    /* Task waiting whether is timeout or not                                 */
    /**************************************************************************/
    STATUS wn_status;

    /**************************************************************************/
    /* User private data                                                      */
    /**************************************************************************/
    VOID *wn_data;
} WAIT_NODE;

/******************************************************************************/
/* Macros to intialize wait node                                              */
/******************************************************************************/
#define WAIT_NODE_INIT(wn, data) \
    {LIST_INIT((wn).wn_node), NULL, current, ENOERR, data}

/**PROC+***********************************************************************/
/* Name:     add_wait_queue                                                   */
/*                                                                            */
/* Purpose:  add a WAIT_NODE to WAIT_QUEUE according to type of wait queue    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN wq  - wait queue                                              */
/*           IN wn  - wait node to be added                                   */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID add_wait_queue(WAIT_QUEUE *wq, WAIT_NODE *wn)
{
    LQE *pos;
    WAIT_NODE *t;

    BUG_ON(NULL == wn->wn_task);

    if (wq->wq_type == WQ_TYPE_FIFO) {
        pos = &wq->wq_list;
    } else {
        BUG_ON(WQ_TYPE_PRIO != wq->wq_type);
        LIST_FOR_EACH(pos, &wq->wq_list) {
            t = LIST_ENTRY(pos, WAIT_NODE, wn_node);
            BUG_ON(NULL == t->wn_task);
            BUG_ON(TASK_STATE_SUSPEND != t->wn_task->state);
            if (t->wn_task->priority > wn->wn_task->priority)
                break;
        }
    }

    ++wq->wq_count;
    wn->wn_wq = wq;
    __list_add(&wn->wn_node, pos->prev, pos);
}

/**PROC+***********************************************************************/
/* Name:     remove_wait_queue                                                */
/*                                                                            */
/* Purpose:  remove the WAIT_NODE from its WAIT_QUEUE                         */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN wn  - wait node to be removed                                 */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID remove_wait_queue(WAIT_NODE *wn)
{
    BUG_ON(FALSE == lqe_in_list(&wn->wn_node));
    list_del(&wn->wn_node);
    --wn->wn_wq->wq_count;
}

/**PROC+***********************************************************************/
/* Name:     wq_timeout_proc                                                  */
/*                                                                            */
/* Purpose:  WAIT_NODE timeout process                                        */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN data - WAIT_NODE                                              */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID wq_timeout_proc(VOID *data)
{
    WAIT_NODE *wn = (WAIT_NODE *)data;

    remove_wait_queue(wn);

    wn->wn_status = -ETIMEOUT;
}

/**PROC+***********************************************************************/
/* Name:     sleep_on                                                         */
/*                                                                            */
/* Purpose:  Current task is sleeping on WAIT_QUEUE                           */
/*                                                                            */
/* Returns:  -ETIMEOUT, if waiting timeout, otherwise ENOERR                  */
/*                                                                            */
/* Params:   IN wq    - wait queue                                            */
/*           IN ticks - waiting timeout ticks                                 */
/*           IN data  - user data will be passed to WAKEUP_FUNC               */
/*                                                                            */
/**PROC-***********************************************************************/
STATUS sleep_on(WAIT_QUEUE *wq, TICK_COUNT ticks, VOID *data)
{
    WAIT_NODE wn = WAIT_NODE_INIT(wn, data);

    sched_lock();
    add_wait_queue(wq, &wn);
    task_suspend(current, ticks, wq_timeout_proc, &wn);
    sched_unlock();

    return wn.wn_status;
}

/**PROC+***********************************************************************/
/* Name:     wake_up                                                          */
/*                                                                            */
/* Purpose:  wake up WAIT_NODE on WAIT_QUEUE                                  */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN wq    - wait queue                                            */
/*           IN nr    - how many WAIT_NODE to be waken up                     */
/*           IN func  - called by every WAIT_NODE                             */
/*           IN data  - user data will be passed to WAKEUP_FUNC               */
/*                                                                            */
/**PROC-***********************************************************************/
VOID wake_up(WAIT_QUEUE *wq, INT nr, WAKEUP_FUNC func, VOID *data)
{
    WAIT_NODE *wn, *nxt;

    sched_lock();

    LIST_FOR_EACH_ENTRY_SAFE(wn, nxt, &wq->wq_list, wn_node) {

        if ((NULL != func) && func(wn->wn_data, data))
            break;

        remove_wait_queue(wn);
        BUG_ON(NULL == wn->wn_task);
        task_resume(wn->wn_task);

        if (!--nr)
            break;
    }

    sched_unlock();
}

/******************************************************************************/
