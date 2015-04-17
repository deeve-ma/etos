/******************************************************************************
File    :  etos_gioi_interface.c

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

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

static gioi_driver_t _os_gioi_drivers[MAX_GIOI_DRVIER_NUM];

static pfunc_rx_notifier _os_gioi_rx_notifiers[MAX_GIOI_DRVIER_NUM];

static BOOL _os_gioi_opened[MAX_GIOI_DRVIER_NUM];

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/
static s32 _etos_gioi_drop_rx_data(u32 port, u32 len)
{
    u8 buf[16];
    u32 each_len;
    gioi_driver_t *pt_gioi_driver = &_os_gioi_drivers[port];

    while (len > 0) {
        if (len > 16) {
            each_len = 16;
        } else {
            each_len = len;
        }

        len -= each_len;

        if (pt_gioi_driver && pt_gioi_driver->pfunc_gioi_get_bytes) {
            pt_gioi_driver->pfunc_gioi_get_bytes(port, buf, each_len);
        }
    }

    return ETOS_NOT_SUPPORT;
}

/*default rx notifier for general IO interface */
static s32 _etos_gioi_rx_notifier_wrapper(u32 port, gioi_rx_nfy_e type, u32 len)
{
    gioi_driver_t *pt_gioi_driver;
    u32 recv_len = len;
    if ((port < MAX_GIOI_DRVIER_NUM) && (type < RX_NOTIFY_MAX)) {
        if (_os_gioi_rx_notifiers[port]) {
            return _os_gioi_rx_notifiers[port](port, type, len);
        } else {
            /*just skip received data*/
            pt_gioi_driver = &_os_gioi_drivers[port];
            if (CLOSE_TO_OVERFLOW == type) {
                pt_gioi_driver->pfunc_gioi_rx_buf_length(port, &recv_len);
            }

            return _etos_gioi_drop_rx_data(port, recv_len);
        }
    }

    return ETOS_INVALID_PARAM;
}

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

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
s32 etos_gioi_register_driver(u32 port, gioi_driver_t *pt_io_driver)
{
    gioi_driver_t *pt_gioi_driver;

    if ((port >= MAX_GIOI_DRVIER_NUM) || (pt_io_driver == NULL)) {
        return ETOS_INVALID_PARAM;
    } else {
        pt_gioi_driver = &_os_gioi_drivers[port];
        memcpy(pt_gioi_driver, pt_io_driver, sizeof(gioi_driver_t));
    }
    return ETOS_RET_OK;
}



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
s32 etos_gioi_deregister_driver(u32 port)
{
    gioi_driver_t *pt_gioi_driver;

    if (port >= MAX_GIOI_DRVIER_NUM) {
        return ETOS_INVALID_PARAM;
    } else {
        if (_os_gioi_opened[port]) {
            return ETOS_NOT_SUPPORT;
        }
        pt_gioi_driver = &_os_gioi_drivers[port];
        memset(pt_gioi_driver, 0, sizeof(gioi_driver_t));
    }
    return ETOS_RET_OK;
}



