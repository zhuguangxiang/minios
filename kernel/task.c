/**MOD+************************************************************************/
/* Module:  task.c                                                            */
/*                                                                            */
/* Purpose: Task management & scheduler of MiniOS                             */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/task.h"
#include "kernel/hsr.h"
#include "common/bug.h"
#include "hal/port.h"

/******************************************************************************/
/* Check SCHED_PRIORITY_MAX_NR is valid or not                                */
/******************************************************************************/
#if ((SCHED_PRIORITY_MAX_NR <= 0) || (SCHED_PRIORITY_MAX_NR > 1024))
#error SCHED_PRIORITY_MAX_NR is out of range(1 ~ 1024)
#endif

/**STRUCT+*********************************************************************/
/* Structure: SCHED_BITMAP                                                    */
/*                                                                            */
/* Description: bitmap used by scheduler                                      */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* Bitmap group, each bit represents ULONG(4 bytes)                       */
    /**************************************************************************/
    ULONG group;

    /**************************************************************************/
    /* Bitmap array, each bit represents a priority                           */
    /**************************************************************************/
    ULONG array[BITS_TO_ULONG(SCHED_PRIORITY_MAX_NR)];
} SCHED_BITMAP;

/**PROC+***********************************************************************/
/* Name:     sched_bitmap_set                                                 */
/*                                                                            */
/* Purpose:  Set scheduler bitmap                                             */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN num  - num-th bit                                             */
/*           IN map  - scheduler bitmap                                       */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID sched_bitmap_set(INT num, SCHED_BITMAP *map)
{
    INT group_nr = num >> 5;
    INT bitmap_nr = num & 0x1f;

    map->group |= (1 << group_nr);
    map->array[group_nr] |= (1 << bitmap_nr);
}

/**PROC+***********************************************************************/
/* Name:     sched_bitmap_clear                                               */
/*                                                                            */
/* Purpose:  Clear scheduler bitmap                                           */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN num  - num-th bit                                             */
/*           IN map  - scheduler bitmap                                       */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID sched_bitmap_clear(INT num, SCHED_BITMAP *map)
{
    INT group_nr = num >> 5;
    INT bitmap_nr = num & 0x1f;
    ULONG val = map->array[group_nr] & (~(1 << bitmap_nr));

    if (0 == val)
        map->group &= ~(1 << group_nr);
    map->array[group_nr] = val;
}

/**STRUCT+*********************************************************************/
/* Structure: SCHED_BITMAP                                                    */
/*                                                                            */
/* Description: bitmap used by scheduler                                      */
/**STRUCT-*********************************************************************/
typedef struct {
    /**************************************************************************/
    /* queue_priority is the highest priority of tasks in queue_array         */
    /**************************************************************************/
    BYTE queue_priority;
    BYTE queue_reserved[3];

    /**************************************************************************/
    /* queue_map is a bitmap of priorities of tasks in queue_array            */
    /**************************************************************************/
    SCHED_BITMAP queue_map;

    /**************************************************************************/
    /* queue_array is running queue of tasks                                  */
    /**************************************************************************/
    LQE queue_array[SCHED_PRIORITY_MAX_NR];
} RUN_QUEUE;

/**PROC+***********************************************************************/
/* Name:     rq_task_add                                                      */
/*                                                                            */
/* Purpose:  add task to running queue based on priority                      */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task  - task control block                                    */
/*           IN rq    - running queue                                         */
/*           IN first - flag of adding to list head or tail                   */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID rq_task_add(TASK *task, RUN_QUEUE *rq, BOOL first)
{
    BYTE priority = task->priority;
    LQE *list = rq->queue_array + priority;

    if (TRUE == first)
        list_add(&task->run_node, list);
    else
        list_add_tail(&task->run_node, list);

    sched_bitmap_set(priority, &rq->queue_map);

    if (rq->queue_priority > priority)
        rq->queue_priority = priority;
}

/**PROC+***********************************************************************/
/* Name:     rq_task_del                                                      */
/*                                                                            */
/* Purpose:  remove task frome running queue                                  */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task  - task control block to be removed from running queue   */
/*           IN rq    - running queue                                         */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID rq_task_del(TASK *task, RUN_QUEUE *rq)
{
    BYTE priority = task->priority;
    LQE *list = rq->queue_array + priority;

    list_del(&task->run_node);

    if (list_empty(list)) {
        sched_bitmap_clear(priority, &rq->queue_map);
        if (rq->queue_priority == priority) {
            INT group_nr = HAL_FIND_FIRST_SET(rq->queue_map.group);
            INT bitmap_nr = HAL_FIND_FIRST_SET(rq->queue_map.array[group_nr]);
            rq->queue_priority = (group_nr << 5) + bitmap_nr;
        }
    }
}

