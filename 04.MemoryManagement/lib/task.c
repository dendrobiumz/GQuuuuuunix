#include "task.h"
#include "string.h"
#include "console.h"
#include "sched.h"

extern u8 task_stack[TASK_MAX_NRS][STACK_SIZE];
extern i64 taskNrs;

struct cpu gCpus[MAX_CPU_NUMS];

static void dummy_task() {
    PRINTK("task running...\n");
    struct task_struct *t = get_cur_task();    
    t->lk.release(&t->lk);  // simulating process fork + exec -> forkret
    while (true) {      
        intr_on();  // simulating when s-mode enter u-mode, sret will turn on timer intr
        PRINTK("run ...\n");
        for (volatile int i = 0; i < 10000000; ++i)
            ;        
        PRINTK("\trun run ...\n");
    }
}

void init_context(struct context *ctx)
{
    ctx->s[0] = 0;
    ctx->s[1] = 0;
    ctx->s[2] = 0;
    ctx->s[3] = 0;
    ctx->s[4] = 0;
    ctx->s[5] = 0;
    ctx->s[6] = 0;
    ctx->s[7] = 0;
    ctx->s[8] = 0;
    ctx->s[9] = 0;
    ctx->s[10] = 0;
    ctx->s[11] = 0;

    ctx->ra = 0;
    ctx->sp = 0;
}

void init_task(struct task_struct *task, const int pid)
{
    task->state = RUNNABLE;
    task->pid = pid;
    task->context.ra = (u64)dummy_task;
    task->context.sp = (u64) (task_stack[taskNrs % TASK_MAX_NRS] + STACK_SIZE);
    init_spin_lock(&task->lk);
}

void init_cpu(struct cpu *cpu)
{
    init_context(&cpu->context);
    cpu->task = NULL;
}

int get_cpuid()
{
  int id = r_tp();
  return id;
}

struct cpu* get_cur_cpu()
{
    int id = get_cpuid();
    struct cpu *cpu = &gCpus[id];
    return cpu;
}

struct task_struct* get_cur_task()
{
    struct cpu *cur_cpu = get_cur_cpu();
    struct task_struct *task = cur_cpu->task;
    return task;
}

void push_off(void)
{
  int old = intr_get();

  intr_off();
  if(get_cur_cpu()->off_nrs == 0)
    get_cur_cpu()->intr_ena_before = old;
  get_cur_cpu()->off_nrs += 1;
}

void pop_off(void)
{
    struct cpu *c = get_cur_cpu();
    if(intr_get())
        PANIC("pop_off - interruptible");
    if(c->off_nrs < 1)
        PANIC("pop_off - nrs < 1");
    c->off_nrs -= 1;
    if(c->off_nrs == 0 && c->intr_ena_before)
        intr_on();
}