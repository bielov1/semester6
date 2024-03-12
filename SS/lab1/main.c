#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "allocator.h"
#include "tester.h"
#include "block.h"
#include "tree.h"
#include "avl/avl_impl.h"

static void * buf_alloc(size_t size)
{
    char *buf;
    size_t i;

    buf = mem_alloc(size);
    if (buf != NULL) {
        for (i = 0; i < size; ++i) {
            buf[i] = (char)rand();
        }
    }
    return buf;
}

#include "tree.h"

int main(void)
{

    // void *ptr1, *ptr2, *ptr3, *ptr4;
    // ptr1 = mem_alloc(90000);
    // ptr2 = mem_alloc(1);
    // ptr3 = mem_alloc(9000);
    // ptr4 = mem_alloc(431);
    // mem_show("allocs");
    // ptr1 = mem_realloc(ptr1, 80000);
    // mem_show("realloc ptr1 -> 80000");
    // mem_free(ptr1);
    // mem_show("free ptr1");
    // mem_realloc(ptr4, 999);
    // mem_show("realloc ptr4 -> 999");
    srand(time(NULL));
    tester(true);
}
