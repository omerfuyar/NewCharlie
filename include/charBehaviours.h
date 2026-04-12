#pragma once
#include "Renderer.h"
#include "StoryManager.h"
#include "shucio/shucio.h"
#include <stdio.h>

// define how characters will look in the terminal and how will behave if player steps on them
#define CHAR_TABLE                                                                           \
    CHAR_ENTRY(BORDER_HORIZONTAL, '-', floorBehaviour, SHUAttribute_ColorFGBlue)             \
    CHAR_ENTRY(BORDER_VERTICAL, '|', floorBehaviour, SHUAttribute_ColorFGBlue)               \
    CHAR_ENTRY(PLAYER, '@', floorBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGYellow)   \
    CHAR_ENTRY(FLOOR, ' ', floorBehaviour, SHUAttribute_ColorBGBlack)                        \
    CHAR_ENTRY(WALL, '#', wallBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGGreen)       \
    CHAR_ENTRY(PORTAL, 'O', portalBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGMagenta) \
    CHAR_ENTRY(NPC, 'I', npcBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGCyan)

#define CHAR_ENTRY(a, b, c, ...) CHAR_##a = b,
typedef enum CHAR_ID
{
    CHAR_TABLE
} CHAR_ID;
#undef CHAR_ENTRY

void setAttributesForCharacter(CHAR_ID character);
int callBehaviourForCharacter(CHAR_ID character, const Map *map, Player *player, MapCharacterData data);

static int floorBehaviour(const Map *map, Player *player, MapCharacterData data)
{
    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter((CHAR_ID)(map->data[player->y][player->x]));
    SHU_PutCharacter(map->data[player->y][player->x]);

    player->x = data.x;
    player->y = data.y;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_SetAttributes(SHUAttribute_Reset);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);

    return 0;
}

static int wallBehaviour(const Map *map, Player *player, MapCharacterData data)
{
    (void)map;
    (void)player;
    (void)data;

    return 0;
}

static int portalBehaviour(const Map *map, Player *player, MapCharacterData data)
{ // data : index is the target map index
    (void)map;
    (void)player;
    (void)data;

    const Map *targetMap = &(map - map->index)[data.index];

    renderMap(targetMap, &player->x, &player->y);

    return 0;
}

static int npcBehaviour(const Map *map, Player *player, MapCharacterData data)
{ // data : index is the npc index in the map
    const NPC *npc = map->npcs + data.index;

    startDialogue(map, npc, player);

    return 0;
}
