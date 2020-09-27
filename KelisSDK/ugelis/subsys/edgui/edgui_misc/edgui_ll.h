/**
 * @file edgui_ll.c
 * Handle linked lists. The nodes are dynamically allocated by the 'edgui_mem' module.
 */

#ifndef EDGUI_LL_H
#define EDGUI_LL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "edgui_mem.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Dummy type to make handling easier*/
typedef uint8_t edgui_ll_node_t;

/** Description of a linked list*/
typedef struct
{
    uint32_t n_size;
    edgui_ll_node_t * head;
    edgui_ll_node_t * tail;
} edgui_ll_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
 * @param node_size the size of 1 node in bytes
 */
void edgui_ll_init(edgui_ll_t * ll_p, uint32_t node_size);

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * edgui_ll_ins_head(edgui_ll_t * ll_p);

/**
 * Insert a new node in front of the n_act node
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the new head
 */
void * edgui_ll_ins_prev(edgui_ll_t * ll_p, void * n_act);

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * edgui_ll_ins_tail(edgui_ll_t * ll_p);

/**
 * Remove the node 'node_p' from 'll_p' linked list.
 * It does not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void edgui_ll_rem(edgui_ll_t * ll_p, void * node_p);

/**
 * Remove and free all elements from a linked list. The list remain valid but become empty.
 * @param ll_p pointer to linked list
 */
void edgui_ll_clear(edgui_ll_t * ll_p);

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 * @param head true: be the head in the new list
 *             false be the head in the new list
 */
void edgui_ll_chg_list(edgui_ll_t * ll_ori_p, edgui_ll_t * ll_new_p, void * node, bool head);

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * edgui_ll_get_head(const edgui_ll_t * ll_p);

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * edgui_ll_get_tail(const edgui_ll_t * ll_p);

/**
 * Return with the pointer of the next node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the next node
 */
void * edgui_ll_get_next(const edgui_ll_t * ll_p, const void * n_act);

/**
 * Return with the pointer of the previous node after 'n_act'
 * @param ll_p pointer to linked list
 * @param n_act pointer a node
 * @return pointer to the previous node
 */
void * edgui_ll_get_prev(const edgui_ll_t * ll_p, const void * n_act);

/**
 * Return the length of the linked list.
 * @param ll_p pointer to linked list
 * @return length of the linked list
 */
uint32_t edgui_ll_get_len(const edgui_ll_t * ll_p);

/**
 * Move a nodw before an other node in the same linked list
 * @param ll_p pointer to a linked list
 * @param n_act pointer to node to move
 * @param n_after pointer to a node which should be after `n_act`
 */
void edgui_ll_move_before(edgui_ll_t * ll_p, void * n_act, void * n_after);

/**
 * Check if a linked list is empty
 * @param ll_p pointer to a linked list
 * @return true: the linked list is empty; false: not empty
 */
bool edgui_ll_is_empty(edgui_ll_t * ll_p);
/**********************
 *      MACROS
 **********************/

#define EDGUI_LL_READ(list, i) for(i = edgui_ll_get_head(&list); i != NULL; i = edgui_ll_get_next(&list, i))

#define EDGUI_LL_READ_BACK(list, i) for(i = edgui_ll_get_tail(&list); i != NULL; i = edgui_ll_get_prev(&list, i))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
