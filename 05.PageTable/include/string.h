#ifndef _STRING_H
#define _STRING_H

#include "types.h"

void* memcpy(void *dst, const void *src, i64 n);

void* memset(void* str, int c, i64 n);

i64 strlen(const char* str);

#endif