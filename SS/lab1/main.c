#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "allocator.h"
#include "allocator_impl.h"
#include <stdbool.h>
#include "block.h"

int main() {
    Arena arena;
    arena.size = DEF_ARENA_SIZE;
    arena.used = 0;

    for (int i = 0; i < BLOCK_QUANTITY; i++) {
        size_t size = ALLOCATOR_PAGE_SIZE;
        Block* nb = malloc(size);
        if (i == 0) {
            set_size_curr(nb, size);
            set_size_prev(nb, 0);
        } else {
            set_size_curr(nb, size);
            set_size_prev(nb, size);
        }
        arena.blocks_link[i] = nb;
    }

    for (int i = 0; i < BLOCK_QUANTITY; i++) {
        Block* b = mem_alloc(&arena, ALLOCATOR_PAGE_SIZE);
        if (b == NULL) {
            continue;
        }

        if (i == 0) {
            set_first_block(b);
        }

        if (i == BLOCK_QUANTITY-1) {
            set_last_block(b);
        }

    }

    //mem_free(arena.blocks_link[0]);
    mem_show(&arena);

    return 0;
}
