	.file	"spinwait.c"
	.text
	.globl	barrier
	.type	barrier, @function
barrier:
.LFB40:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	leaq	lock(%rip), %rdi
	call	pthread_mutex_lock@PLT
	movl	state(%rip), %ebx
	movl	waiting(%rip), %eax
	cmpl	$7, %eax
	je	.L2
	leal	1(%rax), %edx
	movl	%edx, waiting(%rip)
.L3:
	leaq	lock(%rip), %rdi
	call	pthread_mutex_unlock@PLT
.L4:
	movl	state(%rip), %eax
	cmpl	%ebx, %eax
	je	.L4
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L2:
	.cfi_restore_state
	movl	$0, waiting(%rip)
	movl	$1, %eax
	subl	%ebx, %eax
	movl	%eax, state(%rip)
	jmp	.L3
	.cfi_endproc
.LFE40:
	.size	barrier, .-barrier
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hello World! %ld\n"
.LC1:
	.string	"Bye Bye World! %ld\n"
	.text
	.globl	HelloWorld
	.type	HelloWorld, @function
HelloWorld:
.LFB41:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rdi, %rdx
	leaq	.LC0(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movl	$0, %eax
	call	barrier
	movq	%rbx, %rdx
	leaq	.LC1(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movl	$0, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE41:
	.size	HelloWorld, .-HelloWorld
	.globl	main
	.type	main, @function
main:
.LFB42:
	.cfi_startproc
	endbr64
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$88, %rsp
	.cfi_def_cfa_offset 128
	movq	%fs:40, %rax
	movq	%rax, 72(%rsp)
	xorl	%eax, %eax
	movl	$0, %esi
	leaq	lock(%rip), %rdi
	call	pthread_mutex_init@PLT
	movq	%rsp, %rbx
	movq	%rbx, %r12
	movl	$0, %ebp
	leaq	HelloWorld(%rip), %r13
.L10:
	movq	%rbp, %rcx
	movq	%r13, %rdx
	movl	$0, %esi
	movq	%r12, %rdi
	call	pthread_create@PLT
	addq	$1, %rbp
	addq	$8, %r12
	cmpq	$8, %rbp
	jne	.L10
	leaq	64(%rbx), %rbp
.L11:
	movl	$0, %esi
	movq	(%rbx), %rdi
	call	pthread_join@PLT
	addq	$8, %rbx
	cmpq	%rbp, %rbx
	jne	.L11
	leaq	lock(%rip), %rdi
	call	pthread_mutex_destroy@PLT
	movq	72(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L16
	movl	$0, %eax
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L16:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE42:
	.size	main, .-main
	.globl	state
	.bss
	.align 4
	.type	state, @object
	.size	state, 4
state:
	.zero	4
	.globl	waiting
	.align 4
	.type	waiting, @object
	.size	waiting, 4
waiting:
	.zero	4
	.globl	lock
	.align 32
	.type	lock, @object
	.size	lock, 40
lock:
	.zero	40
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
