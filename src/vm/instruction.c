#include "instruction.h"
#include "bytecode.h"
#include "state.h"
#include "../data/strings.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// *---------------------
// * Instruction Helpers
// *---------------------

ms_status ms_dounit(ms_state *state, ms_unit *unit) {
    assert(unit->bytecode.count > 0);
    state->unit = unit;
    state->ip = unit->bytecode.data;

    register uint8_t opcode;
    const register ms_ins *ins;
    register ms_byte *ins_s;

    while (state->ip < unit->bytecode.top) {
        ins_s = state->ip;
        opcode = *state->ip++;
        if (opcode >= MS_OP_UNKNOWN)
            goto runtime_error;
        ins = ms_ins_get(opcode);

        #ifdef MS_DEBUG
        char *dasm = ms_ins_disassemble(unit, &(ms_byte *){ins_s});
        printf("[EXE] %s\n", dasm);
        free(dasm);
        #endif

        ms_status res = ins->operation(state);
        if (res == MS_STATUS_RUNTIME_ERROR) {
            char *dasm = ms_ins_disassemble(unit, &(ms_byte *){ins_s});
            fprintf(stderr, "[%s]\n", dasm);
            free(dasm);

            fprintf(stderr, " ^\n %s\n", state->err);
            free(state->err);
            goto runtime_error;
        }
        if (res == MS_STATUS_RETURN) {
            // Temporary behavior, stop execution
            break;
        }
    }

    state->ip = nullptr;
    state->unit = nullptr;
    return MS_STATUS_OK;

runtime_error:
    state->ip = nullptr;
    state->unit = nullptr;
    return MS_STATUS_RUNTIME_ERROR;
}

void ms_ins_push(ms_unit *unit, ms_opcode opcode, ...) {
    ms_vec_push(&unit->bytecode, &opcode);
    va_list arglist;
    va_start(arglist, format);
        const ms_ins *ins = ms_ins_get(opcode);
        for (int i = 0; i < ins->operands.count; ++i)
            ms_vec_append(&unit->bytecode, va_arg(arglist, void *), ms_pdata_get(ins->operands.array[i].pt)->size);
    va_end(arglist);
}

char *ms_ins_disassemble(ms_unit *unit, ms_byte **ip) {
    uint64_t offset = (uint64_t)*ip - (uint64_t)unit->bytecode.data;
    const ms_debug_chunk *current_chunk = nullptr;
    while (unit->dbg_chunks.count > 0 && unit->dbg_index < unit->dbg_chunks.count) {
        current_chunk = &ms_vec_get(&unit->dbg_chunks, ms_debug_chunk, unit->dbg_index);
        if (current_chunk->offset > offset) {
            unit->dbg_index--;
            current_chunk = &ms_vec_get(&unit->dbg_chunks, ms_debug_chunk, unit->dbg_index);
            if (current_chunk->offset < offset)
                break;
            continue;
        } else if (current_chunk->offset < offset && unit->dbg_index < unit->dbg_chunks.count - 1) {
            unit->dbg_index++;
            if (current_chunk->offset < offset)
                continue;
            else if (current_chunk->offset > offset) {
                unit->dbg_index--;
                break;
            } else break;
        } else break;
    }

    const ms_ins *ins = ms_ins_get(MS_OP_UNKNOWN);
    if (**ip < MS_OP_UNKNOWN) // Valid
        ins = ms_ins_get(**ip);

    char *output = ms_format("%04llu  ", offset);
    if (current_chunk) {
        char *line_no = ms_format("[%03llu]  ", current_chunk->line_number);
        output = ms_strcat(output, line_no);
        free(line_no);
    }
    output = ms_strcat(output, ins->asm_name);
    (*ip)++;

    for (int i = 0; i < ins->operands.count; ++i) {
        if (i == 0)
            output = ms_strcat(output, "  [ ");

        ms_operand oprnd = ins->operands.array[i];

        char *param = ms_format("%s: ", oprnd.asm_name);
        output = ms_strcat(output, param);
        free(param);

        auto pdata = ms_pdata_get(oprnd.pt);
        ms_pvalue val = { .pt = oprnd.pt };
        memcpy(&val.value, *ip, pdata->size);

        char *ops = pdata->op.string(val);
        output = ms_strcat(output, ops);
        free(ops);

        if (i == ins->operands.count - 1)
            output = ms_strcat(output, " ]");
        else
            output = ms_strcat(output, ", ");

        *ip += ms_pdata_get(oprnd.pt)->size;
    }

    return output;
}

