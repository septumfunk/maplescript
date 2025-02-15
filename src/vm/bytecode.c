#include "bytecode.h"
#include "../data/strings.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char *ms_pbyte_string(ms_pvalue self) {
    return ms_format("%02X", self.value.byte);
}

char *ms_pnum_string(ms_pvalue self) {
    return ms_format("%f", self.value.number);
}

ms_pvalue ms_pnum_add(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_NUMBER,
        .value.number = obj1.value.number + obj2.value.number,
    };
}

ms_pvalue ms_pnum_sub(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_NUMBER,
        .value.number = obj1.value.number - obj2.value.number,
    };
}

char *ms_pint_string(ms_pvalue self) {
    return ms_format("%lld", self.value.integer);
}

ms_pvalue ms_pint_add(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_INTEGER,
        .value.integer = obj1.value.integer + obj2.value.integer,
    };
}

ms_pvalue ms_pint_sub(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_INTEGER,
        .value.integer = obj1.value.integer - obj2.value.integer,
    };
}

char *ms_pptr_string(ms_pvalue self) {
    return ms_format("%p", self.value.pointer);
}

ms_pvalue ms_pptr_add(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_POINTER,
        .value.pointer = (char *)obj1.value.pointer + obj2.value.integer,
    };
}

ms_pvalue ms_pptr_sub(ms_pvalue obj1, ms_pvalue obj2) {
    return (ms_pvalue) {
        .pt = MS_P_POINTER,
        .value.pointer = (char *)obj1.value.pointer - obj2.value.integer,
    };
}

const struct ms_pdata MS_PRIMITIVES[MS_P_COUNT] = {
    [MS_P_BYTE] = {
        .pt = MS_P_BYTE,
        .name = "byte",
        .size = sizeof(ms_byte),
        .op = {
            .string = ms_pbyte_string,
        },
    },
    [MS_P_NUMBER] = {
        .pt = MS_P_NUMBER,
        .name = "number",
        .size = sizeof(ms_number),
        .op = {
            .string = ms_pnum_string,
            .add = ms_pnum_add,
            .sub = ms_pnum_sub,
        }
    },
    [MS_P_INTEGER] = {
        .pt = MS_P_INTEGER,
        .name = "integer",
        .size = sizeof(ms_integer),
        .op = {
            .string = ms_pint_string,
            .add = ms_pint_add,
            .sub = ms_pint_sub,
        }
    },
    [MS_P_POINTER] = {
        .pt = MS_P_POINTER,
        .name = "pointer",
        .size = sizeof(ms_pointer),
        .op = {
            .string = ms_pptr_string,
            .add = ms_pptr_add,
            .sub = ms_pptr_sub,
        }
    },
};

ms_unit ms_unit_new() {
    return (ms_unit) {
        .bytecode = ms_vec_new(uint8_t),
        .constants = ms_vec_new(ms_pvalue),
        .dbg_chunks = ms_vec_new(ms_debug_chunk),
    };
}

void ms_unit_delete(ms_unit *self) {
    ms_vec_delete(&self->bytecode);
    ms_vec_delete(&self->constants);
    ms_vec_delete(&self->dbg_chunks);
}

void ms_unit_save(ms_unit *self, const char *path) {
    FILE *f = fopen(path, "w");
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file '%s' for writing", path);
        fclose(f);
        return;
    }

    uint64_t written = fwrite(&(ms_unit_info) {
        .header = MS_UNIT_HEADER,
        .version = MS_BYTECODE_VERSION,
        .loc_constants = /*sizeof(ms_unit_info)*/ 0,
        .loc_bytecode = sizeof(ms_unit_info),
    }, sizeof(uint8_t), sizeof(ms_unit_info), f);
    if (written != sizeof(ms_unit_info)) {
        fprintf(stderr, "Failed to write chunk info to file '%s'", path);
        fclose(f);
        return;
    }

    // TODO: constants

    if (fwrite(self->bytecode.data, self->bytecode.element_size, self->bytecode.count, f) != self->bytecode.element_size * self->bytecode.count) {
        fprintf(stderr, "Failed to write bytecode to file '%s'", path);
        fclose(f);
        return;
    }
}

/*void ms_unit_load_file(ms_unit *self, const char *path) {

}*/

void ms_unit_push_debug(ms_unit *self, uint64_t offset, uint64_t line_number) {
    ms_vec_push(&self->dbg_chunks, &(ms_debug_chunk) {
        .offset = offset,
        .line_number = line_number,
    });
}

uint64_t ms_unit_push_constant(ms_unit *self, enum ms_ptag pt, void *data) {
    ms_pvalue value = { .pt = pt };
    memcpy(&value.value, data, ms_pdata_get(pt)->size);

    ms_vec_push(&self->constants, &value);
    uint64_t offset = self->constants.count - 1;
    return offset;
}

ms_pvalue ms_unit_get_constant(ms_unit *self, uint64_t offset) {
    return ms_vec_get(&self->constants, ms_pvalue, offset);
}
