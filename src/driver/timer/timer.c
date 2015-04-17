/******************************************************************************
File    :  timer.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		timer and pwm control source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-14      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "drivers.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/
/*PCLK = 405/4=101.25MHz*/
#define PRESCALER0_TIMER01          (250)
#define PRESCALER1_TIMER234         (250)


#define TCFG1_MUX4_VALUE            (3)   /*1/16*/
/* ~25KHz*/

#if 0
#define TCNTB4_VALUE                (50000)
/* ~2s */
#else
#define TCNTB4_VALUE                (400)
/* ~16ms */
#endif

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

extern u32 *g_os_boot_sp;

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/
static u32 _timer_intr_cnt;

static u32 _timer_intr_flag;

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/
etos_isr_ret_e module_timer_isr_idic(u32 current_tick, void *arg)
{
    etos_isr_ret_e ret = ETOS_ISR_RESCHEDULE_DISABLE;
    u32 timer_no = (u32)arg;
    u32 *sp_addr = g_os_boot_sp;

    current_tick = current_tick;

    _timer_intr_cnt++;

    switch (timer_no) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
#if 0
            if ((_timer_intr_cnt % 256) == 0) {
                xlogt(LOG_MODULE_DRV, "INTR: timer tick: (256 tick ~= 4s) 0x%x curr=%d\r\n", os_get_cpu_intr_enabled(),
                      etos_sched_get_tick());
            }
#else
            if ((_timer_intr_cnt % 10) == 0) {
                //xlogt(LOG_MODULE_DRV, "INTR: timer tick: intr:0x%x curr=%d\r\n", os_get_cpu_intr_enabled(), etos_sched_get_tick());
                _timer_intr_flag ^= 1;
                if (_timer_intr_flag) {
                    set_led_on(2);
                } else {
                    set_led_off(2);
                }
            }
#endif
            ret = ETOS_ISR_RESCHEDULE_UPDATE_TICK | ETOS_ISR_RESCHEDULE_ENABLE;
            break;
        default:
            break;
    }

    return ret;
}

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 timer_hw_config_pinmux(u32 timer_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    switch (timer_no) {
        case 0:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 1:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 2:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 3:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 4:
            ret = ETOS_RET_OK;
            break;
        default:
            break;
    }

    return ret;
}


s32 timer_hw_config_timer(u32 timer_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    switch (timer_no) {
        case 0:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 1:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 2:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 3:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 4:
            REG_SET_FIELD(TCFG0, TCFG0_PRESCALER1_FROM, TCFG0_PRESCALER1_BITS, PRESCALER1_TIMER234);
            REG_SET_FIELD(TCFG1, TCFG1_MUX4_FROM, TCFG1_MUX4_BITS, TCFG1_MUX4_VALUE);
            REG_SET_FIELD(TCFG1, TCFG1_DMA_MODE_FROM, TCFG1_DMA_MODE_BITS, 0);
            REG_SET_VALUE(TCNTB4, TCNTB4_VALUE);
            ret = ETOS_RET_OK;
            break;
        default:
            break;
    }

    return ret;
}



s32 timer_hw_start_timer(u32 timer_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    switch (timer_no) {
        case 0:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 1:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 2:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 3:
            ret = ETOS_NOT_SUPPORT;
            break;
        case 4:
            REG_SET_BIT(TCON, TCON_TIMERx_MANUAL_UPDATE_BIT(4));
            REG_CLR_BIT(TCON, TCON_TIMERx_OUTPUT_INVERTER_BIT(4));

            ret = board_interrupt_register_intr_routine(INT_TIMER4_NO, module_timer_isr_idic, (void *)timer_no);
            ret += interrupt_hw_clear_intr(INT_TIMER4_NO);
            ret += interrupt_hw_unmask_intr(INT_TIMER4_NO);

            REG_SET_BIT(TCON, TCON_TIMERx_START_BIT(4));
            REG_CLR_BIT(TCON, TCON_TIMERx_MANUAL_UPDATE_BIT(4));
            REG_SET_BIT(TCON, TCON_TIMER4_AUTO_RELOAD_BIT);

            break;
        default:
            break;
    }

    return ret;
}


s32 timer_hw_stop_timer(u32 timer_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    switch (timer_no) {
        case 0:
            REG_CLR_BIT(TCON, TCON_TIMER0_START_BIT);
            ret = ETOS_RET_OK;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            REG_CLR_BIT(TCON, TCON_TIMERx_START_BIT(timer_no));
            ret = ETOS_RET_OK;
            break;
        default:
            break;
    }

    return ret;
}




/* EOF */

