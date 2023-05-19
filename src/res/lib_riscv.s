.section .text
.globl _start

# _syscall0(int syscall_id)
_syscall0:
	mv a7, a0
	ecall
	ret

# _syscall1(int syscall_id, int arg0)
_syscall1:
	mv a7, a0
	mv a0, a1
	ecall
	ret

# _syscall2(int syscall_id, int arg0, int arg1)
_syscall2:
	mv a7, a0
	mv a0, a1
	mv a1, a2
	ecall
	ret

# _syscall3(int syscall_id, int arg0, int arg1, int arg2)
_syscall3:
	mv a7, a0
	mv a0, a1
	mv a1, a2
	mv a2, a3
	ecall
	ret

_start:

	call main
    addi a0, x0, 0
	addi a7, x0, 93
	ecall

	