#pragma once
#include <stdint.h>

typedef struct ms_stack {
    void *top;
    void *bottom;
    int32_t count;
    size_t _element_size;
} ms_stack;

ms_stack ms_stack_new(size_t element_size);
void ms_stack_delete(ms_stack *self);
void ms_stack_push(ms_stack *self, void *data);
void *ms_stack_pop(ms_stack *self);
void ms_stack_set_top(ms_stack *self, int32_t index);