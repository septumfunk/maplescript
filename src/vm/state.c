#include "state.h"
#include "bytecode.h"
#include "../memory/alloc.h"
#include <stdio.h>
#include <stdlib.h>

ms_state *ms_state_new() {
    ms_state *state = ms_calloc(1, sizeof(ms_state));
    *state = (ms_state) {
        .unit = nullptr, // Empty by default
        .ip = nullptr,

        .stack = ms_stack_new(MS_STACK_MAX),
        .usertypes = ms_vec_new(void *),
    };

    return state;
}

void ms_state_delete(ms_state *state) {
    if (state->unit)
        ms_unit_delete(state->unit);
    ms_stack_delete(&state->stack);
    ms_vec_delete(&state->usertypes);
}

ms_byte *ms_push(ms_state *state, ms_pvalue value) {
    return ms_stack_push(&state->stack, &value);
}

void ms_pop(ms_state *state, ms_integer count) {
    ms_stack_pop(&state->stack, (uint64_t)count, ms_pvalue);
}

ms_pvalue ms_get(ms_state *state, ms_integer offset) {
    return *ms_stack_get(&state->stack, offset, ms_pvalue);
}

ms_pvalue ms_loadconst(ms_state *state, ms_integer offset) {
    return ms_unit_get_constant(state->unit, (uint64_t)offset);
}

const ms_pdata *ms_type(ms_state *state, ms_integer offset) {
    return ms_pdata_get(ms_get(state, offset).pt);
}

void ms_print(ms_state *state, ms_integer offset) {
    ms_pvalue val = ms_get(state, offset);
    char *str = ms_pdata_get(val.pt)->op.string(val);
    printf("PRNT %s\n", str);
    free(str);
}
