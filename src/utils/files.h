#pragma once

#include <stdio.h>
#include <stdlib.h>

inline char* file2buf(const char* filePath) {
    FILE *file = fopen(filePath, "rb");
    if (!file) {
        printf("Failed to open file: %s\n", filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *output = (char*)malloc(length + 1);
    if (!output) {
        printf("Failed to allocate memory for shader source\n");
        fclose(file);
        return NULL;
    }

    size_t readLength = fread(output, 1, length, file);
    if (readLength != length) {
        printf("Error reading file: %s : %zi, %i\n", filePath, readLength, length);
        free(output);
        fclose(file);
        return NULL;
    }

    output[length] = '\0';
    fclose(file);
    return output;
}