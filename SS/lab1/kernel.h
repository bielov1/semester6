#ifndef KERNEL_H
#define KERNEL_H


#include "block.h"
#include <sys/mman.h>
#include <stdint.h>

void* memalloc(size_t size);

#endif
