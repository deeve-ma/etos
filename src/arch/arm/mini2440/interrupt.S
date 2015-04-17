/******************************************************************************
File    :  interrupt.S

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
(version 2) as published by the Free Software Foundation. See 
the LICENSE file in the top-level directory for more details.

Description:
		exact time os arm interrupt assembly source

History:

Date           Author       Notes
----------     -------      -------------------------
2013-6-1       deeve        Create

*******************************************************************************/


#include "etos_cfg.h"
#include "etos_arm.h"




/*
 *************************************************************************
 *
 * get arm CPSR
 *
 * u32 os_get_cpu_cpsr(void)
 * r0=return value: cpsr
 *************************************************************************
 */
/* u32 os_get_cpu_cpsr(void)*/
.globl os_get_cpu_cpsr 
os_get_cpu_cpsr:
	mrs   r0,  cpsr
	mov   pc,  lr  


/*
 *************************************************************************
 *
 * get interrupt state
 *
 * u32 os_get_cpu_intr_enabled(void)
 * r0=return value: 0 -- interrupt disabled    nonzero value -- interrupt enabled
 *************************************************************************
 */
.globl os_get_cpu_intr_enabled 
os_get_cpu_intr_enabled:
	mrs   r0,  cpsr
	and   r0,  r0, #(DISABLE_IRQ)              /*only keep IRQ bit*/
	eor   r0,  r0, #(DISABLE_IRQ)              /*异或*/
 	mov   pc,  lr                              /*函数返回*/


/*
 *************************************************************************
 *
 * disable cpu interrupt
 *
 * void os_disable_interrupt(void)
 *************************************************************************
 */ 
.globl os_disable_interrupt
os_disable_interrupt:
	mrs   r0,  cpsr
	orr   r0,  r0, #(DISABLE_IRQ)              /*关中断*/
	msr   cpsr_c, r0 
	mov   pc,  lr                              /*函数返回*/


/*
 *************************************************************************
 *
 * enable cpu interrupt
 *
 * void os_enable_interrupt(void)
 *************************************************************************
 */ 
.globl os_enable_interrupt 
os_enable_interrupt:
	mrs   r0,  cpsr
	bic   r0,  r0, #(DISABLE_IRQ)              /*开中断*/
	msr   cpsr_c, r0 
	mov   pc,  lr                              /*函数返回*/




/*
 *************************************************************************
 *
 * switch to a task in interrupt context
 *
 * etos_tcb_t * os_swtich_to_task_level_and_keep_idic(etos_tcb_t *pt_task_tcb, u32 *sp_start, u32 useless)
 * r0=pt_task_tcb  it is also the return value
 * r1=sp_start     (r1 is the sp which task will start)
 * r2=useless
 *************************************************************************
 */
.globl os_swtich_to_task_level_and_keep_idic 
os_swtich_to_task_level_and_keep_idic:
	mov   r2,   lr
	msr   cpsr, #(SVC_MODE|DISABLE_IRQ)  /*先切换回svc模式*/
	mov   sp,   r1                       /*设置sp值*/
	mov   pc,   r2                       /*函数返回*/


/*
 *************************************************************************
 *
 * switch to boot code, not need save current context and never return
 *
 * void os_switch_to_boot_code(u32 *sp_dest)
 * r0=sp_dest  (r0 is the sp which task need switch to)
 *************************************************************************
 */
.globl os_switch_to_boot_code 
os_switch_to_boot_code:
	msr   cpsr_c, #(SVC_MODE|DISABLE_IRQ)   /*先切换回svc模式*/
	mov   sp,   r0                      /*还原sp值*/
	ldmfd sp!,  {r0}                      /*最先还原的是cpsr*/
	msr   spsr_cxsf, r0                        /*为什么不直接送cpsr呢,因为下面的ldm指令中包含pc*/
	ldmfd sp!, {r0-r12, lr, pc}^          /*有pc会额外同时做一个动作: spsr -> cpsr*/
	                                      /*再依次还原r0-r12,lr和pc*/

/*
 *************************************************************************
 *
 * switch to a task in interrupt context, never return
 *
 * void os_switch_to_task(u32 *sp_dest)
 * r0=sp_dest  (r0 is the sp which task need switch to)
 *************************************************************************
 */
.globl os_switch_to_task 
os_switch_to_task:
	msr   cpsr, #(SVC_MODE|DISABLE_IRQ)   /*先切换回svc模式*/
	mov   sp,   r0                        /*还原sp值*/
	ldmfd sp!,  {r0}                      /*最先还原的是cpsr*/
	msr   spsr, r0                        /*为什么不直接送cpsr呢,因为下面的ldm指令中包含pc*/
	ldmfd sp!,  {r0-r12, lr, pc}^         /*有pc会额外同时做一个动作: spsr -> cpsr*/
	                                      /*再依次还原r0-r12,lr和pc*/

