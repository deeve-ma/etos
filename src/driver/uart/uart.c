/******************************************************************************
File    :  uart.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		uart driver source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "drivers.h"
/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/*only support uart0 (one port) now*/

static BOOL _uart_hw_configured0 = FALSE;

static u8 _uart_tx_buffer0[UART_TX_BUF_SIZE];
static u8 _uart_rx_buffer0[UART_RX_BUF_SIZE];

static u32 _uart_tx_buf_write_id;
static u32 _uart_tx_buf_free_len;
static u32 _uart_tx_buf_read_id;

static u32 _uart_rx_buf_write_id;
static u32 _uart_rx_buf_received_len;
static u32 _uart_rx_buf_read_id;


pfunc_rx_notifier uart_rx_nofier[UART_PORT_MAX];

#if (UART_DIRECT_TO_SEND == 0)
static u32 _uart_tx_err_cnt;
#endif
static u32 _uart_rx_err_cnt;

static u32 _uart_need_send_min_size;
static BOOL _uart_need_send;


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 uart_open(u32 port)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        _uart_hw_configured0 = FALSE;

        memset(_uart_tx_buffer0, 0, sizeof(_uart_tx_buffer0));
        _uart_tx_buf_write_id = 0;
        _uart_tx_buf_free_len = UART_TX_BUF_SIZE;
        _uart_tx_buf_read_id = 0;

        memset(_uart_rx_buffer0, 0, sizeof(_uart_rx_buffer0));
        _uart_rx_buf_write_id = 0;
        _uart_rx_buf_read_id = 0;
        _uart_rx_buf_received_len = 0;

        _uart_need_send_min_size = 64;
        _uart_need_send = FALSE;

        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_close(u32 port)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        _uart_hw_configured0 = FALSE;
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

/*config & enable*/
s32 uart_config(u32 port, u32 cfg_mask, void *cfg_arg)
{
    uart_config_t *pt_uart_cfg;
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (cfg_arg) {
            ret = ETOS_RET_OK;
            pt_uart_cfg = (uart_config_t *)cfg_arg;
            ret += uart_hw_config_pinmux(port);
            ret += uart_hw_config_clock(port, pt_uart_cfg->baud_rate);
            ret += uart_hw_config_line_control(port, pt_uart_cfg->parity_mode,
                                               pt_uart_cfg->stop_bits, pt_uart_cfg->data_bits);
            ret += uart_hw_config_mode(port, FALSE, FALSE);

            if (ret == ETOS_RET_OK) {
                _uart_hw_configured0 = TRUE;
            }
        }

    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_startup(u32 port)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        ret = board_interrupt_register_intr_routine(INT_UART0_NO, module_uart_isr_idic, (void *)PORT_0_UART_0);

        ret += interrupt_hw_clear_subintr(UART0_SUBINT_TXD0_NO);
        ret += interrupt_hw_clear_subintr(UART0_SUBINT_RXD0_NO);
        ret += interrupt_hw_clear_subintr(UART0_SUBINT_ERR0_NO);
        ret += interrupt_hw_clear_intr(INT_UART0_NO);

        ret += interrupt_hw_unmask_intr(INT_UART0_NO);
#if (UART_DIRECT_TO_SEND == 0)
        ret += interrupt_hw_unmask_subintr(UART0_SUBINT_TXD0_NO);
#endif
        ret += interrupt_hw_unmask_subintr(UART0_SUBINT_RXD0_NO);
        ret += interrupt_hw_unmask_subintr(UART0_SUBINT_ERR0_NO);
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_shutdown(u32 port, u32 reason)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        ret = interrupt_hw_mask_intr(INT_UART0_NO);
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

/*output function*/
s32 uart_tx_buf_size(u32 port, u32 *size)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (size) {
            *size = UART_TX_BUF_SIZE;
        }
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_tx_buf_free(u32 port, u32 *free_size)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (free_size) {
            *free_size = _uart_tx_buf_free_len;
        }
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

#if (UART_DIRECT_TO_SEND)
s32 uart_put_char(u32 port, u8 ch)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        ret = uart_hw_send_char(port, ch);
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}


u32 uart_put_bytes(u32 port, BOOL wait_until_sent, u8 *buf, u32 len)
{
    u32 send_len = 0;
    s32 ret;

    if (port == 0) {
        if (buf && len) {
            send_len = 0;
            ret = 0;
            while (len) {
                ret += uart_put_char(port, *buf++);
                len--;
                send_len++;
            }
        }
    } else if (port == 1) {
        send_len = 0;
    } else if (port == 2) {
        send_len = 0;
    }

    return send_len;
}

#else

s32 uart_put_char(u32 port, u8 ch)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        ret = uart_hw_send_char(port, ch);
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}


