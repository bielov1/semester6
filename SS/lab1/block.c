#include <stdio.h>
#include <stdlib.h>
#include "block.h"
#include "kernel.h"

void block_split(Block* a, int p) {
    // if (p < 1 || a == NULL) return;

    // Block* ptr_next = a->next;
    // Block* x = a;
    // size_t size = x->size_curr/p;

    // for (int i = 0; i < p; i++) {
    //     Block* nb = (Block*)memalloc(size);
    //     nb->size_curr = size;
    //     if (i == 0 && x->first_Block) {
    //         nb->size_prev = 0;
    //     } else {
    //         nb->size_prev = x->size_curr;
    //     }

    //     x->next = nb;
    //     x = nb;;

    // }

    // if (!a->last_Block) x->next = ptr;
    // free(a);
}

void block_merge(Block* a) {
    // if (a == NULL || a->last_Block ||a->last_Block) return;

    // Block* right_neighbor = a->next;

    // if (right_neighbor->next) {
    //     a->next = right_neighbor->next;
    // } else {
    //     a->next = NULL;
    // }
    // a->size_curr += right_neighbor->size_curr;

    // free(right_neighbor);
}
