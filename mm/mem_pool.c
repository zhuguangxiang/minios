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

STATIC INLINE VOID init_mem_pool_map(MEM_POOL *pool)
{
    UINT32 i;
    UINT32 *map;

    for (i = 0; i < pool->blk_num; i++) {
        map = (UINT32 *)(pool->start + pool->blk_size * i);
        *map = i + 1;
    }

    /* last block */
    map = (UINT32 *)(pool->start + pool->blk_size * (i - 1));
    *map = POOL_BLOCK_END;
}

VOID mem_pool_init(MEM_POOL *pool, VOID *start, UINT32 size,
                   UINT32 blk_size, INT wait)
{
    BUG_ON(size < 4);
    BUG_ON(size < blk_size);
    BUG_ON(size % blk_size);
    BUG_ON(size / blk_size > (~POOL_BLOCK_END + 1));

    pool->start     = start;
    pool->size      = size;
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

    if (pool->blk_free != POOL_BLOCK_END) {
        /* memory available */
        block = pool->start + pool->blk_free * pool->blk_size;
        pool->blk_free = *(UINT32 *)block;
        ++pool->blk_inuse;
    }

    mutex_unlock(&pool->lock);

    return block;
}

VOID mem_pool_free(MEM_POOL *pool, VOID *b)
{
    UINT32 idx;
    UINT32 *map;

    BUG_ON((UINT8 *)b < pool->start);
    BUG_ON((UINT8 *)b > (pool->start + pool->size));
    BUG_ON(((UINT8 *)b - pool->start) % pool->blk_size);

    mutex_lock(&pool->lock);

    idx = ((UINT8 *)b - pool->start) / pool->blk_size;
    map = (UINT32 *)(pool->start + pool->blk_size * idx);
    *map = pool->blk_free;
    pool->blk_free = idx;
    --pool->blk_inuse;

    mutex_unlock(&pool->lock);
}

/******************************************************************************/
