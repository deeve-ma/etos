/******************************************************************************
File    :  etos_random.c

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		random generator source file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-22      deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_random.h"
/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/
#if (ETOS_RANDOM_USE_MORE_MEMORY)

#define MT_ARRAY_SIZE         (624)  /*do not modify it*/

static u32 _os_random_mt_array[MT_ARRAY_SIZE];
static u32 _os_random_mt_index;

#else

static u32 _os_random_sys_seed;

#endif
/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
#if (ETOS_RANDOM_USE_MORE_MEMORY)

/*
Mersenne Twist Pseudorandom Number Generator
梅森旋转算法
*/


/**
 * initialise random seed.
 * initialise system random seed
 *
 * @param[in]    seed     init random seed
 *
 * @return  none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_random_sys_init_seed(u32 seed)
{
    u32 i, last_val, p;

    _os_random_mt_array[0] = seed;

    for (i = 1; i < MT_ARRAY_SIZE; i++) {
        last_val = _os_random_mt_array[i - 1];
        p = (last_val ^ (last_val >> 30)) + i;
        _os_random_mt_array[i] = 0x6c078965 * p;
    }
}



/**
 * update random seed.
 * update system random seed, it is called in interrupt service routine
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_random_sys_update(void)
{
    u32 i, val;

    for (i = 0; i < MT_ARRAY_SIZE; i++) {
        val = (_os_random_mt_array[i] & 0x80000000) +
              (_os_random_mt_array[(i + 1) % MT_ARRAY_SIZE] & 0x7fffffff);
        _os_random_mt_array[i] = _os_random_mt_array[(i + 397) % MT_ARRAY_SIZE] ^ (val >> 1);
        if (val % 2) { /*val is odd*/
            _os_random_mt_array[i] ^= 0x9908b0df;
        }
    }
}



/**
 * get random number.
 * get system random number
 *
 * @param[in]    void
 *
 * @return   random number
 *
 * @note     none
 * @authors    deeve
 * @date       2015/4/14
 */
u32 etos_random_sys_get(void)
{
    u32 val;

    if (_os_random_mt_index == 0) {
        etos_random_sys_update();
    }

    val = _os_random_mt_array[_os_random_mt_index];
    val = val ^ (val >> 11);
    val = val ^ ((val << 7) & 0x9d2c5680);
    val = val ^ ((val << 15) & 0xefc60000);
    val = val ^ (val >> 18);

    _os_random_mt_index = (_os_random_mt_index + 1) % MT_ARRAY_SIZE;

    return val;
}



/**
 * dump random number.
 * dump random number from input seed
 *
 * @param[in]    input_seed
 *
 * @return   random number
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
u32 etos_random_dump_value(u32 input_seed)
{
    u32 val;

    val = input_seed;
    val = val ^ (val >> 11);
    val = val ^ ((val << 7) & 0x9d2c5680);
    val = val ^ ((val << 15) & 0xefc60000);
    val = val ^ (val >> 18);

    return val;
}


#else


/*
线性同余发生器(Linear Congruential Generator)代表了最好最朴素的伪随机数产生器算法。
主要原因是容易理解，容易实现，而且速度快。

LCG 算法数学上基于公式：
X(n+1) = (a * X(n) + c) % m

a c m 的取值同glibc
a = 1103515245
c = 12345
m = 2^32
*/


/**
 * initialise random seed.
 * initialise system random seed
 *
 * @param[in]    seed     init random seed
 *
 * @return  none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_random_sys_init_seed(u32 seed)
{
    _os_random_sys_seed = seed & 0x7fffffff;
}



/**
 * update random seed.
 * update system random seed, it is called in interrupt service routine
 *
 * @param[in]    void
 *
 * @return   none
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
void etos_random_sys_update(void)
{
    _os_random_sys_seed = _os_random_sys_seed * 1103515245 + 12345;
}



/**
 * get random number.
 * get system random number
 *
 * @param[in]    void
 *
 * @return   random number
 *
 * @note     none
 * @authors    deeve
 * @date       2015/4/14
 */
u32 etos_random_sys_get(void)
{
    _os_random_sys_seed = _os_random_sys_seed * 1103515245 + 12345;
    return _os_random_sys_seed;
}



/**
 * dump random number.
 * dump random number from input seed
 *
 * @param[in]    input_seed
 *
 * @return   random number
 *
 * @note none
 * @authors    deeve
 * @date       2015/4/14
 */
u32 etos_random_dump_value(u32 input_seed)
{
    input_seed = input_seed * 1103515245 + 12345;
    return input_seed;
}


#endif

/* EOF */

