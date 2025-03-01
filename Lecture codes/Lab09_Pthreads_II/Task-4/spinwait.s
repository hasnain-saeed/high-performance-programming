	.file	"spinwait.c"
	.text
	.globl	lock
	.bss
	.align 32
	.type	lock, @object
	.size	lock, 40
lock:
	.zero	40
	.globl	waiting
	.align 4
	.type	waiting, @object
	.size	waiting, 4
waiting:
	.zero	4
	.globl	state
	.align 4
	.type	state, @object
	.size	state, 4
state:
	.zero	4
	.text
	.globl	barrier
	.type	barrier, @function
barrier:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	lock(%rip), %rax
	movq	%rax, %rdi
	call	pthread_mutex_lock@PLT
	movl	state(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	waiting(%rip), %eax
	addl	$1, %eax
	movl	%eax, waiting(%rip)
	movl	waiting(%rip), %eax
	cmpl	$8, %eax
	jne	.L2
	movl	$0, waiting(%rip)
	movl	$1, %eax
	subl	-4(%rbp), %eax
	movl	%eax, state(%rip)
.L2:
	leaq	lock(%rip), %rax
	movq	%rax, %rdi
	call	pthread_mutex_unlock@PLT
	nop
.L3:
	movl	state(%rip), %eax
	cmpl	%eax, -4(%rbp)
	je	.L3
	nop
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	barrier, .-barrier
	.section	.rodata
.LC0:
	.string	"Hello World! %ld\n"
.LC1:
	.string	"Bye Bye World! %ld\n"
	.text
	.globl	HelloWorld
	.type	HelloWorld, @function
HelloWorld:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	call	barrier
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	HelloWorld, .-HelloWorld
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$112, %rsp
	movl	%edi, -100(%rbp)
	movq	%rsi, -112(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, %esi
	leaq	lock(%rip), %rax
	movq	%rax, %rdi
	call	pthread_mutex_init@PLT
	movq	$0, -88(%rbp)
	jmp	.L7
.L8:
	movq	-88(%rbp), %rax
	leaq	-80(%rbp), %rcx
	movq	-88(%rbp), %rdx
	salq	$3, %rdx
	leaq	(%rcx,%rdx), %rdi
	movq	%rax, %rcx
	leaq	HelloWorld(%rip), %rax
	movq	%rax, %rdx
	movl	$0, %esi
	call	pthread_create@PLT
	addq	$1, -88(%rbp)
.L7:
	cmpq	$7, -88(%rbp)
	jle	.L8
	movq	$0, -88(%rbp)
	jmp	.L9
.L10:
	movq	-88(%rbp), %rax
	movq	-80(%rbp,%rax,8), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	addq	$1, -88(%rbp)
.L9:
	cmpq	$7, -88(%rbp)
	jle	.L10
	leaq	lock(%rip), %rax
	movq	%rax, %rdi
	call	pthread_mutex_destroy@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L12
	call	__stack_chk_fail@PLT
.L12:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
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
