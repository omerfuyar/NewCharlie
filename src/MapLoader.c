#include "MapLoader.h"
#include <stdio.h>
#include <string.h>

int LoadMap(const char *file, Map *retMap)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load map file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_MAX_WIDTH + 8] = {0};
    char lineCount = 0;
    if (fgets(lineBuffer, MAP_MAX_WIDTH + 8, mapFile) != NULL)
    {
        if (sscanf(lineBuffer, "%d,%d", &retMap->playerStartX, &retMap->playerStartY) != 2)
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

    while (fgets(lineBuffer, MAP_MAX_WIDTH + 8, mapFile) != NULL)
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        // if (lineBuffer[0] == '\0')
        //{
        //     continue;
        // }

        lineCount++;

        if (lineCount > MAP_MAX_HEIGHT)
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

        memcpy(retMap->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);
    return 0;

error:
    fclose(mapFile);
    return 1;
}
