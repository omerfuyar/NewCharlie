#include "ResourceLoader.h"
#include "charBehaviours.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static int parseMapHeader(FILE *mapFile, char *lineBuffer, int lineBufferSize, Map *retMap)
{
    // id
    if (fgets(lineBuffer, lineBufferSize, mapFile) == NULL)
    {
        fprintf(stderr, "Error: Map file is empty.\n");
        return 1;
    }

    if (lineBuffer[0] != 'i')
    {
        fprintf(stderr, "Error: Map file has invalid format. First (ID) line must start with 'i'.\n");
        return 1;
    }

    if (sscanf(lineBuffer + 2, "%d", &retMap->index) != 1)
    {
        fprintf(stderr, "Error: Map file has invalid format. Failed to read map index from first line.\n");
        return 1;
    }

    // portal
    if (fgets(lineBuffer, lineBufferSize, mapFile) == NULL)
    {
        fprintf(stderr, "Error: Map file does not contain a valid portal definition.\n");
        return 1;
    }

    if (lineBuffer[0] != 'p')
    {
        fprintf(stderr, "Error: Map file has invalid format. Second (Portal) line must start with 'p'.\n");
        return 1;
    }

    if (sscanf(lineBuffer + 2, "%d,%d,%d,%d", &retMap->portals[0].targetMapIndex, &retMap->portals[1].targetMapIndex, &retMap->portals[2].targetMapIndex, &retMap->portals[3].targetMapIndex) != 4)
    {
        fprintf(stderr, "Error: Map file has invalid format. Failed to read portal index from second line.\n");
        return 1;
    }

    // interactable
    if (fgets(lineBuffer, lineBufferSize, mapFile) == NULL)
    {
        fprintf(stderr, "Error: Map file does not contain a valid interactable definition.\n");
        return 1;
    }

    if (lineBuffer[0] != 'n')
    {
        fprintf(stderr, "Error: Map file has invalid format. Third (NPC) line must start with 'n'.\n");
        return 1;
    }

    if (sscanf(lineBuffer + 2, "%d,%d,%d,%d", &retMap->npcs[0].index, &retMap->npcs[1].index, &retMap->npcs[2].index, &retMap->npcs[3].index) != 4)
    {
        fprintf(stderr, "Error: Map file has invalid format. Failed to read interactable index from third line.\n");
        return 1;
    }

    return 0;
}

static int loadMap(const char *file, Map *retMap)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load map file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_MAX_WIDTH + 8] = {0};
    int lineCount = 0;
    char playerFound = 0;
    char portalsFound = 0;

    lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0'; // id line
    if (parseMapHeader(mapFile, lineBuffer, MAP_MAX_WIDTH + 8, retMap) != 0)
    {
        fprintf(stderr, "Error: Failed to parse header of map file '%s'.\n", file);
        fclose(mapFile);
        return 1;
    }

    while (fgets(lineBuffer, MAP_MAX_WIDTH + 8, mapFile) != NULL)
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        lineCount++;

        if (lineCount > MAP_MAX_HEIGHT)
        {
            fprintf(stderr, "Error: Map file '%s' has too many rows.\n", file);
            fclose(mapFile);
            return 1;
        }

        size_t lineLength = strlen(lineBuffer);
        if (lineLength > MAP_MAX_WIDTH)
        {
            fprintf(stderr, "Error: Line in map file '%s' has too many columns.\n", file);
            fclose(mapFile);
            return 1;
        }

        for (size_t i = 0; i < lineLength; i++)
        {
            int posX = (int)i;
            int posY = lineCount - 1;

            switch (lineBuffer[i])
            {
            case CHAR_PLAYER:
                if (playerFound)
                {
                    fprintf(stderr, "Error: Multiple player start positions found in map file '%s'.\n", file);
                    fclose(mapFile);
                    return 1;
                }

                retMap->playerStartX = posX;
                retMap->playerStartY = posY;
                playerFound = 1;
                lineBuffer[i] = ' ';
                break;
            case CHAR_PORTAL:
                if (portalsFound >= MAP_INTERACTABLE_MAX_COUNT)
                {
                    fprintf(stderr, "Error: Too many portals found in map file '%s'. Maximum is %d.\n", file, MAP_INTERACTABLE_MAX_COUNT);
                    fclose(mapFile);
                    return 1;
                }

                retMap->portals[(int)portalsFound].x = posX;
                retMap->portals[(int)portalsFound].y = posY;
                portalsFound++;
                break;
            }
        }

        memcpy(retMap->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);

    if (!playerFound)
    {
        fprintf(stderr, "Error: No player start position found in map file '%s'.\n", file);
        return 1;
    }

    return 0;
}

