#pragma once

#define STRING_MAX_SIZE 128

#define MAP_MAX_WIDTH 64
#define MAP_MAX_HEIGHT 32
#define MAP_DATA_MAX_SIZE (MAP_MAX_WIDTH * MAP_MAX_HEIGHT)

#define MAP_PORTALS_MAX_COUNT 4
#define MAP_MAX_COUNT 16

#define PORTRAIT_MAX_COUNT 8

// define how characters will look in the terminal
#define CHAR_TABLE                                                         \
    CHAR_ENTRY(BORDER_HORIZONTAL, '-', SHUAttribute_ColorFGBlue)           \
    CHAR_ENTRY(BORDER_VERTICAL, '|', SHUAttribute_ColorFGBlue)             \
    CHAR_ENTRY(PLAYER, '@', SHUAttribute_Bold, SHUAttribute_ColorFGYellow) \
    CHAR_ENTRY(WALL, '#', SHUAttribute_Bold, SHUAttribute_ColorFGGreen)

#define CHAR_ENTRY(a, b, ...) CHAR_##a = b,
typedef enum CHAR_ID
{
    CHAR_TABLE
} CHAR_ID;
#undef CHAR_ENTRY

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
