/******************************************************************************
File    :  etos_msgq.c

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os message queue

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-20     deeve        Create
2015-4-14      deeve        Add some comments

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

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
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
s32 etos_msgq_create(etos_msg_type msg_type, etos_msg_handle *msg_handle)
{
    etos_msgq_head_t *msgq_head;

    if (msg_handle == NULL) {
        return ETOS_INVALID_PARAM;
    }

    msgq_head = (etos_msgq_head_t *)malloc(sizeof(etos_msgq_head_t));
    if (msgq_head == NULL) {
        return ETOS_NO_MEM;
    }

    msgq_head->msg_type = msg_type;
    msgq_head->msg_check_flag = ETOS_MSG_CHECK_FLAG;
    msgq_head->recv_task = 0;
    INIT_LIST_HEAD(&msgq_head->list);

    *msg_handle = (etos_msg_handle)msgq_head;

    return ETOS_RET_OK;
}



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
s32 etos_msgq_destroy(etos_msg_handle msg_handle)
{
    list_t *entry;
    etos_msgq_buf_t *msgq_buf;
    etos_task_state_e recv_task_state;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;
    etos_init_critical();

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)) {
        return ETOS_INVALID_PARAM;
    }

    /*clear msgq*/
    etos_enter_critical();

    if (etos_sched_get_task_state(msgq_head->recv_task, &recv_task_state)) {
        xlogw(LOG_MODULE_ETOS, "get task state error\n");
    } else {
        if (recv_task_state & ETOS_TASK_PENDING_MSG) {
            /*task is waiting for message*/
            etos_exit_critical();
            return ETOS_NOT_SUPPORT;
        }
    }

    while (!list_is_empty(&msgq_head->list)) { //not empty, it has msg
        entry = list_dequeue(&msgq_head->list);
        msgq_buf = list_entry(entry, etos_msgq_buf_t, list);
        if (msgq_buf) {
            free(msgq_buf);  //free buf
        }
    }
    free(msgq_head);  // free head

    etos_exit_critical();

    return ETOS_RET_OK;
}



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
u8 *etos_msgq_get_buf(etos_msg_handle msg_handle, u32 len)
{
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;

    if ((msgq_head == NULL) || (len == 0)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)) {
        return NULL;
    }

    msgq_buf = (etos_msgq_buf_t *)malloc(len + sizeof(etos_msgq_buf_t));
    if (msgq_buf == NULL) {
        return NULL;
    }

    msgq_buf->msg_check_flag = ETOS_MSG_CHECK_FLAG;

    xlogi(LOG_MODULE_ETOS, "msg_get_buf: handle=0x%x len=%d\r\n", msg_handle, len);

    return msgq_buf->msg_data;
}



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
s32 etos_msgq_release_buf(etos_msg_handle msg_handle, u8 *msg_buf)
{
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)
        || (msg_buf == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    /*根据msg_data的指针获取etos_msgq_buf_t的指针*/
    msgq_buf = list_entry(msg_buf, etos_msgq_buf_t, msg_data);
    if (msgq_buf->msg_check_flag != ETOS_MSG_CHECK_FLAG) {
        return ETOS_INVALID_PARAM;
    }

    xlogi(LOG_MODULE_ETOS, "release buf: handle=0x%x buf=0x%x\r\n", msg_handle, (u32)(msgq_buf->msg_data));


    //INIT_LIST_HEAD(&msgq_buf->list);  //have done in recv list_dequeue
    free(msgq_buf);

    return ETOS_RET_OK;
}



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
s32 etos_msgq_send(etos_msg_handle msg_handle, u8 *msg_buf)
{
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;
    etos_init_critical();

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)
        || (msg_buf == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    /* 根据msg_data的指针获取etos_msgq_buf_t的指针 */
    msgq_buf = list_entry(msg_buf, etos_msgq_buf_t, msg_data);
    if (msgq_buf->msg_check_flag != ETOS_MSG_CHECK_FLAG) {
        return ETOS_RET_FAIL;
    }

    if (etos_intr_in_isr()) {
        msgq_buf->send_task = ETOS_INTR_VIRTUAL_TASK_HANDLE;
    } else {
        msgq_buf->send_task = etos_sched_get_current_task();
    }

    xlogi(LOG_MODULE_ETOS, "msg_send: handle=0x%x buf=0x%x\r\n", msg_handle, (u32)msg_buf);

    etos_enter_critical();

    list_add_tail(&msgq_buf->list, &msgq_head->list);
    etos_sched_resume_task(msgq_head->recv_task, ETOS_TASK_PENDING_MSG);

    etos_exit_critical();

    return ETOS_RET_OK;
}



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
s32 etos_msgq_send_idic(etos_msg_handle msg_handle, u8 *msg_buf)
{
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)
        || (msg_buf == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    /* 根据msg_data的指针获取etos_msgq_buf_t的指针 */
    msgq_buf = list_entry(msg_buf, etos_msgq_buf_t, msg_data);
    if (msgq_buf->msg_check_flag != ETOS_MSG_CHECK_FLAG) {
        return ETOS_RET_FAIL;
    }

    if (etos_intr_in_isr()) {
        msgq_buf->send_task = ETOS_INTR_VIRTUAL_TASK_HANDLE;
    } else {
        msgq_buf->send_task = etos_sched_get_current_task();
    }

    xlogi(LOG_MODULE_ETOS, "msg_send: handle=0x%x buf=0x%x\r\n", msg_handle, (u32)msg_buf);

    list_add_tail(&msgq_buf->list, &msgq_head->list);
    etos_sched_resume_task_idic(msgq_head->recv_task, ETOS_TASK_PENDING_MSG);

    return ETOS_RET_OK;
}



