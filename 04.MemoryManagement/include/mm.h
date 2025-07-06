#ifndef MM_H
#define MM_H

#include "list.h"

struct per_cpu_pages {
    int count;
    struct list_head head;
};

void* kalloc();

void kfree(void *addr);

void* _kalloc();

void _kfree(void *addr, int n);

void* page_alloc(int n);

void page_free(void *addr, int n);

void kinit_pa();

#endif