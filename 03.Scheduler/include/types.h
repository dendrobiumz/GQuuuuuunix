#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char     u8;
typedef unsigned short    u16;
typedef unsigned int      u32;
typedef unsigned long     u64;

typedef unsigned long     size_t;
typedef long              ssize_t;

typedef char              i8;
typedef short             i16;
typedef int               i32;
typedef long              i64;

typedef u64               paddr_t;
typedef u64               vaddr_t;

typedef struct __attribute__ ((aligned (4))) atomic_int {
    volatile int val;
} atomic_int_t;


typedef struct __attribute__ ((aligned (8))) atomic_long {
    volatile long val;
} atomic_long_t;

typedef struct __attribute__ ((aligned (8))) atomic_ulong {
    volatile unsigned long val;
} atomic_ulong_t;


#define NULL              ((void*)(0x0))
#define true              (1)
#define false             (0)

#endif