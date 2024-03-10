/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_AVL_IMPL_H
#define	_AVL_IMPL_H

/*
 * This is a private header file.  Applications should not directly include
 * this file.
 */

//#include <sys/types.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef unsigned long ulong_t;

/*
 * generic AVL tree implementation for kernel use
 *
 * There are 5 pieces of information stored for each node in an AVL tree
 *
 *	pointer to less than child
 *	pointer to greater than child
 *	a pointer to the parent of this node
 *	an indication  [0/1]  of which child I am of my parent
 *	a "balance" (-1, 0, +1)	 indicating which child tree is taller
 *
 * Since they only need 3 bits, the last two fields are packed into the
 * bottom bits of the parent pointer on 64 bit machines to save on space.
 */

struct avl_node {
	struct avl_node *avl_child[2];	/* left/right children */
	struct avl_node *avl_parent;	/* this node's parent */
	unsigned short avl_child_index;	/* my index in parent's avl_child[] */
	short avl_balance;		/* balance value: -1, 0, +1 */
	size_t key;
	struct avl_node *next;
	struct avl_node *prev;
};

#define	AVL_XPARENT(n)		((n)->avl_parent)
#define	AVL_SETPARENT(n, p)	((n)->avl_parent = (p))

#define	AVL_XCHILD(n)		((n)->avl_child_index)
#define	AVL_SETCHILD(n, c)	((n)->avl_child_index = (unsigned short)(c))

#define	AVL_XBALANCE(n)		((n)->avl_balance)
#define	AVL_SETBALANCE(n, b)	((n)->avl_balance = (short)(b))

/*
 * macros used to create/access an avl_index_t
 */
#define	AVL_INDEX2NODE(x)	((x).node)
#define	AVL_INDEX2CHILD(x)	((x).bit)
#define	AVL_MKINDEX(n, c)	(avl_index_t){ .node = (n), .bit = (c) }


/*
 * The tree structure. The fields avl_root, avl_compar, and avl_offset come
 * first since they are needed for avl_find().	We want them to fit into
 * a single 64 byte cache line to make avl_find() as fast as possible.
 */
struct avl_tree {
	struct avl_node *avl_root;	/* root node in tree */
//	ulong_t avl_numnodes;		/* number of nodes in the tree */
};

#ifdef	__cplusplus
}
#endif

#endif	/* _AVL_IMPL_H */
