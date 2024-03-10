/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_AVL_H
#define	_AVL_H

/*
 * This is a private header file.  Applications should not directly include
 * this file.
 */

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include "avl_impl.h"

/*
 * This is a generic implemenatation of AVL trees for use in the Solaris kernel.
 * The interfaces provide an efficient way of implementing an ordered set of
 * data structures.
 *
 * AVL trees provide an alternative to using an ordered linked list. Using AVL
 * trees will usually be faster, however they requires more storage. An ordered
 * linked list in general requires 2 pointers in each data structure. The
 * AVL tree implementation uses 3 pointers. The following chart gives the
 * approximate performance of operations with the different approaches:
 *
 *	Operation	 Link List	AVL tree
 *	---------	 --------	--------
 *	lookup		   O(n)		O(log(n))
 *
 *	insert 1 node	 constant	constant
 *
 *	delete 1 node	 constant	between constant and O(log(n))
 *
 *	delete all nodes   O(n)		O(n)
 *
 *	visit the next
 *	or prev node	 constant	between constant and O(log(n))
 *
 *
 * The data structure nodes are anchored at an "avl_tree_t" (the equivalent
 * of a list header) and the individual nodes will have a field of
 * type "avl_node_t" (corresponding to list pointers).
 *
 * The type "avl_index_t" is used to indicate a position in the list for
 * certain calls.
 *
 * The usage scenario is generally:
 *
 * 1. Create the list/tree with: avl_create()
 *
 * followed by any mixture of:
 *
 * 2a. Insert nodes with: avl_add(), or avl_find() and avl_insert()
 *
 * 2b. Visited elements with:
 *	 avl_first() - returns the lowest valued node
 *	 avl_last() - returns the highest valued node
 *	 AVL_NEXT() - given a node go to next higher one
 *	 AVL_PREV() - given a node go to previous lower one
 *
 * 2c.	Find the node with the closest value either less than or greater
 *	than a given value with avl_nearest().
 *
 * 2d. Remove individual nodes from the list/tree with avl_remove().
 *
 * and finally when the list is being destroyed
 *
 * 3. Use avl_destroy_nodes() to quickly process/free up any remaining nodes.
 *    Note that once you use avl_destroy_nodes(), you can no longer
 *    use any routine except avl_destroy_nodes() and avl_destoy().
 *
 * 4. Use avl_destroy() to destroy the AVL tree itself.
 *
 * Any locking for multiple thread access is up to the user to provide, just
 * as is needed for any linked list implementation.
 */


/*
 * Type used for the root of the AVL tree.
 */
typedef struct avl_tree avl_tree_t;

/*
 * The data nodes in the AVL tree must have a field of this type.
 */
typedef struct avl_node avl_node_t;

/*
 * An opaque type used to locate a position in the tree where a node
 * would be inserted.
 */
typedef struct {
	struct avl_node *node;
	int bit;
} avl_index_t;


/*
 * Direction constants used for avl_nearest().
 */
#define	AVL_BEFORE	(0)
#define	AVL_AFTER	(1)


/*
 * Prototypes
 *
 * Where not otherwise mentioned, "void *" arguments are a pointer to the
 * user data structure which must contain a field of type avl_node_t.
 *
 * Also assume the user data structures looks like:
 *	struct my_type {
 *		...
 *		avl_node_t	my_link;
 *		...
 *	};
 */

/*
 * Initialize an AVL tree. Arguments are:
 *
 * tree	  - the tree to be initialized
 * compar - function to compare two nodes, it must return exactly: -1, 0, or +1
 *	    -1 for <, 0 for ==, and +1 for >
 * size	  - the value of sizeof(struct my_type)
 * offset - the value of OFFSETOF(struct my_type, my_link)
 */
extern void avl_create(avl_tree_t *tree);

/*
 * Find a node with a matching value in the tree. Returns the matching node
 * found. If not found, it returns NULL and then if "where" is not NULL it sets
 * "where" for use with avl_insert() or avl_nearest().
 *
 * node	  - node that has the value being looked for
 * where  - position for use with avl_nearest() or avl_insert(), may be NULL
 */
extern struct avl_node *avl_find(avl_tree_t *tree, const struct avl_node *node, avl_index_t *where);

/*
 * Return the next or previous valued node in the tree.
 * AVL_NEXT() will return NULL if at the last node.
 * AVL_PREV() will return NULL if at the first node.
 *
 * node	  - the node from which the next or previous node is found
 */
#define	AVL_NEXT(tree, node)	avl_walk(tree, node, AVL_AFTER)
#define	AVL_PREV(tree, node)	avl_walk(tree, node, AVL_BEFORE)

/*
 * Add a single node to the tree.
 * The node must not be in the tree, and it must not
 * compare equal to any other node already in the tree.
 *
 * node	  - the node to add
 */
extern void avl_add(avl_tree_t *tree, struct avl_node *node, size_t key);


/*
 * Remove a single node from the tree.	The node must be in the tree.
 *
 * node	  - the node to remove
 */
extern void avl_remove(avl_tree_t *tree, struct avl_node *node);

/*
 * Return B_TRUE if there are zero nodes in the tree, B_FALSE otherwise.
 */
extern bool avl_is_empty(avl_tree_t *tree);

struct avl_node *avl_find_best(struct avl_tree *tree, size_t key);
void avl_walk(const struct avl_tree *tree,
    void (*func)(const struct avl_node *, bool));


#ifdef	__cplusplus
}
#endif

#endif	/* _AVL_H */
