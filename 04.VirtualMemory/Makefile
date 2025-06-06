CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -Iinclude -fno-builtin -mcmodel=medany -march=rv64g -mabi=lp64 -g -std=gnu11
CFLAGS += -ffreestanding -fno-common -mno-relax
CFLAGS += -O0 -g3 -ggdb -fno-omit-frame-pointer -Wall -Wextra -Werror

LDFLAGS = -T kernel.ld
CORE = 2

QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp $(CORE) -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump
GDB = riscv64-unknown-elf-gdb



all: kernel.elf

kernel.elf: lib/string.c lib/task.c lib/switch.s lib/spin_lock.c lib/sched.c entry.s lib/console.c lib/trap.c lib/kvec.s start.c  main.c
	$(CC) $(CFLAGS) -T kernel.ld -o kernel.elf $^


qemu: $(TARGET)
	@qemu-system-riscv64 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel kernel.elf

debug:
	@qemu-system-riscv64 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel kernel.elf -s -S


clean:
	rm -f *.elf