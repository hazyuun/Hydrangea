CC = i686-elf-gcc
AS = i686-elf-as
AR = i686-elf-ar

CCFLAGS   = -g -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra
CCLDFLAGS = -ffreestanding -O2 -nostdlib -lgcc -mno-red-zone
ASFLAGS   = --32
INCLUDE   = -Iinclude -I.

DIR_LIB    = ./lib
DIR_OBJ    = ./obj

TARGET = $(DIR_LIB)/libc.a

SRC_C    = $(shell find . -name "*.s")
SRC_S    = $(shell find . -name "*.c")

OBJ_C      = $(patsubst ./%.s, ./obj/%.o, $(SRC_C))
OBJ_S      = $(patsubst ./%.c, ./obj/%.o, $(SRC_S))

OBJ = $(OBJ_C) $(OBJ_S)

.PHONY: all
all: dirs $(OBJ)
	@echo "Making $(TARGET)"
	@$(AR) -crs $(TARGET) $(OBJ)

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJ)) $(DIR_LIB)

obj/%.o: %.c
	@echo [CC] $@
	@$(CC) -c $< -o $@ $(CCFLAGS) $(INCLUDE)

obj/%.o: %.s
	@echo [AS] $@
	@$(AS) $< -o $@ $(ASFLAGS)

.PHONY: clean
clean:
	@rm -rf obj
