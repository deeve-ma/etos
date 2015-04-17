/******************************************************************************
File    :  interrupt_hw.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		board interrupt control header file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-3       deeve        Create

*******************************************************************************/
#ifndef __INTERRUPT_HW_H__
#define __INTERRUPT_HW_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define SRCPND        (0x4A000000)
/*each bit correspond to INTRCTL_XXX..*/
/*
0 = The interrupt has not been requested.
1 = The interrupt source has asserted the interrupt request.
*/



#define INTMOD        (0x4A000004)
/*each bit correspond to INTRCTL_XXX..*/
/*
0 = IRQ mode             1 = FIQ mode
*/
#define SRCPND_DEFAULT_VALUE    (0x00000000)



#define INTMSK        (0x4A000008)
/*each bit correspond to INTRCTL_XXX..*/
/*
0 = Interrupt service is available.
1 = Interrupt service is masked.
*/
#define INTMSK_DEFAULT_VALUE    (0xFFFFFFFF)



#define INTPND        (0x4A000010)
/*each bit correspond to INTRCTL_XXX.., only for IRQ, not for FIQ*/
/*
0 = The interrupt has not been requested.
1 = The interrupt source has asserted the interrupt request.
*/



#define INTOFFSET     (0x4A000014)
/*each bit correspond to INTRCTL_XXX..*/
/*
only for read
Indicate the IRQ interrupt request source
*/


#define INTRCTL_EINT0_BIT       (0)
#define INTRCTL_EINT1_BIT       (1)
#define INTRCTL_EINT2_BIT       (2)
#define INTRCTL_EINT3_BIT       (3)
#define INTRCTL_EINT4_7_BIT     (4)
#define INTRCTL_EINT8_23_BIT    (5)
#define INTRCTL_INT_CAM_BIT     (6)
#define INTRCTL_nBATT_FLT_BIT   (7)
#define INTRCTL_INT_TICK_BIT    (8)
#define INTRCTL_WDT_AC97_BIT    (9)
#define INTRCTL_INT_TIMER0_BIT  (10)
#define INTRCTL_INT_TIMER1_BIT  (11)
#define INTRCTL_INT_TIMER2_BIT  (12)
#define INTRCTL_INT_TIMER3_BIT  (13)
#define INTRCTL_INT_TIMER4_BIT  (14)
#define INTRCTL_INT_UART2_BIT   (15)
#define INTRCTL_INT_LCD_BIT     (16)
#define INTRCTL_INT_DMA0_BIT    (17)
#define INTRCTL_INT_DMA1_BIT    (18)
#define INTRCTL_INT_DMA2_BIT    (19)
#define INTRCTL_INT_DMA3_BIT    (20)
#define INTRCTL_INT_SDI_BIT     (21)
#define INTRCTL_INT_SPI0_BIT    (22)
#define INTRCTL_INT_UART1_BIT   (23)
#define INTRCTL_INT_NFCON_BIT   (24)
#define INTRCTL_INT_USBD_BIT    (25)
#define INTRCTL_INT_USBH_BIT    (26)
#define INTRCTL_INT_IIC_BIT     (27)
#define INTRCTL_INT_UART0_BIT   (28)
#define INTRCTL_INT_SPI1_BIT    (29)
#define INTRCTL_INT_RTC_BIT     (30)
#define INTRCTL_INT_ADC_BIT     (31)

#define INTR_MAX_NUM            (32)



#define PRIORITY        (0x4A00000c)
/*
IRQ priority control register
如果ARB_SEL位是 00b，优先级是REQ0，REQ1，REQ2，REQ3，REQ4，和REQ5.
如果ARB_SEL位是 01b，优先级是REQ0，REQ2，REQ3，REQ4，REQ1，和REQ5.
如果ARB_SEL位是 10b，优先级是REQ0，REQ3，REQ4，REQ1，REQ2，和REQ5.
如果ARB_SEL位是 11b，优先级是REQ0，REQ4，REQ1，REQ2，REQ3，和REQ5.
注意仲裁器的 REQ0 总是有最高优先级，REQ5 总是有最低优先级。此外通过改变
ARB_SEL 位，我们可以翻转 REQ1 到 REQ4 的优先级。
如果 ARB_MODE 位是 1，ARB_SEL 位以翻转的方式改变。例如如果 REQ1 被服务，则
ARB_SEL位自动的变为01b，把REQ1放到最低的优先级
如果REQ0 或REQ5 被服务，ARB_SEL位完全不会变化。
如果REQ1 被服务，ARB_SEL位变为 01b
如果REQ2 被服务，ARB_SEL位变为 10b
如果REQ3 被服务，ARB_SEL位变为 11b
如果REQ4 被服务，ARB_SEL位变为 00b
*/
#define PRIORITY_DEFAULT_VALUE  (0x7f)

#define PRIORITY_ARB_MODE0_BIT   (0)
#define PRIORITY_ARB_MODE1_BIT   (1)
#define PRIORITY_ARB_MODE2_BIT   (2)
#define PRIORITY_ARB_MODE3_BIT   (3)
#define PRIORITY_ARB_MODE4_BIT   (4)
#define PRIORITY_ARB_MODE5_BIT   (5)
#define PRIORITY_ARB_MODE6_BIT   (6)

#define PRIORITY_ARB_SELx_BITS   (2)
#define PRIORITY_ARB_SEL0_FROM   (7)
#define PRIORITY_ARB_SEL1_FROM   (9)
#define PRIORITY_ARB_SEL2_FROM   (11)
#define PRIORITY_ARB_SEL3_FROM   (13)
#define PRIORITY_ARB_SEL4_FROM   (15)
#define PRIORITY_ARB_SEL5_FROM   (17)
#define PRIORITY_ARB_SEL6_FROM   (19)







