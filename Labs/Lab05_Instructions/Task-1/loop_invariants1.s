	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 0
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function main
lCPI0_0:
	.quad	0x401ecccccccccccd              ; double 7.7000000000000001
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	stur	wzr, [x29, #-4]
	stur	w0, [x29, #-8]
	stur	x1, [x29, #-16]
	mov	w8, #10000
	stur	w8, [x29, #-20]
	ldur	w8, [x29, #-20]
	ldur	w9, [x29, #-20]
	mul	w9, w8, w9
                                        ; implicit-def: $x8
	mov	x8, x9
	sxtw	x8, w8
	lsl	x0, x8, #3
	bl	_malloc
	str	x0, [sp, #32]
	stur	wzr, [x29, #-24]
	b	LBB0_1
LBB0_1:                                 ; =>This Inner Loop Header: Depth=1
	ldur	w8, [x29, #-24]
	ldur	w9, [x29, #-20]
	ldur	w10, [x29, #-20]
	mul	w9, w9, w10
	subs	w8, w8, w9
	cset	w8, ge
	tbnz	w8, #0, LBB0_4
	b	LBB0_2
LBB0_2:                                 ;   in Loop: Header=BB0_1 Depth=1
	ldr	x8, [sp, #32]
	ldursw	x9, [x29, #-24]
	movi	d0, #0000000000000000
	str	d0, [x8, x9, lsl #3]
	b	LBB0_3
LBB0_3:                                 ;   in Loop: Header=BB0_1 Depth=1
	ldur	w8, [x29, #-24]
	add	w8, w8, #1
	stur	w8, [x29, #-24]
	b	LBB0_1
LBB0_4:
	adrp	x0, l_.str@PAGE
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	stur	wzr, [x29, #-24]
	b	LBB0_5
LBB0_5:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_7 Depth 2
	ldur	w8, [x29, #-24]
	ldur	w9, [x29, #-20]
	subs	w8, w8, w9
	cset	w8, ge
	tbnz	w8, #0, LBB0_12
	b	LBB0_6
LBB0_6:                                 ;   in Loop: Header=BB0_5 Depth=1
	ldur	w8, [x29, #-24]
	ldur	w9, [x29, #-20]
	mul	w8, w8, w9
	str	w8, [sp, #20]
	ldur	w8, [x29, #-24]
	ldur	w9, [x29, #-20]
	mul	w8, w8, w9
	scvtf	d0, w8
	adrp	x8, lCPI0_0@PAGE
	ldr	d1, [x8, lCPI0_0@PAGEOFF]
	fdiv	d0, d0, d1
	str	d0, [sp, #24]
	stur	wzr, [x29, #-28]
	b	LBB0_7
LBB0_7:                                 ;   Parent Loop BB0_5 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldur	w8, [x29, #-28]
	ldur	w9, [x29, #-20]
	subs	w8, w8, w9
	cset	w8, ge
	tbnz	w8, #0, LBB0_10
	b	LBB0_8
LBB0_8:                                 ;   in Loop: Header=BB0_7 Depth=2
	ldr	d0, [sp, #24]
	ldur	s2, [x29, #-28]
                                        ; implicit-def: $d1
	fmov	s1, s2
	sshll.2d	v1, v1, #0
                                        ; kill: def $d1 killed $d1 killed $q1
	scvtf	d1, d1
	fadd	d1, d0, d1
	ldr	x8, [sp, #32]
	ldr	w9, [sp, #20]
	ldur	w10, [x29, #-28]
	add	w9, w9, w10
	add	x8, x8, w9, sxtw #3
	ldr	d0, [x8]
	fadd	d0, d0, d1
	str	d0, [x8]
	b	LBB0_9
LBB0_9:                                 ;   in Loop: Header=BB0_7 Depth=2
	ldur	w8, [x29, #-28]
	add	w8, w8, #1
	stur	w8, [x29, #-28]
	b	LBB0_7
LBB0_10:                                ;   in Loop: Header=BB0_5 Depth=1
	b	LBB0_11
LBB0_11:                                ;   in Loop: Header=BB0_5 Depth=1
	ldur	w8, [x29, #-24]
	add	w8, w8, #1
	stur	w8, [x29, #-24]
	b	LBB0_5
LBB0_12:
	movi	d0, #0000000000000000
	str	d0, [sp, #40]
	stur	wzr, [x29, #-24]
	b	LBB0_13
LBB0_13:                                ; =>This Inner Loop Header: Depth=1
	ldur	w8, [x29, #-24]
	ldur	w9, [x29, #-20]
	ldur	w10, [x29, #-20]
	mul	w9, w9, w10
	subs	w8, w8, w9
	cset	w8, ge
	tbnz	w8, #0, LBB0_16
	b	LBB0_14
LBB0_14:                                ;   in Loop: Header=BB0_13 Depth=1
	ldr	x8, [sp, #32]
	ldursw	x9, [x29, #-24]
	ldr	d1, [x8, x9, lsl #3]
	ldr	d0, [sp, #40]
	fadd	d0, d0, d1
	str	d0, [sp, #40]
	b	LBB0_15
LBB0_15:                                ;   in Loop: Header=BB0_13 Depth=1
	ldur	w8, [x29, #-24]
	add	w8, w8, #1
	stur	w8, [x29, #-24]
	b	LBB0_13
LBB0_16:
	ldr	d0, [sp, #40]
	mov	x8, sp
	str	d0, [x8]
	adrp	x0, l_.str.1@PAGE
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	ldr	x0, [sp, #32]
	bl	_free
	mov	w0, #0
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	add	sp, sp, #96
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"fast\n"

l_.str.1:                               ; @.str.1
	.asciz	"Done. sum = %15.3f\n"

.subsections_via_symbols
