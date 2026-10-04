/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/null_backend.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_BACKEND_NULL_BACKEND_H
#define THANDOR_AUDIO_BACKEND_NULL_BACKEND_H

#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

uint32_t SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,SoundVoiceSet **outVoiceSet);

void SoundBackendDisabled_ReleaseSampleVoiceSet(SoundVoiceSet *voiceSet);

Bool8 SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          SoundVoiceSet *voiceSet,SoundVoice **outVoice);

Bool8 SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          SoundVoiceSet *voiceSet,SoundVoice **outVoice);

void SoundBackendDisabled_StopVoice(SoundVoice *voice);

Bool8 SoundBackendDisabled_IsVoiceFinished(SoundVoice *voice);

void SoundBackendDisabled_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          SoundVoice *voice);

void SoundBackendDisabled_StopAllVoices();

#endif /* THANDOR_AUDIO_BACKEND_NULL_BACKEND_H */
