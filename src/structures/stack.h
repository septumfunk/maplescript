#pragma once
#include "types.h"
#include <stdint.h>

typedef struct {
    size_t size;
    uint8_t *bottom;
    uint8_t *top;
} ms_stack;

ms_stack ms_stack_new(size_t size_mb);
void ms_stack_delete(ms_stack *self);

ms_result ms_stack_push(ms_stack *self, ms_primitive type, void *data);
ms_result ms_stack_insert(ms_stack *self, ms_primitive type, void *data, int offset);
void ms_stack_pop(ms_stack *self);

#define ms_stack_get(self, type, index) *(type *)_ms_stack_get(self, index)
void *_ms_stack_get(ms_stack *self, int offset);
