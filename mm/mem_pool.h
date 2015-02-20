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
#include "config/config.h"

typedef struct {
    UINT8  *start;      /* pool memory start address */
    UINT32 size;        /* pool memory size */
    UINT32 *blk_map;    /* block map address */
    UINT32 blk_size;    /* block size */
    UINT32 blk_num;     /* block total number */

    UINT32 blk_inuse;   /* used block number */
    UINT32 blk_free;    /* free block index */
    UINT32 blk_lfree;   /* last free block index */
    MUTEX  lock;        /* pool lock */
} MEM_POOL;

#define BLOCK_END           0xFF000000
#define BLOCK_BYTE_MAGIC    0xc5

#define POOL_MAP(name, blk_num) UINT32 name[blk_num]

VOID mem_pool_init(MEM_POOL *pool, VOID *start, UINT32 size, UINT32 *blk_map,
                   UINT32 blk_size, INT wait);
VOID *mem_pool_alloc(MEM_POOL *pool);
VOID mem_pool_free(MEM_POOL *pool, VOID *block);

#endif /* _MEM_POOL_H_ */

/******************************************************************************/
