/******************************************************************************
File    :  gpio.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		gpio driver header file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25     deeve        Create

*******************************************************************************/
#ifndef __GPIO_H__
#define __GPIO_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

#include "etos_cfg.h"
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/*
Pinmux register: PnCON(port control register) determines which function is used for each pin
                 GPACON-GPJCON

port data register: PnDAT is used to input/output port data
                    GPADAT-GPJDAT

port pull_up regiser: PnUP is used to enable/disable pull up function (write:0/1)
                     GPBUP-GPJUP
*/


#define GPACON   (0x56000000)
#define GPADAT   (0x56000014)


#define GPBCON   (0x56000010)
#define GPBDAT   (0x56000014)
#define GPBUP    (0x56000018)


#define GPCCON   (0x56000020)
#define GPCDAT   (0x56000024)
#define GPCUP    (0x56000028)


#define GPDCON   (0x56000030)
#define GPDDAT   (0x56000034)
#define GPDUP    (0x56000038)


#define GPECON   (0x56000040)
#define GPEDAT   (0x56000044)
#define GPEUP    (0x56000048)


#define GPFCON   (0x56000050)
#define GPFDAT   (0x56000054)
#define GPFUP    (0x56000058)


#define GPGCON   (0x56000060)
#define GPGDAT   (0x56000064)
#define GPGUP    (0x56000068)


#define GPHCON   (0x56000070)
#define GPHDAT   (0x56000074)
#define GPHUP    (0x56000078)


#define GPJCON   (0x560000d0)
#define GPJDAT   (0x560000d4)
#define GPJUP    (0x560000d8)



/*field define*/

#define GPACON_OUTPUT       (0)


#define GPxCON_PORTn_FROM(port_n)  (port_n<<1)  /*port_n * 2*/
#define GPxCON_PORTn_BITS          (2)


#define GPxCON_INPUT        (0)
#define GPxCON_OUTPUT       (1)


#define GPxDAT_PORTn_FROM(port_n)  (port_n)  /*port_n*/
#define GPxDAT_PORTn_BITS          (1)


#define GPxDAT_HIGH         (1)
#define GPxDAT_LOW          (0)


#define GPxUP_PORTn_FROM(port_n)  (port_n)  /*port_n*/
#define GPxUP_PORTn_BITS          (1)


#define GPxUP_ENABLE        (0)
#define GPxUP_DISABLE       (1)


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/


void set_led_on(u32 led_x);
void set_led_off(u32 led_x);

void set_beep_on(void);
void set_beep_off(void);



#endif  /* __GPIO_H__ */

/* EOF */

