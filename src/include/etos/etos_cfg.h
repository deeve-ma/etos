/******************************************************************************
File    :  etos_cfg.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os config header file
		相关功能的开关都在该文件中定义

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-12     deeve        Create

*******************************************************************************/
#ifndef __ETOS_CFG_H__
#define __ETOS_CFG_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/* version define */
#define ETOS_VERSION_MAIN    (0)    /*major version*/
#define ETOS_VERSION_SUB     (11)   /*develop version*/


/* -->  ETOS TASK defines  --> start*/

#define ETOS_MAX_PRIORITY_TASK_NUM               (32) /*do not let is larger than 32*/
#define ETOS_MAX_TASK_NAME_LEN                   (8)

#define ETOS_MAX_TASK_NUM     ETOS_MAX_PRIORITY_TASK_NUM

/* <--  ETOS TASK defines  <-- end*/



/* -->  ETOS interrupt defines  --> start*/

#define ETOS_UPDATE_RANDOM_IN_INTR               (1)
#define ETOS_KEEP_INTR_RANDOM_NUM                (1)

/* <--  ETOS interrupt defines  <-- end*/



/* -->  ETOS memory defines  --> start*/

/* <--  ETOS memory defines  <-- end*/



/* -->  ETOS message queue defines  --> start*/

/* <--  ETOS message queue defines  <-- end*/



/* -->  ETOS hw op defines  --> start*/

/* <--  ETOS hw op defines  <-- end*/



/* -->  ETOS random defines  --> start*/

#define ETOS_RANDOM_USE_MORE_MEMORY               (0)
/*more than 2k memory is necessary for Mersenne Twist Pseudorandom Number Generator*/

/* <--  ETOS random defines  <-- end*/



/* -->  ETOS common defines  --> start*/

#define ETOS_FORCE_ASSERT                         (1)   //enable it will cause hw operation slower

#define ETOS_DISABLE_INTERRUPT_AFTER_PANIC        (1)   //panic then disable all interrupt

#define ETOS_DEBUG                                (1)

/* <--  ETOS common defines  <-- end*/



/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

#endif  /* __ETOS_CFG_H__ */

/* EOF */

