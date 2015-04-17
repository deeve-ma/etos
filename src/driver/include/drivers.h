/******************************************************************************
File    :  drivers.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		header files for all drivers

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25      deeve        Create

*******************************************************************************/
#ifndef __DRIVER_H__
#define __DRIVER_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "clock_power.h"
#include "gpio.h"
#include "uart.h"
#include "interrupt_hw.h"
#include "timer.h"
/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define UART_DIRECT_TO_SEND       (1)      /* uart tx always tx done when it is set to 1 */
/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __DRIVER_H__ */

/* EOF */

