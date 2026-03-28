#include "Renderer.h"
#include "StoryManager.h"

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
        SHU_PutString("Error: Terminal is too small for current configurations. Minimum size %dx%d. Current size %dx%d.\n", MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT, terminalWidth, terminalHeight);
        goto error;
    }

    const char *mapsDir = "resources/maps";

    Map maps[MAP_MAX_COUNT] = {0};
    Player player = {0};

    int numMaps = loadMaps(mapsDir, maps, MAP_MAX_COUNT);
    if (numMaps == 0)
    {
        goto error;
    }

    Map *currentMap = &maps[0];

    renderBorders();

    renderInputField("Go north", "Go south", "Go east", "Go west");
    SHU_SetAttributes(SHUAttribute_Bold, SHUAttribute_ColorBGCyan, SHUAttribute_ColorFGGreen);
    renderTextField("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec a diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec consectetur ante hendrerit. Donec et mollis dolor. Praesent et diam eget libero egestas mattis sit amet vitae augue. Nam tincidunt congue enim, ut porta lorem lacinia consectetur.");

mapChange:
    renderBottomLayer(currentMap, &player);
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
            currentMap = (currentMap == &maps[0]) ? &maps[1] : &maps[0];
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
