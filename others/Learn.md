# Learn

### LLVM and Clang

1. LLVM是一个虚拟机，抽象了不同的硬件平台，其byte code更加接近硬件的实际功能，独立于任何具体硬件存在。Clang是LLVM的前端，负责把c/c++翻译为LLVM能懂的byte code,LLVM再将byte code翻译为具体的机器指令
2. Clang的编译

   1. co LLVM:  svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm
   2. 转到 llvm/tools目录下，co clang:  svn co http://llvm.org/svn/llvm-project/cfe/trunk clang
   3. 新建一个build目录， 打开Developer Command Prompt, cmake -G "Visual Studio 15" ..\llvm 开始编译。新建一个目录的原因是避免build in the source tree，build in the source truee 会把源目录弄得很乱。
3. 从docker启动clang：
   1. docker pull tim03/clang
   2. docker run -it --rm -v (win_dir):(linux_dir) tim03/clang