#ifndef _CONSOLE_H
#define _CONSOLE_H


#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg


void printf(const char *str, ...);
void puts(char *s);


#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("[PANIC]: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)


#define PRINTK(fmt, ...)                                                       \
    do {                                                                       \
        printf(""fmt"\n", ##__VA_ARGS__);                                      \
    } while(0)                                                                 \


#endif