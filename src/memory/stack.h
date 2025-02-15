#pragma once
#include <stdint.h>

typedef struct {
    size_t size;
    uint8_t *bottom;
    uint8_t *top;
} ms_stack;

ms_stack ms_stack_new(size_t size_mb);
void ms_stack_delete(ms_stack *self);

void *_ms_stack_push(ms_stack *self, void *data, size_t size);
#define ms_stack_push(self, data) _ms_stack_push(self, data, sizeof(*data))
void *_ms_stack_insert(ms_stack *self, void *data, size_t size, int64_t offset);
#define ms_stack_insert(self, data) _ms_stack_insert(self, data, sizeof(*data))
void _ms_stack_pop(ms_stack *self, uint64_t count, size_t step_size);
#define ms_stack_pop(self, count, type) _ms_stack_pop(self, count, sizeof(type))

void *_ms_stack_get(ms_stack *self, int64_t offset, uint32_t step_size);
#define ms_stack_get(self, offset, type) (type *)_ms_stack_get(self, offset, sizeof(type))
