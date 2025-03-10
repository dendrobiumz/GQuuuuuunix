#include "types.h"
#include "console.h"
#include "riscv.h"

void kernelvec();

void kernel_main(void) 
{
    u64 cpu_id = r_tp();
    w_stvec((u64)kernelvec);
    if (cpu_id == 0)
        printf("[CPU-%d] Boot Successfully. Now it's in kernel main function.\n\n", cpu_id);
    
    intr_on();
    for (;;)
        asm volatile("wfi");
}