u32 uart_put_bytes(u32 port, BOOL wait_until_sent, u8 *buf, u32 len)
{
    BOOL configured;
    u32 free_len, copy_len;
    u32 ret_send_len = 0;
    etos_init_critical();

    if (port == 0) {
        if (buf && len) {
            configured = _uart_hw_configured0;
            if (wait_until_sent && configured) {
                ret_send_len = 0;
                etos_enter_critical();
                while (len) {
                    uart_put_char(port, *buf++);
                    len--;
                    ret_send_len++;
                }
                etos_exit_critical();
            } else { /*can be buffered*/
                free_len = _uart_tx_buf_free_len;
                if (len <= free_len) {
                    ret_send_len = len;

                    if (configured) {
                        etos_enter_critical();
                    }

                    copy_len = UART_TX_BUF_SIZE - _uart_tx_buf_write_id;
                    if (copy_len >= len) {
                        memcpy(&_uart_tx_buffer0[_uart_tx_buf_write_id], buf, len);
                        _uart_tx_buf_write_id += len;
                    } else {
                        if (copy_len) {
                            memcpy(&_uart_tx_buffer0[_uart_tx_buf_write_id], buf, copy_len);
                        }
                        buf += copy_len;
                        copy_len = len - copy_len;
                        memcpy(&_uart_tx_buffer0[0], buf, copy_len);
                        _uart_tx_buf_write_id = copy_len;
                    }
                    _uart_tx_buf_free_len -= len;

                    if ((UART_TX_BUF_SIZE - _uart_tx_buf_free_len) >= _uart_need_send_min_size) {
                        _uart_need_send = TRUE;
                    }

                    /*send 1 byte to trigger interrupt*/
                    if (configured) {
                        if (_uart_need_send) {
                            _uart_tx_buf_free_len++;
                            _uart_tx_buf_read_id = _uart_tx_buf_read_id % UART_TX_BUF_SIZE;
                            uart_put_char(port, _uart_tx_buffer0[_uart_tx_buf_read_id]);
                            _uart_tx_buf_read_id++;
                        }

                        etos_exit_critical();
                    }
                } else {
                    ASSERT(len <= free_len);
                }
            }
        }
    } else if (port == 1) {
        ret_send_len = 0;
    } else if (port == 2) {
        ret_send_len = 0;
    }

    return ret_send_len;
}

#endif


/*input function*/
s32 uart_rx_buf_size(u32 port, u32 *size)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (size) {
            *size = UART_RX_BUF_SIZE;
        }
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_rx_buf_length(u32 port, u32 *len)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (len) {
            *len = _uart_rx_buf_received_len;
        }
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_reg_rx_notifier(u32 port, pfunc_rx_notifier rx_nfy)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (rx_nfy) {
            uart_rx_nofier[0] = rx_nfy;
            ret = ETOS_RET_OK;
        }
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}

