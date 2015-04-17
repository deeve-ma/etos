/******************************************************************************
File    :  gpio.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		gpio control

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"
#include "gpio.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
void set_led_on(u32 led_x)
{
    u32 led_con_from = GPxCON_PORTn_FROM(5) + 2 * (led_x - 1);
    u32 led_data_from = GPxDAT_PORTn_FROM(5) + led_x - 1;
    u32 led_up_from = GPxUP_PORTn_FROM(5) + led_x - 1;

    if ((led_x >= 1) && (led_x <= 4)) {
        /*disable pull up*/
        REG_SET_BIT(GPBUP, led_up_from);

        /*set port to output*/
        REG_SET_FIELD(GPBCON, led_con_from, GPxCON_PORTn_BITS, GPxCON_OUTPUT);

        /*set output low -> led on*/
        REG_CLR_BIT(GPBDAT, led_data_from);
    }
}

void set_led_off(u32 led_x)
{
    u32 led_con_from = GPxCON_PORTn_FROM(5) + 2 * (led_x - 1);
    u32 led_data_from = GPxDAT_PORTn_FROM(5) + led_x - 1;
    u32 led_up_from = GPxUP_PORTn_FROM(5) + led_x - 1;

    if ((led_x >= 1) && (led_x <= 4)) {
        /*disable pull up*/
        REG_SET_BIT(GPBUP, led_up_from);

        /*set port to output*/
        REG_SET_FIELD(GPBCON, led_con_from, GPxCON_PORTn_BITS, GPxCON_OUTPUT);

        /*set output high -> led off*/
        REG_SET_BIT(GPBDAT, led_data_from);
    }
}

void set_beep_on(void)
{
    /*GPB0 -> output*/

    /*disable pull up*/
    REG_SET_BIT(GPBUP, 0);

    /*set port to output*/
    REG_SET_FIELD(GPBCON, 0, GPxCON_PORTn_BITS, GPxCON_OUTPUT);

    /*set output high -> led off*/
    REG_SET_BIT(GPBDAT, 0);
}


void set_beep_off(void)
{
    /*GPB0 -> output*/

    /*disable pull up*/
    REG_SET_BIT(GPBUP, 0);

    /*set port to output*/
    REG_SET_FIELD(GPBCON, 0, GPxCON_PORTn_BITS, GPxCON_OUTPUT);

    /*set output high -> led off*/
    REG_CLR_BIT(GPBDAT, 0);
}


/* EOF */

