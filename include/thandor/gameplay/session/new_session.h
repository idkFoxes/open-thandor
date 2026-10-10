/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/new_session.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_NEW_SESSION_H
#define THANDOR_GAMEPLAY_SESSION_NEW_SESSION_H

#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/core/contracts.h>
#include <thandor/core/math/fixed_point.h>

/* Timer rate of InGameRuntime_PeriodicCountdownAndClockTick (g_TimerRegisterPeriodic takes a frequency). */
inline constexpr int INGAME_PERIODIC_TIMER_HZ = 80;
/* Reload value of g_InGameNetworkTickCountdown: the periodic timer counts it down, and a simulation step only
   runs at zero, so the game advances at most 80 / 4 = 20 steps per second. */
inline constexpr int INGAME_TIMER_TICKS_PER_SIMULATION_STEP = 4;

/* World object pool of a session (InGameRuntime_InitializeNewSession, InGameRuntime_InitializeLoadedSession): records of
   sizeof(WorldObjectRecord) bytes, handed to WorldRuntime_AttachObjectArray */
inline constexpr int INGAME_WORLD_OBJECT_RECORD_COUNT = 0x4000;
inline constexpr int INGAME_WORLD_DWORD_ARRAY_COUNT = 256; /* g_InGameWorldRuntimeDwordArray256 */

/* Camera pitch clamp of a session (WorldRuntimeContext.motion, angle16 as unsigned dwords): -0x3C00 and -0x1800 */
inline constexpr uint32_t INGAME_CAMERA_MINIMUM_PITCH_ANGLE16 = 0xFFFFC400;
inline constexpr uint32_t INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16 = 0xFFFFE800;

/* Camera distance range of a session (WorldRuntimeContext.minimum/maximumCameraDistanceQ12, world units in Q12).
   The original allowed 8..19; widened (port) to 6..32 for high resolutions and wide screens. The camera is local
   view state only (no simulation, state hash or network command reads it). Limits found:
   - closer than 6 the camera cuts into the largest buildings (headquarters) at the shallowest pitch, because a
     model is only drawn while it lies fully in front of the near plane, and model textures get blocky;
   - 32 is the original's own alternate (debug) camera maximum (g_WorldMotionAlternateMaximumDistanceQ12). There
     the terrain pass of the largest map (gemezel, 131x131) queues about 3000-5000 of the 40960 primitive packets
     (measured up to distance 48); every queue append is bounded, the terrain row spans are sized by the grid,
     not the view, and the target ray (4 x maximum) stays far below FIELD_GRID_RAYCAST_MAX_STEPS. */
inline constexpr uint32_t INGAME_CAMERA_MINIMUM_DISTANCE_Q12 = 6 * Q12_ONE;
inline constexpr uint32_t INGAME_CAMERA_MAXIMUM_DISTANCE_Q12 = 32 * Q12_ONE;

bool InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                        uint32_t *outError);

extern uint32_t g_EndMovieVariantIndex;
extern uint16_t *g_EndMoviePath;
extern uint32_t g_HostCommandBatchSyncSentThisInterval;

extern uint32_t g_EndGameResultsCurrentMusicTrackId;
extern WorldObjectRecord *g_InGameWorldObjectRecords;
extern uintptr_t g_InGameWorldRuntimeDwordArray256[256];

#endif /* THANDOR_GAMEPLAY_SESSION_NEW_SESSION_H */
