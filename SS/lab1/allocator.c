#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "allocator.h"
#include "block.h"
#include "config.h"
#include "allocator_impl.h"
#include "kernel.h"

#define ARENA_SIZE (ALLOCATOR_ARENA_PAGES * ALLOCATOR_PAGE_SIZE)
#define BLOCK_SIZE_MAX (ARENA_SIZE - BLOCK_STRUCT_SIZE)

static tree_type blocks_tree = TREE_INITIALIZER;

static Block* arena_alloc(size_t size) {
    Block *block;

    if (size > BLOCK_SIZE_MAX) {
        block = kernel_alloc(size);
        if (block != NULL) {
            arena_init(block, size - BLOCK_STRUCT_SIZE);
        }
    } else {
        block = kernel_alloc(ARENA_SIZE);
        if (block != NULL) {
            arena_init(block, ARENA_SIZE - BLOCK_STRUCT_SIZE);
        }
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

    if (size > BLOCK_SIZE_MAX) {
        if (size > SIZE_MAX - (ALIGN - 1)) {
            // переповнення
            return NULL;
        }
        size_t arena_size = (ROUND_BYTES(size) / ALLOCATOR_PAGE_SIZE) * ALLOCATOR_PAGE_SIZE + BLOCK_STRUCT_SIZE;
        block = arena_alloc(arena_size);
        return block_to_payload(block);
    }

    if (size < BLOCK_SIZE_MIN) {
        size = BLOCK_SIZE_MIN; //64
    }
    size_t aligned_size = ROUND_BYTES(size);
    node = tree_find_best(&blocks_tree, aligned_size);
    if (node == NULL) {
        block = arena_alloc(aligned_size);
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
    block_clr_flag_busy(block);

    if (block_get_size_curr(block) > BLOCK_SIZE_MAX) {
        kernel_free(block, block_get_size_curr(block) + BLOCK_STRUCT_SIZE);
    } else {

        if (!block_get_flag_last(block)) {
            block_r = block_next(block);
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
            block_dontneed(block);
            tree_add_block(block);
        }
    }
}

void* mem_realloc(void* ptr1, size_t size) {
    void *ptr2;
    Block* block1, *block2, *block_r, *block_n;
    size_t size_curr;

    if (size < BLOCK_SIZE_MIN) {
        size = BLOCK_SIZE_MIN;
    }

    size = ROUND_BYTES(size);

    if (ptr1 == NULL) {
        return mem_alloc(size);
    }
    block1 = payload_to_block(ptr1);
    size_curr = block_get_size_curr(block1);

    if (size_curr > BLOCK_SIZE_MAX) {
        if (size == size_curr) {
            return ptr1;
        }

        goto move_large_block;
    }

    if (size == size_curr) {
        return ptr1;
    }

    //зменшуємо
    if (size < size_curr) {
        if (!block_get_flag_last(block1)) {
            block_r = block_split(block1, size);
            if (block_r != NULL) {
                block_n = block_next(block_r);
                if (!block_get_flag_busy(block_n)) {
                    tree_remove_block(block_n);
                    block_merge(block_r, block_n);
                }

                tree_add_block(block_r);
                return block_to_payload(block1);
            }
        }
    }

    //збільшуємо
    if (size > size_curr) {
        if (!block_get_flag_last(block1)) {
            block_r = block_next(block1);
            if (!block_get_flag_busy(block_r)) {
                size_t total_size = size_curr + block_get_size_curr(block_r) + BLOCK_STRUCT_SIZE;
                if (total_size >= size) {
                    tree_remove_block(block_r);
                    block_merge(block1, block_r);
                    block_n = block_split(block1, size);
                    if (block_n != NULL) {
                        tree_add_block(block_n);
                    }
                    return block_to_payload(block1);
                }
            }
        }
    }

move_large_block:
    ptr2 = mem_alloc(size);
    if (ptr2 != NULL) {
        memcpy(ptr2, ptr1, size_curr < size ? size_curr : size);
        mem_free(ptr1);
    }
    return ptr2;
}
