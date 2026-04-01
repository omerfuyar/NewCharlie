#pragma once

#define STRING_MAX_SIZE 128

#define PLAYER_MAX_FLAGS 32

#define MAP_MAX_WIDTH 64
#define MAP_MAX_HEIGHT 32
#define MAP_DATA_MAX_SIZE (MAP_MAX_WIDTH * MAP_MAX_HEIGHT)

#define MAP_INTERACTABLE_MAX_COUNT 4

#define MAP_MAX_COUNT 16

#define PORTRAIT_MAX_COUNT 8

#define NPC_MAX_COUNT 16
#define NPC_NODE_MAX_COUNT 16

#define INPUT_FIELD_SELECTION_COUNT 4
#define INPUT_FIELD_SELECTION_HEIGHT 1

#define TEXT_FIELD_MAX_WIDTH 64
#define INPUT_FIELD_MAX_HEIGHT (INPUT_FIELD_SELECTION_COUNT * INPUT_FIELD_SELECTION_HEIGHT)

#define MIN_TERMINAL_WIDTH (MAP_MAX_WIDTH + TEXT_FIELD_MAX_WIDTH + 3)
#define MIN_TERMINAL_HEIGHT (MAP_MAX_HEIGHT + 2)

#define TEXT_FIELD_MAX_LENGTH (TEXT_FIELD_MAX_WIDTH * (MAP_MAX_HEIGHT - INPUT_FIELD_MAX_HEIGHT - 1))
#define INPUT_FIELD_MAX_LENGTH (TEXT_FIELD_MAX_WIDTH * INPUT_FIELD_MAX_HEIGHT)

// in text field, use \\X to change look, where X is a char representing the look, defined in LOOK_TABLE
#define LOOK_TABLE                             \
    LOOK_ENTRY(RESET, '_', SHUAttribute_Reset) \
    LOOK_ENTRY(BOLD, '?', SHUAttribute_Bold, SHUAttribute_ColorFGRed)

#define LOOK_ENTRY(a, b, ...) LOOK_##a = b,
typedef enum LOOK_ID
{
    LOOK_TABLE
} LOOK_ID;
#undef LOOK_ENTRY

typedef struct DialogueChoice
{
    char text[TEXT_FIELD_MAX_WIDTH];
    int nextNodeIndex; // Which node to jump to (-1 to end)
    int requiredFlag;  // Condition to show this choice (-1 for none)
    int flagToSet;     // Provide an item/advance quest (-1 for none)
} DialogueChoice;

typedef struct DialogueNode
{
    char text[TEXT_FIELD_MAX_LENGTH];
    int requiredFlag;
    int numChoices;
    DialogueChoice choices[INPUT_FIELD_SELECTION_COUNT];
} DialogueNode;

typedef struct NPC
{
    int index;
    int x;
    int y;
    int numNodes;
    DialogueNode nodes[NPC_NODE_MAX_COUNT];
} NPC;

typedef struct Portal
{
    int targetMapIndex;
    int x;
    int y;
} Portal;

typedef struct Map
{
    int index;
    int playerStartX;
    int playerStartY;

    Portal portals[MAP_INTERACTABLE_MAX_COUNT];
    NPC npcs[MAP_INTERACTABLE_MAX_COUNT];

    char data[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
} Map;

typedef struct Portrait
{
    char data[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
} Portrait;

typedef struct Player
{
    char flags[PLAYER_MAX_FLAGS];
    int x;
    int y;
} Player;

typedef struct MapCharacterData
{
    int index;
    int x;
    int y;
} MapCharacterData;
