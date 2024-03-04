#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "allocator.h"
#include "tester.h"

struct T {
    void *ptr;
    size_t size;
    unsigned int checksum;
};

static unsigned int
buf_checksum(const unsigned char *c, size_t size)
{
    unsigned int checksum = 0;

    while (size--)
        checksum = (checksum << 3) ^ (checksum >> 5) ^ *c++;
    return checksum;
}

static void
buf_fill(unsigned char *c, size_t size)
{
    while (size--)
        *c++ = (unsigned char)rand();
}

static void *
buf_alloc(size_t size)
{
    void *ptr;

    ptr = mem_alloc(size);
    if (ptr != NULL)
        buf_fill(ptr, size);
    return ptr;
}

void
tester(const bool verbose)
{
    const size_t t_NUM = 100;
    const size_t SZ_MIN = 1;
    const size_t SZ_MAX = 4094 * 10;
    unsigned long M = 1000;
    struct T t[t_NUM];
    void *ptr;
    size_t idx, size, size_min;
    unsigned int checksum;

    for (idx = 0; idx < t_NUM; ++idx)
        t[idx].ptr = NULL;
    for (unsigned long i = 0; i < M; ++i) {
        if (verbose)
            mem_show("------------------------");
        for (idx = 0; idx < t_NUM; ++idx) {
            if (t[idx].ptr != NULL)
                if (buf_checksum(t[idx].ptr, t[idx].size) != t[idx].checksum) {
                    printf("1. Checksum failed at [%p]\n", t[idx].ptr);
                    return;
                }
        }
        idx = (size_t)rand() % t_NUM;
        if (t[idx].ptr == NULL) {
            size = ((size_t)rand() % (SZ_MAX - SZ_MIN)) + SZ_MIN;
            if (verbose)
                printf("ALLOC %zu ", size);
            ptr = buf_alloc(size);
            if (ptr != NULL) {
                t[idx].size = size;
                t[idx].ptr = ptr;
                t[idx].checksum = buf_checksum(ptr, size);
                if (verbose)
                    printf("-> [%p]\n", ptr);
            } else {
                if (verbose)
                    printf("failed\n");
            }
        } else if (rand() & 1) {
            if (verbose)
                printf("REALLOC [%p] %zu ", t[idx].ptr, t[idx].size);
            size = ((size_t)rand() % (SZ_MAX - SZ_MIN)) + SZ_MIN;
            size_min = size < t[idx].size ? size : t[idx].size;
            checksum = buf_checksum(t[idx].ptr, size_min);
            ptr = mem_realloc(t[idx].ptr, size);
            if (ptr != NULL) {
                if (verbose)
                    printf("-> [%p] %zu\n", ptr, size);
                if (checksum != buf_checksum(ptr, size_min)) {
                    printf("2. Checksum failed at [%p]\n", ptr);
                    return;
                }
                buf_fill(ptr, size);
                t[idx].ptr = ptr;
                t[idx].size = size;
                t[idx].checksum = buf_checksum(t[idx].ptr, t[idx].size);
            } else {
                if (verbose)
                    printf("failed\n");
            }
        } else {
            if (verbose)
                printf("FREE [%p]\n", t[idx].ptr);
            mem_free(t[idx].ptr);
            t[idx].ptr = NULL;
        }
    }
    for (idx = 0; idx < t_NUM; ++idx) {
        if (t[idx].ptr != NULL)
            if (buf_checksum(t[idx].ptr, t[idx].size) != t[idx].checksum) {
                printf("3. Checksum failed at [%p]\n", t[idx].ptr);
                return;
            }
        mem_free(t[idx].ptr);
    }
    if (verbose)
        mem_show("------------------------");
}
