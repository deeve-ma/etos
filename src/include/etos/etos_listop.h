/******************************************************************************
File    :  etos_listop.h

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		exact time os list operation (like linux kernel list)

History:

Date           Author       Notes
----------     -------      -------------------------
2013-10-19     deeve        Create

*******************************************************************************/
#ifndef __ETOS_LISTOP_H__
#define __ETOS_LISTOP_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_types.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

#define OFFSETOF(type, member) ((u32) &((type *)0)->member)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define CONTAINER_OF(ptr, type, member) ({           \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - OFFSETOF(type, member) ); })


struct list_head {
    struct list_head *next, *prev;
};

typedef struct list_head list_t;


#define LIST_HEAD_INIT(name) { &(name), &(name) }


/*just for declare a variable*/
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)


/*reset ptr's prev/next point to itself*/
#define INIT_LIST_HEAD(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    } while (0)

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

/**
 * list_add - add a new entry.
 * Insert a new_node entry after the specified head
 * This is good for implementing stacks
 *
 * @param[in]    new_node    entry to be added
 * @param[in]    head        list head to add it after
 *
 * @return none
 *
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
void list_add(struct list_head *new_node, struct list_head *head);



/**
 * list_add_tail - add a new_node entry to the list tail.
 * Insert a new_node entry before the specified head
 * This is useful for implementing queues
 *
 * @param[in]    new_node    entry to be added
 * @param[in]    head        list head to add it before
 *
 * @return none
 *
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
void list_add_tail(struct list_head *new_node, struct list_head *head);



/**
 * list_del - deletes entry from list.
 * deletes entry from list, the entry still point it's prev/next node
 * but it's prev/next node do not point it again
 *
 * @param[in]    entry    the element to delete from the list
 *
 * @return none
 *
 * @note  list_is_empty on entry does not return true after this,
 *        the entry is in an undefined state
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
void list_del(struct list_head *entry);



/**
 * list_del_init - deletes entry from list and reinitialize it.
 * deletes entry from list, the entry point itslef
 *
 * @param[in]    entry    the element to delete from the list
 *
 * @return none
 *
 * @see  list_del
 * @authors    deeve
 * @date       2013/10/19
 */
void list_del_init(struct list_head *entry);



/**
 * list_move - delete from one list and add as another's head.
 *
 * @param[in]    list    the entry to move
 * @param[in]    head    the head that will precede our entry
 *
 * @return none
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
void list_move(struct list_head *list, struct list_head *head);



/**
 * list_move_tail - delete from one list and add as another's tail.
 *
 * @param[in]    list    the entry to move
 * @param[in]    head    the head that will follow our entry
 *
 * @return none
 *
 * @note none
 * @see  list_move
 * @authors    deeve
 * @date       2013/10/19
 */
void list_move_tail(struct list_head *list, struct list_head *head);



/**
 * list_splice - join two lists.
 * 将整个list添加到head的头
 *
 * @param[in]    list    the new_node list to add
 * @param[in]    head    the place to add it in the first lis
 *
 * @return none
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
void list_splice(struct list_head *list, struct list_head *head);



/**
 * list_is_empty - tests whether a list is empty.
 *
 * @param[in]    head    the list to test
 *
 * @return BOOL
 * @retval true    when the list is empty
 * @retval false   when the list is NOT empty
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
BOOL list_is_empty(struct list_head *head);



/**
 * list_dequeue - dequeue the head of the list if there are more than one entry.
 * 从head后取一个node出来
 *
 * @param[in]    list     the list to dequeue
 *
 * @return struct list_head *
 * @retval 0(NULL)       fail
 * @retval ohter val     success
 *
 * @note when the list is empty, it return NULL
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
struct list_head *list_dequeue( struct list_head *list );



/**
 * list_replace - replace node with new_node in a list.
 * 替换一个node
 *
 * @param[in]    node        old node
 * @param[in]    new_node    new node
 *
 * @return none
 *
 * @authors    deeve
 * @date       2013/10/19
 */
void list_replace(struct list_head *node, struct list_head *new_node);



/**
 * list_replace - replace node with new_node in a list, and reinitialize node.
 * 替换一个node,并且取消old node的链接关系
 *
 * @param[in]    node        old node
 * @param[in]    new_node    new node
 *
 * @return none
 *
 * @authors    deeve
 * @date       2013/10/19
 */
void list_replace_init(struct list_head *node, struct list_head *new_node);



/**************************************************************************/


/**
 * list_entry - get the struct for this entry.
 *
 * @ptr    the &struct list_head pointer.
 * @type   the type of the struct this is embedded in.
 * @member the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))




/**
 * list_for_each	-	iterate over a list.
 *
 * @pos    the &struct list_head to use as a loop counter.
 * @head   the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
         pos = pos->next)



/**
 * list_for_each_safe	-	iterate over a list safe against removal of list entry.
 *
 * @pos    the &struct list_head to use as a loop counter.
 * @n      another &struct list_head to use as temporary storage
 * @head   the head for your list.
 *
 * @see   list_for_each
 * @note  if you want to delte the node when scan the list, use this function
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)



/**
 * list_for_each_prev	-	iterate over a list in reverse order.
 *
 * @pos    the &struct list_head to use as a loop counter.
 * @head   the head for your list.
 *
 * @see   list_for_each
 * @note  scan from tail to head
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); \
         pos = pos->prev)



#define list_for_each_prev_safe(pos, p, head) \
    for (pos = (head)->prev, p = (pos)->prev; pos != (head); \
         pos = p, p = (pos)->prev)



/**
 * list_for_each_entry	-	iterate over list of given type.
 *
 * @pos    the type * to use as a loop counter.
 * @head   the head for your list.
 * @member the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
    for (pos = list_entry((head)->next, typeof(*pos), member);	\
         &pos->member != (head); 	\
         pos = list_entry(pos->member.next, typeof(*pos), member))



#endif  /* __ETOS_LISTOP_H__ */

/* EOF */

