/******************************************************************************
File    :  etos_task.c

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os task impliment

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-19     deeve        Create
2015-4-15      deeve        Remove unnecessary code

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/
#define MIN_STACK_LEN                  (512)
#define DEFAULT_TASK_NAME              "NULL"
/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/*schedule mask, it corresponds to task schedulable state*/
extern u32 g_os_sched_original_mask;    /*declared in etos_schedule.c*/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/* 优先级占位mask bits，一个bit对应一个priority 的task
 * 注意: _os_task_priority_mask[0:1] 对应的task的priority是31:30
 * 和g_os_sched_priority_mask一样
 */
/*
 * task priority mask, each priority occupy one bit
 * but priority 1 is in the bit 30
 */
static u32 _os_task_priority_mask;


/*task control block*/
static etos_tcb_t _os_task_tcb[ETOS_MAX_TASK_NUM];


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

#if (ETOS_ENABLE_EXACT_TASK)
s32 _add_task_by_tick(etos_tcb_t *pt_os_task_tcb, u32 schedule_tick, u32 bit_mask)
{
    etos_init_critical();
    pt_os_task_tcb->schedule_tick = schedule_tick;
    etos_enter_critical();
    list_add_tail(&pt_os_task_tcb->list, &_t_list_exact_task_pend);
    _task_exact_mask |= bit_mask;
    etos_exit_critical();

    //如果tick<=32+current_tick
}
#endif


/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

/**
 * etos task management initialise.
 * init etos task management module, need called before any task API
 *
 * @param[in]    void
 *
 * @return
 * @retval 0                success
 * @retval other            fail
 *
 * @see        etos_task_deinit
 * @authors    deeve
 * @date       2013/10/20
 */
s32 etos_task_init(void)
{
    _os_task_priority_mask = 0;

    return ETOS_RET_OK;
}



/**
 * destroy etos task management.
 * deinit etos task management module
 *
 * @param[in]    void
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @see        etos_task_init
 * @authors    deeve
 * @date       2013/10/20
 */
s32 etos_task_deinit(void)
{
    _os_task_priority_mask = 0;

    memset(_os_task_tcb, 0, sizeof(_os_task_tcb));

    return ETOS_RET_OK;
}



/**
 * create a new task in etos.
 * create a new RTOS task. the task will be serviced early if its priority is higher
 * as we know, in RTOS, if the higher priority task is not release cpu, the lower priority
 * task will wait
 *
 * etos的task分两种，一种是按优先级运行的task(优先级是 0 ~ ETOS_MAX_PRIORITY_TASK_NUM-1).
 * 一种是按时间点准确调度的task(优先级从ETOS_MAX_PRIORITY_TASK_NUM开始),本函数是创建基于
 * 优先级的task，priority数值越大，优先级越高
 * task_entry()的示例code如下:
 *     void* task_entry(void* arg)
 *     {
 *          ...
 *          while(1){
 *              ...
 *              if (exp){
 *                  break;
 *              }
 *              ...
 *          }
 *          ...
 *     }
 *
 * @param[in]    task_name      task name, max length is ETOS_MAX_TASK_NAME_LEN
 * @param[in]    priority       0 ~ ETOS_MAX_PRIORITY_TASK_NUM-1,only one task in one priority
 * @param[in]    task_entry     function pointer of task entry
 * @param[in]    arg            task entry arguments
 * @param[in]    stack_len      task stack length
 * @param[out]   task_handle    output task handle
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note  new task will not be scheduled until next schedulable time
 * @authors    deeve
 * @date       2013/10/19
 */
