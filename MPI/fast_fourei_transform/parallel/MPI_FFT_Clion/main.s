	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"main.c"
	.def	 sprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,sprintf
	.globl	sprintf                 # -- Begin function sprintf
	.p2align	4, 0x90
sprintf:                                # @sprintf
.seh_proc sprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 104(%rsp)
	movq	%r8, 96(%rsp)
	movq	%rdx, 64(%rsp)
	movq	%rcx, 56(%rsp)
	leaq	96(%rsp), %rcx
	movq	%rcx, 40(%rsp)
	movq	40(%rsp), %r9
	movq	64(%rsp), %rdx
	movq	56(%rsp), %rcx
	xorl	%eax, %eax
	movl	%eax, %r8d
	callq	_vsprintf_l
	movl	%eax, 52(%rsp)
	movl	52(%rsp), %eax
	addq	$72, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,sprintf
	.seh_endproc
                                        # -- End function
	.def	 vsprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,vsprintf
	.globl	vsprintf                # -- Begin function vsprintf
	.p2align	4, 0x90
vsprintf:                               # @vsprintf
.seh_proc vsprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	xorl	%eax, %eax
	movl	%eax, %r9d
	movq	%r8, 64(%rsp)
	movq	%rdx, 56(%rsp)
	movq	%rcx, 48(%rsp)
	movq	64(%rsp), %rcx
	movq	56(%rsp), %r8
	movq	48(%rsp), %rdx
	movq	%rcx, 40(%rsp)          # 8-byte Spill
	movq	%rdx, %rcx
	movq	$-1, %rdx
	movq	40(%rsp), %r10          # 8-byte Reload
	movq	%r10, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$72, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,vsprintf
	.seh_endproc
                                        # -- End function
	.def	 _snprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_snprintf
	.globl	_snprintf               # -- Begin function _snprintf
	.p2align	4, 0x90
_snprintf:                              # @_snprintf
.seh_proc _snprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 104(%rsp)
	movq	%r8, 64(%rsp)
	movq	%rdx, 56(%rsp)
	movq	%rcx, 48(%rsp)
	leaq	104(%rsp), %rcx
	movq	%rcx, 32(%rsp)
	movq	32(%rsp), %r9
	movq	64(%rsp), %r8
	movq	56(%rsp), %rdx
	movq	48(%rsp), %rcx
	callq	_vsnprintf
	movl	%eax, 44(%rsp)
	movl	44(%rsp), %eax
	addq	$72, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_snprintf
	.seh_endproc
                                        # -- End function
	.def	 _vsnprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsnprintf
	.globl	_vsnprintf              # -- Begin function _vsnprintf
	.p2align	4, 0x90
_vsnprintf:                             # @_vsnprintf
.seh_proc _vsnprintf
# %bb.0:
	subq	$88, %rsp
	.seh_stackalloc 88
	.seh_endprologue
	xorl	%eax, %eax
	movl	%eax, %r10d
	movq	%r9, 80(%rsp)
	movq	%r8, 72(%rsp)
	movq	%rdx, 64(%rsp)
	movq	%rcx, 56(%rsp)
	movq	80(%rsp), %rcx
	movq	72(%rsp), %r8
	movq	64(%rsp), %rdx
	movq	56(%rsp), %r9
	movq	%rcx, 48(%rsp)          # 8-byte Spill
	movq	%r9, %rcx
	movq	%r10, %r9
	movq	48(%rsp), %r10          # 8-byte Reload
	movq	%r10, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$88, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_vsnprintf
	.seh_endproc
                                        # -- End function
	.def	 fill_double_arr;
	.scl	2;
	.type	32;
	.endef
	.globl	__xmm@00000000000000004530000043300000 # -- Begin function fill_double_arr
	.section	.rdata,"dr",discard,__xmm@00000000000000004530000043300000
	.p2align	4
