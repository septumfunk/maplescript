#pragma once
#include <stdint.h>

void *ms_malloc(size_t size);
void *ms_calloc(uint64_t count, size_t size);
void *ms_realloc(void *block, size_t size);

void ms_arrcpy(size_t size, char src[size], char dest[size]);
