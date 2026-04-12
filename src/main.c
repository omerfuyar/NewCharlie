#include "Renderer.h"
#include "ResourceLoader.h"
#include "StoryManager.h"
#include <signal.h>

void handleSignal(int signal)
{
    (void)signal;
    SHU_ClearTerminal();
    SHU_Terminate();
}

int main(const int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);
    signal(SIGSEGV, handleSignal);

    SHU_Initialize();
    SHU_SetTerminalAlternate(1);
    SHU_SetCursorVisibility(0);
    SHU_ClearTerminal();

    int terminalWidth = 0;
    int terminalHeight = 0;
    SHU_GetTerminalSize(&terminalWidth, &terminalHeight);

    if (terminalWidth < MIN_TERMINAL_WIDTH || terminalHeight < MIN_TERMINAL_HEIGHT)
    {
        SHU_PutString("Error: Terminal is too small for current configurations. Minimum size %dx%d. Current size %dx%d.\n",
                      MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT, terminalWidth, terminalHeight);
        goto error;
    }

    Map maps[MAP_MAX_COUNT] = {0};
    Portrait portraits[PORTRAIT_MAX_COUNT] = {0};
    Player player = {0};
    player.flags[0] = 0xFF;

    int numPortraits = loadPortraits("resources/portraits", portraits, PORTRAIT_MAX_COUNT);
    if (numPortraits == 0)
    {
        goto error;
    }

    int numMaps = loadMaps("resources/maps",
                           portraits, PORTRAIT_MAX_COUNT,
                           maps, MAP_MAX_COUNT);
    if (numMaps == 0)
    {
        goto error;
    }

    Map *currentMap = &maps[0];

    renderBorders();

    // renderInputField("Go north", "Go south", "Go east", "Go west");m
    // renderTextField("Lorem ipsum dolor \"\\\\\" '\\?sit amet\\_', consectetur adipiscing elit. Donec a diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec consectetur ante hendrerit. Donec et mollis dolor. Praesent et diam eget libero egestas mattis sit amet vitae augue. Nam tincidunt congue enim, ut porta lorem lacinia consectetur.");

    renderMap(currentMap, &player.x, &player.y);
    renderPlayer(currentMap, &player, SHUKey_Invalid);

    while (1)
    {
        SHUKey key = SHU_Key();

        if (key == SHUKey_Escape)
        {
            break;
        }

        if (renderPlayer(currentMap, &player, key) != 0)
        {
            goto error;
        }
    }

    SHU_Terminate();
    return 0;

error:
    SHU_PutString("\nAn error occurred. Press any key to exit.\n");
    SHU_Key();

    SHU_Terminate();
    return 1;
}
