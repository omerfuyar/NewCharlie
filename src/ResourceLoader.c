#include "ResourceLoader.h"
#include "charBehaviours.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define scanCheck(src, targetCount, format, ...)                                                                 \
    do                                                                                                           \
    {                                                                                                            \
        if (sscanf(src, format, __VA_ARGS__) != targetCount)                                                     \
        {                                                                                                        \
            fprintf(stderr, "Error: Failed to parse line '%s' in map file. Expected format: %s\n", src, format); \
            fclose(mapFile);                                                                                     \
            return 1;                                                                                            \
        }                                                                                                        \
    } while (0)

#ifdef DEBUG
int loglog(const char *format, ...)
{
    FILE *logFile = fopen(LOG_FILE, "a+");
    if (!logFile)
    {
        return 1;
    }

    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    fclose(logFile);
    return 0;
}
#endif

static int loadMap(const char *file, const Portrait *portraits, int maxPortraits, Map *retBufStart, int maxMaps)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load map file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_LINE_MAX_SIZE] = {0};
    int lineCount = 0;
    char playerFound = 0;
    char portalsFound = 0;
    char npcsFound = 0;

    Map *map = NULL;
    NPC *currentNpc = NULL;
    NPCNode *currentNode = NULL;

    while (fgets(lineBuffer, sizeof(lineBuffer), mapFile))
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        if (lineBuffer[0] == '\0')
        {
            continue;
        }

        if (strncmp(lineBuffer, "MAP_ID ", 7) == 0)
        {
            int indexBuffer = -1;
            scanCheck(lineBuffer + 7, 1, "%d", &indexBuffer);

            if (indexBuffer < 0 || indexBuffer >= maxMaps)
            {
                fprintf(stderr, "Error: Map file '%s' has invalid map index %d. Max index is %d.\n", file, indexBuffer, maxMaps - 1);
                fclose(mapFile);
                return 1;
            }

            map = retBufStart + indexBuffer;
            memset(map, 0, sizeof(Map)); // Clean stale/garbage data
            map->index = indexBuffer;

            // Pre-initialize targets to -1 (empty)
            for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
            {
                map->portals[i].targetMapIndex = -1;
                map->npcs[i].index = -1;
            }
        }
        else if (strncmp(lineBuffer, "PORTALS ", 8) == 0)
        {
            char *parseCursor = lineBuffer + 8;
            int bytesRead = 0;

            for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
            {
                if (sscanf(parseCursor, "%d%n", &map->portals[i].targetMapIndex, &bytesRead) != 1)
                {
                    break;
                }

                parseCursor += bytesRead;

                if (*parseCursor == ',')
                {
                    parseCursor++;
                }
            }
        }
        else if (strncmp(lineBuffer, "NPCS ", 5) == 0)
        {
            char *parseCursor = lineBuffer + 5;
            int bytesRead = 0;

            for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
            {
                if (sscanf(parseCursor, "%d%n", &map->npcs[i].index, &bytesRead) != 1)
                {
                    break;
                }

                parseCursor += bytesRead;

                if (*parseCursor == ',')
                {
                    parseCursor++;
                }
            }
        }
        else if (strncmp(lineBuffer, "NPC ", 4) == 0)
        {
            int npcId = -1;
            int portraitIndex = -1;
            scanCheck(lineBuffer + 4, 2, "%d PORTRAIT %d", &npcId, &portraitIndex);

            currentNpc = NULL;
            for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
            {
                if (map->npcs[i].index == npcId)
                {
                    currentNpc = map->npcs + i;
                    currentNpc->nodesCount = 0;
                    break;
                }
            }

            if (currentNpc == NULL)
            {
                fprintf(stderr, "Error: NPC with ID %d defined with 'NPC' but not found in the 'NPCS' line list.\n", npcId);
                fclose(mapFile);
                return 1;
            }

            if (portraitIndex > -1)
            {
                if (portraitIndex >= maxPortraits)
                {
                    fprintf(stderr, "Error: Portrait index %d for NPC %d in map file '%s' is out of bounds. Max index is %d.\n", portraitIndex, npcId, file, maxPortraits - 1);
                    fclose(mapFile);
                    return 1;
                }

                currentNpc->portrait = portraits + portraitIndex;
            }
            else
            {
                currentNpc->portrait = NULL;
            }
        }
        else if (strncmp(lineBuffer, "NODE ", 5) == 0)
        {
            if (currentNpc->nodesCount >= NPC_NODE_MAX_COUNT)
            {
                fprintf(stderr, "Error: Too many nodes for NPC %d. Max %d.\n", currentNpc->index, NPC_NODE_MAX_COUNT);
                fclose(mapFile);
                return 1;
            }

            currentNode = currentNpc->nodes + currentNpc->nodesCount;
            currentNode->choiceCount = 0;

            strncpy(currentNode->text, lineBuffer + 5, NODE_TEXT_SIZE - 1);
            currentNode->text[NODE_TEXT_SIZE - 1] = '\0';

            currentNpc->nodesCount++;
        }
        else if (strncmp(lineBuffer, "CHOICE ", 7) == 0)
        {
            if (currentNode->choiceCount >= INPUT_FIELD_SELECTION_COUNT)
            {
                fprintf(stderr, "Error: Too many choices in a single node for NPC %d. Max %d.\n", currentNpc->index, INPUT_FIELD_SELECTION_COUNT);
                fclose(mapFile);
                return 1;
            }

            NPCChoice *currentChoice = currentNode->choices + currentNode->choiceCount;
            int charsRead = 0;
            scanCheck(lineBuffer + 7, 3, "req:%d set:%d goto:%d %n", &currentChoice->requiredFlags, &currentChoice->action, &currentChoice->nextNodeIndex, &charsRead);
            //! read rest of the line as text, with protection against overflow

            strncpy(currentChoice->text, (lineBuffer + 7) + charsRead, CHOICE_TEXT_SIZE - 1);
            currentChoice->text[CHOICE_TEXT_SIZE - 1] = '\0';

            currentNode->choiceCount++;
        }
        else if (strncmp(lineBuffer, "GRID", 4) == 0)
        {
            break;
        }
        else if (lineBuffer[0] == '#')
        {
            continue;
        }
        else
        {
            fprintf(stderr, "Error: Map file '%s' has invalid format. Line %d starts with invalid prefix '%s'.\n", file, lineCount + 1, lineBuffer);
            fclose(mapFile);
            return 1;
        }
    }

    if (map == NULL)
    {
        fprintf(stderr, "Error: Map index 'MAP_ID' was never defined before parsing data in '%s'.\n", file);
        fclose(mapFile);
        return 1;
    }

    while (fgets(lineBuffer, sizeof(lineBuffer), mapFile) != NULL)
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        if (lineBuffer[0] == '\0')
        {
            continue;
        }

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

                map->playerStartX = posX;
                map->playerStartY = posY;
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

                map->portals[(int)portalsFound].x = posX;
                map->portals[(int)portalsFound].y = posY;
                portalsFound++;
                break;
            case CHAR_NPC:
                if (npcsFound >= MAP_INTERACTABLE_MAX_COUNT)
                {
                    fprintf(stderr, "Error: Too many NPCs found in map file '%s'. Maximum is %d.\n", file, MAP_INTERACTABLE_MAX_COUNT);
                    fclose(mapFile);
                    return 1;
                }

                map->npcs[(int)npcsFound].x = posX;
                map->npcs[(int)npcsFound].y = posY;
                npcsFound++;
                break;
            }
        }

        memcpy(map->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);

    if (!playerFound)
    {
        fprintf(stderr, "Error: No player start position found in map file '%s'.\n", file);
        return 1;
    }

    return 0;
}