/**PROC+***********************************************************************/
/* Name:     rq_task_mov_tail                                                 */
/*                                                                            */
/* Purpose:  remove task from running queue and add to list tail              */
/*           Not change priority bitmap                                       */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task  - task control block to be removed from running queue   */
/*           IN rq    - running queue                                         */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID rq_task_mov_tail(TASK *task, RUN_QUEUE *rq)
{
    LQE *list = rq->queue_array + task->priority;
    list_del(&task->run_node);
    list_add_tail(&task->run_node, list);
}

/******************************************************************************/
/* Running queue of MiniOS                                                    */
/******************************************************************************/
STATIC RUN_QUEUE running_q;

/******************************************************************************/
/* OS running flag                                                            */
/******************************************************************************/
BOOL os_running;

/******************************************************************************/
/* Task switch count                                                          */
/******************************************************************************/
UINT task_switches;

/******************************************************************************/
/* Current task of CPU                                                        */
/******************************************************************************/
TASK *current;

/******************************************************************************/
/* All tasks list                                                             */
/******************************************************************************/
STATIC LQE task_list;

/******************************************************************************/
/* Tasks count                                                                */
/******************************************************************************/
STATIC UINT task_count;

/**PROC+***********************************************************************/
/* Name:     rq_pick_task                                                     */
/*                                                                            */
/* Purpose:  pick highest priority task to run from running_q                 */
/*           This function will be called by ISR and scheduler.               */
/*                                                                            */
/* Returns:  task needs to run                                                */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
TASK *rq_pick_task(VOID)
{
    RUN_QUEUE *rq = &running_q;
    LQE *list = rq->queue_array + rq->queue_priority;
    LQE *node = list_first(list);
    BUG_ON(NULL == node);
    return LIST_ENTRY(node, TASK, run_node);
}

/**PROC+***********************************************************************/
/* Name:     rq_need_sched                                                    */
/*                                                                            */
/* Purpose:  Test whether running queue need scheduled or not                 */
/*           This function will be called by ISR and scheduler.               */
/*                                                                            */
/* Returns:  TRUE if another higher priority task needs to run                */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
BOOL rq_need_sched(VOID)
{
    if ((os_running == FALSE) || (current->lock_count > 0))
        return FALSE;
    else if (current == rq_pick_task())
        return FALSE;
    else
        return TRUE;
}

/**PROC+***********************************************************************/
/* Name:     switch_to                                                        */
/*                                                                            */
/* Purpose:  Switch current task to another task                              */
/*           Need disable all interrupts                                      */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN to - task needs to run                                        */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID switch_to(TASK *to)
{
    TASK *from;

    task_switches++;

    if ((TASK_STATE_SUSPEND == current->state) &&
        (current->flags & TASK_FLAGS_TICK_SCHED)) {
        /* yield CPU */
        current->time_slice = TICK_SCHED_QUANTUM;
    }

#ifdef TASK_STACK_CHECK
    task_stack_check(to);
#endif

    /* switch context */
    from = current;
    current = to;
    HAL_TASK_SWITCH_CONTEXT(&to->stack, &from->stack);
}

/**PROC+***********************************************************************/
/* Name:     task_lock                                                        */
/*                                                                            */
/* Purpose:  Forbidden scheduler                                              */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_lock(VOID)
{
    if (NULL == current)
        return;

    ++current->lock_count;
}

/**PROC+***********************************************************************/
/* Name:     task_unlock                                                      */
/*                                                                            */
/* Purpose:  Try to enable scheduler                                          */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_unlock(VOID)
{
    INT lock;
    TASK *to;

    if (NULL == current)
        return;

    lock = current->lock_count - 1;
    BUG_ON(lock < 0);

    if (lock == 0) {
        handle_pending_hsrs();

        HAL_DISABLE_INTERRUPTS();
        current->lock_count = 0;
        to = rq_pick_task();
        if (current != to)
            switch_to(to);
        HAL_ENABLE_INTERRUPTS();
    } else {
        current->lock_count = lock;
    }
}

