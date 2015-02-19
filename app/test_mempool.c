/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#include "mm/mem_pool.h"

MEM_POOL pool;
UINT32 mem[16];

void test_mem_pool(void)
{
	mem_pool_init(&pool, mem, sizeof(mem), 4, WQ_TYPE_FIFO);
}

/*--------------------------------------------------------------------------*/
// EOF test_mempool.c
