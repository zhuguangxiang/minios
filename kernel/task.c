/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "os/task.h"
#include "os/hsr.h"
#include "hal/port.h"

/*--------------------------------------------------------------------------*/

#if ((SCHED_PRIORITY_MAX_NR <= 0) || (SCHED_PRIORITY_MAX_NR > 1024))
#error SCHED_PRIORITY_MAX_NR is out of range(1 ~ 1024)
#endif

typedef struct {
    uint32_t group;
    uint32_t bitmap[BITS_TO_UINT32(SCHED_PRIORITY_MAX_NR)];
} sched_bitmap_t;

typedef struct {
    uint32_t highest_priority;
    sched_bitmap_t queue_map;
    list_head_t queue_array[SCHED_PRIORITY_MAX_NR];
} run_queue_t;

/*--------------------------------------------------------------------------*/

static inline void rq_bitmap_set(sched_bitmap_t *map, int32_t nr)
{
    uint32_t group_nr = nr >> 5;
    uint32_t bitmap_nr = nr & 0x1f;

    map->group |= (1 << group_nr);
    map->bitmap[group_nr] |= (1 << bitmap_nr);
}

static inline void rq_bitmap_clear(sched_bitmap_t *map, int32_t nr)
{
    uint32_t group_nr = nr >> 5;
    uint32_t bitmap_nr = nr & 0x1f;
    uint32_t value = map->bitmap[group_nr] & (~(1 << bitmap_nr));

    if (0 == value)
        map->group &= ~(1 << group_nr);
    map->bitmap[group_nr] = value;
}

static void rq_task_add(run_queue_t *rq, task_t *task, bool_t first)
{
    uint8_t priority = task->priority;
    list_head_t *list = rq->queue_array + priority;

    if (TRUE == first)
        LIST_ADD(list, &task->run_node);
    else
        LIST_ADD_TAIL(list, &task->run_node);

    rq_bitmap_set(&rq->queue_map, priority);

    if (rq->highest_priority > priority)
        rq->highest_priority = priority;
}

static inline void rq_task_move_tail(run_queue_t *rq, task_t *task)
{
    list_head_t *list = rq->queue_array + task->priority;
    LIST_DEL(&task->run_node);
    LIST_ADD_TAIL(list, &task->run_node);
}

static void rq_task_delete(run_queue_t *rq, task_t *task)
{
    uint8_t priority = task->priority;
    list_head_t *list = rq->queue_array + priority;

    LIST_DEL(&task->run_node);

    if (LIST_EMPTY(list)) {
        rq_bitmap_clear(&rq->queue_map, priority);

        if (rq->highest_priority == priority) {
            uint32_t group = HAL_FIND_FIRST_SET(rq->queue_map.group);
            uint32_t bitmap = HAL_FIND_FIRST_SET(rq->queue_map.bitmap[group]);
            rq->highest_priority = (group << 5) + bitmap;
        }
    }
}

/*--------------------------------------------------------------------------*/

static run_queue_t run_queue;
uint32_t task_switches;
int32_t sched_lock = 1;
task_t *current;

list_head_t task_list;
uint32_t task_count;

/*--------------------------------------------------------------------------*/

task_t *rq_pick_task(void)
{
    run_queue_t *rq = &run_queue;
    list_head_t *list = rq->queue_array + rq->highest_priority;
    list_head_t *node = LIST_FIRST(list);
    BUG_ON(NULL == node);
    return LIST_ENTRY(node, task_t, run_node);
}

bool_t need_sched(void)
{
    if (sched_lock > 0)
        return FALSE;

    return (current != rq_pick_task()) ? TRUE : FALSE;
}

static void schedule(void)
{
    task_t *from;
    task_t *to;

    HAL_DISABLE_INTERRUPTS();

    sched_lock = 0;

    from = current;
    to = rq_pick_task();

    if (from != to) {
        // count task switch
        task_switches++;

        // save time slice
        if ((TASK_SUSPEND == from->state) && TICK_SCHED_TASK(from))
            from->time_slice = TICK_SCHED_QUANTUM;

#ifdef TASK_STACK_CHECK
        task_stack_check(to);
#endif
        // switch context
        current = to;
        HAL_TASK_SWITCH_CONTEXT(&to->stack, &from->stack);
    }

    HAL_ENABLE_INTERRUPTS();
}

void task_lock(void)
{
    ++sched_lock;
}

void task_unlock(void)
{
    extern void handle_pending_hsrs(void);
    int32_t lock = sched_lock - 1;
    if (lock <= 0) {
        handle_pending_hsrs();
        schedule();
    } else {
        sched_lock = lock;
    }
}

/*--------------------------------------------------------------------------*/

