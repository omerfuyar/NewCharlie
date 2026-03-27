#include "MapLoader.h"
#include "Renderer.h"
#include <stdio.h>
#include <string.h>

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
        SHU_PutString("Error: Terminal is too small for current configurations. Minimum size is %dx%d.\n", MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT);
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

    const char *mapFile = "resources/maps/map1.txt";
    const char *testFile = "resources/maps/test.txt";

    Map map1 = {0};
    Map test = {0};
    Player player = {0};

    if (LoadMap(mapFile, &map1) != 0)
    {
        goto error;
    }

    if (LoadMap(testFile, &test) != 0)
    {
        goto error;
    }

    Map *currentMap = &map1;

    renderBorders();

    renderInputField("Go north", "Go south", "Go east", "Go west");
    renderTextField("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec a diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec consectetur ante hendrerit. Donec et mollis dolor. Praesent et diam eget libero egestas mattis sit amet vitae augue. Nam tincidunt congue enim, ut porta lorem lacinia consectetur.");

mapChange:
    renderMap(currentMap, &player);
    renderUpperLayer(currentMap, &player, SHUKey_Invalid);

    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_Escape)
        {
            break;
        }

        if (key == SHUKey_Space)
        {
            currentMap = (currentMap == &map1) ? &test : &map1;
            goto mapChange;
        }

        renderUpperLayer(currentMap, &player, key);
    }

    SHU_Terminate();
    return 0;

error:
    SHU_PutString("\nAn error occurred. Press any key to exit.\n");
    SHU_Key();

    SHU_Terminate();
    return 1;
}
