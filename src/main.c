#include "structures/vec.h"
#include "vm/chunk.h"
#include "vm/opcodes.h"
#include "vm/types.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    uint64_t offset = 0;
    ms_chunk chunk = ms_chunk_new();

    ms_chunk_push_debug(&chunk, offset, 1);
    offset = ms_chunk_push_instruction(&chunk, MS_OP_CONSTANT, &(ms_pointer){0});
    offset = ms_chunk_push_instruction(&chunk, MS_OP_CONSTANT, &(ms_pointer){1});
    ms_chunk_push_debug(&chunk, offset, 2);
    offset = ms_chunk_push_instruction(&chunk, MS_OP_ADD, &(ms_pointer){0x1779381649}, &(ms_pointer){0x1121827158});
    offset = ms_chunk_push_instruction(&chunk, MS_OP_RETURN);

    char *dasm = ms_chunk_disassemble_all(&chunk);
    printf("%s\n", dasm);
    free(dasm);

    ms_chunk_save(&chunk, "example.msc");

    return 0;
}
