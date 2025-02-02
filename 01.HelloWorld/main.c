#include "types.h"
#include "console.h"
#include "riscv.h"


void kernel_main(void) 
{
    //memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    u64 cpu_id = r_tp();
    if (cpu_id == 0)
        printf("[CPU%d] Boot Successfully. Now it's in kernel main function.\n", cpu_id);

    for (;;)
        ;
}


