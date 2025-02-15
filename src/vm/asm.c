#include "asm.h"
#include "../data/strings.h"
#include "bytecode.h"
#include "instruction.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_assembler ms_assembler_new() {
    ms_assembler asmb = {
        .ppt = ms_map_new(),
        .const_count = 0,
        .constants = ms_map_new(),
        .labels = ms_map_new(),

        .opcodes = ms_map_new(),
        .types = ms_map_new(),
    };

    for (uint64_t i = 0; i < MS_OP_COUNT; ++i) {
        const ms_ins *ins = ms_ins_get(i);
        ms_map_insert(&asmb.opcodes, ins->asm_name, &ins);
    }

    for (uint64_t i = 0; i < MS_P_COUNT; ++i) {
        const ms_pdata *pd = ms_pdata_get(i);
        ms_map_insert(&asmb.types, pd->name, &pd);
    }

    return asmb;
}

void ms_assembler_delete(ms_assembler *assembler) {
    ms_map_delete(&assembler->opcodes);
    ms_map_delete(&assembler->labels);
    ms_map_delete(&assembler->constants);
}

ms_status ms_assemble(ms_assembler *assembler, ms_unit *unit, const char *string) {
    // Remove windows line breaks
    char *lb_removed = ms_strdup(string);
    for (char *c = lb_removed; c < lb_removed + strlen(lb_removed); ++c)
        *c = *c == '\r' ? ' ' : *c;

    char *cm_removed = nullptr;
    for (char *line = strtok(lb_removed, "\n"); line; line = strtok(NULL, "\n")) {
        for (char *c = line; c < line + strlen(line); ++c) {
            if (*c == ';')
                goto next;
            char ap[2] = { *c, '\0' };
            cm_removed = ms_strcat(cm_removed, ap);
        }
        next: continue;
    }
    free(lb_removed);

    // Preprocess
    char *preprocessed = nullptr;
    for (char *token = strtok(cm_removed, " "); token; token = strtok(NULL, " ")) {
        if (ms_strcmp(token, "#define")) {
            char *key = strtok(NULL, " ");
            char *value = ms_strdup(strtok(NULL, "\n"));
            ms_map_insert(&assembler->ppt, key, &value);
            continue;
        }
        if (ms_map_exists(&assembler->ppt, token)) {
            preprocessed = ms_strcat(preprocessed, ms_map_get(&assembler->ppt, char *, token));
            continue;
        }
        preprocessed = ms_strcat(preprocessed, token);
        preprocessed = ms_strcat(preprocessed, " ");
    }
    free(cm_removed);

    for (char *token = strtok(preprocessed, " "); token; token = strtok(NULL, " ")) {
        if (ms_strcmp(token, "#const")) {
            char *name = strtok(NULL, " ");
            char *type = strtok(NULL, " ");
            if (!ms_map_exists(&assembler->types, type)) {
                fprintf(stderr, "Compile Error: Unknown Type '%s'\n", type);
                return MS_STATUS_COMPILE_ERROR;
            }

            ms_pdata *pd = ms_map_get(&assembler->types, ms_pdata *, type);
            char *value = strtok(NULL, " ");
            ms_pvalue v = {.pt = pd->pt};
            switch (v.pt) {
                case MS_P_BYTE:
                    v.value.byte = (ms_byte)atoll(value);
                    break;
                case MS_P_NUMBER:
                    v.value.number = (ms_number)atof(value);
                    break;
                case MS_P_INTEGER:
                    v.value.integer = (ms_integer)atoll(value);
                    break;
                case MS_P_POINTER:
                    v.value.pointer = (ms_pointer)atoll(value);
                    break;
                default:
                    return MS_STATUS_COMPILE_ERROR;
            }

            ms_const_alias ca = { v, (ms_integer)assembler->const_count++ };
            ms_map_insert(&assembler->constants, name, &ca);
            ms_unit_push_constant(unit, v.pt, &v.value);
            continue;
        }

        if (*(token + strlen(token) - 1) == ':') { // Label
            *(token + strlen(token) - 1) = 0;
            ms_map_insert(&assembler->labels, token, &(ms_integer) { (ms_integer)unit->bytecode.count });
            continue;
        }

        if (!ms_map_exists(&assembler->opcodes, token)) { // Unknown opcode
            fprintf(stderr, "Compile Error: Unknown Opcode '%s'\n", token);
            return MS_STATUS_COMPILE_ERROR;
        }

        const ms_ins *ins = ms_map_get(&assembler->opcodes, const ms_ins *, token);
        uint64_t ip = unit->bytecode.count;
        ms_vec_push(&unit->bytecode, (void *)&ins->opcode);
        for (uint64_t i = 0; i < ins->operands.count; ++i) {
            const ms_operand *oprnd = &ins->operands.array[i];
            char *value = strtok(NULL, " ");
            if (ms_map_exists(&assembler->constants, value)) {
                const ms_const_alias *ca = &ms_map_get(&assembler->constants, ms_const_alias, value);
                ms_vec_append(&unit->bytecode, (void *)&ca->index, sizeof(ca->index));
            } else switch (oprnd->pt) {
                case MS_P_BYTE:
                    auto b = (ms_byte)atoll(value);
                    ms_vec_append(&unit->bytecode, &b, sizeof(b));
                    break;
                case MS_P_NUMBER:
                    auto num = (ms_number)atof(value);
                    ms_vec_append(&unit->bytecode, &num, sizeof(num));
                    break;
                case MS_P_INTEGER:
                    if (ms_map_exists(&assembler->labels, value)) {
                        auto offset = ms_map_get(&assembler->labels, uint64_t, value);
                        ms_vec_append(&unit->bytecode, &(ms_integer){(ms_integer)offset}, sizeof(ms_integer));
                        break;
                    }
                    auto in = (ms_integer)atoll(value);
                    ms_vec_append(&unit->bytecode, &in, sizeof(in));
                    break;
                case MS_P_POINTER:
                    auto ptr = (ms_pointer)atoll(value);
                    ms_vec_append(&unit->bytecode, &ptr, sizeof(ptr));
                    break;
                default: return MS_STATUS_COMPILE_ERROR; //! WHAT
            }
        }

        ip += (uint64_t)unit->bytecode.data;
        char *dasm = ms_ins_disassemble(unit, &(ms_byte *){(ms_byte *)ip});
        char *out = ms_format("[ASM] %s\n", dasm);
        printf("%s", out);
        free(out);
        free(dasm);
    }

    // Entry
    if (ms_map_exists(&assembler->labels, "main"))
        unit->entry = ms_map_get(&assembler->labels, ms_integer, "main");

    printf("Assembly completed!");
    free(preprocessed);
    return MS_STATUS_OK;
}
