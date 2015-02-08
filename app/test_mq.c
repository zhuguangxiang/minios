/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#include "common/stdio.h"
#include "kernel/msg_queue.h"
#include "kernel/task.h"
#include "common/string.h"

union {
	TASK _task;
	char _stack[8192];
} test_mq_task_1, test_mq_task_2, test_mq_task_3;

UINT8 test_mq_buf[1024];

MSG_Q test_mq;

void test_mq_task_1_entry(void *p)
{
	char *msg = "hello msg, from task1";
	int count = 0;
    while (1) {
		task_sleep(10);
        printf("[MSGQ-SEND]%s\r\n", current->name);
		mq_send_msg(&test_mq, msg, strlen(msg) + 1, WAIT_FOREVER);
		count++;
		if (count > 10000)
			task_exit();
    }
}

void test_mq_task_2_entry(void *p)
{
	char msg[64];
	UINT32 actual_len;

    while (1) {
		task_sleep(10);
        mq_recv_msg(&test_mq, msg, sizeof(msg), &actual_len, WAIT_FOREVER);
        printf("[MSGQ-RECV]%s, %s\r\n", current->name, msg);
    }
}

void test_mq_task_3_entry(void *p)
{
	char msg[64];
	UINT32 actual_len;

    while (1) {
		task_sleep(10);
        mq_recv_msg(&test_mq, msg, sizeof(msg), &actual_len, WAIT_FOREVER);
        printf("[MSGQ-RECV]%s, %s\r\n", current->name, msg);
    }
}

void app_test_mq(void)
{
	init_mq(&test_mq, test_mq_buf, sizeof(test_mq_buf), 64, WQ_TYPE_FIFO);

    TASK_PARA task_para = {
        .name = "test_mq_task_1",
        .priority = 8,
        .flags = TASK_FLAGS_TICK_SCHED,
        .entry = test_mq_task_1_entry,
        .para = NULL,
        .stack_base = (ADDRESS)&test_mq_task_1 + sizeof(TASK),
        .stack_size = 8192 - sizeof(TASK),
    };

    task_create((TASK *)&test_mq_task_1, &task_para, 1);

    task_para.name = "test_mq_task_2";
	task_para.entry = test_mq_task_2_entry;
	task_para.priority = 6;
	task_para.stack_base = (ADDRESS)&test_mq_task_2 + sizeof(TASK);

    task_create((TASK *)&test_mq_task_2, &task_para, 1);

    task_para.name = "test_mq_task_3";
	task_para.entry = test_mq_task_2_entry;
	task_para.priority = 7;
	task_para.stack_base = (ADDRESS)&test_mq_task_3 + sizeof(TASK);

    task_create((TASK *)&test_mq_task_3, &task_para, 1);

}

/*--------------------------------------------------------------------------*/
// EOF test_sema.c