/******************************************************************************/

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
s32 etos_gioi_open(u32 port, u32 *handle)
{
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver;

    if ((port >= MAX_GIOI_DRVIER_NUM) || (handle == NULL)) {
        return ETOS_INVALID_PARAM;
    } else {
        pt_gioi_driver = &_os_gioi_drivers[port];
        if (pt_gioi_driver && pt_gioi_driver->pfunc_gioi_open) {
            ret = pt_gioi_driver->pfunc_gioi_open(port);
            if (pt_gioi_driver->pfunc_gioi_reg_rx_notifier) {
                ret |= pt_gioi_driver->pfunc_gioi_reg_rx_notifier(port, _etos_gioi_rx_notifier_wrapper);
            }
        }
    }

    if (ret == ETOS_RET_OK) {
        *handle = (u32)pt_gioi_driver;
        _os_gioi_opened[port] = TRUE;
    } else {
        *handle = 0;
    }

    return ret;
}



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
s32 etos_gioi_close(u32 handle)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((handle == 0) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_close) {
            ret = pt_gioi_driver->pfunc_gioi_open(port);
            _os_gioi_opened[port] = FALSE;
        }
    }
    return ret;
}



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
u32 etos_gioi_get_handle(u32 port)
{
    gioi_driver_t *pt_gioi_driver;

    if ((port < MAX_GIOI_DRVIER_NUM) && _os_gioi_opened[port]) {
        pt_gioi_driver = &_os_gioi_drivers[port];
        if (pt_gioi_driver) {
            return (u32)pt_gioi_driver;
        }
    }

    return 0;
}



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
s32 etos_gioi_register_rx_notifier(u32 handle, pfunc_rx_notifier rx_notifier)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((rx_notifier == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        _os_gioi_rx_notifiers[port] = rx_notifier;
        ret = ETOS_RET_OK;
    }
    return ret;
}



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
s32 etos_gioi_config(u32 handle, u32 cfg_mask, void *cfg_arg)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((cfg_arg == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_config) {
            ret = pt_gioi_driver->pfunc_gioi_config(port, cfg_mask, cfg_arg);
        }
    }
    return ret;
}



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
s32 etos_gioi_startup(u32 handle)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (_os_gioi_rx_notifiers[port] == NULL) {
            ret = ETOS_RET_FAIL;
        } else if (pt_gioi_driver->pfunc_gioi_startup) {
            ret = pt_gioi_driver->pfunc_gioi_startup(port);
        }
    }
    return ret;
}



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
s32 etos_gioi_shutdown(u32 handle, u32 reason)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_shutdown) {
            ret = pt_gioi_driver->pfunc_gioi_shutdown(port, reason);
        }
    }
    return ret;
}



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
s32 etos_gioi_tx_buf_size(u32 handle, u32 *size)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((size == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_tx_buf_size) {
            ret = pt_gioi_driver->pfunc_gioi_tx_buf_size(port, size);
        }
    }
    return ret;
}



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
s32 etos_gioi_tx_buf_free(u32 handle, u32 *free_size)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((free_size == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_tx_buf_free) {
            ret = pt_gioi_driver->pfunc_gioi_tx_buf_free(port, free_size);
        }
    }
    return ret;
}



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
s32 etos_gioi_put_char(u32 handle, u8 ch)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_put_char) {
            ret = pt_gioi_driver->pfunc_gioi_put_char(port, ch);
        }
    }
    return ret;
}



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
u32 etos_gioi_put_bytes(u32 handle, BOOL wait_until_sent, u8 *buf, u32 len)
{
    u32 port;
    u32 ret = 0;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((buf == NULL) || (len == 0) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ret;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_put_bytes) {
            ret = pt_gioi_driver->pfunc_gioi_put_bytes(port, wait_until_sent, buf, len);
        }
    }
    return ret;
}



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
s32 etos_gioi_rx_buf_size(u32 handle, u32 *size)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((size == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_rx_buf_size) {
            ret = pt_gioi_driver->pfunc_gioi_rx_buf_size(port, size);
        }
    }
    return ret;
}



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
s32 etos_gioi_rx_buf_length(u32 handle, u32 *len)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((len == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_rx_buf_length) {
            ret = pt_gioi_driver->pfunc_gioi_rx_buf_length(port, len);
        }
    }
    return ret;
}



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
s32 etos_gioi_get_char(u32 handle, u8 *ch)
{
    u32 port;
    s32 ret = ETOS_NOT_SUPPORT;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((ch == NULL) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ETOS_INVALID_PARAM;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_get_char) {
            ret = pt_gioi_driver->pfunc_gioi_get_char(port, ch);
        }
    }
    return ret;
}



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
u32 etos_gioi_get_bytes(u32 handle, u8 *buf, u32 len)
{
    u32 port;
    u32 ret = 0;
    gioi_driver_t *pt_gioi_driver = (gioi_driver_t *)handle;

    if ((buf == NULL) || (len == 0) || (pt_gioi_driver < &_os_gioi_drivers[0])
        || (pt_gioi_driver > &_os_gioi_drivers[MAX_GIOI_DRVIER_NUM - 1])) {
        return ret;
    } else {
        port = pt_gioi_driver - &_os_gioi_drivers[0];
        if (pt_gioi_driver->pfunc_gioi_get_bytes) {
            ret = pt_gioi_driver->pfunc_gioi_get_bytes(port, buf, len);
        }
    }
    return ret;
}


/* EOF */

