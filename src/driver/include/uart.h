/******************************************************************************
File    :  uart.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		header file for uart

History:

Date           Author       Notes
----------     -------      -------------------------
2015-2-25      deeve        Create

*******************************************************************************/
#ifndef __UART_H__
#define __UART_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_includes.h"
#include "gpio.h"
/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define GPHCON_UART_PIN_MODE             (2)


/*UART line control registers*/
/*uart0*/
#define ULCON0           0x50000000
/*uart1 & uart2*/
#define ULCON1           0x50004000
#define ULCON2           0x50008000


#define ULCONx_WORD_LEN_FROM       (0)
#define ULCONx_WORD_LEN_BITS       (2)

#define WORD_LEN_5b  (0)
#define WORD_LEN_6b  (1)
#define WORD_LEN_7b  (2)
#define WORD_LEN_8b  (3)



#define ULCONx_STOP_BIT            (2)

#define STOP_BIT_1b   (0)
#define STOP_BIT_2b   (1)



#define ULCONx_PARITY_MODE_FROM       (3)
#define ULCONx_PARITY_MODE_BITS       (3)

#define PARITY_NONE  (0)
#define PARITY_ODD   (4)
#define PARITY_EVEN  (5)
#define PARITY_FORCE_1 (6)
#define PARITY_FORCE_0 (7)


#define ULCONx_IR_MODE                (6)

#define IR_MODE_ENALBE       (1)
#define IR_MODE_DISABLE      (0)




/*UART control registers*/
/*uart0*/
#define UCON0           0x50000004
/*uart1 & uart2*/
#define UCON1           0x50004004
#define UCON2           0x50008004


#define UCONx_RECV_MODE_FROM       (0)
#define UCONx_RECV_MODE_BITS       (2)
#define RECV_MODE_DISABLE     (0)
#define RECV_MODE_INTERRUPT   (1)
#define RECV_MODE_DMA0_3      (2)   /*for uart0 or uart2*/
#define RECV_MODE_DMA1        (1)   /*only for uart1*/


#define UCONx_TRANSMIT_MODE_FROM     (2)
#define UCONx_TRANSMIT_MODE_BITS     (2)
#define TRANSMIT_MODE_DISABLE     (0)
#define TRANSMIT_MODE_INTERRUPT   (1)
#define TRANSMIT_MODE_DMA0_3      (2)   /*for uart0 or uart2*/
#define TRANSMIT_MODE_DMA1        (1)   /*only for uart1*/


#define UCONx_SEND_BREAK_BIT      (4)

#define UCONx_LOOP_MODE_BIT      (5)

#define UCONx_INTERRUPT_AT_RX_ERR_BIT    (6)

#define UCONx_RX_TIMEOUT_ENABLE_BIT      (7)

#define UCONx_RX_INTERRUPT_TYPE_BIT      (8)
#define RX_PULSE_TRIGGER         (0)
#define RX_LELVEL_TRIGGER        (1)

#define UCONx_TX_INTERRUPT_TYPE_BIT      (9)
#define TX_PULSE_TRIGGER         (0)
#define TX_LELVEL_TRIGGER        (1)


#define UCONx_CLOCK_SEL_FROM     (10)
#define UCONx_CLOCK_SEL_BITS     (2)
#define CLK_PCLK               (0)   //2
#define CLK_UEXTCLK            (1)
#define CLK_FCLK_n             (3)


#define UCONx_CLOCK_FCLK_DIVIDER_FROM    (12)
#define UCONx_CLOCK_FCLK_DIVIDER_BITS    (4)



/*UART FIFO control registers*/
/*uart0*/
#define UFCON0           0x50000008
/*uart1 & uart2*/
#define UFCON1           0x50004008
#define UFCON2           0x50008008

#define UFCONx_FIFO_ENABLE_BIT        (0)
#define UFCONx_RX_FIFO_RESET_BIT      (1)
#define UFCONx_TX_FIFO_RESET_BIT      (2)

#define UFCONx_RX_FIFO_TRIGGER_LVL_FROM  (4)
#define UFCONx_RX_FIFO_TRIGGER_LVL_BITS  (2)
#define RX_TRIGGER_1B    (0)
#define RX_TRIGGER_8B    (1)
#define RX_TRIGGER_16B   (2)
#define RX_TRIGGER_32B   (3)

