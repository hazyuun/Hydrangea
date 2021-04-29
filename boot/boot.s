.set ALIGN, 	1
.set MEMINFO,	2
.set VIDEO,		4
.set FLAGS, 	ALIGN | MEMINFO | VIDEO
.set MAGIC, 	0x1BADB002
.set CHECKSUM,  -(MAGIC+FLAGS)

.section .multiboot
.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	.long 0, 0, 0, 0, 0
	.long 0
	.long 1024, 768, 32

.section .bss
.align 16
stb:
	.skip 16384
stt:

.section .text
.global _start
_start:
	mov $stt, %esp
	push %ebx
	push %eax
	call kmain
  cli
loop:	hlt
	jmp loop

