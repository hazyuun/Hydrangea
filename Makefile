
CC = i686-elf-gcc  
AS = i686-elf-as

KERNEL_SRC_C = $(wildcard kernel/*.c) $(wildcard kernel/vga/*.c) $(wildcard kernel/tty/*.c) $(wildcard kernel/io/*.c) $(wildcard kernel/drivers/*.c)

KERNEL_SRC_S = $(wildcard kernel/*.s) $(wildcard kernel/vga/*.s) $(wildcard kernel/tty/*.s) $(wildcard kernel/io/*.s) $(wildcard kernel/drivers/*.s)

ker:
	$(AS) boot/boot.s -o boot.o
	$(CC) -c $(KERNEL_SRC_C) -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	$(CC) -T linker.ld -o bin/yuunos.bin -ffreestanding -O2 -nostdlib *.o -lgcc

clean:
	@rm -f *.o

