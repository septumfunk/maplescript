#include "stack.h"
#include "../memory/mem.h"
#include "types.h"
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

ms_result ms_stack_push(ms_stack *self, ms_primitive type, void *data) {
    const ms_type *ins = ms_type_get(type);
    if (self->size - ((uint64_t)self->top - (uint64_t)self->bottom) < ins->size + sizeof(type)) {
        fprintf(stderr, "Runtime error: Stack overflow!");
        return MS_RESULT_RUNTIME_ERROR;
    }

    memcpy(self->top, data, ins->size);
    self->top += ins->size;
    memcpy(self->top, &type, sizeof(type));
    self->top += sizeof(type);

    return MS_RESULT_OK;
}

ms_result ms_stack_insert(ms_stack *self, ms_primitive type, void *data, int offset) {
    if (offset >= 0)
        return ms_stack_push(self, type, data);

    const ms_type *t = ms_type_get(type);
    if (self->size - ((uint64_t)self->top - (uint64_t)self->bottom) < t->size + sizeof(type)) {
        fprintf(stderr, "Runtime error: Stack overflow!");
        return MS_RESULT_RUNTIME_ERROR;
    }

    uint8_t *b = self->top;
    for (int i = 0; i < offset; ++i) {
        b--;
        const ms_type *tb = ms_type_get(*b);
        b -= tb->size;
    }

    memcpy(b + t->size + sizeof(type), b, (uint64_t)self->top - (uint64_t)b);
    memcpy(b, data, t->size);

    return MS_RESULT_OK;
}

void ms_stack_pop(ms_stack *self, uint64_t count) {
    for (uint64_t i = 0; i < count; ++i)
        self->top -= ms_type_get(*--self->top)->size;
}

ms_value *ms_stack_get(ms_stack *self, int offset) {
    if (self->top == self->bottom)
        return nullptr;

    uint8_t *b = self->top;
    b--;
    for (int i = 0; i < offset; ++i) {
        const ms_type *tb = ms_type_get(*b);
        b -= tb->size;
        b--;
    }

    const ms_type *t = ms_type_get(*b);
    return ms_value_new(*b, b + t->size);
}
