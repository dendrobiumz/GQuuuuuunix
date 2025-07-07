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
    char tmp[1024];
    int tmp_idx = 0;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++; // Skip '%'
            // Check for conversion specifiers
            if (*fmt == '\0') { // '%' at end of string
                uart_putchar('%');
                goto end;
            }
            if (*fmt == '%') { // "%%" prints '%'
                uart_putchar('%');
            } else if (*fmt == 's') { // String
                const char *s = va_arg(vargs, const char *);
                while (*s) {
                    uart_putchar(*s++);
                }
            } else if (*fmt == 'd') { // Decimal integer
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
            } else if (*fmt == 'x') { // Hexadecimal (32-bit)
                int value = va_arg(vargs, int);
                for (int i = 7; i >= 0; i--) {
                    int nibble = (value >> (i * 4)) & 0xf;
                    uart_putchar("0123456789abcdef"[nibble]);
                }
            } else if (*fmt == 'l') { // Could be "lx" or "llx"
                fmt++; // Move past the 'l'
                if (*fmt == 'x') {
                    // "lx" conversion (long in hex)
                    long value = va_arg(vargs, long);
                    // Assuming 64-bit long, print 16 hex digits
                    for (int i = 15; i >= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        uart_putchar("0123456789abcdef"[nibble]);
                    }
                } else if (*fmt == 'l') {
                    fmt++; // Move past the second 'l'
                    if (*fmt == 'x') {
                        // "llx" conversion (long long in hex)
                        long long value = va_arg(vargs, long long);
                        // Print 16 hex digits for a 64-bit value
                        for (int i = 15; i >= 0; i--) {
                            int nibble = (value >> (i * 4)) & 0xf;
                            uart_putchar("0123456789abcdef"[nibble]);
                        }
                    } else if (*fmt == 'd') {
                        long long value = va_arg(vargs, long long);

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
                    }
                } else {
                    // Handle error or fallback if needed.
                    // For now, just print the character.
                    uart_putchar('l');
                    continue;
                }
            } else if (*fmt == '0') {
                fmt++;
                int zero_padding_cnt = 0, i;
                int multiplend = 1;
                while (*fmt >= '0' && *fmt <= '9')
                    tmp[tmp_idx++] = *fmt++;
                
                for (i = 0; i < tmp_idx - 1; i++)
                    multiplend *= 10;

                for (i = 0; i < tmp_idx; i++) {
                    zero_padding_cnt += ((int)(tmp[i] - '0')) * multiplend;
                    multiplend /= 10;
                }



                tmp_idx = 0;
                if (*fmt == 'd') { // Decimal integer
                    int value = va_arg(vargs, int);
                    int number_decimal_digit = 0;
                    int decimal_value = value;
                    while (decimal_value) {
                        decimal_value /= 10;
                        ++number_decimal_digit;
                    }
                    for (i = 0; i < zero_padding_cnt - number_decimal_digit; i++)
                        uart_putchar('0');

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
                } else if (*fmt == 'x') { // Hexadecimal (32-bit)
                    int value = va_arg(vargs, int);
                    for (int i = 7; i >= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        uart_putchar("0123456789abcdef"[nibble]);
                    }
                } else if (*fmt == 'l') { // Could be "lx" or "llx"
                    fmt++; // Move past the 'l'
                    if (*fmt == 'x') {
                        // "lx" conversion (long in hex)
                        long value = va_arg(vargs, long);
                        // Assuming 64-bit long, print 16 hex digits
                        for (int i = 15; i >= 0; i--) {
                            int nibble = (value >> (i * 4)) & 0xf;
                            uart_putchar("0123456789abcdef"[nibble]);
                        }
                    } else if (*fmt == 'l') {
                        fmt++; // Move past the second 'l'
                        if (*fmt == 'x') {
                            // "llx" conversion (long long in hex)
                            long long value = va_arg(vargs, long long);
                            // Print 16 hex digits for a 64-bit value
                            for (int i = 15; i >= 0; i--) {
                                int nibble = (value >> (i * 4)) & 0xf;
                                uart_putchar("0123456789abcdef"[nibble]);
                            }
                        } else if (*fmt == 'd') {
                            long long value = va_arg(vargs, long long);
                            int number_decimal_digit = 0;
                            long long  decimal_value = value;
                            while (decimal_value) {
                                decimal_value /= 10;
                                ++number_decimal_digit;
                            }
                            for (i = 0; i < zero_padding_cnt - number_decimal_digit; i++)
                                uart_putchar('0');
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
                        }
                    }
                } else {
                    // Unrecognized specifier – you might want to print it literally
                    uart_putchar(*fmt);
                }
            }

            // } else if (*fmt > '0' && *fmt <= '9') {
                 
            // }
            else {
                // Unrecognized specifier – you might want to print it literally
                uart_putchar(*fmt);
            }
        } else {
            uart_putchar(*fmt);
        }
        fmt++;
    }

end:
    va_end(vargs);
}