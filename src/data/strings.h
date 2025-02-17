#pragma once
#include <string.h>

#define MS_CR "\33[2K\r"

#define ms_strcmp(str1, str2) (strcmp(str1, str2) == 0)
char *ms_strdup(const char *string);
char *ms_format(const char *format, ...);
char *ms_strcat(char *dst, const char *src);
