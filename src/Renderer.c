#include "Renderer.h"
#include "charBehaviours.h"
#include <string.h>

void setAttributesForCharacter(CHAR_ID character)
{
    switch (character)
    {
#define CHAR_ENTRY(a, b, c, ...)        \
    case CHAR_##a:                      \
        SHU_SetAttributes(__VA_ARGS__); \
        break;
        CHAR_TABLE
#undef CHAR_ENTRY
    }
}

void setAttributesForLook(LOOK_ID look)
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

int callBehaviourForCharacter(CHAR_ID character, const Map *map, Player *player, MapCharacterData data)
{
    switch (character)
    {
#define CHAR_ENTRY(a, b, c, ...) \
    case CHAR_##a:               \
        return c(map, player, data);
        CHAR_TABLE
#undef CHAR_ENTRY
    }

    return 0;
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

    SHU_SetCursorPosition(*playerX + 1, *playerY + 1);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);
}

// on new interaction, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
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
int renderPlayer(const Map *map, Player *player, SHUKey key)
{
    int newX = player->x + (key == SHUKey_ArrowRight) - (key == SHUKey_ArrowLeft);
    int newY = player->y + (key == SHUKey_ArrowDown) - (key == SHUKey_ArrowUp);

    if (newX < 0 || newX >= MAP_MAX_WIDTH || newY < 0 || newY >= MAP_MAX_HEIGHT || (player->x == newX && player->y == newY))
    {
        return 0;
    }

    MapCharacterData data = {0};

    for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
    {
        if (map->portals[i].x == newX && map->portals[i].y == newY)
        {
            data.index = map->portals[i].targetMapIndex;
            data.x = newX;
            data.y = newY;
            goto skipPos;
        }

        if (map->npcs[i].x == newX && map->npcs[i].y == newY)
        {
            data.index = map->npcs[i].index;
            data.x = newX;
            data.y = newY;
            goto skipPos;
        }
    }

    data.index = -1;
    data.x = newX;
    data.y = newY;

skipPos:

    int result = callBehaviourForCharacter(map->data[newY][newX], map, player, data);
    if (result != 0)
    {
        return result;
    }

    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);

    return 0;
}
