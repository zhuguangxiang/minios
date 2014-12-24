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
#define TASK_FLAGS_TICK_SCHED    (1 << 0)
#define TASK_FLAGS_URGENT        (1 << 1)

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
    ULONG stack_size;

    /**************************************************************************/
    /* Task lock count                                                        */
    /**************************************************************************/
    INT lock_count;

    /**************************************************************************/
    /* Task state, one of TASK_STATE_(RUNNING, SUSPEND, ZOMBIE)               */
    /**************************************************************************/
    BYTE state;

    /**************************************************************************/
    /* Task flags, one of TASK_STATE_(TICK_SCHED, URGENT, AUTO_START)         */
    /**************************************************************************/
    BYTE flags;

    /**************************************************************************/
    /* Task priority                                                          */
    /**************************************************************************/
    BYTE priority;

    /**************************************************************************/
    /* Task default priority                                                  */
    /**************************************************************************/
    BYTE default_priority;

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
    BYTE priority;           /* task priority           */
    BYTE flags;              /* task flags              */
    TASK_ENTRY entry;        /* task entry function     */
    VOID *para;              /* task private data       */
    ADDRESS stack_base;      /* task stack base address */
    ULONG stack_size;        /* task stack size         */
} TASK_PARA;

/******************************************************************************/
/* Current task control block, can be used directly                           */
/******************************************************************************/
extern TASK *current;

/**API+************************************************************************/

VOID task_lock(VOID);
VOID task_unlock(VOID);

VOID task_create(TASK *task, TASK_PARA *para, BOOL auto_start);
VOID task_suspend(TASK *task, LONG ticks, CLEANUP cleanup, VOID *info);
VOID task_resume(TASK *task);

VOID task_yield(VOID);
VOID task_sleep(INT ticks);
VOID task_exit(VOID);

STATIC INLINE ULONG task_get_stack_base(TASK *task)
{
    return task->stack_base - TASK_STACK_CHECK_DATA_SIZE;
}

STATIC INLINE ULONG task_get_stack_size(TASK *task)
{
    return task->stack_size + 2 * TASK_STACK_CHECK_DATA_SIZE;
}

#ifdef TASK_STACK_CHECK
VOID task_stack_check(TASK *to);
#endif

#ifdef TASK_STACK_MEASURE
ULONG task_measure_stack_usage(TASK *task);
#endif

/**API-************************************************************************/

#endif /* _MINIOS_TASK_H_ */

/******************************************************************************/
// EOF task.h
