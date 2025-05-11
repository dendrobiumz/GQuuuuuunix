#include "atomic_ops.h"


static int cmpxchgu32(volatile u32 *ptr, u32 expected, u32 desired)
{
    int ret = -1;
    intr_off();
    if (*ptr == expected) {
        smp_mb();
        *ptr = desired;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

static int cmpxchgu64(volatile u64 *ptr, u64 expected, u64 desired)
{
    int ret = -1;
    intr_off();
    if (*ptr == expected) {
        smp_mb();
        *ptr = desired;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

static int cmpxchgu16(volatile u16 *ptr, u16 expected, u16 desired)
{
    int ret = -1;
    intr_off();
    if (*ptr == expected) {
        smp_mb();
        *ptr = desired;
        smp_mb();
        ret = 0;        
    }
    intr_on();
    return ret;
}

static int cmpxchgu8(volatile u8 *ptr, u8 expected, u8 desired)
{
    int ret = -1;
    intr_off();
    if (*ptr == expected) {
        smp_mb();
        *ptr = desired;
        smp_mb();        
        ret = 0;
    }
    intr_on();
    return ret;
}


int __cmpxchg(void *ptr, u64 expected, u64 desired, i64 sz)
{
    int ret;
    switch(sz) {
    case 1:
        ret = cmpxchgu8(ptr, expected, desired);
        break;
    case 2:
        ret = cmpxchgu16(ptr, expected, desired);
        break;
    case 4:
        ret = cmpxchgu32(ptr, expected, desired);
        break;
    case 8:
        ret = cmpxchgu64(ptr, expected, desired);
        break;
    deafult:
        __builtin_unreachable();
    }
    return ret;
}

static int test_set_u32(volatile u32 *ptr)
{
    int ret = -1;
    intr_off();
    if (*ptr == 0) {
        smp_mb();
        *ptr = 1;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

static int test_set_u64(volatile u64 *ptr)
{
    int ret = -1;
    intr_off();
    if (*ptr == 0) {
        smp_mb();
        *ptr = 1;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

static int test_set_u16(volatile u16 *ptr)
{
    int ret = -1;
    intr_off();
    if (*ptr == 0) {
        smp_mb();
        *ptr = 1;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

static int test_set_u8(volatile u8 *ptr)
{
    int ret = -1;
    intr_off();
    if (*ptr == 0) {
        smp_mb();
        *ptr = 1;
        smp_mb();
        ret = 0;
    }
    intr_on();
    return ret;
}

int __test_set(void *ptr, i64 sz)
{
    int ret;
    switch(sz) {
    case 1:
        ret = test_set_u8(ptr);
        break;
    case 2:
        ret = test_set_u16(ptr);
        break;
    case 4:
        ret = test_set_u32(ptr);
        break;
    case 8:
        ret = test_set_u64(ptr);
        break;
    default:
        __builtin_unreachable();
    }
    return ret;
}