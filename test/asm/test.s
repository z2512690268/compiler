# .section .text
# .globl _start

# _start:
#     # 将要输出的字符串的地址存储到a0寄存器
#     lui a0, %hi(msg)
#     addi a0, a0, %lo(msg)

#     # 将要输出的字符串的长度存储到a1寄存器
#     li a1, 13

#     # 使用SBI系统调用来输出字符串
#     li a7, 0x01   # SBI_CONSOLE_PUTSTR
#     ecall

#     # 退出程序
#     li a7, 0x0    # SBI_SHUTDOWN
#     ecall

# .section .data
# msg:
#     .ascii "Hello, world!\n"





# .section .text
# .globl _start

# _start:
#     # 初始化堆栈指针
#     la sp, _stack_end

#     # 初始化全局偏移量寄存器
#     lui gp, %hi(_global_pointer)
#     addi gp, gp, %lo(_global_pointer)

#     # 调用 main 函数
#     jal ra, main

#     # 如果 main 函数返回了，直接退出程序
#     li a7, 93
#     ecall

# .section .bss
# .global _stack_end
# _stack_end:
#     .skip 4096

# .section .data
# .global _global_pointer
# _global_pointer:
#     .word 0


# 代码段.
.text
# `main` 函数, 程序的入口.
.globl main
main:
  addi  sp, sp, -16
  sw    ra, 12(sp)
  sw    s0, 8(sp)
  sw    s1, 4(sp)
  la    s0, hello_str
  li    s1, 0
1:
  add a0, a0, 0
  add   a0, s0, s1
  lbu   a0, 0(a0)
  beqz  a0, 1f
#   li    a7, 0x01           # SBI_CONSOLE_PUTCHAR
#   mv    a0, a0             # 将 ASCII 码复制到 a0 中
#   li    a1, 0x01           # 输出流 ID：1（控制台）
#   ecall
#   nop                     # 等待 SBI 调用完成
  call putchar
  addi  s1, s1, 1
  j     1b + 4
1:
  li    a0, 0
#   li    a7, 0x00           # SBI_CONSOLE_GETCHAR
#   li    a1, 0x01           # 输入流 ID：1（控制台）
#   ecall
#   nop                     # 等待 SBI 调用完成
  lw    s1, 4(sp)
  lw    s0, 8(sp)
  lw    ra, 12(sp)
  addi  sp, sp, 16
  ret

# 数据段.
.data
# 字符串 "Hello, world!\n\0".
hello_str:
  .asciz "Hello, world!\n"




# .section .data
# input_prompt:
#     .ascii "Enter a number: "
#     input_prompt_end:
#     .byte 0

# output_prompt:
#     .ascii "The number is: "
#     output_prompt_end:
#     .byte 0

# .section .text
# .globl _start
# _start:
#     # 打印输入提示信息
#     la a0, input_prompt
#     li a7, 4
#     ecall

#     # 读取用户输入
#     la a0, input_buffer
#     li a1, 1024
#     li a7, 63
#     ecall

#     # 将输入转换为数字
#     la t0, input_buffer
#     li t1, 0
# convert_loop:
#     lbu t2, 0(t0)
#     beqz t2, convert_done
#     sub t2, t2, '0'
#     mul t1, t1, 10
#     add t1, t1, t2
#     addi t0, t0, 1
#     j convert_loop
# convert_done:

#     # 打印输出提示信息
#     la a0, output_prompt
#     li a7, 4
#     ecall

#     # 打印结果
#     mv a0, t1
#     li a7, 1
#     ecall

#     # 退出程序
#     li a7, 93
#     ecall

# .section .bss
# input_buffer:
#     .space 1024







# .section .text
# .globl uart_init

# uart_init:
#     //将以下代码翻译为汇编：
#     # // disable interrupt
#     # WriteReg(REG_IER, 0x00);
#     # // set baud rate
#     # WriteReg(REG_LCR, 0x80);
#     # WriteReg(UART_DLL, 0x03);
#     # WriteReg(UART_DLM, 0x00);
#     # // set word length to 8-bits
#     # WriteReg(REG_LCR, 0x03);
#     # // enable FIFOs
#     # WriteReg(REG_FCR, 0x07);
#     # // enable receiver interrupts
#     # WriteReg(REG_IER, 0x01);

#     # 禁用中断
#     li t0, 0x00
#     lui t1, %hi(0x10000000)
#     addi t1, t1, %lo(0x10000000)
#     sb t0, 1(t1)

#     # 设置波特率
#     li t0, 0x80
#     sb t0, 3(t1)
#     li t0, 0x03
#     sb t0, 0(t1)
#     li t0, 0x00
#     sb t0, 1(t1)

