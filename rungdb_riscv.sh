if [ "$1" == "-h" ]; then
  echo "Usage: ./script.sh <test_file_name> <directory_path> <build_path>"
  echo "If directory_path is not given, default path is test/pipeline, build_path is the output directory, default is build/test/"
  exit
fi

test=$1

path="test/pipeline/"
buildpath="build/test/"
if [ -n "$2" ]; then
  path=$2
fi
if [ -n "$3" ]; then
    buildpath=$3
fi


file=${test}.riscv
obj=${test}.o
exe=${test}

mkdir -p ${buildpath}
cp ${path}${file} ${buildpath}${file}

file=${buildpath}${file}
obj=${buildpath}${obj}
exe=${buildpath}${exe}

sed -i 's/main/originmain/g' ${file}
echo "
	.global main

main:
    addi  sp, sp, -8
    sw    ra, 4(sp)
	call starttime
	#call originmain
	#addi s0, a0, 0
	call stoptime
	#addi a0, s0, 0
	#call putint
	#addi a0, x0, 10
	#call putch
	#addi a0, s0, 0
    lw    ra, 4(sp)
    addi  sp, sp, 16
    ret
" >> ${file}


clang -x assembler-with-cpp ${file} -c -o ${obj} -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
ld.lld ${obj} -L$CDE_LIBRARY_PATH/riscv32 -lsysy -o ${exe}
qemu-riscv32-static -g 1234 ${exe}
