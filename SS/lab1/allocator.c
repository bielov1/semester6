#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "allocator.h"
#include "block.h"
#include "config.h"
#include "allocator_impl.h"
#include "kernel.h"

#define ARENA_SIZE (ALLOCATOR_ARENA_PAGES * ALLOCATOR_PAGE_SIZE)
#define BLOCK_SIZE_MAX (ARENA_SIZE - BLOCK_STRUCT_SIZE)

static tree_type blocks_tree = TREE_INITIALIZER;

static Block* arena_alloc(void) {
    Block *block;
    block = kernel_alloc(ARENA_SIZE);
    if (block != NULL) {
        arena_init(block, ARENA_SIZE - BLOCK_STRUCT_SIZE);
    }
    return block;
}

static void tree_add_block(Block* block) {
    assert(block_get_flag_busy(block) == false);
    tree_add(&blocks_tree, block_to_node(block), block_get_size_curr(block));
}

static void tree_remove_block(Block* block) {
    assert(block_get_flag_busy(block) == false);
    tree_remove(&blocks_tree, block_to_node(block));

}

void* mem_alloc(size_t size) {
    Block *block, *block_r;
    tree_node_type *node;

    if (size > BLOCK_SIZE_MAX) return NULL;

    if (size < BLOCK_SIZE_MIN) {
        size = BLOCK_SIZE_MIN; //64
    }

    size_t aligned_size = ROUND_BYTES(size);

    node = tree_find_best(&blocks_tree, aligned_size);
    if (node == NULL) {
        block = arena_alloc();
        if (block == NULL) {
            return NULL;
        }

    } else {
        tree_remove(&blocks_tree, node);
        block = node_to_block(node);
    }

    block_r = block_split(block, aligned_size);
    if (block_r != NULL) {
        tree_add_block(block_r);
    }

    return block_to_payload(block);
}

static void show_node(const tree_node_type *node, const bool linked) {
    Block* block = node_to_block(node);
    printf("[%20p] %10zu %10zu %s %s %s %s\n", (void*)block,
    block_get_size_curr(block),
    block_get_size_prev(block),
    block_get_flag_busy(block) ? "busy" : "free",
    block_get_flag_first(block) ? "first" : "",
    block_get_flag_last(block) ? "last" : "",
    linked ? "linked" : "");
}

void mem_show(const char *msg) {
    printf("%s:\n", msg);
    if (tree_is_empty(&blocks_tree)) {
        printf("Tree is empty\n");
    } else {
        tree_walk(&blocks_tree, show_node);
    }
}

void mem_free(void *ptr) {
    Block *block, *block_r, *block_l;

    if (ptr == NULL) {
        return;
    }

    block = payload_to_block(ptr);
    if (block == NULL) {
        return;
    }
    block_clr_flag_busy(block);

    if (!block_get_flag_last(block)) {
        block_r = block_next(block);
        if (block_r == NULL) {
            return;
        }
        if (!block_get_flag_busy(block_r)) {
            tree_remove_block(block_r);
            block_merge(block, block_r);
        }
    }
    if (!block_get_flag_first(block)) {
        block_l = block_prev(block);
        if (!block_get_flag_busy(block_l)) {
            tree_remove_block(block_l);
            block_merge(block_l, block);
            block = block_l;
        }
    }

    if (block_get_flag_first(block) && block_get_flag_last(block)) {
        kernel_free(block, ARENA_SIZE);
    } else {
        // TODO check possible memory reset
        // [@@@@                                     ]
        //      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        //         |         |         |         |   =======>
        //         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //         ^ ptr ------------------------>
        //                        size
        //              kernel_reset(ptr, size)
        tree_add_block(block);
    }
}

void* mem_realloc(void* ptr, size_t size) {
    (void)ptr;
    (void)size;
    return NULL;
/*    if (ptr == NULL) {
        return NULL;
    }

    if (size == 0) {
        mem_free(ptr);
        return NULL;
    }

    Block *block, *block_r, *block_rr, *block_rr_payload;

    if (size > BLOCK_SIZE_MAX) return NULL;

    // if we passed not aligned size, then block_next() in mem_show()
    // couldn't switch to next block correctly
    // BLOCK_STRUCT_SIZE = 16
    size_t aligned_size = ROUND_BYTES(size);

    block = payload_to_block(ptr);
    // Зменшуємо блок in-place
    if (get_size_curr(block) > aligned_size) {
        block_split(block, aligned_size);
        block_r = block_next(block);
        block_rr = block_next(block_r);
        set_size_prev(block_rr, get_size_curr(block_r));
        return block_to_payload(block);
    }
    // Збільшуємо блок in-place
    if (get_size_curr(block) < aligned_size) {
        block_r = block_next(block);
        if (!get_flag_busy(block_r)) {
            size_t total_size = get_size_curr(block) + get_size_curr(block_r);
            if (total_size > aligned_size) {
                block_merge(block, block_r);
                block_split(block, aligned_size);
                return block_to_payload(block);
            }
        }

        block_rr = mem_alloc(aligned_size);
        if (block_rr == NULL) {
            return NULL;
        }
        memcpy(block_rr, block_to_payload(block), aligned_size);
        mem_free(block_to_payload(block));

        return block_rr;
    }

    return ptr;
*/
}
