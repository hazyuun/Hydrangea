.set ALIGN, 	1
.set MEMINFO,	2
.set FLAGS, 	ALIGN | MEMINFO
.set MAGIC, 	0x1BADB002
.set CHECKSUM,  -(MAGIC+FLAGS)

.section .multiboot
.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.section .bss
.align 16
stb:
	.skip 16384
stt:

.section .text
.global _start
_start:
	mov $stt, %esp
	call kmain
    cli
loop:	hlt
	jmp loop