static int loadPortrait(const char *file, Portrait *retPortals)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load portrait file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_MAX_WIDTH + 8] = {0};
    int lineCount = 0;

    while (fgets(lineBuffer, MAP_MAX_WIDTH + 8, mapFile) != NULL)
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        lineCount++;

        if (lineCount > MAP_MAX_HEIGHT)
        {
            fprintf(stderr, "Error: Portrait file '%s' has too many rows.\n", file);
            fclose(mapFile);
            return 1;
        }

        size_t lineLength = strlen(lineBuffer);
        if (lineLength > MAP_MAX_WIDTH)
        {
            fprintf(stderr, "Error: Line in portrait file '%s' has too many columns.\n", file);
            fclose(mapFile);
            return 1;
        }

        memcpy(retPortals->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);

    return 0;
}

int loadMaps(const char *directory, Map *retMaps, int maxMaps)
{
    int loadedMaps = 0;
    char pattern[STRING_MAX_SIZE] = {0};

#ifdef _WIN32
    WIN32_FIND_DATAA ffd = {0};
    snprintf(pattern, STRING_MAX_SIZE, "%s\\*.map", directory);

    HANDLE hFind = FindFirstFileA(pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error: Failed to open directory '%s' for maps. Put .map files in the directory.\n", directory);
        return 0;
    }

    do
    {
        if (loadedMaps + 1 > maxMaps)
        {
            fprintf(stderr, "Error: Too many .map files in directory '%s'. Maximum given is %d.\n", directory, maxMaps);
            FindClose(hFind);
            return 0;
        }

        snprintf(pattern, STRING_MAX_SIZE, "%s\\%s", directory, ffd.cFileName);

        if (loadMap(pattern, &retMaps[loadedMaps]) != 0)
        {
            fprintf(stderr, "Error: Failed to load map file '%s' from folder '%s'.\n", ffd.cFileName, directory);
            FindClose(hFind);
            return 0;
        }

        loadedMaps++;
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
#else
    DIR *dir = opendir(directory);

    if (!dir)
    {
        fprintf(stderr, "Error: Failed to open directory '%s' for maps. Put .map files in the directory.\n", directory);
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        size_t entryLen = strlen(entry->d_name);
        if (entryLen < 4 || strncmp(entry->d_name + entryLen - 4, ".map", 4) != 0)
        {
            continue;
        }

        if (loadedMaps + 1 > maxMaps)
        {
            fprintf(stderr, "Error: Too many .map files in directory '%s'. Maximum given is %d.\n", directory, maxMaps);
            closedir(dir);
            return 0;
        }

        snprintf(pattern, STRING_MAX_SIZE, "%s/%s", directory, entry->d_name);

        if (loadMap(pattern, &retMaps[loadedMaps]) != 0)
        {
            fprintf(stderr, "Error: Failed to load map file '%s' from folder '%s'.\n", entry->d_name, directory);
            closedir(dir);
            return 0;
        }

        loadedMaps++;
    }

    closedir(dir);
#endif

    return loadedMaps;
}

int loadPortraits(const char *directory, Portrait *retPortals, int maxPortraits)
{
    int loadedPortraits = 0;
    char pattern[STRING_MAX_SIZE] = {0};

#ifdef _WIN32
    WIN32_FIND_DATAA ffd = {0};
    snprintf(pattern, STRING_MAX_SIZE, "%s\\*.por", directory);

    HANDLE hFind = FindFirstFileA(pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error: Failed to open directory '%s' for portraits. Put .por files in the directory.\n", directory);
        return 0;
    }

    do
    {
        if (loadedPortraits + 1 > maxPortraits)
        {
            fprintf(stderr, "Error: Too many .por files in directory '%s'. Maximum given is %d.\n", directory, maxPortraits);
            FindClose(hFind);
            return 0;
        }

        snprintf(pattern, STRING_MAX_SIZE, "%s\\%s", directory, ffd.cFileName);

        if (loadPortrait(pattern, &retPortals[loadedPortraits]) != 0)
        {
            fprintf(stderr, "Error: Failed to load portrait file '%s' from folder '%s'.\n", ffd.cFileName, directory);
            FindClose(hFind);
            return 0;
        }

        loadedPortraits++;
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
#else
    DIR *dir = opendir(directory);

    if (!dir)
    {
        fprintf(stderr, "Error: Failed to open directory '%s' for portraits. Put .por files in the directory.\n", directory);
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        size_t entryLen = strlen(entry->d_name);
        if (entryLen < 4 || strncmp(entry->d_name + entryLen - 4, ".por", 4) != 0)
        {
            continue;
        }

        if (loadedPortraits + 1 > maxPortraits)
        {
            fprintf(stderr, "Error: Too many .por files in directory '%s'. Maximum given is %d.\n", directory, maxPortraits);
            closedir(dir);
            return 0;
        }

        snprintf(pattern, STRING_MAX_SIZE, "%s/%s", directory, entry->d_name);

        if (loadPortrait(pattern, &retPortals[loadedPortraits]) != 0)
        {
            fprintf(stderr, "Error: Failed to load portrait file '%s' from folder '%s'.\n", entry->d_name, directory);
            closedir(dir);
            return 0;
        }

        loadedPortraits++;
    }

    closedir(dir);
#endif

    return loadedPortraits;
}
