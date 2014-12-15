/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _MINIOS_TASK_H_
#define _MINIOS_TASK_H_

#include "os/timer.h"
#include "os/os_const.h"

#define SCHED_PRIORITY_MAX_NR 32
#define TICK_SCHED_QUANTUM    10

#define TICK_SCHED_FLAG    (1 << 0)
#define TIMER_ACTIVE_FLAG  (1 << 1)
#define URGENT_FLAG        (1 << 2)
#define AUTO_START_FLAG    (1 << 3)

#define TICK_SCHED_TASK(task) \
    (((task)->flags & TICK_SCHED_FLAG) ? TRUE : FALSE)

#define URGENT_TASK(task) \
    (((task)->flags & URGENT_FLAG) ? TRUE : FALSE)

#define TIMER_ACTIVE_TASK(task) \
    (((task)->flags & TIMER_ACTIVE_FLAG) ? TRUE : FALSE)

#define TASK_RUNNING  1
#define TASK_SUSPEND  2
#define TASK_ZOMBIE   3

typedef void (*task_entry_t)(void *);
typedef void (*cleanup_t)(void *);

typedef struct {
    addr_t stack;                  /* stack top pointer, must be first      */
    addr_t stack_base;             /* stack base pointer                    */
    uint32_t stack_size;           /* stack size                            */

    uint8_t state;                 /* (RUNNING, SUSPEND, ZOMBIE)            */
    uint8_t flags;                 /* TICK_SCHED, TIMER_ACTIVE, URGENT, ... */
    uint8_t priority;              /* priority                              */
    uint8_t default_priority;      /* default priority                      */

    list_head_t run_node;          /* running list in run queue             */

    int32_t time_slice;            /* time slice                            */

    timer_t timer;                 /* timer for task waiting something      */

    task_entry_t entry;            /* task entry function                   */
    void *para;                    /* task parameter                        */

    cleanup_t cleanup;             /* timeout to cleanup some resource      */
    void *cleanup_info;            /* clean up private data                 */

    list_head_t list;              /* all tasks list                        */

    char *name;                    /* task name                             */
} task_t;

/*--------------------------------------------------------------------------*/

void task_lock(void);
void task_unlock(void);
void task_create(task_t *task, char *name, uint8_t priority, uint8_t flags,
    addr_t stack_base, uint32_t stack_size, task_entry_t entry, void *para);
void task_suspend(task_t *task, int32_t ticks, cleanup_t cleanup, void *info);
void task_resume(task_t *task);

/*--------------------------------------------------------------------------*/

#define TASK_DEFAULT_STACK_SIZE 8192
#define IDLE_TASK_STACK_SIZE    4096

#define TASK_UNION(name, stacksize) \
union {                             \
    task_t _task;                   \
    uint8_t _stack[stacksize];      \
} name

#define DEFAULT_TASK_UNION(name) TASK_UNION(name, TASK_DEFAULT_STACK_SIZE)

static inline void task_union_create(void *t, char *name, uint8_t priority,
    uint8_t flags, uint32_t stack_size, task_entry_t entry, void *para)
{
    task_t *task = (task_t *)t;
    task_create(task, name, priority, flags, (addr_t)(task + 1),
        stack_size - sizeof(task_t), entry, para);
}

static inline void default_task_union_create(void *t, char *name,
    uint8_t priority, uint8_t flags, task_entry_t entry, void *para)
{
    task_t *task = (task_t *)t;
    task_create(task, name, priority, flags, (addr_t)(task + 1),
        TASK_DEFAULT_STACK_SIZE - sizeof(task_t), entry, para);
}

/*--------------------------------------------------------------------------*/

static inline uint32_t task_get_stack_base(task_t *task)
{
    return task->stack_base - TASK_STACK_CHECK_DATA_SIZE;
}

static inline uint32_t task_get_stack_size(task_t *task)
{
    return task->stack_size + 2 * TASK_STACK_CHECK_DATA_SIZE;
}

#ifdef TASK_STACK_CHECK
void task_stack_check(task_t *to);
#endif

#ifdef TASK_STACK_MEASURE
uint32_t task_measure_stack_usage(task_t *task);
#endif

/*--------------------------------------------------------------------------*/

void task_yield(void);
void task_sleep(int32_t ticks);
void task_exit(void);
void task_restart(task_t *task, uint8_t flags);

#endif // _MINIOS_TASK_H_

/*--------------------------------------------------------------------------*/
// EOF task.h
