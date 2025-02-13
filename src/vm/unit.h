#pragma once
#include "../structures/vec.h"
#include "opcodes.h"
#include <stdint.h>

#define MS_UNIT_HEADER "MAPLE"
#define MS_BYTECODE_VERSION 1.0f

typedef struct {
    uint64_t offset;
    uint64_t line_number;
} ms_debug_chunk;

typedef struct {
    ms_vec bytecode;
    ms_vec constants;
    ms_vec dbg_chunks;
    uint64_t dbg_index;
} ms_unit;

#pragma pack(push, 1)
typedef struct {
    char header[5];
    float version;
    uint64_t loc_constants;
    uint64_t loc_bytecode;
} ms_unit_info;
#pragma pack(pop)

ms_unit ms_unit_new();
void ms_unit_delete(ms_unit *self);
void ms_unit_save(ms_unit *self, const char *path);
void ms_unit_load_file(ms_unit *self, const char *path);

uint64_t ms_unit_push_instruction(ms_unit *self, ms_opcode opcode, ...);
void ms_unit_push_debug(ms_unit *self, uint64_t offset, uint64_t line_number);

char *ms_unit_disassemble(ms_unit *chunk, uint64_t *offset);
char *ms_unit_disassemble_all(ms_unit *chunk);
