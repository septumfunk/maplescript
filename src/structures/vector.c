#include "vector.h"
#include "../memory/mem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_vec _ms_vec_new(size_t element_size) {
    return (ms_vec) {
        .element_size = element_size,
        .slots = MS_VEC_INITIAL_SIZE,
        .count = 0,
        .data = ms_calloc(MS_VEC_INITIAL_SIZE, element_size),
    };
}

void ms_vec_delete(ms_vec *self) {
    free(self->data);
    self->slots = 0;
    self->count = 0;
    self->data = nullptr;
}

void ms_vec_push(ms_vec *self, void *data) {
    if (self->count == self->slots) // Vector is full, double size.
        self->data = ms_realloc(self->data, (self->slots *= 2) * self->element_size);

    memcpy(self->data + self->count * self->element_size, data, self->element_size);
    self->count++;
}

void *ms_vec_pop(ms_vec *self) {
    self->count--;

    uint8_t *new_data = ms_malloc(self->element_size);
    memcpy(new_data, self->data + self->count * self->element_size, self->element_size);

    if (self->slots > MS_VEC_INITIAL_SIZE && self->count <= self->slots / 2) // Reduce size if possible
        self->data = ms_realloc(self->data, self->slots /= 2);

    return new_data;
}

void ms_vec_insert(ms_vec *self, uint64_t index, void *data) {
    assert(index <= self->count);
    if (self->count == self->slots) // Vector is full, double size.
        self->data = ms_realloc(self->data, (self->slots *= 2) * self->element_size);
    self->count++;

    if (index == self->count - 1) {
        ms_vec_push(self, data);
        return;
    }

    memmove(self->data + (index + 1) * self->element_size, self->data + index * self->element_size, self->element_size * (self->count - index - 1));
    memcpy(self->data + index * self->element_size, data, self->element_size);
}

const void *_ms_vec_get(ms_vec *self, uint64_t index) {
    assert(index < self->count);
    return self->data + index * self->element_size;
}

/*void ms_vec_remove(ms_vec *self, uint64_t index) {
    // TODO
}*/
