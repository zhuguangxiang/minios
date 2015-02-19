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
#include "common/fifo.h"
#include "common/bug.h"

/**STRUCT+*********************************************************************/
/* Structure: MSG_Q                                                           */
/*                                                                            */
/* Description: message queue control block                                   */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* FIFO of the queue                                                      */
    /**************************************************************************/
    FIFO mq_fifo;

    /**************************************************************************/
    /* Current number of messages in the queue                                */
    /**************************************************************************/
    UINT32 mq_count;

    /**************************************************************************/
    /* MAX length of msg                                                      */
    /**************************************************************************/
    UINT32 mq_max_msg_len;

    /**************************************************************************/
    /* Wait queue                                                             */
    /**************************************************************************/
    WAIT_QUEUE mq_wait_q;
} MSG_Q;

STATIC INLINE VOID init_mq(MSG_Q *mq, UINT8 *buffer, UINT32 size,
    INT max_msg_len, INT type)
{
    BUG_ON(size < (max_msg_len + 4));

    fifo_init(&mq->mq_fifo, buffer, size);
    mq->mq_count = 0;
    mq->mq_max_msg_len = max_msg_len;
    init_wait_queue(&mq->mq_wait_q, type);
}

INT mq_recv_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    UINT32 *actual_len, TICK_COUNT timeout);
INT __mq_send_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    TICK_COUNT timeout, BOOL broadcast);

STATIC INLINE INT mq_send_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    TICK_COUNT timeout)
{
    return __mq_send_msg(mq, buffer, len, timeout, FALSE);
}

STATIC INLINE INT mq_broadcast_msg(MSG_Q *mq, VOID *buffer, UINT32 len,
    TICK_COUNT timeout)
{
    return __mq_send_msg(mq, buffer, len, timeout, TRUE);
}

#endif /* _MINIOS_MSG_QUEUE_H_ */

/******************************************************************************/
