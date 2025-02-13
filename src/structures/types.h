#pragma once
#include <stdint.h>

typedef enum : uint8_t {
    MS_PRIMITIVE_TYPE,
    MS_PRIMITIVE_POINTER,

    MS_PRIMITIVE_U8,
    MS_PRIMITIVE_U16,
    MS_PRIMITIVE_U32,
    MS_PRIMITIVE_U64,

    MS_PRIMITIVE_I8,
    MS_PRIMITIVE_I16,
    MS_PRIMITIVE_I32,
    MS_PRIMITIVE_I64,

    MS_PRIMITIVE_F32,
    MS_PRIMITIVE_F64,

    MS_PRIMITIVE_COUNT,
} ms_primitive;

typedef void *ms_pointer;

typedef uint8_t ms_u8;
typedef uint16_t ms_u16;
typedef uint32_t ms_u32;
typedef uint64_t ms_u64;

typedef int8_t ms_i8;
typedef int16_t ms_i16;
typedef int32_t ms_i32;
typedef int64_t ms_i64;

typedef float ms_f32;
typedef double ms_f64;

typedef void *(*ms_add)(void *value1, void *value2);
typedef void *(*ms_sub)(void *value1, void *value2);
typedef char *(*ms_tostring)(void *value);

typedef struct {
    const char *const name;
    const size_t size;

    ms_add add;
    ms_sub sub;
    ms_tostring tostring;
} ms_type;

typedef enum {
    MS_RESULT_OK,
    MS_RESULT_RETURN,
    MS_RESULT_COMPILE_ERROR,
    MS_RESULT_RUNTIME_ERROR,
} ms_result;

typedef struct {
    ms_primitive type;
    void *data;
} ms_value;

#define ms_type_get(index) (&MS_PRIMITIVES[index])
extern const ms_type MS_PRIMITIVES[MS_PRIMITIVE_COUNT];

ms_value *ms_value_new(ms_primitive type, void *data);
void ms_value_delete(ms_value *value);
#define ms_value_get(value, type) (*(type *)value->data)