static int loadPortrait(const char *file, Portrait *retBufStart, int maxPortraits)
{
    FILE *mapFile = fopen(file, "r");

    if (mapFile == 0)
    {
        fprintf(stderr, "Error: Failed to load portrait file '%s'.\n", file);
        return 1;
    }

    char lineBuffer[MAP_MAX_WIDTH + 8] = {0};
    int lineCount = 0;

    if (fgets(lineBuffer, sizeof(lineBuffer), mapFile) == NULL)
    {
        fprintf(stderr, "Error: Portrait file '%s' is empty.\n", file);
        fclose(mapFile);
        return 1;
    }

    int bufPortraitIndex = -1;

    if (sscanf(lineBuffer, "%d", &bufPortraitIndex) != 1)
    {
        fprintf(stderr, "Error: Failed to parse portrait index in portrait file '%s'. Put the portrait index on the first line.\n", file);
        fclose(mapFile);
        return 1;
    }

    if (bufPortraitIndex < 0 || bufPortraitIndex >= maxPortraits)
    {
        fprintf(stderr, "Error: Portrait index in portrait file '%s' is out of bounds. Must be between 0 and %d.\n", file, maxPortraits - 1);
        fclose(mapFile);
        return 1;
    }

    Portrait *portrait = retBufStart + bufPortraitIndex;

    portrait->index = bufPortraitIndex;

    while (fgets(lineBuffer, MAP_MAX_WIDTH + 8, mapFile) != NULL)
    {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        if (lineBuffer[0] == '\0')
        {
            continue;
        }

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

        memcpy(portrait->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);

    return 0;
}

int loadMaps(const char *directory, const Portrait *portraits, int maxPortraits, Map *retBufStart, int maxMaps)
{
    char pattern[FILE_NAME_MAX_SIZE] = {0};
    int loadedMaps = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA ffd = {0};
    snprintf(pattern, FILE_NAME_MAX_SIZE, "%s\\*.map", directory);

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

        snprintf(pattern, FILE_NAME_MAX_SIZE, "%s\\%s", directory, ffd.cFileName);

        if (loadMap(pattern, portraits, maxPortraits, retBufStart, maxMaps) != 0)
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

        snprintf(pattern, FILE_NAME_MAX_SIZE, "%s/%s", directory, entry->d_name);

        if (loadMap(pattern, portraits, maxPortraits, retBufStart, maxMaps) != 0)
        {
            fprintf(stderr, "Error: Failed to load map file '%s' from folder '%s'.\n", entry->d_name, directory);
            closedir(dir);
            return 0;
        }

        loadedMaps++;
    }

    closedir(dir);
#endif

    for (int i = 0; i < loadedMaps; i++)
    {
        const Map *currentMap = retBufStart + i;

        for (int j = 0; j < MAP_INTERACTABLE_MAX_COUNT; j++)
        {
            const Portal *currentPortal = currentMap->portals + j;

            if (currentPortal->targetMapIndex >= loadedMaps)
            {
                fprintf(stderr, "Error: Map file with index %d has a portal with target map index %d, but only %d maps were loaded. Make sure all portal target indices are between -1 and %d and there are no duplicate map indices.\n",
                        currentMap->index, currentPortal->targetMapIndex, loadedMaps, maxMaps - 1);
                return 0;
            }

            // todo maybe also for npcs
        }
    }

    return loadedMaps;
}

int loadPortraits(const char *directory, Portrait *retPortraits, int maxPortraits)
{
    int loadedPortraits = 0;
    char pattern[FILE_NAME_MAX_SIZE] = {0};

#ifdef _WIN32
    WIN32_FIND_DATAA ffd = {0};
    snprintf(pattern, FILE_NAME_MAX_SIZE, "%s\\*.por", directory);

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

        snprintf(pattern, FILE_NAME_MAX_SIZE, "%s\\%s", directory, ffd.cFileName);

        if (loadPortrait(pattern, retPortraits, maxPortraits) != 0)
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

        snprintf(pattern, FILE_NAME_MAX_SIZE, "%s/%s", directory, entry->d_name);

        if (loadPortrait(pattern, retPortraits, maxPortraits) != 0)
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
