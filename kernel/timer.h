/**INC+************************************************************************/
/* Header:  timer.h                                                           */
/*                                                                            */
/* Purpose: Timer Management of MiniOS                                        */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_TIMER_H_
#define _MINIOS_TIMER_H_

#include "common/types.h"
#include "common/list.h"
#include "config/config.h"

/**STRUCT+*********************************************************************/
/* Structure: TIMER                                                           */
/*                                                                            */
/* Description: Timer control block                                           */
/**STRUCT-*********************************************************************/
typedef VOID (*TIMEOUT_PROC)(VOID *);
typedef struct timer {
    /**************************************************************************/
    /* Timer added to system timer_active_list                                */
    /**************************************************************************/
    LQE tm_node;

    /**************************************************************************/
    /* Timer expired ticks                                                    */
    /**************************************************************************/
    UINT32 tm_expires;

    /**************************************************************************/
    /* Timer expired to call this function                                    */
    /**************************************************************************/
    TIMEOUT_PROC tm_proc;

    /**************************************************************************/
    /* User data passed to TIMEOUT_PROC                                       */
    /**************************************************************************/
    VOID *tm_data;
} TIMER;

/******************************************************************************/
/* Macros to intialize TIMER                                                  */
/******************************************************************************/
#define TIMER_INIT(name) {LIST_INIT((name).tm_node), 0, NULL, NULL}

STATIC INLINE VOID init_timer(TIMER *timer)
{
    init_list(&timer->tm_node);
    timer->tm_expires = 0;
    timer->tm_proc = NULL;
    timer->tm_data = NULL;
}

/******************************************************************************/
/* Macros to test timer is active                                             */
/******************************************************************************/
#define TIMER_ACTIVE(timer) lqe_in_list(&(timer)->tm_node)

VOID timer_start(TIMER *timer, TICK_COUNT ticks, TIMEOUT_PROC proc, VOID *data);
VOID timer_stop(TIMER *timer);

/*
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result.
 *
 */
#define time_after(a,b)     ((INT32)(b) - (INT32)(a) < 0)
#define time_before(a,b)    time_after(b,a)
#define time_after_eq(a,b)  ((INT32)(a) - (INT32)(b) >= 0)
#define time_before_eq(a,b) time_after_eq(b,a)

/******************************************************************************/

STATIC INLINE INT32 ms_to_ticks(INT32 ms)
{
    INT32 ticks;

    if (ms <= 0)
        return 0;

    if (ms * CLOCK_HZ <= 1000) /* ms <= 1000/OS_HZ */
        ticks = 1;
    else
        ticks = (ms * CLOCK_HZ) / 1000; /* ms / (1000/OS_HZ) */

    return ticks;
}

STATIC INLINE INT32 second_to_ticks(INT32 second)
{
    return ms_to_ticks(1000*second);
}

STATIC INLINE INT32 minute_to_ticks(INT32 minute)
{
    return second_to_ticks(60*minute);
}

STATIC INLINE INT32 hour_to_ticks(INT32 hour)
{
    return minute_to_ticks(60*hour);
}

#endif /* _MINIOS_TIMER_H_ */

/******************************************************************************/
