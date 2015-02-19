/**INC+************************************************************************/
/* Header:  mem_pool.h                                                        */
/*                                                                            */
/* Purpose: fixed block memory pool of MiniOS                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_MEM_POOL_H_
#define _MINIOS_MEM_POOL_H_

#include "common/types.h"
#include "kernel/mutex.h"

typedef struct {
    VOID *mem_start;
    UINT32 mem_size;

    UINT32 *block_start;
    UINT32 block_size;
    UINT32 free_block_index;

    UINT32 block_total_count;
    UINT32 block_free_count;

    MUTEX mm_mutex;
} MEM_POOL;

#define POOL_BLOCK_END   0xFF000000

VOID mem_pool_init(MEM_POOL *pool, VOID *mem_start, UINT32 mem_size,
    UINT32 block_size, INT wait_type);
VOID *mem_pool_alloc(MEM_POOL *pool);
VOID mem_pool_free(MEM_POOL *pool, VOID *block);

#endif /* _MEM_POOL_H_ */

/******************************************************************************/
