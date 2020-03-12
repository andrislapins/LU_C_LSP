	.file	"LSP_MD3_andris_lapins.c"
	.text
	.comm	ht,8,8
	.comm	md5Flag,4,4
	.comm	dateFlag,4,4
	.comm	result,4,4
	.section	.rodata
.LC0:
	.string	"-h"
.LC1:
	.string	"-m"
.LC2:
	.string	"-d"
	.align 8
.LC3:
	.string	"Incorrect format of the command and its flags"
.LC4:
	.string	"."
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$0, md5Flag(%rip)
	movl	$0, dateFlag(%rip)
	movl	$0, result(%rip)
	call	ht_create
	movq	%rax, ht(%rip)
	movl	$1, -4(%rbp)
	jmp	.L2
.L8:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L3
	call	printHelpText
	movl	$0, %eax
	jmp	.L4
.L3:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L5
	movl	$1, md5Flag(%rip)
	jmp	.L6
.L5:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC2(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L7
	movl	$1, dateFlag(%rip)
	jmp	.L6
.L7:
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	movl	$1, %eax
	jmp	.L4
.L6:
	addl	$1, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L8
	leaq	.LC4(%rip), %rdi
	call	processDirectories
	movl	%eax, result(%rip)
	movq	ht(%rip), %rax
	movq	%rax, %rdi
	call	ht_dump
	movl	result(%rip), %eax
.L4:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC5:
	.string	"An error occurred opening the directory %s\n"
.LC6:
	.string	".."
	.align 8
.LC7:
	.string	"Could not get file attributes."
.LC8:
	.string	"%Y-%m-%d %H:%M"
	.text
	.globl	processDirectories
	.type	processDirectories, @function
processDirectories:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1344, %rsp
	movq	%rdi, -1336(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-1336(%rbp), %rax
	movq	%rax, %rdi
	call	opendir@PLT
	movq	%rax, -1312(%rbp)
	cmpq	$0, -1312(%rbp)
	jne	.L12
	movq	-1336(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC5(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, %eax
	jmp	.L20
.L19:
	movq	-1304(%rbp), %rax
	addq	$19, %rax
	leaq	.LC4(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L12
	movq	-1304(%rbp), %rax
	addq	$19, %rax
	leaq	.LC6(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L12
	movq	-1336(%rbp), %rdx
	leaq	-1008(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	leaq	-1008(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, %rdx
	movl	$0, %eax
	movq	%rdx, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	leaq	-1(%rax), %rdx
	leaq	-1008(%rbp), %rax
	addq	%rdx, %rax
	movw	$47, (%rax)
	movq	-1304(%rbp), %rax
	leaq	19(%rax), %rdx
	leaq	-1008(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcat@PLT
	leaq	-1232(%rbp), %rdx
	leaq	-1008(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	lstat@PLT
	testl	%eax, %eax
	jns	.L13
	leaq	.LC7(%rip), %rdi
	call	puts@PLT
	movl	$1, %eax
	jmp	.L20
.L13:
	movl	-1208(%rbp), %eax
	andl	$16384, %eax
	testl	%eax, %eax
	je	.L14
	leaq	-1008(%rbp), %rax
	movq	%rax, %rdi
	call	processDirectories
	jmp	.L12
.L14:
	movq	-1144(%rbp), %rax
	movq	%rax, -1320(%rbp)
	leaq	-1296(%rbp), %rdx
	leaq	-1320(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	localtime_r@PLT
	leaq	-1296(%rbp), %rdx
	leaq	-1088(%rbp), %rax
	movq	%rdx, %rcx
	leaq	.LC8(%rip), %rdx
	movl	$80, %esi
	movq	%rax, %rdi
	call	strftime@PLT
	movl	md5Flag(%rip), %eax
	testl	%eax, %eax
	je	.L15
	movl	dateFlag(%rip), %eax
	testl	%eax, %eax
	je	.L15
	movq	-1184(%rbp), %rcx
	movq	-1304(%rbp), %rax
	leaq	19(%rax), %rsi
	movq	ht(%rip), %rax
	leaq	-1088(%rbp), %rdi
	leaq	-1008(%rbp), %rdx
	movq	%rdi, %r8
	movq	%rax, %rdi
	call	ht_set_md5_date
	jmp	.L12
.L15:
	movl	md5Flag(%rip), %eax
	testl	%eax, %eax
	je	.L17
	movq	ht(%rip), %rax
	leaq	-1008(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_set_md5
	jmp	.L12
.L17:
	movl	dateFlag(%rip), %eax
	testl	%eax, %eax
	je	.L18
	movq	-1184(%rbp), %rcx
	movq	-1304(%rbp), %rax
	leaq	19(%rax), %rsi
	movq	ht(%rip), %rax
	leaq	-1088(%rbp), %rdi
	leaq	-1008(%rbp), %rdx
	movq	%rdi, %r8
	movq	%rax, %rdi
	call	ht_set_date
	jmp	.L12
.L18:
	movq	-1184(%rbp), %rcx
	movq	-1304(%rbp), %rax
	leaq	19(%rax), %rsi
	movq	ht(%rip), %rax
	leaq	-1008(%rbp), %rdx
	movq	%rax, %rdi
	call	ht_set
.L12:
	movq	-1312(%rbp), %rax
	movq	%rax, %rdi
	call	readdir@PLT
	movq	%rax, -1304(%rbp)
	cmpq	$0, -1304(%rbp)
	jne	.L19
	movq	-1312(%rbp), %rax
	movq	%rax, %rdi
	call	closedir@PLT
	movl	$0, %eax
.L20:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L21
	call	__stack_chk_fail@PLT
.L21:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	processDirectories, .-processDirectories
	.section	.rodata
	.align 8
.LC9:
	.string	"md3.o(1)\t\t\tUser Commands\t\t\tmd3.o(1)\n"
.LC10:
	.string	"NAME"
	.align 8
.LC11:
	.string	"\tmd3.o - Print out duplicate file paths with equal size "
.LC12:
	.string	"and name\n"
.LC13:
	.string	"SYNOPSIS"
.LC14:
	.string	"\t./md3.o [option]...\n"
.LC15:
	.string	"DESCRIPTION"
	.align 8
.LC16:
	.string	"\tOutput file duplicate paths according to the passed flags.\n"
.LC17:
	.string	"\t-h\n\tshows the man page\n"
	.align 8
.LC18:
	.string	"\t-d\n\tprints out the paths files with equal file size, "
.LC19:
	.string	"name and modification date\n"
	.align 8
.LC20:
	.string	"\t-m\n\tprints out the paths files with equal md5 checksum\n"
.LC21:
	.string	"AUTHOR"
.LC22:
	.string	"\tWritten by Andris Lapins."
	.text
	.globl	printHelpText
	.type	printHelpText, @function
printHelpText:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	.LC9(%rip), %rdi
	call	puts@PLT
	leaq	.LC10(%rip), %rdi
	call	puts@PLT
	leaq	.LC11(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC12(%rip), %rdi
	call	puts@PLT
	leaq	.LC13(%rip), %rdi
	call	puts@PLT
	leaq	.LC14(%rip), %rdi
	call	puts@PLT
	leaq	.LC15(%rip), %rdi
	call	puts@PLT
	leaq	.LC16(%rip), %rdi
	call	puts@PLT
	leaq	.LC17(%rip), %rdi
	call	puts@PLT
	leaq	.LC18(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC19(%rip), %rdi
	call	puts@PLT
	leaq	.LC20(%rip), %rdi
	call	puts@PLT
	leaq	.LC21(%rip), %rdi
	call	puts@PLT
	leaq	.LC22(%rip), %rdi
	call	puts@PLT
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	printHelpText, .-printHelpText
	.globl	hash
	.type	hash, @function
hash:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	$0, -8(%rbp)
	movl	$0, -16(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movl	%eax, -12(%rbp)
	jmp	.L24
.L25:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-16(%rbp), %edx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -16(%rbp)
.L24:
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jb	.L25
	movq	-8(%rbp), %rcx
	movabsq	$3777893186295716171, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$11, %rax
	imulq	$10000, %rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	hash, .-hash
	.globl	ht_set
	.type	ht_set, @function
ht_set:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$72, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movq	-80(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	hash
	movl	%eax, -36(%rbp)
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %edx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	jne	.L30
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %edx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, (%rbx)
	jmp	.L27
.L31:
	movq	-32(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -32(%rbp)
.L30:
	cmpq	$0, -32(%rbp)
	jne	.L31
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 40(%rax)
.L27:
	addq	$72, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	ht_set, .-ht_set
	.globl	hash_date
	.type	hash_date, @function
hash_date:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	$0, -8(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movl	%eax, -16(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movl	%eax, -12(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L33
.L34:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -20(%rbp)
.L33:
	movl	-20(%rbp), %eax
	cmpl	%eax, -16(%rbp)
	ja	.L34
	movl	$0, -20(%rbp)
	jmp	.L35
.L36:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -20(%rbp)
.L35:
	movl	-20(%rbp), %eax
	cmpl	%eax, -12(%rbp)
	ja	.L36
	movq	-8(%rbp), %rcx
	movabsq	$3777893186295716171, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$11, %rax
	imulq	$10000, %rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	hash_date, .-hash_date
	.globl	ht_set_date
	.type	ht_set_date, @function
ht_set_date:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movq	%r8, -88(%rbp)
	movq	-88(%rbp), %rdx
	movq	-80(%rbp), %rcx
	movq	-64(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	hash_date
	movl	%eax, -36(%rbp)
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %edx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	jne	.L41
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %edx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, (%rbx)
	jmp	.L38
.L42:
	movq	-32(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -32(%rbp)
.L41:
	cmpq	$0, -32(%rbp)
	jne	.L42
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 40(%rax)
.L38:
	addq	$88, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	ht_set_date, .-ht_set_date
	.globl	hash_md5
	.type	hash_md5, @function
hash_md5:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L44
.L45:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -12(%rbp)
.L44:
	cmpl	$15, -12(%rbp)
	jle	.L45
	movq	-8(%rbp), %rcx
	movabsq	$3777893186295716171, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$11, %rax
	imulq	$10000, %rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	hash_md5, .-hash_md5
	.section	.rodata
.LC23:
	.string	"rb"
.LC24:
	.string	"%s can not be opened\n"
	.text
	.globl	ht_set_md5
	.type	ht_set_md5, @function
ht_set_md5:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$1208, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -1208(%rbp)
	movq	%rsi, -1216(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movq	-1216(%rbp), %rax
	leaq	.LC23(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -1176(%rbp)
	cmpq	$0, -1176(%rbp)
	jne	.L48
	movq	-1216(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC24(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L47
.L48:
	leaq	-1168(%rbp), %rax
	movq	%rax, %rdi
	call	MD5_Init@PLT
	jmp	.L50
.L51:
	movl	-1200(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-1056(%rbp), %rcx
	leaq	-1168(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	MD5_Update@PLT
.L50:
	movq	-1176(%rbp), %rdx
	leaq	-1056(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1024, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	fread@PLT
	movl	%eax, -1200(%rbp)
	cmpl	$0, -1200(%rbp)
	jne	.L51
	leaq	-1168(%rbp), %rdx
	leaq	-1072(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	MD5_Final@PLT
	movq	-1176(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	leaq	-1072(%rbp), %rax
	movq	%rax, %rdi
	call	hash_md5
	movl	%eax, -1196(%rbp)
	movq	-1208(%rbp), %rax
	movq	(%rax), %rax
	movl	-1196(%rbp), %edx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -1192(%rbp)
	cmpq	$0, -1192(%rbp)
	jne	.L53
	movq	-1208(%rbp), %rax
	movq	(%rax), %rax
	movl	-1196(%rbp), %edx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	movq	-1216(%rbp), %rdx
	leaq	-1072(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, (%rbx)
	jmp	.L47
.L54:
	movq	-1192(%rbp), %rax
	movq	%rax, -1184(%rbp)
	movq	-1184(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -1192(%rbp)
.L53:
	cmpq	$0, -1192(%rbp)
	jne	.L54
	movq	-1216(%rbp), %rdx
	leaq	-1072(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, %rdx
	movq	-1184(%rbp), %rax
	movq	%rdx, 40(%rax)
.L47:
	movq	-24(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L56
	call	__stack_chk_fail@PLT
.L56:
	addq	$1208, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	ht_set_md5, .-ht_set_md5
	.globl	hash_md5_date
	.type	hash_md5_date, @function
hash_md5_date:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movq	$0, -8(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movl	%eax, -16(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movl	%eax, -12(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L58
.L59:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	-64(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -20(%rbp)
.L58:
	cmpl	$15, -20(%rbp)
	jle	.L59
	movl	$0, -20(%rbp)
	jmp	.L60
.L61:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -20(%rbp)
.L60:
	movl	-20(%rbp), %eax
	cmpl	%eax, -16(%rbp)
	ja	.L61
	movl	$0, -20(%rbp)
	jmp	.L62
.L63:
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rax,%rdx), %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -20(%rbp)
.L62:
	movl	-20(%rbp), %eax
	cmpl	%eax, -12(%rbp)
	ja	.L63
	movq	-8(%rbp), %rcx
	movabsq	$3777893186295716171, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$11, %rax
	imulq	$10000, %rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	hash_md5_date, .-hash_md5_date
	.globl	ht_set_md5_date
	.type	ht_set_md5_date, @function
ht_set_md5_date:
.LFB15:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$1240, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -1208(%rbp)
	movq	%rsi, -1216(%rbp)
	movq	%rdx, -1224(%rbp)
	movq	%rcx, -1232(%rbp)
	movq	%r8, -1240(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movq	-1224(%rbp), %rax
	leaq	.LC23(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -1176(%rbp)
	cmpq	$0, -1176(%rbp)
	jne	.L66
	movq	-1224(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC24(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L65
.L66:
	leaq	-1168(%rbp), %rax
	movq	%rax, %rdi
	call	MD5_Init@PLT
	jmp	.L68
.L69:
	movl	-1200(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-1056(%rbp), %rcx
	leaq	-1168(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	MD5_Update@PLT
.L68:
	movq	-1176(%rbp), %rdx
	leaq	-1056(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1024, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	fread@PLT
	movl	%eax, -1200(%rbp)
	cmpl	$0, -1200(%rbp)
	jne	.L69
	leaq	-1168(%rbp), %rdx
	leaq	-1072(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	MD5_Final@PLT
	movq	-1176(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	leaq	-1072(%rbp), %rcx
	movq	-1240(%rbp), %rdx
	movq	-1232(%rbp), %rsi
	movq	-1216(%rbp), %rax
	movq	%rax, %rdi
	call	hash_md5_date
	movl	%eax, -1196(%rbp)
	movq	-1208(%rbp), %rax
	movq	(%rax), %rax
	movl	-1196(%rbp), %edx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -1192(%rbp)
	cmpq	$0, -1192(%rbp)
	jne	.L71
	movq	-1208(%rbp), %rax
	movq	(%rax), %rax
	movl	-1196(%rbp), %edx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %rbx
	leaq	-1072(%rbp), %rdx
	movq	-1224(%rbp), %rcx
	movq	-1216(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	ht_pair_md5_date
	movq	%rax, (%rbx)
	jmp	.L65
.L72:
	movq	-1192(%rbp), %rax
	movq	%rax, -1184(%rbp)
	movq	-1184(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -1192(%rbp)
.L71:
	cmpq	$0, -1192(%rbp)
	jne	.L72
	movq	-1224(%rbp), %rdx
	movq	-1216(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	ht_pair
	movq	%rax, %rdx
	movq	-1184(%rbp), %rax
	movq	%rdx, 40(%rax)
.L65:
	movq	-24(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L74
	call	__stack_chk_fail@PLT
.L74:
	addq	$1240, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	ht_set_md5_date, .-ht_set_md5_date
	.section	.rodata
	.align 8
.LC25:
	.string	"Could not get file attributes for pairing."
	.text
	.globl	ht_pair
	.type	ht_pair, @function
ht_pair:
.LFB16:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$336, %rsp
	movq	%rdi, -328(%rbp)
	movq	%rsi, -336(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-240(%rbp), %rdx
	movq	-336(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	lstat@PLT
	testl	%eax, %eax
	jns	.L76
	leaq	.LC25(%rip), %rdi
	call	puts@PLT
.L76:
	movl	$48, %edi
	call	malloc@PLT
	movq	%rax, -312(%rbp)
	movq	-328(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	addq	$1, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 16(%rax)
	movq	-152(%rbp), %rax
	movq	%rax, -320(%rbp)
	leaq	-304(%rbp), %rdx
	leaq	-320(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	localtime_r@PLT
	leaq	-304(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %rcx
	leaq	.LC8(%rip), %rdx
	movl	$80, %esi
	movq	%rax, %rdi
	call	strftime@PLT
	movl	$80, %edi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 32(%rax)
	movq	-312(%rbp), %rax
	movq	(%rax), %rax
	movq	-328(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-312(%rbp), %rax
	movq	16(%rax), %rax
	movq	-336(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-192(%rbp), %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 24(%rax)
	movq	-312(%rbp), %rax
	movq	32(%rax), %rax
	leaq	-96(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-312(%rbp), %rax
	movq	$0, 40(%rax)
	movq	-312(%rbp), %rax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L78
	call	__stack_chk_fail@PLT
.L78:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	ht_pair, .-ht_pair
	.globl	ht_pair_md5_date
	.type	ht_pair_md5_date, @function
ht_pair_md5_date:
.LFB17:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$352, %rsp
	movq	%rdi, -328(%rbp)
	movq	%rsi, -336(%rbp)
	movq	%rdx, -344(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-240(%rbp), %rdx
	movq	-336(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	lstat@PLT
	testl	%eax, %eax
	jns	.L80
	leaq	.LC25(%rip), %rdi
	call	puts@PLT
.L80:
	movl	$48, %edi
	call	malloc@PLT
	movq	%rax, -312(%rbp)
	movq	-328(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	addq	$1, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 16(%rax)
	movq	-344(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-152(%rbp), %rax
	movq	%rax, -320(%rbp)
	leaq	-304(%rbp), %rdx
	leaq	-320(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	localtime_r@PLT
	leaq	-304(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %rcx
	leaq	.LC8(%rip), %rdx
	movl	$80, %esi
	movq	%rax, %rdi
	call	strftime@PLT
	movl	$80, %edi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 32(%rax)
	movq	-312(%rbp), %rax
	movq	(%rax), %rax
	movq	-328(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-312(%rbp), %rax
	movq	16(%rax), %rax
	movq	-336(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-312(%rbp), %rax
	movq	8(%rax), %rax
	movq	-344(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-192(%rbp), %rdx
	movq	-312(%rbp), %rax
	movq	%rdx, 24(%rax)
	movq	-312(%rbp), %rax
	movq	32(%rax), %rax
	leaq	-96(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-312(%rbp), %rax
	movq	$0, 40(%rax)
	movq	-312(%rbp), %rax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L82
	call	__stack_chk_fail@PLT
.L82:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	ht_pair_md5_date, .-ht_pair_md5_date
	.globl	ht_create
	.type	ht_create, @function
ht_create:
.LFB18:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$8, %edi
	call	malloc@PLT
	movq	%rax, -8(%rbp)
	movl	$80000, %edi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, (%rax)
	movl	$0, -12(%rbp)
	jmp	.L84
.L85:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	$0, (%rax)
	addl	$1, -12(%rbp)
.L84:
	cmpl	$9999, -12(%rbp)
	jle	.L85
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	ht_create, .-ht_create
	.section	.rodata
.LC26:
	.string	"=== %s %ld %s"
.LC27:
	.string	"%0x"
.LC28:
	.string	"=== %s %ld "
.LC29:
	.string	"=== %s %ld %s\n"
	.text
	.globl	ht_dump
	.type	ht_dump, @function
ht_dump:
.LFB19:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L88
.L101:
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.L102
	movq	-8(%rbp), %rax
	movq	40(%rax), %rax
	testq	%rax, %rax
	je	.L103
	movl	md5Flag(%rip), %eax
	testl	%eax, %eax
	je	.L92
	movl	dateFlag(%rip), %eax
	testl	%eax, %eax
	je	.L92
	movq	-8(%rbp), %rax
	movq	(%rax), %rcx
	movq	-8(%rbp), %rax
	movq	24(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC26(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -16(%rbp)
	jmp	.L93
.L94:
	movq	-8(%rbp), %rax
	movq	8(%rax), %rdx
	movl	-16(%rbp), %eax
	cltq
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC27(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -16(%rbp)
.L93:
	cmpl	$15, -16(%rbp)
	jle	.L94
	movl	$10, %edi
	call	putchar@PLT
	jmp	.L95
.L92:
	movl	md5Flag(%rip), %eax
	testl	%eax, %eax
	je	.L96
	movq	-8(%rbp), %rax
	movq	24(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC28(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -12(%rbp)
	jmp	.L97
.L98:
	movq	-8(%rbp), %rax
	movq	(%rax), %rdx
	movl	-12(%rbp), %eax
	cltq
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC27(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -12(%rbp)
.L97:
	cmpl	$15, -12(%rbp)
	jle	.L98
	movl	$10, %edi
	call	putchar@PLT
	jmp	.L99
.L96:
	movq	-8(%rbp), %rax
	movq	(%rax), %rcx
	movq	-8(%rbp), %rax
	movq	24(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC29(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L95:
	jmp	.L99
.L100:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movq	-8(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -8(%rbp)
.L99:
	cmpq	$0, -8(%rbp)
	jne	.L100
	movl	$10, %edi
	call	putchar@PLT
	jmp	.L90
.L102:
	nop
	jmp	.L90
.L103:
	nop
.L90:
	addl	$1, -20(%rbp)
.L88:
	cmpl	$9999, -20(%rbp)
	jle	.L101
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	ht_dump, .-ht_dump
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
