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

STATIC INLINE VOID init_mem_pool_block_map(MEM_POOL *pool)
{
    UINT32 i;

    for (i = 0; i < pool->block_total_count; i++)
        pool->block_start[i] = i + 1;
    pool->block_start[pool->block_total_count - 1] = POOL_BLOCK_END;
}

VOID mem_pool_init(MEM_POOL *pool, VOID *mem_start, UINT32 mem_size,
    UINT32 block_size, INT wait_type)
{
    BUG_ON(mem_size < 4);
    BUG_ON(mem_size < block_size);
    BUG_ON(mem_size % block_size);
    BUG_ON(mem_size/block_size > (~POOL_BLOCK_END + 1));

    pool->mem_start = mem_start;
    pool->mem_size = mem_size;

    pool->block_start = mem_start;
    pool->block_size = block_size;
    pool->free_block_index = 0;

    pool->block_total_count = mem_size/block_size;
    pool->block_free_count = pool->block_total_count;

    init_mutex(&pool->mm_mutex, MUTEX_PROTOCOL_INHERIT, wait_type);

    init_mem_pool_block_map(pool);
}

VOID *mem_pool_alloc(MEM_POOL *pool)
{
    VOID *block = NULL;

    mutex_lock(&pool->mm_mutex);

    if (pool->free_block_index != POOL_BLOCK_END) {
        /* memory available */
        block = (UINT8 *)pool->block_start +
                pool->free_block_index * pool->block_size;
        pool->free_block_index = pool->block_start[pool->free_block_index];
        --pool->block_free_count;
    }

    mutex_unlock(&pool->mm_mutex);

    return block;
}

VOID mem_pool_free(MEM_POOL *pool, VOID *block)
{
    UINT32 block_index;

    BUG_ON(((UINT8 *)block < (UINT8 *)pool->block_start) ||
        ((UINT8 *)block > (UINT8 *)pool->block_start +
            pool->block_size * pool->block_total_count));

    mutex_lock(&pool->mm_mutex);

    block_index = ((UINT8 *)block - (UINT8 *)pool->block_start) /
                  pool->block_size;
    pool->block_start[block_index] = pool->free_block_index;
    pool->free_block_index = block_index;
    ++pool->block_free_count;

    mutex_unlock(&pool->mm_mutex);
}

/******************************************************************************/
