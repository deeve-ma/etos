/******************************************************************************
File    :  boot.S

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os arm boot file
		porting etos on arm

History:

Date           Author       Notes
----------     -------      -------------------------
2013-11-10     deeve        Create

*******************************************************************************/

#include "etos_cfg.h"
#include "etos_arm.h"

/*
 *************************************************************************
 *
 * ARM vector table
 *
 *************************************************************************
 */

.globl _start
_start:
    b       reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq
/*not use b, because of relocate*/

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:  .word not_used
_irq:		.word irq
_fiq:		.word fiq

	.balignl 8,0xaa5555aa

/*
 *************************************************************************
 *
 * necessary variable
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE

.globl _code_start
_code_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start


.globl _bss_end
_bss_end:
	.word __bss_end


_UNCONCERNED_STACK:
	.word	UNCONCERNED_STACK

_OS_BOOT_STACK:
	.word	OS_BOOT_STACK

_IRQ_STACK:
	.word	IRQ_STACK

/*
 *************************************************************************
 *
 * Normal exception handler
 *
 *************************************************************************
 */
	.align
undefined_instruction:
    mov r0, #0x100000
	bl 	not_used

	.align
software_interrupt:
    mov r0, #0x200000
	bl 	not_used

	.align
prefetch_abort:
    mov r0, #0x400000
	bl 	not_used

	.align
data_abort:
    mov r0, #0x800000
	bl 	not_used

	.align
not_used:
	b 	not_used

	.align
fiq:
    mov r0, #0x1000000
	bl 	not_used
/*do not use fiq here, just use irq*/

    .align
/*
 *************************************************************************
 *
 * ARM IRQ 中断发生的时候:
 * 1: 将当前执行程序的下一条指令地址保存到 LR 中,就是将 PC-4 保存到 R14_irq 中
 * 1: 拷贝当前 CPSR 到 SPSR_irq
 * 3: 改变 CPSR 模式位的值,切换到 IRQ 模式 (切换到arm状态,不会是thumb状态)
 * 2: PC 跳到 0x00000018 处运行,因为这是 IRQ 的中断入口
 *    (例外的情况是设置了协处理器CP15,会跳到另外的地址: 0xFFFF0018)
 *  r0-r7 在所有模式下都是相同的寄存器
 *************************************************************************
 */

irq:
	ldr sp, _IRQ_STACK       /*always reset sp, not need support nested interrupt*/
	                         /*and sp is grown in etos_isr_main_idic in each interrupt*/
	stmfd sp!, {r1-r3}       /*入栈r1-r3,这几个寄存器的公用的,先入栈保护起来*/
	mov   r1, sp             /*sp_irq 保存到 r1,因为r1可以在svc模式下访问*/
	add   sp, sp, #12        /*实际sp_irq被我们多压了3个register进去，所以调整一下*/
	sub   r2, lr, #4         /*调整返回地址,被中断的程序的返回地址,还原的时候需要加载到pc中*/
	mrs   r3, spsr           /*此时spsr保存的是被中断程序的cpsr(svc模式下的，是有开中断的)*/
	msr   cpsr_cxsf, #(SVC_MODE|DISABLE_IRQ)
	                         /*切换模式到svc模式，因为这样才能访问sp_svc,只能切换过来压栈*/
	stmfd sp!, {r2}          /*这里的sp已经是svc模式下的sp了,首先保存将来需要还原的pc*/
	stmfd sp!, {r4-r12, lr}  /*再依次入栈svc模式下的 r4-r12和r14(lr), sp=r13不用入栈*/
	ldmfd r1!, {r4-r6}       /*从r1地址处加载数据到r4-r6,r1保存的是sp_irq的值,结合前面的说明知道,
	                           这里完成的动作是 r1-r3 -> r4-r6 */
	stmfd sp!, {r4-r6}       /*相当于入栈 r1-r3 */
	stmfd sp!, {r0}          /*入栈r0, 我们没有动过它的值,可直接入栈*/
	stmfd sp!, {r3}          /*r3保存的是svc模式下被中断的时候的cpsr*/

	ldr r0, =g_os_running_task_num             /*interrupt happened before any task have run*/
	ldr r1, [r0]                               /*r1 = g_os_running_task_num*/
	ldr r2, =g_os_boot_sp                      /*r2 = &g_os_boot_sp*/
	cmp r1, #0                                 /* if (g_os_running_task_num == 0) */
	beq no_running_task                        /*   sp -> g_os_boot_sp            */
	ldr r0, = g_os_current_task_handle         /* else                            */
	ldr r2, [r0]                               /*   sp -> g_os_current_task_handle->register_stack_pointer */
	                                           /*r2 = &g_os_current_task_handle->register_stack_pointer   */
no_running_task:
	str sp, [r2]                               /*保存sp*/

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
 *                              ---> SP(R13) saved in current_task_handle->register_stack_pointer
 *                              ---> or SP(R13) saved in g_os_boot_sp
 */

	msr cpsr_c, #(IRQ_MODE|DISABLE_IRQ) /*切回irq模式并禁用中断*/

	b  etos_isr_main_idic        /*因为禁用了中断,所以不存在嵌套中断的情况,执行完中断后直接进行调度*/
	                             /*在这个函数最终会调用到switch_to_task完成任务切换*/
	                             /*即 etos_isr_main_idic() 不会返回*/

