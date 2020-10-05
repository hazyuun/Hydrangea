
CC = i686-elf-gcc  
AS = i686-elf-as

KERNEL_SRC_C = $(wildcard kernel/*.c) $(wildcard kernel/vga/*.c) $(wildcard kernel/tty/*.c) $(wildcard kernel/io/*.c) $(wildcard kernel/drivers/*.c) $(wildcard kernel/cpu/*.c) $(wildcard kernel/mem/*.c) $(wildcard kernel/ds/*.c)
LIBK_SRC = $(wildcard libk/*.c)
KERNEL_SRC_S = $(wildcard kernel/*.s) $(wildcard kernel/vga/*.s) $(wildcard kernel/tty/*.s) $(wildcard kernel/io/*.s) $(wildcard kernel/drivers/*.s) $(wildcard kernel/cpu/*.s)

ker:
	$(AS) boot/boot.s -o boot.o 
	$(AS) kernel/cpu/isr_as.s -o isr_as.o
	$(AS) kernel/cpu/gdt_as.s -o gdt_as.o
	$(CC) -c $(LIBK_SRC) $(KERNEL_SRC_C) -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Ikernel -I. -Ilibk
	$(CC) -T linker.ld -o bin/yuunos.bin -ffreestanding -O2 -nostdlib *.o -lgcc    

clean:
	@rm -f *.o

