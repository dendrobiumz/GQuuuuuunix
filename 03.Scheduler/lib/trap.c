#include "types.h"
#include "riscv.h"
#include "console.h"
#include "fixed_point.h"

#define TICKS 0x3FFFF

u64 gticks = 0;
extern fixed_point_t time;


static int get_time_decimal(fixed_point_t fp, u32 *fraction)
{
    u64 mantissa = fp.data & ((1ULL << fp.mantissa_bit) - 1);
    u64 denom = 1ULL << fp.mantissa_bit;

    u32 decimal = 0;
    u32 factor = 100000;  // For 4 decimal digits

    mantissa *= factor;
    decimal = mantissa / denom;

    *fraction = decimal;
    return 0;
}

void kernel_trap_handler()
{
    u64 sepc = r_sepc();
    u64 sstatus = r_sstatus();
    u64 scause = r_scause();
    u32 fraction_decimal = 0;
    switch(scause) {
    case 0x8000000000000001L:
        u64 hartid = r_tp();
        if (hartid == 0) {
            gticks++;
            time.data += TICKS;
            get_time_decimal(time, &fraction_decimal);
            printf("[%05lld.%05d] tick tick tick...\n", time.data >> time.mantissa_bit, fraction_decimal);
        }
    } 
    w_sepc(sepc);
    w_sstatus(sstatus);
}
