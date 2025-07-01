#include "mm.h"
#include "spin_lock.h"
#include "mem_layout.h"
#include "param.h"
#include "console.h"
#include "string.h"
#include "riscv.h"

extern char end[];

struct {
    struct list_head head;
    spin_lock_t lk;    
} g_kmem;


// void *kalloc()
// {
//     return page_alloc(1);
// }

// void kfree_range(void *start, int n)
// {
//     char *addr = (char*) PGROUNDUP((u64)start);
//     // for (; addr <= _end; addr += PG_SZ)
//     //     kfree(addr, 1);
//     page_free((void*) addr, n);
// }

void *kalloc()
{
    struct list_head *r;
    g_kmem.lk.acquire(&g_kmem.lk);
    if (list_empty(&g_kmem.head)) {
        r = NULL;
        goto out;
    }
    r = g_kmem.head.next;
    r->next->prev = &g_kmem.head;
    g_kmem.head.next = r->next;

out:
    g_kmem.lk.release(&g_kmem.lk);
    return (void*)r;
}

void kfree(void *pa)
{
    struct list_head *r;

    if(((u64)pa % PGSIZE) != 0 || (char*)pa < end || (u64)pa >= PA_STOP)
        PANIC("kfree");

    // Fill with junk to catch dangling refs.
    memset(pa, 6, PGSIZE);

    r = (struct list_head*)pa;

    g_kmem.lk.acquire(&g_kmem.lk);
    list_add_tail(r, &g_kmem.head);
    g_kmem.lk.release(&g_kmem.lk);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*) PGROUNDUP((u64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// void _kfree(void *addr, int n)
// {
//     u64 pa = (u64) addr;
//     struct list_head *node;
//     if ((pa % PG_SZ) != 0 || (char*)pa < end || pa >= PA_STOP)
//         PANIC("kfree");
    
//     if (n == 1) {
//         memset(addr, 6, PG_SZ);
//         node = (struct list_head*) pa;

//         g_kmem.lk.acquire(&g_kmem.lk);
//         list_add(node, &g_kmem.head);
//         g_kmem.lk.release(&g_kmem.lk);
//     } else {
//         page_free(addr, n);
//     }
// }

void kinit_pa()
{
    init_spin_lock(&g_kmem.lk);
    INIT_LIST_HEAD(&g_kmem.head);
    freerange(end, (void*) PA_STOP);   // CHECK HERE -> ERROR
}

static void* link_pages_internal(void *addr, int n)
{
    u64 prev = (u64) addr, cur = prev + PG_SZ;
    struct list_head *node = NULL;

    for (int i = 1; i < n; i++) {
        node = (struct list_head*) cur;
        node->prev = (struct list_head*) prev;
        ((struct list_head*) prev)->next = node;
        node->next = (struct list_head*) (cur + PG_SZ);
        prev = cur;
        cur += PG_SZ;
    }
    return (void*) prev;
}

/**
 *  Buddy Allocator allocate power of 2 page
 *  @n: page numbers
 *  
 *  
 */
void *page_alloc(int n)
{
    if (n <= 0)
        return NULL;

    int pages = 1;
    while (pages < n)
        pages <<= 1;

    int pg_cnt = 0;
    struct list_head *cur = NULL, *start = NULL, *last = NULL, *next = NULL;

    g_kmem.lk.acquire(&g_kmem.lk);
    if (list_empty(&g_kmem.head))
        goto out;

    cur = g_kmem.head.next;
    start = cur;
    last = cur;
    
    ++pg_cnt;
    while (pg_cnt != pages && cur->next != &g_kmem.head) {
        next = cur->next;
        if (((char*)cur) + PG_SZ == (char*)(next)) {
            ++pg_cnt;
            last = next;
        } else {
            pg_cnt = 1;
            start = next;
        }
        cur = next;
    }

    if (pg_cnt != pages) {
        start = NULL;
        goto out;
    }

    start->prev->next = last->next;
    last->next->prev = start->prev;
out:
    g_kmem.lk.release(&g_kmem.lk);
    return (void*) start;
}

/**
 *  Buddy Allocator free page allocate from page_alloc function
 * @addr: allocated page by page_alloc 
 * @n: page numbers
 *  
 *  
 */
void page_free(void *addr, int n)
{
    if (n < 0)
        return;

    //memset(addr, 6, n * PG_SZ);
    void *last_page = link_pages_internal(addr, n);
    
    // struct list_head *prev_next = NULL;
    g_kmem.lk.acquire(&g_kmem.lk);
    
    if (list_empty(&g_kmem.head)) {
        g_kmem.head.next = (struct list_head*) addr;
        ((struct list_head*) addr)->prev = &g_kmem.head;
        ((struct list_head*) last_page)->next = &g_kmem.head;
        g_kmem.head.prev = (struct list_head*) last_page;
        goto out;
    }

    struct list_head *first = g_kmem.head.next;
    while ((u64)first->next != (u64)addr && (u64) first->next < (u64)addr)
        first = first->next;
    
    first->next->prev = (struct list_head*) last_page;
    ((struct list_head*) last_page)->next = first->next;

    first->next = (struct list_head*) addr;
    ((struct list_head*) addr)->prev = first;
out:
    g_kmem.lk.release(&g_kmem.lk);
}