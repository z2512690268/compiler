
file=$1

riscv32-unknown-elf-gcc -x assembler -g test/pipeline/${file}.riscv -o test/pipeline/${file}.exe -static -march=rv32im -mabi=ilp32 -Wl,--build-id=none -Wl,-Ttext=0x80000000

qemu-riscv32-static test/pipeline/${file}.exe