#ifdef TASK_STACK_CHECK
/**PROC+***********************************************************************/
/* Name:     task_stack_check                                                 */
/*                                                                            */
/* Purpose:  Check stack is overflow ?                                        */
/*                                                                            */
/* Returns:  VOID                                                             */
/*                                                                            */
/* Params:   IN task   - TASK control block                                   */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_stack_check(TASK *task)
{
    ULONG sig = (ULONG)task;
    ULONG *base = (ULONG *)task_get_stack_base(task);
    ULONG *top = (ULONG *)(task->stack_base + task->stack_size);

    BUG_ON((sizeof(ADDRESS) - 1) & (ADDRESS)base);
    BUG_ON((sizeof(ADDRESS) - 1) & (ADDRESS)top);
    BUG_ON(task->stack < (ADDRESS)base);
    BUG_ON(task->stack > (ADDRESS)top);

    for (INT i = 0; i < TASK_STACK_CHECK_DATA_SIZE/sizeof(ULONG); i++) {
        BUG_ON((sig ^ (i * 0x01010101)) != base[i]);
        BUG_ON((sig ^ (i * 0x10101010)) != top[i]);
    }
}
#endif

#ifdef TASK_STACK_MEASURE
/**PROC+***********************************************************************/
/* Name:     task_measure_stack_usage                                         */
/*                                                                            */
/* Purpose:  Measure task stack usage                                         */
/*                                                                            */
/* Returns:  Used stack size                                                  */
/*                                                                            */
/* Params:   IN task   - TASK control block                                   */
/*                                                                            */
/**PROC-***********************************************************************/
ULONG task_measure_stack_usage(TASK *task)
{
    ULONG *base = (ULONG *)task->stack_base;
    ULONG size = task->stack_size/sizeof(ULONG);
    int i;

    for (i = 0; i < size; i ++) {
        if (base[i] != 0xdeadbeaf)
            break;
    }

    return (size - i) * sizeof(ULONG);
}
#endif

/**PROC+***********************************************************************/
/* Name:     task_init_stack                                                  */
/*                                                                            */
/* Purpose:  initialize task stack                                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task   - TASK control block                                   */
/*           IN s_base - task stack base address                              */
/*           IN s_size - task stack size                                      */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID task_init_stack(TASK *task, ADDRESS s_base, ULONG s_size)
{
#ifdef TASK_STACK_SIZE_MINIMUM
    BUG_ON(s_size < TASK_STACK_SIZE_MINIMUM);
#endif

#ifdef TASK_STACK_CHECK
    {
        ULONG sig = (ULONG)task;
        ULONG *base = (ULONG *)s_base;
        ULONG *top = (ULONG *)(s_base + s_size - TASK_STACK_CHECK_DATA_SIZE);
        INT i;
        for (i = 0; i < TASK_STACK_CHECK_DATA_SIZE/sizeof(ULONG); i++) {
            base[i] = (sig ^ (i * 0x01010101));
            top[i] = (sig ^ (i * 0x10101010));
        }
        BUG_ON(&base[i] >= &top[0]);
        s_base += i * sizeof(ULONG);
        s_size -= i * sizeof(ULONG) * 2;
        BUG_ON(s_size < 256);
    }
#endif

#ifdef TASK_STACK_MEASURE
    {
        ULONG *base = (ULONG *)s_base;
        ULONG size = s_size/sizeof(ULONG);
        INT i;
        for (i = 0; i < size; i++)
            base[i] = 0xdeadbeaf;
    }
#endif

    task->stack_base = s_base;
    task->stack_size = s_size;
    task->stack = s_base + s_size;

#ifdef TASK_STACK_CHECK
    task_stack_check(task);
#endif
}

/**PROC+***********************************************************************/
/* Name:     task_create                                                      */
/*                                                                            */
/* Purpose:  Create the task and add to running queue if necessary            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task - TASK control block                                     */
/*           IN para - Task creating parameters                               */
/*           IN auto_start - auto start task                                  */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_create(TASK *task, TASK_PARA *para, BOOL auto_start)
{
    BUG_ON(para->priority >= SCHED_PRIORITY_MAX_NR);

    task_init_stack(task, para->stack_base, para->stack_size);
    task->priority = para->priority;
    task->default_priority = para->priority;
    task->state = TASK_STATE_SUSPEND;
    task->entry = para->entry;
    task->para = para->para;
    task->time_slice = 0;
    task->lock_count = 0;
    init_list(&task->run_node);
    init_list(&task->list);
    task->name = para->name;

    list_add(&task->list, &task_list);
    ++task_count;

    if (para->flags & TASK_FLAGS_TICK_SCHED) {
        task->flags |= TASK_FLAGS_TICK_SCHED;
        task->time_slice = TICK_SCHED_QUANTUM;
    }

    if (para->flags & TASK_FLAGS_URGENT)
        task->flags |= TASK_FLAGS_URGENT;

    HAL_TASK_BUILD_STACK(&task->stack);

    if (auto_start)
        task_resume(task);
}

