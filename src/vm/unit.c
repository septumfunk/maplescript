#include "unit.h"
#include "instructions.h"
#include "../structures/strings.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

ms_unit ms_unit_new() {
    return (ms_unit) {
        .bytecode = ms_vec_new(uint8_t),
        .constants = ms_vec_new(ms_value *),
        .dbg_chunks = ms_vec_new(ms_debug_chunk),
    };
}

void ms_unit_delete(ms_unit *self) {
    ms_vec_delete(&self->bytecode);

    for (uint64_t i = 0; i < self->constants.count; ++i)
        ms_value_delete((ms_value *)ms_vec_get(&self->constants, ms_value *, i));
    ms_vec_delete(&self->constants);

    ms_vec_delete(&self->dbg_chunks);
}

void ms_unit_save(ms_unit *self, const char *path) {
    FILE *f = fopen(path, "w");
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file '%s' for writing", path);
        fclose(f);
        return;
    }

    uint64_t written = fwrite(&(ms_unit_info) {
        .header = MS_UNIT_HEADER,
        .version = MS_BYTECODE_VERSION,
        .loc_constants = /*sizeof(ms_unit_info)*/ 0,
        .loc_bytecode = sizeof(ms_unit_info),
    }, sizeof(uint8_t), sizeof(ms_unit_info), f);
    if (written != sizeof(ms_unit_info)) {
        fprintf(stderr, "Failed to write chunk info to file '%s'", path);
        fclose(f);
        return;
    }

    // TODO: constants

    if (fwrite(self->bytecode.data, self->bytecode.element_size, self->bytecode.count, f) != self->bytecode.element_size * self->bytecode.count) {
        fprintf(stderr, "Failed to write bytecode to file '%s'", path);
        fclose(f);
        return;
    }
}

/*void ms_unit_load_file(ms_unit *self, const char *path) {

}*/

uint64_t ms_unit_push_instruction(ms_unit *self, ms_opcode opcode, ...) {
    ms_vec_push(&self->bytecode, &opcode);

    va_list arglist;
    va_start(arglist, format);
        const ms_instruction *ins = ms_instruction_get(opcode);
        for (int i = 0; i < ins->operands.count; ++i)
            ms_vec_append(&self->bytecode, va_arg(arglist, void *), ins->operands.array[i].type->size);
    va_end(arglist);

    return self->bytecode.count - 1;
}

void ms_unit_push_debug(ms_unit *self, uint64_t offset, uint64_t line_number) {
    ms_vec_push(&self->dbg_chunks, &(ms_debug_chunk) {
        .offset = offset,
        .line_number = line_number,
    });
}

uint64_t ms_unit_push_constant(ms_unit *self, ms_primitive type, void *data) {
    ms_value *val = ms_value_new(type, data);
    ms_vec_push(&self->constants, &val);
    uint64_t offset = self->constants.count - 1;
    ms_vec_append(&self->constants, data, ms_type_get(type)->size);
    return offset;
}

char *ms_unit_disassemble(ms_unit *chunk, uint64_t *offset) {
    const ms_opcode *op = &ms_vec_get(&chunk->bytecode, ms_opcode, *offset);
    const ms_instruction *ins = ms_instruction_get(MS_OP_UNKNOWN);

    const ms_debug_chunk *current_chunk = nullptr;
    while (chunk->dbg_chunks.count > 0 && chunk->dbg_index < chunk->dbg_chunks.count) {
        current_chunk = &ms_vec_get(&chunk->dbg_chunks, ms_debug_chunk, chunk->dbg_index);
        if (current_chunk->offset > *offset) {
            chunk->dbg_index--;
            current_chunk = &ms_vec_get(&chunk->dbg_chunks, ms_debug_chunk, chunk->dbg_index);
            if (current_chunk->offset < *offset)
                break;
            continue;
        } else if (current_chunk->offset < *offset && chunk->dbg_index < chunk->dbg_chunks.count - 1) {
            chunk->dbg_index++;
            if (current_chunk->offset < *offset)
                continue;
            else if (current_chunk->offset > *offset) {
                chunk->dbg_index--;
                break;
            } else break;
        } else break;
    }

    if (*op < MS_OP_UNKNOWN) // Valid
        ins = ms_instruction_get(*op);

    char *output = ms_format("%04llu  ", *offset);
    if (current_chunk) {
        char *line_no = ms_format("[%03llu]  ", current_chunk->line_number);
        output = ms_strcat(output, line_no);
        free(line_no);
    }
    output = ms_strcat(output, ins->asm_name);
    (*offset)++;

    for (int i = 0; i < ins->operands.count; ++i) {
        if (i == 0)
            output = ms_strcat(output, "  [ ");

        ms_operand oprnd = ins->operands.array[i];

        char *param = ms_format("%s: ", oprnd.asm_name);
        output = ms_strcat(output, param);
        free(param);

        char *ops = oprnd.type->tostring((void *)&ms_vec_get(&chunk->bytecode, uint8_t, *offset));
        output = ms_strcat(output, ops);
        free(ops);

        if (i == ins->operands.count - 1)
            output = ms_strcat(output, " ]");
        else
            output = ms_strcat(output, ", ");

        *offset += oprnd.type->size;
    }

    return output;
}

char *ms_unit_disassemble_all(ms_unit *chunk) {
    char *output = nullptr;

    uint64_t offset = 0;
    while (offset < chunk->bytecode.count) {
        char *line = ms_unit_disassemble(chunk, &offset);
        output = ms_strcat(output, line);
        output = ms_strcat(output, "\n");
        free(line);
    }

    return output;
}
