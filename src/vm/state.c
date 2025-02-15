#include "state.h"
#include "bytecode.h"
#include "../memory/alloc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#pragma clang diagnostic ignored "-Wformat-nonliteral"

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
    if (state->err)
        free(state->err);
}

ms_byte *ms_push(ms_state *state, ms_pvalue value) {
    return ms_stack_push(&state->stack, &value);
}

void ms_pop(ms_state *state, ms_integer count) {
    ms_stack_pop(&state->stack, (uint64_t)count, ms_pvalue);
}

ms_pvalue ms_get(ms_state *state, ms_integer reg) {
    return *ms_stack_get(&state->stack, reg, ms_pvalue);
}

void ms_set(ms_state *state, ms_integer reg, ms_pvalue value) {
    ms_stack_set(&state->stack, reg, &value);
}

ms_pvalue ms_loadconst(ms_state *state, ms_integer index) {
    return ms_unit_get_constant(state->unit, (uint64_t)index);
}

const ms_pdata *ms_type(ms_state *state, ms_integer reg) {
    return ms_pdata_get(ms_get(state, reg).pt);
}

void ms_print(ms_state *state, ms_integer reg) {
    ms_pvalue val = ms_get(state, reg);
    char *str = ms_pdata_get(val.pt)->op.string(val);
    printf("PRNT %s\n", str);
    free(str);
}

void ms_error(ms_state *state, const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *fmt = ms_calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(fmt, size + 1, format, arglist);
    va_end(arglist);

    if (state->err)
        free(state->err);
    state->err = fmt;
}
