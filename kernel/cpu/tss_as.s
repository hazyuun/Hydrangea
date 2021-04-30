.section .text
.align 4

.global tss_load
tss_load:
	mov 4(%esp), %ax
	ltr %ax
	ret
