#include "types.h"
#include "strings.h"
#include "../memory/mem.h"
#include <stdlib.h>
#include <string.h>

char *ms_type_tostring(void *);
char *ms_pointer_tostring(void *);

void *ms_u8_add(void *, void *);
void *ms_u8_sub(void *, void *);
char *ms_u8_tostring(void *);

void *ms_u16_add(void *, void *);
void *ms_u16_sub(void *, void *);
char *ms_u16_tostring(void *);

void *ms_u32_add(void *, void *);
void *ms_u32_sub(void *, void *);
char *ms_u32_tostring(void *);

void *ms_u64_add(void *, void *);
void *ms_u64_sub(void *, void *);
char *ms_u64_tostring(void *);


void *ms_i8_add(void *, void *);
void *ms_i8_sub(void *, void *);
char *ms_i8_tostring(void *);

void *ms_i16_add(void *, void *);
void *ms_i16_sub(void *, void *);
char *ms_i16_tostring(void *);

void *ms_i32_add(void *, void *);
void *ms_i32_sub(void *, void *);
char *ms_i32_tostring(void *);

void *ms_i64_add(void *, void *);
void *ms_i64_sub(void *, void *);
char *ms_i64_tostring(void *);


void *ms_f32_add(void *, void *);
void *ms_f32_sub(void *, void *);
char *ms_f32_tostring(void *);

void *ms_f64_add(void *, void *);
void *ms_f64_sub(void *, void *);
char *ms_f64_tostring(void *);

const ms_type MS_PRIMITIVES[MS_PRIMITIVE_COUNT] = {
    [MS_PRIMITIVE_TYPE] = {
        .name = "type",
        .size = sizeof(ms_primitive),
        .add = nullptr,
        .sub = nullptr,
        .tostring = ms_type_tostring,
    },
    [MS_PRIMITIVE_POINTER] = {
        .name = "pointer",
        .size = sizeof(ms_pointer),
        .add = ms_u64_add,
        .sub = ms_u64_sub,
        .tostring = ms_pointer_tostring,
    },

    [MS_PRIMITIVE_U8] = {
        .name = "u8",
        .size = sizeof(ms_u8),
        .add = ms_u8_add,
        .sub = ms_u8_sub,
        .tostring = ms_u8_tostring,
    },
    [MS_PRIMITIVE_U16] = {
        .name = "u16",
        .size = sizeof(ms_u16),
        .add = ms_u16_add,
        .sub = ms_u16_sub,
        .tostring = ms_u16_tostring,
    },
    [MS_PRIMITIVE_U32] = {
        .name = "u32",
        .size = sizeof(ms_u32),
        .add = ms_u32_add,
        .sub = ms_u32_sub,
        .tostring = ms_u32_tostring,
    },
    [MS_PRIMITIVE_U64] = {
        .name = "u64",
        .size = sizeof(ms_u64),
        .add = ms_u64_add,
        .sub = ms_u64_sub,
        .tostring = ms_u64_tostring,
    },

    [MS_PRIMITIVE_I8] = {
        .name = "i8",
        .size = sizeof(ms_i8),
        .add = ms_i8_add,
        .sub = ms_i8_sub,
        .tostring = ms_i8_tostring,
    },
    [MS_PRIMITIVE_I16] = {
        .name = "i16",
        .size = sizeof(ms_i16),
        .add = ms_i16_add,
        .sub = ms_i16_sub,
        .tostring = ms_i16_tostring,
    },
    [MS_PRIMITIVE_I32] = {
        .name = "i32",
        .size = sizeof(ms_i32),
        .add = ms_i32_add,
        .sub = ms_i32_sub,
        .tostring = ms_i32_tostring,
    },
    [MS_PRIMITIVE_I64] = {
        .name = "i64",
        .size = sizeof(ms_i64),
        .add = ms_i64_add,
        .sub = ms_i64_sub,
        .tostring = ms_i64_tostring,
    },

    [MS_PRIMITIVE_F32] = {
        .name = "f32",
        .size = sizeof(ms_f32),
        .add = ms_f32_add,
        .sub = ms_u32_sub,
        .tostring = ms_f32_tostring,
    },
    [MS_PRIMITIVE_F64] = {
        .name = "f64",
        .size = sizeof(ms_f64),
        .add = ms_f64_add,
        .sub = ms_f64_sub,
        .tostring = ms_f64_tostring,
    },
};

ms_value *ms_value_new(ms_primitive type, void *data) {
    const ms_type *t = ms_type_get(type);
    ms_value *value = ms_calloc(1, sizeof(ms_value) + t->size);
    value->data = value + sizeof(ms_value);
    memcpy(value->data, data, ms_type_get(type)->size);
    return value;
}

void ms_value_delete(ms_value *value) {
    free(value);
}

char *ms_type_tostring(void *primitive) {
    // TODO handle invalid type
    return ms_format("%s", ms_type_get(*(ms_primitive *)primitive)->name);
}

char *ms_pointer_tostring(void *pointer) {
    char *ptr = ms_format("0x");
    for (uint64_t i = 0; i < sizeof(ms_pointer); ++i) {
        char *hex = ms_format("%02X", ((uint8_t *)pointer)[i]);
        ptr = ms_strcat(ptr, hex);
        free(hex);
    }

    return ptr;
}

char *ms_u8_tostring(void *u8) { return ms_format("%u", *(ms_u8 *)u8); }
char *ms_u16_tostring(void *u16) { return ms_format("%hu", *(ms_u16 *)u16); }
char *ms_u32_tostring(void *u32) { return ms_format("%u", *(ms_u32 *)u32); }
char *ms_u64_tostring(void *u64) { return ms_format("%llu", *(ms_u64 *)u64); }

char *ms_i8_tostring(void *i8) { return ms_format("%d", *(ms_i8 *)i8); }
char *ms_i16_tostring(void *i16) { return ms_format("%hu", *(ms_i16 *)i16); }
char *ms_i32_tostring(void *i32) { return ms_format("%d", *(ms_i32 *)i32); }
char *ms_i64_tostring(void *i64) { return ms_format("%lld", *(ms_i64 *)i64); }

char *ms_f32_tostring(void *f32) { return ms_format("%f", (double)*(ms_f32 *)f32); }
char *ms_f64_tostring(void *f64) { return ms_format("%f", *(ms_f64 *)f64); }
