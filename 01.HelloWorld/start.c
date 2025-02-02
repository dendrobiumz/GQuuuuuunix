#include "types.h"
#include "riscv.h"
#include "param.h"


__attribute__ ((aligned (16))) char kstack[STACK_SIZE * MAX_CPU_NUMS];

extern void kernel_main();

void start()
{
    w_mepc((u64)kernel_main);
    w_pmpcfg0(0xf);
    
    u64 id = r_mhartid();
    w_tp(id);
    asm volatile("mret");
}