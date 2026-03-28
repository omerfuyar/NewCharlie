#pragma once
#include "shucio/shucio.h"
#include "config.h"

typedef struct Map
{
    int index;
    int playerStartX;
    int playerStartY;
    int portals[MAP_PORTALS_MAX_COUNT];
    char data[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
} Map;

typedef struct Portrait
{
    char data[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
} Portrait;

typedef struct Player
{
    int x;
    int y;
} Player;

// loads .map files in the given directory, returns the number of maps loaded, or 0 on error
int loadMaps(const char *directory, Map *maps, int maxMaps);

// loads .por files in the given directory, returns the number of portraits loaded, or 0 on error
int loadPortraits(const char *directory, Portrait *portraits, int maxPortraits);

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
void renderPlayer(const Map *map, Player *player, SHUKey key);
