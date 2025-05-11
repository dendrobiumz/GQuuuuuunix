#ifndef _ATOMIC_OPS_H
#define _ATOMIC_OPS_H
#include "riscv.h"
#include "types.h"

static inline void smp_mb(void)
{ 
    __asm__ volatile("fence rw, rw" ::: "memory");
}

int __cmpxchg(void *ptr, u64 expected, u64 desired, i64 sz);

#define CMPXCHG(ptr, old, new) ({ \
    typeof(*(ptr)) __old = (old); \
    typeof(*(ptr)) __new = (new); \
    __cmpxchg(ptr, old, new, sizeof(*(ptr))); \
})

int __test_set(void *ptr, i64 sz);

#define TEST_SET(ptr) ({ \
    __test_set(ptr, sizeof(*(ptr))); \
})

#endif