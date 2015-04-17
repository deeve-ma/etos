/******************************************************************************
File    :  etos_listop.c

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

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions Declaring                  *
 ******************************************************************************/
static  void _check_head(struct list_head *head);
static  void _list_add(struct list_head *new_node,
                       struct list_head *prev,
                       struct list_head *next);
static  void _list_del(struct list_head *prev,
                       struct list_head *next);

/******************************************************************************
 *                                 Global Functions                           *
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
void list_add(struct list_head *new_node, struct list_head *head)
{
    _check_head(head);
    _list_add(new_node, head, head->next);
}


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
void list_add_tail(struct list_head *new_node, struct list_head *head)
{
    _check_head(head);
    _list_add(new_node, head->prev, head);
}


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
void list_del(struct list_head *entry)
{
    _list_del(entry->prev, entry->next);
}


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
void list_del_init(struct list_head *entry)
{
    _list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}


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
void list_move(struct list_head *list, struct list_head *head)
{
    _check_head(head);
    _list_del(list->prev, list->next);
    list_add(list, head);
}


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
void list_move_tail(struct list_head *list, struct list_head *head)
{
    _check_head(head);
    _list_del(list->prev, list->next);
    list_add_tail(list, head);
}


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
void list_splice(struct list_head *list, struct list_head *head)
{
    struct list_head *first = list;
    struct list_head *last  = list->prev;
    struct list_head *at    = head->next;

    first->prev = head;
    head->next  = first;

    last->next = at;
    at->prev   = last;
}


/**
 * list_is_empty - tests whether a list is empty.
 *
 * @param[in]    head    the list to test
 *
 * @return bool
 * @retval true    when the list is empty
 * @retval false   when the list is NOT empty
 *
 * @note none
 * @see
 * @authors    deeve
 * @date       2013/10/19
 */
BOOL list_is_empty(struct list_head *head)
{
    return (head->next == head);
}


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
struct list_head *list_dequeue(struct list_head *list )
{
    struct list_head *next, *prev, *result = ((void *)0);

    prev = list;
    next = prev->next;

    if ( next != prev ) {
        result = next;
        next = next->next;
        next->prev = prev;
        prev->next = next;
        result->prev = result->next = result;
    }

    return result;
}


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
void list_replace(struct list_head *node, struct list_head *new_node)
{
    struct list_head *next, *prev;

    if (node && new_node) {
        prev = node->prev;
        next = node->next;

        _list_add(new_node, prev, next);
    }
}


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
void list_replace_init(struct list_head *node, struct list_head *new_node)
{
    struct list_head *next, *prev;

    if (node && new_node) {
        prev = node->prev;
        next = node->next;

        _list_add(new_node, prev, next);

        INIT_LIST_HEAD(node);
    }
}


/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/
static  void _check_head(struct list_head *head)
{
    if ((head->next == 0) && (head->prev == 0)) {
        INIT_LIST_HEAD(head);
    }
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static  void _list_add(struct list_head *new_node,
                       struct list_head *prev,
                       struct list_head *next)
{

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;

}


/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static  void _list_del(struct list_head *prev,
                       struct list_head *next)
{

    next->prev = prev;
    prev->next = next;
}

/* EOF */

