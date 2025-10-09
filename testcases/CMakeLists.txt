cmake_minimum_required(VERSION 3.14)
project(CompilerTests)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件路径
include_directories(
        ${CMAKE_SOURCE_DIR}/../src
        ${CMAKE_SOURCE_DIR}/../src/AST/node
        ${CMAKE_SOURCE_DIR}/../src/parser
)

# 自动下载 gtest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# 编译测试可执行文件
add_executable(testcases
        test.cpp
        ../src/parser/parser.cpp
        ../src/AST/node/basic.cpp

        ../src/AST/node/basic.h
        ../src/parser/parser.h
)

# 链接被测库 + gtest
target_link_libraries(testcases
        PRIVATE
        gtest_main
)

# 把测试注册到 CTest
enable_testing()
add_test(NAME testcases COMMAND testcases)