cmake_minimum_required(VERSION 3.17)
project(CalculatePi C)

set(CMAKE_C_STANDARD 11)

add_executable(CalculatePi main.c)
target_link_libraries(CalculatePi pthread)