#pragma once
#include "opcodes.h"
#include <stddef.h>
#include <stdint.h>

typedef ms_result (*ms_operation)(void *_vm);

typedef struct {
    ms_operation operation;
    const char *const asm_name;
    const struct {
        const uint8_t count;
        const ms_operand *const array;
    } operands;
} ms_instruction;

#define ms_instruction_get(opcode) (&MS_INSTRUCTION_SET[opcode])
extern const ms_instruction MS_INSTRUCTION_SET[MS_OP_COUNT];
