#include "alloc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void *ms_malloc(size_t size) {
    void *block = malloc(size);
    assert(block && "Failed to malloc memory block.");
    return block;
}

void *ms_calloc(uint64_t count, size_t size) {
    void *block = calloc(count, size);
    assert(block && "Failed to calloc memory block.");
    return block;
}

void *ms_realloc(void *block, size_t size) {
    void *new_block = realloc(block, size);
    assert(new_block && "Failed to reallocate memory block.");
    return new_block;
}

void ms_arrcpy(size_t size, char src[size], char dest[size]) {
    memcpy(dest, src, size);
}
