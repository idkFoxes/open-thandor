#ifndef THANDOR_AUDIO_BACKEND_RUNTIME_H
#define THANDOR_AUDIO_BACKEND_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/backend/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00583410 */
void __cdecl DirectSound_Shutdown(void);

/* 0x00417570 */
DirectSoundVoiceSet * SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset);

/* 0x00417580 */
void SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00417590 */
DirectSoundVoiceSet * SoundBackendDisabled_CreatePcmVoiceSet (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz, AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount, void *pcmData);

/* 0x004175A0 */
void SoundBackendDisabled_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x004175B0 */
IDirectSoundBuffer * SoundBackendDisabled_PlayOneShot (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, DirectSoundVoiceSet *voiceSet);

/* 0x004175C0 */
IDirectSoundBuffer * SoundBackendDisabled_PlayLooping (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, DirectSoundVoiceSet *voiceSet);

/* 0x004175D0 */
void SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice);

/* 0x004175E0 */
void SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice);

/* 0x00417600 */
qword SoundBackendDisabled_QueryVoiceRegs(IDirectSoundBuffer *voice);

/* 0x00417610 */
void SoundBackendDisabled_SetVoiceGains (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, IDirectSoundBuffer *voice);

/* 0x00583140 */
dword __cdecl DirectSound_Init(void);

/* 0x00583490 */
DirectSoundVoiceSet * DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset);

/* 0x00583690 */
void DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00583720 */
DirectSoundVoiceSet * DirectSound_CreatePcmVoiceSet (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz, AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount, void *pcmData);

/* 0x005838D0 */
void DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet);

/* 0x00583940 */
IDirectSoundBuffer * DirectSound_PlayOneShot (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, DirectSoundVoiceSet *voiceSet);

/* 0x00583A70 */
IDirectSoundBuffer * DirectSound_PlayLooping (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, DirectSoundVoiceSet *voiceSet);

/* 0x00583B90 */
void DirectSound_StopVoice(IDirectSoundBuffer *voice);

/* 0x00583BC0 */
void DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice);

/* 0x00583C00 */
void __cdecl DirectSound_StopAllVoices(void);

/* 0x00583C60 */
qword DirectSound_QueryVoiceRegsStub(IDirectSoundBuffer *voice);

/* 0x00583C70 */
void DirectSound_SetVoiceGains (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15, IDirectSoundBuffer *voice);

/* 0x004175F0 */
void __cdecl SoundBackendDisabled_StopAllVoices(void);

#endif /* THANDOR_AUDIO_BACKEND_RUNTIME_H */
