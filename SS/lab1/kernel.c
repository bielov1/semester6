#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kernel.h"

#define DEBUG_KERNEL_RESET

static _Noreturn void
failed_kernel_alloc(void)
{
#define msg "kernel_alloc() failed\n"
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
#undef msg
    exit(EXIT_FAILURE);
}

static _Noreturn void
failed_kernel_free(void)
{
#define msg "kernel_free() failed\n"
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
#undef msg
    exit(EXIT_FAILURE);
}

static _Noreturn void
failed_kernel_reset(void)
{
#define msg "kernel_reset() failed\n"
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
#undef msg
    exit(EXIT_FAILURE);
}

#if !(defined(_WIN32) || defined(_WIN64))

#include <sys/mman.h>
#include <errno.h>

void *
kernel_alloc(size_t size)
{
    void *ptr;

#if defined(MAP_ANONYMOUS)
# define MMAP_FLAG_ANON MAP_ANONYMOUS
#elif defined(MAP_ANON)
# define MMAP_FLAG_ANON MAP_ANON
#else
# error "Do not know how to get anonymous memory"
#endif
    ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MMAP_FLAG_ANON|MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED) {
        if (errno == ENOMEM)
            return NULL;
        failed_kernel_alloc();
    }
    return ptr;
}

void
kernel_free(void *ptr, size_t size)
{
    if (munmap(ptr, size) < 0)
        failed_kernel_free();
}

void
kernel_reset(void *ptr, size_t size)
{
#ifdef DEBUG_KERNEL_RESET
    memset(ptr, 0x7e, size);
#endif
    if (madvise(ptr, size, MADV_DONTNEED) < 0)
        failed_kernel_reset();
}

#else

void *
kernel_alloc(size_t size)
{
    void *ptr;

    ptr = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL) {
        // Check error code and call failed_kernel_alloc() if necessarily.
        // failed_kernel_alloc();
        return NULL;
    }
    return ptr;
}

void
kernel_free(void *ptr, size_t size)
{
    (void)size;
    if (VirtualFree(ptr, 0, MEM_RELEASE) == 0)
        failed_kernel_free();
}

void
kernel_reset(void *ptr, size_t size)
{
#ifdef DEBUG_KERNEL_RESET
    memset(ptr, 0x7e, size);
#endif
    if (ViraulAlloc(ptr, size, MEM_RESET, PAGE_READWRITE) == NULL)
        failed_kernel_reset();
}

#endif /* deined(_WIN32) || defined(_WIN64) */
