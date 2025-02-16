#include "types.h"
#include "riscv.h"
#include "param.h"


__attribute__ ((aligned (16))) char kstack[STACK_SIZE * MAX_CPU_NUMS];

extern void kernel_main();

void start()
{
    u64 x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);
    
    w_mepc((u64)kernel_main);
    w_pmpcfg0(0xf);
    
    u64 id = r_mhartid();
    w_tp(id);
    asm volatile("mret");
}