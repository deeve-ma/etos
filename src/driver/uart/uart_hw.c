/******************************************************************************
File    :  uart_hw.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		hardware operation for uart

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25      deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
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

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

s32 uart_hw_config_pinmux(u32 port)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        /*config TXD[0]  ->  GPH2*/
        /*disable pull up*/
        REG_SET_BIT(GPHUP, 2);
        /*set port to txd*/
        REG_SET_FIELD(GPHCON, GPxCON_PORTn_FROM(2), GPxCON_PORTn_BITS, GPHCON_UART_PIN_MODE);

        /*config RXD[0]  ->  GPH3*/
        /*disable pull up*/
        REG_SET_BIT(GPHUP, 3);
        /*set port to rxd*/
        REG_SET_FIELD(GPHCON, GPxCON_PORTn_FROM(3), GPxCON_PORTn_BITS, GPHCON_UART_PIN_MODE);

        /*config nRTS0  ->  GPH1*/
        /*disable pull up*/
        REG_SET_BIT(GPHUP, 1);
        /*set port to rts0*/
        REG_SET_FIELD(GPHCON, GPxCON_PORTn_FROM(1), GPxCON_PORTn_BITS, GPHCON_UART_PIN_MODE);


        /*config nCTS0  ->  GPH0*/
        /*disable pull up*/
        REG_SET_BIT(GPHUP, 0);
        /*set port to cts0*/
        REG_SET_FIELD(GPHCON, GPxCON_PORTn_FROM(0), GPxCON_PORTn_BITS, GPHCON_UART_PIN_MODE);

        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}



s32 uart_hw_config_clock(u32 port, uart_baud_rate_e baud_rate)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (baud_rate == BAUD_RATE_115200) {
            REG_SET_FIELD(UCON0, UCONx_CLOCK_SEL_FROM, UCONx_CLOCK_SEL_BITS, CLK_PCLK);
            REG_SET_VALUE(UBRDIV0, S3C2440_UBRDIV_115200);
            ret = ETOS_RET_OK;
        } else {
            ret = ETOS_NOT_SUPPORT;
        }
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}



s32 uart_hw_config_line_control(u32 port, u8 parity_mode, u8 stop_bits, u8 data_bits)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        REG_SET_FIELD(ULCON0, ULCONx_PARITY_MODE_FROM, ULCONx_PARITY_MODE_BITS, parity_mode);
        REG_SET_FIELD(ULCON0, ULCONx_WORD_LEN_FROM, ULCONx_WORD_LEN_BITS, data_bits);
        if (stop_bits) {
            REG_SET_BIT(ULCON0, ULCONx_STOP_BIT);
        } else {
            REG_CLR_BIT(ULCON0, ULCONx_STOP_BIT);
        }
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}



s32 uart_hw_config_mode(u32 port, BOOL dma_mode, BOOL enable_afc)
{
    u32 fifio_reg_val;
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (dma_mode || enable_afc) {
            ret = ETOS_NOT_SUPPORT;
        } else {
            REG_SET_VALUE(UMCON0, 0);

            fifio_reg_val = ((1 << UFCONx_FIFO_ENABLE_BIT)
                             | (RX_TRIGGER_8B << UFCONx_RX_FIFO_TRIGGER_LVL_FROM)
                             | (TX_TRIGGER_16B << UFCONx_TX_FIFO_TRIGGER_LVL_FROM));
            REG_SET_VALUE(UFCON0, fifio_reg_val);

            REG_SET_BIT(UCON0, UCONx_RX_TIMEOUT_ENABLE_BIT);  /*it is only available at rx dma mode*/

            REG_SET_BIT(UCON0, UCONx_TX_INTERRUPT_TYPE_BIT);
            REG_CLR_BIT(UCON0, UCONx_RX_INTERRUPT_TYPE_BIT);   /*PULSE trigger*/

            REG_SET_FIELD(UCON0, UCONx_TRANSMIT_MODE_FROM, UCONx_TRANSMIT_MODE_BITS, TRANSMIT_MODE_INTERRUPT);
            REG_SET_FIELD(UCON0, UCONx_RECV_MODE_FROM, UCONx_RECV_MODE_BITS, RECV_MODE_INTERRUPT);

            ret = ETOS_RET_OK;
        }
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}



s32 uart_hw_send_char(u32 port, u8 ch)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        /*wait fifo not full*/
        REG_WAIT_ZERO(UFSTAT0, UFSTATx_TX_FIFO_FULL_BIT);
        REG_SET_VALUE(UTXH0, ch);
        ret = ETOS_RET_OK;
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}




u32 uart_hw_recved_cnt(u32 port)
{
    u32 reg_addr = UFSTAT0;

    if (port == 0) {
        reg_addr = UFSTAT0;
    } else if (port == 1) {
        reg_addr = UFSTAT1;
    } else if (port == 2) {
        reg_addr = UFSTAT2;
    }

    return REG_GET_FIELD(reg_addr, UFSTATx_RX_FIFO_CNT_FROM, UFSTATx_RX_FIFO_CNT_BITS);
}



s32 uart_hw_recv_char(u32 port, u8 *ch)
{
    s32 ret = ETOS_INVALID_PARAM;

    if (port == 0) {
        if (ch) {
            if (REG_GET_FIELD(UFSTAT0, UFSTATx_RX_FIFO_CNT_FROM, UFSTATx_RX_FIFO_CNT_BITS)) {
                *ch = (u8)(REG_GET_VALUE(URXH0) & 0xff);
                ret = ETOS_RET_OK;
            } else {
                ret = ETOS_RET_FAIL;
            }
        }
    } else if (port == 1) {
        ret = ETOS_NOT_SUPPORT;
    } else if (port == 2) {
        ret = ETOS_NOT_SUPPORT;
    }

    return ret;
}


/* EOF */

