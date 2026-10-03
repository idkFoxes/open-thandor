/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/input/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/gameplay/input/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(8)) uint32_t g_KeyboardStateMask = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark1PositionXQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark1PositionYQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark1PositionZQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark1PositionMagnitudeQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark2PositionXQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark2PositionYQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark2PositionZQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark2PositionMagnitudeQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark3PositionXQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark3PositionYQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark3PositionZQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark3PositionMagnitudeQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark4PositionXQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark4PositionYQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark4PositionZQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark4PositionMagnitudeQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark5PositionXQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark5PositionYQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark5PositionZQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark5PositionMagnitudeQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark6PositionXQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark6PositionYQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark6PositionZQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark6PositionMagnitudeQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark7PositionXQ12 = 0;

__declspec(align(16)) uint32_t g_LevelCameraBookmark7PositionYQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark7PositionZQ12 = 0;

__declspec(align(8)) uint32_t g_LevelCameraBookmark7PositionMagnitudeQ12 = 0;

__declspec(align(4)) uint32_t g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 = 0;

__declspec(align(4)) InGameCommandPayloadTripletValue32 g_InGameSelectionInsertTripletDwords[12] = {0};

__declspec(align(4)) InGameCommandPayloadTripletValue32 g_InGameSelectionRemoveTripletDwords[12] = {0};

__declspec(align(4)) uint32_t g_InGamePlacementHeading16 = 0;

__declspec(align(8)) uint32_t g_InGamePlacementPointerCaptureX = 0;

__declspec(align(4)) uint32_t g_InGamePlacementPointerCaptureY = 0;

__declspec(align(8)) uint32_t g_InGamePlacementWorldYQ12 = 0;

__declspec(align(4)) uint32_t g_InGamePlacementWorldXQ12 = 0;

__declspec(align(8)) uint32_t g_InGameCommandPreviewHeading16 = 0;

__declspec(align(4)) uint32_t g_InGameCommandPreviewWorldYQ12 = 0;

__declspec(align(16)) uint32_t g_InGameCommandPreviewWorldXQ12 = 0;

__declspec(align(4)) uint32_t g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = 0;

__declspec(align(8)) uint32_t g_InGameCommandPointerCaptureX = 0;

__declspec(align(4)) uint32_t g_InGameCommandPointerCaptureY = 0;

__declspec(align(4)) uint32_t g_InGamePointerInteractionStateFlags = 0;

/* uint32_t[8]: command id per pointer mode (modifier mask & variant mask); gameplay/input/world.c */
__declspec(align(8)) uint32_t g_InGamePointerModeCommandIds[8] = {26, 38, 39, 40, 41, 42, 43, 44};

__declspec(align(16)) InGameCameraCommandDispatchTable g_InGameCameraCommandDispatchRecords16 = {
    .records = {
        /*  0 */ {.keyCode = EncodedDigit1, .continuationEntryAddress = 0x56F360},
        /*  1 */ {.keyCode = EncodedDigit2, .continuationEntryAddress = 0x56F3B0},
        /*  2 */ {.keyCode = EncodedDigit3, .continuationEntryAddress = 0x56F400},
        /*  3 */ {.keyCode = EncodedDigit4, .continuationEntryAddress = 0x56F450},
        /*  4 */ {.keyCode = EncodedDigit5, .continuationEntryAddress = 0x56F4A0},
        /*  5 */ {.keyCode = EncodedDigit6, .continuationEntryAddress = 0x56F4F0},
        /*  6 */ {.keyCode = EncodedDigit7, .continuationEntryAddress = 0x56F540},
        /*  7 */ {.keyCode = EncodedLowercaseS, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F7D0},
        /*  8 */ {.keyCode = EncodedDigit1, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F590},
        /*  9 */ {.keyCode = EncodedDigit2, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F5E0},
        /* 10 */ {.keyCode = EncodedDigit3, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F630},
        /* 11 */ {.keyCode = EncodedDigit4, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F680},
        /* 12 */ {.keyCode = EncodedDigit5, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F6D0},
        /* 13 */ {.keyCode = EncodedDigit6, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F720},
        /* 14 */ {.keyCode = EncodedDigit7, .requiredModifierMask = 0x30, .continuationEntryAddress = 0x56F770},
        /* 15 */ {.keyCode = EncodedLowercaseC, .requiredModifierMask = 0xC, .continuationEntryAddress = 0x56F7E0}
    },
    .alignmentPadding = {144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144}};
