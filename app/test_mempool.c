/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#include "mm/mem_pool.h"

MEM_POOL p1;
UINT32 mem1[4];
UINT32 p1_map[4];

MEM_POOL p2;
UINT32 mem2[16];
UINT32 p2_map[4];

void test_mem_pool(void)
{
	void *b1;
	void *b2;
	void *b3;
	void *b4;
	void *b5;
	mem_pool_init(&p1, mem1, sizeof(mem1), p1_map, 4, WQ_TYPE_FIFO);

	b1 = mem_pool_alloc(&p1);
	b2 = mem_pool_alloc(&p1);
	b3 = mem_pool_alloc(&p1);
	b4 = mem_pool_alloc(&p1);
	b5 = mem_pool_alloc(&p1);

	mem_pool_free(&p1, b2);
	mem_pool_free(&p1, b4);
	mem_pool_free(&p1, b1);
	mem_pool_free(&p1, b3);
	
    mem_pool_init(&p2, mem2, sizeof(mem2), p2_map, 16, WQ_TYPE_FIFO);

	b1 = mem_pool_alloc(&p2);
	b2 = mem_pool_alloc(&p2);
	b3 = mem_pool_alloc(&p2);
	b4 = mem_pool_alloc(&p2);
	b5 = mem_pool_alloc(&p2);

	mem_pool_free(&p2, b2);
	mem_pool_free(&p2, b4);
	mem_pool_free(&p2, b1);
	mem_pool_free(&p2, b3);
}

/*--------------------------------------------------------------------------*/
// EOF test_mempool.c
