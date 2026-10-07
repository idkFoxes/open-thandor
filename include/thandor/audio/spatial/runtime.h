/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/spatial/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_SPATIAL_RUNTIME_H
#define THANDOR_AUDIO_SPATIAL_RUNTIME_H

#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Capacity of g_SpatialSoundSlots (SpatialSoundPool_Init allocates SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot)) */
inline constexpr int SPATIAL_SOUND_SLOT_COUNT = 0x100;
/* Channel gains are Q15 (0x8000 = full volume, the clamp before playback); positioned sounds whose
   attenuated gain is not above 0x100 are not played (SpatialSound_PlayPositionedOneShot) */
inline constexpr int SPATIAL_SOUND_GAIN_Q15_FULL = 0x8000;
inline constexpr int SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 = 0x100;

Bool8 SpatialSoundPool_Init(uint32_t *outError);

void SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 viewAngle1,AngleTurn32 viewAngle0,GraphicsWorldCoordinateQ12 originZ,
          GraphicsWorldCoordinateQ12 originY,GraphicsWorldCoordinateQ12 originX);

void SpatialSound_PlayPositionedOneShot(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SoundVoiceSet **voiceSetRef);

void SpatialSound_UpdateDesiredPositionedGains
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot);

SpatialSoundSlot *SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset);

void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot);

void SpatialSoundPool_ClearDesiredGains();

void SpatialSoundPool_ApplyDesiredGains();

extern AudioMixerGainQ15 g_SoundEffectsGainQ15;

extern int32_t g_ReverseStereoMask;

#endif /* THANDOR_AUDIO_SPATIAL_RUNTIME_H */
