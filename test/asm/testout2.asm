.text:

exit:
  addi	sp,sp,-16
  li	a1,0
  sw	s0,8(sp)
  sw	ra,12(sp)
  mv	s0,a0
  jal	ra, __call_exitprocs
  lw	a0,64(gp) # 80003850 <_global_impure_ptr>
  lw	a5,60(a0)
  beqz	a5, exit+0x28
  jalr	a5
  mv	a0,s0
  jal	ra, _exit

register_fini:
	li	a5,0
	beqz	a5,80000044 <register_fini+0x14>
	lui	a0,0x80000
	addi	a0,a0,1036 # 8000040c <__BSS_END__+0xffffcb50>
	j	800003f8 <atexit>
	ret

_start:
	auipc	gp,0x3
	addi	gp,gp,1992 # 80003810 <__global_pointer$>
	addi	a0,gp,84 # 80003864 <__malloc_max_total_mem>
	addi	a2,gp,172 # 800038bc <__BSS_END__>
	sub	a2,a2,a0
	li	a1,0
	jal	ra,800001e0 <memset>
	auipc	a0,0x0
	addi	a0,a0,916 # 800003f8 <atexit>
	beqz	a0,8000007c <_start+0x34>
	auipc	a0,0x0
	addi	a0,a0,924 # 8000040c <__libc_fini_array>
	jal	ra,800003f8 <atexit>
	jal	ra,80000144 <__libc_init_array>
	lw	a0,0(sp)
	addi	a1,sp,4
	li	a2,0
	jal	ra,800000f8 <main>
	j	80000000 <exit>

__do_global_dtors_aux:
	addi	sp,sp,-16
	sw	s0,8(sp)
	lbu	a5,104(gp) # 80003878 <completed.1>
	sw	ra,12(sp)
	bnez	a5,800000c8 <__do_global_dtors_aux+0x34>
	li	a5,0
	beqz	a5,800000c0 <__do_global_dtors_aux+0x2c>
	lui	a0,0x80003
	mv	a0,a0
	auipc	ra,0x0
	jalr	zero # 0 <exit-0x80000000>
	li	a5,1
	sb	a5,104(gp) # 80003878 <completed.1>
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	ret

frame_dummy:
	li	a5,0
	beqz	a5,800000f4 <frame_dummy+0x1c>
	lui	a0,0x80003
	addi	a1,gp,108 # 8000387c <object.0>
	mv	a0,a0
	auipc	t1,0x0
	jr	zero # 0 <exit-0x80000000>
	ret

main:
# 代码段.
.text
# `main` 函数, 程序的入口.
.globl main
main:
  addi  sp, sp, -16
	addi	sp,sp,-16
  sw    ra, 12(sp)
	sw	ra,12(sp)
  sw    s0, 8(sp)
	sw	s0,8(sp)
  sw    s1, 4(sp)
	sw	s1,4(sp)
  la    s0, hello_str
	auipc	s0,0x3
	addi	s0,s0,-248 # 80003010 <__DATA_BEGIN__>
  li    s1, 0
	li	s1,0
1:
  add   a0, s0, s1
	add	a0,s0,s1
  lbu   a0, 0(a0)
	lbu	a0,0(a0) # 80003000 <__BSS_END__+0xfffff744>
  beqz  a0, 1f
	beqz	a0,8000012c <main+0x34>
#   li    a7, 0x01           # SBI_CONSOLE_PUTCHAR
#   mv    a0, a0             # 将 ASCII 码复制到 a0 中
#   li    a1, 0x01           # 输出流 ID：1（控制台）
#   ecall
#   nop                     # 等待 SBI 调用完成
  call putchar
	jal	ra,800002c4 <putchar>
  addi  s1, s1, 1
	addi	s1,s1,1
  j     1b
	j	80000114 <main+0x1c>
1:
  li    a0, 0
	li	a0,0
#   li    a7, 0x00           # SBI_CONSOLE_GETCHAR
#   li    a1, 0x01           # 输入流 ID：1（控制台）
#   ecall
#   nop                     # 等待 SBI 调用完成
  lw    s1, 4(sp)
	lw	s1,4(sp)
  lw    s0, 8(sp)
	lw	s0,8(sp)
  lw    ra, 12(sp)
	lw	ra,12(sp)
  addi  sp, sp, 16
	addi	sp,sp,16
  ret
	ret

__libc_init_array:
	addi	sp,sp,-16
	sw	s0,8(sp)
	lui	a5,0x80003
	lui	s0,0x80003
	sw	s2,0(sp)
	addi	a5,a5,4 # 80003004 <__BSS_END__+0xfffff748>
	addi	a4,s0,4 # 80003004 <__BSS_END__+0xfffff748>
	sw	ra,12(sp)
	sw	s1,4(sp)
	sub	s2,a5,a4
	beq	a5,a4,80000190 <__libc_init_array+0x4c>
	srai	s2,s2,0x2
	addi	s0,s0,4
	li	s1,0
	lw	a5,0(s0)
	addi	s1,s1,1
	addi	s0,s0,4
	jalr	a5
	bltu	s1,s2,8000017c <__libc_init_array+0x38>
	lui	s0,0x80003
	lui	a5,0x80003
	addi	a5,a5,12 # 8000300c <__BSS_END__+0xfffff750>
	addi	a4,s0,4 # 80003004 <__BSS_END__+0xfffff748>
	sub	s2,a5,a4
	srai	s2,s2,0x2
	beq	a5,a4,800001c8 <__libc_init_array+0x84>
	addi	s0,s0,4
	li	s1,0
	lw	a5,0(s0)
	addi	s1,s1,1
	addi	s0,s0,4
	jalr	a5
	bltu	s1,s2,800001b4 <__libc_init_array+0x70>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	lw	s2,0(sp)
	addi	sp,sp,16
	ret

memset:
	li	t1,15
	mv	a4,a0
	bgeu	t1,a2,80000224 <memset+0x44>
	andi	a5,a4,15
	bnez	a5,80000290 <memset+0xb0>
	bnez	a1,80000278 <memset+0x98>
	andi	a3,a2,-16
	andi	a2,a2,15
	add	a3,a3,a4
	sw	a1,0(a4)
	sw	a1,4(a4)
	sw	a1,8(a4)
	sw	a1,12(a4)
	addi	a4,a4,16
	bltu	a4,a3,80000204 <memset+0x24>
	bnez	a2,80000224 <memset+0x44>
	ret
	sub	a3,t1,a2
	slli	a3,a3,0x2
	auipc	t0,0x0
	add	a3,a3,t0
	jr	12(a3)
	sb	a1,14(a4)
	sb	a1,13(a4)
	sb	a1,12(a4)
	sb	a1,11(a4)
	sb	a1,10(a4)
	sb	a1,9(a4)
	sb	a1,8(a4)
	sb	a1,7(a4)
	sb	a1,6(a4)
	sb	a1,5(a4)
	sb	a1,4(a4)
	sb	a1,3(a4)
	sb	a1,2(a4)
	sb	a1,1(a4)
	sb	a1,0(a4)
	ret
	zext.b	a1,a1
	slli	a3,a1,0x8
	or	a1,a1,a3
	slli	a3,a1,0x10
	or	a1,a1,a3
	j	800001f8 <memset+0x18>
	slli	a3,a5,0x2
	auipc	t0,0x0
	add	a3,a3,t0
	mv	t0,ra
	jalr	-96(a3)
	mv	ra,t0
	addi	a5,a5,-16
	sub	a4,a4,a5
	add	a2,a2,a5
	bgeu	t1,a2,80000224 <memset+0x44>
	j	800001f4 <memset+0x14>

_putchar_r:
	lw	a2,8(a0)
	j	80000468 <_putc_r>

putchar:
	lw	a5,72(gp) # 80003858 <_impure_ptr>
	mv	a1,a0
	lw	a2,8(a5)
	mv	a0,a5
	j	80000468 <_putc_r>

__call_exitprocs:
	addi	sp,sp,-48
	sw	s4,24(sp)
	lw	s4,64(gp) # 80003850 <_global_impure_ptr>
	sw	s2,32(sp)
	sw	ra,44(sp)
	lw	s2,328(s4)
	sw	s0,40(sp)
	sw	s1,36(sp)
	sw	s3,28(sp)
	sw	s5,20(sp)
	sw	s6,16(sp)
	sw	s7,12(sp)
	sw	s8,8(sp)
	beqz	s2,8000034c <__call_exitprocs+0x74>
	mv	s6,a0
	mv	s7,a1
	li	s5,1
	li	s3,-1
	lw	s1,4(s2)
	addi	s0,s1,-1
	bltz	s0,8000034c <__call_exitprocs+0x74>
	slli	s1,s1,0x2
	add	s1,s2,s1
	beqz	s7,8000037c <__call_exitprocs+0xa4>
	lw	a5,260(s1)
	beq	a5,s7,8000037c <__call_exitprocs+0xa4>
	addi	s0,s0,-1
	addi	s1,s1,-4
	bne	s0,s3,80000334 <__call_exitprocs+0x5c>
	lw	ra,44(sp)
	lw	s0,40(sp)
	lw	s1,36(sp)
	lw	s2,32(sp)
	lw	s3,28(sp)
	lw	s4,24(sp)
	lw	s5,20(sp)
	lw	s6,16(sp)
	lw	s7,12(sp)
	lw	s8,8(sp)
	addi	sp,sp,48
	ret
	lw	a5,4(s2)
	lw	a3,4(s1)
	addi	a5,a5,-1
	beq	a5,s0,800003e4 <__call_exitprocs+0x10c>
	sw	zero,4(s1)
	beqz	a3,80000340 <__call_exitprocs+0x68>
	lw	a5,392(s2)
	sll	a4,s5,s0
	lw	s8,4(s2)
	and	a5,a4,a5
	bnez	a5,800003c8 <__call_exitprocs+0xf0>
	jalr	a3
	lw	a4,4(s2)
	lw	a5,328(s4)
	bne	a4,s8,800003bc <__call_exitprocs+0xe4>
	beq	a5,s2,80000340 <__call_exitprocs+0x68>
	beqz	a5,8000034c <__call_exitprocs+0x74>
	mv	s2,a5
	j	80000320 <__call_exitprocs+0x48>
	lw	a5,396(s2)
	lw	a1,132(s1)
	and	a4,a4,a5
	bnez	a4,800003ec <__call_exitprocs+0x114>
	mv	a0,s6
	jalr	a3
	j	800003ac <__call_exitprocs+0xd4>
	sw	s0,4(s2)
	j	80000390 <__call_exitprocs+0xb8>
	mv	a0,a1
	jalr	a3
	j	800003ac <__call_exitprocs+0xd4>

atexit:
	mv	a1,a0
	li	a3,0
	li	a2,0
	li	a0,0
	j	8000086c <__register_exitproc>

