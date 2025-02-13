#pragma once
#include "unit.h"
#include "../structures/stack.h"

typedef struct {
    ms_unit *unit;
    const uint8_t *ip;
    ms_stack stack;
} ms_vm;

ms_vm ms_vm_new();
void ms_vm_delete(ms_vm *self);

ms_result ms_vm_dostring(ms_vm *self, const char *code);
ms_result ms_vm_dofile(ms_vm *self, const char *path);
ms_result ms_vm_dounit(ms_vm *self, ms_unit *unit);