#define UFCONx_TX_FIFO_TRIGGER_LVL_FROM  (6)
#define UFCONx_TX_FIFO_TRIGGER_LVL_BITS  (2)
#define TX_TRIGGER_0B    (0)
#define TX_TRIGGER_16B   (1)
#define TX_TRIGGER_32B   (2)
#define TX_TRIGGER_48B   (3)



/*UART MODEM control registers*/
/*uart0*/
#define UMCON0           0x5000000c
/*uart1*/
#define UMCON1           0x5000400c


#define UFCONx_AUTO_FLOW_CONTROL_BIT      (4)
#define AFC_ENABLE    (1)
#define AFC_DISABLE   (0)

/*If AFC bit is disabled, nRTS must be controlled by software*/
#define UMCONx_REQ_TO_SEND_BIT            (0)
#define RTS_INACTIVE   (0)
#define RTS_ACTIVE     (1)
/*IEEE802.11提供了如下解决方案。在参数配置中，若使用RTS/CTS协议，同时设置传送上限字节数
一旦待传送的数据大于此上限值时，即启动RTS/CTS握手协议：
首先，A向B发送RTS信号，表明A要向B发送若干数据，B收到RTS后，向所有基站发出CTS信号，表明已准备就绪，
A可以发送，其余基站暂时"按兵不动"，然后，A向B发送数据，最后，B接收完数据后，即向所有基站广播ACK确认帧，
这样，所有基站又重新可以平等侦听、竞争信道了。
CTS 是对RTS 的应答
*/

/*UART Tx/Rx status registers*/
/*uart0*/
#define UTRSTAT0         0x50000010
/*uart1 & uart2*/
#define UTRSTAT1         0x50004010
#define UTRSTAT2         0x50008010

#define UTRSTATx_RECV_BUF_DATA_READY_BIT   (0)
#define UTRSTATx_TRANSMIT_BUF_EMPTY_BIT    (1)
#define UTRSTATx_TRANSMITTER_EMPTY_BIT     (2)




/*UART Rx error status registers*/
/*uart0*/
#define UERSTAT0         0x50000014
/*uart1 & uart2*/
#define UERSTAT1         0x50004014
#define UERSTAT2         0x50008014

#define UERSTATx_OVERRUN_ERR_BIT       (0)
#define UERSTATx_PARITY_ERR_BIT        (1)
#define UERSTATx_FRAME_ERR_BIT         (2)
#define UERSTATx_BREAK_DETECT_BIT      (3)


/*UART FIFO status registers*/
/*uart0*/
#define UFSTAT0         0x50000018
/*uart1 & uart2*/
#define UFSTAT1         0x50004018
#define UFSTAT2         0x50008018

#define UFSTATx_RX_FIFO_CNT_FROM      (0)
#define UFSTATx_RX_FIFO_CNT_BITS      (6)

#define UFSTATx_RX_FIFO_FULL_BIT      (6)

#define UFSTATx_TX_FIFO_CNT_FROM      (8)
#define UFSTATx_TX_FIFO_CNT_BITS      (6)

#define UFSTATx_TX_FIFO_FULL_BIT      (14)



#define UART_TX_FIFO_SIZE   (64)
#define UART_RX_FIFO_SIZE   (64)




/*UART MODEM status registers*/
/*uart0*/
#define UMSTAT0           0x5000001c
/*uart1*/
#define UMSTAT1           0x5000401c


#define UMSTATx_CLR_TO_SEND_BIT       (0)
#define CTS_ACTIVE         (1)
#define CTS_INACTIVE       (0)

#define UMSTATx_DELTA_CTS_BIT         (4)


/*UART transmit buffer registers*/
/*uart0*/
#define UTXH0_L           0x50000020   //little endian
#define UTXH0_B           0x50000023   //big endian

/*uart1 & uart2*/
#define UTXH1_L           0x50004020   //little endian
#define UTXH1_B           0x50004023   //big endian

#define UTXH2_L           0x50008020   //little endian
#define UTXH2_B           0x50008023   //big endian