/*
 *************************************************************************
 *
 * reset code at boot
 *
 *************************************************************************
 */

reset:
	mrs r0, cpsr
	bic r0, r0, #(MODE_MASK)       /*clear mode bit*/
	orr r0, r0, #(SVC_MODE)        /*set the cpu to SVC mode*/
	msr cpsr, r0                   /*因为我们在usr mode下无法直接进行模式切换*/

	/*
	 * step 1: board previous settings
	 * 和板子相关,可能你需要修改该段代码
	 */
	ldr     r0, =0x53000000
	mov     r1, #0x0
	str     r1, [r0]   /*turn off the watchdog*/

	mov	r1, #0xffffffff
	ldr	r0, =0x4a000008
	str	r1, [r0]
	ldr	r1, =0x7ff
	ldr	r0, =0x4a00001c
	str	r1, [r0]	   /* mask all interrupt (intmsk & subintmsk)*/

	/*
	 * step 2: low level init
	 */
	mov	r0, #0                  /* cpu related       */
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB      */

	mrc	p15, 0, r0, c1, c0, 0   /* disable MMU stuff and caches   */
	bic	r0, r0, #0x00002300	    /* clear bits 13, 9:8 (--V- --RS) */
	bic	r0, r0, #0x00000087	    /* clear bits 7, 2:0 (B--- -CAM)  */
	orr	r0, r0, #0x00000002     /* set bit 2 (A) Align            */
	orr	r0, r0, #0x00001000	    /* set bit 12 (I) I-Cache         */
	mcr	p15, 0, r0, c1, c0, 0

	bl lowlevel_init           /* 主要是初始化ram等动作
	                            * Position Independent Code, implemented in assembly code
	                            */

	/*
	 * step 3: init stack for all mode, we never use usr mode, must setup before call C code
	 */
	mrs	r0,  cpsr
	bic r0, r0, #(MODE_MASK)

	ldr r2, _UNCONCERNED_STACK

	orr r1, r0, #(UND_MODE|DISABLE_IRQ)
	msr	cpsr_cxsf, r1
	mov sp, r2

	orr r1, r0, #(ABT_MODE|DISABLE_IRQ)
	msr	cpsr_cxsf, r1
	mov sp, r2

	orr r1, r0, #(FIQ_MODE|DISABLE_IRQ)
	msr	cpsr_cxsf, r1
	mov sp, r2

	orr r1, r0, #(SYS_MODE|DISABLE_IRQ)   /*usr mode sp is same with sys mode*/
	msr	cpsr_cxsf, r1
	mov sp, r2

	ldr r2, _OS_BOOT_STACK
    orr r1, r0, #(SVC_MODE|DISABLE_IRQ)
	msr	cpsr_cxsf, r1        /*return to svc mode*/
	mov sp, r2               /*set stack for boot*/
	mov	fp, #0	             /*no previous frame, so fp=0*/

	bl clock_init_ll         /* C code for clock setting, do not use global/static variables */


	/*
	 * step 4: copy code & data to sram
	 */
relocate:				      /* relocate code for nand boot  */
	adr	r0, _start		      /* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		  /* test if we run from flash or RAM */
	cmp     r0, r1
	beq     clear_bss

	ldr	r2, _code_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		      /* r2 <- size of code which need to copy */

	bl copy_code_data_to_ram  /* r0: source, r1: dest, r2: size */

	cmp r0, #0x0              /*check return value*/
	blne led_1_on

	/*
	 * step 5: clear bss
	 * 为正常的C函数执行建立环境
	 */
clear_bss:
	ldr	r0, _bss_start		     /* load start address of bss segment */
	ldr	r1, _bss_end		     /* load end   address of bss segment */
	mov r2, #0x00000000
clr_bss:
    str	r2, [r0]		         /* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clr_bss

	@bl led_2_on

	/*
	 * step 6: jump to main
	 * 现在开始在copy代码后的ram中执行了哦
	 */
	ldr	pc, _main

_main:	.word  main




/*
 *************************************************************************
 *
 * debug led
 *
 *************************************************************************
 */
#define GPIO_CTL_BASE 0x56000000
#define oGPIO_B 0x10
#define oGPIO_CON 0x0
/* R/W, Configures the pins of the port */
#define oGPIO_DAT 0x4
#define oGPIO_UP 0x8
/* R/W, Pull-up disable register */
led_1_on:
	mov	r1, #GPIO_CTL_BASE
	add	r1, r1, #oGPIO_B
	ldr	r2, =0x295551
	str	r2, [r1, #oGPIO_CON]
	mov	r2, #0xff
	str	r2, [r1, #oGPIO_UP]
	ldr	r2, =0x1c0
	str	r2, [r1, #oGPIO_DAT]
	mov	pc, lr

.globl led_2_on
led_2_on:
	mov	r1, #GPIO_CTL_BASE
	add	r1, r1, #oGPIO_B
	ldr	r2, =0x295551
	str	r2, [r1, #oGPIO_CON]
	mov	r2, #0xff
	str	r2, [r1, #oGPIO_UP]
	ldr	r2, =0x1a0
	str	r2, [r1, #oGPIO_DAT]
	mov	pc, lr

