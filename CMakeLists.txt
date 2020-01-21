cmake_minimum_required(VERSION 3.15)
project(CustomString)

set(CMAKE_CXX_STANDARD 14)

add_executable(CustomString src/main.cpp src/String.cpp src/String.h)