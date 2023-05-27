if [ "$1" == "-h" ]; then
  echo "Usage: ./script.sh <test_file_name> <directory_path> <build_path>"
  echo "If directory_path is not given, default path is test/pipeline, build_path is the output directory, default is build/test/"
  exit
fi

test=$1

path="test/pipeline/"
buildpath="build/test/"
postfix=input
if [ -n "$2" ]; then
  path=$2
fi
if [ -n "$3" ]; then
    buildpath=$3
fi
if [ -n "$4" ]; then
    postfix=$4
fi


file=${test}.${postfix}
obj=${test}.o
exe=${test}

mkdir -p ${buildpath}
cp ${path}${file} ${buildpath}${file}

file=${buildpath}${file}
obj=${buildpath}${obj}
exe=${buildpath}${exe}

sed -i 's/main/originmain/g' ${file}
echo "
extern void putint(int);
extern void putch(int);
extern void starttime();
extern void stoptime();

int main() {
    starttime();
    int a = originmain();
    stoptime(); 
    putint(a);
    putch(10); 
    return a;
}
" >> ${file}

clang -x c $file -L$CDE_LIBRARY_PATH/native -lsysy -o $exe
$exe