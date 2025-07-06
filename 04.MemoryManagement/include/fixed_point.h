#ifndef _FIXED_POINT_H
#define _FIXED_POINT_H

#include "types.h"

typedef struct fixed_point {
    u64 data;
    u8 mantissa_bit;
} fixed_point_t;


static inline u64 fixed_point_get_integer(fixed_point_t fp)
{
    return fp.data >> fp.mantissa_bit;
}

static inline u64 fixed_point_get_mantissa(fixed_point_t fp)
{
    return fp.data & ((1ULL << fp.mantissa_bit) - 1);
}

#endif