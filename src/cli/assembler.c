#include "assembler.h"
#include "../vm/asm.h"
#include "../data/fs.h"
#include "../data/strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *asm_help =
"Usage: maple asm <subpath>\n"
"Subpaths:\n"
"   - build\n"
"   - run\n"
;

int ms_arg_asm_help(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("%s", asm_help);
    return 0;
}

int _gen_unit(const char *path, ms_unit *unit) {
    ms_assembler assembler = ms_assembler_new();
    *unit = ms_unit_new();
    char *asm_file = ms_file_string(path);

    if (!asm_file) {
        fprintf(stderr, "Unable to load file '%s'.\n", path);
        goto error;
    }

    printf("Assembling file '%s'.\n", path);
    if (ms_assemble(&assembler, unit, asm_file) != MS_STATUS_OK) {
        fprintf(stderr, "Failed to assemble file '%s'.\n", path);
        goto error;
    }
    free(asm_file);

    ms_assembler_delete(&assembler);
    return 0;
error:
    free(asm_file);
    ms_unit_delete(unit);
    ms_assembler_delete(&assembler);
    return 1;
}


int ms_arg_asm_build(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: maple asm build <file>\n");
        return 1;
    }

    ms_state *state = ms_state_new();
    ms_unit unit;
    _gen_unit(*argv, &unit);

    char *dup = ms_strdup(*argv);
    char *true_name = strtok(dup, ".");
    char *new_name = ms_format("%s.maple", true_name);
    ms_unit_save(&unit, new_name);
    free(new_name);
    free(dup);

    ms_unit_delete(&unit);
    ms_state_delete(state);
    return 0;
}

int ms_arg_asm_run(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: maple asm run <file>\n");
        return 1;
    }

    ms_state *state = ms_state_new();
    ms_unit unit;
    int ret = 0;
    ret = _gen_unit(*argv, &unit);
    if (ret) return ret;

    if (ms_dounit(state, &unit) != MS_STATUS_OK)
        ret = 1;

    ms_unit_delete(&unit);
    ms_state_delete(state);
    return ret;
}
