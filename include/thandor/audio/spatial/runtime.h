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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050B5D0 */
bool SpatialSoundPool_Init(void);

/* 0x0050B600 */
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 viewAngle1,AngleTurn32 viewAngle0,GraphicsWorldCoordinateQ12 originZ,
          GraphicsWorldCoordinateQ12 originY,GraphicsWorldCoordinateQ12 originX);

/* 0x0050B6E0 */
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_PlayPositionedOneShot
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef);

/* 0x0050B7D0 */
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_UpdateDesiredPositionedGains
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot);

/* 0x0050B8C0 */
SpatialSoundSlotEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset);

/* 0x0050B940 */
SpatialSoundSlot * SpatialSoundSlot_CreateFromPcm (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz, AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount, void *pcmData);

/* 0x0050B9D0 */
void __thandor_void_preserve_eax_ecx SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot);

/* 0x0050BA00 */
void __thandor_void_preserve_eax_ecx SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot);

/* 0x0050BA30 */
void __thandor_void_preserve_eax_ecx SpatialSoundPool_ClearDesiredGains(void);

/* 0x0050BA60 */
void __thandor_void_preserve_eax_ecx_edx SpatialSoundPool_ApplyDesiredGains(void);

#endif /* THANDOR_AUDIO_SPATIAL_RUNTIME_H */
