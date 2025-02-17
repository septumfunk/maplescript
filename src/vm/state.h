#pragma once
#include "bytecode.h"
#include "../memory/stack.h"

#define MS_STACK_MAX 1024
#define MS_DEBUG

typedef struct {
    char *str;
    ms_byte *ins_s;
} ms_runtime_error;

typedef struct {
    ms_unit *unit;
    ms_byte *ip;
    char *err;

    ms_stack stack;
    ms_vec usertypes;
} ms_state;

ms_state *ms_state_new();
void ms_state_delete(ms_state *state);

ms_byte *ms_push(ms_state *state, ms_pvalue value);
void ms_pop(ms_state *state, ms_integer count);
ms_pvalue ms_get(ms_state *state, ms_integer reg);
void ms_set(ms_state *state, ms_integer reg, ms_pvalue value);
ms_pvalue ms_loadconst(ms_state *state, ms_integer index);

const ms_pdata *ms_type(ms_state *state, ms_integer reg);
void ms_print(ms_state *state, ms_integer reg);

void ms_error(ms_state *state, const char *format, ...);

ms_pointer ms_vcalloc(ms_state *state, ms_integer size);
void ms_vfree(ms_state *state, ms_pointer ptr);
