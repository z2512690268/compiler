# xmake
# wget https://xmake.io/shget.text -O - | bash
# apt install build-essential unzip
# echo "export XMAKE_ROOT=y" >> ~/.bashrc
# source ~/.bashrc

# 交叉编译工具链
mkdir -p tools/riscv-gcc
cd tools/riscv-gcc
wget https://ghdl.feizhuqwq.cf/https://github.com/stnolting/riscv-gcc-prebuilt/releases/download/rv32i-4.0.0/riscv32-unknown-elf.gcc-12.1.0.tar.gz
tar -xvf ./riscv32-unknown-elf.gcc-12.1.0.tar.gz
rm ./riscv32-unknown-elf.gcc-12.1.0.tar.gz
echo -e "\nexport PATH=\$PATH:$PWD/tools/riscv-gcc/bin\n" >> ~/.bashrc
source ~/.bashrc