__libc_fini_array:
	addi	sp,sp,-16
	sw	s0,8(sp)
	lui	a5,0x80003
	lui	s0,0x80003
	addi	a5,a5,12 # 8000300c <__BSS_END__+0xfffff750>
	addi	s0,s0,16 # 80003010 <__BSS_END__+0xfffff754>
	sub	s0,s0,a5
	sw	s1,4(sp)
	sw	ra,12(sp)
	srai	s1,s0,0x2
	beqz	s1,80000454 <__libc_fini_array+0x48>
	addi	s0,s0,-4
	add	s0,s0,a5
	lw	a5,0(s0)
	addi	s1,s1,-1
	addi	s0,s0,-4
	jalr	a5
	bnez	s1,80000440 <__libc_fini_array+0x34>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_putc_r:
	addi	sp,sp,-32
	sw	s2,24(sp)
	sw	ra,28(sp)
	mv	s2,a0
	beqz	a0,80000484 <_putc_r+0x1c>
	lw	a5,56(a0)
	beqz	a5,800004cc <_putc_r+0x64>
	lw	a5,8(a2)
	addi	a5,a5,-1
	sw	a5,8(a2)
	bgez	a5,800004a8 <_putc_r+0x40>
	lw	a4,24(a2)
	blt	a5,a4,800004e4 <_putc_r+0x7c>
	zext.b	a5,a1
	li	a4,10
	beq	a5,a4,800004e4 <_putc_r+0x7c>
	lw	a5,0(a2)
	zext.b	a0,a1
	addi	a4,a5,1
	sw	a4,0(a2)
	sb	a1,0(a5)
	lw	ra,28(sp)
	lw	s2,24(sp)
	addi	sp,sp,32
	ret
	sw	a2,12(sp)
	sw	a1,8(sp)
	jal	ra,80000f80 <__sinit>
	lw	a2,12(sp)
	lw	a1,8(sp)
	j	80000484 <_putc_r+0x1c>
	lw	ra,28(sp)
	mv	a0,s2
	lw	s2,24(sp)
	addi	sp,sp,32
	j	800005a4 <__swbuf_r>

putc:
	addi	sp,sp,-16
	sw	s2,0(sp)
	lw	s2,72(gp) # 80003858 <_impure_ptr>
	sw	s0,8(sp)
	sw	s1,4(sp)
	sw	ra,12(sp)
	mv	s1,a0
	mv	s0,a1
	beqz	s2,80000524 <putc+0x2c>
	lw	a5,56(s2)
	beqz	a5,80000574 <putc+0x7c>
	lw	a5,8(s0)
	addi	a5,a5,-1
	sw	a5,8(s0)
	bgez	a5,80000548 <putc+0x50>
	lw	a4,24(s0)
	blt	a5,a4,80000580 <putc+0x88>
	zext.b	a5,s1
	li	a4,10
	beq	a5,a4,80000580 <putc+0x88>
	lw	a5,0(s0)
	zext.b	a0,s1
	addi	a4,a5,1
	sw	a4,0(s0)
	sb	s1,0(a5)
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	lw	s2,0(sp)
	addi	sp,sp,16
	ret
	mv	a0,s2
	jal	ra,80000f80 <__sinit>
	j	80000524 <putc+0x2c>
	mv	a2,s0
	lw	s0,8(sp)
	lw	ra,12(sp)
	mv	a1,s1
	mv	a0,s2
	lw	s1,4(sp)
	lw	s2,0(sp)
	addi	sp,sp,16
	j	800005a4 <__swbuf_r>

__swbuf_r:
	addi	sp,sp,-32
	sw	s0,24(sp)
	sw	s1,20(sp)
	sw	s2,16(sp)
	sw	ra,28(sp)
	sw	s3,12(sp)
	mv	s2,a0
	mv	s1,a1
	mv	s0,a2
	beqz	a0,800005d4 <__swbuf_r+0x30>
	lw	a5,56(a0)
	beqz	a5,80000710 <__swbuf_r+0x16c>
	lw	a5,24(s0)
	lh	a4,12(s0)
	sw	a5,8(s0)
	andi	a5,a4,8
	beqz	a5,80000668 <__swbuf_r+0xc4>
	lw	a5,16(s0)
	beqz	a5,80000668 <__swbuf_r+0xc4>
	slli	a3,a4,0x12
	zext.b	s3,s1
	zext.b	s1,s1
	bgez	a3,80000690 <__swbuf_r+0xec>
	lw	a4,0(s0)
	lw	a3,20(s0)
	sub	a5,a4,a5
	bge	a5,a3,800006c0 <__swbuf_r+0x11c>
	lw	a3,8(s0)
	addi	a2,a4,1
	sw	a2,0(s0)
	addi	a3,a3,-1
	sw	a3,8(s0)
	sb	s3,0(a4)
	lw	a4,20(s0)
	addi	a5,a5,1
	beq	a4,a5,800006f8 <__swbuf_r+0x154>
	lhu	a5,12(s0)
	andi	a5,a5,1
	beqz	a5,80000648 <__swbuf_r+0xa4>
	li	a5,10
	beq	s1,a5,800006f8 <__swbuf_r+0x154>
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	mv	a0,s1
	lw	s1,20(sp)
	addi	sp,sp,32
	ret
	mv	a1,s0
	mv	a0,s2
	jal	ra,8000072c <__swsetup_r>
	bnez	a0,80000708 <__swbuf_r+0x164>
	lh	a4,12(s0)
	zext.b	s3,s1
	lw	a5,16(s0)
	slli	a3,a4,0x12
	zext.b	s1,s1
	bltz	a3,80000600 <__swbuf_r+0x5c>
	lw	a3,100(s0)
	lui	a2,0x2
	or	a4,a4,a2
	lui	a2,0xffffe
	addi	a2,a2,-1 # ffffdfff <__BSS_END__+0x7fffa743>
	and	a3,a3,a2
	sh	a4,12(s0)
	lw	a4,0(s0)
	sw	a3,100(s0)
	lw	a3,20(s0)
	sub	a5,a4,a5
	blt	a5,a3,80000610 <__swbuf_r+0x6c>
	mv	a1,s0
	mv	a0,s2
	jal	ra,80000b50 <_fflush_r>
	bnez	a0,80000708 <__swbuf_r+0x164>
	lw	a4,0(s0)
	lw	a3,8(s0)
	li	a5,1
	addi	a2,a4,1
	addi	a3,a3,-1
	sw	a2,0(s0)
	sw	a3,8(s0)
	sb	s3,0(a4)
	lw	a4,20(s0)
	bne	a4,a5,80000634 <__swbuf_r+0x90>
	mv	a1,s0
	mv	a0,s2
	jal	ra,80000b50 <_fflush_r>
	beqz	a0,80000648 <__swbuf_r+0xa4>
	li	s1,-1
	j	80000648 <__swbuf_r+0xa4>
	jal	ra,80000f80 <__sinit>
	j	800005d4 <__swbuf_r+0x30>

__swbuf:
	mv	a5,a0
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	mv	a2,a1
	mv	a1,a5
	j	800005a4 <__swbuf_r>

__swsetup_r:
	lw	a5,72(gp) # 80003858 <_impure_ptr>
	addi	sp,sp,-16
	sw	s0,8(sp)
	sw	s1,4(sp)
	sw	ra,12(sp)
	mv	s1,a0
	mv	s0,a1
	beqz	a5,80000754 <__swsetup_r+0x28>
	lw	a4,56(a5)
	beqz	a4,800007d0 <__swsetup_r+0xa4>
	lh	a5,12(s0)
	andi	a4,a5,8
	beqz	a4,800007e4 <__swsetup_r+0xb8>
	lw	a4,16(s0)
	beqz	a4,80000804 <__swsetup_r+0xd8>
	andi	a3,a5,1
	beqz	a3,8000079c <__swsetup_r+0x70>
	lw	a3,20(s0)
	sw	zero,8(s0)
	li	a0,0
	neg	a3,a3
	sw	a3,24(s0)
	beqz	a4,800007b8 <__swsetup_r+0x8c>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	andi	a3,a5,2
	li	a2,0
	bnez	a3,800007ac <__swsetup_r+0x80>
	lw	a2,20(s0)
	sw	a2,8(s0)
	li	a0,0
	bnez	a4,80000788 <__swsetup_r+0x5c>
	andi	a4,a5,128
	beqz	a4,80000788 <__swsetup_r+0x5c>
	ori	a5,a5,64
	sh	a5,12(s0)
	li	a0,-1
	j	80000788 <__swsetup_r+0x5c>
	mv	a0,a5
	jal	ra,80000f80 <__sinit>
	lh	a5,12(s0)
	andi	a4,a5,8
	bnez	a4,80000760 <__swsetup_r+0x34>
	andi	a4,a5,16
	beqz	a4,8000085c <__swsetup_r+0x130>
	andi	a4,a5,4
	bnez	a4,80000828 <__swsetup_r+0xfc>
	lw	a4,16(s0)
	ori	a5,a5,8
	sh	a5,12(s0)
	bnez	a4,80000768 <__swsetup_r+0x3c>
	andi	a3,a5,640
	li	a2,512
	beq	a3,a2,80000768 <__swsetup_r+0x3c>
	mv	a1,s0
	mv	a0,s1
	jal	ra,80001540 <__smakebuf_r>
	lh	a5,12(s0)
	lw	a4,16(s0)
	j	80000768 <__swsetup_r+0x3c>
	lw	a1,48(s0)
	beqz	a1,80000848 <__swsetup_r+0x11c>
	addi	a4,s0,64
	beq	a1,a4,80000844 <__swsetup_r+0x118>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	lh	a5,12(s0)
	sw	zero,48(s0)
	lw	a4,16(s0)
	andi	a5,a5,-37
	sw	zero,4(s0)
	sw	a4,0(s0)
	j	800007f8 <__swsetup_r+0xcc>
	li	a4,9
	sw	a4,0(s1)
	ori	a5,a5,64
	j	800007c4 <__swsetup_r+0x98>

__register_exitproc:
	lw	a4,64(gp) # 80003850 <_global_impure_ptr>
	lw	a5,328(a4)
	beqz	a5,800008cc <__register_exitproc+0x60>
	lw	a4,4(a5)
	li	a6,31
	blt	a6,a4,800008fc <__register_exitproc+0x90>
	slli	a6,a4,0x2
	beqz	a0,800008b4 <__register_exitproc+0x48>
	add	t1,a5,a6
	sw	a2,136(t1) # 80000174 <__libc_init_array+0x30>
	lw	a7,392(a5)
	li	a2,1
	sll	a2,a2,a4
	or	a7,a7,a2
	sw	a7,392(a5)
	sw	a3,264(t1)
	li	a3,2
	beq	a0,a3,800008d8 <__register_exitproc+0x6c>
	addi	a4,a4,1
	sw	a4,4(a5)
	add	a5,a5,a6
	sw	a1,8(a5)
	li	a0,0
	ret
	addi	a5,a4,332
	sw	a5,328(a4)
	j	80000878 <__register_exitproc+0xc>
	lw	a3,396(a5)
	addi	a4,a4,1
	sw	a4,4(a5)
	or	a3,a3,a2
	sw	a3,396(a5)
	add	a5,a5,a6
	sw	a1,8(a5)
	li	a0,0
	ret
	li	a0,-1
	ret