/**
 * receive a message queue buffer.
 * receive a message queue buffer, maybe it will cause task reschedule when
 * there is no message queue buffert to be received
 * do not call this API in an interrupt context
 *
 * @param[in]    msg_handle
 * @param[out]   msg_buf_ptr       output message queue buffer
 * @param[out]   send_task         output send message queue buffer task
 *                                 it equal ETOS_INTR_VIRTUAL_TASK_HANDLE when send message in ISR
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
s32 etos_msgq_recv(etos_msg_handle msg_handle, u8 **msg_buf_ptr, etos_task_handle *send_task)
{
    list_t *entry;
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;
    etos_init_critical();

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)
        || (msg_buf_ptr == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    if (etos_intr_in_isr()) {
        xlogf(LOG_MODULE_ETOS, "can not recv message in ISR\r\n");
    }

    msgq_head->recv_task = etos_sched_get_current_task();

    while (1) {
        if (list_is_empty(&msgq_head->list)) {
            /*pending itself becasue of message*/
            etos_sched_pending_task(msgq_head->recv_task, ETOS_TASK_PENDING_MSG);
        } else {
            etos_enter_critical();
            entry = list_dequeue(&msgq_head->list);
            if (entry) {
                msgq_buf = list_entry(entry, etos_msgq_buf_t, list);
                etos_exit_critical();
                break;
            } else {
                etos_exit_critical();
                continue;
            }
        }
    }

    ASSERT(msgq_buf->msg_check_flag == ETOS_MSG_CHECK_FLAG);

    xlogi(LOG_MODULE_ETOS, "msg_recv: handle=0x%x buf=0x%x\r\n", msg_handle, (u32)(msgq_buf->msg_data));

    if (send_task) {
        *send_task = msgq_buf->send_task;
    }

    *msg_buf_ptr = msgq_buf->msg_data;

    return ETOS_RET_OK;
}



/**
 * receive a message queue buffer.
 * receive a message queue buffer, it won't be blocking when
 * there is no message queue buffert to be received, instead, it output NULL buffer pointer
 * do not call this API in an interrupt context
 *
 * @param[in]    msg_handle
 * @param[out]   msg_buf_ptr       output message queue buffer
 * @param[out]   send_task         output send message queue buffer task
 *                                 it equal ETOS_INTR_VIRTUAL_TASK_HANDLE when send message in ISR
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
s32 etos_msgq_recv_no_block(etos_msg_handle msg_handle, u8 **msg_buf_ptr, etos_task_handle *send_task)
{
    list_t *entry;
    etos_msgq_buf_t *msgq_buf;
    etos_msgq_head_t *msgq_head = (etos_msgq_head_t *)msg_handle;
    etos_init_critical();

    if ((msgq_head == NULL)
        || (msgq_head->msg_check_flag != ETOS_MSG_CHECK_FLAG)
        || (msg_buf_ptr == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    if (etos_intr_in_isr()) {
        xlogf(LOG_MODULE_ETOS, "can not recv message in ISR\r\n");
    }

    msgq_head->recv_task = etos_sched_get_current_task();

    if (list_is_empty(&msgq_head->list)) {
        *msg_buf_ptr = NULL;
        if (send_task) {
            send_task = 0;
        }
        return ETOS_RET_OK;
    } else {
        etos_enter_critical();
        entry = list_dequeue(&msgq_head->list);
        msgq_buf = list_entry(entry, etos_msgq_buf_t, list);
        etos_exit_critical();
    }

    ASSERT(msgq_buf->msg_check_flag == ETOS_MSG_CHECK_FLAG);

    xlogi(LOG_MODULE_ETOS, "msg_recv: handle=0x%x buf=0x%x\r\n", msg_handle, (u32)(msgq_buf->msg_data));

    if (send_task) {
        *send_task = msgq_buf->send_task;
    }

    *msg_buf_ptr = msgq_buf->msg_data;

    return ETOS_RET_OK;
}



/* EOF */

