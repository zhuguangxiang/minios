/**INC+************************************************************************/
/* Header:  task.h                                                            */
/*                                                                            */
/* Purpose: task management of MiniOS kenrel                                  */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_TASK_H_
#define _MINIOS_TASK_H_

#include "common/types.h"
#include "common/list.h"
#include "kernel/timer.h"
#include "config/config.h"

/******************************************************************************/
/* Macros of task state                                                       */
/* MiniOS has only three state, running, suspend, and zombie. When task is    */
/* created, its state is suspend. When task is finished, its state is zombie. */
/******************************************************************************/
#define TASK_STATE_RUNNING    1
#define TASK_STATE_SUSPEND    2
#define TASK_STATE_ZOMBIE     3

/******************************************************************************/
/* Macros of task flags                                                       */
/* If TICK_SCHED is set, the task is scheduled by tick clock.                 */
/* If URGENT is set, the task is firstly scheduled with the same priority.    */
/******************************************************************************/
#define TASK_FLAGS_TICK_SCHED          (1 << 0)
#define TASK_FLAGS_URGENT              (1 << 1)
#define TASK_FLAGS_PRIORITY_INHERITED  (1 << 2)

/**STRUCT+*********************************************************************/
/* Structure: TASK                                                            */
/*                                                                            */
/* Description: Task control block                                            */
/**STRUCT-*********************************************************************/
typedef VOID (*TASK_ENTRY)(VOID *);
typedef VOID (*CLEANUP)(VOID *);
typedef struct task {
    /**************************************************************************/
    /* Stack top address, MUST be first of TASK structure                     */
    /**************************************************************************/
    ADDRESS stack;

    /**************************************************************************/
    /* Stack base address                                                     */
    /**************************************************************************/
    ADDRESS stack_base;

    /**************************************************************************/
    /* Stack size                                                             */
    /**************************************************************************/
    UINT32 stack_size;

    /**************************************************************************/
    /* Task state, one of TASK_STATE_(RUNNING, SUSPEND, ZOMBIE)               */
    /**************************************************************************/
    UINT8 state;

    /**************************************************************************/
    /* Task priority                                                          */
    /**************************************************************************/
    UINT8 priority;

    /**************************************************************************/
    /* Task default priority                                                  */
    /**************************************************************************/
    UINT8 default_priority;

    /**************************************************************************/
    /* Task flags, one of TASK_FLAGS_(TICK_SCHED, URGENT)                     */
    /**************************************************************************/
    UINT8 flags;

    /**************************************************************************/
    /* How many mutexes have been locked. When all mutexes are unlocked, task */
    /* is restored to its original priority.                                  */
    /**************************************************************************/
    INT8 mutex_count;

    /**************************************************************************/
    /* Original priority, used for mutex                                      */
    /**************************************************************************/
    UINT8 original_priority;

    /**************************************************************************/
    /* Scheduler lock count                                                   */
    /**************************************************************************/
    INT16 lock_count;

    /**************************************************************************/
    /* Task time slice schedule value                                         */
    /**************************************************************************/
    INT time_slice;

    /**************************************************************************/
    /* Task running node in running queue                                     */
    /**************************************************************************/
    LQE run_node;

    /**************************************************************************/
    /* Task entry function                                                    */
    /**************************************************************************/
    TASK_ENTRY entry;

    /**************************************************************************/
    /* Task parameter                                                         */
    /**************************************************************************/
    VOID *para;

    /**************************************************************************/
    /* Task sleep timer                                                       */
    /**************************************************************************/
    TIMER timer;

    /**************************************************************************/
    /* Task cleanup function used for waiting resource timeout                */
    /**************************************************************************/
    CLEANUP cleanup;

    /**************************************************************************/
    /* Task cleanup data                                                      */
    /**************************************************************************/
    VOID *cleanup_info;

    /**************************************************************************/
    /* Task list                                                              */
    /**************************************************************************/
    LQE list;

    /**************************************************************************/
    /* Task name                                                              */
    /**************************************************************************/
    CONST CHAR *name;
} TASK;

/**STRUCT+*********************************************************************/
/* Structure: TASK_PARA                                                       */
/*                                                                            */
/* Description: Task creating parameters                                      */
/**STRUCT-*********************************************************************/
typedef struct {
    CONST CHAR *name;        /* task name               */
    UINT8 priority;           /* task priority           */
    UINT8 flags;              /* task flags              */
    TASK_ENTRY entry;        /* task entry function     */
    VOID *para;              /* task private data       */
    ADDRESS stack_base;      /* task stack base address */
    UINT32 stack_size;        /* task stack size         */
} TASK_PARA;

/******************************************************************************/
/* Current task control block, can be used directly                           */
/******************************************************************************/
extern TASK *current;

VOID task_lock(VOID);
VOID task_unlock(VOID);

VOID task_create(TASK *task, TASK_PARA *para, BOOL auto_start);
VOID task_suspend(TASK *task, TICK_COUNT ticks, CLEANUP cleanup, VOID *info);
VOID task_resume(TASK *task);

VOID task_yield(VOID);
VOID task_sleep(INT ticks);
VOID task_exit(VOID);

VOID sched_lock(VOID);
VOID sched_unlock(VOID);

STATIC INLINE UINT32 task_get_stack_base(TASK *task)
{
    return task->stack_base - TASK_STACK_CHECK_DATA_SIZE;
}

STATIC INLINE UINT32 task_get_stack_size(TASK *task)
{
    return task->stack_size + 2 * TASK_STACK_CHECK_DATA_SIZE;
}

#ifdef TASK_STACK_CHECK
VOID task_stack_check(TASK *to);
#endif

#ifdef TASK_STACK_MEASURE
UINT32 task_measure_stack_usage(TASK *task);
#endif

VOID task_set_inherit_priority(UINT8 priority, TASK *task);
VOID task_clear_inherit_priority(TASK *task);

#endif /* _MINIOS_TASK_H_ */

/******************************************************************************/
