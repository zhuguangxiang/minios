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

void app_test_mutex(void);
void app_test_sem(void);
void app_test_mq(void);
void test_mem_pool(void);
void test_fs(void);

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

VOID s3c2440_do_usbd_interrupt(INT irq, VOID *data)
{
    s3c2440_disable_irq(irq);
    
    s3c2440_clear_irq(irq);
    s3c2440_enable_irq(irq);
}

void usbd_gpio_c_5_set(void)
{
    UINT32 tmp = READ_REG(GPCCON);
    tmp |= (1 << 10);
    WRITE_REG(GPCCON, tmp);

    tmp = READ_REG(GPCDAT);
    tmp |= (1 << 5);
    WRITE_REG(GPCDAT, tmp);

    register_irq(S3C2440_IRQ_USBD, s3c2440_do_usbd_interrupt, NULL);
    s3c2440_enable_irq(S3C2440_IRQ_USBD);
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

	test_fs();
    //usbd_gpio_c_5_set();
    //app_test_mutex();
    //app_test_sem();
    //app_test_mq();
	//test_mem_pool();
}

/*--------------------------------------------------------------------------*/
// EOF app.c
