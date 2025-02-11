#pragma once
#include "string.h"
#include <stdint.h>

#define MS_MAP_DEFAULT_BUCKETS 8

/// A key/value pair that contains size information and a possible reference to a chain of pairs.
typedef struct ms_key_value {
    const char *key; /// The key which this pair is indexed by.
    const struct {
        const void *const pointer; /// A read only pointer to the data on the heap.
        const size_t size; /// The size of the memory block pointed to.
    } value; /// Contains information about the value contained by this pair.

    struct ms_key_value *next; /// Can either point to the next pair in a series of conflicts, or `nullptr` if there is none.
} ms_key_value;

/// A fast and simple hashmap indexed by strings which may contain a value of any type.
typedef struct {
    uint64_t bucket_count; /// The amount of currently available buckets. Will expand to reduce conflicts as the map grows.
    uint64_t pair_count; /// The amount of pairs currently held within the map.
    ms_key_value **buckets; /// A pointer to the first bucket in an array of them.
} ms_map;

/// Create a new empty map with the default amount of buckets `MS_MAP_DEFAULT_BUCKETS 8`
ms_map ms_map_new(void);
/// Delete and clean up after a map and all of its pairs.
void ms_map_delete(ms_map *self);
/// Clear a map's pairs and reset it to its default state `MS_MAP_DEFAULT_BUCKETS 8`
void ms_map_clear(ms_map *self);
/// Insert a key/value pair into the map. The value's size is specified by the `size` parameter, which determines how many bytes to copy.
/// It's recommended to use the convenience macro, `ms_map_insert`.
void _ms_map_insert(ms_map *self, const char *key, void *value, size_t size);
#define ms_map_insert(self, key, value) {\
    auto val = value;\
    _ms_map_insert(self, key, val, sizeof(*val));\
}
/// Get a value from a map by its key.
/// It's recommended to use the convenience macro, `ms_map_get`.
const void *_ms_map_get(ms_map *self, const char *key);
#define ms_map_get(self, type, key) (*(type *)_ms_map_get(self, key))
/// Check if a key has a corresponding value within the map.
bool ms_map_exists(ms_map *self, const char *key);
/// Remove and free a value from a map by its key.
void ms_map_remove(ms_map *self, const char *key);
