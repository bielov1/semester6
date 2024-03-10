#include <assert.h>

#include "block.h"
#include "config.h"
#include "kernel.h"

Block* block_split(Block *block, size_t size)
{
    Block* block_r;
    size_t size_rest;

    block_set_flag_busy(block);
    size_rest = block_get_size_curr(block) - size;
    if (size_rest >= BLOCK_STRUCT_SIZE + BLOCK_SIZE_MIN) {
        size_rest -= BLOCK_STRUCT_SIZE;
        block_set_size_curr(block, size);
        block_r = block_next(block);
        block_init(block_r);
        block_set_size_curr(block_r, size_rest);
        block_set_size_prev(block_r, size);
        block_set_offset(block_r, block_get_offset(block) + size + BLOCK_STRUCT_SIZE);
        if (block_get_flag_last(block)) {
            block_clr_flag_last(block);
            block_set_flag_last(block_r);
        } else {
            block_set_size_prev(block_next(block_r), size_rest);
        }
        return block_r;
    }
    return NULL;
}

void
block_merge(Block *block, Block *block_r)
{
    assert(block_get_flag_busy(block_r) == false);
    assert(block_next(block) == block_r);

    size_t size;

    size = block_get_size_curr(block) + block_get_size_curr(block_r) +
    BLOCK_STRUCT_SIZE;
    block_set_size_curr(block, size);

    if (block_get_flag_last(block_r)) {
        block_set_flag_last(block);
    }
    else {
        block_set_size_prev(block_next(block_r), size);
    }

}

#include <stdio.h>

void block_dontneed(Block* block) {
    size_t size_curr;
    size_t offset, offset1, offset2;

    size_curr = block_get_size_curr(block);
    if (size_curr - sizeof(tree_node_type) < ALLOCATOR_PAGE_SIZE) {
        //printf("less than page\n");
        return;
    }

    offset = block_get_offset(block);

    offset1 = offset + BLOCK_STRUCT_SIZE + sizeof(tree_node_type);
    offset1 = (offset1 + ALLOCATOR_PAGE_SIZE - 1) & ~((size_t)ALLOCATOR_PAGE_SIZE - 1);

    offset2 = offset + size_curr + BLOCK_STRUCT_SIZE;
    offset2 &= ~((size_t)ALLOCATOR_PAGE_SIZE - 1);

    if (offset1 == offset2) {
        //printf("Middle of the page\n");
        return;
    }

    assert(((offset2 - offset1) & ((size_t)ALLOCATOR_PAGE_SIZE - 1)) == 0);

    //printf("offset1 - %zu, offset2 - %zu\n", offset1, offset2);
    kernel_reset((char*)block + (offset1 - offset), offset2 - offset1);
}
