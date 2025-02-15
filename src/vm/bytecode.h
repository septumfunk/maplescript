#pragma once
#include "../data/vec.h"
#include <stdint.h>

#define MS_UNIT_HEADER { 0x69, 'M', 'P', 'V', 'M' }
#define MS_BYTECODE_VERSION 1.1f

typedef uint8_t ms_byte;
typedef double ms_number;
typedef int64_t ms_integer;
typedef void *ms_pointer;

typedef enum : uint8_t {
    MS_OP_NOOP,
    MS_OP_LDCT,
    MS_OP_POP,
    MS_OP_RET,
    MS_OP_PUTS,
    MS_OP_PRNT,
    MS_OP_JUMP,

    MS_OP_ADD,
    MS_OP_SUB,
    MS_OP_CONV,

    MS_OP_REF,
    MS_OP_ALOC,
    MS_OP_FREE,
    MS_OP_COPY,

    MS_OP_UNKNOWN,
    MS_OP_COUNT,
} ms_opcode;

typedef union {
    ms_byte byte;
    ms_number number;
    ms_integer integer;
    ms_pointer pointer;
} ms_pinnerval;

typedef struct {
    enum ms_ptag : uint8_t {
        MS_P_BYTE,
        MS_P_NUMBER,
        MS_P_INTEGER,
        MS_P_POINTER,

        MS_P_COUNT,
    } pt;
    ms_pinnerval value;
} ms_pvalue;

typedef struct {
    enum ms_ptag pt;
    uint64_t size;
} ms_pointer_header;

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
        ms_pvalue (*convert[MS_P_COUNT])(ms_pvalue self);
    } op;
    ms_pinnerval default_value;
} ms_pdata;

typedef enum {
    MS_STATUS_OK,
    MS_STATUS_RETURN,
    MS_STATUS_COMPILE_ERROR,
    MS_STATUS_RUNTIME_ERROR,
} ms_status;

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
    ms_integer entry;
} ms_unit;

#pragma pack(push, 1)
typedef struct {
    char header[5];
    float version;
    uint64_t loc_constants;
    uint64_t loc_bytecode;
    ms_integer entry;
} ms_unit_info;
#pragma pack(pop)

ms_unit ms_unit_new();
void ms_unit_delete(ms_unit *self);
void ms_unit_save(ms_unit *self, const char *path);
ms_status ms_unit_load_file(ms_unit *self, const char *path);

uint64_t ms_unit_push_instruction(ms_unit *self, ms_opcode opcode, ...);
void ms_unit_push_debug(ms_unit *self, uint64_t offset, uint64_t line_number);

uint64_t ms_unit_push_constant(ms_unit *self, enum ms_ptag pt, void *data);
ms_pvalue ms_unit_get_constant(ms_unit *self, uint64_t offset);
