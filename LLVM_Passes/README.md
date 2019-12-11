# LLVM Setup

Installing LLVM + Clang

## Prerequisites

Code compilation basics and xdot are required to run this
```
$ sudo apt-get install build-essential
$ sudo apt-get install ninja-build
$ sudo apt-get install libffi6 libffi-dev
```

## Installing Clang and LLVM

Download LLVM and Clang source code
```
$ wget http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz
$ wget http://releases.llvm.org/7.0.1/cfe-7.0.1.src.tar.xz
$ wget http://llvm.org/releases/7.0.1/compiler-rt-7.0.1.src.tar.xz
```

Extract LLVM 
```
$ tar -xvf llvm-7.0.1.src.tar.xz
$ mv llvm-7.0.1.src llvm
$ cd llvm
```

Extract clang and the compiler
```
tar -xf ../cfe-7.0.1.src.tar.xz -C tools &&
tar -xf ../compiler-rt-7.0.1.src.tar.xz -C projects &&
mv tools/cfe-7.0.1.src tools/clang &&
mv projects/compiler-rt-7.0.1.src projects/compiler-rt
```

Compile LLVM (From : http://www.linuxfromscratch.org/blfs/view/8.4/general/llvm.html )
```
mkdir -v build &&
cd       build &&
CC=gcc CXX=g++                                  \
cmake -DCMAKE_INSTALL_PREFIX=/usr               \
      -DLLVM_ENABLE_FFI=ON                      \
      -DCMAKE_BUILD_TYPE=Release                \
      -DLLVM_BUILD_LLVM_DYLIB=ON                \
      -DLLVM_LINK_LLVM_DYLIB=ON                 \
      -DLLVM_TARGETS_TO_BUILD="host;AMDGPU;BPF" \
      -DLLVM_BUILD_TESTS=ON                     \
      -Wno-dev -G Ninja ..                      &&
ninja
```

Check everything was build correctly using:
```
$ ninja check-all
```



You can compile programs using:
```
~/llvm/llvm/build/bin/clang -c -emit-llvm ~/llvm/Test_Programs/source_code/test_example.c -o ~/llvm/Test_Programs/source_code/test_example.bc
```



Creating a pass: Followed a tutorial from
http://llvm.org/docs/WritingAnLLVMPass.html

Adding our own pass: Create a new folder inside 
```
$ mkdir llvm/lib/Transforms/testPass
$ cd llvm/lib/Transforms/testPass
```

Create the testPass as found in my code

Add the following line to: lib/Transforms/CMakeLists.txt
```
>>> add_subdirectory(testPass)
```

Recompile LLVM
```
$ cd ~/llvm/llvm/build
$ ninja
```

Run the test pass using:
```
$ ~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMtestPass.so -testpass < ~/llvm/Test_Programs/bit_code/test_example.bc > /dev/null
```


## Adding the TIP language

First we need to download the TIPC compiler. Start by installing the dependencies
```
$ sudo apt-get install openjdk-8-jdk-headless
$ sudo apt-get install cmake pkg-config uuid-dev
$ sudo apt-get install antlr4
```

Clone the repo into your base folder
```
$ git clone https://github.com/matthewbdwyer/tipc.git
```

Download Antlr4 
```
$ wget https://www.antlr.org/download/antlr-4.7.2-complete.jar
```

Edit tipC CMakeLists to have to file we just downloaded
```
$ nano tipc/src/CMakeLists.txt
```


Build tipC
```
$ mkdir build
$ cd build
$ cmake ../src
$ make
```

Note: If you get the error it doesn’t know where LLVM feel free to install LLVM using apt-get. The downside to this is that you have two version of LLVM on your system then.

## Running the LLVM Pass

You are able to run the dynamic pass using either the shell script in this folder or using following commands:

### Static:

First convert the C code into LLVM bitcode using:
```
 $ ~/llvm/llvm/build/bin/clang -c -emit-llvm -fno-discard-value-names ~/llvm/Test_Programs/source_code/simple_if.c -o ~/llvm/Test_Programs/bit_code/simple_if.bc
```

Then you can run the static pass on that bitcode using:
```
$  ~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMstaticConstraints.so -findConstraints < ~/llvm/Test_Programs/bit_code/simple_if.bc > /dev/null
```

### Dynamic:

Running the dynamic pass is a little bit more complicated as it requires numerous files. First start by compiling the C code into LLVM bitcode:
```
 $ ~/llvm/llvm/build/bin/clang -c -emit-llvm -fno-discard-value-names ~/llvm/Test_Programs/source_code/simple_if.c -o ~/llvm/Test_Programs/bit_code/simple_if.bc
```

Then run the dynamic pass on the bitcode to instrument it with the logging functions:
```
~/llvm/llvm/build/bin/opt -load ~/llvm/llvm/build/lib/LLVMdynamicConstraints.so -insertConstraints < ~/llvm/Test_Programs/bit_code/simple_if.bc > ~/llvm/Test_Programs/bit_code/simple_if_instrumented.bc
```

Compile the auxilary logging functions into bitcode:
```
 $ ~/llvm/llvm/build/bin/clang -c -emit-llvm ~/llvm/Test_Programs/source_code/dynamic_print_library.c -o ~/llvm/Test_Programs/bit_code/dynamic_print_library.bc
```

Link your instrumented code with the auxilary logging functions:
```
 $ ~/llvm/llvm/build/bin/llvm-link ~/llvm/Test_Programs/bit_code/dynamic_print_library.bc ~/llvm/Test_Programs/bit_code/simple_if_instrumented.bc -S -o ~/llvm/Test_Programs/bit_code/simple_if_linked.bc
```

Finally run your code using lli with the input after the instruction. (this example uses an input of 42):
```
 $ ~/llvm/llvm/build/bin/lli ~/llvm/Test_Programs/bit_code/simple_if_pint_instrumented.bc 42
```

### TIPC

To use TIP code as bitcode you can do the following:
```
$ ./tipc -d ../test/if.tip
$ mv ../test/if.tip.bc ../../Test_Programs/bit_code
```

Note that the TIPC compiler might require that you link the LLVM bitcode with the intrinsics data from LLVM. Thus after linking the auxiliary logging functions also link the intrinsics bitcode.
```
~/llvm/llvm/build/bin/llvm-link ~/llvm/tipc/intrinsics/tip_intrinsics.bc ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc -S -o ~/llvm/Test_Programs/bit_code/${FILENAME}_linked.bc
```
