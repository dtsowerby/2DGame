#pragma once

char* file2buf(const char* filePath);

typedef struct MapFile
{
    unsigned int height;
    unsigned int width;
    unsigned int** data;
} Map;

Map parseMapFile(const char *filename);