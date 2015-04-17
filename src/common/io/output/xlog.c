/******************************************************************************
File    :  xlog.c

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		extended log for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-10-12     deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_gioi_interface.h"
#include "xlog.h"
#include "uart.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/
static u32 _xlog_output_handle;
static u8 _xlog_level_cfg[LOG_MODULE_MAX];

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 xlog_init(u32 output_port)
{
    u32 handle;
    s32 i;
    s32 ret = ETOS_RET_FAIL;
    uart_config_t uart_config = {
        BAUD_RATE_115200,
        WORD_LEN_8b,
        STOP_BIT_1b,
        PARITY_NONE,
        AFC_DISABLE
    };

    ret = etos_gioi_open(output_port, &handle);

    if (ret == ETOS_RET_OK) {
        _xlog_output_handle = handle;

        //TODO: config & register rx notifier
        etos_gioi_config(handle, 0xff, (void *)&uart_config);
    }

    for (i = 0; i < LOG_MODULE_MAX; i++) {
        xlog_level_set(i, LOG_LEVEL_MASK_DEFAULT);
    }

    return ret;
}

s32 xlog_destroy(void)
{
    s32 ret = ETOS_RET_FAIL;

    if (_xlog_output_handle) {
        ret = etos_gioi_close(_xlog_output_handle);
    }

    return ret;
}

u32 xlog_get_output_handle(void)
{
    return _xlog_output_handle;
}


s32 xlog_level_set(u32 module, u8 level)
{
    _xlog_level_cfg[module] = level;
    return ETOS_RET_OK;
}


s32 xlog_level_get(u32 module, u8 *level)
{
    s32 ret = ETOS_INVALID_PARAM;
    if (level && (module < LOG_MODULE_MAX)) {
        *level = _xlog_level_cfg[module];
        ret = ETOS_RET_OK;
    }
    return ret;
}

u8 *xlog_level_get_config(void)
{
    return _xlog_level_cfg;
}


/* EOF */