__sflush_r:
	lh	a5,12(a1)
	addi	sp,sp,-32
	sw	s0,24(sp)
	sw	s3,12(sp)
	sw	ra,28(sp)
	sw	s1,20(sp)
	sw	s2,16(sp)
	andi	a4,a5,8
	mv	s0,a1
	mv	s3,a0
	bnez	a4,80000a50 <__sflush_r+0x14c>
	lui	a4,0x1
	addi	a4,a4,-2048 # 800 <exit-0x7ffff800>
	lw	a3,4(a1)
	or	a4,a5,a4
	sh	a4,12(a1)
	blez	a3,80000ad4 <__sflush_r+0x1d0>
	lw	a6,40(s0)
	beqz	a6,80000a30 <__sflush_r+0x12c>
	srli	a5,a5,0xc
	lw	s1,0(s3)
	andi	a5,a5,1
	sw	zero,0(s3)
	lw	a1,28(s0)
	bnez	a5,80000ae4 <__sflush_r+0x1e0>
	li	a2,0
	li	a3,1
	mv	a0,s3
	jalr	a6
	li	a5,-1
	mv	a2,a0
	beq	a0,a5,80000b20 <__sflush_r+0x21c>
	lh	a4,12(s0)
	lw	a6,40(s0)
	lw	a1,28(s0)
	andi	a4,a4,4
	beqz	a4,800009b0 <__sflush_r+0xac>
	lw	a4,4(s0)
	lw	a5,48(s0)
	sub	a2,a2,a4
	beqz	a5,800009b0 <__sflush_r+0xac>
	lw	a5,60(s0)
	sub	a2,a2,a5
	li	a3,0
	mv	a0,s3
	jalr	a6
	li	a5,-1
	bne	a0,a5,80000aec <__sflush_r+0x1e8>
	lw	a3,0(s3)
	li	a5,29
	bltu	a5,a3,80000aa8 <__sflush_r+0x1a4>
	lui	a4,0xdfc00
	addi	a4,a4,-2 # dfbffffe <__BSS_END__+0x5fbfc742>
	sra	a4,a4,a3
	andi	a4,a4,1
	lh	a5,12(s0)
	bnez	a4,80000aac <__sflush_r+0x1a8>
	lw	a2,16(s0)
	lui	a4,0xfffff
	addi	a4,a4,2047 # fffff7ff <__BSS_END__+0x7fffbf43>
	and	a4,a5,a4
	srli	a5,a5,0xc
	sh	a4,12(s0)
	sw	zero,4(s0)
	sw	a2,0(s0)
	andi	a5,a5,1
	bnez	a5,80000b44 <__sflush_r+0x240>
	lw	a1,48(s0)
	sw	s1,0(s3)
	beqz	a1,80000a30 <__sflush_r+0x12c>
	addi	a5,s0,64
	beq	a1,a5,80000a2c <__sflush_r+0x128>
	mv	a0,s3
	jal	ra,800010f0 <_free_r>
	sw	zero,48(s0)
	li	a0,0
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	addi	sp,sp,32
	ret
	lw	s2,16(a1)
	beqz	s2,80000a30 <__sflush_r+0x12c>
	lw	s1,0(a1)
	andi	a5,a5,3
	sw	s2,0(a1)
	sub	s1,s1,s2
	li	a4,0
	bnez	a5,80000a74 <__sflush_r+0x170>
	lw	a4,20(a1)
	sw	a4,8(s0)
	bgtz	s1,80000a88 <__sflush_r+0x184>
	j	80000a30 <__sflush_r+0x12c>
	add	s2,s2,a0
	blez	s1,80000a30 <__sflush_r+0x12c>
	lw	a5,36(s0)
	lw	a1,28(s0)
	mv	a3,s1
	mv	a2,s2
	mv	a0,s3
	jalr	a5
	sub	s1,s1,a0
	bgtz	a0,80000a80 <__sflush_r+0x17c>
	lh	a5,12(s0)
	ori	a5,a5,64
	lw	ra,28(sp)
	sh	a5,12(s0)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	li	a0,-1
	addi	sp,sp,32
	ret
	lw	a3,60(a1)
	bgtz	a3,80000948 <__sflush_r+0x44>
	li	a0,0
	j	80000a34 <__sflush_r+0x130>
	lw	a2,80(s0)
	j	80000990 <__sflush_r+0x8c>
	lh	a5,12(s0)
	lw	a3,16(s0)
	lui	a4,0xfffff
	addi	a4,a4,2047 # fffff7ff <__BSS_END__+0x7fffbf43>
	and	a4,a5,a4
	srli	a5,a5,0xc
	sh	a4,12(s0)
	sw	zero,4(s0)
	sw	a3,0(s0)
	andi	a5,a5,1
	beqz	a5,80000a10 <__sflush_r+0x10c>
	sw	a0,80(s0)
	j	80000a10 <__sflush_r+0x10c>
	lw	a5,0(s3)
	beqz	a5,80000984 <__sflush_r+0x80>
	li	a4,29
	beq	a5,a4,80000b38 <__sflush_r+0x234>
	li	a4,22
	bne	a5,a4,80000aa8 <__sflush_r+0x1a4>
	sw	s1,0(s3)
	li	a0,0
	j	80000a34 <__sflush_r+0x130>
	bnez	a3,80000a10 <__sflush_r+0x10c>
	sw	a0,80(s0)
	j	80000a10 <__sflush_r+0x10c>

_fflush_r:
	addi	sp,sp,-32
	sw	s0,24(sp)
	sw	ra,28(sp)
	mv	s0,a0
	beqz	a0,80000b6c <_fflush_r+0x1c>
	lw	a5,56(a0)
	beqz	a5,80000b88 <_fflush_r+0x38>
	lh	a5,12(a1)
	bnez	a5,80000b9c <_fflush_r+0x4c>
	lw	ra,28(sp)
	lw	s0,24(sp)
	li	a0,0
	addi	sp,sp,32
	ret
	sw	a1,12(sp)
	jal	ra,80000f80 <__sinit>
	lw	a1,12(sp)
	lh	a5,12(a1)
	beqz	a5,80000b74 <_fflush_r+0x24>
	mv	a0,s0
	lw	s0,24(sp)
	lw	ra,28(sp)
	addi	sp,sp,32
	j	80000904 <__sflush_r>

fflush:
	beqz	a0,80000c1c <fflush+0x6c>
	addi	sp,sp,-32
	sw	s0,24(sp)
	mv	s0,a0
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	sw	ra,28(sp)
	beqz	a0,80000bd4 <fflush+0x24>
	lw	a5,56(a0)
	beqz	a5,80000c04 <fflush+0x54>
	lh	a5,12(s0)
	bnez	a5,80000bf0 <fflush+0x40>
	lw	ra,28(sp)
	lw	s0,24(sp)
	li	a0,0
	addi	sp,sp,32
	ret
	mv	a1,s0
	lw	s0,24(sp)
	lw	ra,28(sp)
	addi	sp,sp,32
	j	80000904 <__sflush_r>
	sw	a0,12(sp)
	jal	ra,80000f80 <__sinit>
	lh	a5,12(s0)
	lw	a0,12(sp)
	beqz	a5,80000bdc <fflush+0x2c>
	j	80000bf0 <fflush+0x40>
	lw	a0,64(gp) # 80003850 <_global_impure_ptr>
	lui	a1,0x80001
	addi	a1,a1,-1200 # 80000b50 <__BSS_END__+0xffffd294>
	j	8000148c <_fwalk_reent>

__fp_lock:
	li	a0,0
	ret

_cleanup_r:
	lui	a1,0x80002
	addi	a1,a1,432 # 800021b0 <__BSS_END__+0xffffe8f4>
	j	8000148c <_fwalk_reent>

__fp_unlock:
	li	a0,0
	ret

80000c48 <__sinit.part.0>:
	addi	sp,sp,-32
	lui	a5,0x80001
	sw	ra,28(sp)
	sw	s0,24(sp)
	sw	s1,20(sp)
	sw	s2,16(sp)
	sw	s3,12(sp)
	sw	s4,8(sp)
	sw	s5,4(sp)
	sw	s6,0(sp)
	lw	s0,4(a0)
	addi	a5,a5,-972 # 80000c34 <__BSS_END__+0xffffd378>
	sw	a5,60(a0)
	addi	a4,a0,748
	li	a5,3
	sw	a4,744(a0)
	sw	a5,740(a0)
	sw	zero,736(a0)
	li	a5,4
	mv	s2,a0
	sw	a5,12(s0)
	li	a2,8
	li	a1,0
	sw	zero,0(s0)
	sw	zero,4(s0)
	sw	zero,8(s0)
	sw	zero,100(s0)
	sw	zero,16(s0)
	sw	zero,20(s0)
	sw	zero,24(s0)
	addi	a0,s0,92
	jal	ra,800001e0 <memset>
	lui	s6,0x80002
	lw	s1,8(s2)
	lui	s5,0x80002
	lui	s4,0x80002
	lui	s3,0x80002
	addi	s6,s6,-128 # 80001f80 <__BSS_END__+0xffffe6c4>
	addi	s5,s5,-28 # 80001fe4 <__BSS_END__+0xffffe728>
	addi	s4,s4,108 # 8000206c <__BSS_END__+0xffffe7b0>
	addi	s3,s3,228 # 800020e4 <__BSS_END__+0xffffe828>
	lui	a5,0x10
	sw	s6,32(s0)
	sw	s5,36(s0)
	sw	s4,40(s0)
	sw	s3,44(s0)
	sw	s0,28(s0)
	addi	a5,a5,9 # 10009 <exit-0x7ffefff7>
	sw	a5,12(s1)
	li	a2,8
	li	a1,0
	sw	zero,0(s1)
	sw	zero,4(s1)
	sw	zero,8(s1)
	sw	zero,100(s1)
	sw	zero,16(s1)
	sw	zero,20(s1)
	sw	zero,24(s1)
	addi	a0,s1,92
	jal	ra,800001e0 <memset>
	lw	s0,12(s2)
	lui	a5,0x20
	sw	s6,32(s1)
	sw	s5,36(s1)
	sw	s4,40(s1)
	sw	s3,44(s1)
	sw	s1,28(s1)
	addi	a5,a5,18 # 20012 <exit-0x7ffdffee>
	sw	a5,12(s0)
	sw	zero,0(s0)
	sw	zero,4(s0)
	sw	zero,8(s0)
	sw	zero,100(s0)
	sw	zero,16(s0)
	sw	zero,20(s0)
	sw	zero,24(s0)
	addi	a0,s0,92
	li	a2,8
	li	a1,0
	jal	ra,800001e0 <memset>
	lw	ra,28(sp)
	sw	s6,32(s0)
	sw	s5,36(s0)
	sw	s4,40(s0)
	sw	s3,44(s0)
	sw	s0,28(s0)
	lw	s0,24(sp)
	li	a5,1
	sw	a5,56(s2)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	lw	s4,8(sp)
	lw	s5,4(sp)
	lw	s6,0(sp)
	addi	sp,sp,32
	ret

__sfmoreglue:
	addi	sp,sp,-16
	addi	a5,a1,-1
	sw	s0,8(sp)
	slli	s0,a5,0x1
	add	s0,s0,a5
	slli	s0,s0,0x2
	add	s0,s0,a5
	slli	s0,s0,0x3
	sw	s2,0(sp)
	mv	s2,a1
	addi	a1,s0,116
	sw	s1,4(sp)
	sw	ra,12(sp)
	jal	ra,800017a8 <_malloc_r>
	mv	s1,a0
	beqz	a0,80000e28 <__sfmoreglue+0x5c>
	addi	a0,a0,12
	sw	zero,0(s1)
	sw	s2,4(s1)
	sw	a0,8(s1)
	addi	a2,s0,104
	li	a1,0
	jal	ra,800001e0 <memset>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s2,0(sp)
	mv	a0,s1
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

