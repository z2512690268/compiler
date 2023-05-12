build:
	~/bin/riscv32-unknown-elf-gcc -g test.s -o test  -static -nostdlib -march=rv32im -mabi=ilp32 -Wl,--build-id=none -Wl,-Ttext=0x80000000