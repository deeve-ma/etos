/******************************************************************************
File    :  etos_task.h

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
2015-4-15      deeve        Remove unnecessary code and add comments


*******************************************************************************/
#ifndef __ETOS_TASK_H__
#define __ETOS_TASK_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_mem.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

#define ETOS_TASK_HANDLE_IS_VALID(h)  ((h) && (((etos_tcb_t*)(h))->task_handle == (h)))

typedef void *(*func_entry)(void *arg);

typedef enum _etos_task_state_e {
    ETOS_TASK_INVALID = 0x00,
    ETOS_TASK_CREATED = 0x01,
    ETOS_TASK_RUNNING = 0x02,          /* 正在占用CPU的task的state */
    ETOS_TASK_INTERRUPTED = 0x04,      /* 被中断打断的task */
    ETOS_TASK_READY = 0x08,            /* state between pending and runniing */
    ETOS_TASK_PENDING_SLEEP = 0x10,    /* pending by sleep */
    ETOS_TASK_PENDING_SELF = 0x20,     /* release cpu by itself manually */
    ETOS_TASK_PENDING_MSG = 0x40,      /* pending because of receive message */
    ETOS_TASK_END = 0x80,              /* task function reach to its end (return)*/
#if (ETOS_ENABLE_EXACT_TASK)
    ETOS_TASK_PEND
#endif
} etos_task_state_e;


typedef struct _etos_tcb_t {
    u32  *register_stack_pointer;     //task 当前的堆栈指针
    u32  *stack_begin_addr;           //task 的堆栈
    u32  stack_len;                   //stack length
#if (ETOS_ENABLE_EXACT_TASK)
    list_t  list;                     //just for exact task，用来串联exact task的双向链表的结构
    u32  schedule_tick;               //just for exact task，精确调用该task的时间:tick
    BOOL auto_expand;
#endif
    u32  priority;                    //值越大，优先级越高.但是和mask不是按bit对应的
    func_entry task_entry;            //task 的函数入口
    void *arg;                        //传给task_entry的参数
    etos_task_handle  task_handle;    //create task的返回值
    etos_task_state_e  task_state;    //task 的状态
    char task_name[ETOS_MAX_TASK_NAME_LEN];
} etos_tcb_t;



/******************************************************************************
 *                                 Declar Functions                           *
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
s32 etos_task_init(void);



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
s32 etos_task_deinit(void);



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
                     void *arg,  u32 stack_len, etos_task_handle *task_handle);



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
s32 etos_task_delete(etos_task_handle task_handle);



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
s32 etos_task_destroy_idic(etos_task_handle task_handle);



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
etos_tcb_t *etos_task_get_task(u32 priority_id);



#endif  /* __ETOS_TASK_H__ */

/* EOF */