/**PROC+***********************************************************************/
/* Name:     task_timeout_proc                                                */
/*                                                                            */
/* Purpose:  Task waiting timeout callback                                    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN para  - TASK control block                                    */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID task_timeout_proc(VOID *para)
{
    TASK *task = (TASK *)para;

    if (TASK_STATE_SUSPEND != task->state)
        return;

    BUG_ON(TIMER_ACTIVE(&task->timer));

    if (NULL != task->cleanup)
        task->cleanup(task->cleanup_info);

    task_resume(task);
}

/**PROC+***********************************************************************/
/* Name:     task_suspend                                                     */
/*                                                                            */
/* Purpose:  Suspend the task with timeout                                    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task  - TASK control block                                    */
/*           IN ticks - waiting ticks to auto resume                          */
/*                      if ticks <= 0, it will not be auto resumed            */
/*           IN cleanup - waiting resource timeout to cleanup function        */
/*           IN info    - cleanup data                                        */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_suspend(TASK *task, LONG ticks, CLEANUP cleanup, VOID *info)
{
    TASK *to;

    BUG_ON(os_running == FALSE);

    if (task->state != TASK_STATE_RUNNING)
        return;

    task_lock();

    handle_pending_hsrs();

    if (ticks > 0) {
        BUG_ON(TIMER_ACTIVE(&task->timer));
        timer_start(&task->timer, ticks, task_timeout_proc, task);
    }

    task->cleanup = cleanup;
    task->cleanup_info = info;
    task->state = TASK_STATE_SUSPEND;
    rq_task_del(task, &running_q);

    HAL_DISABLE_INTERRUPTS();
    to = rq_pick_task();
    BUG_ON(current == to);
    switch_to(to);
    HAL_ENABLE_INTERRUPTS();

    task_unlock();
}

/**PROC+***********************************************************************/
/* Name:     task_resume                                                      */
/*                                                                            */
/* Purpose:  Resume the task to run and call cleanup function                 */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN task  - TASK control block                                    */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_resume(TASK *task)
{
    if (task->state != TASK_STATE_SUSPEND)
        return;

    task_lock();

    if (TIMER_ACTIVE(&task->timer))
        timer_stop(&task->timer);

    task->cleanup = NULL;
    task->cleanup_info = NULL;
    task->state = TASK_STATE_RUNNING;
    rq_task_add(task, &running_q, task->flags & TASK_FLAGS_URGENT);

    task_unlock();
}

/**PROC+***********************************************************************/
/* Name:     task_yield                                                       */
/*                                                                            */
/* Purpose:  Current task yield CPU                                           */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_yield(VOID)
{
    task_lock();

    if (current->flags & TASK_FLAGS_TICK_SCHED)
        current->time_slice = TICK_SCHED_QUANTUM;

    rq_task_mov_tail(current, &running_q);

    task_unlock();
}

/**PROC+***********************************************************************/
/* Name:     task_sleep                                                       */
/*                                                                            */
/* Purpose:  Current task sleep ticks                                         */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN ticks - sleeping ticks count                                  */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_sleep(INT ticks)
{
    if (ticks <= 0)
        return;

    task_suspend(current, ticks, NULL, NULL);
}

/**PROC+***********************************************************************/
/* Name:     task_exit                                                        */
/*                                                                            */
/* Purpose:  Current task exit                                                */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_exit(VOID)
{
    HAL_DISABLE_INTERRUPTS();
    rq_task_del(current, &running_q);
    current->state = TASK_STATE_ZOMBIE;
    //BUG_ON(TIMER_ACTIVE(&current->timer));
    BUG_ON(current->cleanup != NULL);
    ++task_switches;
    current = rq_pick_task();
#ifdef TASK_STACK_CHECK
    task_stack_check(current);
#endif
    HAL_LOAD_TASK_CONTEXT(&current->stack);

    /* NEVER go here */
    HAL_ENABLE_INTERRUPTS();
    BUG_ON(1);
}

