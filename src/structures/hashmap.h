#pragma once
#include "string.h"
#include <stdint.h>

#define MS_HASHMAP_DEFAULT_BUCKETS 8

typedef struct ms_key_value {
    ms_string key;
    const struct {
        void *pointer;
        const size_t size;
    } value;

    struct ms_key_value *next;
} ms_key_value;

typedef struct ms_hashmap {
    uint64_t bucket_count;
    uint64_t pair_count;
    ms_key_value **buckets;
} ms_hashmap;

ms_hashmap ms_hashmap_new(void);
void ms_hashmap_delete(ms_hashmap *self);
void ms_hashmap_clear(ms_hashmap *self);

ms_key_value *ms_hashmap_insert(ms_hashmap *self, ms_string key, void *value);
ms_key_value *ms_hashmap_get(ms_hashmap *self, ms_string key);
void ms_hashmap_remove(ms_hashmap *self, ms_string key);
