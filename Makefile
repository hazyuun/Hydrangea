CC = i686-elf-gcc
AS = i686-elf-as

CCFLAGS   = -g -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra
CCLDFLAGS = -ffreestanding -O2 -nostdlib -lgcc -mno-red-zone
ASFLAGS   = --32
INCLUDE   = -Ikernel -I. -Ilibk

DIR_ISO    = ./iso
DIR_INITRD = ./initrd

TARGET = ./bin/yuunos.bin
ISO    = $(DIR_ISO)/yuunos.iso
INITRD = $(DIR_ISO)/boot/initrd.tar

SRC_BOOT      = $(shell find ./boot -name "*.s")
SRC_KERNEL_C	= $(shell find ./kernel -name "*.c")
SRC_KERNEL_S	= $(shell find ./kernel -name "*.s")
SRC_LIBK      = $(shell find ./libk -name "*.c")

OBJ_BOOT      = $(patsubst ./boot/%.s, ./obj/boot/%.o, $(SRC_BOOT))
OBJ_KERNEL_C  = $(patsubst ./kernel/%.c, ./obj/kernel/%.o, $(SRC_KERNEL_C))
OBJ_KERNEL_S  = $(patsubst ./kernel/%.s, ./obj/kernel/%.o, $(SRC_KERNEL_S))
OBJ_LIBK      = $(patsubst ./libk/%.c, ./obj/libk/%.o, $(SRC_LIBK))

OBJ = $(OBJ_BOOT) $(OBJ_KERNEL_C) $(OBJ_KERNEL_S) $(OBJ_LIBK)

.PHONY: kernel
kernel: dirs $(OBJ)
	@echo "Linking everything into $(TARGET)"
	@$(CC) -T linker.ld $(OBJ) -o $(TARGET) $(CCLDFLAGS)

.PHONY: iso
iso: kernel
	@echo "Making initrd"
	@tar -cf $(INITRD) $(DIR_INITRD)
	@echo "Making the iso"
	@rm -f $(ISO)
	@cp $(TARGET) $(DIR_ISO)/boot
	@grub-mkrescue -o $(ISO) $(DIR_ISO)
	@mv $(ISO) .

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJ)) bin

obj/%.o: %.c
	@echo [CC] $@
	@$(CC) -c $< -o $@ $(CCFLAGS) $(INCLUDE)

obj/%.o: %.s
	@echo [AS] $@
	@$(AS) $< -o $@ $(ASFLAGS)

.PHONY: clean
clean:
	@rm -rf obj

.PHONY: clean_all
clean_all:
	@rm -rf obj bin $(INITRD) $(ISO)
