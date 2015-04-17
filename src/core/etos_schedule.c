/******************************************************************************
File    :  etos_schedule.c

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


/*已经在run的task个数, 应该总是在0和1之间跳动*/
/*running task number, not include the pending and interrupted tasks*/
u32 g_os_running_task_num;



/*用于保存boot code部分的sp
 *主要场景是boot code中有中断产生，但是又没有task可运行
 *或者是唯一的task运行结束后需要切回去
 */
/*the stack pointer(sp) in boot code*/
u32 *g_os_boot_sp;



/*当前正在执行的task
 *可能为0，场景是boot code切第一个task，或者是task 结束执行
 */
/*current task handle, it will be zero in boot code or task end*/
etos_task_handle g_os_current_task_handle;



/* 记录一次中断最开始的时候的调度mask
 * create/pending/resume task会改变该mask的值，另外，一个task结束也会自动改变mask的值
 * bit和task priority不是一一对应的，切记
 */
/*schedule mask, it corresponds to task schedulable state*/
u32 g_os_sched_original_mask;



/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

static const u32 _os_mod_37_bit_position[] = { // map a bit value mod 37 to its position
    32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0,
    13, 4, 7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21,
    14, 9, 5, 20, 8, 19, 18
};


/* 调度的时候使用的mask bits，记录两个可调度中断间的临时mask状态
 * 如果对应的bit为1，则表示对应的task的可以被调度执行的
 * 各task切换时先调整在 _os_sched_priority_mask_between_2_intrs 中的调度状态
 * bit和task priority不是一一对应的，切记
 */
/*schedule mask between 2 interrupts, it corresponds to task schedulable state*/
static u32 _os_sched_priority_mask_between_2_intrs;



/* etos system tick*/
static etos_tick _os_tick;


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/*the common entry for all task*/
s32 _etos_sched_task_common_entry(etos_tcb_t *pt_os_task_tcb)
{
    s32 ret = ETOS_RET_FAIL;

    if (pt_os_task_tcb->task_entry) {
        ret = (s32)pt_os_task_tcb->task_entry(pt_os_task_tcb->arg);
    } else {
        ASSERT(0);
    }

    return ret;
}

/*start a task in disable interrupt context*/
void _etos_sched_start_task_idic(etos_tcb_t *pt_os_task_tcb)
{
    s32 ret;
    u32 mask_val;
    etos_task_handle task_handle;

    pt_os_task_tcb->task_state = ETOS_TASK_RUNNING;
    g_os_current_task_handle = pt_os_task_tcb->task_handle;

    g_os_running_task_num++;

    etos_enable_cpu_interrupt(); /*enable interrupt first*/

    ret = _etos_sched_task_common_entry(pt_os_task_tcb);

    /*it is the end of this task*/
    if (ret) {
        xlogw(LOG_MODULE_ETOS, "task:%s return %x\n", pt_os_task_tcb->task_name, ret);
    }

    mask_val = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb->priority;
    mask_val = 1 << mask_val;

    /*remove from schedule list*/
    etos_disable_cpu_interrupt();

    _os_sched_priority_mask_between_2_intrs &= (~mask_val);
    pt_os_task_tcb->task_state = ETOS_TASK_END;

    g_os_running_task_num--;
    g_os_current_task_handle = 0;

    //g_os_sched_original_mask &= (~mask_val);
    ret = etos_task_destroy_idic(pt_os_task_tcb->task_handle);
    if (ret) {
        xlogw(LOG_MODULE_ETOS, "task:%s destroy:%d\n", pt_os_task_tcb->task_name, ret);
    }

    /*do schedule, maybe interruptted before switch context idic */
    task_handle = etos_sched_pick_next_task_idic(etos_sched_get_tick());
    etos_sched_do_schedule_idic(task_handle, ETOS_TASK_END);

    /* etos_enable_cpu_interrupt();  do not enable interrupt in this function
     * becaue it is auto enabled when task rescheduled
     */
}




