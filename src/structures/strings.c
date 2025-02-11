#include "strings.h"
#include "../memory/mem.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#pragma clang diagnostic ignored "-Wformat-nonliteral"

char *ms_strdup(const char *string) {
    auto len = strlen(string) + 1; // Include terminator
    char *new_string = ms_malloc(len);
    memcpy(new_string, string, len);
    return new_string;
}

char *ms_format(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *fmt = ms_calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(fmt, size + 1, format, arglist);
    va_end(arglist);

    return fmt;
}
