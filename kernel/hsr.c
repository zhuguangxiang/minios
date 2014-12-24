/**MOD+************************************************************************/
/* Module:  hsr.c                                                             */
/*                                                                            */
/* Purpose: High Service Routine used by interrupt                            */
/*          MiniOS supports ISR(diable irq) and HSR(enable irq)               */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "kernel/hsr.h"
#include "kernel/task.h"
#include "common/bug.h"
#include "hal/port.h"
#include "config/config.h"

/******************************************************************************/
/* Check HSR_PRIORITY_MAX_NR is valid or not                                  */
/******************************************************************************/
#if ((HSR_PRIORITY_MAX_NR <= 0) || (HSR_PRIORITY_MAX_NR > 32))
#error HSR_PRIORITY_MAX_NR is out of range(1 ~ 32)
#endif

/******************************************************************************/
/* Bitmap of hsr_array                                                        */
/******************************************************************************/
STATIC ULONG hsr_bitmap;

/******************************************************************************/
/* HSR priority queue                                                         */
/******************************************************************************/
STATIC LQE hsr_array[HSR_PRIORITY_MAX_NR];

/**PROC+***********************************************************************/
/* Name:     handle_pending_hsrs                                              */
/*                                                                            */
/* Purpose:  Handle pending HSRS called by sched_unlock                       */
/*           To call this function, sched_lock_count is coming to be 0.       */
/*           See sched_unlock how to call it                                  */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID handle_pending_hsrs(VOID)
{
    INT nr;
    LQE *list, *node;
    HSR *hsr;

    /* to prevent hsrs to schedule from interrupt */
    ++current->lock_count;

    while (1) {
        nr = HAL_FIND_FIRST_SET(hsr_bitmap);
        if (nr < 0) break;

        list = hsr_array + nr;
        node = list_first(list);
        BUG_ON(NULL == node);
        hsr = LIST_ENTRY(node, HSR, node);
        hsr->func(hsr->data);

        HAL_DISABLE_INTERRUPTS();
        --hsr->count;
        if (hsr->count <= 0) {
            list_del(node);
            if (list_empty(list))
                hsr_bitmap &= ~(1 << nr);
        }
        HAL_ENABLE_INTERRUPTS();
    }

    --current->lock_count;
}

/**PROC+***********************************************************************/
/* Name:     isr_handle_pending_hsrs                                          */
/*                                                                            */
/* Purpose:  Handle pending HSRS called by ISR                                */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
VOID isr_handle_pending_hsrs(VOID)
{
    BUG_ON(current->lock_count < 0);

    if (current->lock_count == 0) {
        handle_pending_hsrs();
    }
}

/**PROC+***********************************************************************/
/* Name:     activiate_hsr                                                    */
/*                                                                            */
/* Purpose:  activiate HSR called by ISR                                      */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN hsr   - HSR control block                                     */
/*           IN data  - HSR user data                                         */
/*                                                                            */
/**PROC-***********************************************************************/
VOID activiate_hsr(HSR *hsr, VOID *data)
{
    BYTE priority = hsr->priority;

    BUG_ON(priority >= HSR_PRIORITY_MAX_NR);

    ++hsr->count;

    if (!lqe_in_list(&hsr->node)) {
        list_add_tail(&hsr->node, hsr_array + priority);
        hsr_bitmap |= (1 << priority);
    }
}

/**PROC+***********************************************************************/
/* Name:     mod_init_hsr                                                     */
/*                                                                            */
/* Purpose:  initialize HSR module                                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   None                                                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC VOID mod_init_hsr(VOID)
{
    for (INT i = 0; i < HSR_PRIORITY_MAX_NR; i++)
        init_list(hsr_array + i);
}

MOD_INIT_CALL(mod_init_hsr, MOD_HSR_LVL);

/******************************************************************************/
// EOF hsr.c
