/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/spatial/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_SPATIAL_RUNTIME_H
#define THANDOR_AUDIO_SPATIAL_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/spatial/runtime. */
/* Capacity of g_SpatialSoundSlots (SpatialSoundPool_Init allocates 0x1000 bytes of 0x10-byte slots) */
#define SPATIAL_SOUND_SLOT_COUNT 0x100
/* Channel gains are Q15 (0x8000 = full volume, the clamp before playback); positioned sounds whose
   attenuated gain is not above 0x100 are not played (SpatialSound_PlayPositionedOneShot) */
#define SPATIAL_SOUND_GAIN_Q15_FULL 0x8000
#define SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 0x100
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050B5D0 */
StatusResult SpatialSoundPool_Init(void);

/* 0x0050B600 */
void SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 viewAngle1,AngleTurn32 viewAngle0,GraphicsWorldCoordinateQ12 originZ,
          GraphicsWorldCoordinateQ12 originY,GraphicsWorldCoordinateQ12 originX);

/* 0x0050B6E0 */
void SpatialSound_PlayPositionedOneShot(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef);

/* 0x0050B7D0 */
void SpatialSound_UpdateDesiredPositionedGains
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot);

/* 0x0050B8C0 */
SpatialSoundSlotResult SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset);

/* 0x0050B940 */
SpatialSoundSlotResult SpatialSoundSlot_CreateFromPcm (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz, AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount, void *pcmData);

/* 0x0050B9D0 */
void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot);

/* 0x0050BA00 */
void SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot);

/* 0x0050BA30 */
void SpatialSoundPool_ClearDesiredGains(void);

/* 0x0050BA60 */
void SpatialSoundPool_ApplyDesiredGains(void);

#endif /* THANDOR_AUDIO_SPATIAL_RUNTIME_H */
