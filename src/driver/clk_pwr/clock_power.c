/******************************************************************************
File    :  clock_power.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		clock power module

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-26      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "clock_power.h"

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
/**
 * init mcu clock.
 * config system clock: FCLK HCLK PCLK
 *
 * @param[in]    void
 *
 * @return       none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/12
 */
void clock_init(void)
{
    u32 i;

    REG_SET_VALUE(LOCKTIME, LOCKTIME_DEFAULT);

    REG_SET_VALUE(CLKDIVN, S3C2440_CLKDIV);

    /* 如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode” */
    __asm__(    "mrc    p15, 0, r1, c1, c0, 0\n"    /* read ctrl register   */
                "orr    r1, r1, #0xc0000000\n"      /* Asynchronous         */
                "mcr    p15, 0, r1, c1, c0, 0\n"    /* write ctrl register  */
                :::"r1"
           );

    REG_SET_VALUE(UPLLCON, S3C2440_UPLL_CLK);

    /*for some delay*/
    for (i = 0; i < 100; i++);

    REG_SET_VALUE(MPLLCON, S3C2440_MPLL_CLK);

    /*for some delay*/
    for (i = 0; i < 8000; i++);


    //REG_SET_VALUE(CLKCON, CLKCON_DEFAULT);

    REG_CLR_BIT(CAMDIVN, CAMDIVN_HCLK3_HALF_BIT);
    REG_CLR_BIT(CAMDIVN, CAMDIVN_HCLK4_HALF_BIT);
}


/* EOF */

