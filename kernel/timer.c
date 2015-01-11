/**MOD+************************************************************************/
/* Module:  timer.c                                                           */
/*                                                                            */
/* Purpose: Soft timer implementation of MiniOS                               */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/timer.h"
#include "kernel/hsr.h"
#include "hal/port.h"
#include "common/bug.h"

/******************************************************************************/
/* External functions & variables                                             */
/******************************************************************************/
VOID task_time_slice_proc(VOID);
VOID sched_lock(VOID);
VOID sched_unlock(VOID);

/******************************************************************************/
/* Local variables                                                            */
/******************************************************************************/
volatile ULONG jiffies;
STATIC LQE timer_active_list;

/**PROC+***********************************************************************/
/* Name:     mod_init_timer                                                   */
/*                                                                            */
/* Purpose:  Timer management module initialization                           */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
static VOID mod_init_timer(VOID)
{
    jiffies = 0xffffd000;
    init_list(&timer_active_list);
    HAL_TIMER_INIT();
}

MOD_INIT_CALL(mod_init_timer, MOD_TIMER_LVL);

/**PROC+***********************************************************************/
/* Name:     handle_timer_active_list                                         */
/*                                                                            */
/* Purpose:  Hardware tick HSR                                                */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID handle_timer_active_list(VOID)
{
    LQE *pos, *nxt;
    TIMER *t;

    LIST_FOR_EACH_SAFE(pos, nxt, &timer_active_list) {
        t = LIST_ENTRY(pos, TIMER, tm_node);
        if (time_before(jiffies, t->tm_expires))
            break;
        list_del(pos);
        t->tm_proc(t->tm_data);
    }
}

/**PROC+***********************************************************************/
/* Name:     tick_hsr_function                                                */
/*                                                                            */
/* Purpose:  Hardware tick HSR                                                */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN data - user data                                              */
/*                                                                            */
/**PROC-***********************************************************************/
VOID tick_hsr_function(VOID *data)
{
    handle_timer_active_list();
    task_time_slice_proc();
}

/******************************************************************************/
/* Tick HSR define                                                            */
/******************************************************************************/
STATIC HSR tick_hsr = HSR_INIT(tick_hsr, 0, tick_hsr_function, "tick_hsr");

/**PROC+***********************************************************************/
/* Name:     tick_increase                                                    */
/*                                                                            */
/* Purpose:  Hardware tick ISR                                                */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID tick_increase(VOID)
{
    ++jiffies;
    activiate_hsr(&tick_hsr, NULL);
}

/**PROC+***********************************************************************/
/* Name:     timer_start                                                      */
/*                                                                            */
/* Purpose:  start an app timer                                               */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN timer  - timer control block                                  */
/*           IN ticks  - timer ticks                                          */
/*           IN proc   - timer timeout callback function                      */
/*           IN data   - user data                                            */
/*                                                                            */
/**PROC-***********************************************************************/
VOID timer_start(TIMER *timer, LONG ticks, TIMEOUT_PROC proc, VOID *data)
{
    LQE *pos;
    TIMER *t;

    BUG_ON(TIMER_ACTIVE(timer));
    timer->tm_proc = proc;
    timer->tm_data = data;
    timer->tm_expires = jiffies + ticks;

    sched_lock();

    LIST_FOR_EACH(pos, &timer_active_list) {
        t = LIST_ENTRY(pos, TIMER, tm_node);
        if (time_after(t->tm_expires, timer->tm_expires))
            break;
    }

    __list_add(&timer->tm_node, pos->prev, pos);

    sched_unlock();
}

/**PROC+***********************************************************************/
/* Name:     timer_stop                                                       */
/*                                                                            */
/* Purpose:  stop an app timer before timeout                                 */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN timer  - timer control block                                  */
/*                                                                            */
/**PROC-***********************************************************************/
VOID timer_stop(TIMER *timer)
{
    sched_lock();

    if (lqe_in_list(&timer->tm_node))
        list_del(&timer->tm_node);

    sched_unlock();
}

/******************************************************************************/
// EOF timer.c
