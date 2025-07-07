#ifndef _TASK_STRUCT_H
#define _TASK_STRUCT_H

#include "types.h"
#include "spin_lock.h"
#include "riscv.h"
#include "param.h"

enum taskstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };



struct __attribute__ ((aligned (8))) context {
    u64 ra;
    u64 sp;
  
    // callee-saved
    u64 s[12]; //s0 - s11
};

void init_context(struct context *ctx);


struct __attribute__ ((aligned (8))) task_struct {
    //char name[64];
    int pid;
    struct context context;
    enum taskstate state;
    spin_lock_t lk;
};

void init_task(struct task_struct *task, const int pid);

struct __attribute__ ((aligned (8))) cpu {
    struct task_struct *task;
    struct context context;
    int off_nrs;
    int intr_ena_before;
};

void init_cpu(struct cpu *cpu);

// extern struct task_struct gTasks[TASK_MAX_NRS];

int get_cpuid();

struct cpu* get_cur_cpu();

struct task_struct* get_cur_task();

void push_off();
void pop_off();

#endif