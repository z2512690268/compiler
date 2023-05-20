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
mkdir -p ${buildpath}


file=${test}.koopa
obj=${test}.o
exe=${test}


file=${path}${file}
obj=${buildpath}${obj}
exe=${buildpath}${exe}

koopac ${file} | llc --filetype=obj -o ${obj}
clang ${obj} -L$CDE_LIBRARY_PATH/native -lsysy -o ${exe}
${exe}
