/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _MINIOS_TASK_H_
#define _MINIOS_TASK_H_

#include "os/timer.h"

#define SCHED_PRIORITY_MAX_NR 32
#define TICK_SCHED_QUANTUM    10

#define TICK_SCHED_ENABLED  (1 << 0)
#define TASK_TIMER_ACTIVE   (1 << 1)

#define TASK_RUNNING  1
#define TASK_SUSPEND  2
#define TASK_ZOMBIE   3

typedef void (*task_entry_t)(void *);
typedef void (*cleanup_t)(void *);

typedef struct {
    address_t stack;
    address_t stack_base;
    uint32_t stack_size;

    uint8_t state;
    uint8_t flags;
    uint8_t priority;
    uint8_t default_priority;

    list_head_t ready_node;

    int32_t time_slice;

    timer_t timer;

    task_entry_t entry;
    void *para;

    cleanup_t cleanup;
    void *cleanup_info;

    char *name;
} task_t;

/*--------------------------------------------------------------------------*/

void task_lock(void);
void task_unlock(void);
void task_create(task_t *task, char *name, uint8_t priority, uint8_t options,
    address_t stack_base, uint32_t stack_size, task_entry_t entry, void *para);
void task_suspend(task_t *task, int32_t timeout, cleanup_t cleanup, void *info);
void task_resume(task_t *task, int first);

/*--------------------------------------------------------------------------*/

#define TASK_DEFAULT_STACK_SIZE 8192
#define IDLE_TASK_STACK_SIZE 4096

typedef struct {
    task_t task;
    uint32_t magic;
    uint8_t stack[0];
} task_struct_t;

#define TASK_STRUCT_MAIGC 0xbeefbeef

#define TASK_STRUCT(name, _stack_size)    \
struct {                                  \
    task_struct_t _task;                  \
    uint8_t _stack[_stack_size];          \
} name =                                  \
{                                         \
    {.task = {.stack_size = _stack_size}, \
     .magic = TASK_STRUCT_MAIGC,},        \
};

static inline void task_struct_create(void *t, char *name,
    uint8_t priority, uint8_t options, task_entry_t entry, void *para)
{
    task_struct_t *task = (task_struct_t *)t;
    BUG_ON(TASK_STRUCT_MAIGC != task->magic);
    task_create(&task->task, name, priority, options, (address_t)task->stack,
        task->task.stack_size, entry, para);
}

static inline void task_struct_suspend(void *t, int32_t timeout,
    cleanup_t cleanup, void *info)
{
    task_struct_t *task = (task_struct_t *)t;
    BUG_ON(TASK_STRUCT_MAIGC != task->magic);
    task_suspend(&task->task, timeout, cleanup, info);
}

static inline void task_struct_resume(void *t, int first)
{
    task_struct_t *task = (task_struct_t *)t;
    BUG_ON(TASK_STRUCT_MAIGC != task->magic);
    task_resume(&task->task, first);
}

/*--------------------------------------------------------------------------*/

void task_yield(void);
void task_sleep(int32_t ticks);
void task_exit(void);
void task_restart(task_t *task);

/*--------------------------------------------------------------------------*/

#endif // _MINIOS_TASK_H_
// EOF task.h
