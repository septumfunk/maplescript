#pragma once
#include <stdint.h>

uint8_t *ms_file(const char *path, size_t *size);
char *ms_file_string(const char *path);