s32 uart_get_char(u32 port, u8 *ch)
{
    s32 ret = ETOS_INVALID_PARAM;
    etos_init_critical();

    if (port == 0) {
        if (ch) {
            etos_enter_critical();
            _uart_rx_buf_read_id = _uart_rx_buf_read_id % UART_RX_BUF_SIZE;
            if (_uart_rx_buf_received_len) {
                _uart_rx_buf_received_len--;
                *ch = _uart_rx_buffer0[_uart_rx_buf_read_id];
                _uart_rx_buf_read_id++;
            }
            etos_exit_critical();
            ret = ETOS_RET_OK;
        }
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}


u32 uart_get_bytes(u32 port, u8 *buf, u32 len)
{
    u32 recved_len, copy_len;
    u32 ret_send_len = 0;
    etos_init_critical();

    if (port == 0) {
        recved_len = _uart_rx_buf_received_len;
        if (len <= recved_len) {
            ret_send_len = len;

            etos_enter_critical();

            copy_len = UART_RX_BUF_SIZE - _uart_rx_buf_read_id;
            if (copy_len >= len) {
                memcpy(buf, &_uart_rx_buffer0[_uart_rx_buf_read_id], len);
                _uart_rx_buf_read_id += len;
            } else {
                memcpy(buf, &_uart_rx_buffer0[_uart_rx_buf_read_id], copy_len);
                buf += copy_len;
                copy_len = len - copy_len;
                memcpy(buf, &_uart_rx_buffer0[0], copy_len);
                _uart_rx_buf_read_id = copy_len;
            }

            _uart_rx_buf_received_len -= len;

            etos_exit_critical();
        } else {
            ASSERT(len <= recved_len);
        }
    } else if (port == 1) {
        ret_send_len = 0;
    } else if (port == 2) {
        ret_send_len = 0;
    }

    return ret_send_len;
}


etos_isr_ret_e module_uart_isr_idic(u32 current_tick, void *arg)
{

    u32 reg_val, data_len;
    s32 all_ret_val = ETOS_RET_OK;
    u32 port = (u32)arg;
    gioi_rx_nfy_e rx_nfy_type = RX_NOTIFY_MAX;
    BOOL real_recved = FALSE;

    current_tick = current_tick;


    if (port == 0) {
        reg_val = interrupt_hw_get_requested_subintr();

#if (UART_DIRECT_TO_SEND == 0)
        /*process tx interrupt*/
        if (reg_val & (1 << INT_UART0_INT_TXD0_BIT)) {
            data_len = UART_TX_BUF_SIZE - _uart_tx_buf_free_len;

            if (data_len != 0) {
                set_led_on(4);
                /*some data need to send*/
                while (data_len) {
                    _uart_tx_buf_read_id = _uart_tx_buf_read_id % UART_TX_BUF_SIZE;
                    _uart_tx_buf_free_len++;
                    all_ret_val += uart_put_char(port, _uart_tx_buffer0[_uart_tx_buf_read_id]);
                    _uart_tx_buf_read_id++;
                    data_len--;
                }

                _uart_need_send = FALSE;
            }

            all_ret_val = interrupt_hw_clear_subintr(UART0_SUBINT_TXD0_NO);

            if (all_ret_val) {
                _uart_tx_err_cnt++;
            }
        }
#endif

        /*process rx interrupt*/
        if (reg_val & (1 << INT_UART0_INT_RXD0_BIT)) {
            /*save to rx buffer*/
            data_len = uart_hw_recved_cnt(PORT_0_UART_0);

            if ((UART_RX_BUF_SIZE - _uart_rx_buf_received_len) < data_len) {
                data_len = UART_RX_BUF_SIZE - _uart_rx_buf_received_len;
            }

            do {
                while (data_len) {
                    _uart_rx_buf_write_id = _uart_rx_buf_write_id % UART_RX_BUF_SIZE;
                    _uart_rx_buf_received_len++;
                    all_ret_val += uart_hw_recv_char(port, &_uart_rx_buffer0[_uart_rx_buf_write_id]);
                    _uart_rx_buf_write_id++;
                    data_len--;
                    real_recved = TRUE;
                }

                data_len = uart_hw_recved_cnt(PORT_0_UART_0);

                if ((UART_RX_BUF_SIZE - _uart_rx_buf_received_len) < data_len) {
                    data_len = UART_RX_BUF_SIZE - _uart_rx_buf_received_len;
                }
            } while (data_len && (_uart_rx_buf_received_len != UART_RX_BUF_SIZE));

            if (real_recved) {
                rx_nfy_type = RX_LENGTH_UPDATE;
                if ((_uart_rx_buf_received_len << 2) > (UART_RX_BUF_SIZE * 3)) {
                    rx_nfy_type = CLOSE_TO_OVERFLOW;   /* _uart_rx_buf_received_len > (3/4 x UART_RX_BUF_SIZE) */
                }

                /*notify up layer*/
                all_ret_val += uart_rx_nofier[0](PORT_0_UART_0, rx_nfy_type, _uart_rx_buf_received_len);
            } else {
                /*rx timeout interrupt*/
                if (_uart_rx_buf_received_len > 0) {
                    /*notify up layer*/
                    all_ret_val += uart_rx_nofier[0](PORT_0_UART_0, RX_LENGTH_UPDATE, _uart_rx_buf_received_len);
                }
                xlogt(LOG_MODULE_DRV, "RXTM\r\n");
            }

            all_ret_val += interrupt_hw_clear_subintr(UART0_SUBINT_RXD0_NO);

            if (all_ret_val) {
                _uart_rx_err_cnt++;
            }
        }

        if (reg_val & (1 << INT_UART0_INT_ERR0_BIT)) {
            all_ret_val = interrupt_hw_clear_subintr(UART0_SUBINT_ERR0_NO);
            _uart_rx_err_cnt++;
        }
    } else if (port == 1) {

    } else if (port == 2) {

    }

    return ETOS_ISR_RESCHEDULE_ENABLE;
}



s32 uart_driver_init(u32 port)
{
#if (__STDC_VERSION__ >= 199901L)  /*c99*/
    gioi_driver_t uart_drv = {
        .pfunc_gioi_open            = uart_open,
        .pfunc_gioi_close           = uart_close,
        .pfunc_gioi_config          = uart_config,
        .pfunc_gioi_startup         = uart_startup,
        .pfunc_gioi_shutdown        = uart_shutdown,
        .pfunc_gioi_tx_buf_size     = uart_tx_buf_size,
        .pfunc_gioi_tx_buf_free     = uart_tx_buf_free,
        .pfunc_gioi_put_char        = uart_put_char,
        .pfunc_gioi_put_bytes       = uart_put_bytes,
        .pfunc_gioi_rx_buf_size     = uart_rx_buf_size,
        .pfunc_gioi_rx_buf_length   = uart_rx_buf_length,
        .pfunc_gioi_reg_rx_notifier = uart_reg_rx_notifier,
        .pfunc_gioi_get_char        = uart_get_char,
        .pfunc_gioi_get_bytes       = uart_get_bytes
    };
#else
    gioi_driver_t uart_drv = {
        uart_open,
        uart_close,
        uart_config,
        uart_startup,
        uart_shutdown,
        uart_tx_buf_size,
        uart_tx_buf_free,
        uart_put_char,
        uart_put_bytes,
        uart_rx_buf_size,
        uart_rx_buf_length,
        uart_reg_rx_notifier,
        uart_get_char,
        uart_get_bytes
    };
#endif

    return etos_gioi_register_driver(port, &uart_drv);
}
/* EOF */

