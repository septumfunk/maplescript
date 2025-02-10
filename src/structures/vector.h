#pragma once
#include <stdint.h>

#define MS_VECTOR_INITIAL_SIZE 4

/// A generic dynamic vector. Be aware that data may move around the heap,
/// and the size of the vector may not always be equal to the amount of
/// elements in it.
typedef struct {
    size_t const element_size; /// The size of an individual element in the vector.
    uint64_t slots; /// The amount of currently available slots.
    uint64_t count; /// The amount of currently used slots.
    uint8_t *data; /// A series of sequential bytes representing vector elements.
} ms_vector;

/// Creates a new vector with a specified element size.
ms_vector ms_vector_new(size_t element_size);
/// Cleans up after a vector, deleting all elements.
void ms_vector_delete(ms_vector *self);
/// Copies an element to the end of a vector.
void ms_vector_push(ms_vector *self, void *data);
/// Copies and then removes the last vector element, returning a pointer to its copy on the heap.
void *ms_vector_pop(ms_vector *self);
/// Copies an element into the vector at a specified index, pushing all other elements forward.
void ms_vector_insert(ms_vector *self, uint64_t index, void *data);
/// Gets a value from the vector, returning a pointer to it.
/// This pointer may only be valid until the next vector operation is executed,
/// as a vector's data may not always stay in the same place.
void *ms_vector_get(ms_vector *self, uint64_t index);
/// Removes an element from a vector at the specified index.
void ms_vector_remove(ms_vector *self, uint64_t index);