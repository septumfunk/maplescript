#include "arg.h"
#include "assembler.h"
#include "../data/strings.h"
#include "../vm/instruction.h"
#include <stdio.h>

int ms_args_parse(int argc, char **argv) {
    const ms_argpath *ap = &MS_ARGROOT;
    argc--;
    argv++;

    int c = argc;
    for (int i = 0; i < c; ++i) {
        for (uint64_t i = 0; i < ap->subpaths.count; ++i) {
            if (ms_strcmp(*argv, ap->subpaths.array[i].name)) {
                ap = &ap->subpaths.array[i];
                goto con;
            }
        }
        break;
    con:
        argc--;
        argv++;
        continue;
    }
    return ap->callback(argc, argv);
}

const char *maple_help =
"Usage: maple <subpath>\n"
"Subpaths:\n"
"   - asm\n"
"   - run\n"
;

int ms_arg_maple_help(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("%s", maple_help);
    return 0;
}

int ms_arg_maple_run(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: maple run <file>\n");
        return 1;
    }

    ms_state *state = ms_state_new();
    ms_unit unit;
    if (ms_unit_load_file(&unit, *argv) != MS_STATUS_OK) {
        fprintf(stderr, "Failed to load unit file '%s'.", *argv);
        goto error;
    }

    if (ms_dounit(state, &unit) != MS_STATUS_OK)
        goto error;

    ms_unit_delete(&unit);
    ms_state_delete(state);
    return 0;
error:
    ms_unit_delete(&unit);
    ms_state_delete(state);
    return 1;
}

const ms_argpath MS_ARGROOT = {
    .name = "maplescript",
    .callback = ms_arg_maple_help,
    .subpaths = {
        .count = 2,
        .array = (const ms_argpath[]) {
            {
                .name = "run",
                .callback = ms_arg_maple_run,
                .subpaths.count = 0,
            },
            {
                .name = "asm",
                .callback = ms_arg_asm_help,
                .subpaths = {
                    .count = 2,
                    .array = (const ms_argpath[]) {
                        {
                            .name = "build",
                            .callback = ms_arg_asm_build,
                            .subpaths.count = 0,
                        },
                        {
                            .name = "run",
                            .callback = ms_arg_asm_run,
                            .subpaths.count = 0,
                        }
                    },
                },
            },
        },
    }
};