__sfp:
	addi	sp,sp,-32
	sw	s2,16(sp)
	lw	s2,64(gp) # 80003850 <_global_impure_ptr>
	sw	s3,12(sp)
	sw	ra,28(sp)
	lw	a5,56(s2)
	sw	s0,24(sp)
	sw	s1,20(sp)
	mv	s3,a0
	beqz	a5,80000f14 <__sfp+0xd0>
	addi	s2,s2,736
	li	s1,-1
	lw	a5,4(s2)
	lw	s0,8(s2)
	addi	a5,a5,-1
	bgez	a5,80000e90 <__sfp+0x4c>
	j	80000f04 <__sfp+0xc0>
	addi	s0,s0,104
	beq	a5,s1,80000f04 <__sfp+0xc0>
	lh	a4,12(s0)
	addi	a5,a5,-1
	bnez	a4,80000e88 <__sfp+0x44>
	lui	a5,0xffff0
	addi	a5,a5,1 # ffff0001 <__BSS_END__+0x7ffec745>
	sw	a5,12(s0)
	sw	zero,100(s0)
	sw	zero,0(s0)
	sw	zero,8(s0)
	sw	zero,4(s0)
	sw	zero,16(s0)
	sw	zero,20(s0)
	sw	zero,24(s0)
	li	a2,8
	li	a1,0
	addi	a0,s0,92
	jal	ra,800001e0 <memset>
	sw	zero,48(s0)
	sw	zero,52(s0)
	sw	zero,68(s0)
	sw	zero,72(s0)
	lw	ra,28(sp)
	mv	a0,s0
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	addi	sp,sp,32
	ret
	lw	s0,0(s2)
	beqz	s0,80000f20 <__sfp+0xdc>
	mv	s2,s0
	j	80000e74 <__sfp+0x30>
	mv	a0,s2
	jal	ra,80000c48 <__sinit.part.0>
	j	80000e6c <__sfp+0x28>
	li	a1,428
	mv	a0,s3
	jal	ra,800017a8 <_malloc_r>
	mv	s0,a0
	beqz	a0,80000f60 <__sfp+0x11c>
	addi	a0,a0,12
	li	a5,4
	sw	zero,0(s0)
	sw	a5,4(s0)
	sw	a0,8(s0)
	li	a2,416
	li	a1,0
	jal	ra,800001e0 <memset>
	sw	s0,0(s2)
	mv	s2,s0
	j	80000e74 <__sfp+0x30>
	sw	zero,0(s2)
	li	a5,12
	sw	a5,0(s3)
	j	80000ee4 <__sfp+0xa0>

_cleanup:
	lw	a0,64(gp) # 80003850 <_global_impure_ptr>
	lui	a1,0x80002
	addi	a1,a1,432 # 800021b0 <__BSS_END__+0xffffe8f4>
	j	8000148c <_fwalk_reent>

__sinit:
	lw	a5,56(a0)
	beqz	a5,80000f8c <__sinit+0xc>
	ret
	j	80000c48 <__sinit.part.0>

__sfp_lock_acquire:
	ret

__sfp_lock_release:
	ret

__sinit_lock_acquire:
	ret

__sinit_lock_release:
	ret

__fp_lock_all:
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	lui	a1,0x80001
	addi	a1,a1,-980 # 80000c2c <__BSS_END__+0xffffd370>
	j	800013e8 <_fwalk>

__fp_unlock_all:
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	lui	a1,0x80001
	addi	a1,a1,-960 # 80000c40 <__BSS_END__+0xffffd384>
	j	800013e8 <_fwalk>

_malloc_trim_r:
	addi	sp,sp,-32
	sw	s3,12(sp)
	sw	s0,24(sp)
	sw	s1,20(sp)
	sw	s2,16(sp)
	sw	s4,8(sp)
	sw	ra,28(sp)
	mv	s4,a1
	mv	s2,a0
	addi	s3,gp,-968 # 80003448 <__malloc_av_>
	jal	ra,80001f1c <__malloc_lock>
	lw	a4,8(s3)
	lui	a5,0x1
	addi	s0,a5,-17 # fef <exit-0x7ffff011>
	lw	s1,4(a4)
	andi	s1,s1,-4
	add	s0,s1,s0
	sub	s0,s0,s4
	srli	s0,s0,0xc
	addi	s0,s0,-1
	slli	s0,s0,0xc
	blt	s0,a5,80001030 <_malloc_trim_r+0x70>
	li	a1,0
	mv	a0,s2
	jal	ra,80001f24 <_sbrk_r>
	lw	a5,8(s3)
	add	a5,a5,s1
	beq	a0,a5,8000105c <_malloc_trim_r+0x9c>
	mv	a0,s2
	jal	ra,80001f20 <__malloc_unlock>
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	lw	s4,8(sp)
	li	a0,0
	addi	sp,sp,32
	ret
	neg	a1,s0
	mv	a0,s2
	jal	ra,80001f24 <_sbrk_r>
	li	a5,-1
	beq	a0,a5,800010bc <_malloc_trim_r+0xfc>
	addi	a5,gp,132 # 80003894 <__malloc_current_mallinfo>
	lw	a4,0(a5)
	lw	a3,8(s3)
	sub	s1,s1,s0
	ori	s1,s1,1
	sub	a4,a4,s0
	mv	a0,s2
	sw	s1,4(a3)
	sw	a4,0(a5)
	jal	ra,80001f20 <__malloc_unlock>
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	lw	s4,8(sp)
	li	a0,1
	addi	sp,sp,32
	ret
	li	a1,0
	mv	a0,s2
	jal	ra,80001f24 <_sbrk_r>
	lw	a4,8(s3)
	li	a3,15
	sub	a5,a0,a4
	bge	a3,a5,80001030 <_malloc_trim_r+0x70>
	lw	a3,76(gp) # 8000385c <__malloc_sbrk_base>
	ori	a5,a5,1
	sw	a5,4(a4)
	sub	a0,a0,a3
	sw	a0,132(gp) # 80003894 <__malloc_current_mallinfo>
	j	80001030 <_malloc_trim_r+0x70>

_free_r:
	beqz	a1,8000126c <_free_r+0x17c>
	addi	sp,sp,-16
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	s0,a1
	mv	s1,a0
	sw	ra,12(sp)
	jal	ra,80001f1c <__malloc_lock>
	lw	a0,-4(s0)
	addi	a4,s0,-8
	andi	a5,a0,-2
	add	a2,a4,a5
	addi	a1,gp,-968 # 80003448 <__malloc_av_>
	lw	a3,4(a2)
	lw	a6,8(a1)
	andi	a3,a3,-4
	beq	a6,a2,800012cc <_free_r+0x1dc>
	sw	a3,4(a2)
	andi	a0,a0,1
	add	a6,a2,a3
	bnez	a0,800011dc <_free_r+0xec>
	lw	t1,-8(s0)
	lw	a6,4(a6)
	sub	a4,a4,t1
	lw	a7,8(a4)
	addi	a0,gp,-960 # 80003450 <__malloc_av_+0x8>
	add	a5,a5,t1
	andi	a6,a6,1
	beq	a7,a0,800012a0 <_free_r+0x1b0>
	lw	t1,12(a4)
	sw	t1,12(a7)
	sw	a7,8(t1)
	beqz	a6,80001318 <_free_r+0x228>
	ori	a3,a5,1
	sw	a3,4(a4)
	sw	a5,0(a2)
	li	a3,511
	bltu	a3,a5,800011fc <_free_r+0x10c>
	andi	a3,a5,-8
	addi	a3,a3,8
	lw	a0,4(a1)
	add	a3,a1,a3
	lw	a2,0(a3)
	srli	a6,a5,0x5
	li	a5,1
	sll	a5,a5,a6
	or	a5,a5,a0
	addi	a0,a3,-8
	sw	a0,12(a4)
	sw	a2,8(a4)
	sw	a5,4(a1)
	sw	a4,0(a3)
	sw	a4,12(a2)
	lw	s0,8(sp)
	lw	ra,12(sp)
	mv	a0,s1
	lw	s1,4(sp)
	addi	sp,sp,16
	j	80001f20 <__malloc_unlock>
	lw	a0,4(a6)
	andi	a0,a0,1
	beqz	a0,80001270 <_free_r+0x180>
	ori	a3,a5,1
	sw	a3,-4(s0)
	sw	a5,0(a2)
	li	a3,511
	bgeu	a3,a5,80001188 <_free_r+0x98>
	srli	a3,a5,0x9
	li	a2,4
	bltu	a2,a3,80001320 <_free_r+0x230>
	srli	a3,a5,0x6
	addi	a0,a3,57
	addi	a2,a3,56
	slli	a0,a0,0x3
	add	a0,a1,a0
	lw	a3,0(a0)
	addi	a0,a0,-8
	bne	a0,a3,80001234 <_free_r+0x144>
	j	80001378 <_free_r+0x288>
	lw	a3,8(a3)
	beq	a0,a3,80001240 <_free_r+0x150>
	lw	a2,4(a3)
	andi	a2,a2,-4
	bltu	a5,a2,8000122c <_free_r+0x13c>
	lw	a0,12(a3)
	sw	a0,12(a4)
	sw	a3,8(a4)
	lw	s0,8(sp)
	sw	a4,8(a0)
	lw	ra,12(sp)
	mv	a0,s1
	lw	s1,4(sp)
	sw	a4,12(a3)
	addi	sp,sp,16
	j	80001f20 <__malloc_unlock>
	ret
	add	a5,a5,a3
	addi	a0,gp,-960 # 80003450 <__malloc_av_+0x8>
	lw	a3,8(a2)
	beq	a3,a0,80001354 <_free_r+0x264>
	lw	a6,12(a2)
	ori	a0,a5,1
	add	a2,a4,a5
	sw	a6,12(a3)
	sw	a3,8(a6)
	sw	a0,4(a4)
	sw	a5,0(a2)
	j	80001180 <_free_r+0x90>
	bnez	a6,800013d8 <_free_r+0x2e8>
	lw	a1,8(a2)
	lw	a2,12(a2)
	add	a3,a3,a5
	ori	a5,a3,1
	sw	a2,12(a1)
	sw	a1,8(a2)
	sw	a5,4(a4)
	add	a4,a4,a3
	sw	a3,0(a4)
	j	800011c4 <_free_r+0xd4>
	andi	a0,a0,1
	add	a3,a5,a3
	bnez	a0,800012f4 <_free_r+0x204>
	lw	a0,-8(s0)
	sub	a4,a4,a0
	lw	a5,12(a4)
	lw	a2,8(a4)
	add	a3,a3,a0
	sw	a5,12(a2)
	sw	a2,8(a5)
	ori	a2,a3,1
	lw	a5,80(gp) # 80003860 <__malloc_trim_threshold>
	sw	a2,4(a4)
	sw	a4,8(a1)
	bltu	a3,a5,800011c4 <_free_r+0xd4>
	lw	a1,92(gp) # 8000386c <__malloc_top_pad>
	mv	a0,s1
	jal	ra,80000fc0 <_malloc_trim_r>
	j	800011c4 <_free_r+0xd4>
	add	a5,a5,a3
	j	80001278 <_free_r+0x188>
	li	a2,20
	bgeu	a2,a3,80001344 <_free_r+0x254>
	li	a2,84
	bltu	a2,a3,80001394 <_free_r+0x2a4>
	srli	a3,a5,0xc
	addi	a0,a3,111
	addi	a2,a3,110
	slli	a0,a0,0x3
	j	80001218 <_free_r+0x128>
	addi	a0,a3,92
	addi	a2,a3,91
	slli	a0,a0,0x3
	j	80001218 <_free_r+0x128>
	sw	a4,20(a1)
	sw	a4,16(a1)
	ori	a3,a5,1
	sw	a0,12(a4)
	sw	a0,8(a4)
	sw	a3,4(a4)
	add	a4,a4,a5
	sw	a5,0(a4)
	j	800011c4 <_free_r+0xd4>
	lw	a6,4(a1)
	srai	a2,a2,0x2
	li	a5,1
	sll	a5,a5,a2
	or	a5,a5,a6
	sw	a5,4(a1)
	j	80001244 <_free_r+0x154>
	li	a2,340
	bltu	a2,a3,800013b0 <_free_r+0x2c0>
	srli	a3,a5,0xf
	addi	a0,a3,120
	addi	a2,a3,119
	slli	a0,a0,0x3
	j	80001218 <_free_r+0x128>
	li	a2,1364
	bltu	a2,a3,800013cc <_free_r+0x2dc>
	srli	a3,a5,0x12
	addi	a0,a3,125
	addi	a2,a3,124
	slli	a0,a0,0x3
	j	80001218 <_free_r+0x128>
	li	a0,1016
	li	a2,126
	j	80001218 <_free_r+0x128>
	ori	a3,a5,1
	sw	a3,4(a4)
	sw	a5,0(a2)
	j	800011c4 <_free_r+0xd4>

