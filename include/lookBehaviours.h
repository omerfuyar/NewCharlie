#pragma once

// in text and input field, use \X to change look, where X is a char representing the look, defined in LOOK_TABLE
#define LOOK_TABLE                                                                                 \
    LOOK_ENTRY(RESET, '_', SHUAttribute_Reset)                                                     \
    LOOK_ENTRY(IMPORTANT, '!', SHUAttribute_Bold, SHUAttribute_ColorFGRed, SHUAttribute_Underline) \
    LOOK_ENTRY(MYSTERIOUS, '?', SHUAttribute_Italic, SHUAttribute_ColorFGBlue)                     \
    LOOK_ENTRY(ACTION, '*', SHUAttribute_Italic, SHUAttribute_ColorFGMagenta)                      \
    LOOK_ENTRY(ROBOT, 'R', SHUAttribute_Bold, SHUAttribute_ColorFGCyan)                            \
    LOOK_ENTRY(PLAYER, 'P', SHUAttribute_ColorFGGreen)

#define LOOK_ENTRY(a, b, ...) LOOK_##a = b,
typedef enum LOOK_ID
{
    LOOK_TABLE
} LOOK_ID;
#undef LOOK_ENTRY

void setAttributesForLook(LOOK_ID look);
