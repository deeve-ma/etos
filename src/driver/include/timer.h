/******************************************************************************
File    :  timer.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		timer and pwm control header file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-14      deeve        Create

*******************************************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/*TIMER CONFIGURATION REGISTER0*/
#define TCFG0            0x51000000
/*
Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
{prescaler value} = 0~255
{divider value} = 2, 4, 8, 16
*/

#define TCFG0_PRESCALER0_FROM            (0)
#define TCFG0_PRESCALER0_BITS            (8)

#define TCFG0_PRESCALER1_FROM            (8)
#define TCFG0_PRESCALER1_BITS            (8)

#define TCFG0_DEAD_ZONE_LEN_FROM         (16)
#define TCFG0_DEAD_ZONE_LEN_BITS         (8)



/*TIMER CONFIGURATION REGISTER1*/
#define TCFG1            0x51000004

#define TCFG1_MUX0_FROM            (0)
#define TCFG1_MUX0_BITS            (4)

#define TCFG1_MUX1_FROM            (4)
#define TCFG1_MUX1_BITS            (4)

#define TCFG1_MUX2_FROM            (8)
#define TCFG1_MUX2_BITS            (4)

#define TCFG1_MUX3_FROM            (12)
#define TCFG1_MUX3_BITS            (4)

#define TCFG1_MUX4_FROM            (16)
#define TCFG1_MUX4_BITS            (4)

#define TCFG1_DMA_MODE_FROM        (20)
#define TCFG1_DMA_MODE_BITS        (4)



/*TIMER CONTROL (TCON) REGISTER*/
#define TCON             0x51000008


#define TCON_TIMER0_START_BIT    (0)
#define TCON_TIMER0_MANUAL_UPDATE_BIT    (1)
#define TCON_TIMER0_OUTPUT_INVERTER_BIT  (2)
#define TCON_TIMER0_AUTO_RELOAD_BIT  (3)


/*x>0*/
#define TCON_TIMERx_START_BIT(x)    (4*(x)+4)

#define TCON_TIMERx_MANUAL_UPDATE_BIT(x)    (4*(x)+5)

/*x>0 && x<4*/
#define TCON_TIMERx_OUTPUT_INVERTER_BIT(x)  (4*(x)+6)

/*x>0 && x<4*/
#define TCON_TIMERx_AUTO_RELOAD_BIT(x)  (4*(x)+7)

#define TCON_TIMER4_AUTO_RELOAD_BIT     (22)



/*TIMER 0 COUNT BUFFER REGISTER*/
#define TCNTB0              0x5100000c

/*TIMER 0 COMPARE BUFFER REGISTER*/
#define TCMPB0              0x51000010

/*TIMER 0 COUNT OBSERVATION REGISTER*/
#define TCNTO0              0x51000014
/*it is a read only register*/


/*TIMER 1 COUNT BUFFER REGISTER*/
#define TCNTB1              0x51000018

/*TIMER 1 COMPARE BUFFER REGISTER*/
#define TCMPB1              0x5100001c

/*TIMER 1 COUNT OBSERVATION REGISTER*/
#define TCNTO1              0x51000020
/*it is a read only register*/


/*TIMER 2 COUNT BUFFER REGISTER*/
#define TCNTB2              0x51000024

/*TIMER 2 COMPARE BUFFER REGISTER*/
#define TCMPB2              0x51000028

/*TIMER 2 COUNT OBSERVATION REGISTER*/
#define TCNTO2              0x5100002c
/*it is a read only register*/


/*TIMER 3 COUNT BUFFER REGISTER*/
#define TCNTB3              0x51000030

/*TIMER 3 COMPARE BUFFER REGISTER*/
#define TCMPB3              0x51000034

/*TIMER 3 COUNT OBSERVATION REGISTER*/
#define TCNTO3              0x51000038
/*it is a read only register*/


/*TIMER 4 COUNT BUFFER REGISTER*/
#define TCNTB4              0x5100003c

/*TIMER 4 COUNT OBSERVATION REGISTER*/
#define TCNTO4              0x51000040
/*it is a read only register*/



/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

s32 timer_hw_config_pinmux(u32 timer_no);

s32 timer_hw_config_timer(u32 timer_no);

s32 timer_hw_start_timer(u32 timer_no);

s32 timer_hw_stop_timer(u32 timer_no);


#endif  /* __TIMER_H__ */

/* EOF */