s32 etos_task_create(const char *task_name, u32 priority, void * (*task_entry)(void *arg),
                     void *arg,  u32 stack_len, etos_task_handle *task_handle)
{
    u32 bit_mask, priority_id;
    etos_tcb_t *pt_os_task_tcb;
    etos_init_critical();

    if ((priority >= ETOS_MAX_PRIORITY_TASK_NUM)
        || (task_entry == NULL)
        || (stack_len < MIN_STACK_LEN)
        || (task_handle == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    /*priority 和 _os_task_priority_mask 不是按位对应的，
      原因是为了方便schedule的时候好查找最高优先级的task*/
    priority_id = ETOS_MAX_PRIORITY_TASK_NUM - 1 - priority;

    bit_mask = 1 << priority_id;

    /* had a task already */
    if (bit_mask & _os_task_priority_mask) {
        return ETOS_NOT_SUPPORT;
    }

    pt_os_task_tcb = &_os_task_tcb[priority_id];

    pt_os_task_tcb->stack_begin_addr = (u32 *)malloc(stack_len);
    if (pt_os_task_tcb->stack_begin_addr == NULL) {
        return ETOS_NO_MEM;
    }

    /* assume task stack grow from high to low */
    pt_os_task_tcb->register_stack_pointer = pt_os_task_tcb->stack_begin_addr + stack_len;
    pt_os_task_tcb->stack_len = stack_len;

    pt_os_task_tcb->priority = priority;
    pt_os_task_tcb->task_entry = task_entry;
    pt_os_task_tcb->arg = arg;

    pt_os_task_tcb->task_handle = (etos_task_handle)pt_os_task_tcb;
    pt_os_task_tcb->task_state = ETOS_TASK_CREATED;

    if (task_name) {
        strncpy(pt_os_task_tcb->task_name, task_name, ETOS_MAX_TASK_NAME_LEN);
    } else {
        strncpy(pt_os_task_tcb->task_name, DEFAULT_TASK_NAME, ETOS_MAX_TASK_NAME_LEN);
    }

    *task_handle = (etos_task_handle)pt_os_task_tcb;

    etos_enter_critical();

    /* had a task already */
    if (bit_mask & _os_task_priority_mask) {
        free(pt_os_task_tcb->stack_begin_addr);
        etos_exit_critical();
        return ETOS_NOT_SUPPORT;
    }

    _os_task_priority_mask |= bit_mask;
    g_os_sched_original_mask |= bit_mask;

    etos_exit_critical();

    return ETOS_RET_OK;
}


#if (ETOS_ENABLE_EXACT_TASK)

/**
 * create a new exact task.
 * etos的task分两种，一种是按优先级运行的task(优先级是 0 ~ ETOS_MAX_PRIORITY_TASK_NUM-1).
 * 一种是按时间点精确调度的task(优先级从ETOS_MAX_PRIORITY_TASK_NUM开始),本函数是创建后者
 * 的task，这里的priority只是用来判决冲突的时候调度，越大的priority越有优先调度的权利
 * first_tick 是该task第一次被调度的时间点，其后是否需要被调度需要task自己设置
 * 需要注意的是，在task_entry函数退出前的最后一个函数必须是etos_task_exit()
 * 即task_entry()的示例code如下:
 *     void* task_entry(void* arg)
 *     {
 *          ...
 *          while(1){
 *              ...
 *              if (exp){
 *                  break;
 *              }
 *              ...
 *          }
 *
 *          etos_task_exit();
 *     }
 *
 * @param[in]    priority    begin with ETOS_MAX_PRIORITY_TASK_NUM，最好不要重复优先级
 * @param[in]    task_entry  a function pointer for task entry
 * @param[in]    arg          task entry arguments
 * @param[in]    near_exact_sched_func  在临近被调度的tick的前一个tick调用的函数
 * @param[in]    stack        a pointer of task stack. 优先级高于stack_len. 即如果同时
 *                            设置了stack和stack_len,则使用stack作为task的堆栈
 * @param[in]    stack_len    task stack length,如果stack=NULL,则由ETOS调用内存分配函数分配
 *                            一块空间作为task的堆栈.好处是系统在task结束后会自动回收这部分内存
 * @param[in]    first_tick  第一个期望被调度(执行)的tick
 * @param[in]    name   task name, max length is ETOS_MAX_TASK_NAME_LEN
 * @param[out]   task_handle output handle
 *
 * @return
 * @retval 0       success
 * @retval -1      fail
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2013/10/20
 */
s32 etos_task_create_exact(u32 priority, void * (*task_entry)(void *arg), void *arg,
                           u32 *stack, u32 stack_len, u32 first_tick, char *name, u32 *task_handle)
{
    u32 my_priority, bit_mask;
    etos_tcb_t *pt_os_task_tcb;
    etos_init_critical();

    if ((priority < ETOS_MAX_PRIORITY_TASK_NUM) || (task_entry == NULL)
        || (!stack && (stack_len < MIN_STACK_LEN)) || (task_handle == NULL) ) {
        return ETOS_INVALID_PARAM;
    }

    //可以改进
    if (first_tick <= (etos_get_current_tick() + ETOS_STRICT_TASK_READY_COUNT)) {
        return ETOS_INVALID_PARAM;
    }

    my_priority = priority - ETOS_MAX_PRIORITY_TASK_NUM;
    bit_mask = 1 << my_priority;

    if (bit_mask & _task_exact_mask) {
        //have had a task already
        return ETOS_INVALID_PARAM;
    }

    pt_os_task_tcb = &_os_task_tcb[priority];
    if (stack) {
        pt_os_task_tcb->stack_begin_addr = stack;
    } else {
        pt_os_task_tcb->stack_begin_addr = (u32 *)malloc(stack_len);
        if (pt_os_task_tcb->task_stack == NULL) {
            return ETOS_NO_MEM;
        }
    }

    pt_os_task_tcb->register_stack_pointer = pt_os_task_tcb->stack_begin_addr;
    pt_os_task_tcb->priority = priority;
    pt_os_task_tcb->task_entry = task_entry;
    pt_os_task_tcb->arg = arg;
    //pt_os_task_tcb->schedule_tick = first_tick;
    pt_os_task_tcb->auto_expand = false;

    pt_os_task_tcb->task_handle = (u32 *)pt_os_task_tcb;
    *task_handle = (u32)pt_os_task_tcb;
    pt_os_task_tcb->task_state = ETOS_TASK_PEND;
    if (name) {
        strncpy(pt_os_task_tcb->task_name, name, ETOS_MAX_TASK_NAME_LEN);
    } else {
        strncpy(pt_os_task_tcb->task_name, DEFAULT_TASK_NAME, ETOS_MAX_TASK_NAME_LEN);
    }

    return _add_task_by_tick(pt_os_task_tcb, first_tick);

    etos_enter_critical();
    list_add_tail(&pt_os_task_tcb->list, &_t_list_exact_task_pend);
    _task_exact_mask |= bit_mask;
    etos_exit_critical();

    return ETOS_RET_OK;

}


#endif



/**
 * delete a not scheduled task.
 * if a task have run even once, you can not detele the task by this API
 * 如果一个task已经运行过，即被调度过一次以上，那么就不能使用该API进行删除的动作
 * 原因在于运行过的task其中可能有一些资源需要task自己去释放，这种情况要结束task的运行
 * 只能靠task自己从task_entry中退出
 *
 * @param[in]     task_handle
 *
 * @return
 * @retval 0         success
 * @retval other     fail
 *
 * @note       make task body exit by itself is a better choice
 * @authors    deeve
 * @date       2013/10/27
 */
s32 etos_task_delete(etos_task_handle task_handle)
{
    u32 priority_id;
    s32 ret = ETOS_INVALID_PARAM;
    etos_tcb_t *pt_os_task_tcb = (etos_tcb_t *)task_handle;
    etos_init_critical();

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        if (pt_os_task_tcb->priority < ETOS_MAX_PRIORITY_TASK_NUM) {
            priority_id = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb->priority;

            etos_enter_critical();

            /*not run*/
            if ((pt_os_task_tcb->task_state == ETOS_TASK_CREATED) &&
                ((pt_os_task_tcb->stack_begin_addr + pt_os_task_tcb->stack_len)
                 == pt_os_task_tcb->register_stack_pointer)) {
                pt_os_task_tcb->task_state = ETOS_TASK_INVALID;
                _os_task_priority_mask &= ~(1 << priority_id);
                g_os_sched_original_mask &= ~(1 << priority_id);
                ret = ETOS_RET_OK;
            } else { /*return fail when task is running*/
                ret = ETOS_RET_FAIL;
            }

            etos_exit_critical();
        }
    }

    return ret;
}



/**
 * destroy a task in disable interrupt context.
 * destroy a task, release some resource, and it will be invoked
 * at the end of a task automatically in etos
 *
 * @param[in]    task_handle
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note       user do not need to call this API manually
 * @authors    deeve
 * @date       2015/4/15
 */
s32 etos_task_destroy_idic(etos_task_handle task_handle)
{
    u32 priority_id;
    s32 ret = ETOS_INVALID_PARAM;
    etos_tcb_t *pt_os_task_tcb = (etos_tcb_t *)task_handle;

    if (ETOS_TASK_HANDLE_IS_VALID(task_handle)) {
        if (pt_os_task_tcb->priority < ETOS_MAX_PRIORITY_TASK_NUM) {
            priority_id = ETOS_MAX_PRIORITY_TASK_NUM - 1 - pt_os_task_tcb->priority;

            _os_task_priority_mask &= ~(1 << priority_id);
            g_os_sched_original_mask &= ~(1 << priority_id);

            if (pt_os_task_tcb->stack_begin_addr) {
                free(pt_os_task_tcb->stack_begin_addr);
            } else {
                ASSERT(pt_os_task_tcb->stack_begin_addr);
            }
            memset(pt_os_task_tcb, 0, sizeof(etos_tcb_t));
            ret = ETOS_RET_OK;
        }
    }

    return ret;
}



/**
 * get task for schedule moduel.
 * get task control block by priority_id, priority_id is not priority
 * priority_id == ETOS_MAX_PRIORITY_TASK_NUM - 1 - priority
 *
 * @param[in]    priority_id
 *
 * @return   task control block prointer
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2015/4/15
 */
etos_tcb_t *etos_task_get_task(u32 priority_id)
{
    etos_tcb_t *ret = NULL;

    if (priority_id < ETOS_MAX_PRIORITY_TASK_NUM) {
        ret = &_os_task_tcb[priority_id];
    } else {
        ASSERT(priority_id < ETOS_MAX_PRIORITY_TASK_NUM);
    }

    return ret;
}



/* EOF */

