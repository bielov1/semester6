#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include "kernel.h"
#include "allocator_impl.h"
#include <stdlib.h>


//TODO: implement function for releasing flag_busy bit in block`s size_curr --- DONE

#define BLOCK_OCCUPIED (1 << 0)
#define BLOCK_LAST (1 << 1)
#define BLOCK_FIRST (1 << 2)

#define SIZE_MASK (~(BLOCK_OCCUPIED | BLOCK_LAST | BLOCK_FIRST))

typedef struct Block {
    size_t size_curr;
    size_t size_prev;
} Block;

void block_split(Block* block, size_t aligned_size);
void block_merge(Block* block, Block* block_r);

static inline char* block_to_payload(const Block *block) {
    return (char*)block + BLOCK_STRUCT_SIZE;
}

static inline Block* payload_to_block(const void *ptr) {
    return (Block*)((char*)ptr - BLOCK_STRUCT_SIZE);
}

static inline size_t get_size_curr(Block* b) {
    return b->size_curr & SIZE_MASK;
}

static inline size_t get_size_prev(Block* b) {
    return b->size_prev & SIZE_MASK;
}

static inline bool get_flag_busy(Block* b) {
    return (b->size_curr & BLOCK_OCCUPIED) != 0;
}

static inline bool get_first_block(Block* b) {
    return (b->size_curr & BLOCK_FIRST) != 0;
}

static inline bool get_last_block(Block* b) {
    return (b->size_curr & BLOCK_LAST) != 0;
}


static inline Block* block_next(Block* block) {
    return (Block*)((char*)block + BLOCK_STRUCT_SIZE + get_size_curr(block));
}

static inline Block* block_prev(Block* block) {
    return (Block*)((char*)block - BLOCK_STRUCT_SIZE - get_size_prev(block));
}

static inline void set_size_curr(Block* b, size_t size) {
    b->size_curr = size;
}

static inline void set_size_prev(Block* b, size_t size) {
    b->size_prev = size;
}

static inline void set_flag_busy(Block* b) {
    b->size_curr |= BLOCK_OCCUPIED;
}

static inline void release_flag_busy(Block* b) {
    b->size_curr &= ~BLOCK_OCCUPIED;
}

static inline void set_first_block(Block* b) {
    b->size_curr |= BLOCK_FIRST;
}

static inline void set_last_block(Block* b) {
    b->size_curr |= BLOCK_LAST;
}

static inline void clr_first_block(Block* b) {
    b->size_curr &= ~BLOCK_FIRST;
}

static inline void clr_last_block(Block* b) {
    b->size_curr &= ~BLOCK_LAST;
}

static inline void initArena(Block* b, size_t size) {
    set_size_curr(b, size);
    set_size_prev(b, 0);
    set_first_block(b);
    set_last_block(b);
}

#endif
