cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(maplescript C)

add_executable(${PROJECT_NAME}
    src/main.c

    src/memory/mem.c

    src/structures/map.c
    src/structures/strings.c
    src/structures/vec.c

    src/vm/chunk.c
    src/vm/instructions.c
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Werror -Wextra -Wpedantic -Wconversion
    -Wsign-conversion -Wshadow -Wformat=2
    -Wold-style-cast -Woverloaded-virtual
    -Wdouble-promotion -Wnon-virtual-dtor
    -Wnull-dereference -Wstrict-overflow -Wundef
)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()