#ifdef TASK_STACK_CHECK
void task_stack_check(task_t *to)
{
    uint32_t sig = (uint32_t)to;
    uint32_t *base = (uint32_t *)task_get_stack_base(to);
    uint32_t *top = (uint32_t *)(to->stack_base + to->stack_size);

    BUG_ON((sizeof(addr_t) - 1) & (addr_t)base);
    BUG_ON((sizeof(addr_t) - 1) & (addr_t)top);
    BUG_ON(to->stack < (addr_t)base);
    BUG_ON(to->stack > (addr_t)top);

    for (int i = 0; i < TASK_STACK_CHECK_DATA_SIZE/sizeof(uint32_t); i++) {
        BUG_ON((sig ^ (i * 0x01010101)) != base[i]);
        BUG_ON((sig ^ (i * 0x10101010)) != top[i]);
    }
}
#endif

#ifdef TASK_STACK_MEASURE
uint32_t task_measure_stack_usage(task_t *task)
{
    uint32_t *base = (uint32_t *)task->stack_base;
    uint32_t size = task->stack_size/sizeof(uint32_t);
    int i;

    for (i = 0; i < size; i ++) {
        if (base[i] != 0xdeadbeaf)
            break;
    }

    return (size - i) * sizeof(uint32_t);
}
#endif

