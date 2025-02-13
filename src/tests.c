#include "tests.h"
#include "structures/map.h"
#include "structures/types.h"
#include "vm/unit.h"
#include "vm/vm.h"
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
    uint64_t offset;
} ms_shared_test_data;

//* Tests

int ms_test_unit(ms_shared_test_data *test_data) {
    test_data->unit = ms_unit_new();
    test_data->offset = 0;

    // Manual instructions
    ms_unit_push_debug(&test_data->unit, test_data->offset, 1);
    test_data->offset = ms_unit_push_instruction(&test_data->unit, MS_OP_CONSTANT, &(ms_pointer){0});
    test_data->offset = ms_unit_push_instruction(&test_data->unit, MS_OP_CONSTANT, &(ms_pointer){1});
    ms_unit_push_debug(&test_data->unit, test_data->offset, 2);
    test_data->offset = ms_unit_push_instruction(&test_data->unit, MS_OP_RETURN);

    return EXIT_SUCCESS;
}

int ms_test_disassemble(ms_shared_test_data *test_data) {
    char *dasm = ms_unit_disassemble_all(&test_data->unit);
    printf("%s\n", dasm);
    free(dasm);

    return EXIT_SUCCESS;
}

int ms_test_execute_unit(ms_shared_test_data *test_data) {
    ms_vm vm = ms_vm_new();
    return ms_vm_dounit(&vm, &test_data->unit) == MS_RESULT_RUNTIME_ERROR;
}

int ms_test_map(ms_shared_test_data *test_data) {
    (void)test_data;

    ms_map map = ms_map_new();
    ms_map_insert(&map, "test", &(float){3.14159f});
    printf("%f", (double)ms_map_get(&map, float, "test"));
    ms_map_delete(&map);

    return EXIT_SUCCESS;
}

//! Execution

int ms_run_tests(int argc, char **argv) {
    // Tests to be run
    static const ms_test tests_to_run[] = {
        ms_test_func(ms_test_unit),
        ms_test_func(ms_test_disassemble),
        ms_test_func(ms_test_execute_unit),
        ms_test_func(ms_test_map),
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
    }
    printf("[TEST] All tests completed. %llu/%llu tests failed.\n", test_data.tests_failed, test_data.test_count);

    return test_data.tests_failed > 0;
}
