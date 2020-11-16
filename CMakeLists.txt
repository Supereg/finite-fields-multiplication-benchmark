cmake_minimum_required(VERSION 3.17)
project(finite-fields-multiplication)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

add_executable(HW1 main.cpp poly.h)