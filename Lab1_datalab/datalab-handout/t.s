	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 4
	.globl	_howManyBits            ## -- Begin function howManyBits
	.p2align	4, 0x90
_howManyBits:                           ## @howManyBits
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, %edx
	sarl	$31, %edx
	xorl	%edi, %edx
	xorl	%eax, %eax
	cmpl	$65535, %edx            ## imm = 0xFFFF
	seta	%al
	shll	$4, %eax
	movl	%eax, %ecx
	sarl	%cl, %edx
	xorl	%r8d, %r8d
	cmpl	$255, %edx
	seta	%r8b
	leal	(,%r8,8), %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	sarl	%cl, %edx
	xorl	%edi, %edi
	cmpl	$15, %edx
	seta	%dil
	leal	(,%rdi,4), %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	sarl	%cl, %edx
	xorl	%esi, %esi
	cmpl	$3, %edx
	seta	%sil
	leal	(%rsi,%rsi), %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	sarl	%cl, %edx
	xorl	%ecx, %ecx
	cmpl	$1, %edx
	seta	%cl
	sarl	%cl, %edx
	leal	(%rax,%r8,8), %eax
	leal	(%rax,%rdi,4), %eax
	leal	(%rax,%rsi,2), %eax
	orl	%ecx, %eax
	cmpl	$1, %edx
	sbbl	$-1, %eax
	incl	%eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	callq	_howManyBits
	leaq	L_.str(%rip), %rdi
	movl	%eax, %esi
	xorl	%eax, %eax
	callq	_printf
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%i\n"


.subsections_via_symbols
