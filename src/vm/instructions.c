#include "instructions.h"
#include "opcodes.h"
#include "types.h"

const ms_instruction MS_INSTRUCTION_SET[MS_OP_COUNT] = {
    [MS_OP_CONSTANT] = (ms_instruction) {
        .asm_name = "con",
        .opcode = MS_OP_CONSTANT,
        .operands = {
            .count = 1,
            .array = (ms_operand[]) {
                (ms_operand) { .asm_name = "ofs", .size = sizeof(ms_pointer) },
            },
        },
    },
    [MS_OP_ADD] = (ms_instruction) {
        .asm_name = "add",
        .opcode = MS_OP_ADD,
        .operands = {
            .count = 2,
            .array = (ms_operand[]) {
                (ms_operand) { .asm_name = "n1", .size = sizeof(ms_number) },
                (ms_operand) { .asm_name = "n2", .size = sizeof(ms_number) },
            },
        },
    },
    [MS_OP_SUBTRACT] = (ms_instruction) {
        .asm_name = "sub",
        .opcode = MS_OP_SUBTRACT,
        .operands = {
            .count = 2,
            .array = (ms_operand[]) {
                (ms_operand) { .asm_name = "n1", .size = sizeof(ms_number) },
                (ms_operand) { .asm_name = "n2", .size = sizeof(ms_number) },
            },
        },
    },
    [MS_OP_RETURN] = (ms_instruction) {
        .asm_name = "ret",
        .opcode = MS_OP_RETURN,
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    },
    [MS_OP_UNKNOWN] = (ms_instruction) {
        .asm_name = "???",
        .opcode = MS_OP_UNKNOWN,
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    }
};
