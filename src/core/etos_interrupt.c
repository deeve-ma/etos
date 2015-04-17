/******************************************************************************
File    :  etos_interrupt.c

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		etos interrupt process, etos do not check interrupt source
		user need process the interrupt in detail
		exact time os 通用中断处理函数部分

History:

Date           Author       Notes
----------     -------      -------------------------
2013-11-23     deeve        Create
2015-4-14      deeve        Add some comments
2015-4-15      deeve        Add etos_intr_in_isr()

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

static pfunc_user_isr _os_pfunc_user_isr_dispatcher;

#if (ETOS_KEEP_INTR_RANDOM_NUM)
static u32 _os_intr_random_val;
#endif

/*if it is in ISR*/
static BOOL _os_intr_in_isr;


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

/**
 * disable cpu interrupt.
 * disable cpu interrupt, not hardware interrupt
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @see  etos_enable_cpu_interrupt()
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_disable_cpu_interrupt(void)
{
    /*maybe it is implemented in assembly code and in the diretory arch*/
    os_disable_interrupt();
}



/**
 * enable cpu interrupt.
 * enable cpu interrupt, not hardware interrupt
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @see  etos_disable_cpu_interrupt()
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_enable_cpu_interrupt(void)
{
    /*maybe it is implemented in assembly code and in the diretory arch*/
    os_enable_interrupt();
}



/**
 * disable cpu interrupt.
 * disable cpu interrupt, not hardware interrupt, output state is for nested call
 *
 * @param[out]    state    output paramter
 *                         it will be ZERO when it had disable interrupt before call this API
 *
 * @return   none
 *
 * @note  it is just for the define etos_enter_critical()
 * @see   etos_intr_enable_cpu_interrupt()
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_intr_disable_cpu_interrupt(u32 *state)
{
    *state = os_get_cpu_intr_enabled();
    os_disable_interrupt();
}



/**
 * enable cpu interrupt.
 * enable cpu interrupt, not hardware interrupt
 *
 * @param[in]    state    which is the output of etos_intr_disable_cpu_interrupt()
 *
 * @return   none
 *
 * @note  it is just for the define etos_exit_critical()
 * @see   etos_intr_disable_cpu_interrupt()
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_intr_enable_cpu_interrupt(u32 *state)
{
    if (*state) {
        os_enable_interrupt();
    }
}



/**
 * register interrupt dispatcher.
 * register interrupt dispatcher which will be called when there is a cpu interrupt incoming
 *
 * @param[in]    intr_dispatcher     function pointer of interrupt dispatcher
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note  only can register one dispatcher, all interrupt will process in this dispatcher
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_register_interrupt_dispatcher(pfunc_user_isr intr_dispatcher)
{
    _os_pfunc_user_isr_dispatcher = intr_dispatcher;
    if (intr_dispatcher) {
        return ETOS_RET_OK;
    } else {
        return ETOS_INVALID_PARAM;
    }
}



/**
 * if it is in interrupt service routine.
 * if current context is in ISR, return TRUE, otherwise, return FALSE
 *
 * @param[in]    void
 *
 * @return   BOOL
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/15
 */
BOOL etos_intr_in_isr(void)
{
    return _os_intr_in_isr;
}


/**
 * interrupt service routine main entry.
 * the C code of interrupt service routine, it is called by assembly code in DISABLE interrupt context
 * and had saved task context
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @note       always disable interrupt in this function,
 *             because do not support nested interrupt at this moment
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_isr_main_idic(void)
{
    register etos_task_handle task_handle;
    register etos_isr_ret_e  isr_ret = ETOS_ISR_RESCHEDULE_DISABLE;

    task_handle = etos_sched_get_current_task();
    /*maybe task_handle==0, it is handled in etos_sched_do_schedule_in_isr()*/

    _os_intr_in_isr = TRUE;

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        etos_sched_set_task_state(task_handle, ETOS_TASK_INTERRUPTED);
    }

    /*invoke user interrupt service routine*/
    if (_os_pfunc_user_isr_dispatcher) {
        isr_ret = _os_pfunc_user_isr_dispatcher(etos_sched_get_tick());
    }

#if (ETOS_UPDATE_RANDOM_IN_INTR)
    etos_random_sys_update();

#if (ETOS_KEEP_INTR_RANDOM_NUM)
    if (_os_intr_random_val == 0) {
        _os_intr_random_val = etos_random_sys_get();
    } else {
        _os_intr_random_val = etos_random_dump_value(_os_intr_random_val);
    }
#endif
#endif

    _os_intr_in_isr = FALSE;

    /* do not need reschedule */
    if (isr_ret == ETOS_ISR_RESCHEDULE_DISABLE) {
        /*return to task before*/
        etos_sched_do_schedule_in_isr(task_handle);
        return;
    }

    if (isr_ret & ETOS_ISR_RESCHEDULE_UPDATE_TICK) {
        etos_sched_adjust_tick(1);
        /*sleep module*/
        etos_sleep_update_tick_in_isr(etos_sched_get_tick());
    }

    if (isr_ret & ETOS_ISR_RESCHEDULE_ENABLE) {
        /*reschedule*/
        etos_sched_reset_reschedule_engine();
        task_handle = etos_sched_pick_next_task_in_isr(etos_sched_get_tick());
        etos_sched_do_schedule_in_isr(task_handle);
    } else {
        /*return to task before*/
        etos_sched_do_schedule_in_isr(task_handle);
        return;
    }
}



/* EOF */

