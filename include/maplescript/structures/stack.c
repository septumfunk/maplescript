#include "stack.h"
#include "../memory/alloc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

ms_stack ms_stack_new(size_t element_size) {
    return (ms_stack) {
        .top = nullptr,
        .bottom = nullptr,
        .count = 0,
        ._element_size = element_size,
    };
}

void ms_stack_free(ms_stack *self) {
    free(self->top);
}

void ms_stack_push(ms_stack *self, void *data) {
    size_t size = self->_element_size * self->count;
    self->bottom = ms_realloc(self->bottom, size + self->_element_size);
    self->top = self->bottom + size;
    memcpy(self->top, data, self->_element_size);
    self->count++;
}

void *ms_stack_pop(ms_stack *self) {
    if (self->count == 0)
        return nullptr;

    void *new_block = ms_malloc(self->_element_size);
    memcpy(new_block, self->top, self->_element_size);
    self->count--;
    self->bottom = ms_realloc(self->bottom, self->count * self->_element_size);
    self->top = self->bottom + (self->count * self->_element_size);

    return new_block;
}

void ms_stack_set_top(ms_stack *self, int32_t top) {
    assert(top <= self->count && top >= -self->count);
    if (top < 0) self->count += top;
    else self->count = top;

    self->bottom = ms_realloc(self->bottom, self->count * self->_element_size);
    self->top = self->bottom + (self->count * self->_element_size);
}