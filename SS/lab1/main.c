#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "allocator.h"
#include "tester.h"

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
    // void *ptr1, *ptr2, *ptr3, *ptr4;

    // ptr1 = buf_alloc(1);
    // ptr2 = buf_alloc(1);
    // ptr3 = buf_alloc(1);
    // ptr4 = buf_alloc(1);
    // mem_show("allocs");
    // mem_free(ptr1);
    // mem_show("free(ptr4)");

    tester(true);
}
