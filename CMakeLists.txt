cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(maplescript C)

add_executable(${PROJECT_NAME}
    src/main.c
    src/tests.c

    src/cli/arg.c
    src/cli/assembler.c

    src/data/fs.c
    src/data/map.c
    src/data/strings.c
    src/data/vec.c

    src/memory/alloc.c
    src/memory/stack.c

    src/vm/asm.c
    src/vm/bytecode.c
    src/vm/instruction.c
    src/vm/state.c
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Werror -Wextra -pedantic -Wconversion
    -Wsign-conversion -Wformat=2 -Wundef
    -Wold-style-cast -Woverloaded-virtual
    -Wdouble-promotion -Wnon-virtual-dtor
    -Wnull-dereference -Wstrict-overflow
)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()