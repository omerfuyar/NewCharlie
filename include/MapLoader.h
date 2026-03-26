#pragma once

#define MAP_MAX_WIDTH 64
#define MAP_MAX_HEIGHT 32
#define MAP_DATA_SIZE (MAP_MAX_WIDTH * MAP_MAX_HEIGHT)

typedef char Map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

int LoadMap(const char *file, Map retMap, int *playerX, int *playerY);
