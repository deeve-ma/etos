/******************************************************************************
File    :  etos_msgq.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os message queue header file

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-20     deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/
#ifndef __ETOS_MSGQ_H__
#define __ETOS_MSGQ_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_mem.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
typedef u32 etos_msg_type;
typedef u32 etos_msg_handle;


#define ETOS_MSG_CHECK_FLAG          (0x19851123)

typedef struct _etos_msgq_head {
    list_t            list;
    etos_msg_type     msg_type;
    etos_task_handle  recv_task;
    u32               msg_check_flag;
} etos_msgq_head_t;


typedef struct _etos_msgq_buf {
    list_t            list;
    etos_task_handle  send_task;
    u32               msg_check_flag;
    u8                msg_data[0];   /*msg content*/
} etos_msgq_buf_t;


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/


/**
 * create a message queue.
 * create a message queue for IPC(Communication between task)
 *
 * @param[in]    msg_type
 * @param[out]   msg_handle  output handle for other message queue API
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see        etos_msgq_destroy()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_create(etos_msg_type msg_type, etos_msg_handle *msg_handle);



/**
 * destroy a message queue.
 * destroy a message queue which is created by etos_msgq_create
 *
 * @param[in]    msg_handle
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see        etos_msgq_create()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_destroy(etos_msg_handle msg_handle);



/**
 * get a buffer.
 * get a message queue buffer which length is len
 * the buffer is the message content buffer to be send
 * user fill the buffer and send to target task
 *
 * @param[in]    msg_handle
 * @param[in]    len          buffer length
 *
 * @return    the message buffer pointer
 * @retval other   success
 * @retval 0       fail
 *
 * @note  it may be called before etos_msgq_send()
 * @see        etos_msgq_release_buf()
 * @authors    deeve
 * @date       2015/4/14
 */
u8 *etos_msgq_get_buf(etos_msg_handle msg_handle, u32 len);



/**
 * release a buffer.
 * release a message queue buffer which is got from etos_msgq_get_buf()
 *
 * @param[in]    msg_handle
 * @param[in]    msg_buf      which is returned by etos_msgq_get_buf()
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note it may be called after etos_msgq_recv()
 * @see        etos_msgq_get_buf()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_release_buf(etos_msg_handle msg_handle, u8 *msg_buf);



/**
 * send a message queue buffer.
 * send a message queue buffer to the task which is wait to receive massge queue
 *
 * @param[in]    msg_handle
 * @param[in]    msg_buf      which is returned by etos_msgq_get_buf()
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see        etos_msgq_recv()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_send(etos_msg_handle msg_handle, u8 *msg_buf);



/**
 * send a message queue buffer.
 * send a message queue buffer to the task which is wait to receive massge queue
 * it is called in a disable interrupt context
 *
 * @param[in]    msg_handle
 * @param[in]    msg_buf      which is returned by etos_msgq_get_buf()
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note none
 * @see        etos_msgq_recv()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_send_idic(etos_msg_handle msg_handle, u8 *msg_buf);



/**
 * receive a message queue buffer.
 * receive a message queue buffer, maybe it will cause task reschedule when
 * there is no message queue buffert to be received
 * do not call this API in an interrupt context
 *
 * @param[in]    msg_handle
 * @param[out]   msg_buf_ptr       output message queue buffer
 * @param[out]   send_task         output send message queue buffer task
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note       it will be blocking when there is no message buffer to be received
 * @see        etos_msgq_send()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_recv(etos_msg_handle msg_handle, u8 **msg_buf_ptr, etos_task_handle *send_task);



/**
 * receive a message queue buffer.
 * receive a message queue buffer, it won't be blocking when
 * there is no message queue buffert to be received, instead, it output NULL buffer pointer
 * do not call this API in an interrupt context
 *
 * @param[in]    msg_handle
 * @param[out]   msg_buf_ptr       output message queue buffer
 * @param[out]   send_task         output send message queue buffer task
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note       it is a non-blocking API of etos_msgq_recv()
 * @see        etos_msgq_send()
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_msgq_recv_no_block(etos_msg_handle msg_handle, u8 **msg_buf_ptr, etos_task_handle *send_task);



#endif  /* __ETOS_MSGQ_H__ */

/* EOF */

