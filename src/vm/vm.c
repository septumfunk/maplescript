#include "vm.h"
#include "unit.h"
#include "instructions.h"
#include "types.h"
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>

#define MS_VM_DISASSEMBLE

ms_vm ms_vm_new() {
    return (ms_vm) {
        .unit = nullptr,
        .ip = nullptr,
    };
}

void ms_vm_delete(ms_vm *self) {
    if (self->unit)
        ms_unit_delete(self->unit);
}

ms_result ms_vm_run(ms_vm *self) {
    assert(self->unit->bytecode.count > 0);
    self->ip = self->unit->bytecode.data;

    uint64_t offset;
    register uint8_t opcode;
    const register ms_instruction *ins;
    while (self->ip < self->unit->bytecode.data + self->unit->bytecode.count) {
        offset = (uint64_t)self->ip - (uint64_t)self->unit->bytecode.data;
        opcode = *self->ip++;
        if (opcode >= MS_OP_UNKNOWN)
            goto runtime_error;
        ins = ms_instruction_get(opcode);

        #ifdef MS_VM_DISASSEMBLE
        char *dasm = ms_unit_disassemble(self->unit, &offset);
        printf("[EXE] %s\n", dasm);
        #endif

        ms_result res = ins->operation(self);
        if (res == MS_RESULT_RUNTIME_ERROR) {
            // Log runtime error disassembly.
            uint64_t offset = (uint64_t)self->ip - (uint64_t)self->unit->bytecode.data;
            char *dasm = ms_unit_disassemble(self->unit, &offset);
            fprintf(stderr, "MapleScript runtime error:\n%s\n", dasm);
            goto runtime_error;
        }
        if (res == MS_RESULT_RETURN) {
            // Temporary behavior, stop execution
            break;
        }
    }
    return MS_RESULT_OK;

runtime_error:
    self->ip = nullptr;
    return MS_RESULT_RUNTIME_ERROR;
}

/*ms_result ms_vm_dostring(const char *code) {
    // TODO
}

ms_result ms_vm_dofile(ms_vm *self, const char *path) {
    // TODO
}*/

ms_result ms_vm_dounit(ms_vm *self, ms_unit *unit) {
    self->unit = unit;
    ms_result res = ms_vm_run(self);
    self->unit = nullptr; // Reset
    return res;
}
