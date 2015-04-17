/******************************************************************************
File    :  etos_gioi_interface.h

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		general IO interface for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-6-22      deeve        Create
2014-10-6      deeve        Redesign interface
2015-4-13      deeve        Add some comments


*******************************************************************************/
#ifndef __ETOS_GIOI_INTERFACE_H__
#define __ETOS_GIOI_INTERFACE_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define MAX_GIOI_DRVIER_NUM       (8)

/*port number define*/
#define PORT_0_UART_0             (0)
#define PORT_1_UART_1             (1)
#define PORT_2_USB_0              (2)
#define PORT_3_USB_1              (3)
#define PORT_4_SDIO_0             (4)
#define PORT_5_SDIO_1             (5)


typedef enum _gioi_rx_nfy {
    CLOSE_TO_OVERFLOW,
    RX_LENGTH_UPDATE,
    RX_NOTIFY_MAX
} gioi_rx_nfy_e;

typedef s32 (*pfunc_rx_notifier)(u32 port, gioi_rx_nfy_e type, u32 len);

typedef struct _gioi_driver {
    s32 (*pfunc_gioi_open)(u32 port);
    s32 (*pfunc_gioi_close)(u32 port);

    /*config & enable*/
    s32 (*pfunc_gioi_config)(u32 port, u32 cfg_mask, void *cfg_arg);
    s32 (*pfunc_gioi_startup)(u32 port);
    s32 (*pfunc_gioi_shutdown)(u32 port, u32 reason);

    /*output function*/
    s32 (*pfunc_gioi_tx_buf_size)(u32 port, u32 *size);
    s32 (*pfunc_gioi_tx_buf_free)(u32 port, u32 *free_size);

    s32 (*pfunc_gioi_put_char)(u32 port, u8 ch);
    u32 (*pfunc_gioi_put_bytes)(u32 port, BOOL wait_until_sent, u8 *buf, u32 len);

    /*input function*/
    s32 (*pfunc_gioi_rx_buf_size)(u32 port, u32 *size);

    s32 (*pfunc_gioi_rx_buf_length)(u32 port, u32 *len);
    s32 (*pfunc_gioi_reg_rx_notifier)(u32 port, pfunc_rx_notifier rx_nfy);

    s32 (*pfunc_gioi_get_char)(u32 port, u8 *ch);
    u32 (*pfunc_gioi_get_bytes)(u32 port, u8 *buf, u32 len);
} gioi_driver_t;

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

/*
             |
uper  layer  |                                         app layer
             |                                            |
----------------------------------------------------------V---------------------------
             |                   etos_gioi_open()/etos_gioi_register_rx_notifier()
             |                        etos_gioi_get_char()/etos_gioi_put_bytes()
general io   |                                            |
interface    |                                            |
             |                                            |
             |   etos_gioi_register_driver() ------------>|
---------------------------^------------------------------V---------------------------
             |             |                              |
lower layer  |        driver layer                    gio driver
             |
*/



/*
-------------------------------------------------------------------------------
*/



/* these 2 APIs below is for lower layer register (eg: driver layer) */


/**
 * register a general io driver.
 * register a general io driver which is specified by port,
 * only one driver can be registered for the corresponding port
 *
 * @param[in]    port               port number
 * @param[in]    pt_io_driver       the pointer to gioi driver for the corresponding port
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note  it is should be invoked before calling etos_gioi_open()
 *        and it is just for lower layer register driver
 * @see   etos_gioi_deregister_driver()
 * @authors    deeve
 * @date       2015/4/13
 */
s32  etos_gioi_register_driver(u32 port, gioi_driver_t *pt_io_driver);



/**
 * deregister a general io driver.
 * deregister a general io driver which is specified by port
 *
 * @param[in]    port         port number
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for lower layer deregister driver
 * @see  etos_gioi_register_driver()
 * @authors    deeve
 * @date       2015/4/13
 */
s32  etos_gioi_deregister_driver(u32 port);



/*
-------------------------------------------------------------------------------
*/



/* APIs below is for uper layer invoking */



