#include "spin_lock.h"
#include "atomic.h"
#include "sched.h"
#include "console.h"
#include "task.h"

static void acquire_lk(struct spin_lock *lk)
{
    push_off();
    if (lk->is_holding(lk))
        PANIC("spin lock acquire is holding");
    while (TEST_SET(&lk->isLocked) != 0)
        ;
        
    smp_mb();
    lk->cpu = get_cur_cpu();
}

static void release_lk(struct spin_lock *lk)
{
    if (!lk->is_holding(lk))
        PANIC("spin lock release not holding");
    
    lk->cpu = NULL;
    smp_mb();
    ATOMIC_STORE(&lk->isLocked, 0);
    pop_off();
}

static int is_holding_lk(struct spin_lock *lk)
{
    int r = lk->isLocked.val && get_cur_cpu() == lk->cpu;
    return r;
}

int init_spin_lock(struct spin_lock *lk)
{
    lk->isLocked.val = 0;
    lk->cpu = NULL;
    lk->acquire = acquire_lk;
    lk->release = release_lk;
    lk->is_holding = is_holding_lk;
    return 0;
}