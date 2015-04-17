/******************************************************************************
File    :  etos_schedule.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os schedule module

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-20     deeve        Create
2015-4-15      deeve        Add some comments

*******************************************************************************/
#ifndef __ETOS_SCHEDULE_H__
#define __ETOS_SCHEDULE_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_mem.h"
#include "etos_task.h"


/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/


/* -->  function below is implemented in assembly code --> start*/

extern s32 os_save_task_and_start_task(u32  **sp_from,
                                       etos_tcb_t *pt_os_task_tcb_next,
                                       void (*task_start_entry)(etos_tcb_t *pt_os_task_tcb),
                                       u32 useless);


/*切换到boot code，不需要保存当前task context, 先切回task level，再换context*/
extern void os_switch_to_boot_code(u32 *sp_dest);


/*先切回task level，再换context*/
extern void os_switch_to_task(u32 *sp_dest);


extern s32 os_switch_task_context(u32  **sp_from, u32  *sp_to, u32 useless);


etos_tcb_t *os_swtich_to_task_level_and_keep_idic(etos_tcb_t *pt_task_tcb, u32 *sp_start, u32 useless);


/*
 * 以上五个函数是需要移植的函数，一般需用汇编实现
 * 实现文件应该位于arch目录下
 */


/* -->  function above is implemented in assembly code --> end*/


/**
 * get consecutive zero bits.
 * count LSB consecutive zero bits in input parameter n
 * LSB=least significant bits
 * n = 1, return 0
 * n = 16 = 10000b, return 4
 *
 * @param[in]    n
 *
 * @return   consecutive zero bits in LSB
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/15
 */
u32 etos_count_consecutive_0_in_lsb(u32 n);



/**
 * get tick.
 * get etos system tick
 *
 * @param[in]    void
 *
 * @return  tick
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/15
 */
etos_tick etos_sched_get_tick(void);



/**
 * set tick.
 * set etos system tick
 *
 * @param[in]    tick
 *
 * @return  none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/15
 */
void etos_sched_set_tick(etos_tick tick);



/**
 * adjust tick.
 * adjust etos system tick, current tick add offset
 *
 * @param[in]    offset
 *
 * @return   none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/15
 */
void etos_sched_adjust_tick(s32 offset);



/**
 * get current task handle.
 * get current running task handle, it is zero in the condition of boot or task end
 *
 * @param[in]    void
 *
 * @return   current task handle
 *
 * @note   it may be zero
 * @authors    deeve
 * @date       2015/4/15
 */
etos_task_handle etos_sched_get_current_task(void);



/**
 * set task state.
 * set task schedulable state
 *
 * @param[in]    task_handle
 * @param[in]    task_state
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see      etos_sched_get_task_state()
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_set_task_state(etos_task_handle task_handle, etos_task_state_e task_state);



/**
 * get task state.
 * get task schedulable state
 *
 * @param[in]    task_handle
 * @param[out]   task_state     output task state
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see    etos_sched_set_task_state()
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_get_task_state(etos_task_handle task_handle, etos_task_state_e *task_state);



/**
 * reset reschedule engine.
 * reset reschedule engine state in each interrupt to confuse with last interrupt state
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @note      it is called in interrupt service routine
 * @authors    deeve
 * @date       2015/4/15
 */
void etos_sched_reset_reschedule_engine(void);



/**
 * pick next task in interrupt service routine.
 * pick next task which will be executed at the end of interrupt service routine
 *
 * @param[in]    tick     current system tick
 *
 * @return    task handle
 *
 * @note  it is called in ISR
 * @see   etos_sched_pick_next_task_idic()
 * @authors    deeve
 * @date       2015/4/15
 */
etos_task_handle etos_sched_pick_next_task_in_isr(etos_tick tick);



/**
 * pick next task in disable interrupt context.
 * pick next task which will be executed in disable interrupt context
 * it is called between task swtich typically
 *
 * @param[in]    tick     current system tick
 *
 * @return    task handle
 *
 * @note  it is called in disable interrupt context
 * @authors    deeve
 * @date       2015/4/15
 */
etos_task_handle etos_sched_pick_next_task_idic(etos_tick tick);



/**
 * do schedule in interrupt.
 * swtich task in interrupt service routine
 * scenarios as below:
 * ------------------------------------------------------------
 *|   task before interrupt    |    new task (interrupt end)   |
 * ------------------------------------------------------------
 *|    boot code               |    boot code                  |
 *|                            |    new task(not start to run) |
 *|    task(running)           |    task (ran)                 |
 * ------------------------------------------------------------
 *
 * @param[in]    task_handle   the task which needs to switch to
 *
 * @return  none
 *
 * @note    it is called in ISR
 * @authors    deeve
 * @date       2015/4/15
 */
void etos_sched_do_schedule_in_isr(etos_task_handle task_handle);



/**
 * do schedule in disable interrupt context.
 * swtich task in disable interrupt context
 * it is called between task swtich typically
 * scenarios as below:
 * ------------------------------------------------------------
 *|   task before swtich     |    new task (need switch to)   |
 * ------------------------------------------------------------
 *|  task(run to end)        |    boot code                   |
 *|                          |    new task(not start to run)  |
 *|  task(will pending)      |    task (ran)                  |
 * ------------------------------------------------------------
 * do NOT do schedule in boot code, because it is the lowest priority task
 * when boot code is running, no other task is executable
 *
 * @param[in]    task_handle      the task which needs to switch to
 * @param[in]    reason           schedule reason
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note   it is called in disable interrupt context
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_do_schedule_idic(etos_task_handle task_handle, etos_task_state_e reason);



/**
 * pending a task.
 * pending a task, it will cause task reschedule
 *
 * @param[in]    task_handle      pending task handle
 * @param[in]    reason           pending reason
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note   maybe user do not need call this API manually
 * @see    etos_sched_resume_task()
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_pending_task(etos_task_handle task_handle, etos_task_state_e reason);



/**
 * resume a task.
 * resume a task, the task resumed may not be executed
 * it will reschedule at next schedulable time
 *
 * @param[in]    task_handle      resume task handle
 * @param[in]    reason           resume reason
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note  maybe user do not need call this API manually
 * @see   etos_sched_pending_task()
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_resume_task(etos_task_handle task_handle, etos_task_state_e reason);



/**
 * resume a task in disable interrupt context.
 * resume a task in disable interrupt context,
 * the task resumed may not be executed
 * it will reschedule at next schedulable time
 *
 * @param[in]    task_handle      resume task handle
 * @param[in]    reason           resume reason
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see   etos_sched_resume_task()
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_sched_resume_task_idic(etos_task_handle task_handle, etos_task_state_e reason);



#endif  /* __ETOS_SCHEDULE_H__ */

/* EOF */

