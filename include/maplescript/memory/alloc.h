#pragma once
#include <stdint.h>

void *ms_malloc(size_t size);
void *ms_calloc(size_t size, uint64_t count);
void *ms_realloc(void *block, size_t size);