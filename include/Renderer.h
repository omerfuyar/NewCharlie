#pragma once
#include "shucio/shucio.h"
#include "config.h"

// for once in program
void renderBorders(void);

// on new map, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderMap(const Map *map, int *playerX, int *playerY);

// on new portrait, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderPortrait(const Portrait *portrait);

// on new text, TEXT_FIELD_MAX_WIDTH x MAP_MAX_HEIGHT limit
void renderTextField(const char *text);

// on new selection, (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + 1) x INPUT_FIELD_MAX_HEIGHT limit
int renderInputField(const char *selection1, const char *selection2, const char *selection3, const char *selection4);

// on player move, fixed size, MAP_MAX_WIDTH x MAP_MAX_HEIGHT limit
int renderPlayer(const Map *map, Player *player, SHUKey key);
