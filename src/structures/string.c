#include <maplescript/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vadefs.h>

ms_string ms_string_new(void) {
    return (ms_string) {
        .length = 0,
        .c_str = nullptr,
    };
}

ms_string ms_string_format(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *c_str = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(c_str, size + 1, format, arglist);
    va_end(arglist);

    return (ms_string) {
        .length = strlen(c_str),
        .c_str = c_str,
    };
}

void ms_string_free(ms_string *self) {
    free(self->c_str);
    self->length = 0;
}

void ms_string_update(ms_string *self, const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *c_str = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(c_str, size + 1, format, arglist);
    va_end(arglist);

    free(self->c_str);
    self->length = strlen(c_str);
    self->c_str = c_str;
}

ms_string ms_string_join(ms_string str1, ms_string str2) {
    char *joined = ms_calloc(1, str1.length + str2.length + 1);
    memcpy(joined, str1.c_str, str1.length);
    memcpy(joined + str1.length, str2.c_str, str2.length);

    return (ms_string) {
        .c_str = joined,
        .length = str1.length + str2.length,
    };
}

ms_string ms_string_duplicate(ms_string self) {
    return (ms_string) {
        .length = self.length,
        .c_str = _strdup(self.c_str),
    };
}

bool ms_string_compare(ms_string str1, ms_string str2) {
    return strcmp(str1.c_str, str2.c_str) == 0;
}