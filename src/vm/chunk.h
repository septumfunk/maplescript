#pragma once
#include "../structures/vec.h"
#include "opcodes.h"
#include <stdint.h>

#define MS_CHUNK_HEADER "MAPLE"
#define MS_BYTECODE_VERSION 1.0f

typedef struct {
    uint64_t offset;
    uint64_t line_number;
} ms_debug_chunk;

typedef struct {
    ms_vec bytecode;
    ms_vec dbg_chunks;
    uint64_t dbg_index;
} ms_chunk;

#pragma pack(push, 1)
typedef struct {
    char header[5];
    float version;
    uint64_t loc_constants;
    uint64_t loc_bytecode;
} ms_chunk_info;
#pragma pack(pop)

ms_chunk ms_chunk_new();
void ms_chunk_delete(ms_chunk *self);
void ms_chunk_save(ms_chunk *self, const char *path);
void ms_chunk_load(ms_chunk *self, const char *path);

uint64_t ms_chunk_push_instruction(ms_chunk *self, ms_opcode opcode, ...);
void ms_chunk_push_debug(ms_chunk *self, uint64_t offset, uint64_t line_number);

char *ms_chunk_disassemble(ms_chunk *chunk, uint64_t *offset);
char *ms_chunk_disassemble_all(ms_chunk *chunk);
