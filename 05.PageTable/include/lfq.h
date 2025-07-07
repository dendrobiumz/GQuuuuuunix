#ifndef LFQ_H
#define LFQ_H

#include "types.h"
#include "list.h"

typedef struct lfq {
    struct list_head_a head;
    atomic_long_t sz;
    int (*push_back)(struct lfq *, struct list_head_a *);
    struct list_head_a * (*pop_front)(struct lfq *);
    int (*is_empty)(struct lfq*);
} lfq_t;


int init_lfq(struct lfq *);

#endif