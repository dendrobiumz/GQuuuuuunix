#include "lfq.h"
#include "atomic.h"

static int push_back(struct lfq *self, struct list_head_a *node)
{
    if (self == NULL || node == NULL)
        return -1;

    atomic_ulong_t tail = self->head.prev;
    u64 desired = (u64) node, expected;

    do {
        expected = ATOMIC_LOAD(&tail);
        ATOMIC_STORE(&node->prev, expected);
        ATOMIC_STORE(&node->next, expected);
    } while (CMP_XCHG(&tail, expected, desired));

    ((struct list_head_a*) expected)->next.val = desired;
    ATOMIC_FETCH_ADD(&self->sz, 1);
    return 0;
}


static struct list_head_a* pop_front(struct lfq *self)
{
    if (self == NULL)
        return -1;

    atomic_ulong_t first = self->head.next;
    u64 expected, desired;

    do {
        expected = ATOMIC_LOAD(&first);
        if (expected == (u64) &self->head)
            return NULL;
        
        desired = ATOMIC_LOAD(&((struct list_head_a*)expected)->next);
    } while (CMP_XCHG(&first, expected, desired));

    ((struct list_head_a*) desired)->prev.val = (u64)&self->head;
    ATOMIC_FETCH_SUB(&self->sz, 1);
    return (struct list_head_a*)expected;
}


static int is_empty(struct lfq *self)
{
    return ATOMIC_LOAD(&self->sz) == 0;
}


void init_lfq(struct lfq *self)
{
    INIT_LIST_HEAD_A(&self->head);
    self->sz.val = 0;
    self->is_empty = is_empty;
    self->pop_front = pop_front;
    self->push_back = push_back;
}

