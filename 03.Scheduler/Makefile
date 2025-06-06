CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -Iinclude -fno-builtin -mcmodel=medany -march=rv64g -mabi=lp64 -g
# CFLAGS += -ffreestanding -fno-common -mno-relax
# CFLAGS += -Wall -Wextra

LDFLAGS = -T kernel.ld
CORE = 1

QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp $(CORE) -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump
GDB = riscv64-unknown-elf-gdb



all: kernel.elf

kernel.elf: entry.s lib/console.c lib/trap.c lib/kvec.s start.c  main.c
	$(CC) $(CFLAGS) -T kernel.ld -o kernel.elf $^


qemu: $(TARGET)
	@qemu-system-riscv64 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel kernel.elf

debug:
	$(GDB) kernel.elf


clean:
	rm -f *.elf