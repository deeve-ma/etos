/******************************************************************************
File    :  xlog.h

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
#ifndef __XLOG_H__
#define __XLOG_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/*default output port*/
#define XLOG_OUTPUT_PORT        (PORT_0_UART_0)


#define LOG_MODULE_ETOS         (0)

#define LOG_MODULE_BOOT         (1)
#define LOG_MODULE_DRV          (2)
#define LOG_MODULE_DISPATCH     (3)
#define LOG_MODULE_T_TASK       (4)



#define LOG_MODULE_LMP          (6)
#define LOG_MODULE_RF           (7)
#define LOG_MODULE_MAX          (8)

#if (ETOS_DEBUG)
#define LOG_LEVEL_MASK_DEFAULT  ((1<<ETOS_LOG_LEVEL_TRACE)|(1<<ETOS_LOG_LEVEL_WARN) \
                                 |(1<<ETOS_LOG_LEVEL_ERROR)|(1<<ETOS_LOG_LEVEL_FATAL)|(1<<ETOS_LOG_LEVEL_INFOR))
#else
#define LOG_LEVEL_MASK_DEFAULT  ((1<<ETOS_LOG_LEVEL_TRACE)|(1<<ETOS_LOG_LEVEL_WARN) \
                                 |(1<<ETOS_LOG_LEVEL_ERROR)|(1<<ETOS_LOG_LEVEL_FATAL))
#endif

typedef enum _log_level_e {
    ETOS_LOG_LEVEL_VERBOSE,    /* 0 -- just for user debug*/
    ETOS_LOG_LEVEL_DEBUG,      /* 1 -- just for user debug */

    ETOS_LOG_LEVEL_INFOR = 3,  /* 3 -- information message */
    ETOS_LOG_LEVEL_TRACE,      /* 4 -- normal case, but need pay attention*/
    ETOS_LOG_LEVEL_WARN,       /* 5 -- warning case */
    ETOS_LOG_LEVEL_ERROR,      /* 6 -- error, but maybe it can recover*/
    ETOS_LOG_LEVEL_FATAL,      /* 7 -- fatal erro, system is invalid*/
} log_level_e;


#ifdef __GNUC__  /*it is defined in gcc*/

#define logv(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_VERBOSE, h, m, ##args)
#define logd(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_DEBUG, h, m, ##args)
#define logi(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_INFOR, h, m, ##args)
#define logt(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_TRACE, h, m, ##args)
#define logw(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_WARN, h, m, ##args)
#define loge(h, m, args...)     log_level_printf(ETOS_LOG_LEVEL_ERROR, h, m, ##args)

#define logv_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_VERBOSE, h, m, ##args)
#define logd_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_DEBUG, h, m, ##args)
#define logi_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_INFOR, h, m, ##args)
#define logt_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_TRACE, h, m, ##args)
#define logw_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_WARN, h, m, ##args)
#define loge_d(h, m, args...)     log_level_printf_directly(ETOS_LOG_LEVEL_ERROR, h, m, ##args)


#define logf(h, m, args...)     log_level_fatal(ETOS_LOG_LEVEL_FATAL, h, m, ##args)


#define log_level_printf(level, output_handle, module, fmt...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            printf(output_handle, ##fmt); }\
    }while(0)

#define log_level_printf_directly(level, output_handle, module, fmt...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            printf_directly(output_handle, ##fmt); }\
    }while(0)

#define log_level_fatal(level, output_handle, module, fmt...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            panic(output_handle, ##fmt); }\
    }while(0)

#define xlogv(m, args...)     logv(xlog_get_output_handle(), m, ##args)
#define xlogd(m, args...)     logd(xlog_get_output_handle(), m, ##args)

#define xlogi(m, args...)     logi(xlog_get_output_handle(), m, ##args)
#define xlogt(m, args...)     logt(xlog_get_output_handle(), m, ##args)
#define xlogw(m, args...)     logw(xlog_get_output_handle(), m, ##args)
#define xloge(m, args...)     loge(xlog_get_output_handle(), m, ##args)
#define xlogf(m, args...)     logf(xlog_get_output_handle(), m, ##args)

