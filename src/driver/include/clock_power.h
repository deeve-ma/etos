/******************************************************************************
File    :  clock_power.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		header file for clock power  module

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-26      deeve        Create

*******************************************************************************/
#ifndef __CLOCK_POWER_H__
#define __CLOCK_POWER_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define LOCKTIME               (0x4C000000)
#define LOCKTIME_DEFAULT       (0xffffffff)

#define LOCKTIME_M_LTIME_FROM  (0)
#define LOCKTIME_M_LTIME_BITS  (16)

#define LOCKTIME_U_LTIME_FROM  (16)
#define LOCKTIME_U_LTIME_BITS  (16)



#define MPLLCON     0x4C000004

#define UPLLCON     0x4C000008


#define xPLLCON_SDIV_FROM     (0)
#define xPLLCON_SDIV_BITS     (2)

#define xPLLCON_PDIV_FROM     (4)
#define xPLLCON_PDIV_BITS     (6)

#define xPLLCON_MDIV_FROM     (12)
#define xPLLCON_MDIV_BITS     (8)


/*
 * 开发板将OM[3:2]固定接地，即clock来源是外部晶振:12M
 * S3C2440有两个PLL:
 *   MPLL:  用于产生FCLK(CPU使用)， HCLK(AHB)， PCLK(APB)三种频率
 *   UPLL:  专门用于驱动USB host/Device, 且必须为48MHz
 * 在系统复位之后，如果没有设定PLL，则采用外部晶振的频率作为FCLK，同时FCLK:HCLK:PCLK的比例关系为1:1:1
 * 通过改变CLKDIVN可以改变FCLK,HCLK,PCLK的分频比
 *
 * MPLL = (2 * m * Fin) / (p * 2^s)   => FCLK
 * m = (MDIV + 8), p = (PDIV + 2), s = SDIV
 * MDIV/PDIV/SDIV in register MPLLCON(0x4C000004)
 *
 * UPLL = (m * Fin) / (p * 2)
 * m = (MDIV + 8), p = (PDIV + 2), s = SDIV
 */

#define S3C2440_405MHZ_MPLL_MDIV     (0x7f)
#define S3C2440_405MHZ_MPLL_PDIV     (2)
#define S3C2440_405MHZ_MPLL_SDIV     (1)


#define S3C2440_400MHZ_MPLL_MDIV     (92)
#define S3C2440_400MHZ_MPLL_PDIV     (1)
#define S3C2440_400MHZ_MPLL_SDIV     (1)

#define S3C2440_200MHZ_MPLL_MDIV     (92)
#define S3C2440_200MHZ_MPLL_PDIV     (1)
#define S3C2440_200MHZ_MPLL_SDIV     (2)


#define S3C2440_MPLL_CLK     ((S3C2440_405MHZ_MPLL_MDIV<<xPLLCON_MDIV_FROM)|(S3C2440_405MHZ_MPLL_PDIV<<xPLLCON_PDIV_FROM)|S3C2440_405MHZ_MPLL_SDIV)


#define S3C2440_96MHZ_UPLL_MDIV     (0x38)
#define S3C2440_96MHZ_UPLL_PDIV     (2)
#define S3C2440_96MHZ_UPLL_SDIV     (1)

#define S3C2440_UPLL_CLK     ((S3C2440_96MHZ_UPLL_MDIV<<xPLLCON_MDIV_FROM)|(S3C2440_96MHZ_UPLL_PDIV<<xPLLCON_PDIV_FROM)|S3C2440_96MHZ_UPLL_SDIV)



#define CLKCON      0x4C00000C
#define CLKCON_DEFAULT        (0xfffff0)

#define CLKCON_IDLE_BIT       (2)
#define CLKCON_SLEEP_BIT      (3)
#define CLKCON_NAND_FLASH_BIT (4)
#define CLKCON_LCDC_BIT       (5)
#define CLKCON_USB_HOST_BIT   (6)
#define CLKCON_USB_DEVICE_BIT (7)
#define CLKCON_PWMTIMER_BIT   (8)
#define CLKCON_SDI_BIT        (9)
#define CLKCON_UART0_BIT      (10)
#define CLKCON_UART1_BIT      (11)
#define CLKCON_UART2_BIT      (12)
#define CLKCON_GPIO_BIT       (13)
#define CLKCON_RTC_BIT        (14)
#define CLKCON_ADC_BIT        (15)
#define CLKCON_IIC_BIT        (16)
#define CLKCON_IIS_BIT        (17)
#define CLKCON_SPI_BIT        (18)
#define CLKCON_CAMERA_BIT     (19)
#define CLKCON_AC97_BIT       (20)




#define CLKSLOW      0x4C000010


#define CLKDIVN      0x4C000014

#define CLKDIVN_PDIVN_BIT      (0)

#define CLKDIVN_HDIVN_FROM      (1)
#define CLKDIVN_HDIVN_BITS      (2)

#define CLKDIVN_DIVN_UPLL_BIT   (3)


/* FCLK:HCLK:PCLK = 1:4:4 */
#define S3C2440_CLKDIV          ((1<<CLKDIVN_DIVN_UPLL_BIT)|(2<<CLKDIVN_HDIVN_FROM)|0)




#define CAMDIVN      0x4C000018

#define CAMDIVN_DIV_FROM         (0)
#define CAMDIVN_DIV_BITS         (4)

#define CAMDIVN_DIV_SEL_BIT      (4)

#define CAMDIVN_HCLK3_HALF_BIT   (8)

#define CAMDIVN_HCLK4_HALF_BIT   (9)

#define CAMDIVN_DVS_EN_BIT       (12)


/******************************************************************************
 *                                 Declar Functions                           *
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
void clock_init(void);

#endif  /* __CLOCK_POWER_H__ */

/* EOF */