/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/


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
u32 etos_count_consecutive_0_in_lsb(u32 n)
{
    return _os_mod_37_bit_position[((-n) & n) % 37];
}



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
etos_tick etos_sched_get_tick(void)
{
    return _os_tick;
}



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
void etos_sched_set_tick(etos_tick tick)
{
    _os_tick = tick;
}



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
void etos_sched_adjust_tick(s32 offset)
{
    _os_tick += offset;
}



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
etos_task_handle etos_sched_get_current_task(void)
{
    return g_os_current_task_handle;
}



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
s32 etos_sched_set_task_state(etos_task_handle task_handle, etos_task_state_e task_state)
{
    s32 ret = ETOS_INVALID_PARAM;
    etos_tcb_t *pt_os_task_tcb = (etos_tcb_t *)task_handle;

    if (pt_os_task_tcb && (pt_os_task_tcb->task_handle == task_handle)) {
        pt_os_task_tcb->task_state = task_state;
        ret = ETOS_RET_OK;
    }

    return ret;
}



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
s32 etos_sched_get_task_state(etos_task_handle task_handle, etos_task_state_e *task_state)
{
    s32 ret = ETOS_INVALID_PARAM;
    etos_tcb_t *pt_os_task_tcb = (etos_tcb_t *)task_handle;

    if (pt_os_task_tcb && (pt_os_task_tcb->task_handle == task_handle)) {
        if (task_state) {
            *task_state = pt_os_task_tcb->task_state;
            ret = ETOS_RET_OK;
        }
    }

    return ret;
}



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
void etos_sched_reset_reschedule_engine(void)
{
    _os_sched_priority_mask_between_2_intrs = g_os_sched_original_mask;
}



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
etos_task_handle etos_sched_pick_next_task_in_isr(etos_tick tick)
{
    return etos_sched_pick_next_task_idic(tick);
}



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
etos_task_handle etos_sched_pick_next_task_idic(etos_tick tick)
{
    etos_tcb_t *pt_os_task_tcb;
    u32 zero_bits_in_lsb, priority_id;

    if (_os_sched_priority_mask_between_2_intrs == 0) { /*no task*/
        return 0;
    }

    tick = tick; /*for compile warning*/

    /*find the highest priority task*/
    zero_bits_in_lsb = etos_count_consecutive_0_in_lsb(_os_sched_priority_mask_between_2_intrs);
    priority_id = zero_bits_in_lsb;

    pt_os_task_tcb = etos_task_get_task(priority_id);

    if ((pt_os_task_tcb->task_state & ETOS_TASK_CREATED)
        || (pt_os_task_tcb->task_state & ETOS_TASK_READY)
        || (pt_os_task_tcb->task_state & ETOS_TASK_INTERRUPTED)) {
        /*find correct task*/
        ASSERT(ETOS_TASK_HANDLE_IS_VALID(pt_os_task_tcb->task_handle));
    } else {
        ASSERT(0);
    }

    return pt_os_task_tcb->task_handle;
}



