#include "MapLoader.h"
#include <stdio.h>
#include <string.h>

int LoadMap(const char *file, Map retMap, int *playerX, int *playerY)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load map file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_MAX_WIDTH + 1] = {0};
    char lineCount = 0;
    if (fgets(lineBuffer, MAP_MAX_WIDTH + 1, mapFile) != NULL)
    {
        if (sscanf(lineBuffer, "%d,%d", playerX, playerY) != 2)
        {
            fprintf(stderr, "Error: Failed to parse player position in map file '%s'.\n", file);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: Failed to read map file '%s'.\n", file);
        return 1;
    }

    while (fgets(lineBuffer, MAP_MAX_WIDTH + 1, mapFile) != NULL)
    {
        if (lineBuffer[0] == '\n')
        {
            continue;
        }

        lineCount++;

        if (lineCount > MAP_MAX_HEIGHT)
        {
            fprintf(stderr, "Error: Map file has too many rows.\n");
            goto error;
        }

        size_t lineLength = strlen(lineBuffer) - 1;
        if (lineLength > MAP_MAX_WIDTH)
        {
            fprintf(stderr, "Error: Line in map file has too many columns.\n");
            goto error;
        }

        memcpy(retMap[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
        // printf("line %d: '%.*s'\n", lineCount, (int)lineLength, retMap[lineCount - 1]);
    }

    fclose(mapFile);
    return 0;

error:
    fclose(mapFile);
    return 1;
}
