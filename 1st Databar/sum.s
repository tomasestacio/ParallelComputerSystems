	.file	"sum.c"
	.text
	.p2align 4,,15
	.globl	sum_naive
	.type	sum_naive, @function
sum_naive:
.LFB533:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L4
	xorl	%edx, %edx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L3:
	addl	(%rsi,%rdx,4), %eax
	addq	$1, %rdx
	cmpl	%edx, %edi
	jg	.L3
	rep ret
.L4:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE533:
	.size	sum_naive, .-sum_naive
	.p2align 4,,15
	.globl	sum_unrolled
	.type	sum_unrolled, @function
sum_unrolled:
.LFB534:
	.cfi_startproc
	leal	3(%rdi), %r8d
	testl	%edi, %edi
	cmovns	%edi, %r8d
	andl	$-4, %r8d
	jle	.L11
	leal	-1(%r8), %eax
	movq	%rsi, %rdx
	shrl	$2, %eax
	salq	$4, %rax
	leaq	16(%rsi,%rax), %rcx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L8:
	addl	(%rdx), %eax
	addq	$16, %rdx
	addl	-12(%rdx), %eax
	addl	-8(%rdx), %eax
	addl	-4(%rdx), %eax
	cmpq	%rcx, %rdx
	jne	.L8
.L7:
	cmpl	%r8d, %edi
	jle	.L9
	subl	$1, %edi
	movslq	%r8d, %rcx
	subl	%r8d, %edi
	leaq	(%rsi,%rcx,4), %rdx
	addq	%rdi, %rcx
	leaq	4(%rsi,%rcx,4), %rcx
	.p2align 4,,10
	.p2align 3
.L10:
	addl	(%rdx), %eax
	addq	$4, %rdx
	cmpq	%rcx, %rdx
	jne	.L10
.L9:
	rep ret
.L11:
	xorl	%eax, %eax
	jmp	.L7
	.cfi_endproc
.LFE534:
	.size	sum_unrolled, .-sum_unrolled
	.p2align 4,,15
	.globl	sum_vectorized
	.type	sum_vectorized, @function
sum_vectorized:
.LFB535:
	.cfi_startproc
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE535:
	.size	sum_vectorized, .-sum_vectorized
	.p2align 4,,15
	.globl	sum_vectorized_unrolled
	.type	sum_vectorized_unrolled, @function
sum_vectorized_unrolled:
.LFB536:
	.cfi_startproc
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE536:
	.size	sum_vectorized_unrolled, .-sum_vectorized_unrolled
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%20s: "
.LC3:
	.string	"%.2f microseconds\n"
.LC4:
	.string	"ERROR!"
	.text
	.p2align 4,,15
	.globl	benchmark
	.type	benchmark, @function
benchmark:
.LFB537:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movl	%edi, %r15d
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rdx, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rcx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	call	*%rdx
	movl	%eax, %ebx
#APP
# 11 "sum.c" 1
	rdtsc
# 0 "" 2
#NO_APP
	movl	%edx, 16(%rsp)
	movl	%eax, 12(%rsp)
	movq	%rbp, %rsi
	movl	%r15d, %edi
	call	*%r13
	addl	%eax, %ebx
#APP
# 11 "sum.c" 1
	rdtsc
# 0 "" 2
#NO_APP
	movl	$.LC0, %edi
	movl	%eax, %r13d
	movq	%r12, %rsi
	xorl	%eax, %eax
	movl	%edx, %r14d
	call	printf
	testl	%r15d, %r15d
	jle	.L21
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L17:
	addl	0(%rbp,%rcx,4), %eax
	addq	$1, %rcx
	cmpl	%ecx, %r15d
	jg	.L17
	addl	%eax, %eax
.L15:
	cmpl	%eax, %ebx
	je	.L23
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	$.LC4, %edi
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	puts
	.p2align 4,,10
	.p2align 3
.L23:
	.cfi_restore_state
	movq	16(%rsp), %r12
	movl	12(%rsp), %ebp
	salq	$32, %r14
	orq	%r13, %r14
	salq	$32, %r12
	orq	%rbp, %r12
	subq	%r12, %r14
	js	.L19
	cvtsi2sdq	%r14, %xmm0
.L20:
	divsd	.LC1(%rip), %xmm0
	movl	$.LC3, %edi
	movl	$1, %eax
	mulsd	.LC2(%rip), %xmm0
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	printf
.L21:
	.cfi_restore_state
	xorl	%eax, %eax
	jmp	.L15
.L19:
	movq	%r14, %rax
	andl	$1, %r14d
	shrq	%rax
	orq	%r14, %rax
	cvtsi2sdq	%rax, %xmm0
	addsd	%xmm0, %xmm0
	jmp	.L20
	.cfi_endproc
.LFE537:
	.size	benchmark, .-benchmark
	.section	.rodata.str1.1
.LC5:
	.string	"naive"
.LC6:
	.string	"unrolled"
.LC7:
	.string	"vectorized"
.LC8:
	.string	"vectorized unrolled"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB538:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%edi, %edi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	xorl	%ebx, %ebx
	call	time
	movq	%rax, %rdi
	call	srand48
	subq	$31136, %rsp
	leaq	15(%rsp), %r12
	andq	$-16, %r12
	.p2align 4,,10
	.p2align 3
.L26:
	call	lrand48
	movl	%eax, (%r12,%rbx)
	addq	$4, %rbx
	cmpq	$31108, %rbx
	jne	.L26
	movq	%r12, %rsi
	movl	$.LC5, %ecx
	movl	$sum_naive, %edx
	movl	$7777, %edi
	call	benchmark
	movq	%r12, %rsi
	movl	$.LC6, %ecx
	movl	$sum_unrolled, %edx
	movl	$7777, %edi
	call	benchmark
	movq	%r12, %rsi
	movl	$.LC7, %ecx
	movl	$sum_vectorized, %edx
	movl	$7777, %edi
	call	benchmark
	movq	%r12, %rsi
	movl	$.LC8, %ecx
	movl	$sum_vectorized_unrolled, %edx
	movl	$7777, %edi
	call	benchmark
	leaq	-16(%rbp), %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE538:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC1:
	.long	3758096384
	.long	1105273574
	.align 8
.LC2:
	.long	0
	.long	1093567616
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
