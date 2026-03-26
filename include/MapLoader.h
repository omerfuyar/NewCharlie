#pragma once

#define MAP_MAX_WIDTH 128
#define MAP_MAX_HEIGHT 32
#define MAP_DATA_SIZE (MAP_MAX_WIDTH * MAP_MAX_HEIGHT)

typedef struct Map
{
    unsigned char width;
    unsigned char height;
    char data[MAP_DATA_SIZE];
} Map;

int LoadMap(const char *file, Map *retMap);