#define xlogv_d(m, args...)     logv_d(xlog_get_output_handle(), m, ##args)
#define xlogd_d(m, args...)     logd_d(xlog_get_output_handle(), m, ##args)

#define xlogi_d(m, args...)     logi_d(xlog_get_output_handle(), m, ##args)
#define xlogt_d(m, args...)     logt_d(xlog_get_output_handle(), m, ##args)
#define xlogw_d(m, args...)     logw_d(xlog_get_output_handle(), m, ##args)
#define xloge_d(m, args...)     loge_d(xlog_get_output_handle(), m, ##args)
#define xlogf_d(m, args...)     logf(xlog_get_output_handle(), m, ##args)

#elif (__STDC_VERSION__ >= 199901L)  /*just for C99*/

#define logv(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_VERBOSE, h, m, format, ##__VA_ARGS__)
#define logd(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_DEBUG, h, m, format, ##__VA_ARGS__)
#define logi(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_INFOR, h, m, format, ##__VA_ARGS__)
#define logt(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_TRACE, h, m, format, ##__VA_ARGS__)
#define logw(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_WARN, h, m, format, ##__VA_ARGS__)
#define loge(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_ERROR, h, m, format, ##__VA_ARGS__)


#define logv_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_VERBOSE, h, m, format, ##__VA_ARGS__)
#define logd_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_DEBUG, h, m, format, ##__VA_ARGS__)
#define logi_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_INFOR, h, m, format, ##__VA_ARGS__)
#define logt_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_TRACE, h, m, format, ##__VA_ARGS__)
#define logw_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_WARN, h, m, format, ##__VA_ARGS__)
#define loge_d(h, m, format, ...)     log_level_printf_directly(ETOS_LOG_LEVEL_ERROR, h, m, format, ##__VA_ARGS__)



#define logf(h, m, format, ...)     log_level_printf(ETOS_LOG_LEVEL_FATAL, h, m, format, ##__VA_ARGS__)

#define log_level_printf(level, output_handle, module, fmt, ...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            printf(output_handle, fmt, ##__VA_ARGS__); }\
    }while(0);

#define log_level_printf_directly(level, output_handle, module, fmt, ...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            printf_directly(output_handle, fmt, ##__VA_ARGS__); }\
    }while(0);

#define log_level_fatal(level, output_handle, module, fmt, ...)  do { \
        u8* level_cfg = xlog_level_get_config(); \
        if ((module < LOG_MODULE_MAX) && \
            ((*(level_cfg + module)) & (1<<level))){ \
            panic(output_handle, fmt, ##__VA_ARGS__); }\
    }while(0);


#define xlogv(m, fmt, ...)     logv(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogd(m, fmt, ...)     logd(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)

#define xlogi(m, fmt, ...)     logi(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogt(m, fmt, ...)     logt(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogw(m, fmt, ...)     logw(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xloge(m, fmt, ...)     loge(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogf(m, fmt, ...)     logf(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)

#define xlogv_d(m, fmt, ...)     logv_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogd_d(m, fmt, ...)     logd_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)

#define xlogi_d(m, fmt, ...)     logi_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogt_d(m, fmt, ...)     logt_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogw_d(m, fmt, ...)     logw_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xloge_d(m, fmt, ...)     loge_d(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)
#define xlogf_d(m, fmt, ...)     logf(xlog_get_output_handle(), m, fmt, ##__VA_ARGS__)

#else

#error "maybe you need use gcc or add compile option: -std=c99 for C99"

#endif


#if (defined(ETOS_DEBUG) || defined(ETOS_FORCE_ASSERT))
#define ASSERT(expr)  if (!(expr)) { \
        panic(xlog_get_output_handle(), \
              "Assert: %s @%s:%d\n", #expr, __FUNCTION__, __LINE__); }
#else
#define ASSERT(expr)  ((void)0)
#endif


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

s32 xlog_init(u32 output_port);

s32 xlog_destroy(void);

u32 xlog_get_output_handle(void);

s32 xlog_level_set(u32 module, u8 level);
s32 xlog_level_get(u32 module, u8 *level);
u8 *xlog_level_get_config(void);

#endif  /* __XLOG_H__ */

/* EOF */