#ifdef _BIG_ENDIAN_
#define UTXH0            UTXH0_B
#define UTXH1            UTXH1_B
#define UTXH2            UTXH2_B
#else
#define UTXH0            UTXH0_L
#define UTXH1            UTXH1_L
#define UTXH2            UTXH2_L
#endif


#define UTXHx_TXDATA_FROM       (0)
#define UTXHx_TXDATA_BITS       (8)



/*UART receive buffer registers*/
/*uart0*/
#define URXH0_L           0x50000024   //little endian
#define URXH0_B           0x50000027   //big endian

/*uart1 & uart2*/
#define URXH1_L           0x50004024   //little endian
#define URXH1_B           0x50004027   //big endian

#define URXH2_L           0x50008024   //little endian
#define URXH2_B           0x50008027   //big endian


#ifdef _BIG_ENDIAN_
#define URXH0            URXH0_B
#define URXH1            URXH1_B
#define URXH2            URXH2_B
#else
#define URXH0            URXH0_L
#define URXH1            URXH1_L
#define URXH2            URXH2_L
#endif



#define URXHx_RXDATA_FROM       (0)
#define URXHx_RXDATA_BITS       (8)


/*UART baud rate divisor registers*/
/*uart0*/
#define UBRDIV0         0x50000028
/*uart1 & uart2*/
#define UBRDIV1         0x50004028
#define UBRDIV2         0x50008028

#define UBRDIVx_UBRDIV_FROM    (0)
#define UBRDIVx_UBRDIV_BITS    (16)


/*
UBRDIVn  = (int)( UART clock / ( buad rate x 16) ) –1
( UART clock : PCLK, FCLK/n or UEXTCLK )

Where, UBRDIVn should be from 1 to (2^16-1),
but can be set zero only using the UEXTCLK which should be smaller than PCLK.
*/

/*here we use PCLK 101.25MHz*/
/* UBRDIV = (101250000/(115200*16))-1*/
#define S3C2440_UBRDIV_115200     (54)


typedef enum _uart_baud_rate_e {
    BAUD_RATE_56000,
    BAUD_RATE_115200,
    BAUD_RATE_961200
} uart_baud_rate_e;


typedef struct _uart_config_t {
    uart_baud_rate_e  baud_rate;
    u8  data_bits;
    u8  stop_bits;
    u8  parity_mode;   /*odd/even/none*/
    u8  flow_control_mode;
} uart_config_t;


#define UART_PORT_MAX       (3)

#define UART_TX_BUF_SIZE    (512)
#define UART_RX_BUF_SIZE    (512)


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/
s32 uart_hw_config_pinmux(u32 port);
s32 uart_hw_config_clock(u32 port, uart_baud_rate_e baud_rate);
s32 uart_hw_config_line_control(u32 port, u8 parity_mode, u8 stop_bits, u8 data_bits);
s32 uart_hw_config_mode(u32 port, BOOL dma_mode, BOOL enable_afc);
s32 uart_hw_send_char(u32 port, u8 ch);
u32 uart_hw_recved_cnt(u32 port);
s32 uart_hw_recv_char(u32 port, u8 *ch);


s32 uart_open(u32 port);
s32 uart_close(u32 port);

/*config & enable*/
s32 uart_config(u32 port, u32 cfg_mask, void *cfg_arg);
s32 uart_startup(u32 port);
s32 uart_shutdown(u32 port, u32 reason);

/*output function*/
s32 uart_tx_buf_size(u32 port, u32 *size);
s32 uart_tx_buf_free(u32 port, u32 *free_size);

s32 uart_put_char(u32 port, u8 ch);
u32 uart_put_bytes(u32 port, BOOL wait_until_sent, u8 *buf, u32 len);

/*input function*/
s32 uart_rx_buf_size(u32 port, u32 *size);

s32 uart_rx_buf_length(u32 port, u32 *len);
s32 uart_reg_rx_notifier(u32 port, pfunc_rx_notifier rx_nfy);

s32 uart_get_char(u32 port, u8 *ch);
u32 uart_get_bytes(u32 port, u8 *buf, u32 len);

etos_isr_ret_e module_uart_isr_idic(u32 current_tick, void *arg);

s32 uart_driver_init(u32 port);


#endif  /* __UART_H__ */

/* EOF */

