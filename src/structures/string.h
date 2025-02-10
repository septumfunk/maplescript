#pragma once

typedef struct ms_string {
    char *c_str;
    size_t length;
} ms_string;

ms_string ms_string_new(void);
ms_string ms_string_format(const char *format, ...);
void ms_string_delete(ms_string *self);

void ms_string_update(ms_string *self, const char *format, ...);
ms_string ms_string_join(ms_string str1, ms_string str2);
ms_string ms_string_duplicate(ms_string self);
bool ms_string_compare(ms_string str1, ms_string str2);
