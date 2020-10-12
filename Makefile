CC = i686-elf-gcc
AS = i686-elf-as

YUUNOS = bin/yuunos.bin
OBJ_PATH = ./obj

# Finding source files
BOOT_SRC		= $(shell find boot -name "*.s")
KERNEL_SRC_C	= $(shell find kernel -name "*.c")
KERNEL_SRC_S	= $(shell find kernel -name "*.s")
LIBK_SRC		= $(shell find libk -name "*.c")

# Generating .o paths
BOOT_OBJ__ = $(patsubst %.s, %.o, $(BOOT_SRC))
KERNEL_OBJ_C__ = $(patsubst %.c, %.o, $(KERNEL_SRC_C))
KERNEL_OBJ_S__ = $(patsubst %.s, %.o, $(KERNEL_SRC_S))
LIBK_OBJ__ = $(patsubst %.c, %.o, $(LIBK_SRC))

BOOT_OBJ = $(addprefix $(OBJ_PATH)/, $(BOOT_OBJ__))
KERNEL_OBJ_C = $(addprefix $(OBJ_PATH)/, $(KERNEL_OBJ_C__))
KERNEL_OBJ_S = $(addprefix $(OBJ_PATH)/, $(KERNEL_OBJ_S__))
LIBK_OBJ = $(addprefix $(OBJ_PATH)/, $(LIBK_OBJ__))
KERNEL_OBJECTS = $(BOOT_OBJ) $(KERNEL_OBJ_C) $(KERNEL_OBJ_S) $(LIBK_OBJ)


# Creates the obj directories
.PHONY: dirs
dirs:
	$(shell mkdir --parents $(dir $(KERNEL_OBJECTS)))

# Builds the kernel
.PHONY: kernel
kernel: dirs $(YUUNOS)

$(YUUNOS): $(BOOT_OBJ) $(KERNEL_OBJ_C) $(KERNEL_OBJ_S) $(LIBK_OBJ)
	@$(shell mkdir --parents $(dir $(YUUNOS)))
	@echo Linking everything into $@
	@$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

$(OBJ_PATH)/kernel/%.o: kernel/%.c
	@echo [CC] $<
	@$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Ikernel -I. -Ilibk

$(OBJ_PATH)/libk/%.o: libk/%.c
	@echo [CC] $<
	@$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Ikernel -I. -Ilibk

$(OBJ_PATH)/kernel/%.o: kernel/%.s
	@echo [AS] $<
	@$(AS) $< -o $@

$(OBJ_PATH)/boot/%.o: boot/%.s
	@echo [AS] $<
	@$(AS) $< -o $@

# Deletes the obj directory
clean:
	@rm -rf $(OBJ_PATH)

