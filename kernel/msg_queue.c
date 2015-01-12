/**MOD+************************************************************************/
/* Module:  msg_queue.c                                                       */
/*                                                                            */
/* Purpose: message queue implementation of MiniOS                            */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/msg_queue.h"
#include "kernel/task.h"
#include "common/error.h"
#include "common/bug.h"

/**PROC+***********************************************************************/
/* Name:     mq_wakeup_send_msg                                               */
/*                                                                            */
/* Purpose:  When the task(sending msg) slept on full queue is waken up,      */
/*           this function will be called to send message into queue.         */
/*           It is called by mq_recv_msg                                      */
/*                                                                            */
/* Returns:  0 - waken up the sleeping task.                                  */
/*                                                                            */
/* Params:   IN data1 - the msg to be send                                    */
/*                      passed by mq_send_msg->sleep_on                       */
/*           IN data2 - the queue into which msg is written                   */
/*                      passed by mq_recv_msg->wake_up_one                    */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INT mq_wakeup_send_msg(VOID *data1, VOID *data2)
{
    VOID *msg = data1;
    MSG_Q *mq = (MSG_Q *)data2;

    /**************************************************************************/
    /* Ensure the queue is not full                                           */
    /**************************************************************************/
    BUG_ON(mq->mq_count >= mq->mq_capacity);

    /**************************************************************************/
    /* Copy message from user into queue                                      */
    /**************************************************************************/
    mq->mq_buf[mq->mq_write] = msg;
    mq->mq_write = (mq->mq_write + 1) % mq->mq_capacity;
    ++mq->mq_count;

    return 0;
}

/**PROC+***********************************************************************/
/* Name:     mq_recv_msg                                                      */
/*                                                                            */
/* Purpose:  receive message from queue                                       */
/*                                                                            */
/* Returns:  ENOERR - recv one msg successfully.                              */
/*           others - recv one msg failed.                                    */
/*                                                                            */
/* Params:   IN mq - the msg queue                                            */
/*           IN timeout - time interval of waiting for receiving msg          */
/*           OUT msg - received msg to user                                   */
/*                                                                            */
/**PROC-***********************************************************************/
STATUS mq_recv_msg(MSG_Q *mq, TICK_COUNT timeout, VOID **msg)
{
    STATUS result;

    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    /**************************************************************************/
    /* Ensure number of messages does not exceed the queue's capacity         */
    /**************************************************************************/
    BUG_ON(mq->mq_count > mq->mq_capacity);

    /**************************************************************************/
    /* Determine if there are messages in the queue                           */
    /**************************************************************************/
    if (mq->mq_count > 0) {
        /**********************************************************************/
        /* If the queue is partial and there are waiting tasks, bug!          */
        /**********************************************************************/
        BUG_ON((mq->mq_count < mq->mq_capacity) &&
            !WAIT_QUEUE_EMPTY(&mq->mq_wait_q));

        /**********************************************************************/
        /* Copy message from queue to user                                    */
        /**********************************************************************/
        *msg = mq->mq_buf[mq->mq_read];
        mq->mq_read = (mq->mq_read + 1) % mq->mq_capacity;
        --mq->mq_count;
        result = ENOERR;

        /**********************************************************************/
        /* Determine if any tasks slept on a full queue can be waken up.      */
        /**********************************************************************/
        if (!WAIT_QUEUE_EMPTY(&mq->mq_wait_q))
            wake_up_one(&mq->mq_wait_q, mq_wakeup_send_msg, mq);
    } else {
        /**********************************************************************/
        /* Queue is empty. Determine if the task wants to sleep               */
        /**********************************************************************/
        BUG_ON(mq->mq_read != mq->mq_write);

        if (NO_WAIT == timeout)
            result = -ENOAVAIL;
        else
            result = sleep_on(&mq->mq_wait_q, timeout, msg);
    }

    /**************************************************************************/
    /* Unlock the scheduler and maybe switch tasks                            */
    /**************************************************************************/
    sched_unlock();
    return result;
}

/**PROC+***********************************************************************/
/* Name:     mq_wakeup_recv_msg                                               */
/*                                                                            */
/* Purpose:  When the task(recving msg) slept on empty queue is waken up,     */
/*           this function will be called to receive msg directly from        */
/*           the sending msg task.                                            */
/*           It is called by __mq_send_msg.                                   */
/*                                                                            */
/* Returns:  0 - waken up the sleeping task.                                  */
/*                                                                            */
/* Params:   IN data1 - buffer to receive msg                                 */
/*                      passed by mq_recv_msg->sleep_on                       */
/*           IN data2 - msg to be send                                        */
/*                      passed by mq_send_msg->wake_up_one(all)               */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INT mq_wakeup_recv_msg(VOID *data1, VOID *data2)
{
    VOID **msg = (VOID **)data1;
    VOID *send_msg = data2;

    /**************************************************************************/
    /* Copy message directly to user                                          */
    /**************************************************************************/
    *msg = send_msg;

    return 0;
}

/**PROC+***********************************************************************/
/* Name:     __mq_send_msg                                                    */
/*                                                                            */
/* Purpose:  send message into queue                                          */
/*                                                                            */
/* Returns:  ENOERR - send one msg successfully.                              */
/*           others - send one msg failed.                                    */
/*                                                                            */
/* Params:   IN mq - the msg queue                                            */
/*           IN msg - send msg to user                                        */
/*           IN timeout - time interval of waiting for sending a msg          */
/*           IN broadcast - send msg to all waiting task                      */
/*                                                                            */
/**PROC-***********************************************************************/
STATUS __mq_send_msg(MSG_Q *mq, VOID *msg, TICK_COUNT timeout, BOOL broadcast)
{
    STATUS result;

    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    /**************************************************************************/
    /* Ensure number of messages does not exceed the queue's capacity         */
    /**************************************************************************/
    BUG_ON(mq->mq_count > mq->mq_capacity);

    /**************************************************************************/
    /* Determine if there is enough room in the queue                         */
    /**************************************************************************/
    if (mq->mq_count == mq->mq_capacity) {
        /**********************************************************************/
        /* Queue does not have room for the message.                          */
        /* Determine if sleep is required.                                    */
        /**********************************************************************/
        if (NO_WAIT == timeout)
            result = -ENOAVAIL;
        else
            result = sleep_on(&mq->mq_wait_q, timeout, msg);
    } else {
        /**********************************************************************/
        /* Determine if a task is waiting on an empty queue.                  */
        /**********************************************************************/
        if (!WAIT_QUEUE_EMPTY(&mq->mq_wait_q) && (0 == mq->mq_count)) {
            /******************************************************************/
            /* Task is waiting on an empty queue for a message                */
            /******************************************************************/
            if (TRUE == broadcast)
                wake_up_all(&mq->mq_wait_q, mq_wakeup_recv_msg, msg);
            else
                wake_up_one(&mq->mq_wait_q, mq_wakeup_recv_msg, msg);
        } else {
            /******************************************************************/
            /* There is enough room in the queue and no task is waiting.      */
            /******************************************************************/
            BUG_ON(!WAIT_QUEUE_EMPTY(&mq->mq_wait_q));

            mq->mq_buf[mq->mq_write] = msg;
            mq->mq_write = (mq->mq_write + 1) % mq->mq_capacity;
            ++mq->mq_count;
        }

        result = ENOERR;
    }

    /**************************************************************************/
    /* Unlock the scheduler and maybe switch tasks                            */
    /**************************************************************************/
    sched_unlock();
    return result;
}

/******************************************************************************/
