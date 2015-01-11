/**INC+************************************************************************/
/* Header:  mutex.h                                                           */
/*                                                                            */
/* Purpose: Mutex implementation of MiniOS                                    */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_MUTEX_H_
#define _MINIOS_MUTEX_H_

/******************************************************************************/
/*--------------------+-------------------+----------------------+------------*/
/*      Feature       | Binary Semaphores |  Counter Semaphores  |   Mutexes  */
/*--------------------+-------------------+----------------------+------------*/
/* Internal states    |    2              |      N               |  2         */
/*--------------------+-------------------+----------------------+------------*/
/* Use from ISRs      |   yes             |     yes              |  no        */
/*--------------------+-------------------+----------------------+------------*/
/* Ownership          |   no              |       no             |  yes       */
/*--------------------+-------------------+----------------------+------------*/
/* Priority Inherit   |    no             |       no             |  yes       */
/*--------------------+-------------------+----------------------+------------*/
/* Initialization     | either taken      |                      | always     */
/*                    | or not taken      |     counter >= 0     | not owned  */
/*--------------------+-------------------+----------------------+------------*/
/* Queue organization | FIFO or Priority  |  FIFO or Priority    | Priority   */
/*--------------------+-------------------+----------------------+------------*/
/******************************************************************************/

#include "kernel/task.h"
#include "kernel/wait_queue.h"

#define MUTEX_PROTOCOL_NONE    0
#define MUTEX_PROTOCOL_INHERIT 1
#define MUTEX_PROTOCOL_CEILING 2

/**STRUCT+*********************************************************************/
/* Structure: MUTEX                                                           */
/*                                                                            */
/* Description: mutex control block                                           */
/**STRUCT-*********************************************************************/
typedef struct mutex {
    /**************************************************************************/
    /* TRUE if locked.                                                        */
    /**************************************************************************/
    BOOL locked;

    /**************************************************************************/
    /* One of MUTEX_PROTOCOL_(NONE, INHERIT and CEILING)                      */
    /**************************************************************************/
    INT protocol;

    /**************************************************************************/
    /* current lockint task                                                   */
    /**************************************************************************/
    TASK *owner;

    /**************************************************************************/
    /* queue of waiting tasks                                                 */
    /**************************************************************************/
    WAIT_QUEUE queue;
} MUTEX;

/******************************************************************************/
/* Macros to initialize mutex                                                 */
/******************************************************************************/
#define MUTEX_INITIALIZER(name, protocol) \
    {FALSE, protocol, NULL, WAIT_QUEUE_INIT((name).queue, WQ_TYPE_FIFO)}

STATIC INLINE VOID init_mutex(MUTEX *mutex, INT protocol)
{
    mutex->locked = FALSE;
    mutex->protocol = protocol;
    mutex->owner = NULL;
    init_wait_queue(&mutex->queue, WQ_TYPE_FIFO);
}

VOID mutex_lock(MUTEX *mutex);
VOID mutex_unlock(MUTEX *mutex);

#endif /* _MINIOS_MUTEX_H_ */

/******************************************************************************/
// EOF mutex.h
