xmake run gram $1 $2 $3
xmake run syntax sysy koopa $3 > log/test/$3.log
xmake run syntax sysy riscv $3 >> log/test/$3.log
./run_koopa.sh $3
./run_riscv.sh $3
# clang -x c test/pipeline/$2.input -L$CDE_LIBRARY_PATH/native -lsysy -o build/test/$2
./run_c.sh $2