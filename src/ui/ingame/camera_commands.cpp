/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/camera_commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/camera_commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_dispatch.h>

/* Module data. */

static const InGameCameraCommandDispatchTable g_InGameCameraCommandDispatchRecords16 = {
    .records = {
        /*  0 */ {.keyCode = EncodedDigit1, .action = InGameCameraKeyAction::RecallBookmark1},
        /*  1 */ {.keyCode = EncodedDigit2, .action = InGameCameraKeyAction::RecallBookmark2},
        /*  2 */ {.keyCode = EncodedDigit3, .action = InGameCameraKeyAction::RecallBookmark3},
        /*  3 */ {.keyCode = EncodedDigit4, .action = InGameCameraKeyAction::RecallBookmark4},
        /*  4 */ {.keyCode = EncodedDigit5, .action = InGameCameraKeyAction::RecallBookmark5},
        /*  5 */ {.keyCode = EncodedDigit6, .action = InGameCameraKeyAction::RecallBookmark6},
        /*  6 */ {.keyCode = EncodedDigit7, .action = InGameCameraKeyAction::RecallBookmark7},
        /*  7 */ {.keyCode = EncodedLowercaseS, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::ToggleShading},
        /*  8 */ {.keyCode = EncodedDigit1, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark1},
        /*  9 */ {.keyCode = EncodedDigit2, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark2},
        /* 10 */ {.keyCode = EncodedDigit3, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark3},
        /* 11 */ {.keyCode = EncodedDigit4, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark4},
        /* 12 */ {.keyCode = EncodedDigit5, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark5},
        /* 13 */ {.keyCode = EncodedDigit6, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark6},
        /* 14 */ {.keyCode = EncodedDigit7, .requiredModifierMask = 0x30, .action = InGameCameraKeyAction::StoreBookmark7},
        /* 15 */ {.keyCode = EncodedLowercaseC, .requiredModifierMask = 0xC, .action = InGameCameraKeyAction::ToggleUnlimitedCamera}
    },
    .alignmentPadding = {144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144}};

uint32_t g_LevelCameraBookmark1PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark1PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark1PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark1PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark2PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark2PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark2PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark2PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark3PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark3PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark3PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark3PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark4PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark4PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark4PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark4PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark5PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark5PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark5PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark5PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark6PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark6PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark6PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark6PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 = 0;

uint32_t g_LevelCameraBookmark7PositionXQ12 = 0;

uint32_t g_LevelCameraBookmark7PositionYQ12 = 0;

uint32_t g_LevelCameraBookmark7PositionZQ12 = 0;

uint32_t g_LevelCameraBookmark7PositionMagnitudeQ12 = 0;

uint32_t g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 = 0;

/* Camera key commands of the world view while the interaction subsystem is active (game paused): installed as
   the world view's dispatchCommandCallback by the activating path of
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState, in place of
   InGameUiRuntime_DispatchCommandByCodeAndModifierFlags. The first g_InGameCameraCommandDispatchRecords16 record
   with this key and a matching modifier selects the command:
     1..7      move the camera to level camera bookmark n
     Alt+1..7  store the current camera as bookmark n
     Alt+S     toggle WORLD_RUNTIME_FLAG_SHADING_ENABLED
     Ctrl+C    toggle WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA
   Returns true when no record matches and false after a command; the world view's pointer
   context (FrontendModelPointerContext_KeyboardEvent) passes unmatched keys on.
*/
Bool8 InGameCameraCommand_DispatchByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,WorldRuntimeContext *worldRuntime)

{
  InGameCameraCommandKeyCode recordKeyCode;
  uint32_t requiredModifiers;
  uint32_t bookmark1PackedAngles;
  uint32_t bookmark2PackedAngles;
  uint32_t bookmark3PackedAngles;
  uint32_t bookmark4PackedAngles;
  uint32_t bookmark5PackedAngles;
  uint32_t bookmark6PackedAngles;
  uint32_t bookmark7PackedAngles;
  const InGameCameraCommandDispatchRecord *currentRecord;
  uint32_t recordIndex;
  
  bookmark7PackedAngles = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
  bookmark6PackedAngles = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
  bookmark5PackedAngles = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
  bookmark4PackedAngles = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
  bookmark3PackedAngles = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
  bookmark2PackedAngles = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
  bookmark1PackedAngles = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
  /* First record with this key whose modifier requirement matches: a record without required modifiers only
     matches when neither Ctrl nor Alt is held (Shift is ignored). The key-code 0 record terminates the table;
     after all sixteen records the table's terminatorKeyCode (0) ends the walk. */
  for (recordIndex = 0; recordIndex < 16; recordIndex++) {
    currentRecord = &g_InGameCameraCommandDispatchRecords16.records[recordIndex];
    recordKeyCode = currentRecord->keyCode;
    requiredModifiers = currentRecord->requiredModifierMask;
    if (recordKeyCode == InGameCameraCommandKeyCode{}) break;
    if ((static_cast<UiActionId>(recordKeyCode) != commandCode) ||
        !UiKeyModifiers_Match(requiredModifiers,(uint32_t)modifierFlags,UiKeyModifierRule::AnyOfMask)) continue;
    /* Matching record: run its action and stop (the original jumped to a continuation address per record). */
    switch(currentRecord->action) {
    case InGameCameraKeyAction::RecallBookmark1: /* 1..7: recall bookmark n */
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
                 g_LevelCameraBookmark1PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark1PackedAngles >> 16,bookmark1PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark1PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark2:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
                 g_LevelCameraBookmark2PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark2PackedAngles >> 16,bookmark2PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark2PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark3:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
                 g_LevelCameraBookmark3PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark3PackedAngles >> 16,bookmark3PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark3PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark4:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
                 g_LevelCameraBookmark4PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark4PackedAngles >> 16,bookmark4PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark4PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark5:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
                 g_LevelCameraBookmark5PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark5PackedAngles >> 16,bookmark5PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark5PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark6:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
                 g_LevelCameraBookmark6PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark6PackedAngles >> 16,bookmark6PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark6PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::RecallBookmark7:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
                 g_LevelCameraBookmark7PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark7PackedAngles >> 16,bookmark7PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark7PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case InGameCameraKeyAction::StoreBookmark1: /* Alt+1..7: store the camera as bookmark n (heading low word, pitch high word) */
      g_LevelCameraBookmark1PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark1PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark1PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark1PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark2:
      g_LevelCameraBookmark2PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark2PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark2PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark2PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark3:
      g_LevelCameraBookmark3PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark3PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark3PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark3PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark4:
      g_LevelCameraBookmark4PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark4PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark4PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark4PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark5:
      g_LevelCameraBookmark5PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark5PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark5PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark5PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark6:
      g_LevelCameraBookmark6PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark6PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark6PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark6PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::StoreBookmark7:
      g_LevelCameraBookmark7PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark7PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark7PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark7PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case InGameCameraKeyAction::ToggleShading: /* Alt+S */
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_SHADING_ENABLED;
      break;
    case InGameCameraKeyAction::ToggleUnlimitedCamera: /* Ctrl+C */
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
    }
    return false;
  }
  return true; /* no camera key: the pointer context passes it on */
}
