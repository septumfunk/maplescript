cmake_minimum_required(VERSION 3.28)
include(Functions.cmake)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(maplescript C)

compiler_flag("-Wall")
compiler_flag("-Werror")
compiler_flag("-pedantic")
add_compile_options("-fsanitize=address,undefined" "-fno-sanitize=leak")
add_link_options("-fsanitize=address,undefined" "-fno-sanitize=leak")

add_executable(${PROJECT_NAME}
    src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)