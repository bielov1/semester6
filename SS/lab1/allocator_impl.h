#include <stddef.h>

#define ALIGN _Alignof(max_align_t)
#define ROUND(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
#define ROUND_BYTES(x) ROUND((x), ALIGN)
