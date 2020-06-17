cmake_minimum_required(VERSION 3.15)
project(CustomString)

set(CMAKE_CXX_STANDARD 17)

add_executable(CustomString
        "src/main.cpp"
        "src/String.cpp"
        "src/DebugAssert.h"
        "include/custom/String.h"
 )

target_include_directories(CustomString PRIVATE include)
