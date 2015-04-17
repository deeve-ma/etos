/******************************************************************************
File    :  etos_mem.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os memory  management

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-19     deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/
#ifndef __ETOS_MEM_H__
#define __ETOS_MEM_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_listop.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

#define malloc(len)    etos_mem_malloc(len)
#define free(ptr)      etos_mem_free(ptr)


#define n_bits_align_up(x, n)    ((((u32)(x))+(n-1)) & ((u32)(~(n-1))))
#define n_bits_align_down(x, n)  (((u32)(x)) & ((u32)(~(n-1))))


typedef struct _mem_pool_item {
    u32 block_size;  //内存池中block的大小
    u32 block_num;   //内存池中block的数量
} mem_pool_item_t;


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

/* assume the return value is x, then 2^x = n */
u32 etos_log_base_2(u32 n);


/**
 * init memory pool.
 * init a mempory pool, need called before any other memory api
 *
 * @param[in]    mem_start    the start address of managed memory
 * @param[in]    mem_len      the length of memory
 *
 * @return s32
 * @retval 0                success
 * @retval other value      fail
 *
 * @authors    deeve
 * @date       2013/10/19
 */
s32 etos_mem_pool_init(u8 *mem_start, mem_pool_item_t pool_items[],
                       u8 **mem_pool_end);


/**
 * destroy memory pool.
 * destory memory pool, you can call it when you do not need memory management any more
 *
 * @param[in]    void
 *
 * @return s32
 * @retval 0                success
 * @retval other value      fail
 *
 * @note none
 * @authors    deeve
 * @date       2013/10/19
 */
s32 etos_mem_destroy(void);



/**
 * add memory pool.
 * add memory pool when three is extra free memory
 *
 * @param[in]    mem_start  the start address of add memory
 * @param[in]    mem_end    the end address of add memory
 * @param[in]    blk_size   the block size which will be add to
 *
 * @return
 * @retval 0       success
 * @retval other   fail
 *
 * @note   if you want add different block size, you need call this API more than one times
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_mem_pool_add(u8 *mem_start, u8 *mem_end, u32 blk_size);



/**
 * malloc a memory from memory pool.
 * just malloc a memory, it usage is same as malloc()
 *
 * @param[in]    len   the length of memory
 *
 * @return
 * @retval 0(NULL)        fail
 * @retval other value    success
 *
 * @note none
 * @see        etos_mem_free()
 * @authors    deeve
 * @date       2013/10/19
 */
void *etos_mem_malloc(u32 len);



/**
 * malloc a memory from memory pool.
 * just malloc a memory, it is called in a disable interrrupt context
 *
 * @param[in]    len   the length of memory
 *
 * @return
 * @retval 0(NULL)        fail
 * @retval other value    success
 *
 * @note none
 * @see        etos_mem_malloc()
 * @authors    deeve
 * @date       2013/10/19
 */
void *etos_mem_malloc_idic(u32 len);



/**
 * free the memory of ptr.
 * return the memory to the memory pool,the ptr is the return value of etos_mem_malloc()
 *
 * @param[in]    ptr    the memory poniter which need to be free
 *
 * @return s32
 * @retval 0                success
 * @retval other value      fail
 *
 * @note none
 * @see        etos_mem_malloc()
 * @authors    deeve
 * @date       2013/10/19
 */
s32 etos_mem_free(void *ptr);



/**
 * free the memory of ptr.
 * return the memory to the memory pool,the ptr is the return value of etos_mem_malloc()
 * it is called in a disable interrrupt context
 *
 * @param[in]    ptr    the memory poniter which need to be free
 *
 * @return s32
 * @retval 0                success
 * @retval other value      fail
 *
 * @note none
 * @see        etos_mem_free()
 * @authors    deeve
 * @date       2013/10/19
 */
s32 etos_mem_free_idic(void *ptr);



/**
 * report memory statistics.
 * print memory statistics for monitor memory usage
 *
 * @param[in]    prompt   print header
 *
 * @return 0
 *
 * @authors    deeve
 * @date       2015/4/14
 */
s32 etos_mem_report(const char *prompt);


#endif  /* __ETOS_MEM_H__ */

/* EOF */

