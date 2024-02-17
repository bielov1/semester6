#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "block.h"
#include "config.h"

typedef struct {
    Block* blocks_link[BLOCK_QUANTITY];
    size_t size;
    size_t used;
} Arena;

Block* mem_alloc(Arena* arena, size_t size);
void mem_show(Arena* arena);
void mem_free(Block* b);

#endif
