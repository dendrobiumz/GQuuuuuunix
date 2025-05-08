#include "types.h"
#include "console.h"
#include "riscv.h"
#include "fixed_point.h"


fixed_point_t time;

void kernelvec();

void kernel_main(void) 
{
    u64 cpu_id = r_tp();
    w_stvec((u64)kernelvec);
    if (cpu_id == 0)
        printf("[CPU-%d] Boot Successfully. Now it's in kernel main function.\n\n", cpu_id);
    
    time.data = 0x0;
    time.mantissa_bit = 32;
    intr_on();
    for (;;)
        asm volatile("wfi");
}


