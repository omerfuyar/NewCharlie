#pragma once
#include "config.h"
#include "shucio/shucio.h"
#include <stddef.h>

// define how characters will look in the terminal and how will behave if player steps on them
#define CHAR_TABLE                                                                           \
    CHAR_ENTRY(BORDER_HORIZONTAL, '-', nullBehaviour, SHUAttribute_ColorFGBlue)              \
    CHAR_ENTRY(BORDER_VERTICAL, '|', nullBehaviour, SHUAttribute_ColorFGBlue)                \
    CHAR_ENTRY(PLAYER, '@', nullBehaviour, SHUAttribute_Bold, SHUAttribute_ColorFGYellow)    \
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
void setAttributesForLook(LOOK_ID look);
int callBehaviourForCharacter(CHAR_ID character, const Map *map, Player *player, int newX, int newY);

static int nullBehaviour(const Map *map, Player *player, int newX, int newY)
{
    (void)map;
    (void)player;
    (void)newX;
    (void)newY;
    return 0;
}

static int floorBehaviour(const Map *map, Player *player, int newX, int newY)
{
    SHU_SetCursorPosition(player->x + 1, player->y + 1);

    setAttributesForCharacter((CHAR_ID)(map->data[player->y][player->x]));
    SHU_PutCharacter(map->data[player->y][player->x]);

    player->x = newX;
    player->y = newY;

    SHU_SetCursorPosition(player->x + 1, player->y + 1);
    SHU_SetAttributes(SHUAttribute_Reset);

    setAttributesForCharacter(CHAR_PLAYER);
    SHU_PutCharacter(CHAR_PLAYER);
    SHU_SetAttributes(SHUAttribute_Reset);

    return 0;
}

static int wallBehaviour(const Map *map, Player *player, int newX, int newY)
{
    (void)map;
    (void)player;
    (void)newX;
    (void)newY;
    return 0;
}

static int portalBehaviour(const Map *map, Player *player, int newX, int newY)
{
    (void)map;
    (void)player;
    (void)newX;
    (void)newY;
    return 0; // todo
}

static int npcBehaviour(const Map *map, Player *player, int newX, int newY)
{
    (void)map;
    (void)player;
    (void)newX;
    (void)newY;
    return 0; // todo
}
