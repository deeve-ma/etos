/******************************************************************************
File    :  etos_arm.h

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		arm support header file for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-6-22      deeve        Create

*******************************************************************************/
#ifndef __ETOS_ARM_H__
#define __ETOS_ARM_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define USR_MODE     0x10
#define FIQ_MODE     0x11
#define IRQ_MODE     0x12
#define SVC_MODE     0x13
#define ABT_MODE     0x17
#define UND_MODE     0x1b
#define SYS_MODE     0x1f
#define MODE_MASK    0x1f
#define DISABLE_IRQ  0x80  /*only care irq, not fiq*/


/* 总共使用4M的memory，最高地址的4.5k用于boot/exception stack */

#define UNCONCERNED_STACK                   (TEXT_BASE + 0x400000)
#define UNCONCERNED_STACK_LEN               (512)

#define IRQ_STACK                           (UNCONCERNED_STACK - UNCONCERNED_STACK_LEN)   /*0x303FFE00 if TEXT_BASE=0x30000000*/
#define IRQ_STACK_LEN                       (2048)


#define OS_BOOT_STACK                       (IRQ_STACK - IRQ_STACK_LEN)  /*0x303FF600 if TEXT_BASE=0x30000000*/
#define OS_BOOT_STACK_LEN                   (2048)

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_ARM_H__ */

/* EOF */

