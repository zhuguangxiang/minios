/**INC+************************************************************************/
/* Header:  config.h                                                          */
/*                                                                            */
/* Purpose: Configuration for the MiniOS                                      */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_CONFIG_H_
#define _MINIOS_CONFIG_H_

/******************************************************************************/
/* HAL_INTERRUPTS_STACK_SIZE - interrupt stack size                           */
/* If HAL_USE_INTERRUPT_STACK defined, start code will use interrupt stack.   */
/* Otherwise it will use HAL_STARTUP_STACK_SIZE                               */
/******************************************************************************/
#define HAL_INTERRUPTS_STACK_SIZE 8192
#define HAL_USE_INTERRUPT_STACK
#define HAL_STARTUP_STACK_SIZE    1024

/******************************************************************************/
/* printf buffer size                                                         */
/******************************************************************************/
#define PRINT_BUF_SIZE 256

/******************************************************************************/
/* Config of task stack check & measure                                       */
/******************************************************************************/
#define TASK_STACK_CHECK
#define TASK_STACK_CHECK_DATA_SIZE 32
#define TASK_STACK_MEASURE

/******************************************************************************/
/* Config of scheduler priority, HSR priority and time slice quantum.         */
/******************************************************************************/
#define SCHED_PRIORITY_MAX_NR 16
#define HSR_PRIORITY_MAX_NR   8
#define TICK_SCHED_QUANTUM    10

/******************************************************************************/
/* TASK_STACK_SIZE_MINIMUM - minimum of task stack size                       */
/******************************************************************************/
#define TASK_STACK_SIZE_MINIMUM 512

/******************************************************************************/
/* If TASK_STACK_SIZE_MINIMUM defined, IDLE_TASK_STACK_SIZE MUST be greater   */
/* than TASK_STACK_SIZE_MINIMUM.                                              */
/******************************************************************************/
#define IDLE_TASK_STACK_SIZE 4096

/******************************************************************************/
/* Clock HZ                                                                   */
/* CPU = 400MHz, AHB = 100MHz, APB = 50MHz                                    */
/* 1(1s), 10(100ms), 20(50ms), 50(20ms), 100(10ms)                            */
/* 200(5ms), 250(4ms), 500(2ms), 1000(1ms)                                    */
/******************************************************************************/
#define CLOCK_HZ 100

/******************************************************************************/
/* System initializing module sequence                                        */
/* See *.lds & MOD_INIT_CALL(fn, lvl)                                         */
/******************************************************************************/
#define MOD_SCHED_LVL    0
#define MOD_HSR_LVL      1
#define MOD_TIMER_LVL    2

#endif /* _MINIOS_CONFIG_H_ */

/******************************************************************************/
