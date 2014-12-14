/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "os/timer.h"
#include "os/hsr.h"

extern void task_lock(void);
extern void task_unlock(void);

volatile uint32_t jiffies = 0xffffd000;
list_head_t timer_list = LIST_HEAD_INIT(timer_list);
bool_t timer_enabled = FALSE;

void timer_hsr_function(void *para)
{
    timer_t *t, *nxt;

    timer_enabled = FALSE;

    LIST_FOR_EACH_ENTRY_SAFE(t, nxt, &timer_list, node) {
        if (time_before(jiffies, t->expires))
            break;
        LIST_DEL(&t->node);
        t->proc(t->data);
    }

    if (!LIST_EMPTY(&timer_list))
        timer_enabled = TRUE;
}

static DECLARE_HSR(timer_hsr, 0, timer_hsr_function, "timer_hsr");

void timer_start(timer_t *timer, int32_t ticks, timeout_t proc, void *data)
{
    timer_t *t;

    BUG_ON(TIMER_ACTIVE(timer));

    timer->proc = proc;
    timer->data = data;
    timer->expires = jiffies + ticks;

    task_lock();

    timer_enabled = FALSE;

    LIST_FOR_EACH_ENTRY(t, &timer_list, node) {
        if (time_after(t->expires, timer->expires))
            break;
    }
    __list_add(&timer->node, t->node.prev, &t->node);

    timer_enabled = TRUE;

    task_unlock();
}

void timer_stop(timer_t *timer)
{
    task_lock();

    timer_enabled = FALSE;

    if (LIST_INLIST(&timer->node))
        LIST_DEL(&timer->node);

    if (!LIST_EMPTY(&timer_list))
        timer_enabled = TRUE;

    task_unlock();
}

void tick_increase(void)
{
    extern void task_time_slice(void);

    ++jiffies;

    if (TRUE == timer_enabled) {
        list_head_t *node = LIST_FIRST(&timer_list);
        timer_t *timer;

        if (node) {
            timer = LIST_ENTRY(node, timer_t, node);
            if (time_after_eq(jiffies, timer->expires))
                activiate_hsr(&timer_hsr, NULL);
        }
    }

    task_time_slice();
}

/*--------------------------------------------------------------------------*/
// EOF timer.c
