#pragma once
#include "../data/vec.h"
#include <stdint.h>

#define MS_UNIT_HEADER "MAPLE"
#define MS_BYTECODE_VERSION 1.0f

typedef uint8_t ms_byte;
typedef double ms_number;
typedef int64_t ms_integer;
typedef void *ms_pointer;

typedef enum : uint8_t {
    MS_OP_NOOP,
    MS_OP_LDCT,
    MS_OP_RET,
    MS_OP_PRNT,

    MS_OP_ADD,
    MS_OP_SUB,

    MS_OP_UNKNOWN,
    MS_OP_COUNT,
} ms_opcode;

typedef struct {
    enum ms_ptag {
        MS_P_BYTE,
        MS_P_NUMBER,
        MS_P_INTEGER,
        MS_P_POINTER,

        MS_P_COUNT,
    } pt;
    union ms_pinnerval {
        ms_byte byte;
        ms_number number;
        ms_integer integer;
        ms_pointer pointer;
    } value;
} ms_pvalue;

typedef struct {
    const char *asm_name;
    const enum ms_ptag pt;
} ms_operand;

typedef struct ms_pdata {
    const enum ms_ptag pt;
    const char *const name;
    const size_t size;

    const struct ms_operators {
        char *(*string)(ms_pvalue self);
        ms_pvalue (*add)(ms_pvalue obj1, ms_pvalue obj2);
        ms_pvalue (*sub)(ms_pvalue obj1, ms_pvalue obj2);
    } op;
} ms_pdata;

extern const ms_pdata MS_PRIMITIVES[MS_P_COUNT];
#define ms_pdata_get(pt) (&MS_PRIMITIVES[pt])

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

uint64_t ms_unit_push_constant(ms_unit *self, enum ms_ptag pt, void *data);
ms_pvalue ms_unit_get_constant(ms_unit *self, uint64_t offset);
