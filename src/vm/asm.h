#pragma once
#include "instruction.h"
#include "../data/map.h"

typedef struct {
    ms_pvalue value;
    ms_integer index;
} ms_const_alias;

typedef struct {
    ms_map ppt;
    uint64_t const_count;
    ms_map constants;
    ms_map labels;

    ms_map opcodes;
    ms_map types;
} ms_assembler;

ms_assembler ms_assembler_new();
void ms_assembler_delete(ms_assembler *assembler);

ms_status ms_assemble(ms_assembler *assembler, ms_unit *unit, const char *string);
