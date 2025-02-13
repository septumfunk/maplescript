#include "types.h"
#include "strings.h"
#include <stdlib.h>

char *ms_number_tostring(void *);
char *ms_pointer_tostring(void *);

const ms_type MS_PRIMITIVES[MS_PRIMITIVE_COUNT] = {
    [MS_PRIMITIVE_NUMBER] = {
        .name = "number",
        .size = sizeof(ms_number),
        .tostring = ms_number_tostring,
    },
    [MS_PRIMITIVE_POINTER] = {
        .name = "pointer",
        .size = sizeof(ms_pointer),
        .tostring = ms_pointer_tostring,
    }
};

char *ms_number_tostring(void *number) {
    return ms_format("%f", (double)*(float *)number);
}

char *ms_pointer_tostring(void *pointer) {
    char *ptr = ms_format("0x");
    for (uint64_t i = 0; i < sizeof(ms_pointer); ++i) {
        char *hex = ms_format("%02X", ((uint8_t *)pointer)[i]);
        ptr = ms_strcat(ptr, hex);
        free(hex);
    }

    return ptr;
}
