/******************************************************************************
File    :  main.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		main for C code

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-13      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"
#include "drivers.h"
#include "etos_arm.h"
#include "build_time.h"  /*it is generated at building*/

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/
/*内存屏障(memory barrier) */
#define set_mb(var, value) do { var = value; mb(); } while(0)
#define mb() __asm__ __volatile__ ("" : : : "memory")


#define MEM_FREE_START_ADDR            (TEXT_BASE + 0x10000)  /*65k*/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/


extern void *input_task_dispatcher_main(void *arg);

extern u32 g_os_running_task_num;


extern u32 _end;        /*defined in link script*/
extern u32 __bss_end;   /*defined in link script*/


/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/
static mem_pool_item_t _mem_pool_items[] = {
    {8,    128},
    {16,   128},
    {32,   256},
    {64,   256},
    {128,  512},
    {256,  512},
    {512,  256},
    {1024, 128},
    {2048, 64},
    {4096, 32},
    {0, 0} /*end flag*/
};

u32 g_count_num;

etos_msg_handle g_msg_handle_dispatcher;

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

static u32 _count_consevutive_right_zero_bits(u32 value)
{
    u32 i;

    for (i = 0; i < 32; i++) {
        if (value & 1) {
            break;
        } else {
            value = value >> 1;
        }
    }

    return i;
}


void count_to_delay(u32 val)
{
    u32 i;
    for (i = 0; i < val; i++) {
        g_count_num = i;

        if (_count_consevutive_right_zero_bits(i) != etos_count_consecutive_0_in_lsb(i)) {
            set_led_on(4);
        }
    }
}


/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
void main(void)
{
    u8 *mem_pool_end;
    s32 ret;
    etos_task_handle dispatch_task_handle;

    mb();

    clock_init();

    board_interrupt_init();

    /*uart init*/
    uart_driver_init(PORT_0_UART_0);

    /*enable log*/
    xlog_init(PORT_0_UART_0);

    xlogw(LOG_MODULE_BOOT, "etos-%u.%u build@%s\r\n", ETOS_VERSION_MAIN, ETOS_VERSION_SUB, build_time);

    if (MEM_FREE_START_ADDR <= (u32)(&_end)) {
        xlogf_d(LOG_MODULE_BOOT, "need adjust free memory address\r\n");
    }

    etos_mem_pool_init((u8 *)(MEM_FREE_START_ADDR), _mem_pool_items, &mem_pool_end);
    xlogt(LOG_MODULE_BOOT, "mem_pool_end(0x%x) < stack_end(0x%x)\r\n", mem_pool_end, OS_BOOT_STACK - OS_BOOT_STACK_LEN);

    ret = uart_startup(PORT_0_UART_0);
    xlogt(LOG_MODULE_BOOT, "ret=%d, intmask=0x%x\r\n", ret, REG(INTMSK));

    etos_random_sys_init_seed(123);

    timer_hw_config_timer(4);
    timer_hw_start_timer(4);

    etos_enable_cpu_interrupt();

    etos_task_init();

    ret = etos_msgq_create(0, &g_msg_handle_dispatcher);

    if (ret) {
        xloge(LOG_MODULE_BOOT, "create msg err:%d\r\n", ret);
    } else {
        ret = etos_task_create("DISPCH", 25, input_task_dispatcher_main, NULL, 1024, &dispatch_task_handle);
        if (ret) {
            xloge(LOG_MODULE_BOOT, "create task err:%d\r\n", ret);
        }
    }

    //count_to_delay(0x92a025);
    count_to_delay(2000);

    xlogt(LOG_MODULE_BOOT, "random=%d\r\n", etos_random_sys_get());

    while (1) {
        count_to_delay(0x50a025);
        xlogt(LOG_MODULE_BOOT, "boot/idle task: random=%u  tick=%d\r\n", etos_random_sys_get(), etos_sched_get_tick());
        xlogt(LOG_MODULE_BOOT, "boot/idle task: running task number=%d\r\n", g_os_running_task_num);
        //etos_mem_report("boot/idle task");
    };
}

/* EOF */

