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


#define NULL              ((void*)(0x0))
#define true              (1)
#define false             (0)

#endif