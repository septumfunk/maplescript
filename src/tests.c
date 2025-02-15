#include "tests.h"
#include "vm/bytecode.h"
#include "vm/instruction.h"
#include "vm/state.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct ms_shared_test_data;
typedef struct {
    const char *test_name;
    int (*function)(struct ms_shared_test_data *test_data);
} ms_test;

#define ms_test_func(func) (ms_test) { .test_name = #func, .function = func, }

typedef struct ms_shared_test_data {
    uint64_t test_count;
    const ms_test *tests_to_run;
    uint64_t tests_failed;

    // Extra
    int argc;
    char **argv;
    ms_unit unit;
    ms_state *state;
} ms_shared_test_data;

//* Tests

int ms_test_unit(ms_shared_test_data *test_data) {
    test_data->state = ms_state_new();
    test_data->unit = ms_unit_new();

    auto n1 = ms_unit_push_constant(&test_data->unit, MS_P_NUMBER, &(ms_number) { 4.20 });
    auto n2 = ms_unit_push_constant(&test_data->unit, MS_P_INTEGER, &(ms_integer) { 69 });

    ms_ins_push(&test_data->unit, MS_OP_LDCT, &n1);
    ms_ins_push(&test_data->unit, MS_OP_LDCT, &n2);
    ms_ins_push(&test_data->unit, MS_OP_ADD);
    ms_ins_push(&test_data->unit, MS_OP_PRNT);
    ms_ins_push(&test_data->unit, MS_OP_RET);

    return EXIT_SUCCESS;
}

int ms_test_disassemble(ms_shared_test_data *test_data) {
    char *dasm = ms_ins_disassemble_all(&test_data->unit);
    printf("%s\n", dasm);
    free(dasm);

    return EXIT_SUCCESS;
}

int ms_test_run(ms_shared_test_data *test_data) {
    return ms_dounit(test_data->state, &test_data->unit);
}

//! Execution

int ms_run_tests(int argc, char **argv) {
    // Tests to be run
    static const ms_test tests_to_run[] = {
        ms_test_func(ms_test_unit),
        ms_test_func(ms_test_disassemble),
        ms_test_func(ms_test_run),
    };

    // Shared data
    ms_shared_test_data test_data = {
        .test_count = sizeof(tests_to_run) / sizeof(ms_test),
        .tests_to_run = tests_to_run,
        .tests_failed = 0,

        .argc = argc,
        .argv = argv,
    };

    // Run Tests
    for (uint64_t i = 0; i < test_data.test_count; ++i) {
        const ms_test *t = &tests_to_run[i];
        printf("[TEST] Executing test '%s'.\n", t->test_name);
        int status = t->function(&test_data);
        fprintf(status ? stderr : stdout, "[TEST] Test '%s' exited with status %d.\n", t->test_name, status);
        printf("==========================================================\n");
        test_data.tests_failed += (status ? 1 : 0);
    }
    printf("[TEST] All tests completed. %llu/%llu tests failed.\n", test_data.tests_failed, test_data.test_count);

    return test_data.tests_failed > 0;
}
