cmake_minimum_required(VERSION 3.10)
project(DesignPatterns)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(patterns main.cpp)

# 添加简化版可执行文件
add_executable(simple_patterns simple_main.cpp)

# 编译选项
if(MSVC)
    target_compile_options(simple_patterns PRIVATE /W4 /utf-8)
else()
    target_compile_options(simple_patterns PRIVATE -Wall -Wextra -std=c++17)
endif()