#include "types.h"
#include "console.h"
#include "riscv.h"


void kernel_main(void) 
{
    u64 cpu_id = r_tp();
    if (cpu_id == 0)
        printf("[CPU%d] Boot Successfully. Now it's in kernel main function.\n", cpu_id);

    for (;;)
        ;
}


