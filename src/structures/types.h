#pragma once
#include <stdint.h>

typedef float ms_number;
typedef uint64_t ms_pointer;

typedef char *(*ms_tostring)(void *value);

typedef enum : uint8_t {
    MS_PRIMITIVE_NUMBER,
    MS_PRIMITIVE_POINTER,

    MS_PRIMITIVE_COUNT,
} ms_primitive;

typedef struct {
    const char *const name;
    const size_t size;
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
    uint8_t *data;
} ms_value;

#define ms_type_get(index) (&MS_PRIMITIVES[index])
extern const ms_type MS_PRIMITIVES[MS_PRIMITIVE_COUNT];
