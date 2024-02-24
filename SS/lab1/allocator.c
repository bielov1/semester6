#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "allocator.h"
#include "block.h"
#include "allocator_impl.h"
#include "kernel.h"

#define ARENA_SIZE (ALLOCATOR_PAGE_SIZE * DEF_ARENA_SIZE)
#define BLOCK_SIZE_MAX (ARENA_SIZE - BLOCK_STRUCT_SIZE)

static Block* Arena = NULL;

static int arena_alloc(void) {
    Arena = kernel_alloc(ARENA_SIZE);
    if (Arena != NULL) {
        initArena(Arena, ARENA_SIZE - BLOCK_STRUCT_SIZE);
        return 0;
    }
    return -1;
}

void* mem_alloc(size_t size) {
    Block* block;
    if (Arena == NULL) {
        if (arena_alloc() < 0) {
            return NULL;
        }
    }

    if (size > BLOCK_SIZE_MAX) return NULL;

    size_t aligned_size = ROUND_BYTES(size);
    for (block = Arena ;; block = block_next(block)) {
        if (!get_flag_busy(block) && get_size_curr(block) >= aligned_size) {
            block_split(block, aligned_size);
            return block_to_payload(block);
        }
        if (get_last_block(block)) break;
    }

    return NULL;
}


void mem_show(const char *msg) {
    Block* block;

    printf("%s:\n", msg);
    if (Arena == NULL) {
        printf("Arena was not created\n");
        return;
    }
    for (block = Arena ;; block=block_next(block)) {
        printf("[%20p] %10zu %10zu %s %s %s\n", (void*)block,
            get_size_curr(block),
            get_size_prev(block),
            get_flag_busy(block) ? "busy" : "free",
            get_first_block(block) ? "first" : "",
            get_last_block(block) ? "last" : "");
        if (get_last_block(block)) break;
    }
}

void mem_free(void* ptr) {
    Block* block, *block_r, *block_l;

    if (ptr == NULL) return;

    block = payload_to_block(ptr); // IMPORTANT
    release_flag_busy(block);      //
    if (!get_last_block(block)) {
        block_r = block_next(block);
        if (!get_flag_busy(block_r)) {
            block_merge(block, block_r);
        }
    }

    if (!get_first_block(block)) {
        block_l = block_prev(block);
        if (!get_flag_busy(block_l)) {
            block_merge(block_l, block);
        }
    }
}


// |###[              ]###[                ]###[                    ]\                   \| (1)

// |###[      ]\      \###[                ]###[                    ]\                   \| (2)

// |###[      ]###[                ]###[                    ]\                           \| (3)


// \             \ - is a block, but without a header, and it is free

// |###[               ]\                                                                \|

void* mem_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
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
}