extern void set_led_on(u32 led_x);
extern void set_led_off(u32 led_x);



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
void etos_sched_do_schedule_in_isr(etos_task_handle task_handle)
{
    register etos_tcb_t *pt_os_task_tcb_next = (etos_tcb_t *)task_handle;
    etos_task_handle *current_task_handle = &g_os_current_task_handle;
    u32 *boot_sp = g_os_boot_sp;

    if (*current_task_handle) {
        /* not from a task end or boot code */
        ASSERT(ETOS_TASK_HANDLE_IS_VALID(*current_task_handle));
    } else {
        /* from a task end or boot code,  not need save current context */
        if (task_handle == 0) {
            /*to boot code*/
            //ASSERT(g_os_running_task_num == 0)

            g_os_running_task_num = 0;
            *current_task_handle = 0;
            os_switch_to_boot_code(boot_sp);
        } else {
            ASSERT(pt_os_task_tcb_next->task_handle == (etos_task_handle)pt_os_task_tcb_next);
            /*to another task*/

            if (pt_os_task_tcb_next->task_state == ETOS_TASK_CREATED) {
                /*start a task*/
                pt_os_task_tcb_next = os_swtich_to_task_level_and_keep_idic(pt_os_task_tcb_next,
                                                                            pt_os_task_tcb_next->register_stack_pointer,
                                                                            0);
                _etos_sched_start_task_idic(pt_os_task_tcb_next );
            } else {
                /*swtich to next task*/
                pt_os_task_tcb_next->task_state = ETOS_TASK_RUNNING;
                g_os_running_task_num++;
                *current_task_handle = pt_os_task_tcb_next->task_handle;
                os_switch_to_task(pt_os_task_tcb_next->register_stack_pointer); /*never return*/
            }
        }
        return ; /*never run to here*/
    }

    /* not from a task end or boot coded */
    if (task_handle == 0) {
        //ASSERT(g_os_running_task_num == 0);

        /*there is no task need to run, switch to boot code*/
        g_os_running_task_num = 0;
        *current_task_handle = 0;
        os_switch_to_boot_code(boot_sp);
    } else {
        ASSERT(pt_os_task_tcb_next->task_handle == (etos_task_handle)pt_os_task_tcb_next);

        /*there is a next task need to swtich*/

        if (pt_os_task_tcb_next->task_state == ETOS_TASK_CREATED) {
            /*start a task*/
            pt_os_task_tcb_next = os_swtich_to_task_level_and_keep_idic(pt_os_task_tcb_next,
                                                                        pt_os_task_tcb_next->register_stack_pointer,
                                                                        0);
            _etos_sched_start_task_idic(pt_os_task_tcb_next );
        } else {
            /*swtich to next task, current task had save context when interrupt happened*/
            pt_os_task_tcb_next->task_state = ETOS_TASK_RUNNING;
            if  (*current_task_handle != pt_os_task_tcb_next->task_handle) {
                *current_task_handle = pt_os_task_tcb_next->task_handle;
                g_os_running_task_num++;
            }
            os_switch_to_task(pt_os_task_tcb_next->register_stack_pointer); /*never return*/
        }
    }
}



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
s32 etos_sched_do_schedule_idic(etos_task_handle task_handle, etos_task_state_e reason)
{
    register etos_tcb_t *pt_os_task_tcb_cur;
    register etos_tcb_t *pt_os_task_tcb_next = (etos_tcb_t *)task_handle;

    if (g_os_current_task_handle) { /* not from a task end */
        ASSERT(ETOS_TASK_HANDLE_IS_VALID(g_os_current_task_handle));

        if ((reason > ETOS_TASK_INVALID) && (reason != ETOS_TASK_END)) {
            etos_sched_set_task_state(g_os_current_task_handle, reason);
        }
    } else { /* not need save current context at task end*/
        if (task_handle == 0) {
            /*from task end to NONE task*/

            /*restore boot code*/
            g_os_running_task_num = 0;
            g_os_current_task_handle = 0;
            os_switch_to_boot_code(g_os_boot_sp);
        } else {
            /*from task end to another task*/
            ASSERT(pt_os_task_tcb_next->task_handle == (etos_task_handle)pt_os_task_tcb_next);

            if (pt_os_task_tcb_next->task_state == ETOS_TASK_CREATED) {
                /*start a task*/
                pt_os_task_tcb_next = os_swtich_to_task_level_and_keep_idic(pt_os_task_tcb_next,
                                                                            pt_os_task_tcb_next->register_stack_pointer,
                                                                            0);

                _etos_sched_start_task_idic(pt_os_task_tcb_next );
            } else {
                /*swtich to next task*/
                pt_os_task_tcb_next->task_state = ETOS_TASK_RUNNING;
                g_os_running_task_num++;
                g_os_current_task_handle = pt_os_task_tcb_next->task_handle;
                os_switch_to_task(pt_os_task_tcb_next->register_stack_pointer); /*never return*/
            }
        }
        return ETOS_RET_OK; /*never run to here, just for compile warning*/
    }

    /* not from a task end */
    pt_os_task_tcb_cur = (etos_tcb_t *)g_os_current_task_handle;

    if (task_handle == 0) {
#if 0   //only 1 task sleep to boot       
        ASSERT(g_os_running_task_num == 0);
#endif
        /*there is no task need to run, switch to boot code*/
        g_os_running_task_num = 0;
        g_os_current_task_handle = 0;
        os_switch_task_context(&pt_os_task_tcb_cur->register_stack_pointer, g_os_boot_sp, 0);
    } else {
        ASSERT(pt_os_task_tcb_next->task_handle == (etos_task_handle)pt_os_task_tcb_next);

        /*there is a next task need to swtich*/

        if (pt_os_task_tcb_next->task_state == ETOS_TASK_CREATED) {
            /*start a task and save current task context*/
            os_save_task_and_start_task(&pt_os_task_tcb_cur->register_stack_pointer,
                                        pt_os_task_tcb_next,
                                        _etos_sched_start_task_idic,
                                        0);
        } else {
            /*swtich to next running task*/
            pt_os_task_tcb_next->task_state = ETOS_TASK_RUNNING;
            if (g_os_current_task_handle != pt_os_task_tcb_next->task_handle) {
                g_os_current_task_handle = pt_os_task_tcb_next->task_handle;
                g_os_running_task_num++;
            }

            xlogi(LOG_MODULE_ETOS, "task num=%d switch from %s to %s\r\n", g_os_running_task_num,
                  pt_os_task_tcb_cur->task_name, pt_os_task_tcb_next->task_name);
            os_switch_task_context(&pt_os_task_tcb_cur->register_stack_pointer, pt_os_task_tcb_next->register_stack_pointer, 0);
        }
    }

    return ETOS_RET_OK; /*never run to here, just for compile warning*/
}



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
s32 etos_sched_pending_task(etos_task_handle task_handle, etos_task_state_e reason)
{
    register etos_task_handle task_handle_next;
    register etos_tcb_t *pt_os_task_tcb_cur, *pt_os_task_tcb_next;
    u32 mask_val;
    u32 *sp_addr;
    etos_init_critical();

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        pt_os_task_tcb_cur = (etos_tcb_t *)task_handle;
    } else {
        return ETOS_INVALID_PARAM;
    }

    mask_val = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb_cur->priority;
    mask_val = 1 << mask_val;

    etos_enter_critical();

    g_os_sched_original_mask &= (~(mask_val));
    _os_sched_priority_mask_between_2_intrs &= (~mask_val);

    ASSERT(g_os_running_task_num != 0);

    g_os_running_task_num--;

    task_handle_next = etos_sched_pick_next_task_idic(etos_sched_get_tick());
    pt_os_task_tcb_next = (etos_tcb_t *)task_handle_next;

    if (pt_os_task_tcb_next) {
        sp_addr = pt_os_task_tcb_next->register_stack_pointer;
        xlogi(LOG_MODULE_ETOS, "pending task: name=%s reason=0x%x switch=%s\r\n",
              pt_os_task_tcb_cur->task_name, reason, pt_os_task_tcb_next->task_name);
        xlogi(LOG_MODULE_ETOS, "cpsr:0x%x lr:0x%x pc:0x%x task num:%d\r\n", *sp_addr,
              *(sp_addr + 14), *(sp_addr + 15), g_os_running_task_num);

    } else {
        sp_addr = g_os_boot_sp;
        xlogi(LOG_MODULE_ETOS, "pending task: name=%s reason=0x%x switch=boot\r\n",
              pt_os_task_tcb_cur->task_name, reason);
        xlogi(LOG_MODULE_ETOS, "cpsr:0x%x lr:0x%x pc:0x%x task num:%d\r\n", *sp_addr,
              *(sp_addr + 14), *(sp_addr + 15), g_os_running_task_num);
    }

    etos_sched_do_schedule_idic(task_handle_next, reason);

    /*enable interrupt when reschedule here*/
    etos_exit_critical();

    return ETOS_RET_OK;
}



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
s32 etos_sched_resume_task(etos_task_handle task_handle, etos_task_state_e reason)
{
    register etos_tcb_t *pt_os_task_tcb;
    u32 mask_val;
    etos_init_critical();

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        pt_os_task_tcb = (etos_tcb_t *)task_handle;
    } else {
        return ETOS_INVALID_PARAM;
    }

    mask_val = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb->priority;
    mask_val = 1 << mask_val;

    etos_enter_critical();

    /*make task can be rescheduled*/
    g_os_sched_original_mask |= mask_val;
    _os_sched_priority_mask_between_2_intrs |= mask_val;


    pt_os_task_tcb->task_state &= (~reason);
    pt_os_task_tcb->task_state |= ETOS_TASK_READY;

    etos_exit_critical();

    xlogi(LOG_MODULE_ETOS, "resume task: name=%s pc=0x%x lr=0x%x reason=0x%x\r\n",
          pt_os_task_tcb->task_name,
          *(pt_os_task_tcb->register_stack_pointer + 15),
          *(pt_os_task_tcb->register_stack_pointer + 14),
          reason);

    return ETOS_RET_OK;
}



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
s32 etos_sched_resume_task_idic(etos_task_handle task_handle, etos_task_state_e reason)
{
    register etos_tcb_t *pt_os_task_tcb;
    u32 mask_val;
    u32 *sp_addr;

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        pt_os_task_tcb = (etos_tcb_t *)task_handle;
    } else {
        return ETOS_INVALID_PARAM;
    }

    mask_val = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb->priority;
    mask_val = 1 << mask_val;

    /*make task can be rescheduled*/
    g_os_sched_original_mask |= mask_val;
    _os_sched_priority_mask_between_2_intrs |= mask_val;

    pt_os_task_tcb->task_state &= (~reason);
    pt_os_task_tcb->task_state |= ETOS_TASK_READY;

    if (g_os_current_task_handle) {
        sp_addr = ((etos_tcb_t *)(g_os_current_task_handle))->register_stack_pointer;
        xlogi(LOG_MODULE_ETOS, "interrupted task: cpsr:0x%x lr:0x%x pc:0x%x\r\n", *sp_addr,
              *(sp_addr + 14), *(sp_addr + 15));

    } else {
        sp_addr = g_os_boot_sp;
        xlogi(LOG_MODULE_ETOS, "interrupted boot: cpsr:0x%x lr:0x%x pc:0x%x\r\n", *sp_addr,
              *(sp_addr + 14), *(sp_addr + 15));
    }

    xlogi(LOG_MODULE_ETOS, "resume task: name=%s pc=0x%x lr=0x%x reason=0x%x\r\n",
          pt_os_task_tcb->task_name,
          *(pt_os_task_tcb->register_stack_pointer + 15),
          *(pt_os_task_tcb->register_stack_pointer + 14),
          reason);

    return ETOS_RET_OK;
}



/* EOF */

