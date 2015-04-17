/******************************************************************************
File    :  etos_hw_op.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os hardware operation header file

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-13     deeve        Create

*******************************************************************************/
#ifndef __ETOS_HW_OP_H__
#define __ETOS_HW_OP_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"


/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define REG_ADDR(addr)               ((volatile u32*)(addr))
#define REG16_ADDR(addr)             ((volatile u16*)(addr))


#define REG(addr)                   (*(REG_ADDR(addr)))
#define REG16(addr)                 (*(REG16_ADDR(addr)))


#define REG_SET_BIT(reg_addr, bit)  REG(reg_addr) |= (1<<(bit))               /*设置register的某一位(设置为1),某一位的位置由bit指定*/
#define REG_CLR_BIT(reg_addr, bit)  REG(reg_addr) &= (~(1<<(bit)))            /*清除register的某一位(设置为0),某一位的位置由bit指定*/

#define REG_GET_BIT(reg_addr, bit)  ((REG(reg_addr) >> bit) & 1)

#define REG_REVERSE_BIT(reg_addr, bit)   REG(reg_addr) ^= (1<<(bit))          /*反转register的某一位(1->0,0->1),某一位的位置由bit指定*/

#define REG_WAIT_ONE(reg_addr, bit)      while(!(REG(reg_addr) & (1<<bit)))   /*等待register的某一位直到它为1，注意，可能阻塞*/
#define REG_WAIT_ZERO(reg_addr, bit)     while((REG(reg_addr) & (1<<bit)))    /*等待register的某一位直到它为0，注意，可能阻塞*/




extern u32 N_BITS_BINARY_ONE[];  /* declared in etos_utility.c */




#if 0

#define REG_CLR_N_BITS(reg, from, n)        reg &= ~(N_BITS_BINARY_ONE[n] << from)


#define REG_SET_N_BITS(reg, from, n, val)   do { \
        ASSERT(n!=0); \
        if (val == 0) { \
            REG_CLR_N_BITS(reg, from, n); \
        } else { \
            u32 _mk = N_BITS_BINARY_ONE[n]; \
            reg |= ((reg & (~(_mk << from))) | ((val & _mk) << from)); \
        } \
    }while(0)



#define REG_GET_N_BITS(reg, from, n)        ((reg >> from) & N_BITS_BINARY_ONE[n])

#endif


#define REG_SET_VALUE(reg_addr, val)      REG(reg_addr) = val
#define REG_GET_VALUE(reg_addr)           REG(reg_addr)


/*
* write the register field to the value "val"
* the register field which start from "from" and its width is "n"
* and the register address is "reg_addr", it is a 32 bits register
*/
/*设置由reg_addr指定的register的从from开始的n位field, 设置的这个field的值是val*/
#define REG_SET_FIELD(reg_addr, from, n, val)      do { \
        ASSERT(n!=0); \
        if (val == 0) { \
            REG(reg_addr) &= ~(N_BITS_BINARY_ONE[n] << from); \
        } else { \
            u32 _mk = N_BITS_BINARY_ONE[n]; \
            REG(reg_addr) = ((REG(reg_addr) & (~(_mk << from))) | ((val & _mk) << from)); \
        } \
    }while(0)



/*
* read the register field value
* the register field which start from "from" and its width is "n"
* and the register address is "reg_addr", it is a 32 bits register
*/
/*读取由reg_addr指定的register的从from开始的n位field*/
#define REG_GET_FIELD(reg_addr, from, n)      ((REG(reg_addr) >> from) & N_BITS_BINARY_ONE[n])



#define REG16_SET_VALUE(reg_addr, val)    REG16(reg_addr) = val
#define REG16_GET_VALUE(reg_addr)         REG16(reg_addr)



/*
* write the register field to the value "val"
* the register field which start from "from" and its width is "n"
* and the register address is "reg_addr", it is a 16 bits register
*/
/*设置由reg_addr指定的register的从from开始的n位field, 设置的这个field的值是val*/
#define REG16_SET_FIELD(reg_addr, from, n, val)      do { \
        ASSERT(n!=0); \
        if (val == 0) { \
            REG16(reg_addr) &= ~(N_BITS_BINARY_ONE[n] << from); \
        } else { \
            u16 _mk = N_BITS_BINARY_ONE[n]; \
            REG16(reg_addr) = ((REG16(reg_addr) & (~(_mk << from))) | ((val & _mk) << from)); \
        } \
    }while(0)


/*
* read the register field value
* the register field which start from "from" and its width is "n"
* and the register address is "reg_addr", it is a 16 bits register
*/
/*读取由reg_addr指定的register的从from开始的n位field*/
#define REG16_GET_FIELD(reg_addr, from, n)      ((REG16(reg_addr) >> from) & N_BITS_BINARY_ONE[n])



/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_HW_OP_H__ */

/* EOF */

