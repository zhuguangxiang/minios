/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _MINIOS_HSR_H_
#define _MINIOS_HSR_H_

#include "os/minios_type.h"
#include "os/list.h"

#define HSR_PRIORITY_MAX_NR 8

typedef void (*hsr_func_t)(void *);

typedef struct {
    list_head_t node;
    hsr_func_t function;
    void *data;
    int32_t count;
    uint8_t priority;
    char *desc;
} hsr_t;

#define DECLARE_HSR(name, prio, func, desc) \
    hsr_t name = {LIST_HEAD_INIT((name).node), func, NULL, 0, prio, desc}

void activiate_hsr(hsr_t *, void *);

#endif // _MINIOS_HSR_H_

/*--------------------------------------------------------------------------*/
// EOF hsr.h