__xmm@00000000000000004530000043300000:
	.long	1127219200              # 0x43300000
	.long	1160773632              # 0x45300000
	.long	0                       # 0x0
	.long	0                       # 0x0
	.globl	__xmm@45300000000000004330000000000000
	.section	.rdata,"dr",discard,__xmm@45300000000000004330000000000000
	.p2align	4
__xmm@45300000000000004330000000000000:
	.quad	4841369599423283200     # double 4503599627370496
	.quad	4985484787499139072     # double 1.9342813113834067E+25
	.text
	.globl	fill_double_arr
	.p2align	4, 0x90
fill_double_arr:                        # @fill_double_arr
.seh_proc fill_double_arr
# %bb.0:
	subq	$24, %rsp
	.seh_stackalloc 24
	.seh_endprologue
	movq	%rdx, 16(%rsp)
	movq	%rcx, 8(%rsp)
	movq	$0, (%rsp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	(%rsp), %rax
	cmpq	16(%rsp), %rax
	jae	.LBB4_4
# %bb.2:                                #   in Loop: Header=BB4_1 Depth=1
	movq	(%rsp), %rax
	imulq	%rax, %rax
	movq	%rax, %xmm0
	movaps	__xmm@00000000000000004530000043300000(%rip), %xmm1 # xmm1 = [1127219200,1160773632,0,0]
	punpckldq	%xmm1, %xmm0    # xmm0 = xmm0[0],xmm1[0],xmm0[1],xmm1[1]
	movapd	__xmm@45300000000000004330000000000000(%rip), %xmm1 # xmm1 = [4.503599627370496E+15,1.9342813113834067E+25]
	subpd	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	unpckhpd	%xmm1, %xmm1    # xmm1 = xmm1[1,1]
	addpd	%xmm0, %xmm1
	movq	8(%rsp), %rax
	movq	(%rsp), %rcx
	movsd	%xmm1, (%rax,%rcx,8)
# %bb.3:                                #   in Loop: Header=BB4_1 Depth=1
	movq	(%rsp), %rax
	addq	$1, %rax
	movq	%rax, (%rsp)
	jmp	.LBB4_1
.LBB4_4:
	addq	$24, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function
	.def	 main;
	.scl	2;
	.type	32;
	.endef
	.globl	__real@4000000000000000 # -- Begin function main
	.section	.rdata,"dr",discard,__real@4000000000000000
	.p2align	3
__real@4000000000000000:
	.quad	4611686018427387904     # double 2
	.globl	__real@3ff0000000000000
	.section	.rdata,"dr",discard,__real@3ff0000000000000
	.p2align	3
__real@3ff0000000000000:
	.quad	4607182418800017408     # double 1
	.globl	__real@3ff199999999999a
	.section	.rdata,"dr",discard,__real@3ff199999999999a
	.p2align	3
__real@3ff199999999999a:
	.quad	4607632778762754458     # double 1.1000000000000001
	.text
	.globl	main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:
	movl	$8000104, %eax          # imm = 0x7A1268
	callq	__chkstk
	subq	%rax, %rsp
	.seh_stackalloc 8000104
	.seh_endprologue
	leaq	64(%rsp), %rcx
	movsd	__real@4000000000000000(%rip), %xmm0 # xmm0 = mem[0],zero
	movsd	__real@3ff0000000000000(%rip), %xmm1 # xmm1 = mem[0],zero
	movl	$0, 8000100(%rsp)
	movsd	%xmm1, 8000088(%rsp)
	movsd	%xmm0, 8000080(%rsp)
	movsd	8000088(%rsp), %xmm0    # xmm0 = mem[0],zero
	movsd	8000080(%rsp), %xmm1    # xmm1 = mem[0],zero
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8000072(%rsp)
	movl	$1000000, %edx          # imm = 0xF4240
	callq	fill_double_arr
	callq	clock
	movl	%eax, st_clk(%rip)
	movl	$0, 60(%rsp)
	movl	$0, 60(%rsp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$100, 60(%rsp)
	jge	.LBB5_4
# %bb.2:                                #   in Loop: Header=BB5_1 Depth=1
	callq	clock
	movl	%eax, 56(%rsp)
	imull	$400, 60(%rsp), %eax    # imm = 0x190
	movslq	%eax, %rcx
	movsd	64(%rsp,%rcx,8), %xmm0  # xmm0 = mem[0],zero
	imull	$100, 60(%rsp), %eax
	movslq	%eax, %rcx
	addsd	64(%rsp,%rcx,8), %xmm0
	movsd	%xmm0, 8000072(%rsp)
	callq	clock
	movsd	__real@3ff199999999999a(%rip), %xmm0 # xmm0 = mem[0],zero
	movl	%eax, 52(%rsp)
	movsd	8000088(%rsp), %xmm1    # xmm1 = mem[0],zero
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, 8000088(%rsp)
	movsd	8000080(%rsp), %xmm1    # xmm1 = mem[0],zero
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, 8000080(%rsp)
	movl	52(%rsp), %eax
	subl	56(%rsp), %eax
	addl	sum_clk(%rip), %eax
	movl	%eax, sum_clk(%rip)
# %bb.3:                                #   in Loop: Header=BB5_1 Depth=1
	movl	60(%rsp), %eax
	addl	$1, %eax
	movl	%eax, 60(%rsp)
	jmp	.LBB5_1
.LBB5_4:
	callq	clock
	movl	%eax, ed_clk(%rip)
	movl	sum_clk(%rip), %edx
	leaq	"??_C@_0BJ@NGDDJGGH@sum?5overhead?5clock?3?5?$CFld?6?$AA@"(%rip), %rcx
	callq	printf
	movl	ed_clk(%rip), %edx
	subl	st_clk(%rip), %edx
	movl	%edx, 48(%rsp)
	movl	48(%rsp), %edx
	leaq	"??_C@_0BL@MBAGMFBK@outer?5overhead?5clock?3?5?$CFld?6?$AA@"(%rip), %rcx
	movl	%eax, 44(%rsp)          # 4-byte Spill
	callq	printf
	movsd	8000072(%rsp), %xmm0    # xmm0 = mem[0],zero
	leaq	"??_C@_03DLDNIBIK@?$CFlf?$AA@"(%rip), %rcx
	movaps	%xmm0, %xmm1
	movq	%xmm0, %rdx
	movl	%eax, 40(%rsp)          # 4-byte Spill
	callq	printf
	xorl	%r8d, %r8d
	movl	%eax, 36(%rsp)          # 4-byte Spill
	movl	%r8d, %eax
	addq	$8000104, %rsp          # imm = 0x7A1268
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function
	.def	 printf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,printf
	.globl	printf                  # -- Begin function printf
	.p2align	4, 0x90
printf:                                 # @printf
.seh_proc printf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 104(%rsp)
	movq	%r8, 96(%rsp)
	movq	%rdx, 88(%rsp)
	movq	%rcx, 64(%rsp)
	leaq	88(%rsp), %rcx
	movq	%rcx, 48(%rsp)
	movq	48(%rsp), %r9
	movq	64(%rsp), %rdx
	movl	$1, %ecx
	movq	%r9, 40(%rsp)           # 8-byte Spill
	movq	%rdx, 32(%rsp)          # 8-byte Spill
	callq	__acrt_iob_func
	xorl	%ecx, %ecx
	movl	%ecx, %r8d
	movq	%rax, %rcx
	movq	32(%rsp), %rdx          # 8-byte Reload
	movq	40(%rsp), %r9           # 8-byte Reload
	callq	_vfprintf_l
	movl	%eax, 60(%rsp)
	movl	60(%rsp), %eax
	addq	$72, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,printf
	.seh_endproc
                                        # -- End function
	.def	 _vsprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsprintf_l
	.globl	_vsprintf_l             # -- Begin function _vsprintf_l
	.p2align	4, 0x90
_vsprintf_l:                            # @_vsprintf_l
.seh_proc _vsprintf_l
# %bb.0:
	subq	$88, %rsp
	.seh_stackalloc 88
	.seh_endprologue
	movq	%r9, 80(%rsp)
	movq	%r8, 72(%rsp)
	movq	%rdx, 64(%rsp)
	movq	%rcx, 56(%rsp)
	movq	80(%rsp), %rcx
	movq	72(%rsp), %r9
	movq	64(%rsp), %r8
	movq	56(%rsp), %rdx
	movq	%rcx, 48(%rsp)          # 8-byte Spill
	movq	%rdx, %rcx
	movq	$-1, %rdx
	movq	48(%rsp), %rax          # 8-byte Reload
	movq	%rax, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$88, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_vsprintf_l
	.seh_endproc
                                        # -- End function
	.def	 _vsnprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsnprintf_l
	.globl	_vsnprintf_l            # -- Begin function _vsnprintf_l
	.p2align	4, 0x90
_vsnprintf_l:                           # @_vsnprintf_l
.seh_proc _vsnprintf_l
# %bb.0:
	subq	$152, %rsp
	.seh_stackalloc 152
	.seh_endprologue
	movq	192(%rsp), %rax
	movq	%r9, 144(%rsp)
	movq	%r8, 136(%rsp)
	movq	%rdx, 128(%rsp)
	movq	%rcx, 120(%rsp)
	movq	192(%rsp), %rcx
	movq	144(%rsp), %rdx
	movq	136(%rsp), %r9
	movq	128(%rsp), %r8
	movq	120(%rsp), %r10
	movq	%rax, 104(%rsp)         # 8-byte Spill
	movq	%rcx, 96(%rsp)          # 8-byte Spill
	movq	%rdx, 88(%rsp)          # 8-byte Spill
	movq	%r9, 80(%rsp)           # 8-byte Spill
	movq	%r8, 72(%rsp)           # 8-byte Spill
	movq	%r10, 64(%rsp)          # 8-byte Spill
	callq	__local_stdio_printf_options
	movq	(%rax), %rax
	orq	$1, %rax
	movq	%rax, %rcx
	movq	64(%rsp), %rdx          # 8-byte Reload
	movq	72(%rsp), %r8           # 8-byte Reload
	movq	80(%rsp), %r9           # 8-byte Reload
	movq	88(%rsp), %rax          # 8-byte Reload
	movq	%rax, 32(%rsp)
	movq	96(%rsp), %rax          # 8-byte Reload
	movq	%rax, 40(%rsp)
	callq	__stdio_common_vsprintf
	movl	%eax, 116(%rsp)
	cmpl	$0, 116(%rsp)
	jge	.LBB8_2
# %bb.1:
	movl	$4294967295, %eax       # imm = 0xFFFFFFFF
	movl	%eax, 60(%rsp)          # 4-byte Spill
	jmp	.LBB8_3
.LBB8_2:
	movl	116(%rsp), %eax
	movl	%eax, 60(%rsp)          # 4-byte Spill
.LBB8_3:
	movl	60(%rsp), %eax          # 4-byte Reload
	addq	$152, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_vsnprintf_l
	.seh_endproc
                                        # -- End function
	.def	 __local_stdio_printf_options;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__local_stdio_printf_options
	.globl	__local_stdio_printf_options # -- Begin function __local_stdio_printf_options
	.p2align	4, 0x90
__local_stdio_printf_options:           # @__local_stdio_printf_options
# %bb.0:
	leaq	__local_stdio_printf_options._OptionsStorage(%rip), %rax
	retq
                                        # -- End function
	.def	 _vfprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vfprintf_l
	.globl	_vfprintf_l             # -- Begin function _vfprintf_l
	.p2align	4, 0x90
_vfprintf_l:                            # @_vfprintf_l
.seh_proc _vfprintf_l
# %bb.0:
	subq	$104, %rsp
	.seh_stackalloc 104
	.seh_endprologue
	movq	%r9, 96(%rsp)
	movq	%r8, 88(%rsp)
	movq	%rdx, 80(%rsp)
	movq	%rcx, 72(%rsp)
	movq	96(%rsp), %rcx
	movq	88(%rsp), %r9
	movq	80(%rsp), %r8
	movq	72(%rsp), %rdx
	movq	%rcx, 64(%rsp)          # 8-byte Spill
	movq	%r9, 56(%rsp)           # 8-byte Spill
	movq	%r8, 48(%rsp)           # 8-byte Spill
	movq	%rdx, 40(%rsp)          # 8-byte Spill
	callq	__local_stdio_printf_options
	movq	(%rax), %rcx
	movq	40(%rsp), %rdx          # 8-byte Reload
	movq	48(%rsp), %r8           # 8-byte Reload
	movq	56(%rsp), %r9           # 8-byte Reload
	movq	64(%rsp), %rax          # 8-byte Reload
	movq	%rax, 32(%rsp)
	callq	__stdio_common_vfprintf
	nop
	addq	$104, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_vfprintf_l
	.seh_endproc
                                        # -- End function
	.data
	.globl	a                       # @a
	.p2align	3
a:
	.quad	4607182418800017408     # double 1

	.globl	b                       # @b
	.p2align	3
b:
	.quad	4611686018427387904     # double 2

	.comm	st_clk,4,2              # @st_clk
	.comm	sum_clk,4,2             # @sum_clk
	.comm	ed_clk,4,2              # @ed_clk
	.section	.rdata,"dr",discard,"??_C@_0BJ@NGDDJGGH@sum?5overhead?5clock?3?5?$CFld?6?$AA@"
	.globl	"??_C@_0BJ@NGDDJGGH@sum?5overhead?5clock?3?5?$CFld?6?$AA@" # @"??_C@_0BJ@NGDDJGGH@sum?5overhead?5clock?3?5?$CFld?6?$AA@"
"??_C@_0BJ@NGDDJGGH@sum?5overhead?5clock?3?5?$CFld?6?$AA@":
	.asciz	"sum overhead clock: %ld\n"

	.section	.rdata,"dr",discard,"??_C@_0BL@MBAGMFBK@outer?5overhead?5clock?3?5?$CFld?6?$AA@"
	.globl	"??_C@_0BL@MBAGMFBK@outer?5overhead?5clock?3?5?$CFld?6?$AA@" # @"??_C@_0BL@MBAGMFBK@outer?5overhead?5clock?3?5?$CFld?6?$AA@"
"??_C@_0BL@MBAGMFBK@outer?5overhead?5clock?3?5?$CFld?6?$AA@":
	.asciz	"outer overhead clock: %ld\n"

	.section	.rdata,"dr",discard,"??_C@_03DLDNIBIK@?$CFlf?$AA@"
	.globl	"??_C@_03DLDNIBIK@?$CFlf?$AA@" # @"??_C@_03DLDNIBIK@?$CFlf?$AA@"
"??_C@_03DLDNIBIK@?$CFlf?$AA@":
	.asciz	"%lf"

	.lcomm	__local_stdio_printf_options._OptionsStorage,8,8 # @__local_stdio_printf_options._OptionsStorage

	.addrsig
	.addrsig_sym _vsnprintf
	.addrsig_sym fill_double_arr
	.addrsig_sym clock
	.addrsig_sym printf
	.addrsig_sym _vsprintf_l
	.addrsig_sym _vsnprintf_l
	.addrsig_sym __stdio_common_vsprintf
	.addrsig_sym __local_stdio_printf_options
	.addrsig_sym _vfprintf_l
	.addrsig_sym __acrt_iob_func
	.addrsig_sym __stdio_common_vfprintf
	.addrsig_sym st_clk
	.addrsig_sym sum_clk
	.addrsig_sym ed_clk
	.addrsig_sym __local_stdio_printf_options._OptionsStorage
	.globl	_fltused
