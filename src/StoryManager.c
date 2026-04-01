#include "StoryManager.h"
#include "Renderer.h"

int startDialogue(const NPC *npc, Player *player)
{
    (void)player;
    renderPortrait(npc->portrait);
    renderTextField(npc->nodes[0].text);
    renderInputField(npc->nodes[0].choices[0].text,
                     npc->nodes[0].choices[1].text,
                     npc->nodes[0].choices[2].text,
                     npc->nodes[0].choices[3].text);
    return 0;
}
