/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "kernel/task.h"
#include "kernel/wait_queue.h"
#include "hal/s3c2440/s3c2440_io.h"
#include "hal/s3c2440/s3c2440_regs.h"
#include "hal/s3c2440/s3c2440_interrupt.h"
#include "common/stdio.h"

#define UAR0_TX_FIFO_FULL()  ((READ_REG(UFSTAT0) & 0x4000) ? 1 : 0)

void put_char(char ch)
{
    /* wait for room for tx */
    while (UAR0_TX_FIFO_FULL());
    WRITE_REG(UTXHB0, ch); /* big-endian */
}

void puts(const char *fmt)
{
    while (*fmt) {
        put_char (*fmt++);
    }
}


union {
	TASK _task;
	char _stack[8192];
} test_task_1, test_task_2, test_task_3;

WAIT_QUEUE wq = WAIT_QUEUE_INIT(wq, WQ_TYPE_PRIO);

INT wake_up_func(VOID *data1, VOID *data2)
{
	printf("wakeup %s\r\n", ((TASK*)data1)->name);
	return 0;
}

void test_task_1_entry(void *p)
{
    while (1) {
        printf("%s\r\n", current->name);
		sleep_on(&wq, 0, current);
        //task_sleep(1);
        //task_struct_resume(&test_task_2, 0);
		//task_yield();
    }
}

void test_task_2_entry(void *p)
{
	int count = 0;
    while (1) {
        printf("%s\r\n", current->name);
		//task_sleep(1);
        //task_struct_resume(&test_task_1, 0);
        //task_sleep(1);
	    //task_yield();
		++count;
		sleep_on(&wq, 0, current);
    }
}

void test_task_3_entry(void *p)
{
    TASK_PARA task_para = {
        .name = "test_task_2",
        .priority = 9,
        .flags = TASK_FLAGS_TICK_SCHED,
        .entry = test_task_2_entry,
        .para = NULL,
        .stack_base = (ADDRESS)&test_task_2 + sizeof(TASK),
        .stack_size = 8192 - sizeof(TASK),
    };

	task_create((TASK *)&test_task_2, &task_para, 1);

	int count = 0;
    while (1) {
        printf("%s\r\n", current->name);
		//task_sleep(1);
        //task_struct_resume(&test_task_1, 0);
        task_sleep(1);
	    //task_yield();
		++count;
		wake_up_all(&wq, wake_up_func, NULL);
    }
}

void app_start(void)
{
    TASK_PARA task_para = {
        .name = "test_task_1",
        .priority = 10,
        .flags = TASK_FLAGS_TICK_SCHED,
        .entry = test_task_1_entry,
        .para = NULL,
        .stack_base = (ADDRESS)&test_task_1 + sizeof(TASK),
        .stack_size = 8192 - sizeof(TASK),
    };

    task_create((TASK *)&test_task_1, &task_para, 1);

	task_para.name = "test_task_2";
	task_para.entry = test_task_2_entry;
	task_para.priority = 9;
	task_para.stack_base = (ADDRESS)&test_task_2 + sizeof(TASK);

	//task_create((TASK *)&test_task_2, &task_para, 1);

	task_para.name = "test_task_3";
	task_para.entry = test_task_3_entry;
	task_para.priority = 13;
	task_para.stack_base = (ADDRESS)&test_task_3 + sizeof(TASK);

	task_create((TASK *)&test_task_3, &task_para, 1);

	app_test_mutex();

}

/*--------------------------------------------------------------------------*/
// EOF app.c
