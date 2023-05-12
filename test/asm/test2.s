# 汇编代码文件 boot.S

# _start是程序入口标签，可在链接时指定
# 使用 .global 指令可将标签作为符号导出
.global _start

# 定义输出字符串
msg:
  .ascii "Hello, RISC-V!\n"

_start:
  # 设置栈指针寄存器（sp）初始值
  lui sp, %hi(_stack_top)
  addi sp, sp, %lo(_stack_top)

  # 设置全局偏移量寄存器（gp）初始值
  lui gp, %hi(_gp)
  addi gp, gp, %lo(_gp)

  # 输出字符串
  la a0, msg    # a0 寄存器指向字符串
  li a7, 4      # 4 是 sys_write 系统调用号
  li a1, 14     # 14 是字符串长度
  ecall         # 发起系统调用

  # 退出程序
  li a0, 0      # 0 是程序返回值
  li a7, 93     # 93 是 sys_exit 系统调用号
  ecall         # 发起系统调用

  # 定义栈顶和全局偏移量符号
  .global _stack_top
  .global _gp

  # 分配 4KB 栈空间
  .section .bss
  .align 2
  _stack_top:
    .skip 4096

  # 设置全局偏移量初始值
  .data
  .align 2
  _gp:
    .word 0x0
