#include "sched.h"
#include "types.h"
#include "riscv.h"
#include "param.h"
#include "task.h"
#include "console.h"
#include "string.h"



struct task_struct gTasks[TASK_MAX_NRS];
u8 task_stack[TASK_MAX_NRS][STACK_SIZE];
i64 taskNrs = 0;


void addTask(struct task_struct *task)
{
    memcpy(&gTasks[taskNrs++ % TASK_MAX_NRS], task, sizeof(struct task_struct));
}


void scheduler()
{
    struct cpu *cur_cpu = get_cur_cpu();
    struct task_struct *t;
    cur_cpu->task = NULL;

    for (;;) {
        intr_on();

        for (t = gTasks; t < &gTasks[TASK_MAX_NRS]; t++) {
            t->lk.acquire(&t->lk);
            if (t->state == RUNNABLE) {
                t->state = RUNNING;
                PRINTK("\t[SCHED] task %d\n", t->pid);
                cur_cpu->task = t;
                switch_to(&cur_cpu->context, &t->context);
                cur_cpu->task = NULL;
            }
            t->lk.release(&t->lk);
        }
    }
}

void sched()
{
    int intr_ena_before;
    struct task_struct *task = get_cur_task();

    if (!task->lk.is_holding(&task->lk))
        PANIC("LOCK NOT HOLDING");

    if(task->state == RUNNING)
        PANIC("TASK RUNNING");

    if(intr_get())
        PANIC("interruptible");

    intr_ena_before = get_cur_cpu()->intr_ena_before;
    switch_to(&task->context, &get_cur_cpu()->context);
    get_cur_cpu()->intr_ena_before = intr_ena_before;
}

void yield()
{
    struct task_struct *task = get_cur_task();
    task->lk.acquire(&task->lk);
    task->state = RUNNABLE;
    sched();
    task->lk.release(&task->lk);
}