#pragma once

// in text and input field, use \X to change look, where X is a char representing the look, defined in LOOK_TABLE
#define LOOK_TABLE                             \
    LOOK_ENTRY(RESET, '_', SHUAttribute_Reset) \
    LOOK_ENTRY(BOLD, '?', SHUAttribute_Bold, SHUAttribute_ColorFGRed)

#define LOOK_ENTRY(a, b, ...) LOOK_##a = b,
typedef enum LOOK_ID
{
    LOOK_TABLE
} LOOK_ID;
#undef LOOK_ENTRY

void setAttributesForLook(LOOK_ID look);
