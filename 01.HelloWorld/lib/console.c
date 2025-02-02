#include   "console.h"
#include   "mem_layout.h"


#define   THR                        (0x0)
#define   LSR                        (0x5)

#define   RBR                        (0x0)

#define   TX_READY                   (0x40)

#define   UART_REG(reg)              ((volatile unsigned char *)(UART_BASE + reg))
#define   READ_UART_REG(reg)         (*UART_REG(reg))
#define   WRITE_UART_REG(reg, val)   (*UART_REG(reg) = val)



static void uart_putchar(char c)
{
    while ((READ_UART_REG(LSR) & TX_READY) == 0)
        ;
    WRITE_UART_REG(THR, c);
}


void puts(char *s)
{
    while (*s)
        uart_putchar(*s++);
}


void printf(const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++; // Skip '%'
            switch (*fmt) { // Read the next character
                case '\0': // '%' at the end of the format string
                    uart_putchar('%');
                    goto end;
                case '%': // Print '%'
                    uart_putchar('%');
                    break;
                case 's': { // Print a NULL-terminated string.
                    const char *s = va_arg(vargs, const char *);
                    while (*s) {
                        uart_putchar(*s);
                        s++;
                    }
                    break;
                }
                case 'd': { // Print an integer in decimal.
                    int value = va_arg(vargs, int);
                    if (value < 0) {
                        uart_putchar('-');
                        value = -value;
                    }

                    int divisor = 1;
                    while (value / divisor > 9)
                        divisor *= 10;

                    while (divisor > 0) {
                        uart_putchar('0' + value / divisor);
                        value %= divisor;
                        divisor /= 10;
                    }

                    break;
                }
                case 'x': { // Print an integer in hexadecimal.
                    int value = va_arg(vargs, int);
                    for (int i = 7; i >= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        uart_putchar("0123456789abcdef"[nibble]);
                    }
                }
            }
        } else {
            uart_putchar(*fmt);
        }

        fmt++;
    }

end:
    va_end(vargs);
}