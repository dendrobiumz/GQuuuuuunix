#ifndef _MEM_LAYOUT_H
#define _MEM_LAYOUT_H

#define   UART_BASE               (0x10000000L)
#define   CLINT_BASE              (0x2000000L)

#define   CLINT_MTIME             (CLINT_BASE + 0xBFF8)
#define   CLINT_MTIMECMP(hartid)  (CLINT_BASE + 0x4000 + 8 * (hartid))

#define   KERNBASE                 0x80000000L
#define   PA_STOP                 (KERNBASE + 128*1024*1024)

#endif