#include "spin_lock.h"
#include "atomic_ops.h"
#include "sched.h"
#include "console.h"

static void acquire_lk(struct spin_lock *lk)
{
    intr_off();
    if (!lk->is_holding(lk))
        PANIC("[PANIC] spin lock acquire");
    while(TEST_SET(&lk->isLocked))
        ;
    smp_mb();
    lk->cpu = get_cur_cpu();
}

static void release_lk(struct spin_lock *lk)
{
    if (!lk->is_holding(lk))
        PANIC("[PANIC] spin lock release");
    
    lk->cpu = NULL;
    smp_mb();
    lk->isLocked = 0;
    intr_on();
}

static int is_holding_lk(struct spin_lock *lk)
{
    if (get_cur_cpu() == lk->cpu)
        return 1;
    return 0;
}

int init_spin_lock(struct spin_lock *lk)
{
    lk->isLocked = 0;
    lk->cpu = NULL;
    lk->acquire = acquire_lk;
    lk->release = release_lk;
    lk->is_holding = is_holding_lk;
    return 0;
}