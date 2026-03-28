#include "Renderer.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static void setAttributesForCharacter(CHAR_ID character)
{
    switch (character)
    {
#define CHAR_ENTRY(a, b, ...)           \
    case CHAR_##a:                      \
        SHU_SetAttributes(__VA_ARGS__); \
        break;
        CHAR_TABLE
#undef CHAR_ENTRY
    }
}

static void setAttributesForLook(LOOK_ID look)
{
    switch (look)
    {
#define LOOK_ENTRY(a, b, ...)           \
    case LOOK_##a:                      \
        SHU_SetAttributes(__VA_ARGS__); \
        break;
        LOOK_TABLE
#undef LOOK_ENTRY
    }
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
            if (lineBuffer[i] == CHAR_PLAYER)
            {
                if (playerFound)
                {
                    fprintf(stderr, "Error: Multiple player start positions found in map file '%s'.\n", file);
                    fclose(mapFile);
                    return 1;
                }

                retMap->playerStartX = (int)i;
                retMap->playerStartY = lineCount - 1;
                playerFound = 1;
                lineBuffer[i] = ' ';
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
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0'; // todo maybe different for linux

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
            fprintf(stderr, "Error: Failed to load map file '%s'.\n", pattern);
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
            fprintf(stderr, "Error: Failed to load map file '%s'.\n", pattern);
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
            fprintf(stderr, "Error: Failed to load portrait file '%s'.\n", pattern);
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
            fprintf(stderr, "Error: Failed to load portrait file '%s'.\n", pattern);
            closedir(dir);
            return 0;
        }

        loadedPortraits++;
    }

    closedir(dir);
#endif

    return loadedPortraits;
}

// for once in program
void renderBorders(void)
{
    setAttributesForCharacter(CHAR_BORDER_HORIZONTAL);

    // horizontals
    SHU_SetCursorPosition(0, 0);
    for (int x = 0; x < MIN_TERMINAL_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetCursorPosition(0, MIN_TERMINAL_HEIGHT - 1);
    for (int x = 0; x < MIN_TERMINAL_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT);
    for (int x = 0; x < TEXT_FIELD_MAX_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetAttributes(SHUAttribute_Reset);
    setAttributesForCharacter(CHAR_BORDER_VERTICAL);

    // verticals
    for (int y = 1; y < MIN_TERMINAL_HEIGHT - 1; y++)
    {
        SHU_SetCursorPosition(0, y);
        SHU_PutCharacter(CHAR_BORDER_VERTICAL);
        SHU_SetCursorPosition(MIN_TERMINAL_WIDTH - 1, y);
        SHU_PutCharacter(CHAR_BORDER_VERTICAL);
    }

    for (int y = 1; y < MAP_MAX_HEIGHT + 1; y++)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 1, y);
        SHU_PutCharacter(CHAR_BORDER_VERTICAL);
    }

    SHU_SetAttributes(SHUAttribute_Reset);
}

// on new text, TEXT_FIELD_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderTextField(const char *text)
{
    int textLength = (int)strlen(text);
    int cursorIndex = 0;

    for (int i = 0; i < textLength; i++)
    {
        char character = text[i];

        if (character == '\\')
        {
            char nextCharacter = (i + 1 < textLength) ? text[i + 1] : LOOK_RESET;

            if (nextCharacter == '\\')
            {
                i++;
                goto putChar;
            }
            else
            {
                setAttributesForLook(nextCharacter);
                i++;
            }
        }
        else
        {
        putChar:
            SHU_SetCursorPosition(MAP_MAX_WIDTH + (cursorIndex % TEXT_FIELD_MAX_WIDTH) + 2, (cursorIndex / TEXT_FIELD_MAX_WIDTH) + 1);
            SHU_PutCharacter(character);
            cursorIndex++;
        }
    }
}

// on new selection, (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + 1) x INPUT_FIELD_MAX_HEIGHT limit
int renderInputField(const char *selection1, const char *selection2, const char *selection3, const char *selection4)
{
    if (selection1 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 1);
        SHU_PutString("> 1: %.*s", TEXT_FIELD_MAX_WIDTH, selection1);
    }

    if (selection2 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 2);
        SHU_PutString("> 2: %.*s", TEXT_FIELD_MAX_WIDTH, selection2);
    }

    if (selection3 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 3);
        SHU_PutString("> 3: %.*s", TEXT_FIELD_MAX_WIDTH, selection3);
    }

    if (selection4 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 4);
        SHU_PutString("> 4: %.*s", TEXT_FIELD_MAX_WIDTH, selection4);
    }

    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_1 && selection1 != NULL)
        {
            return 1;
        }
        else if (key == SHUKey_2 && selection2 != NULL)
        {
            return 2;
        }
        else if (key == SHUKey_3 && selection3 != NULL)
        {
            return 3;
        }
        else if (key == SHUKey_4 && selection4 != NULL)
        {
            return 4;
        }
    }

    return 0;
}

// on new map, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderMap(const Map *map, int *playerX, int *playerY)
{
    SHU_SetCursorPosition(1, 1);

    for (int y = 0; y < MAP_MAX_HEIGHT; y++)
    {
        SHU_SetCursorPosition(1, 1 + y);

        for (int x = 0; x < MAP_MAX_WIDTH; x++)
        {
            setAttributesForCharacter(map->data[y][x]);
            SHU_PutCharacter(map->data[y][x]);
            SHU_SetAttributes(SHUAttribute_Reset);
        }
    }

    *playerX = map->playerStartX;
    *playerY = map->playerStartY;
}

void renderPortrait(const Portrait *portrait)
{
    SHU_SetCursorPosition(1, 1);

    for (int y = 0; y < MAP_MAX_HEIGHT; y++)
    {
        SHU_SetCursorPosition(1, 1 + y);

        for (int x = 0; x < MAP_MAX_WIDTH; x++)
        {
            setAttributesForCharacter(portrait->data[y][x]);
            SHU_PutCharacter(portrait->data[y][x]);
            SHU_SetAttributes(SHUAttribute_Reset);
        }
    }
}

// on player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderPlayer(const Map *map, Player *player, SHUKey key)
{
    int newX = player->x + (key == SHUKey_ArrowRight) - (key == SHUKey_ArrowLeft);
    int newY = player->y + (key == SHUKey_ArrowDown) - (key == SHUKey_ArrowUp);

    if (newX < 0 || newX >= MAP_MAX_WIDTH || newY < 0 || newY >= MAP_MAX_HEIGHT)
    {
        return;
    }

    // todo collisions and events

    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter(map->data[player->y][player->x]);
    SHU_PutCharacter(map->data[player->y][player->x]);

    player->x = newX;
    player->y = newY;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_SetAttributes(SHUAttribute_Reset);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);
}