/*
 *************************************************************************
 *
 * switch task to start
 * 
 * s32 os_save_task_and_start_task(u32  **sp_from,
 *                                 etos_tcb_t *pt_os_task_tcb_next,
 *                                 void (*task_start_entry)(etos_tcb_t *pt_os_task_tcb),
 *                                 u32 useless)
 * r0=sp_from  r1=pt_os_task_tcb_next r2=task_start_entry r3=useless
 *************************************************************************
 */
.globl os_save_task_and_start_task 
os_save_task_and_start_task:
	msr   cpsr_c, #(SVC_MODE|DISABLE_IRQ) /*先关中断*/
	
    /*先入栈保护当前task的信息*/
	add   r3, pc, #40              /*r3为pc为下面的nop指令的位置*/
	stmfd sp!, {r3}                /*先入栈下面nop指令的pc*/	
	stmfd sp!, {r0-r12, lr}        /*入栈 r0-r12, lr*/
	mrs   r3,  cpsr
	bic   r3,  r3, #(MODE_MASK)     /*clear mode bit, 开中断*/
	orr   r3,  r3, #(SVC_MODE)      /*set the cpu to SVC mode*/
	stmfd sp!, {r3}                 /*入栈 cpsr*/

	str sp, [r0]                    /*保存sp到sp_from*/
	
/* The stack frame is assumed to look as follows:
 *
 *							    Interrupted stack           (high memory)
 *                              PC (need to restore in future)
 *                              LR(R14)
 *                              R12
 *                              R11
 *                              R10
 *                              R9
 *                              R8
 *                              R7
 *                              R6
 *                              R5
 *                              R4
 *                              R3
 *                              R2
 *                              R1
 *                              R0 : argument
 *                              CPSR : interrupted status   (low memory)
 *                              ---> SP(R13) saved in sp_from
 */	
	/*设置新的sp并调用task*/
	add   r3, r1, #4               /*r3 = pt_os_task_tcb_next->stack_begin_addr*/
	mov   sp, r3                   /*设置新的task的sp值*/
	                               /*invoke task_start_entry(pt_os_task_tcb_next)*/
	mov   r0, r1                   /*设置参数 pt_os_task_tcb_next*/
	mov   pc, r2                   /*never return to here*/
	nop
	nop                            /*当sp_from对应的task再次被切换回来的时候,会从这里开始执行哦*/
	mov r0, #0                     /*os_save_task_and_start_task()函数返回值*/
    mov pc, lr                     /*函数返回*/



	


/*
 *************************************************************************
 *
 * switch task context in svc mode
 * 
 * s32 os_switch_task_context(u32  **sp_from, u32  *sp_to, u32 useless)
 * r0=sp_from  r1=sp_to r2=useless (useless主要拿来做临时变量用,传任何值无影响)
 *************************************************************************
 */
.globl os_switch_task_context 
os_switch_task_context:
	msr   cpsr_c, #(SVC_MODE|DISABLE_IRQ) /*先关中断*/
	
    /*先入栈保护当前task的信息*/
	add   r2,  pc, #40             /*r2为pc为下面的nop指令的位置*/
	stmfd sp!, {r2}                /*先入栈下面nop指令的pc*/	
	stmfd sp!, {r0-r12, lr}        /*入栈 r0-r12, lr*/
	mrs   r2,  cpsr
	bic   r2,  r2, #(MODE_MASK)     /*clear mode bit, 开中断*/
	orr   r2,  r2, #(SVC_MODE)      /*set the cpu to SVC mode*/
	stmfd sp!, {r2}                 /*入栈 cpsr*/

	str sp, [r0]                    /*保存sp到sp_from*/
	
/* The stack frame is assumed to look as follows:
 *
 *							    Interrupted stack           (high memory)
 *                              PC (need to restore in future)
 *                              LR(R14)
 *                              R12
 *                              R11
 *                              R10
 *                              R9
 *                              R8
 *                              R7
 *                              R6
 *                              R5
 *                              R4
 *                              R3
 *                              R2
 *                              R1
 *                              R0 : argument
 *                              CPSR : interrupted status   (low memory)
 *                              ---> SP(R13) saved in sp_from
 */	
 
	/*从sp_to出栈需要切换过去的context*/
	mov   sp,   r1                 /*还原sp值*/
	ldmfd sp!,  {r0}               /*最先还原的是cpsr*/
	msr   spsr, r0                 /*为什么不直接送cpsr呢,因为下面的ldm指令中包含pc*/
	ldmfd sp!,  {r0-r12, lr, pc}^  /*有pc会额外同时做一个动作: spsr -> cpsr*/
	                               /*再依次还原r0-r12,lr和pc*/
	nop
	nop                            /*当sp_from对应的task再次被切换回来的时候,会从这里开始执行哦*/
	mov r0, #0                     /*switch_task_context()函数返回值*/
    mov pc, lr                     /*函数返回*/


   
