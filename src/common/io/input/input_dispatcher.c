/******************************************************************************
File    :  input_dispatcher.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		input data dispatcher

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-8       deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

extern etos_msg_handle g_msg_handle_dispatcher;

extern void *task_test_main(void *arg);

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

u32 input_dispatcher_get_handle(void)
{
    u32 handle = etos_gioi_get_handle(PORT_0_UART_0);
    if (handle == 0){
        xloge(LOG_MODULE_DISPATCH, "handle==0\r\n");
    }
    return handle;
}


s32 input_dispatcher_entry(u32 port, gioi_rx_nfy_e type, u32 len)
{
    u32 recv_len;
    u8 *rx_buf;
    s32 ret;

    if ((port < MAX_GIOI_DRVIER_NUM) && (type < RX_NOTIFY_MAX)) {
        if (len > 0) {
            ret = etos_gioi_rx_buf_length(input_dispatcher_get_handle(), &recv_len);
            rx_buf = etos_msgq_get_buf(g_msg_handle_dispatcher, recv_len);
            if (rx_buf) {
                etos_gioi_get_bytes(input_dispatcher_get_handle(), rx_buf, recv_len);
                rx_buf[recv_len] = '\0';
                ret += etos_msgq_send_idic(g_msg_handle_dispatcher, rx_buf);
                if (ret) {
                    xlogt(LOG_MODULE_DISPATCH, "INTR: uart(r) send msg err:%d\r\n", ret);
                } else {
                    xlogt(LOG_MODULE_DISPATCH, "INTR: uart recv\r\n");
                }
            }
        }
    }

    return ETOS_RET_OK;
}

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 input_dispatcher_reg_rx_notifier(void)
{
    return etos_gioi_register_rx_notifier(input_dispatcher_get_handle(), input_dispatcher_entry);
}


void *input_task_dispatcher_main(void *arg)
{
    s32 ret;
    etos_task_handle test_task_handle;
    u32 sleep_s;
    u8 *pc_msg_buf;
    char task_name[ETOS_MAX_TASK_NAME_LEN];
    arg = arg;

    input_dispatcher_reg_rx_notifier();

    strcpy(task_name, "TEST");

    xlogt(LOG_MODULE_DRV, "task dispatcher: can be accept command now\r\n");

    while (1) {
        if (etos_msgq_recv(g_msg_handle_dispatcher, &pc_msg_buf, NULL) == ETOS_RET_OK) {
            xlogt(LOG_MODULE_DISPATCH, "task dispatcher RECV:%s\r\n", pc_msg_buf);

            if (strncmp("create:", (const char *)(pc_msg_buf), 7) == 0) {
                sleep_s = pc_msg_buf[7] - '0';
                task_name[4] = pc_msg_buf[7];
                task_name[5] = 0;
                ret = etos_task_create((const char *)task_name,
                                       sleep_s * 3,
                                       task_test_main,
                                       (void *)sleep_s,
                                       2048,
                                       &test_task_handle);
                if (ret) {
                    xloge(LOG_MODULE_DISPATCH, "create task err:%d\r\n", ret);
                }
            }
            etos_msgq_release_buf(g_msg_handle_dispatcher, pc_msg_buf);
        }
    }

    return (void *)0;
}


/* EOF */

