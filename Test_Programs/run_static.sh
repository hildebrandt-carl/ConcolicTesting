#!/bin/bash

while getopts f: option
do
    case "${option}"
    in
    f) FILENAME=${OPTARG};;
    esac
done

if [ -z "${FILENAME}" ];
then
    echo "Please include filename using -f"
    exit 1
fi

cd ~/llvm/llvm/build/
ninja
cd -

echo "Compiling: ${FILENAME}"

if [[ ${FILENAME} == *".tip" ]]; 
then
    FILENAME=${FILENAME::-4}
    EXTENSION=".tip"

    cd ~/llvm/tipc/build
    ./tipc -d ../test/${FILENAME}${EXTENSION}
    mv ../test/${FILENAME}${EXTENSION}.bc ../../Test_Programs/bit_code/${FILENAME}.bc
    cd -

else
    if [[ ${FILENAME} == *".c" ]]; 
    then
        FILENAME=${FILENAME::-2}
        EXTENSION=".c"
        
        ~/llvm/llvm/build/bin/clang -c -emit-llvm -fno-discard-value-names ~/llvm/Test_Programs/source_code/${FILENAME}${EXTENSION} -o ~/llvm/Test_Programs/bit_code/${FILENAME}.bc
    else
        echo "Unknown file extension"
    fi
fi

echo "Running Static Pass on: ${FILENAME}.bc"
echo "==========================================================="
echo ""

~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMstaticConstraints.so -findConstraints < ~/llvm/Test_Programs/bit_code/${FILENAME}.bc > /dev/null