_fwalk:
	addi	sp,sp,-32
	sw	s2,16(sp)
	sw	s3,12(sp)
	sw	s4,8(sp)
	sw	s5,4(sp)
	sw	s6,0(sp)
	sw	ra,28(sp)
	sw	s0,24(sp)
	sw	s1,20(sp)
	mv	s6,a1
	addi	s5,a0,736
	li	s4,0
	li	s3,1
	li	s2,-1
	lw	s1,4(s5)
	lw	s0,8(s5)
	addi	s1,s1,-1
	bltz	s1,80001458 <_fwalk+0x70>
	lhu	a5,12(s0)
	addi	s1,s1,-1
	bgeu	s3,a5,80001450 <_fwalk+0x68>
	lh	a5,14(s0)
	mv	a0,s0
	beq	a5,s2,80001450 <_fwalk+0x68>
	jalr	s6
	or	s4,s4,a0
	addi	s0,s0,104
	bne	s1,s2,80001430 <_fwalk+0x48>
	lw	s5,0(s5)
	bnez	s5,80001420 <_fwalk+0x38>
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	lw	s5,4(sp)
	lw	s6,0(sp)
	mv	a0,s4
	lw	s4,8(sp)
	addi	sp,sp,32
	ret

_fwalk_reent:
	addi	sp,sp,-48
	sw	s2,32(sp)
	sw	s3,28(sp)
	sw	s4,24(sp)
	sw	s5,20(sp)
	sw	s6,16(sp)
	sw	s7,12(sp)
	sw	ra,44(sp)
	sw	s0,40(sp)
	sw	s1,36(sp)
	mv	s5,a0
	mv	s7,a1
	addi	s6,a0,736
	li	s4,0
	li	s3,1
	li	s2,-1
	lw	s1,4(s6)
	lw	s0,8(s6)
	addi	s1,s1,-1
	bltz	s1,80001508 <_fwalk_reent+0x7c>
	lhu	a5,12(s0)
	addi	s1,s1,-1
	bgeu	s3,a5,80001500 <_fwalk_reent+0x74>
	lh	a5,14(s0)
	mv	a1,s0
	mv	a0,s5
	beq	a5,s2,80001500 <_fwalk_reent+0x74>
	jalr	s7
	or	s4,s4,a0
	addi	s0,s0,104
	bne	s1,s2,800014dc <_fwalk_reent+0x50>
	lw	s6,0(s6)
	bnez	s6,800014cc <_fwalk_reent+0x40>
	lw	ra,44(sp)
	lw	s0,40(sp)
	lw	s1,36(sp)
	lw	s2,32(sp)
	lw	s3,28(sp)
	lw	s5,20(sp)
	lw	s6,16(sp)
	lw	s7,12(sp)
	mv	a0,s4
	lw	s4,24(sp)
	addi	sp,sp,48
	ret

__smakebuf_r:
	lh	a5,12(a1)
	addi	sp,sp,-128
	sw	s0,120(sp)
	sw	ra,124(sp)
	sw	s1,116(sp)
	sw	s2,112(sp)
	sw	s3,108(sp)
	sw	s4,104(sp)
	andi	a4,a5,2
	mv	s0,a1
	beqz	a4,800015a0 <__smakebuf_r+0x60>
	addi	a5,a1,67
	sw	a5,0(a1)
	sw	a5,16(a1)
	li	a5,1
	sw	a5,20(a1)
	lw	ra,124(sp)
	lw	s0,120(sp)
	lw	s1,116(sp)
	lw	s2,112(sp)
	lw	s3,108(sp)
	lw	s4,104(sp)
	addi	sp,sp,128
	ret
	lh	a1,14(a1)
	mv	s1,a0
	bltz	a1,80001640 <__smakebuf_r+0x100>
	addi	a2,sp,8
	jal	ra,800022cc <_fstat_r>
	bltz	a0,8000163c <__smakebuf_r+0xfc>
	lw	a5,12(sp)
	lui	s2,0xf
	lui	s3,0x1
	and	s2,s2,a5
	lui	a5,0xffffe
	add	s2,s2,a5
	seqz	s2,s2
	li	s4,1024
	addi	s3,s3,-2048 # 800 <exit-0x7ffff800>
	mv	a1,s4
	mv	a0,s1
	jal	ra,800017a8 <_malloc_r>
	lh	a5,12(s0)
	beqz	a0,80001668 <__smakebuf_r+0x128>
	lui	a4,0x80001
	addi	a4,a4,-972 # 80000c34 <__BSS_END__+0xffffd378>
	sw	a4,60(s1)
	ori	a5,a5,128
	sh	a5,12(s0)
	sw	a0,0(s0)
	sw	a0,16(s0)
	sw	s4,20(s0)
	bnez	s2,800016a0 <__smakebuf_r+0x160>
	or	a5,a5,s3
	lw	ra,124(sp)
	sh	a5,12(s0)
	lw	s0,120(sp)
	lw	s1,116(sp)
	lw	s2,112(sp)
	lw	s3,108(sp)
	lw	s4,104(sp)
	addi	sp,sp,128
	ret
	lh	a5,12(s0)
	andi	a5,a5,128
	li	s2,0
	beqz	a5,80001694 <__smakebuf_r+0x154>
	li	s4,64
	mv	a1,s4
	mv	a0,s1
	jal	ra,800017a8 <_malloc_r>
	lh	a5,12(s0)
	li	s3,0
	bnez	a0,800015f0 <__smakebuf_r+0xb0>
	andi	a4,a5,512
	bnez	a4,80001580 <__smakebuf_r+0x40>
	andi	a5,a5,-4
	ori	a5,a5,2
	addi	a4,s0,67
	sh	a5,12(s0)
	li	a5,1
	sw	a4,0(s0)
	sw	a4,16(s0)
	sw	a5,20(s0)
	j	80001580 <__smakebuf_r+0x40>
	li	s4,1024
	li	s3,0
	j	800015dc <__smakebuf_r+0x9c>
	lh	a1,14(s0)
	mv	a0,s1
	jal	ra,80002330 <_isatty_r>
	bnez	a0,800016b8 <__smakebuf_r+0x178>
	lh	a5,12(s0)
	j	80001614 <__smakebuf_r+0xd4>
	lhu	a5,12(s0)
	andi	a5,a5,-4
	ori	a5,a5,1
	slli	a5,a5,0x10
	srai	a5,a5,0x10
	j	80001614 <__smakebuf_r+0xd4>

__swhatbuf_r:
	addi	sp,sp,-112
	sw	s0,104(sp)
	mv	s0,a1
	lh	a1,14(a1)
	sw	s1,100(sp)
	sw	s2,96(sp)
	sw	ra,108(sp)
	mv	s1,a2
	mv	s2,a3
	bltz	a1,80001748 <__swhatbuf_r+0x78>
	addi	a2,sp,8
	jal	ra,800022cc <_fstat_r>
	bltz	a0,80001748 <__swhatbuf_r+0x78>
	lw	a4,12(sp)
	lui	a5,0xf
	lw	ra,108(sp)
	and	a5,a5,a4
	lui	a4,0xffffe
	add	a5,a5,a4
	lw	s0,104(sp)
	seqz	a5,a5
	sw	a5,0(s2) # f000 <exit-0x7fff1000>
	li	a4,1024
	sw	a4,0(s1)
	lui	a0,0x1
	lw	s1,100(sp)
	lw	s2,96(sp)
	addi	a0,a0,-2048 # 800 <exit-0x7ffff800>
	addi	sp,sp,112
	ret
	lhu	a5,12(s0)
	andi	a5,a5,128
	beqz	a5,80001780 <__swhatbuf_r+0xb0>
	lw	ra,108(sp)
	lw	s0,104(sp)
	li	a5,0
	sw	a5,0(s2)
	li	a4,64
	sw	a4,0(s1)
	lw	s2,96(sp)
	lw	s1,100(sp)
	li	a0,0
	addi	sp,sp,112
	ret
	lw	ra,108(sp)
	lw	s0,104(sp)
	sw	a5,0(s2)
	li	a4,1024
	sw	a4,0(s1)
	lw	s2,96(sp)
	lw	s1,100(sp)
	li	a0,0
	addi	sp,sp,112
	ret

