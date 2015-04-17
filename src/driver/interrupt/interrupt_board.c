/******************************************************************************
File    :  interrupt_board.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		borad interrupt source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-5       deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "interrupt_hw.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

static pfunc_module_isr  _pfunc_module_isrs[INTR_MAX_NUM];

static void *_pfunc_module_isrs_arg[INTR_MAX_NUM];

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
s32 board_interrupt_init(void)
{
    s32 ret;

    /*mask all interrupt*/
    ret = interrupt_hw_mask_all_intr();
    ret += interrupt_hw_mask_all_subintr();

    /*register user interrupt dispatcher*/
    ret += etos_register_interrupt_dispatcher(board_interrupt_isr_idic);

    memset(_pfunc_module_isrs, 0, sizeof(_pfunc_module_isrs));
    memset(_pfunc_module_isrs_arg, 0, sizeof(_pfunc_module_isrs_arg));

    return ret;
}


s32 board_interrupt_register_intr_routine(intr_no_e intr_no, pfunc_module_isr module_isr, void *arg)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        if (module_isr) {
            _pfunc_module_isrs[intr_no] = module_isr;
            _pfunc_module_isrs_arg[intr_no] = arg;
            ret = ETOS_RET_OK;
        }
    }

    return ret;
}


etos_isr_ret_e board_interrupt_isr_idic(u32 current_tick)
{
    u32 reg_val;
    intr_no_e intr_no = INT_MAX_NO;
    etos_isr_ret_e ret = ETOS_ISR_RESCHEDULE_DISABLE;

    /*check which interrupt is requested*/
    reg_val = interrupt_hw_get_requested_intr();

    //xlogt(LOG_MODULE_DRV, "req intr=0x%x\r\n", reg_val);

    /*only for IRQ, NOT for FIQ*/
    if (reg_val == 0) {
        return ret;
    }

    if (reg_val & (reg_val - 1)) { /*not only 1 bit is set to 1*/
        xlogf_d(LOG_MODULE_DRV, "intterrupt src: 0x%x\r\n", reg_val);
    }

    /*only 1 bit is set to 1*/
    intr_no = etos_log_base_2(reg_val);
    if (_pfunc_module_isrs[intr_no]) {
        ret = _pfunc_module_isrs[intr_no](current_tick, _pfunc_module_isrs_arg[intr_no]);
    }

    /*clear interrupt, subinterrupt need clear in module isr*/
    interrupt_hw_clear_intr(intr_no);

    return ret;
}




/* EOF */

