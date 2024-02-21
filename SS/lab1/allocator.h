#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "block.h"
#include "config.h"
#include <stdbool.h>

typedef struct {
    Block* blocks_link[BLOCK_QUANTITY];
    size_t size;
    size_t used;
    Arena* next;
} Arena;

Block* mem_alloc(Arena* arena, size_t size);
void mem_show(Arena* arena);
void mem_free(Arena*, Block* b, size_t size);
void mem_realloc(Arena* a, Block* b, size_t size)

#endif
