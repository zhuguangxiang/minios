/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#include "os/task.h"
#include "hal/s3c2440/s3c2440_io.h"
#include "hal/s3c2440/s3c2440_regs.h"
#include "hal/s3c2440/s3c2440_interrupt.h"

void printf(const char *fmt);

/*
    1(1s), 10(100ms), 20(50ms), 50(20ms), 100(10ms),
    200(5ms), 250(4ms), 500(2ms), 1000(1ms)
 */
#define OS_HZ 100

static inline int32_t ms_to_ticks(int32_t ms)
{
    int32_t ticks;

    if (ms <= 0)
        return 0;

    if (ms * OS_HZ <= 1000) /* ms <= 1000/OS_HZ */
        ticks = 1;
    else
        ticks = (ms * OS_HZ) / 1000; /* ms / (1000/OS_HZ) */

    return ticks;
}

static inline int32_t second_to_ticks(int32_t second)
{
    return ms_to_ticks(1000*second);
}

static inline int32_t minute_to_ticks(int32_t minute)
{
    return second_to_ticks(60*minute);
}

static inline int32_t hour_to_ticks(int32_t hour)
{
    return minute_to_ticks(60*hour);
}

#define S3C2440_IRQ_TIMER0  10
#define S3C2440_Enable_IRQ s3c2440_enable_irq
#define S3C2440_Disable_IRQ s3c2440_disable_irq
#define S3C2440_Clear_IRQ s3c2440_clear_irq

void tick_increase(void);
timer_t test_timer = TIMER_INIT(test_timer);
void test_timer_func(void *data)
{
	extern volatile uint32_t jiffies;
	uint32_t tick = jiffies;
	while (1) {
		printf("test_timer\r\n");
		if (tick < jiffies)
			break;
	}
}

void S3C2440_Do_Timer_Interrupt(int irq, void *data)
{
    S3C2440_Disable_IRQ(irq);
    S3C2440_Clear_IRQ(irq);
    tick_increase();
    S3C2440_Enable_IRQ(irq);
}

bool_t S3C2440_Init_Timer(void)
{
    uint32_t tmp;

    register_irq(S3C2440_IRQ_TIMER0, S3C2440_Do_Timer_Interrupt, NULL);

    tmp = READ_REG(TCFG0);
    tmp |= 199; /* prescaler=199 */
    WRITE_REG(TCFG0, tmp);

    tmp = READ_REG(TCFG1);
    tmp |= 1; /* divider = 1/4 */
    WRITE_REG(TCFG1, tmp);

    tmp = (62500 / OS_HZ); /* timer count */
    WRITE_REG(TCNTB0, tmp);

    /* manual update bit */
    tmp = READ_REG(TCON);
    tmp |= 2;
    WRITE_REG(TCON, tmp);

    /* start timer0, auto-reload and clear manual update bit */
    tmp = READ_REG(TCON);
    tmp |= 9;
    tmp &= ~2;
    WRITE_REG(TCON, tmp);

    /* enable irq */
    S3C2440_Enable_IRQ(S3C2440_IRQ_TIMER0);

    return TRUE;
}

#define UAR0_TX_FIFO_FULL()  ((READ_REG(UFSTAT0) & 0x4000) ? 1 : 0)

void put_char(char ch)
{
    /* wait for room for tx */
    while (UAR0_TX_FIFO_FULL());
    WRITE_REG(UTXHB0, ch); /* big-endian */
}

void printf(const char *fmt)
{
    while (*fmt) {
        put_char (*fmt++);
    }
}

static DEFAULT_TASK_STRUCT(test_task_1);
static DEFAULT_TASK_STRUCT(test_task_2);

void test_task_1_entry(void *p)
{
    while (1) {
        printf("test_task_1\r\n");
        //task_sleep(1);
        //task_struct_resume(&test_task_2, 0);
		//task_yield();
    }
}

void test_task_2_entry(void *p)
{
	int count = 0;
    while (1) {
        printf("test_task_2\r\n");
		//task_sleep(1);
        //task_struct_resume(&test_task_1, 0);
        //task_sleep(1);
	    //task_yield();
		++count;
		if (count > 10000000) {
			timer_start(&test_timer, 100, test_timer_func, 0);
			task_exit();
		}
    }
}

void application_start(void)
{
	S3C2440_Init_Timer();

    task_struct_create(&test_task_1, "test_task_1", 8, TICK_SCHED_FLAG,
        test_task_1_entry, NULL);

    task_struct_create(&test_task_2, "test_task_2", 8, TICK_SCHED_FLAG,
        test_task_2_entry, NULL);
}

/*--------------------------------------------------------------------------*/
// EOF app.c
