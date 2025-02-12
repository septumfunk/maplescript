#include "vec.h"
#include "../memory/mem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_vec _ms_vec_new(size_t element_size) {
    return (ms_vec) {
        .element_size = element_size,
        .slots = 0,
        .count = 0,
        .data = nullptr,
    };
}

void ms_vec_delete(ms_vec *self) {
    free(self->data);
    self->slots = 0;
    self->count = 0;
    self->data = nullptr;
}

void ms_vec_push(ms_vec *self, void *data) {
    if (!self->data || !self->count || !self->slots) {
        self->data = ms_calloc(MS_VEC_INITIAL_SIZE, self->element_size);
        self->slots = MS_VEC_INITIAL_SIZE;
    }

    if (self->count == self->slots) // Vector is full, double size.
        self->data = ms_realloc(self->data, (self->slots *= 2) * self->element_size);

    memcpy(self->data + self->count * self->element_size, data, self->element_size);
    self->count++;
}

void ms_vec_append(ms_vec *self, void *data, uint64_t count) {
    size_t new_size = (self->count + count + 7) & ~(uint64_t)7; // Round to nearest greater multiple of 8.
    if (new_size > self->slots)
        self->data = ms_realloc(self->data, new_size);

    memcpy(self->data + self->count, data, count * self->element_size);
    self->count += count;
}

void *ms_vec_pop(ms_vec *self) {
    assert(self->count > 0 && "Vec is empty.");
    self->count--;

    uint8_t *new_data = ms_malloc(self->element_size);
    memcpy(new_data, self->data + self->count * self->element_size, self->element_size);

    if (self->slots > MS_VEC_INITIAL_SIZE && self->count <= self->slots / 2) // Reduce size if possible
        self->data = ms_realloc(self->data, self->slots /= 2);

    return new_data;
}

void ms_vec_insert(ms_vec *self, uint64_t index, void *data) {
    assert(index <= self->count && "Index out of bounds of vec.");
    if (self->count == self->slots) // Vector is full, double size.
        self->data = ms_realloc(self->data, (self->slots *= 2) * self->element_size);
    self->count++;

    if (index == self->count - 1) {
        ms_vec_push(self, data);
        return;
    }

    memcpy(self->data + (index + 1) * self->element_size, self->data + index * self->element_size, self->element_size * (self->count - index - 1));
    memcpy(self->data + index * self->element_size, data, self->element_size);
}

void ms_vec_set(ms_vec *self, uint64_t index, void *data) {
    assert(index < self->count && "Index out of bounds of vec.");
    memcpy(self->data + index * self->element_size, data, self->element_size);
}

const void *_ms_vec_get(ms_vec *self, uint64_t index) {
    assert(index < self->count && "Index out of bounds of vec.");
    return self->data + index * self->element_size;
}

void ms_vec_remove(ms_vec *self, uint64_t index) {
    assert(index < self->count && "Index out of bounds of vec.");
    self->count--;
    if (self->count - index > 0)
        memcpy(self->data + index * self->element_size, self->data + (index + 1) * self->element_size, self->count - index);
    if (self->slots > MS_VEC_INITIAL_SIZE && self->count <= self->slots / 2) // Reduce size if possible
        self->data = ms_realloc(self->data, self->slots /= 2);
}
