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
#include "common/string.h"

typedef struct {
    VOID *buffer;
    UINT32 len;
    UINT32 actual_len;
} MSG_BUF_INFO;

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
    MSG_BUF_INFO *msg = (MSG_BUF_INFO *)data1;
    MSG_Q *mq = (MSG_Q *)data2;
    UINT32 l;

    /**************************************************************************/
    /* Determine the queue buffer is large enough                             */
    /**************************************************************************/
    if (fifo_space(&mq->mq_fifo) < (msg->len + 4)) {
        /**********************************************************************/
        /* not enough room for this msg. stop wakeup                          */
        /**********************************************************************/
        return -1;
    }

    /**************************************************************************/
    /* Copy message from user into queue                                      */
    /**************************************************************************/
    l = fifo_put(&mq->mq_fifo, (UINT8 *)&msg->len, sizeof(UINT32));
    BUG_ON(l != sizeof(UINT32));

    l = fifo_put(&mq->mq_fifo, msg->buffer, msg->len);
    BUG_ON(l != msg->len);

    /**************************************************************************/
    /* Increase msg count in the queue                                        */
    /**************************************************************************/
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
/*           OUT buffer - received msg buffer                                 */
/*           IN len - expectedly received msg len                             */
/*           OUT actual_len - actually received msg len                       */
/*           IN timeout - time interval of waiting for receiving msg          */
/*                                                                            */
/**PROC-***********************************************************************/
STATUS mq_recv_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    UINT32 *actual_len, TICK_COUNT timeout)
{
    STATUS result;
    UINT32 msg_size = 0;
    MSG_BUF_INFO msg;
    UINT32 l;

    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    /**************************************************************************/
    /* Ensure receiving buffer is large enough                                */
    /**************************************************************************/
    BUG_ON(mq->mq_max_msg_len > len);

    /**************************************************************************/
    /* Determine if there are messages in the queue                           */
    /**************************************************************************/
    if (mq->mq_count > 0) {
        /**********************************************************************/
        /* Copy message from queue to user                                    */
        /**********************************************************************/
        l = fifo_get(&mq->mq_fifo, (UINT8 *)&msg_size, sizeof(UINT32));
        BUG_ON(l != sizeof(UINT32));
        BUG_ON(msg_size > len);

        l = fifo_get(&mq->mq_fifo, (UINT8 *)buffer, msg_size);
        BUG_ON(l != msg_size);

        *actual_len = msg_size;

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
        BUG_ON(0 != fifo_count(&mq->mq_fifo));

        if (NO_WAIT == timeout) {
            result = -ENOAVAIL;
        } else {
            msg.buffer = buffer;
            msg.len = len;
            msg.actual_len = 0;
            result = sleep_on(&mq->mq_wait_q, timeout, &msg);
            *actual_len = msg.actual_len;
        }
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
    MSG_BUF_INFO *recv_msg = (MSG_BUF_INFO *)data1;
    MSG_BUF_INFO *send_msg = (MSG_BUF_INFO *)data2;

    /**************************************************************************/
    /* Copy message directly to user                                          */
    /**************************************************************************/
    BUG_ON(recv_msg->len < send_msg->len);

    memcpy(recv_msg->buffer, send_msg->buffer, send_msg->len);

    recv_msg->actual_len = send_msg->len;
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
/*           IN buffer - to be sent msg                                       */
/*           IN len - msg len                                                 */
/*           IN timeout - time interval of waiting for sending a msg          */
/*           IN broadcast - send msg to all waiting task                      */
/*                                                                            */
/**PROC-***********************************************************************/
STATUS __mq_send_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    TICK_COUNT timeout, BOOL broadcast)
{
    STATUS result;
    MSG_BUF_INFO msg;
    UINT32 l;

    /**************************************************************************/
    /* Prevent preemption                                                     */
    /**************************************************************************/
    sched_lock();

    /**************************************************************************/
    /* Ensure sending buffer len is less equal than max_msg_len               */
    /**************************************************************************/
    BUG_ON(len > mq->mq_max_msg_len);

    /**************************************************************************/
    /* Determine if there is enough room in the queue                         */
    /**************************************************************************/
    if ((len + sizeof(UINT32)) > fifo_space(&mq->mq_fifo)) {
        /**********************************************************************/
        /* Queue does not have room for the message.                          */
        /* Determine if sleep is required.                                    */
        /**********************************************************************/
        if (NO_WAIT == timeout) {
            result = -ENOAVAIL;
        } else {
            msg.buffer = buffer;
            msg.len = len;
            result = sleep_on(&mq->mq_wait_q, timeout, &msg);
        }
    } else {
        /**********************************************************************/
        /* Determine if a task is waiting on an empty queue.                  */
        /**********************************************************************/
        if (!WAIT_QUEUE_EMPTY(&mq->mq_wait_q) && (0 == mq->mq_count)) {
            /******************************************************************/
            /* Task is waiting on an empty queue for a message                */
            /******************************************************************/
            msg.buffer = buffer;
            msg.len = len;

            if (TRUE == broadcast)
                wake_up_all(&mq->mq_wait_q, mq_wakeup_recv_msg, &msg);
            else
                wake_up_one(&mq->mq_wait_q, mq_wakeup_recv_msg, &msg);
        } else {
            /******************************************************************/
            /* There is enough room in the queue and no task is waiting.      */
            /******************************************************************/
            BUG_ON(!WAIT_QUEUE_EMPTY(&mq->mq_wait_q));

            l = fifo_put(&mq->mq_fifo, (UINT8 *)&len, sizeof(UINT32));
            BUG_ON(l != sizeof(UINT32));

            l = fifo_put(&mq->mq_fifo, (UINT8 *)buffer, len);
            BUG_ON(l != len);

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
