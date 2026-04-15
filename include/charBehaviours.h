#pragma once
#include "Renderer.h"
#include "StoryManager.h"
#include "shucio/shucio.h"
#include <stdio.h>

// define how characters will look in the terminal and how will behave if player steps on them
#define CHAR_TABLE                                                                           \
    CHAR_ENTRY(BORDER_HORIZONTAL, '-', wallBehaviour, SHUAttribute_ColorFGBlue)              \
    CHAR_ENTRY(BORDER_VERTICAL, '|', wallBehaviour, SHUAttribute_ColorFGBlue)                \
    CHAR_ENTRY(PLAYER, '@', floorBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGYellow)   \
    CHAR_ENTRY(FLOOR, ' ', floorBehaviour, SHUAttribute_ColorBGBlack)                        \
    CHAR_ENTRY(WALL1, '#', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)      \
    CHAR_ENTRY(WALL2, '=', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)      \
    CHAR_ENTRY(WALL3, '%', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)      \
    CHAR_ENTRY(WALL4, '+', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)      \
    CHAR_ENTRY(DOOR, '&', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)       \
    CHAR_ENTRY(PORTAL, 'O', portalBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGMagenta) \
    CHAR_ENTRY(NPC, 'I', npcBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGCyan)          \
    CHAR_ENTRY(DONE, 'X', floorBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGRed)

#define CHAR_ENTRY(a, b, c, ...) CHAR_##a = b,
typedef enum CHAR_ID
{
    CHAR_TABLE
} CHAR_ID;
#undef CHAR_ENTRY

void setAttributesForCharacter(CHAR_ID character);
int callBehaviourForCharacter(CHAR_ID character, Player *player, MapCharacterData data);

static int floorBehaviour(Player *player, MapCharacterData data)
{
    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter((CHAR_ID)(player->currentMap->data[player->y][player->x]));
    SHU_PutCharacter(player->currentMap->data[player->y][player->x]);

    player->x = data.x;
    player->y = data.y;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_SetAttributes(SHUAttribute_Reset);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);

    return 0;
}

static int wallBehaviour(Player *player, MapCharacterData data)
{
    (void)player;
    (void)data;

    return 0;
}

static int portalBehaviour(Player *player, MapCharacterData data)
{ // data : index is the target map index
    (void)player;
    (void)data;

    player->currentMap = &(player->currentMap - player->currentMap->index)[data.index];
    renderMap(player->currentMap, &player->x, &player->y);

    return 0;
}

static int npcBehaviour(Player *player, MapCharacterData data)
{ // data : index is the npc index in the map
    const NPC *npc = player->currentMap->npcs + data.index;
    startDialogue(player, npc);

    return 0;
}
