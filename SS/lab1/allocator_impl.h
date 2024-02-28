#include <stddef.h>
#include "allocator.h"
#include "block.h"

#define ALIGN (_Alignof(max_align_t))
#define ROUND_BYTES_GENERIC(s) ((((s) + (ALIGN - 1)) / ALIGN) * ALIGN)
#define ROUND_BYTES(s) (((s) + (ALIGN - 1)) & ~(ALIGN-1))

#define BLOCK_STRUCT_SIZE ROUND_BYTES(sizeof(Block))
#define NODE_STRUCT_SIZE (sizeof(Block) - sizeof(block_tree_t))
