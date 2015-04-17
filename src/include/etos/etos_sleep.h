/******************************************************************************
File    :  etos_sleep.h

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
#ifndef __ETOS_SLEEP_H__
#define __ETOS_SLEEP_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"
#include "etos_listop.h"
#include "etos_task.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define TICK_COUNT_IN_16_MILLISECONDS          (1)    //16ms


#define tick_to_ms(tick)        ((tick) * 16 / TICK_COUNT_IN_16_MILLISECONDS)
#define ms_to_tick(ms)          ((ms) * TICK_COUNT_IN_16_MILLISECONDS / 16)


typedef struct _sleep_block {
    list_t  list;
    etos_tcb_t *pt_os_task_tcb;
    u32 wakeup_tick;          /*wakeup after this tick*/
    u32 sleep_ticks;          /*duration*/
} sleep_block_t;


/******************************************************************************
 *                                 Declar Functions                           *
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
void etos_sleep_update_tick_in_isr(etos_tick current_tick);



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
s32 etos_sleep_tick(u32 ticks);



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
s32 etos_sleep_ms(u32 ms);



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
s32 etos_sleep_second(u32 seconds);


#endif  /* __ETOS_SLEEP_H__ */

/* EOF */