#     # 设置字长为8位
#     li t0, 0x03
#     sb t0, 0(t1)

#     # 启用FIFO
#     li t0, 0x07
#     sb t0, 2(t1)

#     # 启用接收中断
#     li t0, 0x01
#     sb t0, 1(t1)

#     ret

# # void uartputc(int c) {
# #     while(ReadReg(REG_LSR) & (1 << 5) == 0)
# #         ;
# #     WriteReg(REG_THR, c);
# # }
# uart_putc:
#     # 读取UART设备状态
#     lui t0, %hi(0x10000000)
#     addi t0, t0, %lo(0x10000000)
#     lw t1, 0(t0)
#     andi t1, t1, 0x20

#     # 检查状态寄存器中的发送缓冲区为空位
#     bnez t1, uart_putc

#     # 将字符写入发送缓冲区
#     sb a0, 0(t0)

#     ret
    

# # 0000000080000022 <putchar>:
# #     80000022:	100137b7          lui	a5,0x10013
# #     80000026:	4398              lw	a4,0(a5)
# #     80000028:	02071693          slli	a3,a4,0x20
# #     8000002c:	fe06cde3          bltz	a3,80000026 <putchar+0x4>
# #     80000030:	0ff57513          andi	a0,a0,255
# #     80000034:	c388              sw	a0,0(a5)
# #     80000036:	8082              ret
# putchar:
#     lui a5, 0x10013
#     lw  a4, 0(a5)
#     slli a3, a4, 0x20
#     bltz a3, putchar
#     andi a0, a0, 255
#     sw  a0, 0(a5)
#     ret


# .section .text
# .globl main
# main:
#     # 初始化堆栈指针
#     la sp, _stack_end

#     # 初始化全局偏移量寄存器
#     lui gp, %hi(_global_pointer)
#     addi gp, gp, %lo(_global_pointer)

#     # 初始化UART设备
#     # jal ra, uart_init

#     # 输出Hello, world!
#     la a3, hello_str
#     li a1, 0
# 1:
#     lbu a0, (a3)
#     beqz a0, 2f
#     jal ra, putchar
#     addi a3, a3, 1     # 指向下一个字符
#     j 1b
# 2:
#     # 如果 main 函数返回了，直接退出程序
#     li a7, 93
#     ecall

# .section .bss
# .global _stack_end
# _stack_end:
#     .skip 4096

# .section .data
# .global _global_pointer
# _global_pointer:
#     .word 0

# # 数据段.
# .data
# # 字符串 "Hello, world!\n\0".
# hello_str:
#     .asciz "Hello, world!\n"


# 0000000080000000 <_start>:
#     80000000:	0040006f          j	80000004 <main>

# 0000000080000004 <main>:
#     80000004:	1141              addi	sp,sp,-16
#     80000006:	e022              sd	s0,0(sp)
#     80000008:	e406              sd	ra,8(sp)
#     8000000a:	00000417          auipc	s0,0x0
#     8000000e:	02e40413          addi	s0,s0,46 # 80000038 <putchar+0x16>
#     80000012:	00044503          lbu	a0,0(s0)
#     80000016:	e111              bnez	a0,8000001a <main+0x16>
#     80000018:	a001              j	80000018 <main+0x14>
#     8000001a:	0405              addi	s0,s0,1
#     8000001c:	006000ef          jal	ra,80000022 <putchar>
#     80000020:	bfcd              j	80000012 <main+0xe>

# 0000000080000022 <putchar>:
#     80000022:	100137b7          lui	a5,0x10013
#     80000026:	4398              lw	a4,0(a5)
#     80000028:	02071693          slli	a3,a4,0x20
#     8000002c:	fe06cde3          bltz	a3,80000026 <putchar+0x4>
#     80000030:	0ff57513          andi	a0,a0,255
#     80000034:	c388              sw	a0,0(a5)
#     80000036:	8082              ret
#     .text
#     .globl _start
# _start:
#     j main

# main:
#     addi sp, sp, -16
#     sd s0, 0(sp)
#     sd ra, 8(sp)
#     auipc s0, 0x0
#     addi s0, s0, 46
#     lbu a0, 0(s0)
#     bnez a0, main
#     addi s0, s0, 1
#     jal ra, putchar
#     j main

# putchar:
#     lui a5, 0x10013
# putchar_1:
#     lw  a4, 0(a5)
#     slli a3, a4, 0x20
#     bltz a3, putchar
#     andi a0, a0, 255
#     sw  a0, 0(a5)
#     ret