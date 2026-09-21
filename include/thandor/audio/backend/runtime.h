/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_BACKEND_RUNTIME_H
#define THANDOR_AUDIO_BACKEND_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/backend/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00583410 */
void __thandor_void_preserve_eax_ecx_edx DirectSound_Shutdown(void);

/* 0x00417570 */
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset);

/* 0x00417580 */
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00417590 */
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData);

/* 0x004175A0 */
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x004175B0 */
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet);

/* 0x004175C0 */
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet);

/* 0x004175D0 */
void __thandor_void_preserve_eax_ecx_edx SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice);

/* 0x004175E0 */
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice);

/* 0x00417600 */
qword SoundBackendDisabled_QueryVoiceRegs(IDirectSoundBuffer *voice);

/* 0x00417610 */
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_SetVoiceGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice);

/* 0x00583140 */
bool __thandor_void_preserve_ecx_edx DirectSound_Init(void);

/* 0x00583490 */
SoundCreateSampleVoiceSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset);

/* 0x00583690 */
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00583720 */
SoundCreatePcmVoiceSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          dword *pcmData);

/* 0x005838D0 */
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00583940 */
SoundPlayVoiceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet);

/* 0x00583A70 */
SoundPlayVoiceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet);

/* 0x00583B90 */
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopVoice(IDirectSoundBuffer *voice);

/* 0x00583BC0 */
bool __thandor_cf_preserve_eax_ecx_edx DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice);

/* 0x00583C00 */
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopAllVoices(void);

/* 0x00583C60 */
qword DirectSound_QueryVoiceRegsStub(IDirectSoundBuffer *voice);

/* 0x00583C70 */
void __thandor_void_preserve_eax_ecx_edx
DirectSound_SetVoiceGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice);

/* 0x004175F0 */
void __thandor_void_preserve_eax_ecx_edx SoundBackendDisabled_StopAllVoices(void);

#endif /* THANDOR_AUDIO_BACKEND_RUNTIME_H */
