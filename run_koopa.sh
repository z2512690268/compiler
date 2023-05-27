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


file=${test}.koopa
obj=${test}.o
exe=${test}

mkdir -p ${buildpath}
cp ${path}${file} ${buildpath}${file}

file=${buildpath}${file}
obj=${buildpath}${obj}
exe=${buildpath}${exe}

sed -i 's/main/originmain/g' ${file}
echo "

fun @main(): i32 {
%entry:
  call @starttime()
	%0 = call @originmain()
	call @stoptime()
  call @putint(%0)
  call @putch(10)
	ret %0
}
" >> ${file}

koopac ${file} | llc --filetype=obj -o ${obj}
clang ${obj} -L$CDE_LIBRARY_PATH/native -lsysy -o ${exe}
${exe}
