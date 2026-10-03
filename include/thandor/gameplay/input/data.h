/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/input/data.h
 */

#ifndef THANDOR_GAMEPLAY_INPUT_DATA_H
#define THANDOR_GAMEPLAY_INPUT_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_KeyboardStateMask;

extern uint32_t g_LevelCameraBookmark1PositionXQ12;

extern uint32_t g_LevelCameraBookmark1PositionYQ12;

extern uint32_t g_LevelCameraBookmark1PositionZQ12;

extern uint32_t g_LevelCameraBookmark1PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark2PositionXQ12;

extern uint32_t g_LevelCameraBookmark2PositionYQ12;

extern uint32_t g_LevelCameraBookmark2PositionZQ12;

extern uint32_t g_LevelCameraBookmark2PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark3PositionXQ12;

extern uint32_t g_LevelCameraBookmark3PositionYQ12;

extern uint32_t g_LevelCameraBookmark3PositionZQ12;

extern uint32_t g_LevelCameraBookmark3PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark4PositionXQ12;

extern uint32_t g_LevelCameraBookmark4PositionYQ12;

extern uint32_t g_LevelCameraBookmark4PositionZQ12;

extern uint32_t g_LevelCameraBookmark4PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark5PositionXQ12;

extern uint32_t g_LevelCameraBookmark5PositionYQ12;

extern uint32_t g_LevelCameraBookmark5PositionZQ12;

extern uint32_t g_LevelCameraBookmark5PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark6PositionXQ12;

extern uint32_t g_LevelCameraBookmark6PositionYQ12;

extern uint32_t g_LevelCameraBookmark6PositionZQ12;

extern uint32_t g_LevelCameraBookmark6PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;

extern uint32_t g_LevelCameraBookmark7PositionXQ12;

extern uint32_t g_LevelCameraBookmark7PositionYQ12;

extern uint32_t g_LevelCameraBookmark7PositionZQ12;

extern uint32_t g_LevelCameraBookmark7PositionMagnitudeQ12;

extern uint32_t g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;

extern InGameCommandPayloadTripletValue32 g_InGameSelectionInsertTripletDwords[12]; /* drag-selection insert batch, four triplets */

extern InGameCommandPayloadTripletValue32 g_InGameSelectionRemoveTripletDwords[12]; /* drag-selection remove batch, four triplets */

extern uint32_t g_InGamePlacementHeading16;

extern uint32_t g_InGamePlacementPointerCaptureX;

extern uint32_t g_InGamePlacementPointerCaptureY;

extern uint32_t g_InGamePlacementWorldYQ12;

extern uint32_t g_InGamePlacementWorldXQ12;

extern uint32_t g_InGameCommandPreviewHeading16;

extern uint32_t g_InGameCommandPreviewWorldYQ12;

extern uint32_t g_InGameCommandPreviewWorldXQ12;

extern uint32_t g_InGameCommandPreviewSurfaceHeightQ12OrSentinel;

extern uint32_t g_InGameCommandPointerCaptureX;

extern uint32_t g_InGameCommandPointerCaptureY;

extern uint32_t g_InGamePointerInteractionStateFlags;

extern uint32_t g_InGamePointerModeCommandIds[8]; /* uint32_t[8]: command id per pointer mode (modifier mask & variant mask); gameplay/input/world.c */

extern InGameCameraCommandDispatchTable g_InGameCameraCommandDispatchRecords16;

#endif
