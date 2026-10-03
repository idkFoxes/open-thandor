/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_RUNTIME_H
#define THANDOR_GAMEPLAY_SESSION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/runtime. */

/* Timer rate of InGameRuntime_PeriodicCountdownAndClockTick (TimerSystem_RegisterPeriodic takes a frequency). */
#define INGAME_PERIODIC_TIMER_HZ 80
/* Reload value of g_InGameNetworkTickCountdown: the periodic timer counts it down, and a simulation step only
   runs at zero, so the game advances at most 80 / 4 = 20 steps per second. */
#define INGAME_TIMER_TICKS_PER_SIMULATION_STEP 4
/* Command code (InGameCommandQueue_AppendLocalPlayerCommand) with which a player reports its level as loaded;
   single player calls its handler FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus directly. */
#define INGAME_COMMAND_PLAYER_READY 0x550
/* Command codes the hotkeys queue in network games (InGameHotkeys_DispatchCommandByFlags); a local game calls the
   handler directly. */
#define INGAME_COMMAND_TOGGLE_PAUSE 0x370 /* InGameCommand_TogglePauseRequest */
#define INGAME_COMMAND_ADJUST_GAME_SPEED 0x3F0 /* InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks */
/* Level script (InGameLevelConditionStorage.schedule): 64 condition records of 16 bytes and 16 end
   triggers of 8 bytes, evaluated by InGameConditionRuntime_UpdateScheduledRecords. */
#define INGAME_SCHEDULED_CONDITION_COUNT 64
#define INGAME_END_CONDITION_TRIGGER_COUNT 16
/* Tokens of a BOOLEAN_POSTFIX_EXPRESSION condition (the bytes after its kind byte); any other byte pushes the
   satisfied bit of the condition with that index onto the bit stack. */
#define INGAME_CONDITION_TOKEN_END 0xFC
#define INGAME_CONDITION_TOKEN_NOT 0xFD
#define INGAME_CONDITION_TOKEN_AND 0xFE
#define INGAME_CONDITION_TOKEN_OR 0xFF
/* World object pool of a session (InGameRuntime_InitializeNewSession, InGameRuntime_InitializeLoadedSession): records of
   sizeof(WorldObjectRecord) bytes, handed to WorldRuntime_AttachObjectArray */
#define INGAME_WORLD_OBJECT_RECORD_COUNT 0x4000
#define INGAME_WORLD_DWORD_ARRAY_COUNT 256 /* g_InGameWorldRuntimeDwordArray256 */
/* Keyboard camera (InGameUiRoot_UpdateFrame): pitch and heading step per frame (angle16) and zoom step */
#define INGAME_CAMERA_KEY_ANGLE_STEP 0x400
#define INGAME_CAMERA_KEY_DISTANCE_STEP_Q12 0x800
/* Camera pitch clamp of a session (WorldRuntimeContext.motion, angle16 as unsigned dwords): -0x3C00 and -0x1800 */
#define INGAME_CAMERA_MINIMUM_PITCH_ANGLE16 0xFFFFC400
#define INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16 0xFFFFE800
/* Minimap zoom: minimapSampleScaleQ12 = high dword of committedDistanceQ12 * this, i.e. 3/32 of the camera
   distance (0.32 fixed point) */
#define INGAME_MINIMAP_DISTANCE_SCALE_Q32 0x6000000
/* Field overlay colour (ARGB8888, opaque mid grey) while an army waits for placement */
#define INGAME_PLACEMENT_OVERLAY_ARGB 0xFF808080
/* Ambient effect sounds and music: a random delay of 1..64 frames ((Random & mask) + 1) before the next one */
#define INGAME_AMBIENT_SOUND_DELAY_MASK 0x3F
/* Faction statistics table sampling (InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState):
   row simulationTick >> 7 is written while these tick bits are clear (8 of every 128 steps) */
#define INGAME_STAT_SAMPLE_TICK_MASK 0x78
/* Reduced update (paused world): the grid refresh runs on even ticks with these bits clear, alternating the
   influence bands and the classification masks by tick bit 1 */
#define INGAME_REDUCED_GRID_REFRESH_TICK_MASK 0xC
/* FieldGridCell.resourceExtractionDescriptor (ArmyRuntime claim, collected into g_TerrainRegionCollectionEntries):
   support bit | faction << 13 | claimedCellTag (share) << 24 */
#define RESOURCE_EXTRACTION_FACTION_SHIFT 13
#define RESOURCE_EXTRACTION_FACTION_MASK 0x7FF /* after the shift: bits 13..23 */
#define RESOURCE_EXTRACTION_SHARE_SHIFT 24
/* Functions are grouped by semantic ownership. */

Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError);

void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot);

void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void);

Bool8 InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot);

void InGameRuntime_ProcessQueuedSessionNotificationTimer(void);

Bool8 InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                        uint32_t *outError);

Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError);

void InGameRuntime_ShutdownAndReleaseResources(void);

void InGameRuntime_ReleaseFactionScratchBuffers(void);

void InGameConditionRuntime_UpdateScheduledRecords(void);

void InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void);

void InGameRuntime_UpdateCursorGridAndViewScaleCache(void);

void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot);

void InGameRuntime_UpdateSimulationAndNetworkTick(void);

uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uint32_t unusedArgument);

extern InGameSimulationStepBatchTicks g_InGameSimulationStepTicks;

extern uint16_t g_SessionEndMoviePathUtf16[17];
extern uint16_t g_FlmEnde0001FlmPathUtf16[17];
extern uint32_t g_SessionNetworkTickCounter;
extern TerrainRegionCollectionCount g_TerrainRegionCollectionStoredCount;
extern TerrainRegionCollectionCount g_TerrainRegionCollectionVisitedCount;
extern uint32_t g_TerrainRegionCollectionEntries;
extern SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks;
extern uint32_t g_InGameActiveEffectVoice;
extern uint32_t g_InGameEffectsEnabled;
extern uint32_t g_InGameActiveMusicVoice;
extern uint32_t g_InGameMusicNextTrackCountdown;
extern uint16_t g_InGameCountdownTextUtf16[8];
extern uint32_t g_InGameNetworkTickCountdown;
extern uint32_t g_InGameStateTickSpinLock;
extern uint32_t g_EndMovieVariantIndex;
extern uint16_t *g_EndMoviePath;

extern InGameRuntimeRoot *g_InGameRuntimeRoot;

extern InGameUiImage g_InGameRuntimeDefaultImageTemplate;

extern UiRootCallbacks g_InGameUiRootCallbacks;

extern SessionNetworkRoleFlags g_SessionNetworkRoleFlags;
extern uint32_t g_SessionNetworkTickInterval; /* uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */

#endif /* THANDOR_GAMEPLAY_SESSION_RUNTIME_H */
