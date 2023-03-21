cmake_minimum_required(VERSION 3.17)
project(cena C)

set(CMAKE_C_STANDARD 99)

add_executable(cena main.c)

target_link_libraries(cena pthread)