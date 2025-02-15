#pragma once
#include "bytecode.h"
#include <stdint.h>

typedef struct {
    enum ms_ptag pt;
    uint64_t *size;
    void *start;
    void *end;
} ms_memblock;

typedef struct ms_memnode {
    ms_memblock block;
    void *max;
    struct ms_memnode *left, *right;
} ms_memnode;

ms_memnode *ms_memnode_new(ms_memblock memblock);
ms_memnode *ms_memnode_insert(ms_memnode *root, ms_memblock memblock);
bool ms_memnode_overlaps(ms_memnode *node, void *pointer);

typedef struct {
    ms_memnode *root;
} ms_allocator;

ms_memblock *ms_allocator_malloc(ms_allocator *allocator);
ms_memblock *ms_allocator_calloc(ms_allocator *allocator);
void ms_allocator_free(ms_allocator *allocator, ms_memblock *block);
ms_memnode *ms_allocator_find(ms_allocator *allocator, void *pointer);
