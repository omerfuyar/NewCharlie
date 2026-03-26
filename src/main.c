#include "shucio/shucio.h"
#include "MapLoader.h"
#include <stdio.h>
#include <string.h>

#define BORDER_CHAR_HORIZONTAL '-'
#define BORDER_CHAR_VERTICAL '|'
#define PLAYER_CHAR '@'

#define TEXT_FIELD_MAX_WIDTH 64
#define INPUT_FIELD_MAX_HEIGHT 4

#define MIN_TERMINAL_WIDTH (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + 3)
#define MIN_TERMINAL_HEIGHT (MAP_MAX_HEIGHT + 2)

#define TEXT_FIELD_MAX_LENGTH (TEXT_FIELD_MAX_WIDTH * (MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT - 1))

typedef struct Player
{
    int x;
    int y;
} Player;

// map at left - text field at right - input field at bottom
// borders between them and all around them

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
void renderMap(const Map map)
{
    SHU_SetCursorPosition(1, 1);

    for (int y = 0; y < MAP_MAX_HEIGHT; y++)
    {
        SHU_SetCursorPosition(1, 1 + y);

        for (int x = 0; x < MAP_MAX_WIDTH; x++)
        {
            SHU_PutCharacter(map[y][x]);
        }
    }
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
void renderInputField(const char *selection1, const char *selection2, const char *selection3, const char *selection4)
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
}

// on player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderUpperLayer(const Map map, Player *player, SHUKey key)
{
    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_PutCharacter(map[player->y][player->x]);

    int newX = player->x + (key == SHUKey_ArrowRight) - (key == SHUKey_ArrowLeft);
    int newY = player->y + (key == SHUKey_ArrowDown) - (key == SHUKey_ArrowUp);

    // todo collision and events

    player->x = newX;
    player->y = newY;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_PutCharacter(PLAYER_CHAR);
}

int main(const int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    SHU_Initialize();
    SHU_SetTerminalAlternate(1);
    SHU_SetCursorVisibility(0);
    SHU_ClearTerminal();

    int terminalWidth = 0;
    int terminalHeight = 0;
    SHU_GetTerminalSize(&terminalWidth, &terminalHeight);

    if (terminalWidth < MIN_TERMINAL_WIDTH || terminalHeight < MIN_TERMINAL_HEIGHT)
    {
        SHU_PutString("Error: Terminal is too small for current configurations.\n");
        goto error;
    }

    /*
    SHU_PutString("terminal size: %d x %d\n", terminalWidth, terminalHeight);

    char inputBuffer[256] = {0};
    SHU_Input(inputBuffer, sizeof(inputBuffer));
    SHU_PutString("\ninput: '%s'", inputBuffer);

    SHU_Key();

    return 1;
    */

    const char *mapFile = "resources/maps/map1";

    Map map1 = {0};
    Player player = {0};

    if (LoadMap(mapFile, map1, &player.x, &player.y) != 0)
    {
        goto error;
    }

    renderBorders();

    renderMap(map1);
    renderUpperLayer(map1, &player, SHUKey_Invalid);
    renderInputField("Go north", "Go south", "Go east", "Go west");
    renderTextField("You are in a room. There are doors to the north, south, east, and west.");
    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_Escape)
        {
            break;
        }

        renderMap(map1);
        renderUpperLayer(map1, &player, key);
    }

    SHU_Terminate();
    return 0;

error:
    SHU_Key();

    SHU_Terminate();
    return 1;
}