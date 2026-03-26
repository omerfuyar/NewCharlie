#include "shucio/shucio.h"
#include "MapLoader.h"
#include "stdio.h"

#define WINDOW_BORDER_SIZE 1

#define TEXT_FIELD_MAX_WIDTH 32
#define INPUT_FIELD_MAX_HEIGHT 4

#define MIN_TERMINAL_WIDTH (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + WINDOW_BORDER_SIZE * 3)
#define MIN_TERMINAL_HEIGHT (MAP_MAX_HEIGHT + INPUT_FIELD_MAX_HEIGHT + WINDOW_BORDER_SIZE * 3)

// map at left - text field at right - input field at bottom
// borders between them and all around them

// for once in program
void renderBorders()
{
}

// on new map and player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderMap(const Map *map)
{
    for (unsigned char y = 0; y < map->height; y++)
    {
        for (unsigned char x = 0; x < map->width; x++)
        {
            SHU_PutCharacter(map->data[y * map->width + x]);
        }
    }
}

// on new text, TEXT_FIELD_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderTextField(const char *text)
{
    (void)text;
}

// on new selection, (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + WINDOW_BORDER_SIZE) x INPUT_FIELD_MAX_HEIGHT limit
void renderInputField(const char *selection1, const char *selection2, const char *selection3, const char *selection4)
{
    (void)selection1;
    (void)selection2;
    (void)selection3;
    (void)selection4;
}

int main(const int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    SHU_Initialize();
    SHU_SetTerminalAlternate(1);
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
    Map map1;

    if (LoadMap(mapFile, &map1) != 0)
    {
        goto error;
    }

    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_Escape)
        {
            break;
        }

        renderMap(&map1);
    }

    SHU_Terminate();
    return 0;

error:
    SHU_Terminate();
    return 1;
}