#define SUBSRCPND     (0x4A000018)
/*
0 = The interrupt has not been requested.
1 = The interrupt source has asserted the interrupt request.
*/

#define INTSUBMSK     (0x4A00001c)
/*
Determine which interrupt source is masked. The masked interrupt source will not be serviced.
0 = Interrupt service is available.
1 = Interrupt service is masked.
*/
#define INTSUBMSK_DEFAULT_VALUE    (0xFFFF)


#define INT_UART0_INT_RXD0_BIT    (0)
#define INT_UART0_INT_TXD0_BIT    (1)
#define INT_UART0_INT_ERR0_BIT    (2)
#define INT_UART1_INT_RXD1_BIT    (3)
#define INT_UART1_INT_TXD1_BIT    (4)
#define INT_UART1_INT_ERR1_BIT    (5)
#define INT_UART2_INT_RXD2_BIT    (6)
#define INT_UART2_INT_TXD2_BIT    (7)
#define INT_UART2_INT_ERR2_BIT    (8)
#define INT_ADC_INT_TC_BIT        (9)
#define INT_ADC_INT_ADC_S_BIT     (10)
#define INT_CAM_INT_CAM_C_BIT     (11)
#define INT_CAM_INT_CAM_P_BIT     (12)
#define INT_WDT_AC97_INT_WDT_BIT  (13)
#define INT_WDT_AC97_INT_AC97_BIT (14)





typedef enum _intr_no_e {
    EINT0_NO      = 0,
    EINT1_NO      = 1,
    EINT2_NO      = 2,
    EINT3_NO      = 3,
    EINT4_7_NO    = 4,
    EINT8_23_NO   = 5,
    INT_CAM_NO    = 6,
    nBATT_FLT_NO  = 7,
    INT_TICK_NO   = 8,
    WDT_AC97_NO   = 9,
    INT_TIMER0_NO = 10,
    INT_TIMER1_NO = 11,
    INT_TIMER2_NO = 12,
    INT_TIMER3_NO = 13,
    INT_TIMER4_NO = 14,
    INT_UART2_NO  = 15,
    INT_LCD_NO    = 16,
    INT_DMA0_NO   = 17,
    INT_DMA1_NO   = 18,
    INT_DMA2_NO   = 19,
    INT_DMA3_NO   = 20,
    INT_SDI_NO    = 21,
    INT_SPI0_NO   = 22,
    INT_UART1_NO  = 23,
    INT_NFCON_NO  = 24,
    INT_USBD_NO   = 25,
    INT_USBH_NO   = 26,
    INT_IIC_NO    = 27,
    INT_UART0_NO  = 28,
    INT_SPI1_NO   = 29,
    INT_RTC_NO    = 30,
    INT_ADC_NO    = 31,
    INT_MAX_NO    = 32
} intr_no_e;


typedef enum _subintr_no_e {
    UART0_SUBINT_RXD0_NO = 0,
    UART0_SUBINT_TXD0_NO = 1,
    UART0_SUBINT_ERR0_NO = 2,
    UART1_SUBINT_RXD1_NO = 3,
    UART1_SUBINT_TXD1_NO = 4,
    UART1_SUBINT_ERR1_NO = 5,
    UART2_SUBINT_RXD2_NO = 6,
    UART2_SUBINT_TXD2_NO = 7,
    UART2_SUBINT_ERR2_NO = 8,
    ADC_SUBINT_TC_NO     = 9,
    ADC_SUBINT_ADC_S_NO  = 10,
    CAM_SUBINT_CAM_C_NO  = 11,
    CAM_SUBINT_CAM_P_NO  = 12,
    WDT_AC97_SUBINT_WDT_NO  = 13,
    WDT_AC97_SUBINT_AC97_NO = 14,
    SUBINT_MAX_NO = 15
} subintr_no_e;


#define SUBINTR_NO_MASK   ((1<<15) - 1)

typedef enum _intr_mode_e {
    INTR_MODE_IRQ = 0,
    INTR_MODE_FIQ = 1
} intr_mode_e;


typedef etos_isr_ret_e (*pfunc_module_isr)(u32 current_tick, void *arg);

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

s32 interrupt_hw_mask_all_intr(void);

s32 interrupt_hw_mask_intr(intr_no_e intr_no);

s32 interrupt_hw_unmask_intr(intr_no_e intr_no);


s32 interrupt_hw_mask_all_subintr(void);

s32 interrupt_hw_mask_subintr(subintr_no_e subintr_no);

s32 interrupt_hw_unmask_subintr(subintr_no_e subintr_no);


s32 interrupt_hw_set_mode(intr_no_e intr_no, intr_mode_e mode);

s32 interrupt_hw_get_mode(intr_no_e intr_no, intr_mode_e *mode);



u32 interrupt_hw_get_requested_intr(void);

u32 interrupt_hw_get_requested_subintr(void);



s32 interrupt_hw_clear_intr(intr_no_e intr_no);

s32 interrupt_hw_clear_subintr(subintr_no_e subintr_no);





s32 board_interrupt_init(void);


s32 board_interrupt_register_intr_routine(intr_no_e intr_no, pfunc_module_isr module_isr, void *arg);


etos_isr_ret_e board_interrupt_isr_idic(u32 current_tick);


#endif  /* __INTERRUPT_HW_H__ */

/* EOF */

