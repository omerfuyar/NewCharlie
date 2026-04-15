#include "Renderer.h"
#include "charBehaviours.h"
#include "lookBehaviours.h"
#include <string.h>
#include <stddef.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

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

int callBehaviourForCharacter(CHAR_ID character, Player *player, MapCharacterData data)
{
    switch (character)
    {
#define CHAR_ENTRY(a, b, c, ...)                                                                 \
    case CHAR_##a:                                                                               \
        loglog("character : '%c', playerX : %d, playerY : %d", character, player->x, player->y); \
        return c(player, data);
        CHAR_TABLE
#undef CHAR_ENTRY
    default:
        loglog("No behaviour defined for character '%c'", character);
        return floorBehaviour(player, data);
    }

    return 0;
}

static void putStringStyled(const char *string, int strLen, int x, int y)
{
    SHU_SetCursorPosition(x, y);

    for (int i = 0; i < strLen; i++)
    {
        char character = string[i];

        if (character == '\\')
        {
            char nextCharacter = (i + 1 < strLen) ? string[i + 1] : LOOK_RESET;

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
            SHU_PutCharacter(character);
        }
    }
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

    SHU_SetCursorPosition(INPUT_FIELD_START_X, INPUT_FIELD_START_Y(1) - 1);
    for (int x = 0; x < INPUT_FIELD_MAX_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetCursorPosition(INPUT_FIELD_START_X, INPUT_FIELD_START_Y(2) - 1);
    for (int x = 0; x < INPUT_FIELD_MAX_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetCursorPosition(INPUT_FIELD_START_X, INPUT_FIELD_START_Y(3) - 1);
    for (int x = 0; x < INPUT_FIELD_MAX_WIDTH; x++)
    {
        SHU_PutCharacter(CHAR_BORDER_HORIZONTAL);
    }

    SHU_SetCursorPosition(INPUT_FIELD_START_X, INPUT_FIELD_START_Y(4) - 1);
    for (int x = 0; x < INPUT_FIELD_MAX_WIDTH; x++)
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

void clearTextField(void)
{
    for (int y = 0; y < TEXT_FIELD_MAX_HEIGHT; y++)
    {
        SHU_SetCursorPosition(TEXT_FIELD_START_X, TEXT_FIELD_START_Y + y);

        for (int x = 0; x < TEXT_FIELD_MAX_WIDTH; x++)
        {
            setAttributesForCharacter(CHAR_FLOOR);
            SHU_PutCharacter(CHAR_FLOOR);
            SHU_SetAttributes(SHUAttribute_Reset);
        }
    }
}

// on new text, TEXT_FIELD_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderTextField(const char *text)
{
    clearTextField();

    int textLength = (int)strlen(text);
    int textIndex = 0;

    while (textIndex < textLength)
    {
        putStringStyled(text + textIndex,
                        TEXT_FIELD_MAX_WIDTH,
                        TEXT_FIELD_START_X,
                        TEXT_FIELD_START_Y + (textIndex / TEXT_FIELD_MAX_WIDTH));

        textIndex += TEXT_FIELD_MAX_WIDTH;
    }
}

void clearInputField(void)
{
    for (int y = 0; y < INPUT_FIELD_SELECTION_HEIGHT; y++)
    {
        for (int n = 0; n < INPUT_FIELD_SELECTION_COUNT; n++)
        {
            SHU_SetCursorPosition(INPUT_FIELD_START_X, INPUT_FIELD_START_Y(n + 1) + y);

            for (int x = 0; x < INPUT_FIELD_MAX_WIDTH; x++)
            {
                setAttributesForCharacter(CHAR_FLOOR);
                SHU_PutCharacter(CHAR_FLOOR);
                SHU_SetAttributes(SHUAttribute_Reset);
            }
        }
    }
}

// on new selection, (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + 1) x INPUT_FIELD_MAX_HEIGHT limit
int renderInputField(const char *selection1, const char *selection2, const char *selection3, const char *selection4)
{
    clearInputField();
    char textBuffer[INPUT_FILED_MAX_TEXT] = {0};

    if (selection1 != NULL)
    {
        int strLen = snprintf(textBuffer, sizeof(textBuffer), "> 1: %.*s", (int)sizeof(textBuffer) - 5, selection1);
        putStringStyled(textBuffer, MIN(strLen, INPUT_FIELD_MAX_WIDTH), INPUT_FIELD_START_X, INPUT_FIELD_START_Y(1));
        if (strLen > INPUT_FIELD_MAX_WIDTH)
        {
            putStringStyled(textBuffer + INPUT_FIELD_MAX_WIDTH, strLen - INPUT_FIELD_MAX_WIDTH, INPUT_FIELD_START_X, INPUT_FIELD_START_Y(1) + 1);
        }
    }

    if (selection2 != NULL)
    {
        int strLen = snprintf(textBuffer, sizeof(textBuffer), "> 2: %.*s", (int)sizeof(textBuffer) - 5, selection2);
        putStringStyled(textBuffer, MIN(strLen, INPUT_FIELD_MAX_WIDTH), INPUT_FIELD_START_X, INPUT_FIELD_START_Y(2));
        if (strLen > INPUT_FIELD_MAX_WIDTH)
        {
            putStringStyled(textBuffer + INPUT_FIELD_MAX_WIDTH, strLen - INPUT_FIELD_MAX_WIDTH, INPUT_FIELD_START_X, INPUT_FIELD_START_Y(2) + 1);
        }
    }

    if (selection3 != NULL)
    {
        int strLen = snprintf(textBuffer, sizeof(textBuffer), "> 3: %.*s", (int)sizeof(textBuffer) - 5, selection3);
        putStringStyled(textBuffer, MIN(strLen, INPUT_FIELD_MAX_WIDTH), INPUT_FIELD_START_X, INPUT_FIELD_START_Y(3));
        if (strLen > INPUT_FIELD_MAX_WIDTH)
        {
            putStringStyled(textBuffer + INPUT_FIELD_MAX_WIDTH, strLen - INPUT_FIELD_MAX_WIDTH, INPUT_FIELD_START_X, INPUT_FIELD_START_Y(3) + 1);
        }
    }

    if (selection4 != NULL)
    {
        int strLen = snprintf(textBuffer, sizeof(textBuffer), "> 4: %.*s", (int)sizeof(textBuffer) - 5, selection4);
        putStringStyled(textBuffer, MIN(strLen, INPUT_FIELD_MAX_WIDTH), INPUT_FIELD_START_X, INPUT_FIELD_START_Y(4));
        if (strLen > INPUT_FIELD_MAX_WIDTH)
        {
            putStringStyled(textBuffer + INPUT_FIELD_MAX_WIDTH, strLen - INPUT_FIELD_MAX_WIDTH, INPUT_FIELD_START_X, INPUT_FIELD_START_Y(4) + 1);
        }
    }

    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_1 && selection1 != NULL)
        {
            return 0;
        }
        else if (key == SHUKey_2 && selection2 != NULL)
        {
            return 1;
        }
        else if (key == SHUKey_3 && selection3 != NULL)
        {
            return 2;
        }
        else if (key == SHUKey_4 && selection4 != NULL)
        {
            return 3;
        }
    }

    return -1;
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

    if (playerX != NULL)
    {
        *playerX = map->playerStartX;
    }

    if (playerY != NULL)
    {
        *playerY = map->playerStartY;
    }
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
            SHU_PutCharacter(portrait->data[y][x]);
        }
    }
}

