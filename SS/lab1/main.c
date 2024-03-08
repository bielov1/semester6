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
    if (buf != NULL)
        for (i = 0; i < size; ++i)
            buf[i] = (char)rand();
    return buf;
}

int main(void)
{
    void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
    ptr1 = buf_alloc(1);
    mem_show("alloc(1)");
    ptr2 = buf_alloc(70000);
    mem_show("alloc(70000)");
    // ptr2 = buf_alloc(455);
    // ptr3 = buf_alloc(2);
    // ptr4 = buf_alloc(88);
    // ptr5 = buf_alloc(600);
    // mem_show("allocs");
    // mem_free(ptr2);
    // mem_show("free ptr2");
    // mem_free(ptr4);
    // mem_show("free ptr4");
    // mem_realloc(ptr1, 900);
    // mem_show("realloc ptr1 -> 900");
    // mem_realloc(ptr5, 20);
    // mem_show("realloc ptr5 -> 20");
    // srand(time(NULL));
    // tester(true);
}
