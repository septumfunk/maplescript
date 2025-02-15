#pragma once
#include "bytecode.h"
#include "state.h"

typedef enum {
    MS_STATUS_OK,
    MS_STATUS_RETURN,
    MS_STATUS_COMPILE_ERROR,
    MS_STATUS_RUNTIME_ERROR,
} ms_status;

typedef ms_status (*ms_operation)(ms_state *state);

typedef struct {
    ms_operation operation;
    const char *const asm_name;
    const struct {
        const uint8_t count;
        const ms_operand *const array;
    } operands;
} ms_ins;

ms_status ms_dounit(ms_state *state, ms_unit *unit);

void ms_ins_push(ms_unit *unit, ms_opcode opcode, ...);
char *ms_ins_disassemble(ms_unit *unit, ms_byte **ip);
char *ms_ins_disassemble_all(ms_unit *unit);

#define ms_ins_get(opcode) (&MS_INSTRUCTION_SET[opcode])
extern const ms_ins MS_INSTRUCTION_SET[MS_OP_COUNT];
