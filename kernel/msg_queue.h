/**INC+************************************************************************/
/* Header:  msg_queue.h                                                       */
/*                                                                            */
/* Purpose: message queue implementation of MiniOS                            */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_MSG_QUEUE_H_
#define _MINIOS_MSG_QUEUE_H_

#include "kernel/wait_queue.h"

/**STRUCT+*********************************************************************/
/* Structure: MSG_Q                                                           */
/*                                                                            */
/* Description: message queue control block                                   */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* Pointer to message queue buffer                                        */
    /**************************************************************************/
    VOID **mq_buf;

    /**************************************************************************/
    /* Write position of message queue                                        */
    /**************************************************************************/
    UINT16 mq_write;

    /**************************************************************************/
    /* Read position of message queue                                         */
    /**************************************************************************/
    UINT16 mq_read;

    /**************************************************************************/
    /* Message queue capacity                                                 */
    /**************************************************************************/
    UINT16 mq_capacity;

    /**************************************************************************/
    /* Current number of messages in the queue                                */
    /**************************************************************************/
    UINT16 mq_count;

    /**************************************************************************/
    /* Wait queue                                                             */
    /**************************************************************************/
    WAIT_QUEUE mq_wait_q;
} MSG_Q;

#define MQ_INIT(name, buf, capacity) \
    {buf, 0, 0, capacity, 0, WAIT_QUEUE_INIT((name).mq_wait_q, WQ_TYPE_FIFO)}

STATIC INLINE VOID init_mq(MSG_Q *mq, VOID **buf, UINT16 capacity)
{
    mq->mq_buf = buf;
    mq->mq_write = 0;
    mq->mq_read = 0;
    mq->mq_capacity = capacity;
    mq->mq_count = 0;
    init_wait_queue(&mq->mq_wait_q, WQ_TYPE_FIFO);
}

STATUS mq_recv_msg(MSG_Q *mq, TICK_COUNT timeout, VOID **msg);
STATUS __mq_send_msg(MSG_Q *mq, VOID *msg, TICK_COUNT timeout, BOOL broadcast);

STATIC INLINE STATUS mq_send_msg(MSG_Q *mq, VOID *msg, TICK_COUNT timeout)
{
    return __mq_send_msg(mq, msg, timeout, FALSE);
}

STATIC INLINE STATUS mq_broadcast_msg(MSG_Q *mq, VOID *msg, TICK_COUNT timeout)
{
    return __mq_send_msg(mq, msg, timeout, TRUE);
}

#endif /* _MINIOS_MSG_QUEUE_H_ */

/******************************************************************************/
