/**MOD+************************************************************************/
/* Module:  mem_pool.c                                                        */
/*                                                                            */
/* Purpose: fixed block memory pool of MiniOS                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "mm/mem_pool.h"
#include "common/bug.h"
#include "kernel/task.h"

#if (MEM_POOL_TWICE_FREE_CHECK > 0)

STATIC VOID mem_pool_twice_free_check(MEM_POOL *pool, UINT32 blk_idx)
{
    UINT32 free_idx = pool->blk_free;

    BUG_ON(blk_idx >= pool->blk_num);

    while (free_idx != pool->blk_num) {
        BUG_ON(free_idx == blk_idx);
        free_idx = pool->blk_map[free_idx];
    }
}

#endif

STATIC INLINE VOID init_mem_pool_map(MEM_POOL *pool)
{
    for (INT i = 0; i < pool->blk_num; i++)
        pool->blk_map[i] = i + 1;
}

VOID mem_pool_init(MEM_POOL *pool, VOID *start, UINT32 size, UINT32 *blk_map,
                   UINT32 blk_size, INT wait)
{
    BUG_ON(size < 4);
    BUG_ON(size < blk_size);
    BUG_ON(size % blk_size);

    pool->start     = start;
    pool->size      = size;
    pool->blk_map   = blk_map;
    pool->blk_size  = blk_size;
    pool->blk_num   = size/blk_size;
    pool->blk_inuse = 0;
    pool->blk_free  = 0;

    init_mutex(&pool->lock, MUTEX_PROTOCOL_INHERIT, wait);

    init_mem_pool_map(pool);
}

VOID *mem_pool_alloc(MEM_POOL *pool)
{
    VOID *block = NULL;

    mutex_lock(&pool->lock);

    if (pool->blk_free != pool->blk_num) {
        /* memory available */
        block = pool->start + pool->blk_free * pool->blk_size;
        pool->blk_free = pool->blk_map[pool->blk_free];
        ++pool->blk_inuse;
    }

    mutex_unlock(&pool->lock);

    return block;
}

VOID mem_pool_free(MEM_POOL *pool, VOID *b)
{
    UINT32 idx;

    BUG_ON((UINT8 *)b < pool->start);
    BUG_ON((UINT8 *)b > (pool->start + pool->size));
    BUG_ON(((UINT8 *)b - pool->start) % pool->blk_size);

    mutex_lock(&pool->lock);

    idx = ((UINT8 *)b - pool->start) / pool->blk_size;

#if (MEM_POOL_TWICE_FREE_CHECK > 0)
    mem_pool_twice_free_check(pool, idx);
#endif

    pool->blk_map[idx] = pool->blk_free;
    pool->blk_free = idx;
    --pool->blk_inuse;

    mutex_unlock(&pool->lock);
}

/******************************************************************************/
