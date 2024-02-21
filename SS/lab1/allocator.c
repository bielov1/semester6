#include <stdio.h>
#include "allocator.h"
#include "block.h"
#include "allocator_impl.h"
#include "kernel.h"

Block* mem_alloc(Arena* arena, size_t size) {
    if ((size + (ALIGN - 1)) < size) return NULL;

    size_t aligned_size = ROUND_BYTES(size);

    if (arena->used + aligned_size > arena->size || aligned_size < size) return NULL;


    Block *block;
    for(int i = 0; i < BLOCK_QUANTITY; i++) {
        block = arena->blocks_link[i];
        if (!get_flag_busy(block) && get_size_curr(block) >= aligned_size) {
            set_flag_busy(block);
            arena->block_busy[i] = true;
            arena->used += aligned_size;
            return block;
        }
    }
    return NULL;
}


void mem_show(Arena* arena) {
    for(int i = 0; i < BLOCK_QUANTITY; i++) {
        Block* b = arena->blocks_link[i];
        if (b == NULL) {
            printf("Block %d was deleted\n", i);
            continue;
        }
        printf("Block %d: ", i);
        printf("size_curr = %ld;\n", get_size_curr(b));
        printf("          size_prev = %ld;\n", get_size_prev(b));
        printf("          flag_busy = %d;\n", get_flag_busy(b));
        printf("          first_block = %d;\n", get_first_block(b));
        printf("          last_block = %d;\n", get_last_block(b));
    }

}

void mem_free(Arena* a, Block* b, size_t size) {
    if (b == NULL || a == NULL) return;

    if (munmap(b, size) == -1) {
        printf("munmap failed\n");
    } else {
        a->used -= size;
        for (int i = 0; i < BLOCK_QUANTITY; i++) {
            if (a->blocks_link[i] == b) {
                a->blocks_link[i] = NULL;
                break;
            }
        }
    }
}


void mem_realloc(Arena* a, Block* b, size_t new_size) {
    if (b == NULL || a == NULL) {
        return;
    }

    size_t old_size = get_size_curr(b);

    if (new_size <= old_size) {

    }



}