// on player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
int renderPlayer(Player *player, SHUKey key)
{
    int newX = player->x + (key == SHUKey_ArrowRight) - (key == SHUKey_ArrowLeft);
    int newY = player->y + (key == SHUKey_ArrowDown) - (key == SHUKey_ArrowUp);

    if (newX < 0 || newX >= MAP_MAX_WIDTH || newY < 0 || newY >= MAP_MAX_HEIGHT || (player->x == newX && player->y == newY))
    {
        goto renderPlayer;
    }

    MapCharacterData data = {0};

    for (int i = 0; i < MAP_INTERACTABLE_MAX_COUNT; i++)
    {
        if (player->currentMap->portals[i].x == newX && player->currentMap->portals[i].y == newY)
        {
            data.index = player->currentMap->portals[i].targetMapIndex;
            data.x = newX;
            data.y = newY;
            goto callBehaviour;
        }

        if (player->currentMap->npcs[i].x == newX && player->currentMap->npcs[i].y == newY)
        {
            data.index = player->currentMap->npcs[i].index;
            data.x = newX;
            data.y = newY;
            goto callBehaviour;
        }
    }

    data.index = -1;
    data.x = newX;
    data.y = newY;

callBehaviour:

    int result = callBehaviourForCharacter(player->currentMap->data[newY][newX], player, data);
    if (result != 0)
    {
        return result;
    }

renderPlayer:

    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);

    return 0;
}
