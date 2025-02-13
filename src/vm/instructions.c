#include "instructions.h"
#include "opcodes.h"
#include "../structures/types.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

ms_result ms_no_op(void *_vm);
ms_result ms_op_constant(void *_vm);
ms_result ms_op_add(void *_vm);
ms_result ms_op_subtract(void *_vm);
ms_result ms_op_return(void *_vm);

const ms_instruction MS_INSTRUCTION_SET[MS_OP_COUNT] = {
    [MS_NO_OP] = {
        .operation = ms_no_op,
        .asm_name = "nop",
        .operands = {
            .count = 0,
            .array = nullptr,
        }
    },
    [MS_OP_CONSTANT] = {
        .operation = ms_op_constant,
        .asm_name = "con",
        .operands = {
            .count = 1,
            .array = (const ms_operand[]) {
                {
                    .asm_name = "ofs",
                    .type = ms_type_get(MS_PRIMITIVE_POINTER),
                },
            },
        },
    },
    [MS_OP_ADD] = {
        .operation = ms_op_add,
        .asm_name = "add",
        .operands = {
            .count = 2,
            .array = (const ms_operand[]) {
                {
                    .asm_name = "n1",
                    .type = ms_type_get(MS_PRIMITIVE_NUMBER),
                },
                {
                    .asm_name = "n2",
                    .type = ms_type_get(MS_PRIMITIVE_NUMBER),
                },
            },
        },
    },
    [MS_OP_SUBTRACT] = {
        .operation = ms_op_subtract,
        .asm_name = "sub",
        .operands = {
            .count = 2,
            .array = (const ms_operand[]) {
                {
                    .asm_name = "n1",
                    .type = ms_type_get(MS_PRIMITIVE_NUMBER),
                },
                {
                    .asm_name = "n2",
                    .type = ms_type_get(MS_PRIMITIVE_NUMBER),
                },
            },
        },
    },
    [MS_OP_RETURN] = {
        .operation = ms_op_return,
        .asm_name = "ret",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    },
    [MS_OP_UNKNOWN] = {
        .operation = ms_no_op,
        .asm_name = "???",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    }
};

ms_result ms_no_op(void *_vm) {
    (void)_vm;
    return MS_RESULT_OK;
}

ms_result ms_op_constant(void *_vm) {
    ms_vm *vm = _vm;
    ms_pointer ptr = *(ms_pointer *)vm->ip;
    vm->ip += sizeof(ms_pointer);

    char *str = ms_type_get(MS_PRIMITIVE_POINTER)->tostring(&ptr);
    printf("Const: %s\n", str);
    free(str);

    return MS_RESULT_OK;
}

ms_result ms_op_add(void *_vm) {
    (void)_vm;
    // TODO
    return MS_RESULT_OK;
}

ms_result ms_op_subtract(void *_vm) {
    (void)_vm;
    // TODO
    return MS_RESULT_OK;
}

ms_result ms_op_return(void *_vm) {
    (void)_vm;
    return MS_RESULT_RETURN;
}


