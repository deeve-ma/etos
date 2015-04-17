/******************************************************************************
File    :  etos_random.h

This file is part of the ETOS distribution
Copyright (c) 2015, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		random generator header file

History:

Date           Author       Notes
----------     -------      -------------------------
2015-3-22      deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/
#ifndef __ETOS_RANDOM_H__
#define __ETOS_RANDOM_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_types.h"
/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

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
void etos_random_sys_init_seed(u32 seed);



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
void etos_random_sys_update(void);



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
u32 etos_random_sys_get(void);



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
u32 etos_random_dump_value(u32 input_seed);



#endif  /* __ETOS_RANDOM_H__ */

/* EOF */

