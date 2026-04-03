#include "StoryManager.h"
#include "Renderer.h"
#include <stddef.h>

int startDialogue(const Map *map, const NPC *npc, Player *player)
{
    (void)player;
    (void)map;
    if (npc->portrait != NULL)
    {
        renderPortrait(npc->portrait);
    }

    // todo flags and requirements for dialogue nodes and choices
    renderTextField(npc->nodes[0].text);
    renderInputField(npc->nodes[0].choices[0].text,
                     npc->nodes[0].choices[1].text,
                     npc->nodes[0].choices[2].text,
                     npc->nodes[0].choices[3].text);

    clearInputField();
    clearTextField();

    return 0;
}
