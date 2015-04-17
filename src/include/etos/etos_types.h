/******************************************************************************
File    :  etos_types.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
(version 2) as published by the Free Software Foundation. See 
the LICENSE file in the top-level directory for more details.

Description:
		ETOS data type define,不同的架构可能需要不同的定义，即需要移植

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-12     deeve        Create

*******************************************************************************/
#ifndef __ETOS_TYPES_H__
#define __ETOS_TYPES_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

#include "etos_asm_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
 
#ifndef BYTE
#define BYTE      u8
#endif

#ifndef BOOL
#define BOOL      u8
#endif


#ifndef TRUE
#define TRUE      (1)
#endif

#ifndef FALSE
#define FALSE     (0)
#endif


#define NULL ((void*)0)


#define ETOS_RET_OK                  (0)
#define ETOS_INVALID_PARAM           (-1)
#define ETOS_NOT_INIT                (-2)
#define ETOS_NOT_SUPPORT             (-4)
#define ETOS_NO_MEM                  (-8)
#define ETOS_RET_FAIL                (-16)


typedef u32 etos_tick;
typedef u32 etos_task_handle;



/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_TYPES_H__ */

/* EOF */

