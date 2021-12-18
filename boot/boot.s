.set ALIGN, 	1
.set MEMINFO,	2
.set VIDEO,		4
.set FLAGS, 	ALIGN | MEMINFO //| VIDEO
.set MAGIC, 	0x1BADB002
.set CHECKSUM,  -(MAGIC+FLAGS)

.set HH_ADDR, 0xC0000000
.set HH_PGDIR_INDEX,  (HH_ADDR >> 22)

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

/* I guess I'll use 4MiB pages cuz I don't wanna deal with page tables hell */

.section .data
.global boot_pgdir
boot_pgdir:
	.align 4096

/* Well, I guess 12 MiB for the kernel is very enough for now, */
/* I may map more pages later if the kernel grows bigger       */

/* Ok, let's identity map the first 12 MiB = 3 * 4 MiB      */
	.long 0x00000083
	.long 0x00400083
	.long 0x00800083

/* Fill with zeros until we reach the page of 0xc0000000 */
/* Note : the -3 is because we already have 3 entries above */
	.fill (HH_PGDIR_INDEX - 3), 4, 0

/* Now, map 12 MiB to the first 12 MiB */
	.long 0x00000083
	.long 0x00400083
	.long 0x00800083

/* And fill the remaining pages with zeros */
	.fill (1024 - HH_PGDIR_INDEX - 3), 4, 0

.section .text
.global _start
_start:
	/* Put the page directory address in cr3 */
	mov $(boot_pgdir - HH_ADDR), %edx
	mov %edx, %cr3

	/* Set PSE bit in cr3 */
	mov %cr4, %edx
	or $0x00000010, %edx
	mov %edx, %cr4

	/* Enable paging ! (hopefully it won't page fault) */
	mov %cr0, %edx
	or $0x80000000, %edx
	mov %edx, %cr0

	/* Jump to higher half */
	lea the_real_start, %edx
	jmp *%edx

the_real_start:
	mov $stt, %esp
	push %ebx
	push %eax
	call kmain
	cli

loop:	hlt
	jmp loop