_malloc_r:
	addi	sp,sp,-48
	sw	s3,28(sp)
	sw	ra,44(sp)
	sw	s0,40(sp)
	sw	s1,36(sp)
	sw	s2,32(sp)
	sw	s4,24(sp)
	sw	s5,20(sp)
	sw	s6,16(sp)
	sw	s7,12(sp)
	sw	s8,8(sp)
	sw	s9,4(sp)
	addi	a5,a1,11
	li	a4,22
	mv	s3,a0
	bltu	a4,a5,8000187c <_malloc_r+0xd4>
	li	a5,16
	bltu	a5,a1,80001a00 <_malloc_r+0x258>
	jal	ra,80001f1c <__malloc_lock>
	li	s1,16
	li	a5,24
	li	a1,2
	addi	s2,gp,-968 # 80003448 <__malloc_av_>
	add	a5,s2,a5
	lw	s0,4(a5) # f004 <exit-0x7fff0ffc>
	addi	a4,a5,-8
	beq	s0,a4,80001b10 <_malloc_r+0x368>
	lw	a5,4(s0)
	lw	a3,12(s0)
	lw	a2,8(s0)
	andi	a5,a5,-4
	add	a5,s0,a5
	lw	a4,4(a5)
	sw	a3,12(a2)
	sw	a2,8(a3)
	ori	a4,a4,1
	mv	a0,s3
	sw	a4,4(a5)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	lw	ra,44(sp)
	lw	s0,40(sp)
	lw	s1,36(sp)
	lw	s2,32(sp)
	lw	s3,28(sp)
	lw	s4,24(sp)
	lw	s5,20(sp)
	lw	s6,16(sp)
	lw	s7,12(sp)
	lw	s8,8(sp)
	lw	s9,4(sp)
	addi	sp,sp,48
	ret
	andi	s1,a5,-8
	bltz	a5,80001a00 <_malloc_r+0x258>
	bltu	s1,a1,80001a00 <_malloc_r+0x258>
	jal	ra,80001f1c <__malloc_lock>
	li	a5,503
	bgeu	a5,s1,80001c24 <_malloc_r+0x47c>
	srli	a5,s1,0x9
	beqz	a5,80001a10 <_malloc_r+0x268>
	li	a4,4
	bltu	a4,a5,80001b80 <_malloc_r+0x3d8>
	srli	a5,s1,0x6
	addi	a1,a5,57
	addi	a0,a5,56
	slli	a3,a1,0x3
	addi	s2,gp,-968 # 80003448 <__malloc_av_>
	add	a3,s2,a3
	lw	s0,4(a3)
	addi	a3,a3,-8
	beq	a3,s0,800018f0 <_malloc_r+0x148>
	li	a2,15
	j	800018dc <_malloc_r+0x134>
	bgez	a4,80001b04 <_malloc_r+0x35c>
	lw	s0,12(s0)
	beq	a3,s0,800018f0 <_malloc_r+0x148>
	lw	a5,4(s0)
	andi	a5,a5,-4
	sub	a4,a5,s1
	bge	a2,a4,800018d0 <_malloc_r+0x128>
	mv	a1,a0
	lw	s0,16(s2)
	addi	a6,gp,-960 # 80003450 <__malloc_av_+0x8>
	beq	s0,a6,80001ad4 <_malloc_r+0x32c>
	lw	a5,4(s0)
	li	a3,15
	andi	a5,a5,-4
	sub	a4,a5,s1
	blt	a3,a4,80001c30 <_malloc_r+0x488>
	sw	a6,20(s2)
	sw	a6,16(s2)
	bgez	a4,80001c04 <_malloc_r+0x45c>
	li	a4,511
	lw	a0,4(s2)
	bltu	a4,a5,80001b20 <_malloc_r+0x378>
	andi	a4,a5,-8
	addi	a4,a4,8 # ffffe008 <__BSS_END__+0x7fffa74c>
	add	a4,s2,a4
	lw	a3,0(a4)
	srli	a2,a5,0x5
	li	a5,1
	sll	a5,a5,a2
	or	a0,a0,a5
	addi	a5,a4,-8
	sw	a5,12(s0)
	sw	a3,8(s0)
	sw	a0,4(s2)
	sw	s0,0(a4)
	sw	s0,12(a3)
	srai	a5,a1,0x2
	li	a2,1
	sll	a2,a2,a5
	bltu	a0,a2,80001a20 <_malloc_r+0x278>
	and	a5,a2,a0
	bnez	a5,8000199c <_malloc_r+0x1f4>
	slli	a2,a2,0x1
	andi	a1,a1,-4
	and	a5,a2,a0
	addi	a1,a1,4
	bnez	a5,8000199c <_malloc_r+0x1f4>
	slli	a2,a2,0x1
	and	a5,a2,a0
	addi	a1,a1,4
	beqz	a5,8000198c <_malloc_r+0x1e4>
	li	a7,15
	slli	t1,a1,0x3
	add	t1,s2,t1
	mv	a0,t1
	lw	a5,12(a0)
	mv	t3,a1
	beq	a0,a5,80001ba4 <_malloc_r+0x3fc>
	lw	a4,4(a5)
	mv	s0,a5
	lw	a5,12(a5)
	andi	a4,a4,-4
	sub	a3,a4,s1
	blt	a7,a3,80001bbc <_malloc_r+0x414>
	bltz	a3,800019b4 <_malloc_r+0x20c>
	add	a4,s0,a4
	lw	a3,4(a4)
	lw	a2,8(s0)
	mv	a0,s3
	ori	a3,a3,1
	sw	a3,4(a4)
	sw	a5,12(a2)
	sw	a2,8(a5)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	j	80001848 <_malloc_r+0xa0>
	li	a5,12
	sw	a5,0(s3)
	li	a0,0
	j	80001848 <_malloc_r+0xa0>
	li	a3,512
	li	a1,64
	li	a0,63
	j	800018b4 <_malloc_r+0x10c>
	lw	s0,8(s2)
	lw	a5,4(s0)
	andi	s6,a5,-4
	bltu	s6,s1,80001a3c <_malloc_r+0x294>
	sub	a4,s6,s1
	li	a5,15
	blt	a5,a4,80001adc <_malloc_r+0x334>
	lw	s5,92(gp) # 8000386c <__malloc_top_pad>
	lw	a4,76(gp) # 8000385c <__malloc_sbrk_base>
	li	a5,-1
	add	s4,s0,s6
	add	s5,s1,s5
	beq	a4,a5,80001df8 <_malloc_r+0x650>
	lui	a5,0x1
	addi	a5,a5,15 # 100f <exit-0x7fffeff1>
	add	s5,s5,a5
	lui	a5,0xfffff
	and	s5,s5,a5
	mv	a1,s5
	mv	a0,s3
	jal	ra,80001f24 <_sbrk_r>
	li	a5,-1
	mv	s7,a0
	beq	a0,a5,80001c84 <_malloc_r+0x4dc>
	bltu	a0,s4,80001c80 <_malloc_r+0x4d8>
	addi	s8,gp,132 # 80003894 <__malloc_current_mallinfo>
	lw	a1,0(s8)
	add	a1,s5,a1
	sw	a1,0(s8)
	mv	a4,a1
	bne	s4,a0,80001d24 <_malloc_r+0x57c>
	slli	a5,a0,0x14
	bnez	a5,80001d24 <_malloc_r+0x57c>
	lw	s7,8(s2)
	add	a5,s6,s5
	ori	a5,a5,1
	sw	a5,4(s7)
	lw	a3,88(gp) # 80003868 <__malloc_max_sbrked_mem>
	bgeu	a3,a1,80001ac0 <_malloc_r+0x318>
	sw	a1,88(gp) # 80003868 <__malloc_max_sbrked_mem>
	lw	a3,84(gp) # 80003864 <__malloc_max_total_mem>
	bgeu	a3,a1,80001acc <_malloc_r+0x324>
	sw	a1,84(gp) # 80003864 <__malloc_max_total_mem>
	mv	s0,s7
	j	80001c8c <_malloc_r+0x4e4>
	lw	a0,4(s2)
	j	80001960 <_malloc_r+0x1b8>
	ori	a5,s1,1
	sw	a5,4(s0)
	add	s1,s0,s1
	sw	s1,8(s2)
	ori	a4,a4,1
	mv	a0,s3
	sw	a4,4(s1)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	j	80001848 <_malloc_r+0xa0>
	lw	a3,12(s0)
	lw	a2,8(s0)
	j	80001824 <_malloc_r+0x7c>
	lw	s0,12(a5) # fffff00c <__BSS_END__+0x7fffb750>
	addi	a1,a1,2
	beq	a5,s0,800018f0 <_malloc_r+0x148>
	j	80001814 <_malloc_r+0x6c>
	srli	a4,a5,0x9
	li	a3,4
	bgeu	a3,a4,80001c6c <_malloc_r+0x4c4>
	li	a3,20
	bltu	a3,a4,80001e44 <_malloc_r+0x69c>
	addi	a2,a4,92
	addi	a3,a4,91
	slli	a2,a2,0x3
	add	a2,s2,a2
	lw	a4,0(a2)
	addi	a2,a2,-8
	bne	a2,a4,80001b5c <_malloc_r+0x3b4>
	j	80001e00 <_malloc_r+0x658>
	lw	a4,8(a4)
	beq	a2,a4,80001b68 <_malloc_r+0x3c0>
	lw	a3,4(a4)
	andi	a3,a3,-4
	bltu	a5,a3,80001b54 <_malloc_r+0x3ac>
	lw	a2,12(a4)
	sw	a2,12(s0)
	sw	a4,8(s0)
	sw	s0,8(a2)
	sw	s0,12(a4)
	j	80001960 <_malloc_r+0x1b8>
	li	a4,20
	bgeu	a4,a5,80001cb0 <_malloc_r+0x508>
	li	a4,84
	bltu	a4,a5,80001e60 <_malloc_r+0x6b8>
	srli	a5,s1,0xc
	addi	a1,a5,111
	addi	a0,a5,110
	slli	a3,a1,0x3
	j	800018b4 <_malloc_r+0x10c>
	addi	t3,t3,1
	andi	a5,t3,3
	addi	a0,a0,8
	beqz	a5,80001ccc <_malloc_r+0x524>
	lw	a5,12(a0)
	j	800019b4 <_malloc_r+0x20c>
	lw	a2,8(s0)
	ori	a1,s1,1
	sw	a1,4(s0)
	sw	a5,12(a2)
	sw	a2,8(a5)
	add	s1,s0,s1
	sw	s1,20(s2)
	sw	s1,16(s2)
	ori	a5,a3,1
	sw	a6,12(s1)
	sw	a6,8(s1)
	sw	a5,4(s1)
	add	a4,s0,a4
	mv	a0,s3
	sw	a3,0(a4)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	j	80001848 <_malloc_r+0xa0>
	add	a5,s0,a5
	lw	a4,4(a5)
	mv	a0,s3
	ori	a4,a4,1
	sw	a4,4(a5)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	j	80001848 <_malloc_r+0xa0>
	srli	a1,s1,0x3
	addi	a5,s1,8
	j	80001800 <_malloc_r+0x58>
	ori	a3,s1,1
	sw	a3,4(s0)
	add	s1,s0,s1
	sw	s1,20(s2)
	sw	s1,16(s2)
	ori	a3,a4,1
	sw	a6,12(s1)
	sw	a6,8(s1)
	sw	a3,4(s1)
	add	a5,s0,a5
	mv	a0,s3
	sw	a4,0(a5)
	jal	ra,80001f20 <__malloc_unlock>
	addi	a0,s0,8
	j	80001848 <_malloc_r+0xa0>
	srli	a4,a5,0x6
	addi	a2,a4,57
	addi	a3,a4,56
	slli	a2,a2,0x3
	j	80001b40 <_malloc_r+0x398>
	beq	s0,s2,80001d14 <_malloc_r+0x56c>
	lw	s0,8(s2)
	lw	a5,4(s0)
	andi	a5,a5,-4
	sub	a4,a5,s1
	bltu	a5,s1,80001ca0 <_malloc_r+0x4f8>
	li	a5,15
	blt	a5,a4,80001adc <_malloc_r+0x334>
	mv	a0,s3
	jal	ra,80001f20 <__malloc_unlock>
	li	a0,0
	j	80001848 <_malloc_r+0xa0>
	addi	a1,a5,92
	addi	a0,a5,91
	slli	a3,a1,0x3
	j	800018b4 <_malloc_r+0x10c>
	lw	a5,8(t1)
	addi	a1,a1,-1
	bne	a5,t1,80001f14 <_malloc_r+0x76c>
	andi	a5,a1,3
	addi	t1,t1,-8
	bnez	a5,80001cc0 <_malloc_r+0x518>
	lw	a4,4(s2)
	not	a5,a2
	and	a5,a5,a4
	sw	a5,4(s2)
	slli	a2,a2,0x1
	bltu	a5,a2,80001a20 <_malloc_r+0x278>
	beqz	a2,80001a20 <_malloc_r+0x278>
	and	a4,a2,a5
	bnez	a4,80001d0c <_malloc_r+0x564>
	slli	a2,a2,0x1
	and	a4,a2,a5
	addi	t3,t3,4
	beqz	a4,80001cfc <_malloc_r+0x554>
	mv	a1,t3
	j	800019a0 <_malloc_r+0x1f8>
	addi	s8,gp,132 # 80003894 <__malloc_current_mallinfo>
	lw	a4,0(s8)
	add	a4,s5,a4
	sw	a4,0(s8)
	lw	a3,76(gp) # 8000385c <__malloc_sbrk_base>
	li	a5,-1
	beq	a3,a5,80001e7c <_malloc_r+0x6d4>
	sub	a5,s7,s4
	add	a5,a5,a4
	sw	a5,0(s8)
	andi	s9,s7,7
	beqz	s9,80001e18 <_malloc_r+0x670>
	lui	a4,0x1
	sub	s7,s7,s9
	addi	a1,a4,8 # 1008 <exit-0x7fffeff8>
	addi	s7,s7,8
	sub	a1,a1,s9
	add	s5,s7,s5
	sub	a1,a1,s5
	addi	a4,a4,-1
	and	s4,a1,a4
	mv	a1,s4
	mv	a0,s3
	jal	ra,80001f24 <_sbrk_r>
	li	a5,-1
	beq	a0,a5,80001ec8 <_malloc_r+0x720>
	sub	a0,a0,s7
	add	s5,a0,s4
	lw	a4,0(s8)
	sw	s7,8(s2)
	ori	a5,s5,1
	add	a1,s4,a4
	sw	a1,0(s8)
	sw	a5,4(s7)
	beq	s0,s2,80001ab4 <_malloc_r+0x30c>
	li	a3,15
	bgeu	a3,s6,80001e84 <_malloc_r+0x6dc>
	lw	a4,4(s0)
	addi	a5,s6,-12
	andi	a5,a5,-8
	andi	a4,a4,1
	or	a4,a4,a5
	sw	a4,4(s0)
	li	a2,5
	add	a4,s0,a5
	sw	a2,4(a4)
	sw	a2,8(a4)
	bltu	a3,a5,80001ddc <_malloc_r+0x634>
	lw	a5,4(s7)
	j	80001ab4 <_malloc_r+0x30c>
	addi	a1,s0,8
	mv	a0,s3
	jal	ra,800010f0 <_free_r>
	lw	s7,8(s2)
	lw	a1,0(s8)
	lw	a5,4(s7)
	j	80001ab4 <_malloc_r+0x30c>
	addi	s5,s5,16
	j	80001a68 <_malloc_r+0x2c0>
	srai	a3,a3,0x2
	li	a5,1
	sll	a5,a5,a3
	or	a0,a0,a5
	sw	a0,4(s2)
	j	80001b6c <_malloc_r+0x3c4>
	add	a1,s7,s5
	neg	a1,a1
	slli	a1,a1,0x14
	srli	s4,a1,0x14
	mv	a1,s4
	mv	a0,s3
	jal	ra,80001f24 <_sbrk_r>
	li	a5,-1
	bne	a0,a5,80001d7c <_malloc_r+0x5d4>
	li	s4,0
	j	80001d84 <_malloc_r+0x5dc>
	li	a3,84
	bltu	a3,a4,80001e90 <_malloc_r+0x6e8>
	srli	a4,a5,0xc
	addi	a2,a4,111
	addi	a3,a4,110
	slli	a2,a2,0x3
	j	80001b40 <_malloc_r+0x398>
	li	a4,340
	bltu	a4,a5,80001eac <_malloc_r+0x704>
	srli	a5,s1,0xf
	addi	a1,a5,120
	addi	a0,a5,119
	slli	a3,a1,0x3
	j	800018b4 <_malloc_r+0x10c>
	sw	s7,76(gp) # 8000385c <__malloc_sbrk_base>
	j	80001d3c <_malloc_r+0x594>
	li	a5,1
	sw	a5,4(s7)
	j	80001ca0 <_malloc_r+0x4f8>
	li	a3,340
	bltu	a3,a4,80001edc <_malloc_r+0x734>
	srli	a4,a5,0xf
	addi	a2,a4,120
	addi	a3,a4,119
	slli	a2,a2,0x3
	j	80001b40 <_malloc_r+0x398>
	li	a4,1364
	bltu	a4,a5,80001ef8 <_malloc_r+0x750>
	srli	a5,s1,0x12
	addi	a1,a5,125
	addi	a0,a5,124
	slli	a3,a1,0x3
	j	800018b4 <_malloc_r+0x10c>
	addi	s9,s9,-8
	add	s5,s5,s9
	sub	s5,s5,s7
	li	s4,0
	j	80001d84 <_malloc_r+0x5dc>
	li	a3,1364
	bltu	a3,a4,80001f08 <_malloc_r+0x760>
	srli	a4,a5,0x12
	addi	a2,a4,125
	addi	a3,a4,124
	slli	a2,a2,0x3
	j	80001b40 <_malloc_r+0x398>
	li	a3,1016
	li	a1,127
	li	a0,126
	j	800018b4 <_malloc_r+0x10c>
	li	a2,1016
	li	a3,126
	j	80001b40 <_malloc_r+0x398>
	lw	a5,4(s2)
	j	80001ce8 <_malloc_r+0x540>

