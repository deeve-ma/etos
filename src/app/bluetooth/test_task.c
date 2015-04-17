/******************************************************************************
File    :  test_task.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		task test source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-15      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"
#include "drivers.h"
#include "etos_arm.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/
extern void count_to_delay(u32 val);
/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
void *task_test_main(void *arg)
{
    u32 random;
    etos_tick tick_cur;
    u32 sleep_s = (u32)arg;
    etos_tick tick_begin = etos_sched_get_tick();

    xlogt(LOG_MODULE_T_TASK, "TEST%d: enter tick:%u\r\n", sleep_s, tick_begin);
    xlogt(LOG_MODULE_T_TASK, "irq:0x%x\r\n", os_get_cpu_intr_enabled());

    while (1) {
        random = etos_random_sys_get() % 200;
        random += (sleep_s * 1000);
        xlogt(LOG_MODULE_T_TASK, "TEST%d: sleep:%u ms\r\n", sleep_s, random);
        etos_sleep_ms(random);
        xlogt(LOG_MODULE_T_TASK, "TEST%d: wakeup: 0x%x\r\n", sleep_s, os_get_cpu_intr_enabled());
        if (sleep_s & 1){
            count_to_delay(0x30a025);
        }
        tick_cur = etos_sched_get_tick();
        if (tick_to_ms(tick_cur - tick_begin) > (1000 * 50)) {
            break;
        }
        xlogt(LOG_MODULE_T_TASK, "TEST%d: irq: 0x%x\r\n", sleep_s, os_get_cpu_intr_enabled());
    }

    xlogt(LOG_MODULE_T_TASK, "TEST%d: exit tick:%u\r\n", sleep_s, tick_cur);

    return (void *)0;
}


/* EOF */

