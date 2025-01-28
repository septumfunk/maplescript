cmake_minimum_required(VERSION 3.29)

include(Functions.cmake)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(maplescript C)

compiler_flag("-Wall")
compiler_flag("-Werror")
compiler_flag("-pedantic")

add_executable(${PROJECT_NAME}
    src/main.c
)