/**
 * general io interface open.
 * open an interface for general io
 *
 * @param[in]    port         port number
 * @param[out]   handle       output handle for the open operation
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling, the output handle is used to invoke
 *       general io interface other api from uper layer
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_open(u32 port, u32 *handle);



/**
 * general io interface close.
 * close an interface for general io
 *
 * @param[in]    handle
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_close(u32 handle);



/**
 * general io interface get handle.
 * get the corresponding handle for the port
 *
 * @param[in]    port
 *
 * @return         handle
 * @retval other   success
 * @retval 0       fail
 *
 * @note it is just for uper layer calling, the return handle is used to invoke
 *       general io interface other api from uper layer
 * @authors    deeve
 * @date       2015/4/13
 */
u32 etos_gioi_get_handle(u32 port);



/**
 * general io interface register rx notifier.
 * register notifier for rx, rx_notifier will be invoked when data is incoming
 *
 * @param[in]    handle
 * @param[in]    rx_notifier   the function pointer of rx notifier
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_register_rx_notifier(u32 handle, pfunc_rx_notifier rx_notifier);



/**
 * general io interface config.
 * config an interface for general io
 *
 * @param[in]    handle
 * @param[in]    cfg_mask     config mask
 * @param[in]    cfg_arg      config argument, need match the corresponding driver
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_config(u32 handle, u32 cfg_mask, void *cfg_arg);



/**
 * general io interface startup.
 * startup an interface for general io
 *
 * @param[in]    handle
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @see        etos_gioi_shutdown()
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_startup(u32 handle);



/**
 * general io interface shutdown.
 * shutdown an interface for general io
 *
 * @param[in]    handle
 * @param[in]    reason   set to zeror now, need match the corresponding driver
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @see        etos_gioi_startup()
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_shutdown(u32 handle, u32 reason);



/**
 * general io interface get tx buffer size.
 * get the total tx buffer size
 *
 * @param[in]    handle
 * @param[out]   size         output buffer size
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_tx_buf_size(u32 handle, u32 *size);



/**
 * general io interface get tx buffer free size.
 * get the tx buffer free size, which is the free space that can be store data for tx
 *
 * @param[in]    handle
 * @param[out]   free_size         output tx buffer free size
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_tx_buf_free(u32 handle, u32 *free_size);



/**
 * general io interface put char.
 * send a single character over the general io interface
 *
 * @param[in]    handle
 * @param[in]    ch          the char which need to be sent
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_put_char(u32 handle, u8 ch);



/**
 * general io interface put bytes.
 * send some data bytes over the general io interface
 *
 * @param[in]    handle
 * @param[in]    wait_until_sent      it is sent or not when function return
 * @param[in]    buf                  data buffer
 * @param[in]    len                  the length of data buffer which need to be sent
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
u32 etos_gioi_put_bytes(u32 handle, BOOL wait_until_sent, u8 *buf, u32 len);



/**
 * general io interface get rx buffer size.
 * get the total rx buffer size, not received length
 *
 * @param[in]    handle
 * @param[out]   size         output buffer size
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_rx_buf_size(u32 handle, u32 *size);



/**
 * general io interface get rx buffer length.
 * get the rx buffer length, it is the received data length
 *
 * @param[in]    handle
 * @param[out]   len           output rx buffer length
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_rx_buf_length(u32 handle, u32 *len);



/**
 * general io interface get char.
 * receive a single character over the general io interface
 *
 * @param[in]    handle
 * @param[out]   ch           the received character
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
s32 etos_gioi_get_char(u32 handle, u8 *ch);



/**
 * general io interface get bytes.
 * receive some data bytes over the general io interface
 *
 * @param[in]       handle
 * @param[in/out]   buf        the received buffer
 * @param[in]       len        the length of received buffer
 *
 * @return    the length of buffer which is stored data in after function return
 * @retval other   success
 * @retval 0       fail
 *
 * @note it is just for uper layer calling
 * @authors    deeve
 * @date       2015/4/13
 */
u32 etos_gioi_get_bytes(u32 handle, u8 *buf, u32 len);


#endif  /* __ETOS_GIOI_INTERFACE_H__ */

/* EOF */

