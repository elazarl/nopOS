#include "malloc.h"
#include <string.h>

#define MAX_ALLOC 1000

struct pool {
    char *p;
    int n;
    int total;
    int alloc[MAX_ALLOC];
    int alloc_size[MAX_ALLOC];
    int nalloc;
};

void *pool_alloc(struct pool *p, int size) {
    void *rv = p->p+p->n;

    p->alloc[p->nalloc]      = p->n;
    p->alloc_size[p->nalloc] = size;

    p->n += size;
    p->nalloc++;
    return rv;
}

void pool_free(struct pool *p, void *free) {
    int i;
    int offset = (char *)free-p->p;
    for (i = p->nalloc-1; i >= 0; i--) {
        if (p->alloc[i] == offset) {
            p->alloc[i]      = -1;
        }
    }
    while (p->alloc[p->nalloc-1] == -1 && p->nalloc >= 0) {
        p->n -= p->alloc_size[p->nalloc-1];
        p->nalloc--;
    }
}

char buf[100000];
struct pool pool = { .p = buf, .total = sizeof(buf)};

void *_malloc(size_t sz) {
    return pool_alloc(&pool, sz);
}

void _free(void *ptr) {
    pool_free(&pool, ptr);
}

int isatty(int fd) {
    return 1;
}


