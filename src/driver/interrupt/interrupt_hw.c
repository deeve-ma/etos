/******************************************************************************
File    :  interrupt_hw.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		board interrupt control source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-3       deeve        Create

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

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 interrupt_hw_mask_all_intr(void)
{
    REG(INTMSK) = INTMSK_DEFAULT_VALUE;
    return ETOS_RET_OK;
}


s32 interrupt_hw_mask_intr(intr_no_e intr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        REG(INTMSK) |= (1 << intr_no);
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_unmask_intr(intr_no_e intr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        REG(INTMSK) &= (~(1 << intr_no));
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_mask_all_subintr(void)
{
    REG(INTSUBMSK) = INTSUBMSK_DEFAULT_VALUE;
    return ETOS_RET_OK;
}


s32 interrupt_hw_mask_subintr(subintr_no_e subintr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (subintr_no < SUBINT_MAX_NO) {
        REG(INTSUBMSK) |= (1 << subintr_no);
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_unmask_subintr(subintr_no_e subintr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (subintr_no < SUBINT_MAX_NO) {
        REG(INTSUBMSK) &= (~(1 << subintr_no));
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_set_mode(intr_no_e intr_no, intr_mode_e mode)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        if (mode) {
            REG_SET_BIT(INTMOD, 1 << intr_no);
        } else {
            REG_CLR_BIT(INTMOD, 1 << intr_no);
        }
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_get_mode(intr_no_e intr_no, intr_mode_e *mode)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        if (mode) {
            if (REG_GET_BIT(INTMOD, intr_no)) {
                *mode = INTR_MODE_FIQ;
            } else {
                *mode = INTR_MODE_IRQ;
            }
        }
        ret = ETOS_RET_OK;
    }

    return ret;
}



u32 interrupt_hw_get_requested_intr(void)
{
    /*only one bit can be set to 1*/
    u32 reg_val = REG_GET_VALUE(INTPND);
    return reg_val;
    //return (intr_no_e)etos_log_base_2(reg_val);
}



u32 interrupt_hw_get_requested_subintr(void)
{
    u32 reg_val = REG_GET_VALUE(SUBSRCPND) & SUBINTR_NO_MASK;
    return reg_val;
}



s32 interrupt_hw_clear_intr(intr_no_e intr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (intr_no < INT_MAX_NO) {
        /*clear SRCPND first, write 1 to clear, ONLY for IRQ, not FIQ*/
        if (REG_GET_BIT(INTMOD, intr_no) == INTR_MODE_IRQ) {
            REG(SRCPND) = (1 << intr_no);
        }
        /*write 1 to clear*/
        REG(INTPND) = (1 << intr_no);
        ret = ETOS_RET_OK;
    }

    return ret;
}


s32 interrupt_hw_clear_subintr(subintr_no_e subintr_no)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (subintr_no < SUBINT_MAX_NO) {
        /*write 1 to clear*/
        REG(SUBSRCPND) = (1 << subintr_no);
        ret = ETOS_RET_OK;
    }

    return ret;
}


/* EOF */