static void task_set_stack(task_t *task, addr_t s_base, uint32_t s_size)
{
#ifdef TASK_STACK_SIZE_MINIMUM
    BUG_ON(s_size < TASK_STACK_SIZE_MINIMUM);
#endif
#ifdef TASK_STACK_CHECK
    {
        uint32_t sig = (uint32_t)task;
        uint32_t *base = (uint32_t *)s_base;
        uint32_t *top = (uint32_t *)(s_base + s_size -
            TASK_STACK_CHECK_DATA_SIZE);
        int i;
        for (i = 0; i < TASK_STACK_CHECK_DATA_SIZE/sizeof(uint32_t); i++) {
            base[i] = (sig ^ (i * 0x01010101));
            top[i] = (sig ^ (i * 0x10101010));
        }
        BUG_ON(&base[i] >= &top[0]);
        s_base += i * sizeof(uint32_t);
        s_size -= i * sizeof(uint32_t) * 2;
        BUG_ON(s_size < 256);
    }
#endif
#ifdef TASK_STACK_MEASURE
    {
        uint32_t *base = (uint32_t *)s_base;
        uint32_t size = s_size/sizeof(uint32_t);
        int i;
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

void task_create(task_t *task, char *name, uint8_t priority, uint8_t flags,
    addr_t stack_base, uint32_t stack_size, task_entry_t entry, void *para)
{
    BUG_ON(priority >= SCHED_PRIORITY_MAX_NR);

    task_set_stack(task, stack_base, stack_size);

    task->priority = priority;
    task->default_priority = priority;
    task->state = TASK_SUSPEND;
    task->entry = entry;
    task->para = para;
    task->time_slice = 0;
    INIT_TIMER(&task->timer);
    INIT_LIST_HEAD(&task->run_node);
    INIT_LIST_HEAD(&task->list);
    task->name = name;

    LIST_ADD(&task_list, &task->list);
    ++task_count;

    if (flags & TICK_SCHED_FLAG) {
        task->flags |= TICK_SCHED_FLAG;
        task->time_slice = TICK_SCHED_QUANTUM;
    }

    if (flags & URGENT_FLAG)
        task->flags |= URGENT_FLAG;

    HAL_TASK_BUILD_STACK(&task->stack);

    if (flags & AUTO_START_FLAG)
        task_resume(task);
}

static void task_timeout(void *para)
{
    task_t *task = (task_t *)para;
    if (TASK_SUSPEND != task->state)
        return;

    BUG_ON(!TIMER_ACTIVE_TASK(task));
    BUG_ON(TIMER_ACTIVE(&task->timer));

    task->flags &= ~TIMER_ACTIVE_FLAG;

    if (NULL != task->cleanup)
        task->cleanup(task->cleanup_info);

    task_resume(task);
}

void task_suspend(task_t *task, int32_t ticks, cleanup_t cleanup, void *info)
{
    if (task->state != TASK_RUNNING)
        return;

    task_lock();

    if (ticks > 0) {
        task->flags |= TIMER_ACTIVE_FLAG;
        BUG_ON(TIMER_ACTIVE(&task->timer));
        timer_start(&task->timer, ticks, task_timeout, task);
    }

    task->cleanup = cleanup;
    task->cleanup_info = info;
    task->state = TASK_SUSPEND;
    rq_task_delete(&run_queue, task);

    task_unlock();
}

void task_resume(task_t *task)
{
    if (task->state != TASK_SUSPEND)
        return;

    task_lock();

    if (TIMER_ACTIVE_TASK(task)) {
        BUG_ON(!TIMER_ACTIVE(&task->timer));
        timer_stop(&task->timer);
        task->flags &= ~TIMER_ACTIVE_FLAG;
    }

    task->cleanup = NULL;
    task->cleanup_info = NULL;
    task->state = TASK_RUNNING;
    rq_task_add(&run_queue, task, URGENT_TASK(task));

    task_unlock();
}

/*--------------------------------------------------------------------------*/

void task_time_slice_hsr(void *data)
{
    task_t *task = (task_t *)data;

    BUG_ON(task != current);

    if (TICK_SCHED_TASK(task) && (task->time_slice <= 0)) {

        if (task->default_priority == SCHED_PRIORITY_MAX_NR - 1) {
            task->time_slice = TICK_SCHED_QUANTUM;
            rq_task_move_tail(&run_queue, task);
            return;
        }

        rq_task_delete(&run_queue, task);
        ++task->priority;
        if (task->priority >= (SCHED_PRIORITY_MAX_NR - 1)) {
            task->priority = task->default_priority;
            task->time_slice = TICK_SCHED_QUANTUM;
        } else {
            task->time_slice = TICK_SCHED_QUANTUM +
            (task->priority - task->default_priority) * TICK_SCHED_QUANTUM / 2;
        }
        rq_task_add(&run_queue, task, URGENT_TASK(task));
    }
}

static DECLARE_HSR(time_slice_hsr, 0, task_time_slice_hsr, "time_slice_hsr");

void task_time_slice(void)
{
    if (TICK_SCHED_TASK(current) && (--current->time_slice <= 0))
        activiate_hsr(&time_slice_hsr, current);
}

/*--------------------------------------------------------------------------*/

void task_yield(void)
{
    task_lock();

    if (TICK_SCHED_TASK(current))
        current->time_slice = TICK_SCHED_QUANTUM;

    rq_task_move_tail(&run_queue, current);

    task_unlock();
}

void task_sleep(int32_t ticks)
{
    if (ticks <= 0)
        return;
    task_suspend(current, ticks, NULL, NULL);
}

void task_exit(void)
{
    HAL_DISABLE_INTERRUPTS();
    rq_task_delete(&run_queue, current);
    current->state = TASK_ZOMBIE;
    BUG_ON(TIMER_ACTIVE(&current->timer));
    BUG_ON(TIMER_ACTIVE_TASK(current));
    BUG_ON(current->cleanup != NULL);
    ++task_switches;
    current = rq_pick_task();
#ifdef TASK_STACK_CHECK
    task_stack_check(current);
#endif
    HAL_LOAD_TASK_CONTEXT(&current->stack);
    HAL_ENABLE_INTERRUPTS();

    /* NEVER go here */
    BUG_ON(1);
}

void task_restart(task_t *task, uint8_t flags)
{
    if (task->state != TASK_ZOMBIE)
        return;

    task->stack = task->stack_base + task->stack_size;
    task->priority = task->default_priority;
    task->state = TASK_SUSPEND;
    task->flags = 0;

    if (flags & TICK_SCHED_FLAG) {
        task->flags |= TICK_SCHED_FLAG;
        task->time_slice = TICK_SCHED_QUANTUM;
    }

    if (flags & URGENT_FLAG)
        task->flags |= URGENT_FLAG;

    HAL_TASK_BUILD_STACK(&task->stack);

    if (flags & AUTO_START_FLAG)
        task_resume(task);
}

/*--------------------------------------------------------------------------*/

static TASK_UNION(idle_task, IDLE_TASK_STACK_SIZE);

static void idle_task_entry(void *para)
{
    while (1);
}

static inline void init_idle_task(void)
{
    task_union_create(&idle_task, "idle_task", SCHED_PRIORITY_MAX_NR - 1,
        AUTO_START_FLAG, IDLE_TASK_STACK_SIZE, idle_task_entry, NULL);
}

/*--------------------------------------------------------------------------*/

void init_sched(void)
{
    for (int i = 0; i < SCHED_PRIORITY_MAX_NR; i++)
        INIT_LIST_HEAD(run_queue.queue_array + i);
    run_queue.highest_priority = SCHED_PRIORITY_MAX_NR - 1;
    INIT_LIST_HEAD(&task_list);
    init_idle_task();
}

void start_sched(void)
{
    sched_lock = 0;
    current = rq_pick_task();
    BUG_ON(NULL == current);
    HAL_LOAD_TASK_CONTEXT(&current->stack);
    BUG_ON(1); /* never go here */
}

void task_entry_wrapper(void)
{
    current->entry(current->para);
    task_exit();
}

/*--------------------------------------------------------------------------*/
// EOF task.c
