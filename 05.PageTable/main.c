#include "types.h"
#include "console.h"
#include "riscv.h"
#include "fixed_point.h"
#include "sched.h"
#include "task.h"
#include "atomic.h"
#include "mm.h"

fixed_point_t time;


extern struct task_struct gTasks[TASK_MAX_NRS];

static atomic_int_t start;

void kernelvec();

void kernel_main(void) 
{
    u64 cpu_id = r_tp();
    
    start.val = 0;
    w_stvec((u64)kernelvec);
    if (cpu_id == 0) {
        kinit_pa();
        printf("GQuuuuuunix Kernel Booting ... \n\n");
        time.data = 0x0;
        time.mantissa_bit = 32;

        for (int i = 0; i < TASK_MAX_NRS; i++)
            init_spin_lock(&gTasks[i].lk);

        struct task_struct *task1, *task2, *task3, *task4, *task5;
        task1 = (struct task_struct*) _kalloc();
        init_task(task1, 0);
        addTask(task1);
        
        task2 = (struct task_struct*) _kalloc();
        init_task(task2, 1);
        addTask(task2);
        
        task3 = (struct task_struct*) _kalloc();
        init_task(task3, 2);
        addTask(task3);

        task4 = (struct task_struct*) _kalloc();
        init_task(task4, 3);
        addTask(task4);
        
        task5 = (struct task_struct*) _kalloc();
        init_task(task5, 4);
        addTask(task5);

        printf("GQuuuuuunix Boot Successfully ... \n\n", cpu_id);
        ATOMIC_STORE(&start, 1);
    } else {
        void *p = page_alloc(8 * cpu_id);
        while (ATOMIC_LOAD(&start) == 0)
            ;
        page_free(p, 8 * cpu_id);
    }

    scheduler();
}


