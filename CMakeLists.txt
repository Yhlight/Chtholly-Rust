cmake_minimum_required(VERSION 3.20)
project(Chtholly CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Find LLVM and the required components
find_package(LLVM 18.1 REQUIRED CONFIG COMPONENTS
    Core
    IR
    Support
    CodeGen
    MC
    Linker
    Target
    Analysis
    AsmParser
    AsmPrinter
    ExecutionEngine
    OrcJIT
    TransformUtils
    ScalarOpts
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Define the executable for the Chtholly compiler
add_executable(chtholly
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
)

# Add LLVM include directories to our target
target_include_directories(chtholly PRIVATE ${LLVM_INCLUDE_DIRS})

# Add LLVM definitions
target_compile_definitions(chtholly PRIVATE ${LLVM_DEFINITIONS})

# Link the required LLVM libraries to our executable
target_link_libraries(chtholly PRIVATE
    LLVMOrcJIT
    LLVMPasses
    LLVMIRPrinter
    LLVMHipStdPar
    LLVMCoroutines
    LLVMipo
    LLVMInstrumentation
    LLVMVectorize
    LLVMLinker
    LLVMFrontendOpenMP
    LLVMFrontendOffloading
    LLVMCFGuard
    LLVMWindowsDriver
    LLVMJITLink
    LLVMOption
    LLVMExecutionEngine
    LLVMRuntimeDyld
    LLVMOrcTargetProcess
    LLVMOrcShared
    LLVMAsmPrinter
    LLVMCodeGen
    LLVMTarget
    LLVMScalarOpts
    LLVMInstCombine
    LLVMAggressiveInstCombine
    LLVMObjCARCOpts
    LLVMTransformUtils
    LLVMCodeGenTypes
    LLVMBitWriter
    LLVMAnalysis
    LLVMProfileData
    LLVMSymbolize
    LLVMDebugInfoBTF
    LLVMDebugInfoPDB
    LLVMDebugInfoMSF
    LLVMDebugInfoDWARF
    LLVMObject
    LLVMTextAPI
    LLVMMCParser
    LLVMIRReader
    LLVMAsmParser
    LLVMMC
    LLVMDebugInfoCodeView
    LLVMBitReader
    LLVMCore
    LLVMRemarks
    LLVMBitstreamReader
    LLVMBinaryFormat
    LLVMTargetParser
    LLVMSupport
    LLVMDemangle
)

# Enable warnings
target_compile_options(chtholly PRIVATE -Wall -Wextra -pedantic)

# Disable RTTI if LLVM has it disabled
if (NOT LLVM_ENABLE_RTTI)
  message(STATUS "RTTI is disabled in LLVM, disabling for Chtholly as well.")
  target_compile_options(chtholly PRIVATE -fno-rtti)
endif()
