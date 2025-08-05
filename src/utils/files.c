#include "utils/files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* file2buf(const char* filePath) {
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
        printf("Error reading file: %s : %zi, %li\n", filePath, readLength, length);
        free(output);
        fclose(file);
        return NULL;
    }

    output[length] = '\0';
    fclose(file);
    return output;
}

#define MAX_LINE_LENGTH 32
#define MAX_ROWS 100
#define MAX_COLS 100

void print_tilemap(Map map) {
    for (int i = 0; i < map.height; ++i) {
        for (int j = 0; j < map.width; ++j) {
            printf("%u ", map.data[i][j]);
        }
        printf("\n");
    }
}

Map parseMapFile(const char *filename) {
    Map map;
    map.data = (unsigned int**)malloc(MAX_ROWS * sizeof(unsigned int*));
    for (size_t i = 0; i < MAX_ROWS; i++)
    {
        map.data[i] = (unsigned int*)malloc(MAX_COLS * sizeof(unsigned int));
        for (size_t j = 0; j < MAX_COLS; j++) {
            map.data[i][j] = 0; // Initialize to zero
        }
    }
    
    map.height = 0;
    map.width = 0;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        //return map;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && map.height < MAX_ROWS) {
        // Remove newline
        line[strcspn(line, "\r\n")] = 0;

        unsigned int col = 0;
        char *token = strtok(line, ",");
        while (token && col < MAX_COLS) {
            map.data[map.height][col++] = atoi(token);
            token = strtok(NULL, ",");
        }

        if (map.width == 0) {
            map.width = col; // Set column count from first row
        } else if (col != map.width) {
            //fprintf(stderr, "Inconsistent column count at row %u %u %u\n", map.height, col, map.width);
            //fclose(file);
        }

        map.height++;
    }

    fclose(file);
    print_tilemap(map);
    return map;
}


void freeMapFile(Map mapFile) {
    if (mapFile.data) {
        // Free each row
        for (unsigned int i = 0; i < mapFile.height; i++) {
            if (mapFile.data[i]) {
                free(mapFile.data[i]);
            }
        }
        // Free the array of row pointers
        free(mapFile.data);
    }
}