char *ms_ins_disassemble_all(ms_unit *unit) {
    ms_byte *ip = unit->bytecode.data;

    char *output = nullptr;
    while (ip < unit->bytecode.top) {
        char *line = ms_ins_disassemble(unit, &ip);
        output = ms_strcat(output, line);
        output = ms_strcat(output, "\n");
        free(line);
    }

    return output;
}

#define ms_operand(type) (*(type *)((state->ip += sizeof(type)) - sizeof(type)))

// *------------
// * Operations
// *------------
// ms_status ms_ins_(ms_state *state)

ms_status ms_ins_noop(ms_state *state) {
    (void)state;
    return MS_STATUS_OK;
}

ms_status ms_ins_ldct(ms_state *state) {
    auto offset = ms_operand(ms_integer);
    auto val = ms_loadconst(state, offset);
    ms_push(state, val);

    return MS_STATUS_OK;
}

ms_status ms_ins_ret(ms_state *state) {
    (void)state;
    // TODO: Stack frames
    return MS_STATUS_RETURN;
}

ms_status ms_ins_prnt(ms_state *state) {
    auto val = ms_get(state, -2);
    if (!ms_pdata_get(val.pt)->op.string) {
        ms_error(state, "Type %s does not implement string.", ms_pdata_get(val.pt)->name);
        return MS_STATUS_RUNTIME_ERROR;
    }

    ms_print(state, -1);
    return MS_STATUS_OK;
}

ms_status ms_ins_add(ms_state *state) {
    auto val1 = ms_get(state, -2);
    auto val2 = ms_get(state, -1);

    if (val1.pt != val2.pt) {
        ms_error(state, "Cannot add values of different types (%s, %s).", ms_pdata_get(val1.pt)->name, ms_pdata_get(val2.pt)->name);
        return MS_STATUS_RUNTIME_ERROR;
    }

    if (!ms_pdata_get(val1.pt)->op.add) {
        ms_error(state, "Type %s does not implement add.", ms_pdata_get(val1.pt)->name);
        return MS_STATUS_RUNTIME_ERROR;
    }

    ms_pop(state, 2);

    ms_push(state, ms_pdata_get(val1.pt)->op.add(val1, val2));

    return MS_STATUS_OK;
}

ms_status ms_ins_sub(ms_state *state) {
    auto val1 = ms_get(state, -2);
    auto val2 = ms_get(state, -1);

    if (val1.pt != val2.pt) {
        ms_error(state, "Cannot subtract values of different types (%s, %s).", ms_pdata_get(val1.pt)->name, ms_pdata_get(val2.pt)->name);
        return MS_STATUS_RUNTIME_ERROR;
    }

    if (!ms_pdata_get(val1.pt)->op.add) {
        ms_error(state, "Type %s does not implement sub.", ms_pdata_get(val1.pt)->name);
        return MS_STATUS_RUNTIME_ERROR;
    }

    ms_pop(state, 2);

    ms_push(state, ms_pdata_get(val1.pt)->op.sub(val1, val2));

    return MS_STATUS_OK;
}

// *-----------------
// * Instruction Set
// *-----------------

const ms_ins MS_INSTRUCTION_SET[MS_OP_COUNT] = {
    [MS_OP_NOOP] = {
        .operation = ms_ins_noop,
        .asm_name = "noop",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },
    [MS_OP_LDCT] = {
        .operation = ms_ins_ldct,
        .asm_name = "ldct",
        .operands = {
            .count = 1,
            .array = (const ms_operand[]) {
                {
                    .pt = MS_P_INTEGER,
                    .asm_name = "offset",
                },
            },
        }
    },
    [MS_OP_RET] = {
        .operation = ms_ins_ret,
        .asm_name = "ret",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },
    [MS_OP_PRNT] = {
        .operation = ms_ins_prnt,
        .asm_name = "prnt",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },

    [MS_OP_ADD] = {
        .operation = ms_ins_add,
        .asm_name = "add",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },
    [MS_OP_SUB] = {
        .operation = ms_ins_sub,
        .asm_name = "sub",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },

    [MS_OP_UNKNOWN] = {
        .operation = ms_ins_noop,
        .asm_name = "???",
        .operands = {
            .count = 0,
            .array = /*(const ms_operand[])*/nullptr,
        }
    },
};
