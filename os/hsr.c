/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "os/hsr.h"
#include "port/port.h"

#if ((HSR_PRIORITY_MAX_NR <= 0) || (HSR_PRIORITY_MAX_NR > 1024))
#error HSR_PRIORITY_MAX_NR is out of range(1 ~ 32)
#endif

static uint32_t hsr_bitmap;
static list_head_t hsr_array[HSR_PRIORITY_MAX_NR];

void handle_pending_hsrs(void)
{
    extern int32_t sched_lock;
    int nr;
    list_head_t *list, *node;
    hsr_t *hsr;

    if (sched_lock > 0)
        return;

    // just only to prevent hisrs to schedule
    ++sched_lock;

    while (1) {
        nr = HAL_FIND_FIRST_SET(hsr_bitmap);
        if (nr < 0)
            break;

        list = hsr_array + nr;
        node = LIST_FIRST(list);
        BUG_ON(NULL == node);
        hsr = LIST_ENTRY(node, hsr_t, node);
        hsr->function(hsr->data);

        HAL_DISABLE_INTERRUPTS();
        --hsr->count;
        if (hsr->count <= 0)
            LIST_DEL(node);
        if (LIST_EMPTY(list))
            hsr_bitmap &= ~(1 << nr);
        HAL_ENABLE_INTERRUPTS();
    }

    --sched_lock;
}

void activiate_hsr(hsr_t *hsr, void *data)
{
    BUG_ON(hsr->priority >= HSR_PRIORITY_MAX_NR);

    ++hsr->count;
    hsr->data = data;

    if (!LIST_INLIST(&hsr->node)) {
        LIST_ADD_TAIL(hsr_array + hsr->priority, &hsr->node);
        hsr_bitmap |= (1 << hsr->priority);
    }
}

void init_hsr(void)
{
    for (int i = 0; i < HSR_PRIORITY_MAX_NR; i++)
        INIT_LIST_HEAD(hsr_array + i);
}

/*--------------------------------------------------------------------------*/
// EOF hisr.c
