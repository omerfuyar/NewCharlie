#include "MapLoader.h"
#include "stdio.h"
#include "string.h"

int LoadMap(const char *file, Map *retMap)
{
    FILE *mapFile = fopen(file, "rb");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load map file '%s'.\n", file);
        return 1;
    }

    retMap->width = 0;
    retMap->height = 0;

    char lineBuffer[MAP_MAX_WIDTH] = {0};

    while (fgets(lineBuffer, MAP_MAX_WIDTH, mapFile) != NULL)
    {
        retMap->height++;

        if (retMap->height > MAP_MAX_HEIGHT)
        {
            fprintf(stderr, "Error: Map file has too many rows.\n");
            goto error;
        }

        size_t lineLength = strlen(lineBuffer);
        if (lineLength > MAP_MAX_WIDTH)
        {
            fprintf(stderr, "Error: Line in map file has too many columns.\n");
            goto error;
        }

        if ((unsigned char)lineLength > retMap->width)
        {
            retMap->width = (unsigned char)lineLength;
        }
    }

    fclose(mapFile);
    return 0;

error:
    fclose(mapFile);
    return 1;
}
