#include "vm.h"
#include "mm.h"
#include "riscv.h"
#include "string.h"
#include "param.h"
#include "console.h"

pagetable_t kpgtbl = NULL;

void init_kpgtabl()
{
    kpgtbl = (pagetable_t) _kalloc();
    memset(kpgtbl, 0, PGSIZE);
}


void kvmmap()
{

}

pte_t* walk_pgtbl(pagetable_t pgtbl, u64 va, int alloc)
{
    int level = 0, offset = 30, mask = (1 - (1 << 10));
    pte_t *pte = 0;
    pagetable_t _pgtbl = pgtbl;

    // walking 3 levels
    // start from level 2
    for (int i = 0; i < 2; i++) {
        level = (va & (mask << offset)) >> offset;
        pte = &_pgtbl[level];
        if (*pte & PTE_V) {
            _pgtbl = ((*pte) >> 10) << 12; // 12: page size(4096 bytes) aligned 
        } else {
            if (alloc && (_pgtbl = _kalloc()) == 0)
                return NULL;
            memset(&_pgtbl, 0, PG_SZ);
            *pte = (((u64)_pgtbl >> 12) << 10) | PTE_V;
        }

        offset -= 9;
    }
    return &_pgtbl[(va & (mask << offset)) >> offset];
}

int map_pages(pagetable_t pgtbl, u64 va, i64 pg_nrs, u64 pa, int flag)
{
    u64 last_page = 0, cur = 0;
    pte_t *pte = NULL;

    if (va % PG_SZ)
        PANIC("[MAPPAGES] VA NOT ALIGNED.");
    
    if (pg_nrs == 0)
        PANIC("[PG_NRS] ZERO.");

    cur = va;
    last_page = va + (pg_nrs - 1) * PG_SZ;

    while (true) {
        pte = walk_pgtbl(pgtbl, va, 1);

        if (*pte & PTE_V)
            PANIC("[MAPPAGES] ALREADY MAPPED");
        *pte = ((pa >> 12) << 10) | flag | PTE_V;
        if (cur == last_page)
            break;
        cur += PG_SZ;
        pa += PG_SZ;
    }
    return 0;
}

