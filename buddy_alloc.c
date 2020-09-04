#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "buddy_alloc.h"


void* balloc(size_t sz) {
    int i = 0, order;
    void *block, *buddy;

    for(; BLOCK_SZ(i) < sz + 1; ++i);
    i = (i < MIN_ORDER) ? MIN_ORDER : i;
    order = i;

    for(;; ++i) {
        if(i > MAX_ORDER)
            return NULL;
        if(ctx_p->freeList[i])
            break;
    }

    block = ctx_p->freeList[i];
    ctx_p->freeList[i] = *(void**)ctx_p->freeList[i];

    while(i-- > order) {
        buddy = GET_BUDDY(block, i);
        ctx_p->freeList[i] = buddy;
    }

    *((uint8_t*)(block - 1)) = order;
    return block;
}

void bfree(void* block) {
    int i = 0;
    void* buddy;
    void** pp;

    i = *((uint8_t*)(block - 1));

    for(;; ++i) {
        buddy = GET_BUDDY(block, i);
        pp = &ctx_p->freeList[i];

        while(*pp && *pp != buddy) {
            pp = (void**) *pp;
        }

        if(*pp != buddy) {
            *(void**) block = ctx_p->freeList[i];
            ctx_p->freeList[i] = block;
            return;
        }

        if(block >= buddy)
            block = buddy;

        *pp = *(void**)*pp;
    }
}


void buddy_init(buddy_t* buddy) {
    ctx_p = buddy;
    memset(ctx_p, 0, sizeof(buddy_t));
    ctx_p->freeList[MAX_ORDER] = ctx_p->pool;
    printf("pool=%p\n", ctx_p->pool);
}

void buddy_term(buddy_t* buddy) {
    memset(buddy, 0, sizeof(buddy_t));
}

int main(int argc, const char** argv) {
    buddy_t* buddy = malloc(sizeof(buddy_t));
    buddy_init(buddy);

    int* test = (int*)balloc(sizeof(int));
    printf("%p, sz=%" PRIu8 "\n", test, BLOCK_SZ(*((uint8_t*)(test - 1))));

    *test = 5;
    bfree(test);



    printf("hello there, %u\n", POOL_SZ);
    return 0;
}