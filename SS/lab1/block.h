#include <stdbool.h>

#include "allocator_impl.h"
#include "tree.h"

#define BLOCK_OCCUPIED (size_t)0x1
#define BLOCK_LAST (size_t)0x2

typedef struct {
    size_t size_curr;
    size_t size_prev;
    size_t offset;
    //bool flag_busy;
    //bool flag_first;
    //bool flag_last;
} Block;

#define BLOCK_STRUCT_SIZE ROUND_BYTES(sizeof(Block))
#define BLOCK_SIZE_MIN ROUND_BYTES(sizeof(tree_node_type))

Block *block_split(Block *, size_t);
void block_merge(Block *, Block *);
void block_dontneed(Block *);


static inline void *
block_to_payload(const Block *block)
{
    return (char *)block + BLOCK_STRUCT_SIZE;
}

static inline Block *
payload_to_block(const void *ptr)
{
    return (Block *)((char *)ptr - BLOCK_STRUCT_SIZE);
}

static inline tree_node_type *
block_to_node(const Block *block)
{
    return block_to_payload(block);
}

static inline Block *
node_to_block(const tree_node_type *node)
{
    return payload_to_block(node);
}

static inline void
block_set_size_curr(Block *block, size_t size)
{
    size_t flags = block->size_curr & (BLOCK_OCCUPIED | BLOCK_LAST);
    block->size_curr = size | flags;
}

static inline size_t
block_get_size_curr(const Block *block)
{
    return block->size_curr & ~(BLOCK_OCCUPIED | BLOCK_LAST);
}

static inline void
block_set_size_prev(Block *block, size_t size)
{
    block->size_prev = size;
}

static inline size_t
block_get_size_prev(const Block *block)
{
    return block->size_prev;
}

static inline void
block_set_flag_busy(Block *block)
{
    block->size_curr |= BLOCK_OCCUPIED;
}

static inline bool
block_get_flag_busy(const Block *block)
{
    return (block->size_curr & BLOCK_OCCUPIED) != 0;
}

static inline void
block_clr_flag_busy(Block *block)
{
    block->size_curr &= ~(BLOCK_OCCUPIED);
}


static inline bool
block_get_flag_first(const Block *block)
{
    return (block->size_prev == 0) ? true : false;
}

static inline void
block_set_flag_last(Block *block)
{
    block->size_curr |= BLOCK_LAST;
}

static inline bool
block_get_flag_last(const Block *block)
{
    return (block->size_curr & BLOCK_LAST) != 0;
}

static inline void
block_clr_flag_last(Block *block)
{
    block->size_curr &= ~(BLOCK_LAST);
}

static inline void block_set_offset(Block* block, size_t offset) {
    block->offset = offset;
}

static inline size_t block_get_offset(Block* block) {
    return block->offset;
}

static inline Block *
block_next(const Block *block)
{
    return (Block *)
        ((char *)block + BLOCK_STRUCT_SIZE + block_get_size_curr(block));
}

static inline Block *
block_prev(const Block *block)
{
    return (Block *)
        ((char *)block - BLOCK_STRUCT_SIZE - block_get_size_prev(block));
}

static inline void
arena_init(Block *block, size_t size)
{
    block->size_curr = size;
    block->size_prev = 0;
    block->offset = 0;
    block_set_flag_last(block);
}

static inline void
block_init(Block *block)
{
    block_clr_flag_busy(block);
    block_clr_flag_last(block);
}
