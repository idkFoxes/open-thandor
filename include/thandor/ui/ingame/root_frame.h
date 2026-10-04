/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/root_frame.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_ROOT_FRAME_H
#define THANDOR_UI_INGAME_ROOT_FRAME_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/root_frame. */

/* Keyboard camera (InGameUiRoot_UpdateFrame): pitch and heading step per frame (angle16) and zoom step */
#define INGAME_CAMERA_KEY_ANGLE_STEP 0x400
#define INGAME_CAMERA_KEY_DISTANCE_STEP_Q12 0x800

/* Minimap zoom: minimapSampleScaleQ12 = high dword of committedDistanceQ12 * this, i.e. 3/32 of the camera
   distance (0.32 fixed point) */
#define INGAME_MINIMAP_DISTANCE_SCALE_Q32 0x6000000
/* Field overlay colour (ARGB8888, opaque mid grey) while an army waits for placement */
#define INGAME_PLACEMENT_OVERLAY_ARGB 0xFF808080
/* Ambient effect sounds and music: a random delay of 1..64 frames ((Random & mask) + 1) before the next one */
#define INGAME_AMBIENT_SOUND_DELAY_MASK 0x3F

/* Functions are grouped by semantic ownership. */

void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot);

void InGameRuntime_UpdateCursorGridAndViewScaleCache();

void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot);

extern SoundVoice *g_InGameActiveEffectVoice;
extern uint32_t g_InGameEffectsEnabled;
extern SoundVoice *g_InGameActiveMusicVoice;
extern uint32_t g_InGameMusicNextTrackCountdown;
extern uint16_t g_InGameCountdownTextUtf16[8];

extern UiRootCallbacks g_InGameUiRootCallbacks;

#endif /* THANDOR_UI_INGAME_ROOT_FRAME_H */
