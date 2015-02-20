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
#include "common/string.h"

#if (MEM_POOL_TWICE_FREE_CHECK > 0)

STATIC VOID mem_pool_twice_free_check(MEM_POOL *pool, UINT32 blk_idx)
{
    UINT32 free_idx = pool->blk_free;

    BUG_ON(blk_idx >= pool->blk_num);

    while (free_idx != pool->blk_lfree) {
        BUG_ON(blk_idx == free_idx);
        free_idx = pool->blk_map[free_idx];
    }

    BUG_ON(blk_idx == pool->blk_lfree);
}

#endif

#if (MEM_POOL_WILD_POINTER_CHECK > 0)

STATIC VOID mem_pool_wild_pointer_check(MEM_POOL *pool)
{
    if (pool->blk_lfree != BLOCK_END) {
        UINT8 *p = pool->start + pool->blk_size * pool->blk_lfree;
        for (INT i = 0; i < pool->blk_size; i++)
            BUG_ON(p[i] != BLOCK_BYTE_MAGIC);
    }
}

#endif

STATIC INLINE VOID init_mem_pool_map(MEM_POOL *pool)
{
    INT i;

    for (i = 0; i < pool->blk_num; i++)
        pool->blk_map[i] = i + 1;

    /* last block*/
    pool->blk_map[i - 1] = BLOCK_END;
}

VOID mem_pool_init(MEM_POOL *pool, VOID *start, UINT32 size, UINT32 *blk_map,
                   UINT32 blk_size, INT wait)
{
    BUG_ON(size < 4);
    BUG_ON(size < blk_size);
    BUG_ON(size % blk_size);
    BUG_ON((size / blk_size) > (~BLOCK_END + 1));

    pool->start     = start;
    pool->size      = size;
    pool->blk_map   = blk_map;
    pool->blk_size  = blk_size;
    pool->blk_num   = size/blk_size;
    pool->blk_inuse = 0;
    pool->blk_free  = 0;
    pool->blk_lfree = pool->blk_num - 1;

    init_mem_pool_map(pool);
    init_mutex(&pool->lock, MUTEX_PROTOCOL_INHERIT, wait);

#if (MEM_POOL_WILD_POINTER_CHECK > 0)

    memset(pool->start, BLOCK_BYTE_MAGIC, pool->size);

#endif

}

VOID *mem_pool_alloc(MEM_POOL *pool)
{
    VOID *block = NULL;

    BUG_ON(pool->blk_lfree > BLOCK_END);
    BUG_ON(pool->blk_free > BLOCK_END);

    mutex_lock(&pool->lock);

#if (MEM_POOL_WILD_POINTER_CHECK > 0)
    mem_pool_wild_pointer_check(pool);
#endif

    if (pool->blk_free != BLOCK_END) {
        /* queue is available */
        BUG_ON(pool->blk_lfree == BLOCK_END);

        block = pool->start + pool->blk_free * pool->blk_size;
        pool->blk_free = pool->blk_map[pool->blk_free];
        ++pool->blk_inuse;

        /* update queue tail if queue is empty */
        if (pool->blk_free == BLOCK_END)
            pool->blk_lfree = BLOCK_END;
    } else {
        /* queue is empty */
        BUG_ON(pool->blk_lfree != BLOCK_END);
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

    BUG_ON(pool->blk_lfree > BLOCK_END);
    BUG_ON(pool->blk_free > BLOCK_END);

    mutex_lock(&pool->lock);

    idx = ((UINT8 *)b - pool->start) / pool->blk_size;
    BUG_ON(idx >= pool->blk_num);

#if (MEM_POOL_TWICE_FREE_CHECK > 0)
    mem_pool_twice_free_check(pool, idx);
#endif

#if (MEM_POOL_WILD_POINTER_CHECK > 0)
    mem_pool_wild_pointer_check(pool);
#endif

    if (pool->blk_lfree != BLOCK_END) {
        /* queue is partial */
        BUG_ON(pool->blk_map[pool->blk_lfree] != BLOCK_END);

        pool->blk_map[pool->blk_lfree] = idx;
        pool->blk_lfree = idx;

    } else {
        /* queue is empty */
        BUG_ON(pool->blk_free != BLOCK_END);
        pool->blk_free = pool->blk_lfree = idx;
    }

#if (MEM_POOL_WILD_POINTER_CHECK > 0)
    memset(b, BLOCK_BYTE_MAGIC, pool->blk_size);
#endif

    pool->blk_map[idx] = BLOCK_END;
    --pool->blk_inuse;

    mutex_unlock(&pool->lock);
}

/******************************************************************************/
