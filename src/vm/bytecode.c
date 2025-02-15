#include "bytecode.h"
#include "../data/strings.h"
#include "../data/fs.h"
#include "../memory/alloc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ms_pbyte_string(ms_pvalue self) {
    return ms_format("%02X", self.value.byte);
}

ms_pvalue ms_pbyte_conv(ms_pvalue self) {
    switch (self.pt) {
        case MS_P_NUMBER:
            self.value.byte = (ms_byte)self.value.number;
            break;
        case MS_P_INTEGER:
            self.value.byte = (ms_byte)self.value.integer;
            break;
        case MS_P_POINTER:
            self.value.byte = (ms_byte)(ms_integer)self.value.pointer;
        default: break;
    }
    self.pt = MS_P_BYTE;

    return self;
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

ms_pvalue ms_pnum_conv(ms_pvalue self) {
    switch (self.pt) {
        case MS_P_BYTE:
            self.value.number = self.value.byte;
            break;
        case MS_P_INTEGER:
            self.value.number = (ms_number)self.value.integer;
            break;
        case MS_P_POINTER:
            self.value.number = (ms_number)(ms_integer)self.value.pointer;
        default: break;
    }
    self.pt = MS_P_NUMBER;

    return self;
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

ms_pvalue ms_pint_conv(ms_pvalue self) {
    switch (self.pt) {
        case MS_P_BYTE:
            self.value.integer = self.value.byte;
            break;
        case MS_P_NUMBER:
            self.value.integer = (ms_integer)self.value.number;
            break;
        case MS_P_POINTER:
            self.value.integer = (ms_integer)self.value.pointer;
        default: break;
    }
    self.pt = MS_P_INTEGER;

    return self;
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

ms_pvalue ms_pptr_conv(ms_pvalue self) {
    switch (self.pt) {
        case MS_P_BYTE:
            self.value.pointer = (ms_pointer)(ms_integer)self.value.byte;
            break;
        case MS_P_NUMBER:
            self.value.pointer = (ms_pointer)(ms_integer)self.value.number;
            break;
        case MS_P_INTEGER:
            self.value.pointer = (ms_pointer)self.value.integer;
        default: break;
    }
    self.pt = MS_P_POINTER;

    return self;
}

const struct ms_pdata MS_PRIMITIVES[MS_P_COUNT] = {
    [MS_P_BYTE] = {
        .pt = MS_P_BYTE,
        .name = "byte",
        .size = sizeof(ms_byte),
        .op = {
            .string = ms_pbyte_string,

            .convert = {
                [MS_P_NUMBER] = ms_pnum_conv,
                [MS_P_POINTER] = ms_pptr_conv,
                [MS_P_INTEGER] = ms_pint_conv,
            },
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

            .convert = {
                [MS_P_BYTE] = ms_pbyte_conv,
                [MS_P_INTEGER] = ms_pint_conv,
                [MS_P_POINTER] = ms_pptr_conv,
            },
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

            .convert = {
                [MS_P_BYTE] = ms_pbyte_conv,
                [MS_P_NUMBER] = ms_pnum_conv,
                [MS_P_POINTER] = ms_pptr_conv,
            },
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

            .convert = {
                [MS_P_BYTE] = ms_pbyte_conv,
                [MS_P_NUMBER] = ms_pnum_conv,
                [MS_P_INTEGER] = ms_pint_conv,
            },
        }
    },
};

ms_unit ms_unit_new() {
    return (ms_unit) {
        .bytecode = ms_vec_new(uint8_t),
        .constants = ms_vec_new(ms_pvalue),
        .dbg_chunks = ms_vec_new(ms_debug_chunk),
        .entry = 0,
    };
}

void ms_unit_delete(ms_unit *self) {
    ms_vec_delete(&self->bytecode);
    ms_vec_delete(&self->constants);
    ms_vec_delete(&self->dbg_chunks);
}

void ms_unit_save(ms_unit *self, const char *path) {
    FILE *f = fopen(path, "wb");
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file '%s' for writing", path);
        return;
    }

    size_t constants_size = 0;
    for (uint64_t i = 0; i < self->constants.count; ++i) {
        ms_pvalue val = ms_unit_get_constant(self, i);
        constants_size +=  sizeof(val.pt) + ms_pdata_get(val.pt)->size;
    }

    size_t bytecode_size = self->bytecode.count;
    size_t buffer_size = sizeof(ms_unit_info) + constants_size + bytecode_size;
    uint8_t *buffer = ms_calloc(1, buffer_size);
    ms_unit_info header = {
        .header = MS_UNIT_HEADER,
        .version = MS_BYTECODE_VERSION,
        .loc_constants = sizeof(ms_unit_info), // After Header
        .loc_bytecode = sizeof(ms_unit_info) + constants_size,
        .entry = self->entry,
    };
    memcpy(buffer, &header, sizeof(ms_unit_info));

    uint8_t *write = buffer + sizeof(ms_unit_info);
    for (uint64_t i = 0; i < self->constants.count; ++i) {
        ms_pvalue val = ms_unit_get_constant(self, i);
        memcpy(write, &val.pt, sizeof(val.pt));
        write += sizeof(val.pt);
        memcpy(write, &val.value, ms_pdata_get(val.pt)->size);
        write += ms_pdata_get(val.pt)->size;
    }

    memmove(buffer + header.loc_bytecode, self->bytecode.data, bytecode_size);

    if (fwrite(buffer, sizeof(uint8_t), buffer_size, f) != buffer_size)
        fprintf(stderr, "Failed to save bytecode unit to file '%s'", path);

    free(buffer);
    fclose(f);
}

ms_status ms_unit_load_file(ms_unit *self, const char *path) {
    *self = ms_unit_new();

    size_t filesize = 0;
    uint8_t *file = ms_file(path, &filesize);
    if (!file)
        return MS_STATUS_RUNTIME_ERROR;

    ms_unit_info header = *(ms_unit_info *)file;

    uint8_t *constants = file + header.loc_constants;
    size_t constant_size = header.loc_bytecode - header.loc_constants;
    uint8_t *bytecode = file + header.loc_bytecode;
    size_t bytecode_size = filesize - constant_size - sizeof(header);

    if ((constant_size > UINT64_MAX - bytecode_size) || (sizeof(header) + constant_size + bytecode_size != filesize)) {
        fprintf(stderr, "Invalid maple unit file.\n");
        free(file);
        return MS_STATUS_RUNTIME_ERROR;
    }

    while (constants < bytecode) {
        enum ms_ptag pt = *constants++;
        ms_unit_push_constant(self, pt, constants);
        constants += ms_pdata_get(pt)->size;
    }
    ms_vec_append(&self->bytecode, bytecode, bytecode_size);
    free(file);

    self->entry = header.entry;

    return MS_STATUS_OK;
}

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
