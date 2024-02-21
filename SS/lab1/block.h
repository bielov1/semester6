#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include "kernel.h"
#include <stdlib.h>


//TODO: implement function for releasing flag_busy bit in block`s size_curr --- DONE

#define BLOCK_OCCUPIED (1 << 0)
#define BLOCK_LAST (1 << 1)
#define BLOCK_FIRST (1 << 2)

#define SIZE_MASK (~(BLOCK_OCCUPIED | BLOCK_LAST | BLOCK_FIRST))

typedef struct Block {
    size_t size_curr;
    size_t size_prev;
    //bool flag_busy;
    //bool first_block;
    //bool last_block;
} Block;

void block_split(Block*, int);
void block_merge(Block*);

// void block_to_payload();
// void payload_to_block();
// void block_next();
// void block_prev();

static inline size_t get_size_curr(Block* b) {
    return b->size_curr & SIZE_MASK;
}

static inline size_t get_size_prev(Block* b) {
    return b->size_prev;
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

static inline void set_size_curr(Block* b, size_t size) {
    b->size_curr = (b->size_curr & ~SIZE_MASK) | (size & SIZE_MASK);
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


#endif
