#ifndef _ATOMIC_OPS_H
#define _ATOMIC_OPS_H
#include "riscv.h"
#include "types.h"
#include "task.h"

enum {
    MEM_ORDER_RLX = 0,
    MEM_ORDER_ACQ,
    MEM_ORDER_REL,
    MEM_ORDER_ACQ_REL,
    MEM_ORDER_SEQ_CST
};

static inline void bad_atomic_type_error(void)
{
    extern void __bad_atomic_type(void) __attribute__((error("unsupported atomic type")));
    __bad_atomic_type();
}

static inline int __test_set_i32(volatile atomic_int_t* a_int)
{
    int old;
    push_off();
    asm volatile (
        "amoswap.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(1), "r"(&a_int->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __test_set_i64(volatile atomic_long_t* a_long)
{
    long old;
    push_off();
    asm volatile (
        "amoswap.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(1), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __test_set_u64(volatile atomic_ulong_t* a_ulong)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoswap.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(1), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define TEST_SET(ATMOIC_PTR) \
({                                   \
   __auto_type __p = (ATMOIC_PTR);  \
    _Generic(*__p,                      \
        atomic_int_t: __test_set_i32,   \
        atomic_long_t: __test_set_i64,  \
        atomic_ulong_t: __test_set_u64,  \
        default: bad_atomic_type_error   \
    )(__p);                              \
})

static inline int __cmp_xchg_i32(volatile atomic_int_t* a_int, int expected, int desired)
{
    int old, result;
    asm volatile("#cmpxchg\n"
    "1: lr.w.aqrl %[old], (%[ptr])\n"
    "   bne  %[old], %[expected], 2f\n"
    "   sc.w.rl %[result], %[desired], (%[ptr])\n"
    "   bnez %[result], 1b\n"
    "2:\n"
    : [result]"=&r" (result), [old]"=&r" (old)
    : [ptr]"r" (&a_int->val), [expected] "r" (expected), [desired] "r" (desired)
    : "memory");
    return old;
}

static inline long __cmp_xchg_i64(volatile atomic_long_t* a_long, long expected, long desired)
{
    long old, result;
    asm volatile("#cmpxchg\n"
    "1: lr.d.aqrl %[old], (%[ptr])\n"
    "   bne  %[old], %[expected], 2f\n"
    "   sc.d.rl %[result], %[desired], (%[ptr])\n"
    "   bnez %[result], 1b\n"
    "2:\n"
    : [result]"=&r" (result), [old]"=&r" (old)
    : [ptr]"r" (&a_long->val), [expected] "r" (expected), [desired] "r" (desired)
    : "memory");
    return old;
}   


static inline unsigned long __cmp_xchg_u64(volatile atomic_ulong_t* a_ulong, 
                                            unsigned long expected, 
                                            unsigned long desired)
{
    unsigned long old, result;
    asm volatile("#cmpxchg\n"
    "1: lr.d.aqrl %[old], (%[ptr])\n"
    "   bne  %[old], %[expected], 2f\n"
    "   sc.d.rl %[result], %[desired], (%[ptr])\n"
    "   bnez %[result], 1b\n"
    "2:\n"
    : [result]"=&r" (result), [old]"=&r" (old)
    : [ptr]"r" (&a_ulong->val), [expected] "r" (expected), [desired] "r" (desired)
    : "memory");
    return old;
}

#define CMP_XCHG(ATMOIC_PTR, EXPECTED, DESIRED) \
({ \
    __auto_type __p = (ATMOIC_PTR); \
    __typeof__((void)0, *__p) __e = (EXPECTED); \
    __typeof__((void)0, *__p) __d = (DESIRED);  \
    _Generic(*__p,                      \
        atomic_int_t: __cmp_xchg_i32,   \
        atomic_long_t: __cmp_xchg_i64,  \
        atomic_ulong_t: __cmp_xchg_u64  \
    )(__p, __e, __d);                   \
})


static inline int __atomic_load_i32(atomic_int_t *a_int)
{
    int old;
    asm volatile("#atomic load\n"
    "lr.w.aqrl %[old], (%[ptr])"
    : [old]"=r" (old)
    : [ptr] "r" (&a_int->val)
    : "memory"
    );
    return old;
}

static inline long __atomic_load_i64(atomic_long_t *a_long)
{
    long old;
    asm volatile("#atomic load\n"
    "lr.d.aqrl %[old], (%[ptr])"
    : [old]"=r" (old)
    : [ptr] "r" (&a_long->val)
    : "memory"
    );
    return old;
}

static inline int __atomic_load_u64(atomic_ulong_t *a_ulong)
{
    unsigned long old;
    asm volatile("#atomic load\n"
    "lr.d.aqrl %[old], (%[ptr])"
    : [old]"=r" (old)
    : [ptr] "r" (&a_ulong->val)
    : "memory"
    );
    return old;
}

#define ATOMIC_LOAD(ATOMIC_PTR) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    _Generic(*__p,                          \
        atomic_int_t: __atomic_load_i32,   \
        atomic_long_t: __atomic_load_i64,  \
        atomic_ulong_t: __atomic_load_u64,  \
        default: bad_atomic_type_error   \
    )(__p);                                \
})


static inline int __atomic_store_i32(volatile atomic_int_t *a_int, int val)
{
    int result, tmp;
    push_off();
    asm volatile("#atomic store\n"
    "1: lr.w %[tmp], (%[ptr])\n"
    "   sc.w %[result], %[val], (%[ptr])\n"
    "   bnez %[result], 1b\n"
    : [tmp]"=&r" (tmp), [result]"=&r" (result)
    : [ptr] "r" (&a_int->val), [val] "r" (val)
    : "memory"
    );
    pop_off();
    return val;
}

static inline long __atomic_store_i64(volatile atomic_long_t *a_long, long val)
{
    long result, tmp;
    push_off();
    asm volatile("#atomic store\n"
    "1: lr.d %[tmp], (%[ptr])\n"
    "   sc.d %[result], %[val], (%[ptr])"
    "   bnez %[result], 1b\n"
    : [tmp]"=&r" (tmp), [result]"=&r" (result)
    : [ptr] "r" (&a_long->val), [val] "r" (val)
    : "memory"
    );
    pop_off();
    return val;
}

static inline unsigned long __atomic_store_u64(volatile atomic_ulong_t *a_ulong, unsigned long val)
{
    unsigned long result, tmp;
    push_off();
    asm volatile("#atomic store\n"
    "1: lr.d %[tmp], (%[ptr])\n"
    "   sc.d %[result], %[val], (%[ptr])"
    "   bnez %[result], 1b\n"
    : [tmp]"=&r" (tmp), [result]"=&r" (result)
    : [ptr] "r" (&a_ulong->val), [val] "r" (val)
    : "memory"
    );
    pop_off();
    return val;
}

#define ATOMIC_STORE(ATOMIC_PTR, VAL) \
({                                    \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL);      \
    _Generic(*__p,                          \
        atomic_int_t: __atomic_store_i32,   \
        atomic_long_t: __atomic_store_i64,  \
        atomic_ulong_t: __atomic_store_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                          \
})


static inline int __atomic_fetch_add_i32(volatile atomic_int_t* a_int, int val)
{
    int old;
    push_off();
    asm volatile (
        "amoadd.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_int->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __atomic_fetch_add_i64(volatile atomic_long_t* a_long, long val)
{
    long old;
    push_off();
    asm volatile (
        "amoadd.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __atomic_fetch_add_u64(volatile atomic_ulong_t* a_ulong, unsigned long val)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoadd.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define ATOMIC_FETCH_ADD(ATMOIC_PTR, VAL) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL); \
    _Generic(*__p,                      \
        atomic_int_t: __atomic_fetch_add_i32,   \
        atomic_long_t: __atomic_fetch_add_i64,  \
        atomic_ulong_t: __atomic_fetch_add_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                             \
})

static inline int __atomic_fetch_sub_i32(volatile atomic_int_t* a_int, int val)
{
    int old;
    push_off();
    asm volatile (
        "amoadd.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_int->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __atomic_fetch_sub_i64(volatile atomic_long_t* a_long, long val)
{
    long old;
    push_off();
    asm volatile (
        "amoadd.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __atomic_fetch_sub_u64(volatile atomic_ulong_t* a_ulong, unsigned long val)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoadd.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define ATOMIC_FETCH_SUB(ATMOIC_PTR, VAL) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL); \
    _Generic(*__p,                      \
        atomic_int_t: __atomic_fetch_sub_i32,   \
        atomic_long_t: __atomic_fetch_sub_i64,  \
        atomic_ulong_t: __atomic_fetch_sub_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                             \
})

static inline int __atomic_fetch_and_i32(volatile atomic_int_t* a_int, int val)
{
    int old;
    push_off();
    asm volatile (
        "amoand.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_int->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __atomic_fetch_and_i64(volatile atomic_long_t* a_long, long val)
{
    long old;
    push_off();
    asm volatile (
        "amoand.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __atomic_fetch_and_u64(volatile atomic_ulong_t* a_ulong, unsigned long val)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoand.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define ATOMIC_FETCH_AND(ATMOIC_PTR, VAL) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL); \
    _Generic(*__p,                      \
        atomic_int_t: __atomic_fetch_and_i32,   \
        atomic_long_t: __atomic_fetch_and_i64,  \
        atomic_ulong_t: __atomic_fetch_and_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                             \
})

static inline int __atomic_fetch_or_i32(volatile atomic_int_t* a_int, int val)
{
    int old;
    push_off();
    asm volatile (
        "amoor.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_int->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __atomic_fetch_or_i64(volatile atomic_long_t* a_long, long val)
{
    long old;
    push_off();
    asm volatile (
        "amoor.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __atomic_fetch_or_u64(volatile atomic_ulong_t* a_ulong, unsigned long val)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoor.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define ATOMIC_FETCH_OR(ATMOIC_PTR, VAL) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL); \
    _Generic(*__p,                      \
        atomic_int_t: __atomic_fetch_or_i32,   \
        atomic_long_t: __atomic_fetch_or_i64,  \
        atomic_ulong_t: __atomic_fetch_or_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                             \
})

static inline int __atomic_fetch_xor_i32(volatile atomic_int_t* a_int, int val)
{
    int old;
    volatile int *ptr = &a_int->val;
    push_off();
    asm volatile (
        "amoxor.w.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(ptr)
        : "memory"
    );
    pop_off();
    return old;
}

static inline long __atomic_fetch_xor_i64(volatile atomic_long_t* a_long, long val)
{
    long old;
    push_off();
    asm volatile (
        "amoxor.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_long->val)
        : "memory"
    );
    pop_off();
    return old;
}

static inline unsigned long __atomic_fetch_xor_u64(volatile atomic_ulong_t* a_ulong, unsigned long val)
{
    unsigned long old;
    push_off();
    asm volatile (
        "amoxor.d.aqrl %0, %1, (%2)"
        : "=r"(old)
        : "r"(val), "r"(&a_ulong->val)
        : "memory"
    );
    pop_off();
    return old;
}

#define ATOMIC_FETCH_XOR(ATMOIC_PTR, VAL) \
({ \
    __auto_type __p = (ATOMIC_PTR); \
    __auto_type __val = (VAL); \
    _Generic(*__p,                      \
        atomic_int_t: __atomic_fetch_xor_i32,   \
        atomic_long_t: __atomic_fetch_xor_i64,  \
        atomic_ulong_t: __atomic_fetch_xor_u64,  \
        default: bad_atomic_type_error   \
    )(__p, __val);                             \
})

static inline void smp_mb(void)
{ 
    __asm__ volatile("fence rw, rw" ::: "memory");
}


#endif