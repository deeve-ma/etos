/******************************************************************************
File    :  printf.h

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		printf functions implement for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-10-6     deeve        Create

*******************************************************************************/
#ifndef __PRINTF_H__
#define __PRINTF_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_types.h"
/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define MAX_PRT_BUF_SIZE    (256)


#define ZEROPAD     (1 << 0)	/* pad with zero */
#define SIGN        (1 << 1)	/* unsigned/signed */
#define PLUS        (1 << 2)	/* show plus */
#define SPACE       (1 << 3)	/* space if plus */
#define LEFT        (1 << 4)	/* left justified */
#define SPECIAL     (1 << 5)	/* 0x */
#define LARGE       (1 << 6)	/* use 'ABCDEF' instead of 'abcdef' */

#ifndef isprint
#define IS_IN_RANGE(c, lower, uper)  (((u32)c >= lower) && ((u32)c <= uper))

#define isprint(c)           IS_IN_RANGE(c, 0x20, 0x7f)
#define isdigit(c)           IS_IN_RANGE(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || IS_IN_RANGE(c, 'a', 'f') || IS_IN_RANGE(c, 'A', 'F'))
#define islower(c)           IS_IN_RANGE(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

#if 1
#define DIVIDE(n,base) ({ \
        u32 __res; \
        __res = ((u32) n) % (u32) base; \
        n = ((u32) n) / (u32) base; \
        __res; \
    })

#else
static inline u32 DIVIDE(u32 *n, u32 base)
{
    u32 ret;
    ret = *n % base;
    *n = (*n) / base;
    return ret;
}
#endif

#if 0 //def __GNUC__  /*it is defined in gcc*/

#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

#if 0
/******************************************************************************
 * GCC always defines __va_copy, but does not define va_copy unless in c99 mode
 * or -ansi is not specified, since it was not part of C90.
 ******************************************************************************/
#define __va_copy(d,s) __builtin_va_copy(d,s)

#if __STDC_VERSION__ >= 199900L || __cplusplus >= 201103L || !defined(__STRICT_ANSI__)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)
#endif
#endif

#else /* #ifdef  _MSC_VER for ms cl.exe*/  /*not build with gcc*/

typedef char *va_list;

#if 0
/*获取类型占用的空间长度，最小占用长度为int的整数倍，why int, 因为长度小于int的类型会被提升为int*/
#define INT_SIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#endif
#define INT_SIZEOF(n)   ((sizeof(n) + sizeof(s32) - 1) & ~(sizeof(s32) - 1))

/*获取可变参数列表的第一个参数的地址(ap是类型为va_list的指针，v是可变参数最左边的参数)*/
#define va_start(ap, param)  (ap = (va_list)&param + INT_SIZEOF(param))

/*获取可变参数的当前参数，返回指定类型并将指针指向下一参数(t参数描述了当前参数的类型)*/
#define va_arg(ap,type)    (*(type *)((ap += INT_SIZEOF(type)) - INT_SIZEOF(type)))


/*清空va_list可变参数列表*/
#define va_end(ap)      (ap = (va_list)0)

#ifndef va_copy
#ifdef __va_copy
#define va_copy(d,s) __va_copy((d),(s))
#else
#define va_copy(d, s) memcpy((&d), (&s), sizeof(va_list))
#endif
#endif

#endif

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

u32 vsnprintf(s8 *buf, u32 buf_size, const s8 *fmt, va_list args);

u32 snprintf(s8 *buf, u32 buf_size, const s8 *fmt, ...);

void printf(u32 output_handle, const s8 *fmt, ...);

void printf_directly(u32 output_handle, const s8 *fmt, ...);

void panic(u32 output_handle, const s8 *fmt, ...);


#endif  /* __PRINTF_H__ */

/* EOF */

