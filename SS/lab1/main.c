#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "allocator.h"
#include "kernel.h"
#include "allocator_impl.h"
#include "block.h"
#include "tester.h"
#include "tree.h"

static void* buf_alloc(size_t size) {
    char *buf;

    buf = mem_alloc(size);
    if (buf != NULL) {
        for (size_t i = 0; i < size; ++i) {
            buf[i] = (char)rand();
        }
    }
    return buf;
}

int main() {
    // block_tree_t tree = TREE_INITIALIZER;
    // void* ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;

    void* ptr1, *ptr2;
    buf_alloc(SIZE_MAX);
    mem_show("Initial");

    ptr1 = buf_alloc(256);
    mem_show("alloc(256)");
    // ptr3 = mem_realloc(ptr1, 257);
    // mem_show("realloc(ptr1 -> 257)");
    // ptr4 = buf_alloc(40);
    // mem_show("alloc(40)");
    // ptr5 = buf_alloc(300);
    // mem_show("alloc(300)");
    // ptr6 = mem_realloc(ptr4, 120);
    // mem_show("realloc(ptr4 -> 120)");
    //srand((unsigned int)time(NULL));
    //tester(true);
}
