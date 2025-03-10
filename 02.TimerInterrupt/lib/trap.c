#include "types.h"
#include "riscv.h"
#include "console.h"

u64 gticks = 0;

void kernel_trap_handler()
{
    u64 sepc = r_sepc();
    u64 sstatus = r_sstatus();
    u64 scause = r_scause();
    switch(scause) {
    case 0x8000000000000001L:
        u64 hartid = r_tp();
        if (hartid == 0) {
            gticks++;
            printf("[%8llx] tick tick tick...\n", gticks);
        }
    } 
    w_sepc(sepc);
    w_sstatus(sstatus);
}
