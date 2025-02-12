#pragma once
#include <stdint.h>

#define MS_VEC_INITIAL_SIZE 4

/// A generic dynamic vec. Be aware that data may move around the heap,
/// and the size of the vec may not always be equal to the amount of
/// elements in it.
typedef struct {
    size_t const element_size; /// The size of an individual element in the vec.
    uint64_t slots; /// The amount of currently available slots.
    uint64_t count; /// The amount of currently used slots.
    uint8_t *data; /// A series of sequential bytes representing vec elements.
} ms_vec;

/// Creates a new vec with a specified element size.
ms_vec _ms_vec_new(size_t element_size);
#define ms_vec_new(type) _ms_vec_new(sizeof(type))
/// Cleans up after a vec, deleting all elements.
void ms_vec_delete(ms_vec *self);
/// Copies an element to the end of a vec.
void ms_vec_push(ms_vec *self, void *data);
/// Copies an array of elements to the end of a vec.
void ms_vec_append(ms_vec *self, void *data, uint64_t count);
/// Copies and then removes the last vec element, returning a pointer to its copy on the heap.
void *ms_vec_pop(ms_vec *self);
/// Copies an element into the vec at a specified index, shifting all elements after it forward.
void ms_vec_insert(ms_vec *self, uint64_t index, void *data);
/// Sets the value of an index of the vec directly.
void ms_vec_set(ms_vec *self, uint64_t index, void *data);
/// Gets a value from the vec, returning a pointer to it.
/// This pointer may only be valid until the next vec operation is executed,
/// as a vec's data may not always stay in the same place.
const void *_ms_vec_get(ms_vec *, uint64_t);
#define ms_vec_get(vec, type, index) (*(const type *const)_ms_vec_get(vec, index))
/// Removes an element from a vec at the specified index.
void ms_vec_remove(ms_vec *self, uint64_t index);
