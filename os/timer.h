/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#ifndef _MINIOS_TIMER_H_
#define _MINIOS_TIMER_H_

#include "os/minios_type.h"
#include "os/list.h"

typedef void (*timeout_t)(void *);

typedef struct {
    list_head_t node;
    uint32_t expires;
    timeout_t proc;
    void *data;
} timer_t;

#define TIMER_INIT(name) {LIST_HEAD_INIT((name).node), 0, NULL, NULL}
#define INIT_TIMER(timer) do {      \
    INIT_LIST_HEAD(&(timer)->node); \
    (timer)->expires = 0;           \
    (timer)->proc = NULL;           \
    (timer)->data = NULL;           \
} while (0)

#define TIMER_ACTIVE(timer) LIST_INLIST(&(timer)->node)

void timer_start(timer_t *timer, int32_t ticks, timeout_t proc, void *data);
void timer_stop(timer_t *timer);
void tick_increase(void);

/*
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result.
 *
 */
#define time_after(a,b)     ((int32_t)(b) - (int32_t)(a) < 0)
#define time_before(a,b)    time_after(b,a)
#define time_after_eq(a,b)  ((int32_t)(a) - (int32_t)(b) >= 0)
#define time_before_eq(a,b) time_after_eq(b,a)

/*--------------------------------------------------------------------------*/
#endif // _MINIOS_TIMER_H_
// EOF timer.h
