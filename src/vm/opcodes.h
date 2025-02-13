#pragma once
#include "types.h"
#include <stdint.h>

typedef enum : uint8_t {
    MS_NO_OP,
    MS_OP_CONSTANT,
    MS_OP_ADD,
    MS_OP_SUBTRACT,
    MS_OP_RETURN,

    MS_OP_UNKNOWN,
    MS_OP_COUNT,
} ms_opcode;

typedef struct {
    const char *asm_name;
    const ms_type *const type;
} ms_operand;
