/******************************************************************************
File    :  etos_utility.c

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

u32 N_BITS_BINARY_ONE[] = {
    /*0*/   0x00000000,    /*not valid*/
    /*1*/   0x00000001,    /*0b1*/
    /*2*/   0x00000003,    /*0b11*/
    /*3*/   0x00000007,    /*0b111*/
    /*4*/   0x0000000f,    /*0b1111*/
    /*5*/   0x0000001f,    /*0b11111*/
    /*6*/   0x0000003f,    /*0b111111*/
    /*7*/   0x0000007f,    /*0b1111111*/
    /*8*/   0x000000ff,    /*0b11111111*/
    /*9*/   0x000001ff,    /*0b111111111*/
    /*10*/  0x000003ff,    /*0b1111111111*/
    /*11*/  0x000007ff,    /*0b11111111111*/
    /*12*/  0x00000fff,    /*0b111111111111*/
    /*13*/  0x00001fff,    /*0b1111111111111*/
    /*14*/  0x00003fff,    /*0b11111111111111*/
    /*15*/  0x00007fff,    /*0b111111111111111*/
    /*16*/  0x0000ffff,    /*0b1111111111111111*/
    /*17*/  0x0001ffff,    /*0b11111111111111111*/
    /*18*/  0x0003ffff,    /*0b111111111111111111*/
    /*19*/  0x0007ffff,    /*0b1111111111111111111*/
    /*20*/  0x000fffff,    /*0b11111111111111111111*/
    /*21*/  0x001fffff,    /*0b111111111111111111111*/
    /*22*/  0x003fffff,    /*0b1111111111111111111111*/
    /*23*/  0x007fffff,    /*0b11111111111111111111111*/
    /*24*/  0x00ffffff,    /*0b111111111111111111111111*/
    /*25*/  0x01ffffff,    /*0b1111111111111111111111111*/
    /*26*/  0x03ffffff,    /*0b11111111111111111111111111*/
    /*27*/  0x07ffffff,    /*0b111111111111111111111111111*/
    /*28*/  0x0fffffff,    /*0b1111111111111111111111111111*/
    /*29*/  0x1fffffff,    /*0b11111111111111111111111111111*/
    /*30*/  0x3fffffff,    /*0b111111111111111111111111111111*/
    /*31*/  0x7fffffff,    /*0b1111111111111111111111111111111*/
    /*32*/  0xffffffff     /*0b11111111111111111111111111111111*/
};

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/


char *strcpy(char *pc_dest, const char *pc_src)
{
    char *ret = pc_dest;

    while ((*pc_dest++ = *pc_src++) != '\0'); /*do nothing in loop body*/

    return ret;
}



char *strncpy(char *pc_dest, const char *pc_src, u32 len)
{
    char *ret = pc_dest;

    while ((len--) && ((*pc_dest++ = *pc_src++) != '\0')); /*do nothing in loop body*/

    return ret;
}



char *strcat(char *pc_dest, const char *pc_src)
{
    char *ret = pc_dest;

    while (*pc_dest) {
        pc_dest++;
    }

    while ((*pc_dest++ = *pc_src++) != '\0'); /*do nothing in loop body*/

    return ret;
}



char *strncat(char *pc_dest, const char *pc_src, u32 len)
{
    char *ret = pc_dest;

    if (len) {
        while (*pc_dest) {
            pc_dest++;
        }
        while ((*pc_dest++ = *pc_src++)) {
            if (--len == 0) {
                *pc_dest = '\0';
                break;
            }
        }
    }

    return ret;
}



int strcmp(const char *pc_s1, const char *pc_s2)
{
    register signed char ret;

    while (1) {
        if ((ret = *pc_s1 - *pc_s2++) != 0 || !*pc_s1++) {
            break;
        }
    }

    return ret;
}



int strncmp(const char *pc_s1, const char *pc_s2, u32 len)
{
    register signed char ret = 0;

    while (len) {
        if ((ret = *pc_s1 - *pc_s2++) != 0 || !*pc_s1++) {
            break;
        }
        len--;
    }

    return ret;
}



u32 strlen(const char *pc_str)
{
    const char *pc_end;

    for (pc_end = pc_str; *pc_end != '\0'; ++pc_end) {
        /*do nothing in loop body*/;
    }

    return (u32)(pc_end - pc_str);
}



void *memset(void *addr, u8 ch, u32 len)
{
    char *pc_str = (char *)addr;

    while (len--) {
        *pc_str++ = ch;
    }

    return addr;
}



void *memcpy(void *dest, void const *src, u32 len)
{
    u32 *pu4_dest = (u32 *) dest;
    u32 const *pu4_src = (u32 const *) src;

    if ((((u32)src & 0x3) == 0) && (((u32)dest & 0x3) == 0)) {
        while (len >= 4) {
            *pu4_dest++ = *pu4_src++;
            len -= 4;
        }
    }

    u8 *pu1_dest = (u8 *) pu4_dest;
    u8 const *pu1_src = (u8 const *) pu4_src;

    while (len--) {
        *pu1_dest++ = *pu1_src++;
    }

    return dest;
}



/* just for link libgcc.a */
int raise(int signum)
{
    return 0;
}


/* EOF */

