#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "block.h"
#include "config.h"
#include <stdbool.h>

void* mem_alloc(size_t size);
void mem_show(const char *msg);
void mem_free(void* block);
void* mem_realloc(void* ptr, size_t size);

#endif
