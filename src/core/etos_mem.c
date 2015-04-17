/******************************************************************************
File    :  etos_mem.c

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		ETOS memory  management

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-19     deeve        Create
2015-4-14      deeve        Add some comments

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/
#define MEM_CHECK_FLAG     (0xdeadbeef)

/*内存池定义区*/

typedef struct _mem_block_header {
    list_t  list;
    u32 total_block_num;
    u32 max_used_blk_num;    /* 统计使用 */
    u32 free_block_num;
} mem_block_header_t;

typedef struct _mem_block {
    u32 check_flag;
    list_t  list;
    u32 block_id;  /* blk_headers[]的下标，释放的时候用它来将该block归还 */
    u32 user_len;
    u8 user_data[0];
} mem_block_t;


typedef struct _mem_pool_header {
    u32 min_block_size;
    u32 max_block_size;
    u32 valid_items;
    u32 base_id_of_blk_size;    /* 2**base_id_of_blk_size = min_block_size */
    mem_block_header_t blk_headers[0];
} mem_pool_header_t;


/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

static mem_pool_header_t *_os_pt_mem_pool_head;




#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n

static const char _os_log_table[256] = {
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

static inline BOOL _etos_is_power_of_2(u32 n)
{
    return n && ((n & (n - 1)) == 0);
}


static inline u32 _etos_aligned_to_power_of_2(u32 n)
{
    if (_etos_is_power_of_2(n)) {
        return n;
    } else {
        n--;
        n |= (n >> 1);
        n |= (n >> 2);
        n |= (n >> 4);
        n |= (n >> 8);
        n |= (n >> 16);
        n++;
        return n;
    }
}

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/

/* assume the return value is x, then 2^x = n */
u32 etos_log_base_2(u32 n)
{
    u32 ret;
    register u32 t1, t2; //temporaries

    if ((t2 = (n >> 16))) {
        ret = (t1 = (t2 >> 8)) ? (24 + _os_log_table[t1]) : (16 + _os_log_table[t2]);
    } else {
        ret = (t1 = (n >> 8)) ? (8 + _os_log_table[t1]) : _os_log_table[n];
    }
    return ret;
}



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
                       u8 **mem_pool_end)
{
    u32 i, j;
    u32 blk_size, max_blk_size, blk_num, min_blk_size = 0, last_size = 0;
    mem_block_header_t *pt_blk_header;
    mem_block_t *pt_block, *pt_block_first, *pt_block_next = NULL;
    list_t *pt_blk_head_list;
    u32 valid_items = 0;

    /* check input parameter */
    if ((mem_start == NULL) || (pool_items == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    for (i = 0; (blk_size = pool_items[i].block_size); i++) {
        if (_etos_is_power_of_2(blk_size)) { /* 2的幂 */
            if (i == 0) {
                if (pool_items[0].block_num == 0) { /* 第一项的block number 不能为0，其后的项是可以的 */
                    return ETOS_INVALID_PARAM;
                }
                last_size = blk_size;
                min_blk_size = blk_size;
            } else {
                /* end flag */
                if ((blk_size == 0) && (pool_items[i].block_num == 0)) {
                    break;
                }
                if (blk_size != (last_size * 2)) { /* 检查是否依次是2的幂 */
                    return ETOS_INVALID_PARAM;
                } else {
                    last_size = blk_size;  /* 不检查block_num主要是为了让后面的block header连续*/
                }
            }
            valid_items++;
        } else {
            return ETOS_INVALID_PARAM;
        }
    }

    max_blk_size = last_size;

    _os_pt_mem_pool_head = (mem_pool_header_t *) n_bits_align_up(mem_start, 4);
    _os_pt_mem_pool_head->min_block_size = min_blk_size;
    _os_pt_mem_pool_head->max_block_size = max_blk_size;
    _os_pt_mem_pool_head->valid_items = valid_items;

#if 1
    _os_pt_mem_pool_head->base_id_of_blk_size = etos_log_base_2(min_blk_size);
#else
    _os_pt_mem_pool_head->base_id_of_blk_size = etos_count_consecutive_0_in_lsb(min_blk_size);
#endif

    /* init block headers */
    for (i = 0; i < valid_items; i++) {
        pt_blk_header = &_os_pt_mem_pool_head->blk_headers[i];
        INIT_LIST_HEAD(&pt_blk_header->list);
        pt_blk_header->total_block_num = pool_items[i].block_num;
        pt_blk_header->free_block_num = pool_items[i].block_num;
        pt_blk_header->max_used_blk_num = 0;
    }

    pt_block_first = (mem_block_t *) &_os_pt_mem_pool_head->blk_headers[valid_items];

    /* init block */
    for (i = 0, pt_block = pt_block_first; i < valid_items; i++) {

        pt_blk_head_list = &_os_pt_mem_pool_head->blk_headers[i].list;
        blk_num = pool_items[i].block_num;
        blk_size = pool_items[i].block_size;

        for (j = 0; j < blk_num; j++, pt_block = pt_block_next) {
            list_add_tail(&pt_block->list, pt_blk_head_list);
            pt_block->block_id = i;
            pt_block->user_len = 0;
            pt_block->check_flag = MEM_CHECK_FLAG;
            pt_block_next = (mem_block_t *)&pt_block->user_data[blk_size];
        }
    }

    if (mem_pool_end) {
        *mem_pool_end = (u8 *)pt_block_next; /* 该地址是没有被占用的起始地址 */
    }

    return ETOS_RET_OK;
}



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
s32 etos_mem_destroy(void)
{
    if (_os_pt_mem_pool_head == NULL) {
        return ETOS_NOT_INIT;
    }

    _os_pt_mem_pool_head = NULL;

    return ETOS_RET_OK;
}


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
s32 etos_mem_pool_add(u8 *mem_start, u8 *mem_end, u32 blk_size)
{
    u32 block_id;
    u32 total_size_add, total_blks_add;
    register mem_pool_header_t *pt_mem_pool_head;
    mem_block_t *pt_block, *pt_block_first;
    list_t *pt_blk_head_list;

    etos_init_critical();

    pt_mem_pool_head = _os_pt_mem_pool_head;

    /* check input parameter */
    if ((mem_start == NULL) || (mem_end == NULL)) {
        return ETOS_INVALID_PARAM;
    }

    if (pt_mem_pool_head == NULL) {
        return ETOS_NOT_INIT;
    }

    if (_etos_is_power_of_2(blk_size)) { /* 2的幂 */
        if ((blk_size < pt_mem_pool_head->min_block_size)
            || (blk_size > pt_mem_pool_head->max_block_size)) {
            return ETOS_INVALID_PARAM;
        }
    } else {
        return ETOS_INVALID_PARAM;
    }


    mem_end = (u8 *) n_bits_align_down(mem_end, 4);
    mem_start = (u8 *) n_bits_align_up(mem_start, 4);

    total_size_add = mem_end - mem_start;
    total_blks_add = total_size_add / (blk_size + sizeof(mem_block_t));
    if (total_blks_add == 0) {
        return ETOS_INVALID_PARAM;
    }

    block_id = etos_log_base_2(blk_size);
    block_id -= pt_mem_pool_head->base_id_of_blk_size;

    /* init first block */
    pt_block_first = (mem_block_t *)mem_start;
    pt_block_first->check_flag = MEM_CHECK_FLAG;
    INIT_LIST_HEAD(&pt_block_first->list);
    pt_block_first->block_id = block_id;
    pt_block_first->user_len = 0;

    pt_blk_head_list = &pt_block_first->list;
    total_blks_add--;

    pt_block = pt_block_first;

    /* init block */
    while (total_blks_add > 0) {
        pt_block = (mem_block_t *)&pt_block->user_data[blk_size];

        pt_block->check_flag = MEM_CHECK_FLAG;
        list_add_tail(&pt_block->list, pt_blk_head_list);
        pt_block->block_id = block_id;
        pt_block->user_len = 0;

        total_blks_add--;
    }

    /* add to corresponding block list tail*/
    etos_enter_critical();
    list_splice(pt_blk_head_list, pt_mem_pool_head->blk_headers[block_id].list.prev);
    etos_exit_critical();

    return ETOS_RET_OK;
}


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
void *etos_mem_malloc(u32 len)
{
    mem_block_t *pt_mem_blk;
    mem_block_header_t *pt_mem_blk_header;
    list_t *entry;
    u32 used_blk_num;
    u32 block_id, valid_items, search_count;
    u32 aligned_len, usr_len;
    etos_init_critical();

    if (_os_pt_mem_pool_head == NULL || (len == 0)) {
        return NULL;
    }

    usr_len = len;
    aligned_len = _etos_aligned_to_power_of_2(len);
    search_count = 1;

    valid_items = _os_pt_mem_pool_head->valid_items;
    block_id = etos_log_base_2(aligned_len);
    block_id -= _os_pt_mem_pool_head->base_id_of_blk_size;
    pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[block_id];

    if (block_id >= valid_items) {
        return NULL;
    }

    do {
        if (pt_mem_blk_header->free_block_num > 0) {

            etos_enter_critical();

            /* remove block from list*/
            entry = list_dequeue(&pt_mem_blk_header->list);
            pt_mem_blk = list_entry(entry, mem_block_t, list);

            pt_mem_blk_header->free_block_num--;

            etos_exit_critical();


            /* check it is error or not */
            ASSERT(pt_mem_blk->block_id == block_id);

            pt_mem_blk->user_len = usr_len;

            used_blk_num = pt_mem_blk_header->total_block_num - pt_mem_blk_header->free_block_num;
            if (pt_mem_blk_header->max_used_blk_num < used_blk_num) {
                pt_mem_blk_header->max_used_blk_num = used_blk_num;
            }

            return (void *)(pt_mem_blk->user_data);
        } else {
            block_id++;
            search_count++;
            pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[block_id];
        }
    } while (block_id < valid_items);

    return NULL;
}



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
void *etos_mem_malloc_idic(u32 len)
{
    mem_block_t *pt_mem_blk;
    mem_block_header_t *pt_mem_blk_header;
    list_t *entry;
    u32 used_blk_num;
    u32 block_id, valid_items, search_count;
    u32 aligned_len, usr_len;

    if (_os_pt_mem_pool_head == NULL || (len == 0)) {
        return NULL;
    }

    usr_len = len;
    aligned_len = _etos_aligned_to_power_of_2(len);
    search_count = 1;

    valid_items = _os_pt_mem_pool_head->valid_items;
    block_id = etos_log_base_2(aligned_len);
    block_id -= _os_pt_mem_pool_head->base_id_of_blk_size;
    pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[block_id];

    if (block_id >= valid_items) {
        return NULL;
    }

    do {
        if (pt_mem_blk_header->free_block_num > 0) {

            /* remove block from list*/
            entry = list_dequeue(&pt_mem_blk_header->list);
            pt_mem_blk = list_entry(entry, mem_block_t, list);

            pt_mem_blk_header->free_block_num--;

            /* check it is error or not */
            ASSERT(pt_mem_blk->block_id == block_id);

            pt_mem_blk->user_len = usr_len;

            used_blk_num = pt_mem_blk_header->total_block_num - pt_mem_blk_header->free_block_num;
            if (pt_mem_blk_header->max_used_blk_num < used_blk_num) {
                pt_mem_blk_header->max_used_blk_num = used_blk_num;
            }

            return (void *)(pt_mem_blk->user_data);
        } else {
            block_id++;
            search_count++;
            pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[block_id];
        }
    } while (block_id < valid_items);

    return NULL;
}




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
s32 etos_mem_free(void *ptr)
{
    mem_block_t *pt_mem_blk;
    mem_block_header_t *pt_mem_blk_header;

    etos_init_critical();

    if (ptr == NULL) {
        return ETOS_INVALID_PARAM;
    }

    pt_mem_blk = list_entry(ptr, mem_block_t, user_data);

    ASSERT(pt_mem_blk->check_flag == MEM_CHECK_FLAG);

    pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[pt_mem_blk->block_id];

    etos_enter_critical();
    list_add_tail(&pt_mem_blk->list, &pt_mem_blk_header->list);
    pt_mem_blk_header->free_block_num++;
    etos_exit_critical();

    return ETOS_RET_OK;
}



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
s32 etos_mem_free_idic(void *ptr)
{
    mem_block_t *pt_mem_blk;
    mem_block_header_t *pt_mem_blk_header;

    if (ptr == NULL) {
        return ETOS_INVALID_PARAM;
    }

    pt_mem_blk = list_entry(ptr, mem_block_t, user_data);

    ASSERT(pt_mem_blk->check_flag == MEM_CHECK_FLAG);

    pt_mem_blk_header = &_os_pt_mem_pool_head->blk_headers[pt_mem_blk->block_id];

    list_add_tail(&pt_mem_blk->list, &pt_mem_blk_header->list);
    pt_mem_blk_header->free_block_num++;

    return ETOS_RET_OK;
}


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
s32 etos_mem_report(const char *prompt)
{
    u32 i, valid_items, block_size;
    mem_block_header_t *pt_blk_header;
    const char *header;

    if (prompt) {
        header = prompt;
    } else {
        header = "etos mem";
    }

    valid_items = _os_pt_mem_pool_head->valid_items;
    block_size = _os_pt_mem_pool_head->min_block_size;

    for (i = 0; i < valid_items; i++) {
        pt_blk_header = &_os_pt_mem_pool_head->blk_headers[i];
        xlogt(LOG_MODULE_ETOS, "%s: (blk size=%4d) total: num:%d used num:%d, max_used:%d\r\n",
              header, block_size, pt_blk_header->total_block_num,
              pt_blk_header->total_block_num - pt_blk_header->free_block_num,
              pt_blk_header->max_used_blk_num);

        block_size *= 2;
    }

    return ETOS_RET_OK;
}



/* EOF */

