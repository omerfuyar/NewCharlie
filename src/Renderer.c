#include "Renderer.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static int loadMap(const char *file, Map *retMap)
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

        lineCount++;

        if (lineCount > MAP_MAX_HEIGHT)
        {
            fprintf(stderr, "Error: Map file has too many rows.\n");
            fclose(mapFile);
            return 1;
        }

        size_t lineLength = strlen(lineBuffer);
        if (lineLength > MAP_MAX_WIDTH)
        {
            fprintf(stderr, "Error: Line in map file has too many columns.\n");
            fclose(mapFile);
            return 1;
        }

        memcpy(retMap->data[lineCount - 1], lineBuffer, MAP_MAX_WIDTH);
    }

    fclose(mapFile);
    return 0;
}

int loadMaps(const char *directory, Map *maps, int maxMaps)
{
    int loadedMaps = 0;

#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    WIN32_FIND_DATAA ffd = {0};
    char pattern[STRING_MAX_SIZE] = {0};
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

        if (loadMap(pattern, &maps[loadedMaps]) != 0)
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

        if (loadMap(pattern, &maps[loadedMaps]) != 0)
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

// for once in program
void renderBorders(void)
{
    // horizontals

    SHU_SetCursorPosition(0, 0);
    for (int x = 0; x < MIN_TERMINAL_WIDTH; x++)
    {
        SHU_PutCharacter(BORDER_CHAR_HORIZONTAL);
    }

    SHU_SetCursorPosition(0, MIN_TERMINAL_HEIGHT - 1);
    for (int x = 0; x < MIN_TERMINAL_WIDTH; x++)
    {
        SHU_PutCharacter(BORDER_CHAR_HORIZONTAL);
    }

    SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT);
    for (int x = 0; x < TEXT_FIELD_MAX_WIDTH; x++)
    {
        SHU_PutCharacter(BORDER_CHAR_HORIZONTAL);
    }

    // verticals

    for (int y = 1; y < MIN_TERMINAL_HEIGHT - 1; y++)
    {
        SHU_SetCursorPosition(0, y);
        SHU_PutCharacter(BORDER_CHAR_VERTICAL);
        SHU_SetCursorPosition(MIN_TERMINAL_WIDTH - 1, y);
        SHU_PutCharacter(BORDER_CHAR_VERTICAL);
    }

    for (int y = 1; y < MAP_MAX_HEIGHT + 1; y++)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 1, y);
        SHU_PutCharacter(BORDER_CHAR_VERTICAL);
    }
}

// on new map, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderMap(const Map *map, Player *player)
{
    SHU_SetCursorPosition(1, 1);

    for (int y = 0; y < MAP_MAX_HEIGHT; y++)
    {
        SHU_SetCursorPosition(1, 1 + y);

        for (int x = 0; x < MAP_MAX_WIDTH; x++)
        {
            SHU_PutCharacter(map->data[y][x]);
        }
    }

    player->x = map->playerStartX;
    player->y = map->playerStartY;
}

// on new text, TEXT_FIELD_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderTextField(const char *text)
{
    int textLength = (int)strlen(text) - 1;
    int lineCount = (textLength + TEXT_FIELD_MAX_WIDTH - 1) / TEXT_FIELD_MAX_WIDTH;

    for (int i = 0; i < lineCount && i < MAP_MAX_HEIGHT; i++)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, 1 + i);
        SHU_PutString("%.*s", TEXT_FIELD_MAX_WIDTH, text + i * TEXT_FIELD_MAX_WIDTH);
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

    if (selection1 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 2);
        SHU_PutString("> 2: %.*s", TEXT_FIELD_MAX_WIDTH, selection2);
    }

    if (selection1 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 3);
        SHU_PutString("> 3: %.*s", TEXT_FIELD_MAX_WIDTH, selection3);
    }

    if (selection1 != NULL)
    {
        SHU_SetCursorPosition(MAP_MAX_WIDTH + 2, MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT + 4);
        SHU_PutString("> 4: %.*s", TEXT_FIELD_MAX_WIDTH, selection4);
    }

    // todo get input and return selected option
    return 0;
}

// on player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderUpperLayer(const Map *map, Player *player, SHUKey key)
{
    int newX = player->x + (key == SHUKey_ArrowRight) - (key == SHUKey_ArrowLeft);
    int newY = player->y + (key == SHUKey_ArrowDown) - (key == SHUKey_ArrowUp);

    // todo collisions and events

    if (newX < 0 || newX >= MAP_MAX_WIDTH || newY < 0 || newY >= MAP_MAX_HEIGHT)
    {
        return;
    }

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_PutCharacter(map->data[player->y][player->x]);

    player->x = newX;
    player->y = newY;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_PutCharacter(PLAYER_CHAR);
}
