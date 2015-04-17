/******************************************************************************
File    :  etos_asm_types.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		ETOS data type define
		不同的架构可能需要不同的定义，即需要移植

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-12     deeve        Create

*******************************************************************************/
#ifndef __ETOS_ASM_TYPES_H__
#define __ETOS_ASM_TYPES_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

/*理论上, 该文件应该是最先被include的文件, except etos_cfg.h*/

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

typedef unsigned char     u8;           /*无符号8位整型类型       */
typedef char              s8;           /*有符号8位整型类型       */
typedef unsigned short    u16;          /*无符号16位整型类型      */
typedef short             s16;          /*有符号16位整型类型      */
typedef unsigned long     u32;          /*无符号32位整型类型      */
typedef long              s32;          /*有符号32位整型类型      */

/*ETOS 不会用到浮点数，所以不要定义*/

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_ASM_TYPES_H__ */

/* EOF */