/**PROC+***********************************************************************/
/* Name:     task_time_slice_proc                                             */
/*                                                                            */
/* Purpose:  task time slice scheduler, called by tick HSR                    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_time_slice_proc(VOID)
{
    RUN_QUEUE *rq = &running_q;
    TASK *task = current;

    BUG_ON(NULL == current);

    if ((task->flags & TASK_FLAGS_TICK_SCHED) && (--task->time_slice <= 0)) {

        if (task->default_priority == SCHED_PRIORITY_MAX_NR - 1) {
            task->time_slice = TICK_SCHED_QUANTUM;
            rq_task_mov_tail(task, rq);
            return;
        }

        rq_task_del(task, rq);
        ++task->priority;
        if (task->priority >= (SCHED_PRIORITY_MAX_NR - 1)) {
            task->priority = task->default_priority;
            task->time_slice = TICK_SCHED_QUANTUM;
        } else {
            task->time_slice = TICK_SCHED_QUANTUM +
            (task->priority - task->default_priority) * TICK_SCHED_QUANTUM / 2;
        }
        rq_task_add(task, rq, task->flags & TASK_FLAGS_URGENT);
    }
}

/**PROC+***********************************************************************/
/* Name:     task_entry_wrapper                                               */
/*                                                                            */
/* Purpose:  task entry wrapper function                                      */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID task_entry_wrapper(VOID)
{
    BUG_ON(NULL == current);
    BUG_ON(NULL == current->entry);
    current->entry(current->para);
    task_exit();
}

/******************************************************************************/
/* Idle task control block & stack                                            */
/******************************************************************************/
STATIC TASK idle_task;
STATIC BYTE idle_task_stack[IDLE_TASK_STACK_SIZE];

/**PROC+***********************************************************************/
/* Name:     idle_task_entry                                                  */
/*                                                                            */
/* Purpose:  idle task function                                               */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN para - idle task parameter                                    */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID idle_task_entry(VOID *para)
{
    while (1);
}

/**PROC+***********************************************************************/
/* Name:     init_idle_task                                                   */
/*                                                                            */
/* Purpose:  initialize idle task                                             */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID init_idle_task(VOID)
{
    TASK_PARA idle_task_para = {
        .name = "idle_task",
        .priority = SCHED_PRIORITY_MAX_NR - 1,
        .flags = 0,
        .entry = idle_task_entry,
        .para = NULL,
        .stack_base = (ADDRESS)idle_task_stack,
        .stack_size = IDLE_TASK_STACK_SIZE,
    };

    task_create(&idle_task, &idle_task_para, 1);
}

/**PROC+***********************************************************************/
/* Name:     mod_init_sched                                                   */
/*                                                                            */
/* Purpose:  initialize schedule module, MUST called first                    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID mod_init_sched(VOID)
{
    os_running = FALSE;
    for (INT i = 0; i < SCHED_PRIORITY_MAX_NR; i++)
        init_list(running_q.queue_array + i);
    running_q.queue_priority = SCHED_PRIORITY_MAX_NR - 1;
    init_list(&task_list);
    init_idle_task();
}

/******************************************************************************/
/* Set initial callback table                                                 */
/******************************************************************************/
MOD_INIT_CALL(mod_init_sched, MOD_SCHED_LVL);

/**PROC+***********************************************************************/
/* Name:     start_sched                                                      */
/*                                                                            */
/* Purpose:  start scheduler                                                  */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID start_sched(VOID)
{
    os_running = TRUE;
    current = rq_pick_task();
    BUG_ON(NULL == current);
    HAL_LOAD_TASK_CONTEXT(&current->stack);

    /* nerver go here */
    BUG_ON(1);
}

/**PROC+***********************************************************************/
/* Name:     os_start                                                         */
/*                                                                            */
/* Purpose:  start MiniOS normally called by system start function            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID os_start(VOID)
{
    extern INIT_CALL __MOD_TBL_START__[], __MOD_TBL_END__[];
    extern VOID app_start(VOID);
    INIT_CALL *call;

    /* initialize modules */
    for (call = __MOD_TBL_START__; call != __MOD_TBL_END__; call++)
        (*call)();

    /* initialize application */
    app_start();

    /* start scheduler */
    start_sched();
}

/******************************************************************************/
// EOF task.c
