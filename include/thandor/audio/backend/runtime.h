/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_BACKEND_RUNTIME_H
#define THANDOR_AUDIO_BACKEND_RUNTIME_H

#include <thandor/audio/backend/types.h>
#include <thandor/core/contracts.h>

/* Voice sets (DirectSoundVoiceSet, the original's layout, kept as the arena block the game holds): every loaded
   sound has up to eight voices, and all sets are listed in a 256-entry registry (SdlAudio_Init allocates it). */
#define DIRECTSOUND_VOICES_PER_SET 8
#define DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY 256
/* SoundSampleAsset.formatVersion the audio backend accepts (1.0 in 16.16) */
#define SOUND_SAMPLE_FORMAT_VERSION 0x10000
/* The .sam decoder produces blocks of 256 stereo 16-bit frames = 0x400 bytes (0x200 shorts). */
#define SOUND_SAMPLE_DECODED_BLOCK_BYTES 0x400
/* Stage numbers SdlAudio_CreateSampleVoiceSet leaves in g_PackageLastErrorPath on failure (the original's
   DirectSound stages) */
#define DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER 100
#define DIRECTSOUND_VOICE_STAGE_FILL 102

extern SoundPlayVoiceProc *g_SoundPlayOneShot;

extern SoundPlayVoiceProc *g_SoundPlayLooping;
extern SoundStopVoiceProc *g_SoundStopVoice;
extern SoundIsVoicePlayingProc *g_SoundIsVoicePlaying;
extern SoundSetVoiceGainsProc *g_SoundSetVoiceGains;

extern SoundReleaseSampleVoiceSetProc *g_SoundReleaseSampleVoiceSet;
extern SoundStopAllVoicesProc *g_SoundStopAllVoices;

extern SoundCreateSampleVoiceSetProc *g_SoundCreateSampleVoiceSet;

/* The original's DirectSound attenuation (1/100 dB) by channel gain Q15 >> 8, the SDL3 backend's gain curve */
extern const int32_t g_DirectSoundGainAttenuation[129];

#endif /* THANDOR_AUDIO_BACKEND_RUNTIME_H */
