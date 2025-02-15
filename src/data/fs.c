#include "fs.h"
#include "../memory/alloc.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t *ms_file(const char *path, size_t *size) {
    if (!size) return nullptr; // Prevent null pointer dereference

    FILE *file = fopen(path, "rb");
    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    if (filesize < 0) { // Check for ftell failure
        fclose(file);
        return nullptr;
    }

    rewind(file);

    uint8_t *buffer = ms_malloc((size_t)filesize);
    size_t bytesRead = fread(buffer, 1, (size_t)filesize, file);
    fclose(file);

    if (bytesRead != (size_t)filesize) {
        free(buffer);
        return nullptr;
    }

    *size = (size_t)filesize;
    return buffer;
}


char *ms_file_string(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);
    char *buffer = ms_malloc((size_t)filesize + 1);
    fread(buffer, 1, (size_t)filesize, file);
    buffer[filesize] = '\0';
    fclose(file);

    return buffer;
}
