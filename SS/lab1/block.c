#include <assert.h>

#include "block.h"

Block* block_split(Block *block, size_t size)
{
    Block* block_r;
    size_t size_rest;

    block_set_flag_busy(block);
    size_rest = block_get_size_curr(block) - size;
    if (size_rest >= BLOCK_STRUCT_SIZE + BLOCK_SIZE_MIN) {
        size_rest -= BLOCK_STRUCT_SIZE;
        block_set_size_curr(block, size);
        printf("block->size_curr = %zu\n", block->size_curr);
        printf("block_get_size_curr(block) = %zu", block_get_size_curr(block));
        block_r = block_next(block);
        block_init(block_r);
        block_set_size_curr(block_r, size_rest);
        block_set_size_prev(block_r, size);
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
    if (block->size_prev == 0) {
        block_set_flag_first(block);
    }
    if (block_get_flag_last(block_r)) {
        block_set_flag_last(block);
    }
    else {
        block_set_size_prev(block_next(block_r), size);
    }

}
