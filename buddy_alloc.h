#ifndef BUDDY_ALLOC_H
#define BUDDY_ALLOC_H

#include <stdint.h>

#define MAX_ORDER             14
#define MIN_ORDER             4

#define BLOCK_SZ(n)          (1 << n)

#define _BASE_ADDR_          ((uintptr_t)ctx_p->mem)
#define _OFFSET_(addr)       ((uintptr_t)addr - _BASE_ADDR_)
#define _GET_BUDDY_(addr, n) (_OFFSET_(addr) ^ (1 << n))
#define GET_BUDDY(addr, n)   ((void*)( _GET_BUDDY_(addr, n) + _BASE_ADDR_))

typedef struct buddy {
    void* freeList [MAX_ORDER + 2];
    uint8_t mem [1 << MAX_ORDER];
} buddy_t;

buddy_t* ctx_p;

void buddy_init(buddy_t* buddy);
void buddy_term(buddy_t* buddy);

void* balloc(size_t sz);
void bdealloc(void* block);


#endif // BUDDY_ALLOC_H