__malloc_lock:
	ret

__malloc_unlock:
	ret

_sbrk_r:
	addi	sp,sp,-16
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	s0,a0
	mv	a0,a1
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,800027a8 <_sbrk>
	li	a5,-1
	beq	a0,a5,80001f60 <_sbrk_r+0x3c>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,80001f4c <_sbrk_r+0x28>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

__sread:
	addi	sp,sp,-16
	sw	s0,8(sp)
	mv	s0,a1
	lh	a1,14(a1)
	sw	ra,12(sp)
	jal	ra,800023f4 <_read_r>
	bltz	a0,80001fb8 <__sread+0x38>
	lw	a5,80(s0)
	lw	ra,12(sp)
	add	a5,a5,a0
	sw	a5,80(s0)
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	lhu	a5,12(s0)
	lui	a4,0xfffff
	addi	a4,a4,-1 # ffffefff <__BSS_END__+0x7fffb743>
	and	a5,a5,a4
	lw	ra,12(sp)
	sh	a5,12(s0)
	lw	s0,8(sp)
	addi	sp,sp,16
	ret

__seofread:
	li	a0,0
	ret

__swrite:
	lh	a5,12(a1)
	addi	sp,sp,-32
	sw	s0,24(sp)
	sw	s1,20(sp)
	sw	s2,16(sp)
	sw	s3,12(sp)
	sw	ra,28(sp)
	andi	a4,a5,256
	mv	s0,a1
	mv	s1,a0
	mv	s2,a2
	mv	s3,a3
	bnez	a4,80002054 <__swrite+0x70>
	lui	a4,0xfffff
	addi	a4,a4,-1 # ffffefff <__BSS_END__+0x7fffb743>
	and	a5,a5,a4
	lh	a1,14(s0)
	sh	a5,12(s0)
	lw	s0,24(sp)
	lw	ra,28(sp)
	mv	a3,s3
	mv	a2,s2
	lw	s3,12(sp)
	lw	s2,16(sp)
	mv	a0,s1
	lw	s1,20(sp)
	addi	sp,sp,32
	j	800020ec <_write_r>
	lh	a1,14(a1)
	li	a3,2
	li	a2,0
	jal	ra,8000238c <_lseek_r>
	lh	a5,12(s0)
	j	80002018 <__swrite+0x34>

__sseek:
	addi	sp,sp,-16
	sw	s0,8(sp)
	mv	s0,a1
	lh	a1,14(a1)
	sw	ra,12(sp)
	jal	ra,8000238c <_lseek_r>
	li	a5,-1
	beq	a0,a5,800020b8 <__sseek+0x4c>
	lhu	a5,12(s0)
	lui	a4,0x1
	lw	ra,12(sp)
	or	a5,a5,a4
	slli	a5,a5,0x10
	srai	a5,a5,0x10
	sw	a0,80(s0)
	sh	a5,12(s0)
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	lhu	a5,12(s0)
	lui	a4,0xfffff
	addi	a4,a4,-1 # ffffefff <__BSS_END__+0x7fffb743>
	and	a5,a5,a4
	slli	a5,a5,0x10
	srai	a5,a5,0x10
	lw	ra,12(sp)
	sh	a5,12(s0)
	lw	s0,8(sp)
	addi	sp,sp,16
	ret

__sclose:
	lh	a1,14(a1)
	j	80002154 <_close_r>

_write_r:
	addi	sp,sp,-16
	mv	a4,a1
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	a1,a2
	mv	s0,a0
	mv	a2,a3
	mv	a0,a4
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,80002820 <_write>
	li	a5,-1
	beq	a0,a5,80002134 <_write_r+0x48>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,80002120 <_write_r+0x34>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_close_r:
	addi	sp,sp,-16
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	s0,a0
	mv	a0,a1
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,8000260c <_close>
	li	a5,-1
	beq	a0,a5,80002190 <_close_r+0x3c>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,8000217c <_close_r+0x28>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_fclose_r:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	sw	s1,4(sp)
	sw	s2,0(sp)
	beqz	a1,800021e4 <_fclose_r+0x34>
	mv	s0,a1
	mv	s1,a0
	beqz	a0,800021dc <_fclose_r+0x2c>
	lw	a5,56(a0)
	beqz	a5,80002290 <_fclose_r+0xe0>
	lh	a5,12(s0)
	bnez	a5,80002204 <_fclose_r+0x54>
	lw	ra,12(sp)
	lw	s0,8(sp)
	li	s2,0
	lw	s1,4(sp)
	mv	a0,s2
	lw	s2,0(sp)
	addi	sp,sp,16
	ret
	mv	a1,s0
	mv	a0,s1
	jal	ra,80000904 <__sflush_r>
	lw	a5,44(s0)
	mv	s2,a0
	beqz	a5,8000222c <_fclose_r+0x7c>
	lw	a1,28(s0)
	mv	a0,s1
	jalr	a5
	bltz	a0,800022a0 <_fclose_r+0xf0>
	lhu	a5,12(s0)
	andi	a5,a5,128
	bnez	a5,800022b0 <_fclose_r+0x100>
	lw	a1,48(s0)
	beqz	a1,80002254 <_fclose_r+0xa4>
	addi	a5,s0,64
	beq	a1,a5,80002250 <_fclose_r+0xa0>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	sw	zero,48(s0)
	lw	a1,68(s0)
	beqz	a1,80002268 <_fclose_r+0xb8>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	sw	zero,68(s0)
	jal	ra,80000f90 <__sfp_lock_acquire>
	sh	zero,12(s0)
	jal	ra,80000f94 <__sfp_lock_release>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	mv	a0,s2
	lw	s2,0(sp)
	addi	sp,sp,16
	ret
	jal	ra,80000f80 <__sinit>
	lh	a5,12(s0)
	beqz	a5,800021e4 <_fclose_r+0x34>
	j	80002204 <_fclose_r+0x54>
	lhu	a5,12(s0)
	li	s2,-1
	andi	a5,a5,128
	beqz	a5,80002238 <_fclose_r+0x88>
	lw	a1,16(s0)
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	j	80002238 <_fclose_r+0x88>

