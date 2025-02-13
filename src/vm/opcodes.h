#pragma once
#include "../structures/types.h"
#include <stdint.h>

typedef enum : uint8_t {
    MS_OP_NOOP,
    MS_OP_LDCT,
    MS_OP_RET,
    MS_OP_PRNT,

    MS_OP_ADD,
    MS_OP_SUB,

    MS_OP_UNKNOWN,
    MS_OP_COUNT,
} ms_opcode;

typedef struct {
    const char *asm_name;
    const ms_type *const type;
} ms_operand;
