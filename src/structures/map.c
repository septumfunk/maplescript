#include "map.h"
#include "../memory/mem.h"
#include "strings.h"
#include <assert.h>
#include <stdio.h>
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
    while (size--) {
        auto cc = *head++;
        hash = (cc ^ hash) * PRIME;
    }
    return hash;
}

ms_map ms_map_new(void) {
    return (ms_map) {
        .bucket_count = 0,
        .pair_count = 0,
        .buckets = nullptr,
    };
}

void ms_map_delete(ms_map *self) {
    ms_map_clear(self);
    free(self->buckets);
}

void ms_map_clear(ms_map *self) {
    for (uint64_t i = 0; i < self->bucket_count; ++i) {
        ms_key_value *pair = self->buckets[i];
        ms_key_value *next = nullptr;
        self->buckets[i] = nullptr;

        while (pair) {
            next = pair->next;
            free((char *)pair->key);
            free((void *)pair->value.pointer);
            pair = next;
        }
    }

    if (self->bucket_count > MS_MAP_DEFAULT_BUCKETS) {
        self->bucket_count = MS_MAP_DEFAULT_BUCKETS;
        self->buckets = ms_realloc(self->buckets, self->bucket_count);
    }
}

double ms_map_load(ms_map *self, uint64_t bucket_count) {
    return (double)self->pair_count / (double)bucket_count;
}

void ms_map_rehash(ms_map *self, uint64_t new_bucket_count) {
    ms_key_value *pairs = nullptr;
    for (uint64_t i = 0; i < self->bucket_count; ++i) {
        ms_key_value *pair = self->buckets[i];
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
        uint32_t hash = ms_fnv1a(pairs->key, strlen(pairs->key), SEED) & (self->bucket_count - 1);
        self->buckets[hash] = ms_push_kv(self->buckets[hash], pairs);
    }
}

void _ms_map_insert(ms_map *self, const char *key, void *value, size_t size) {
    if (!self->buckets || self->bucket_count) {
        self->buckets = ms_calloc(MS_MAP_DEFAULT_BUCKETS, sizeof(ms_key_value *));
        self->bucket_count = MS_MAP_DEFAULT_BUCKETS;
    }

    if (ms_map_exists(self, key))
        ms_map_remove(self, key);

    void *value_copy = ms_malloc(size);
    memcpy(value_copy, value, size);

    uint32_t hash = ms_fnv1a(key, strlen(key), SEED) & (self->bucket_count - 1);
    ms_key_value *pair = ms_malloc(sizeof(ms_key_value));
    memcpy(pair, &(ms_key_value) {
        .key = ms_strdup(key),
        .value = {
            .pointer = value_copy,
            .size = size
        },
    }, sizeof(ms_key_value));
    self->buckets[hash] = ms_push_kv(self->buckets[hash], pair);
}

const void *_ms_map_get(ms_map *self, const char *key) {
    assert(self->buckets && "Map is not initialized.");
    uint32_t hash = ms_fnv1a(key, strlen(key), SEED) & (self->bucket_count - 1);

    ms_key_value *seek = self->buckets[hash];
    while (seek) {
        printf("%s\n", key);
        printf("%s\n", seek->key);
        if (ms_strcmp(key, seek->key))
            break;
        seek = seek->next;
    }

    assert(seek != nullptr && "Map entry does not exist.");

    return seek->value.pointer;
}

void ms_map_remove(ms_map *self, const char *key) {
    uint32_t hash = ms_fnv1a(key, strlen(key), SEED) & (self->bucket_count - 1);

    ms_key_value *seek = self->buckets[hash];
    ms_key_value *seek_p = nullptr;
    while (seek) {
        if (ms_strcmp(key, seek->key)) {
            if (seek_p)
                seek_p->next = seek->next;
            free(seek);
        }
        seek_p = seek;
        seek = seek->next;
    }
}

bool ms_map_exists(ms_map *self, const char *key) {
    uint32_t hash = ms_fnv1a(key, strlen(key), SEED) & (self->bucket_count - 1);
    ms_key_value *seek = self->buckets[hash];
    while (seek) {
        if (ms_strcmp(key, seek->key))
            break;
        seek = seek->next;
    }

    return seek ? true : false;
}
