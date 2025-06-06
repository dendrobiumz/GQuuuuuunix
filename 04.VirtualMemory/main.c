#include "types.h"
#include "console.h"
#include "riscv.h"
#include "fixed_point.h"
#include "sched.h"
#include "task.h"
#include "atomic.h"

fixed_point_t time;

static struct task_struct task1, task2, task3, task4, task5;

extern struct task_struct gTasks[TASK_MAX_NRS];

static atomic_int_t start;

void kernelvec();

void kernel_main(void) 
{
    u64 cpu_id = r_tp();
    
    start.val = 0;
    w_stvec((u64)kernelvec);
    if (cpu_id == 0) {
        printf("GQuuuuuunix Kernel Booting ... \n\n");
        time.data = 0x0;
        time.mantissa_bit = 32;

        for (int i = 0; i < TASK_MAX_NRS; i++)
            init_spin_lock(&gTasks[i].lk);

        
        init_task(&task1, 0);
        addTask(&task1);

        init_task(&task2, 1);
        addTask(&task2);

        init_task(&task3, 2);
        addTask(&task3);

        init_task(&task4, 3);
        addTask(&task4);

        init_task(&task5, 4);
        addTask(&task5);

        printf("GQuuuuuunix Boot Successfully ... \n\n", cpu_id);
        ATOMIC_STORE(&start, 1);
    } else {
        while (ATOMIC_LOAD(&start) == 0)
            ;
    }

    scheduler();
}


