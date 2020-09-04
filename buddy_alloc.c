#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "buddy_alloc.h"


void* balloc(size_t sz) {
    int i = 0, order;
    void *block, *buddy;

     /*
      * Get i as least integer such that i >= log(sz + 1),
      * with 1 extra byte to store order (prefixed in
      * memory)
      */
    for(; BLOCK_SZ(i) < sz + 1; ++i);

    i = (i < MIN_ORDER) ? MIN_ORDER : i; // need 16 bytes
    order = i;

    for(;; ++i) {
        if(i > MAX_ORDER)
            return NULL;
        if(ctx_p->freeList[i]) // get first available block in level
            break;
    }

    block = ctx_p->freeList[i]; //get mem block
    ctx_p->freeList[i] = *(void**)ctx_p->freeList[i]; // set mem block to allocated

    while(i-- > order) { // merge free buddies up to tree root
        buddy = GET_BUDDY(block, i);
        ctx_p->freeList[i] = buddy;
    }

    *((uint8_t*)(block - 1)) = order; // prefix mem with order for dealloc later
    return block;
}

void bdealloc(void* block) {
    int i = 0;
    void* buddy;
    void** pp;

    i = *((uint8_t*)(block - 1)); // fetch order

    for(;; ++i) {
        buddy = GET_BUDDY(block, i);
        pp = &ctx_p->freeList[i];

        while(*pp && *pp != buddy) { // loop through level
            pp = (void**) *pp;
        }

        if(*pp != buddy) { // if buddy not found in level, add it in
            *(void**) block = ctx_p->freeList[i];
            ctx_p->freeList[i] = block;
            return;
        }
        else {
            if(block >= buddy) // always merge to lower block
                block = buddy;

            *pp = *(void**)*pp; // remove block
        }
    }
}

void buddy_init(buddy_t* buddy) {
    ctx_p = buddy;
    memset(ctx_p, 0, sizeof(buddy_t));
    ctx_p->freeList[MAX_ORDER] = ctx_p->mem; // starting addr
}

void buddy_term(buddy_t* buddy) {
    memset(buddy, 0, sizeof(buddy_t));
}

int main(int argc, const char** argv) {
    buddy_t* buddy = malloc(sizeof(buddy_t));
    buddy_init(buddy);

    void* test = balloc(sizeof(int));
    void* a = balloc(sizeof(char));
    printf("%p, sz=%" PRIu8 "\n", test, BLOCK_SZ(*((uint8_t*)(test - 1))));

    bdealloc(test);
    bdealloc(a);

    buddy_term(buddy);
    free(buddy);

    return 0;
}