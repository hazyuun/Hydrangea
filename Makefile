
CC = i686-elf-gcc  
AS = i686-elf-as

KERNEL_SRC = $(wildcard kernel/*.c) $(wildcard kernel/vga/*.c) $(wildcard kernel/tty/*.c)

ker:
	$(AS) boot/boot.s -o boot.o
	$(CC) -c $(KERNEL_SRC) -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	$(CC) -T linker.ld -o bin/yuunos.bin -ffreestanding -O2 -nostdlib *.o -lgcc

clean:
	@rm -f *.o

