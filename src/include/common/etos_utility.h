/******************************************************************************
File    :  etos_utility.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		etos utility
		exact time os 实用函数部分

History:

Date           Author       Notes
----------     -------      -------------------------
2013-11-30     deeve        Create

*******************************************************************************/
#ifndef __ETOS_UTILITY_H__
#define __ETOS_UTILITY_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#if 0
typedef char *va_list;

/*获取类型占用的空间长度，最小占用长度为int的整数倍，why int, 因为长度小于int的类型会被提升为int*/
#define _INTSIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

/*获取可变参数列表的第一个参数的地址(ap是类型为va_list的指针，v是可变参数最左边的参数)*/
#define va_start(ap,v)  (ap = (va_list)&v + _INTSIZEOF(v))

/*获取可变参数的当前参数，返回指定类型并将指针指向下一参数(t参数描述了当前参数的类型)*/
#define va_arg(ap,t)    (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))


/*清空va_list可变参数列表*/
#define va_end(ap)      (ap = (va_list)0)
#endif

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

/*APIs below usage is same with standard C lib*/

char *strcpy(char *pc_dest, const char *pc_src);

char *strncpy(char *pc_dest, const char *pc_src, u32 len);

char *strcat(char *pc_dest, const char *pc_src);

char *strncat(char *pc_dest, const char *pc_src, u32 len);

int strcmp(const char *pc_s1, const char *pc_s2);

int strncmp(const char *pc_s1, const char *pc_s2, u32 len);

u32 strlen(const char *pc_str);

void *memset(void *addr, u8 ch, u32 len);

void *memcpy(void *dest, void const *src, u32 len);

#endif  /* __ETOS_UTILITY_H__ */

/* EOF */

