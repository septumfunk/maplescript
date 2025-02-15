#include "stack.h"
#include "alloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_stack ms_stack_new(size_t size_mb) {
    uint8_t *block = ms_malloc(size_mb);
    return (ms_stack) {
        .size = size_mb,
        .bottom = block,
        .top = block,
    };
}

void ms_stack_delete(ms_stack *self) {
    free(self->bottom);
}

void *_ms_stack_push(ms_stack *self, void *data, size_t size) {
    if (self->size - ((uint64_t)self->top - (uint64_t)self->bottom) < size)
        return nullptr;

    memcpy(self->top, data, size);
    self->top += size;

    return self->top - size;
}

void *_ms_stack_insert(ms_stack *self, void *data, size_t size, int64_t offset) {
    if (offset == -1)
        return _ms_stack_push(self, data, size);

    if (self->size - ((uint64_t)self->top - (uint64_t)self->bottom) < size)
        return nullptr;

    if (offset > 0) {
        memcpy(self->bottom + ((uint64_t)offset * size), self->bottom + (((uint64_t)offset - 1) * size), size);
        memcpy(self->bottom + (((uint64_t)offset - 1) * size), data, size);
        return self->bottom + (((uint64_t)offset - 1) * size);
    }

    uint64_t rofs = (uint64_t)(-offset - 1);
    memcpy(self->top - (rofs - 1) * size, self->top + rofs * size, rofs);
    memcpy(self->top - rofs * size, data, size);
    return self->top - rofs * size;
}

void _ms_stack_pop(ms_stack *self, uint64_t count, size_t step_size) {
    self->top -= step_size * count;
}

void *_ms_stack_get(ms_stack *self, int64_t offset, uint32_t step_size) {
    if (self->top == self->bottom)
        return nullptr;

    if (offset >= 0)
        return self->bottom + offset * (int64_t)(step_size);
    return self->top + offset * (int64_t)(step_size);
}

void _ms_stack_set(ms_stack *self, int64_t offset, uint32_t step_size, void *value) {
    if (self->top == self->bottom)
        return;

    if (offset >= 0)
        memcpy(self->bottom + offset * (int64_t)(step_size), value, step_size);
    else
        memcpy(self->top + offset * (int64_t)(step_size), value, step_size);
}
