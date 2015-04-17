/******************************************************************************
File    :  etos_includes.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os all header files

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-12     deeve        Create

*******************************************************************************/
#ifndef __ETOS_INCLUDES_H__
#define __ETOS_INCLUDES_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"
#include "etos_listop.h"
#include "etos_mem.h"
#include "etos_task.h"
#include "etos_schedule.h"
#include "etos_interrupt.h"
#include "etos_msgq.h"
#include "etos_sleep.h"
#include "etos_utility.h"
#include "etos_hw_op.h"
#include "etos_gioi_interface.h"
#include "etos_random.h"
#include "printf.h"
#include "xlog.h"


/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/*etos do not impliment mutex and semaphore, just turn on/off interrupt*/
#define etos_init_critical()    u32 __intr_enalbed_already
#define etos_enter_critical()   etos_intr_disable_cpu_interrupt(&__intr_enalbed_already)
#define etos_exit_critical()    etos_intr_enable_cpu_interrupt(&__intr_enalbed_already)


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_INCLUDES_H__ */

/* EOF */

