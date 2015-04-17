/******************************************************************************
File    :  etos_sleep.c

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		sleep module for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-11-2      deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

#if 1
static struct list_head _os_slp_ctrl_blk_head = {&_os_slp_ctrl_blk_head, &_os_slp_ctrl_blk_head};
#else
static LIST_HEAD(_os_slp_ctrl_blk_head);
#endif

static sleep_block_t _os_sleep_scb[ETOS_MAX_TASK_NUM];

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/


/**
 * update tick for sleep.
 * update tick for sleep in interrupt service routine
 *
 * @param[in]    current_tick
 *
 * @return   none
 *
 * @note   it is called in interrupt service routine
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_sleep_update_tick_in_isr(etos_tick current_tick)
{
    list_t *pt_entry, *pt_next;
    sleep_block_t *pt_node;
    list_t *pt_os_slp_ctrl_blk_head;

    pt_os_slp_ctrl_blk_head = &_os_slp_ctrl_blk_head;
    if (list_is_empty(pt_os_slp_ctrl_blk_head)) {
        return;
    }

    list_for_each_safe(pt_entry, pt_next, pt_os_slp_ctrl_blk_head) {
        pt_node = list_entry(pt_entry, sleep_block_t, list);
        if (pt_node->wakeup_tick <= current_tick) {
            /*remove it from sleep list*/
            list_del_init(&pt_node->list);

            /*make it can be rescheduled*/
            etos_sched_resume_task_idic(pt_node->pt_os_task_tcb->task_handle, ETOS_TASK_PENDING_SLEEP);
        }
    }
}



/**
 * sleep some ticks.
 * sleep some ticks, it will cause task reschedule
 *
 * @param[in]    ticks   the ticks need to sleep
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note   none
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_sleep_tick(u32 ticks)
{
    etos_task_handle task_handle;
    u32 priority_id;
    s32 ret = ETOS_RET_OK;
    etos_tcb_t *pt_os_task_tcb_cur;
    sleep_block_t *pt_os_sleep_scb;
    etos_init_critical();

    if (etos_intr_in_isr()) {
        xlogf(LOG_MODULE_ETOS, "can not sleep in ISR\r\n");
    }

    task_handle = etos_sched_get_current_task();

    /*can not sleep in boot code*/
    ASSERT(ETOS_TASK_HANDLE_IS_VALID(task_handle));

    pt_os_task_tcb_cur = (etos_tcb_t *)task_handle;

    priority_id = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb_cur->priority;
    pt_os_sleep_scb = &_os_sleep_scb[priority_id];

    pt_os_sleep_scb->pt_os_task_tcb = pt_os_task_tcb_cur;
    pt_os_sleep_scb->sleep_ticks = ticks;

    etos_enter_critical();

    pt_os_sleep_scb->wakeup_tick = etos_sched_get_tick() + ticks;
    INIT_LIST_HEAD(&pt_os_sleep_scb->list);
    list_add_tail(&pt_os_sleep_scb->list, &_os_slp_ctrl_blk_head); /*not sort them, just add to tail*/

    /*pending itself becasue of sleep*/
    ret = etos_sched_pending_task(task_handle, ETOS_TASK_PENDING_SLEEP);
    if (ret != ETOS_RET_OK) {
        xlogf(LOG_MODULE_ETOS, "pending task for sleep fail:%d\r\n", ret);
    }

    etos_exit_critical();

    return ret;
}



/**
 * sleep some milliseconds.
 * sleep some milliseconds, it will cause task reschedule
 *
 * @param[in]    ms    the milliseconds need to sleep
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_sleep_ms(u32 ms)
{
    u32 sleep_ticks = (TICK_COUNT_IN_16_MILLISECONDS * ms) >> 4;

    if (sleep_ticks == 0) {
        sleep_ticks = 1;
    }

    return etos_sleep_tick(sleep_ticks);
}



/**
 * sleep some seconds.
 * sleep some seconds, it will cause task reschedule
 *
 * @param[in]    seconds    the seconds need to sleep
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_sleep_second(u32 seconds)
{
    return etos_sleep_ms(seconds * 1000);
}


/* EOF */

