#ifndef  SPIN_LOCK_H
#define  SPIN_LOCK_H

#include "types.h"

struct cpu;

typedef struct __attribute__ ((aligned (8))) spin_lock {
    atomic_int_t isLocked;
    struct cpu *cpu;
    void (*acquire)(struct spin_lock *lk);
    void (*release)(struct spin_lock *lk);
    int (*is_holding)(struct spin_lock *lk);
} spin_lock_t;

int init_spin_lock(struct spin_lock *lk);

#endif