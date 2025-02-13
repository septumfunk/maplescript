#include "instructions.h"
#include "opcodes.h"
#include "../structures/types.h"
#include "vm.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

#define ms_operand_get(type) (*(type *)((vm->ip += sizeof(type)) - sizeof(type)))

ms_result ms_op_noop(void *_vm);
ms_result ms_op_ldct(void *_vm);
ms_result ms_op_ret(void *_vm);
ms_result ms_op_prnt(void *_vm);

ms_result ms_op_add(void *_vm);
ms_result ms_op_sub(void *_vm);

const ms_instruction MS_INSTRUCTION_SET[MS_OP_COUNT] = {
    [MS_OP_NOOP] = {
        .operation = ms_op_noop,
        .asm_name = "noop",
        .operands = {
            .count = 0,
            .array = nullptr,
        }
    },
    [MS_OP_LDCT] = {
        .operation = ms_op_ldct,
        .asm_name = "ldct",
        .operands = {
            .count = 1,
            .array = (const ms_operand[]) {
                {
                    .asm_name = "offset",
                    .type = ms_type_get(MS_PRIMITIVE_POINTER),
                },
            },
        },
    },
    [MS_OP_RET] = {
        .operation = ms_op_ret,
        .asm_name = "ret",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    },
    [MS_OP_PRNT] = {
        .operation = ms_op_prnt,
        .asm_name = "prnt",
        .operands = {
            .count = 1,
            .array = (const ms_operand[]) {
                {
                    .asm_name = "offset",
                    .type = ms_type_get(MS_PRIMITIVE_I32),
                }
            },
        },
    },

    [MS_OP_ADD] = {
        .operation = ms_op_add,
        .asm_name = "add",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    },
    [MS_OP_SUB] = {
        .operation = ms_op_sub,
        .asm_name = "sub",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    },

    [MS_OP_UNKNOWN] = {
        .operation = ms_op_noop,
        .asm_name = "???",
        .operands = {
            .count = 0,
            .array = nullptr,
        },
    }
};

ms_result ms_op_noop(void *_vm) {
    (void)_vm;
    return MS_RESULT_OK;
}

ms_result ms_op_ldct(void *_vm) {
    ms_vm *vm = _vm;
    auto type = ms_operand_get(ms_primitive);
    auto ptr = ms_operand_get(ms_pointer);

    ms_stack_push(&vm->stack, type, ptr);

    return MS_RESULT_OK;
}

ms_result ms_op_ret(void *_vm) {
    (void)_vm;
    return MS_RESULT_RETURN;
}

ms_result ms_op_prnt(void *_vm) {
    ms_vm *vm = _vm;
    auto offset = ms_operand_get(ms_i32);

    auto val = ms_stack_get(&vm->stack);
    if (!val) {
        fprintf(stderr, "Runtime Error: Empty stack.");
        return MS_RESULT_RUNTIME_ERROR;
    }

    char *tstr = ms_type_get(val->type)->tostring(val->data);
    fprintf(stdout, "%s", tstr);
    free(tstr);

    return MS_RESULT_OK;
}

ms_result ms_op_add(void *_vm) {
    ms_vm *vm = _vm;
    auto num1 = ms_stack_get(&vm->stack, -2);
    auto num2 = ms_stack_get(&vm->stack, -1);

    if (!num1 || !num2 || (num1->type != MS_PRIMITIVE_U8 && num1->type != MS_PRIMITIVEIU8) || (num1->type != MS_PRIMITIVE_U8 && num1->type != MS_PRIMITIVEIU8)) {
        fprintf(stderr, "Runtime Error: Two bytes not found on the stack.");
        return MS_RESULT_RUNTIME_ERROR;
    }

    ms_stack_pop(&vm->stack, 2);
    ms_stack_push(&vm->stack, MS_PRIMITIVE_U8, &(ms_u8){ ms_value_get(num1, ms_u8) + ms_value_get(num2, ms_u8) });

    return MS_RESULT_OK;
}

ms_result ms_op_sub(void *_vm) {
    ms_vm *vm = _vm;
    auto num1 = ms_stack_get(&vm->stack, -2);
    auto num2 = ms_stack_get(&vm->stack, -1);

    if (num1->type != num2->type) {
        fprintf(stderr, "Runtime Error: Cannot subtract values of different types.");
        return MS_RESULT_RUNTIME_ERROR;
    }

    const ms_type *t = ms_type_get(num1->type);
    if (!t->sub) {
        fprintf(stderr, "Runtime Error: Type '%s' does not implement sub.", t->name);
        return MS_RESULT_RUNTIME_ERROR;
    }

    

    ms_stack_pop(&vm->stack, 2);
    ms_stack_push(&vm->stack, MS_PRIMITIVE_U8, &(ms_u8) { ms_value_get(num1, ms_u8) - ms_value_get(num2, ms_u8) });

    return MS_RESULT_OK;
}
