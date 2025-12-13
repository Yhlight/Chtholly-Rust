# Toolchain-LLVM18.cmake

set(CMAKE_C_COMPILER clang-18)
set(CMAKE_CXX_COMPILER clang++-18)

set(LLVM_DIR "/usr/lib/llvm-18/lib/cmake/llvm")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

set(LINKER_FLAGS "-fuse-ld=lld-18 -stdlib=libc++")

set(RUNTIME_LIBS "-lc++ -lc++abi -lpthread")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS} ${RUNTIME_LIBS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS} ${RUNTIME_LIBS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS} ${RUNTIME_LIBS}")

find_program(LLVM_AR NAMES llvm-ar-18 llvm-ar REQUIRED)
find_program(LLVM_RANLIB NAMES llvm-ranlib-18 llvm-ranlib REQUIRED)
set(CMAKE_AR ${LLVM_AR})
set(CMAKE_RANLIB ${LLVM_RANLIB})
