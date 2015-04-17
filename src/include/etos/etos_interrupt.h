/******************************************************************************
File    :  etos_interrupt.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		etos interrupt process header file
		exact time os 通用中断处理函数部分

History:

Date           Author       Notes
----------     -------      -------------------------
2013-11-23     deeve        Create
2015-4-14      deeve        Add some comments
2015-4-15      deeve        Add etos_intr_in_isr()

*******************************************************************************/
#ifndef __ETOS_INTERRUPT_H__
#define __ETOS_INTERRUPT_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

#include "etos_cfg.h"
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

#define ETOS_INTR_VIRTUAL_TASK_HANDLE        (0xf0beef)


typedef enum _etos_isr_ret {
    ETOS_ISR_RESCHEDULE_DISABLE = 0,
    ETOS_ISR_RESCHEDULE_ENABLE  = 1,
    ETOS_ISR_RESCHEDULE_UPDATE_TICK = 2
} etos_isr_ret_e;


typedef etos_isr_ret_e (*pfunc_user_isr)(u32 current_tick);

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/


/* -->  function below is implemented in assembly code --> start*/

extern u32 os_get_cpu_intr_enabled(void);

extern void os_disable_interrupt(void);

extern void os_enable_interrupt(void);

/*
 * 以上三个函数是需要移植的函数，一般需用汇编实现
 * 实现文件应该位于arch目录下
 */

/* -->  function above is implemented in assembly code --> end*/





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
void etos_disable_cpu_interrupt(void);


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
void etos_enable_cpu_interrupt(void);



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
s32 etos_register_interrupt_dispatcher(pfunc_user_isr intr_dispatcher);



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
void etos_intr_disable_cpu_interrupt(u32 *state);



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
void etos_intr_enable_cpu_interrupt(u32 *state);



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
inline BOOL etos_intr_in_isr(void);


#endif  /* __ETOS_INTERRUPT_H__ */

/* EOF */

