cmake_minimum_required(VERSION 3.10)

project(op-finder)

set(CMAKE_CXX_STANDARD 17)

# OpenCV
set(OpenCV_DIR /usr/lib/x86_64-linux-gnu/cmake/opencv4)
find_package(OpenCV REQUIRED)

# LLVM + Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})

add_executable(op-finder
main.cpp
OperationFinder.cpp
OperationFinderAstAction.cpp
OperationFinderAstConsumer.cpp
OperationFinderAstVisitor.cpp
OperationLog.cpp
OperationStorage.cpp
OperatorCounter.cpp
WhileLoop.cpp
)

target_link_libraries(op-finder
clangTooling
clangFrontend
clangSerialization
clangDriver
clangParse
clangSema
clangAnalysis
clangASTMatchers
clangAST
clangBasic
clangEdit
${OpenCV_LIBS}
)
