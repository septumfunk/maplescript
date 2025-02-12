#include "chunk.h"
#include "instructions.h"
#include "../structures/strings.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_chunk ms_chunk_new() {
    return (ms_chunk) {
        .bytecode = ms_vec_new(uint8_t),
        .dbg_chunks = ms_vec_new(ms_debug_chunk),
    };
}

void ms_chunk_delete(ms_chunk *self) {
    ms_vec_delete(&self->bytecode);
    ms_vec_delete(&self->dbg_chunks);
}

void ms_chunk_save(ms_chunk *self, const char *path) {
    FILE *f = fopen(path, "w");
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file '%s' for writing", path);
        fclose(f);
        return;
    }

    uint64_t written = fwrite(&(ms_chunk_info) {
        .header = MS_CHUNK_HEADER,
        .version = MS_BYTECODE_VERSION,
        .loc_constants = /*sizeof(ms_chunk_info)*/ 0,
        .loc_bytecode = sizeof(ms_chunk_info),
    }, sizeof(ms_chunk_info), 1, f);
    if (written != sizeof(ms_chunk_info)) {
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

/*void ms_chunk_load(ms_chunk *self, const char *path) {

}*/

uint64_t ms_chunk_push_instruction(ms_chunk *self, ms_opcode opcode, ...) {
    ms_vec_push(&self->bytecode, &opcode);

    va_list arglist;
    va_start(arglist, format);
        const ms_instruction *ins = &MS_INSTRUCTION_SET[opcode];
        for (int i = 0; i < ins->operands.count; ++i)
            ms_vec_append(&self->bytecode, va_arg(arglist, void *), ins->operands.array[i].size);
    va_end(arglist);

    return self->bytecode.count - 1;
}

void ms_chunk_push_debug(ms_chunk *self, uint64_t offset, uint64_t line_number) {
    ms_vec_push(&self->dbg_chunks, &(ms_debug_chunk) {
        .offset = offset,
        .line_number = line_number,
    });
}

char *ms_chunk_disassemble(ms_chunk *chunk, uint64_t *offset) {
    const ms_opcode *op = &ms_vec_get(&chunk->bytecode, ms_opcode, *offset);
    const ms_instruction *ins = &MS_INSTRUCTION_SET[MS_OP_UNKNOWN];

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
        ins = &MS_INSTRUCTION_SET[*op];

    char *output = ms_format("%04llu  ", *offset);
    if (current_chunk) {
        char *line_no = ms_format("[%03llu]  ", current_chunk->line_number);
        output = realloc(output, strlen(output) + strlen(line_no) + 1);
        strcat(output, line_no);
        free(line_no);
    }
    output = realloc(output, strlen(output) + strlen(ins->asm_name) + 1);
    strcat(output, ins->asm_name);

    for (int i = 0; i < ins->operands.count; ++i) {
        if (i == 0) {
            output = realloc(output, strlen(output) + 5);
            strcat(output, "  [ ");
        }

        ms_operand oprnd = ins->operands.array[i];
        char *ops = ms_format("%s: 0x", oprnd.asm_name);

        for (int j = 0; j < oprnd.size; ++j) {
            uint8_t b = ms_vec_get(&chunk->bytecode, uint8_t, (*offset)++);
            char *hex = ms_format("%02X", b);
            ops = realloc(ops, strlen(ops) + strlen(hex) + 1);
            strcat(ops, hex);
            free(hex);
        }

        output = realloc(output, strlen(output) + strlen(ops) + 1);
        strcat(output, ops);
        free(ops);

        output = realloc(output, strlen(output) + 3);
        if (i == ins->operands.count - 1)
            strcat(output, " ]");
        else
            strcat(output, ", ");
    }

    (*offset)++;
    return output;
}

char *ms_chunk_disassemble_all(ms_chunk *chunk) {
    char *output = nullptr;

    uint64_t offset = 0;
    while (offset < chunk->bytecode.count) {
        char *line = ms_chunk_disassemble(chunk, &offset);
        output = realloc(output, (output ? sizeof(output) : 0) + strlen(line) + 2);
        strcat(output, line);
        strcat(output, "\n");
        free(line);
    }

    return output;
}
