#include "types.h"
#include "riscv.h"
#include "param.h"
#include "mem_layout.h"


__attribute__ ((aligned (16))) char kstack[STACK_SIZE * MAX_CPU_NUMS];
u64 timer_scratch[MAX_CPU_NUMS][5];

extern void kernel_main();
extern void timer_vec();


void init_timer()
{
    u64 hart_id = r_mhartid();

    int interval = 10000;
    *(u64*) CLINT_MTIMECMP(hart_id) = *(u64*) CLINT_MTIME + interval;

    u64 *per_hart_scratch = &timer_scratch[hart_id][0];
    per_hart_scratch[3] = CLINT_MTIMECMP(hart_id);
    per_hart_scratch[4] = interval;
    w_mscratch((u64)per_hart_scratch);

    w_mtvec((u64)timer_vec);
    w_mstatus(r_mstatus() | MSTATUS_MIE);
    w_mie(r_mie() | MIE_MTIE);
}


void start()
{
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);

    w_mepc((u64)kernel_main);
    w_satp(0);

    w_medeleg(0xffff);
    w_mideleg(0xffff); // MTI can not delegate to S-mode, still exec mtvec for MTI first
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf); // PMP0: RWX, TOR mode (A=01), 01111

    init_timer();
    
    int id = r_mhartid();
    w_tp(id);
    asm volatile("mret");
}
