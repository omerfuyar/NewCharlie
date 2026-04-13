#include "StoryManager.h"
#include "Renderer.h"
#include <stddef.h>
#include <stdlib.h>

static int playerCheckRequirements(const Player *player, unsigned int requirements)
{
    if (requirements == 0)
    {
        return 1;
    }

    unsigned char req1 = (unsigned char)((requirements >> 24) & 0xFF);
    unsigned char req2 = (unsigned char)((requirements >> 16) & 0xFF);
    unsigned char req3 = (unsigned char)((requirements >> 8) & 0xFF);
    unsigned char req4 = (unsigned char)(requirements & 0xFF);

    if (req1 != 0 && player->flags[req1] == 0)
    {
        return 0;
    }

    if (req2 != 0 && player->flags[req2] == 0)
    {
        return 0;
    }

    if (req3 != 0 && player->flags[req3] == 0)
    {
        return 0;
    }

    if (req4 != 0 && player->flags[req4] == 0)
    {
        return 0;
    }

    return 1;
}

static int choiceExecuteAction(Player *player, unsigned int action)
{
    if (action == PLAYER_REQUIREMENT_NONE)
    {
        return 0;
    }
    else if (action == PLAYER_REQUIREMENT_IMPOSSIBLE)
    {
        return 1;
    }

    unsigned char act = (unsigned char)((action >> 24) & 0xFF);
    unsigned char arg1 = (unsigned char)((action >> 16) & 0xFF);
    unsigned char arg2 = (unsigned char)((action >> 8) & 0xFF);
    unsigned char arg3 = (unsigned char)(action & 0xFF);

    switch (act)
    {
    case 1: // Set Flags
        if (arg1 > PLAYER_REQUIREMENT_NONE && arg1 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg1] = 1;
        if (arg2 > PLAYER_REQUIREMENT_NONE && arg2 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg2] = 1;
        if (arg3 > PLAYER_REQUIREMENT_NONE && arg3 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg3] = 1;
        loglog("Set player flag %d, %d and %d", arg1, arg2, arg3);
        break;

    case 2: // Clear Flags
        if (arg1 > PLAYER_REQUIREMENT_NONE && arg1 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg1] = 0;
        if (arg2 > PLAYER_REQUIREMENT_NONE && arg2 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg2] = 0;
        if (arg3 > PLAYER_REQUIREMENT_NONE && arg3 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg3] = 0;
        loglog("Clear player flag %d, %d and %d", arg1, arg2, arg3);
        break;

    case 3: // Set and Clear Flags
        if (arg1 > PLAYER_REQUIREMENT_NONE && arg1 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg1] = 1;
        if (arg2 > PLAYER_REQUIREMENT_NONE && arg2 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg2] = 1;
        if (arg3 > PLAYER_REQUIREMENT_NONE && arg3 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg3] = 0;
        loglog("Set player flag %d & %d and clear player flag %d", arg1, arg2, arg3);
        break;

    case 4: // Toggle Flags
        if (arg1 > PLAYER_REQUIREMENT_NONE && arg1 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg1] = !player->flags[arg1];
        if (arg2 > PLAYER_REQUIREMENT_NONE && arg2 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg2] = !player->flags[arg2];
        if (arg3 > PLAYER_REQUIREMENT_NONE && arg3 < PLAYER_REQUIREMENT_IMPOSSIBLE)
            player->flags[arg3] = !player->flags[arg3];
        loglog("Toggle player flag %d, %d and %d", arg1, arg2, arg3);
        break;

    case 5: // Exit Game
        loglog("Exit game");
        exit(0);
        break;

    case 6: // Move Player
        if (arg1 < MAP_MAX_WIDTH && arg2 < MAP_MAX_HEIGHT)
        {
            player->x = arg1;
            player->y = arg2;
            loglog("Move player to (%d, %d)", arg1, arg2);
        }
        break;

    case 7: // Edit Map
        if (arg1 < MAP_MAX_WIDTH && arg2 < MAP_MAX_HEIGHT)
        {
            player->currentMap->data[arg2][arg1] = (char)arg3;
            loglog("Changed map tile at (%d, %d) to '%c'", arg1, arg2, arg3);
        }
        break;

    case 8: // Load Map
        if (arg1 < MAP_MAX_COUNT)
        {
            player->currentMap = &(player->currentMap - player->currentMap->index)[arg1];

            renderMap(player->currentMap, &player->x, &player->y);

            if (arg2 > PLAYER_REQUIREMENT_NONE && arg2 < PLAYER_REQUIREMENT_IMPOSSIBLE)
                player->flags[arg2] = 1;
            if (arg3 > PLAYER_REQUIREMENT_NONE && arg3 < PLAYER_REQUIREMENT_IMPOSSIBLE)
                player->flags[arg3] = 1;

            loglog("Load map %d and Set player flag %d & %d", arg1, arg2, arg3);
        }
        break;

    default:
        loglog("Invalid action type: %d", act);
        break;
    }

    return 1;
}

static int dialogueShowNode(Player *player, const NPCNode *node)
{
    const char *choices[INPUT_FIELD_SELECTION_COUNT] = {0};

    for (int i = 0; i < node->choiceCount && i < INPUT_FIELD_SELECTION_COUNT; i++)
    {
        choices[i] = playerCheckRequirements(player, node->choices[i].requiredFlags) ? node->choices[i].text : NULL;
    }

    renderTextField(node->text);
    int choice = renderInputField(choices[0], choices[1], choices[2], choices[3]);

    if (choice >= 0 && choice < node->choiceCount)
    {
        choiceExecuteAction(player, node->choices[choice].action);
    }

    return node->choices[choice].nextNodeIndex;
}

int startDialogue(Player *player, const NPC *npc)
{
    if (npc->portrait != NULL)
    {
        renderPortrait(npc->portrait);
    }

    clearInputField();
    clearTextField();

    int index = npc->nodesCount - 1;
    while (index >= 0)
    {
        if (playerCheckRequirements(player, npc->nodes[index].requiredFlags))
        {
            break;
        }

        index--;
    }

    while ((index = dialogueShowNode(player, &npc->nodes[index])) >= 0)
    {
    }

    clearInputField();
    clearTextField();

    renderMap(player->currentMap, NULL, NULL);
    renderPlayer(player, SHUKey_Invalid);

    return 0;
}
