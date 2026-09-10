#ifndef THANDOR_AUDIO_SPATIAL_RUNTIME_H
#define THANDOR_AUDIO_SPATIAL_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/spatial/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050B5D0 */
void SpatialSoundPool_Init(void);

/* 0x0050B600 */
undefined8 SpatialSound_RebuildListenerTransformFromPose (AngleTurn32 param_1,int param_2,int param_3,int param_4,int param_5);

/* 0x0050B6E0 */
undefined8 SpatialSound_PlayPositionedOneShot (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15, GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef);

/* 0x0050B7D0 */
void SpatialSound_UpdateDesiredPositionedGains (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15, GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot);

/* 0x0050B8C0 */
SpatialSoundSlot * SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset);

/* 0x0050B940 */
SpatialSoundSlot * SpatialSoundSlot_CreateFromPcm (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz, AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount, void *pcmData);

/* 0x0050B9D0 */
void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot);

/* 0x0050BA00 */
void SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot);

/* 0x0050BA30 */
void SpatialSoundPool_ClearDesiredGains(void);

/* 0x0050BA60 */
void SpatialSoundPool_ApplyDesiredGains(void);

#endif /* THANDOR_AUDIO_SPATIAL_RUNTIME_H */
