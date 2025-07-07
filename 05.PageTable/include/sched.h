#ifndef _SCHED_H
#define _SCHED_H

struct task_struct;
struct context;

void scheduler();

void switch_to(struct context *run, struct context *sub);

void yield();
void sched();

void addTask(struct task_struct *task);

#endif