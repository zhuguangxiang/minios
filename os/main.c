/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

/* extern initialization functions */
void init_sched(void);
void init_hsr(void);
void init_mem(void);
void init_dev(void);
void init_fs(void);

/* extern start scheduler function */
void start_sched(void);

void os_init(void)
{
    init_sched();
    init_hsr();
}

void os_start(void)
{
    start_sched();
}

/*--------------------------------------------------------------------------*/
// EOF minios.c
