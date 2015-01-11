/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#include "common/stdio.h"
#include "kernel/mutex.h"

union {
	TASK _task;
	char _stack[8192];
} test_mutex_task_1, test_mutex_task_2, test_mutex_task_3;

MUTEX test_mutex = MUTEX_INITIALIZER(test_mutex, MUTEX_PROTOCOL_INHERIT);

void test_mutex_task_1_entry(void *p)
{
	int count = 0;
    while (1) {
        mutex_lock(&test_mutex);
        printf("%s\r\n", current->name);
		mutex_unlock(&test_mutex);
		count++;
		if (count > 10000)
			task_exit();
    }
}

void test_mutex_task_2_entry(void *p)
{
    while (1) {
		task_sleep(10);
        mutex_lock(&test_mutex);
        printf("%s\r\n", current->name);
		mutex_unlock(&test_mutex);
    }
}

void test_mutex_task_3_entry(void *p)
{
    while (1) {
		task_sleep(5);
        mutex_lock(&test_mutex);
        printf("%s\r\n", current->name);
		mutex_unlock(&test_mutex);
    }
}

void app_test_mutex(void)
{
    TASK_PARA task_para = {
        .name = "test_mutex_task_1",
        .priority = 8,
        .flags = TASK_FLAGS_TICK_SCHED,
        .entry = test_mutex_task_1_entry,
        .para = NULL,
        .stack_base = (ADDRESS)&test_mutex_task_1 + sizeof(TASK),
        .stack_size = 8192 - sizeof(TASK),
    };

    task_create((TASK *)&test_mutex_task_1, &task_para, 1);

    task_para.name = "test_mutex_task_2";
	task_para.entry = test_mutex_task_2_entry;
	task_para.priority = 6;
	task_para.stack_base = (ADDRESS)&test_mutex_task_2 + sizeof(TASK);

    task_create((TASK *)&test_mutex_task_2, &task_para, 1);

    task_para.name = "test_mutex_task_3";
	task_para.entry = test_mutex_task_3_entry;
	task_para.priority = 7;
	task_para.stack_base = (ADDRESS)&test_mutex_task_3 + sizeof(TASK);

    task_create((TASK *)&test_mutex_task_3, &task_para, 1);

}

/*--------------------------------------------------------------------------*/
// EOF test_mutex.c
