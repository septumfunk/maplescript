#pragma once
#include <stdint.h>

typedef int (*arg_callback)(int argc, char **argv);
typedef struct ms_argpath {
    const char *name;
    arg_callback callback;
    const struct ms_subpaths {
        const uint64_t count;
        const struct ms_argpath *array;
    } subpaths;
} ms_argpath;

int ms_args_parse(int argc, char **argv);

extern const ms_argpath MS_ARGROOT;
