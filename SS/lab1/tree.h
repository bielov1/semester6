#include "avl/avl.h"

typedef struct avl_tree tree_type;
typedef struct avl_node tree_node_type;

#define TREE_INITIALIZER { .avl_root = NULL }
#define tree_add(t, n, k) avl_add((t), (n), (k))
#define tree_remove(t, n) avl_remove((t), (n))
#define tree_find_best(t, k) avl_find_best((t), (k))
#define tree_is_empty(t) avl_is_empty(t)
#define tree_walk(t, f) avl_walk((t), (f))
