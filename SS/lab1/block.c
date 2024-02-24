#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "block.h"
#include "kernel.h"

void block_split(Block* block, size_t size) {
    size_t size_rest = get_size_curr(block) - size;
    if (size_rest >= BLOCK_STRUCT_SIZE) {
        size_rest -= BLOCK_STRUCT_SIZE;

        bool is_first = get_first_block(block);
        bool is_last = get_last_block(block);

        set_size_curr(block, size);
        if (is_first) {
            set_first_block(block);
        }

        Block* block_r = block_next(block);
        set_size_curr(block_r, size_rest);
        set_size_prev(block_r, size);

        if (is_last) {
            clr_last_block(block);
            set_last_block(block_r);
        }

        set_flag_busy(block);
    }
}
void block_merge(Block* block, Block* block_r) {
    size_t size;

    bool is_first = get_first_block(block);

    size = get_size_curr(block) + get_size_curr(block_r) + BLOCK_STRUCT_SIZE;

    set_size_curr(block, size);
    if (is_first) {
        set_first_block(block);
    }

    if (get_last_block(block_r)) {
        set_last_block(block);
    } else {
        set_size_prev(block_next(block_r), size);
    }
}
