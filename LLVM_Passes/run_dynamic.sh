#!/bin/bash

while getopts f:i: option
do
    case "${option}"
    in
    f) FILENAME=${OPTARG};;
    i) INPUT=${OPTARG};;
    esac
done

if [ -z "${FILENAME}" ];
then
    echo "Please include filename using -f"
    exit 1
fi

# if [ -z "${INPUT}" ];
# then
#     echo "Please include input to the compiled program using -i"
#     exit 1
# fi

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

    # Make sure the intrinsics is compiled (needed by tip)
    ~/llvm/llvm/build/bin/clang -c -emit-llvm ~/llvm/tipc/intrinsics/tip_intrinsics.c -o ~/llvm/tipc/intrinsics/tip_intrinsics.bc

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

echo "Running Dynamic Pass on: ${FILENAME}.bc"
echo "==========================================================="
echo ""

~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMdynamicConstraints.so -insertConstraints < ~/llvm/Test_Programs/bit_code/${FILENAME}.bc > ~/llvm/Test_Programs/bit_code/${FILENAME}_instrumented.bc

echo ""
echo "==========================================================="
echo "Compiling: auxilary logging functions"


~/llvm/llvm/build/bin/clang -c -emit-llvm ~/llvm/Test_Programs/source_code/dynamic_print_library.c -o ~/llvm/Test_Programs/bit_code/dynamic_print_library.bc

echo "Linking to auxilary logging functions"

~/llvm/llvm/build/bin/llvm-link ~/llvm/Test_Programs/bit_code/dynamic_print_library.bc ~/llvm/Test_Programs/bit_code/${FILENAME}_instrumented.bc -S -o ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc

echo "Displaying new program auxilary with logging functions"
echo "==========================================================="
echo ""

~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMstaticConstraints.so -displayCode < ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc > /dev/null

echo ""
echo "==========================================================="

# If this is a tip program include intrinsics.bc (shows the compiler where main is)
if [ "$EXTENSION" = ".tip" ]; 
then
    echo "Linking to TIPC intrinsics data"

    ~/llvm/llvm/build/bin/llvm-link ~/llvm/tipc/intrinsics/tip_intrinsics.bc ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc -S -o ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc
fi

echo "Running instrumented code"
echo "==========================================================="
echo ""

~/llvm/llvm/build/bin/lli ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc ${INPUT}