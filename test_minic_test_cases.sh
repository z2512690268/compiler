# 遍历test/minic_test_cases-2021s目录下的所有.c结尾的测试用例

files=$(find test/minic-test-cases-2021s/functional -name "*.c")

# 按照_前面的数字排序
files=$(echo "$files" | sort -t_ -k1 -n)

mkdir -p log/minic
cp build/linux/x86_64/release/sysyc .

# 若存在$1, 则只测试_前面的数字等于$1的测试用例
if [ $# -eq 1 ]; then
    files=$(echo "$files" | sed -n "/\/$1_/p")
fi

# 若存在$1, $2, 则只测试_前面的数字大于等于$1, 小于等于$2的测试用例
if [ $# -eq 2 ]; then
    files=$(echo "$files" | awk -F/ -v min="$1" -v max="$2" '{split($(NF), arr, "_"); if(arr[1]>=min && arr[1]<=max) print}')
fi

for file in $files
do
    # 先打印
    echo $file
    # 后缀名替换
    koopa_file=${file//.c/.koopa}
    riscv_file=${file//.c/.riscv}
    # 不要后缀名
    file_base=${file//.c/}
    # 不要路径名
    file_base=${file_base##*/}

    echo Koopa: $koopa_file
    # 测试编译koopa,如果失败，就退出
    ./sysyc koopa $file $koopa_file > log/minic/$file_base.log
    if [ $? -ne 0 ]; then
        exit 1
    fi
    # 测试执行koopa，如果失败，就退出
    ./run_koopa.sh $file_base test/minic-test-cases-2021s/functional/
    # # 测试编译riscv，如果失败，就退出 
    echo Riscv: $riscv_file  
    ./sysyc riscv $file $riscv_file >> log/minic/$file_base.log
    if [ $? -ne 0 ]; then
        exit 1
    fi
    # 测试编译执行riscv，如果失败，就退出
    ./run_riscv.sh $file_base test/minic-test-cases-2021s/functional/

    # 用clang编译
    echo C: $file
    ./run_c.sh $file_base test/minic-test-cases-2021s/functional/ build/test/ c
    # 换行
    echo
    echo
done

