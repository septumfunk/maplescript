#pragma once
#include "opcodes.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *asm_name;
    ms_opcode opcode;
    struct {
        uint8_t count;
        ms_operand *array;
    } operands;
} ms_instruction;

extern const ms_instruction MS_INSTRUCTION_SET[MS_OP_COUNT];