fclose:
	mv	a1,a0
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	j	800021b0 <_fclose_r>

_fstat_r:
	addi	sp,sp,-16
	mv	a4,a1
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	s0,a0
	mv	a1,a2
	mv	a0,a4
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,80002680 <_fstat>
	li	a5,-1
	beq	a0,a5,80002310 <_fstat_r+0x44>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,800022fc <_fstat_r+0x30>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_isatty_r:
	addi	sp,sp,-16
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	s0,a0
	mv	a0,a1
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,800026e0 <_isatty>
	li	a5,-1
	beq	a0,a5,8000236c <_isatty_r+0x3c>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,80002358 <_isatty_r+0x28>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_lseek_r:
	addi	sp,sp,-16
	mv	a4,a1
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	a1,a2
	mv	s0,a0
	mv	a2,a3
	mv	a0,a4
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,80002720 <_lseek>
	li	a5,-1
	beq	a0,a5,800023d4 <_lseek_r+0x48>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,800023c0 <_lseek_r+0x34>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

_read_r:
	addi	sp,sp,-16
	mv	a4,a1
	sw	s0,8(sp)
	sw	s1,4(sp)
	mv	a1,a2
	mv	s0,a0
	mv	a2,a3
	mv	a0,a4
	sw	ra,12(sp)
	sw	zero,96(gp) # 80003870 <errno>
	jal	ra,80002764 <_read>
	li	a5,-1
	beq	a0,a5,8000243c <_read_r+0x48>
	lw	ra,12(sp)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret
	lw	a5,96(gp) # 80003870 <errno>
	beqz	a5,80002428 <_read_r+0x34>
	lw	ra,12(sp)
	sw	a5,0(s0)
	lw	s0,8(sp)
	lw	s1,4(sp)
	addi	sp,sp,16
	ret

cleanup_glue:
	addi	sp,sp,-32
	sw	s2,16(sp)
	lw	s2,0(a1)
	sw	s0,24(sp)
	sw	s1,20(sp)
	sw	ra,28(sp)
	sw	s3,12(sp)
	sw	s4,8(sp)
	mv	s0,a1
	mv	s1,a0
	beqz	s2,800024c8 <cleanup_glue+0x6c>
	lw	s3,0(s2)
	beqz	s3,800024bc <cleanup_glue+0x60>
	lw	s4,0(s3)
	beqz	s4,800024b0 <cleanup_glue+0x54>
	lw	a1,0(s4)
	beqz	a1,800024a4 <cleanup_glue+0x48>
	jal	ra,8000245c <cleanup_glue>
	mv	a1,s4
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	mv	a1,s3
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	mv	a1,s2
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	mv	a1,s0
	lw	s0,24(sp)
	lw	ra,28(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	lw	s4,8(sp)
	mv	a0,s1
	lw	s1,20(sp)
	addi	sp,sp,32
	j	800010f0 <_free_r>

_reclaim_reent:
	lw	a5,72(gp) # 80003858 <_impure_ptr>
	beq	a5,a0,80002608 <_reclaim_reent+0x118>
	lw	a1,76(a0)
	addi	sp,sp,-32
	sw	s1,20(sp)
	sw	ra,28(sp)
	sw	s0,24(sp)
	sw	s2,16(sp)
	sw	s3,12(sp)
	mv	s1,a0
	beqz	a1,80002558 <_reclaim_reent+0x68>
	li	s2,0
	li	s3,128
	add	a5,a1,s2
	lw	s0,0(a5)
	beqz	s0,80002548 <_reclaim_reent+0x58>
	mv	a1,s0
	lw	s0,0(s0)
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	bnez	s0,80002530 <_reclaim_reent+0x40>
	lw	a1,76(s1)
	addi	s2,s2,4
	bne	s2,s3,80002524 <_reclaim_reent+0x34>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	lw	a1,64(s1)
	beqz	a1,80002568 <_reclaim_reent+0x78>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	lw	s0,328(s1)
	beqz	s0,8000258c <_reclaim_reent+0x9c>
	addi	s2,s1,332
	beq	s0,s2,8000258c <_reclaim_reent+0x9c>
	mv	a1,s0
	lw	s0,0(s0)
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	bne	s2,s0,80002578 <_reclaim_reent+0x88>
	lw	a1,84(s1)
	beqz	a1,8000259c <_reclaim_reent+0xac>
	mv	a0,s1
	jal	ra,800010f0 <_free_r>
	lw	a5,56(s1)
	beqz	a5,800025ec <_reclaim_reent+0xfc>
	lw	a5,60(s1)
	mv	a0,s1
	jalr	a5
	lw	s0,736(s1)
	beqz	s0,800025ec <_reclaim_reent+0xfc>
	lw	a1,0(s0)
	beqz	a1,800025c8 <_reclaim_reent+0xd8>
	mv	a0,s1
	jal	ra,8000245c <cleanup_glue>
	mv	a1,s0
	lw	s0,24(sp)
	lw	ra,28(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	mv	a0,s1
	lw	s1,20(sp)
	addi	sp,sp,32
	j	800010f0 <_free_r>
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	lw	s2,16(sp)
	lw	s3,12(sp)
	addi	sp,sp,32
	ret
	ret

_close:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	li	a7,57
	ecall
	mv	s0,a0
	bltz	a0,8000263c <_close+0x30>
	lw	ra,12(sp)
	mv	a0,s0
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	li	s0,-1
	j	80002628 <_close+0x1c>

_exit:
	li	a7,93
	ecall
	bltz	a0,80002660 <_exit+0x10>
	j	8000265c <_exit+0xc>
	addi	sp,sp,-16
	sw	s0,8(sp)
	mv	s0,a0
	sw	ra,12(sp)
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	j	8000267c <_exit+0x2c>

_fstat:
	addi	sp,sp,-144
	sw	s1,132(sp)
	sw	ra,140(sp)
	mv	s1,a1
	sw	s0,136(sp)
	li	a7,80
	mv	a1,sp
	ecall
	mv	s0,a0
	bltz	a0,800026cc <_fstat+0x4c>
	mv	a0,s1
	mv	a1,sp
	jal	ra,80002864 <_conv_stat>
	lw	ra,140(sp)
	mv	a0,s0
	lw	s0,136(sp)
	lw	s1,132(sp)
	addi	sp,sp,144
	ret
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	li	s0,-1
	j	800026a8 <_fstat+0x28>

_isatty:
	addi	sp,sp,-112
	addi	a1,sp,8
	sw	ra,108(sp)
	jal	ra,80002680 <_fstat>
	li	a5,-1
	beq	a0,a5,80002710 <_isatty+0x30>
	lw	a0,12(sp)
	lw	ra,108(sp)
	srli	a0,a0,0xd
	andi	a0,a0,1
	addi	sp,sp,112
	ret
	lw	ra,108(sp)
	li	a0,0
	addi	sp,sp,112
	ret

_lseek:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	li	a7,62
	ecall
	mv	s0,a0
	bltz	a0,80002750 <_lseek+0x30>
	lw	ra,12(sp)
	mv	a0,s0
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	li	s0,-1
	j	8000273c <_lseek+0x1c>

_read:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	li	a7,63
	ecall
	mv	s0,a0
	bltz	a0,80002794 <_read+0x30>
	lw	ra,12(sp)
	mv	a0,s0
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	li	s0,-1
	j	80002780 <_read+0x1c>

_sbrk:
	lw	a4,100(gp) # 80003874 <heap_end.0>
	addi	sp,sp,-16
	sw	ra,12(sp)
	mv	a5,a0
	bnez	a4,800027d8 <_sbrk+0x30>
	li	a7,214
	li	a0,0
	ecall
	li	a2,-1
	mv	a4,a0
	beq	a0,a2,80002804 <_sbrk+0x5c>
	sw	a0,100(gp) # 80003874 <heap_end.0>
	li	a7,214
	add	a0,a5,a4
	ecall
	lw	a4,100(gp) # 80003874 <heap_end.0>
	add	a5,a5,a4
	bne	a0,a5,80002804 <_sbrk+0x5c>
	lw	ra,12(sp)
	sw	a0,100(gp) # 80003874 <heap_end.0>
	mv	a0,a4
	addi	sp,sp,16
	ret
	jal	ra,8000290c <__errno>
	lw	ra,12(sp)
	li	a5,12
	sw	a5,0(a0)
	li	a0,-1
	addi	sp,sp,16
	ret

_write:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	li	a7,64
	ecall
	mv	s0,a0
	bltz	a0,80002850 <_write+0x30>
	lw	ra,12(sp)
	mv	a0,s0
	lw	s0,8(sp)
	addi	sp,sp,16
	ret
	neg	s0,s0
	jal	ra,8000290c <__errno>
	sw	s0,0(a0)
	li	s0,-1
	j	8000283c <_write+0x1c>

_conv_stat:
	lhu	a4,24(a1)
	lhu	a5,20(a1)
	addi	sp,sp,-16
	lw	t0,28(a1)
	lw	t6,32(a1)
	lw	t5,48(a1)
	lw	t4,64(a1)
	lw	t3,56(a1)
	lw	t1,72(a1)
	lw	t2,76(a1)
	lw	a6,88(a1)
	lw	a7,92(a1)
	sw	s0,12(sp)
	sw	s1,8(sp)
	lw	s0,16(a1)
	lw	s1,8(a1)
	sw	s2,4(sp)
	lw	s2,0(a1)
	lw	a2,104(a1)
	slli	a4,a4,0x10
	or	a5,a5,a4
	lw	a3,108(a1)
	sh	s2,0(a0)
	sh	s1,2(a0)
	sw	s0,4(a0)
	sw	a5,8(a0)
	sh	t0,12(a0)
	sh	t6,14(a0)
	sw	t5,16(a0)
	sw	t4,76(a0)
	sw	t3,72(a0)
	sw	t1,24(a0)
	sw	t2,28(a0)
	sw	a6,40(a0)
	sw	a7,44(a0)
	sw	a2,56(a0)
	lw	s0,12(sp)
	sw	a3,60(a0)
	lw	s1,8(sp)
	lw	s2,4(sp)
	addi	sp,sp,16
	ret

__errno:
	lw	a0,72(gp) # 80003858 <_impure_ptr>
	ret
