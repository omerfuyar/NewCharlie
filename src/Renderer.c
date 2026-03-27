#include "Renderer.h"
#include <string.h>

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
