/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#include "common/stdio.h"
#include "kernel/semaphore.h"
#include "kernel/task.h"

union {
	TASK _task;
	char _stack[8192];
} test_sema_task_1, test_sema_task_2, test_sema_task_3;

SEMA test_sema = SEMA_FIFO_INIT(test_sema, 1);

void test_sema_task_1_entry(void *p)
{
	int count = 0;
    while (1) {
        sem_obtain(&test_sema, WAIT_FOREVER);
        printf("[SEMA]%s\r\n", current->name);
		sem_release(&test_sema);
		count++;
		if (count > 10000)
			task_exit();
    }
}

void test_sema_task_2_entry(void *p)
{
    while (1) {
		task_sleep(5);
        sem_obtain(&test_sema, WAIT_FOREVER);
        printf("[SEMA]%s\r\n", current->name);
		sem_release(&test_sema);
    }
}

void test_sema_task_3_entry(void *p)
{
    while (1) {
		task_sleep(5);
        sem_obtain(&test_sema, WAIT_FOREVER);
        printf("[SEMA]%s\r\n", current->name);
		sem_release(&test_sema);
    }
}

void app_test_sema(void)
{
    TASK_PARA task_para = {
        .name = "test_sema_task_1",
        .priority = 8,
        .flags = TASK_FLAGS_TICK_SCHED,
        .entry = test_sema_task_1_entry,
        .para = NULL,
        .stack_base = (ADDRESS)&test_sema_task_1 + sizeof(TASK),
        .stack_size = 8192 - sizeof(TASK),
    };

    task_create((TASK *)&test_sema_task_1, &task_para, 1);

    task_para.name = "test_sema_task_2";
	task_para.entry = test_sema_task_2_entry;
	task_para.priority = 6;
	task_para.stack_base = (ADDRESS)&test_sema_task_2 + sizeof(TASK);

    task_create((TASK *)&test_sema_task_2, &task_para, 1);

    task_para.name = "test_sema_task_3";
	task_para.entry = test_sema_task_2_entry;
	task_para.priority = 7;
	task_para.stack_base = (ADDRESS)&test_sema_task_3 + sizeof(TASK);

    task_create((TASK *)&test_sema_task_3, &task_para, 1);

}

/*--------------------------------------------------------------------------*/
// EOF test_sema.c
