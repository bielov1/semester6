#include "kernel.h"

void* kernel_alloc(size_t length) {
    return mmap(NULL, length, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}
