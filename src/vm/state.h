#pragma once
#include "bytecode.h"
#include "../memory/stack.h"

#define MS_STACK_MAX 1024
#define MS_DEBUG

typedef struct {
    ms_unit *unit;
    ms_byte *ip;

    ms_stack stack;
    ms_vec usertypes;
} ms_state;

ms_state *ms_state_new();
void ms_state_delete(ms_state *state);

ms_byte *ms_push(ms_state *state, ms_pvalue value);
void ms_pop(ms_state *state, ms_integer count);
ms_pvalue ms_get(ms_state *state, ms_integer offset);
ms_pvalue ms_loadconst(ms_state *state, ms_integer offset);

const ms_pdata *ms_type(ms_state *state, ms_integer offset);
void ms_print(ms_state *state, ms_integer offset);
