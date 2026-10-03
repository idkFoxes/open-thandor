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

/* DirectSound voice sets: every loaded sound owns eight IDirectSoundBuffer voices (voices[0] is the
   buffer that holds the data, the others are DuplicateSoundBuffer copies made on demand), and all sets
   are listed in the 256-entry g_DirectSoundVoiceSetRegistry (DirectSound_Init allocates it). */
#define DIRECTSOUND_VOICES_PER_SET 8
#define DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY 256
/* SoundSampleAsset.formatVersion the DirectSound backend accepts (1.0 in 16.16) */
#define SOUND_SAMPLE_FORMAT_VERSION 0x10000
/* The .sam decoder produces blocks of 256 stereo 16-bit frames = 0x400 bytes (0x200 shorts). */
#define SOUND_SAMPLE_DECODED_BLOCK_BYTES 0x400
/* Stage numbers DirectSound_Create*VoiceSet leave in g_PackageLastErrorPath on failure */
#define DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER 100
#define DIRECTSOUND_VOICE_STAGE_LOCK 101
#define DIRECTSOUND_VOICE_STAGE_FILL 102
/* Memory_ZeroDwords size used for WaveFormat_PCM_22050_Stereo16: sizeof(WAVEFORMATEX), the 18 bytes of fields
   and the two bytes of tail padding */
#define DIRECTSOUND_WAVE_FORMAT_CLEAR_BYTES 0x14
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void DirectSound_Shutdown(void);

uint32_t SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet);

void SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

bool SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice);

bool SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice);

void SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice);

bool SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice);

void SoundBackendDisabled_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice);

uint32_t DirectSound_Init(void);

uint32_t DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet);

void DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);

bool DirectSound_PlayOneShot(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice);

bool DirectSound_PlayLooping(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice);

void DirectSound_StopVoice(IDirectSoundBuffer *voice);

bool DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice);

void DirectSound_StopAllVoices(void);

void DirectSound_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice);

void SoundBackendDisabled_StopAllVoices(void);

extern SoundPlayVoiceProc *g_SoundPlayOneShot;

#endif /* THANDOR_AUDIO_BACKEND_RUNTIME_H */
