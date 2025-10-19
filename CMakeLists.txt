cmake_minimum_required(VERSION 3.22)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.70.0 REQUIRED COMPONENTS regex)

if(Boost_FOUND)
    message(STATUS "Boost found at: ${Boost_ROOT}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

    # 添加 Boost 头文件路径
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found! Please set BOOST_ROOT or install Boost.")
endif()

add_executable(Compiler
        src/main.cpp
        src/parser/parser.cpp
        src/AST/node/basic.cpp
        src/semantic/semantic.cpp

        src/AST/node/basic.h
        src/parser/parser.h
        src/semantic/semantic.h
)

enable_testing()
add_subdirectory(testcases)

# 将 Boost.Regex 库链接到你的可执行文件
target_link_libraries(Compiler PUBLIC Boost::regex)
# 或者如果 Boost::regex 不工作（旧版本CMake或Boost），使用：
# target_link_libraries(Compiler ${Boost_LIBRARIES})