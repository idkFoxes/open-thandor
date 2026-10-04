/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/camera_commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/camera_commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static const InGameCameraCommandDispatchTable g_InGameCameraCommandDispatchRecords16 = {
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

/* Implementation ownership: ui/ingame/camera_commands. */

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
  const InGameCameraCommandDispatchTable *currentRecord;
  const InGameCameraCommandDispatchTable *nextRecord;
  
  bookmark7PackedAngles = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
  bookmark6PackedAngles = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
  bookmark5PackedAngles = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
  bookmark4PackedAngles = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
  bookmark3PackedAngles = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
  bookmark2PackedAngles = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
  bookmark1PackedAngles = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
  /* First record with this key whose modifier requirement matches: a record without required modifiers only
     matches when neither Ctrl nor Alt is held (Shift is ignored). The key-code 0 record terminates the table. */
  nextRecord = &g_InGameCameraCommandDispatchRecords16;
  for (currentRecord = nextRecord;
       recordKeyCode = currentRecord->records[0].keyCode,
       requiredModifiers = currentRecord->records[0].requiredModifierMask, recordKeyCode != 0;
       currentRecord = nextRecord) {
    nextRecord = (const InGameCameraCommandDispatchTable *)(currentRecord->records + 1);
    if ((recordKeyCode != commandCode) ||
        !((requiredModifiers == 0) ? ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0)
                                   : ((modifierFlags & requiredModifiers) != 0))) continue;
    /* Matching record: run its command and stop. The original jumps to the record's continuation address; the
       cases are those addresses. */
    switch(currentRecord->records[0].continuationEntryAddress) {
    case 0x56f360: /* 1..7: recall bookmark n */
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
    case 0x56f3b0:
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
    case 0x56f400:
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
    case 0x56f450:
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
    case 0x56f4a0:
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
    case 0x56f4f0:
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
    case 0x56f540:
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
    case 0x56f590: /* Alt+1..7: store the camera as bookmark n (heading low word, pitch high word) */
      g_LevelCameraBookmark1PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark1PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark1PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark1PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f5e0:
      g_LevelCameraBookmark2PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark2PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark2PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark2PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f630:
      g_LevelCameraBookmark3PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark3PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark3PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark3PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f680:
      g_LevelCameraBookmark4PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark4PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark4PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark4PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f6d0:
      g_LevelCameraBookmark5PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark5PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark5PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark5PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f720:
      g_LevelCameraBookmark6PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark6PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark6PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark6PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f770:
      g_LevelCameraBookmark7PositionXQ12 = (worldRuntime->motion).positionXQ12;
      g_LevelCameraBookmark7PositionYQ12 = (worldRuntime->motion).positionYQ12;
      g_LevelCameraBookmark7PositionZQ12 = (worldRuntime->motion).positionZQ12;
      g_LevelCameraBookmark7PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
      g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
           (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
      break;
    case 0x56f7d0: /* Alt+S */
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_SHADING_ENABLED;
      break;
    case 0x56f7e0: /* Ctrl+C */
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
    }
    return false;
  }
  return true; /* no camera key: the pointer context passes it on */
}
