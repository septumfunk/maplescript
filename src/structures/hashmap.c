#include "hashmap.h"
#include "../memory/mem.h"
#include "string.h"
#include <assert.h>
#include <stdlib.h>

#define PRIME 0x01000193
#define SEED 0x811C9DC5

ms_key_value *ms_push_kv(ms_key_value *list, ms_key_value *new) {
    new->next = list;
    return new;
}

uint32_t ms_fnv1a(const void *data, size_t size, uint32_t hash) {
    assert(data);
    const unsigned char *head = data;
    while (size--)
        hash = (*head++ ^ hash) * PRIME;
    return hash;
}

ms_hashmap ms_hashmap_new(void) {
    return (ms_hashmap) {
        .bucket_count = MS_HASHMAP_DEFAULT_BUCKETS,
        .pair_count = 0,
        .buckets = ms_calloc(sizeof(ms_key_value *), MS_HASHMAP_DEFAULT_BUCKETS),
    };
}

void ms_hashmap_delete(ms_hashmap *self) {
    ms_hashmap_clear(self);
    free(self->buckets);
}

void ms_hashmap_clear(ms_hashmap *self) {
    for (int i = 0; i < self->bucket_count; ++i) {
        ms_key_value *pair = self->buckets[i];
        ms_key_value *next = nullptr;
        self->buckets[i] = nullptr;

        while (pair) {
            next = pair->next;
            ms_string_delete(&pair->key);
            free(pair->value.pointer);
            pair = next;
        }
    }

    if (self->bucket_count > MS_HASHMAP_DEFAULT_BUCKETS) {
        self->bucket_count = MS_HASHMAP_DEFAULT_BUCKETS;
        self->buckets = ms_realloc(self->buckets, self->bucket_count);
    }
}

double ms_hashmap_load(ms_hashmap *self, uint64_t bucket_count) {
    return (float)self->pair_count / (float)bucket_count;
}

void ms_hashmap_rehash(ms_hashmap *self, uint64_t new_bucket_count) {
    ms_key_value *pairs = nullptr;
    for (int i = 0; i < self->bucket_count; ++i) {
        ms_key_value *pair = self->buckets[i];
        ms_key_value *next = nullptr;
        self->buckets[i] = nullptr;

        while (pair) {
            if (pairs)
                pairs->next = pair;

            pairs = pair;
            pair = pair->next;
        }
    }

    self->bucket_count = new_bucket_count;
    self->buckets = ms_realloc(self->buckets, self->bucket_count);
    while (pairs) {
        uint32_t hash = ms_fnv1a(pairs->key.c_str, pairs->key.length, SEED) & (self->bucket_count - 1);
        self->buckets[hash] = ms_push_kv(self->buckets[hash], pairs);
    }
}

ms_key_value *ms_hashmap_insert(ms_hashmap *self, ms_string key, void *value) {
    if (ms_hashmap_get(self, key))
        ms_hashmap_remove(self, key);

    uint32_t hash = ms_fnv1a(key.c_str, key.length, SEED) & (self->bucket_count - 1);
    self->buckets[hash] = ms_push_kv(self->buckets[hash], &(ms_key_value){ key, value });

    return self->buckets[hash];
}

ms_key_value *ms_hashmap_get(ms_hashmap *self, ms_string key) {
    uint32_t hash = ms_fnv1a(key.c_str, key.length, SEED) & (self->bucket_count - 1);

    ms_key_value *seek = self->buckets[hash];
    while (seek) {
        if (ms_string_compare(key, seek->key))
            break;
        seek = seek->next;
    }

    return seek;
}

void ms_hashmap_remove(ms_hashmap *self, ms_string key) {
    uint32_t hash = ms_fnv1a(key.c_str, key.length, SEED) & (self->bucket_count - 1);

    ms_key_value *seek = self->buckets[hash];
    ms_key_value *seek_p = nullptr;
    while (seek) {
        if (ms_string_compare(key, seek->key)) {
            if (seek_p)
                seek_p->next = seek->next;
            free(seek);
        }
        seek_p = seek;
        seek = seek->next;
    }
}
