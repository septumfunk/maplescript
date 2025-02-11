cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(maplescript C)

add_executable(${PROJECT_NAME}
    src/main.c

    src/memory/mem.c

    src/structures/hashmap.c
    src/structures/stringext.c
    src/structures/vector.c
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Werror -Wextra -Wpedantic -Wconversion
    -Wsign-conversion -Wshadow -Wformat=2
    -Wold-style-cast -Woverloaded-virtual
    -Wdouble-promotion -Wnon-virtual-dtor
    -Wnull-dereference -Wstrict-overflow -Wundef
)