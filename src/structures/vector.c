#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

ms_vector ms_vector_new(size_t element_size) {
    return (ms_vector) {
        .element_size = element_size,
        .slots = MS_VECTOR_INITIAL_SIZE,
        .count = 0,
        .data = calloc(MS_VECTOR_INITIAL_SIZE, element_size),
    };
}

void ms_vector_delete(ms_vector *self) {
    free(self->data);
    self->slots = 0;
    self->count = 0;
    self->data = nullptr;
}

void ms_vector_push(ms_vector *self, void *data) {
    if (self->count == self->slots) // Vector is full, double size.
        self->data = realloc(self->data, self->slots *= 2);

    memcpy(self->data + self->count * self->element_size, data, self->element_size);
    self->count++;
}

void *ms_vector_pop(ms_vector *self) {
    self->count--;

    uint8_t *new_data = malloc(self->element_size);
    memcpy(new_data, self->data + self->count * self->element_size, self->element_size);

    if (self->slots > MS_VECTOR_INITIAL_SIZE && self->count <= self->slots / 2) // Reduce size if possible instead of memsetting.
        self->data = realloc(self->data, self->slots /= 2);
    else memset(self->data + self->count * self->element_size, 0, self->element_size);

    return new_data;
}

void ms_vector_insert(ms_vector *self, uint64_t index, void *data) {
    assert(index <= self->count);
    if (self->count == self->slots) // Vector is full, double size.
        self->data = realloc(self->data, self->slots *= 2);

    // TODO
}

void *ms_vector_get(ms_vector *self, uint64_t index) {
    // TODO
}

void ms_vector_remove(ms_vector *self, uint64_t index) {
    // TODO
}
