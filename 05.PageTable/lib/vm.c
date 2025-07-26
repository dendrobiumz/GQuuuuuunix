#include "vm.h"
#include "mm.h"
#include "riscv.h"
#include "string.h"
#include "param.h"
#include "console.h"
#include "mem_layout.h"

pagetable_t kpgtbl = NULL;


// Switch h/w page table register to the kernel's page table,
// and enable paging.
void kvminithart()
{
  // wait for any previous writes to the page table memory to finish.
  sfence_vma();

  w_satp(MAKE_SATP(kpgtbl));

  // flush stale entries from the TLB.
  sfence_vma();
}

void kvmmap()
{

}

pte_t* walk_pgtbl(pagetable_t pgtbl, u64 va, int alloc)
{
    int level = 0, offset = 30;
    u64 mask = ((1UL << 10) - 1);
    pte_t *pte = 0;
    // pagetable_t _pgtbl = pgtbl;

    // walking 3 levels
    // start from level 2
    for (int i = 0; i < 2; i++) {
        level = (va & (mask << offset)) >> offset;
        pte = &pgtbl[level];
        if (*pte & PTE_V) {
            pgtbl = (pagetable_t)(((*pte) >> 10) << 12); // 12: page size(4096 bytes) aligned 
        } else {
            if (alloc && (pgtbl = kalloc()) == 0)
                return NULL;
            memset(pgtbl, 0, PG_SZ);
            *pte = (((u64)pgtbl >> 12) << 10) | PTE_V;
        }

        offset -= 9;
    }
    return &pgtbl[(va & (mask << offset)) >> offset];
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
        if ((pte = walk_pgtbl(pgtbl, cur, 1)) == 0)
            return -1;

        if (*pte & PTE_V)
            PANIC("[MAPPAGES] PTE ALREADY MAPPED");
        *pte = ((pa >> 12) << 10) | flag | PTE_V;
        if (cur == last_page)
            break;
        cur += PG_SZ;
        pa += PG_SZ;
    }
    return 0;
}

void init_kpgtabl()
{
    kpgtbl = (pagetable_t) _kalloc();
    memset(kpgtbl, 0, PGSIZE);
    map_pages(kpgtbl, UART_BASE, 1, UART_BASE, PTE_R | PTE_W);
    map_pages(kpgtbl, KERNBASE, PA_PAGE_NRS, KERNBASE,  PTE_R | PTE_W);
}

