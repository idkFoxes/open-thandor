/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/data.h
 */

#ifndef THANDOR_AUDIO_BACKEND_DATA_H
#define THANDOR_AUDIO_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern SoundPlayVoiceProc *g_SoundPlayLooping;

extern SoundStopVoiceProc *g_SoundStopVoice;

extern SoundIsVoicePlayingProc *g_SoundIsVoicePlaying;

extern SoundSetVoiceGainsProc *g_SoundSetVoiceGains;

extern short g_SoundSampleCoefficientBlock[256];

extern DirectSoundCreate *pDirectSoundCreate;

extern char dynapi_4[7];

extern char dynapi_20[18];

extern IDirectSound *g_DirectSound;

extern IDirectSoundBuffer *g_PrimarySoundBuffer;

extern TH_LEGACY_LONG g_PrimaryVolume;

extern TH_LEGACY_LONG g_PrimaryPan;

extern WAVEFORMATEX WaveFormat_PCM_22050_Stereo16; /* (20 bytes, followed by 12 bytes of NOP fill) */

extern DSBUFFERDESC_DX6 PrimarySoundBufferDesc;

extern DirectSoundVoiceSet **g_DirectSoundVoiceSetRegistry;

extern int32_t g_DirectSoundGainAttenuation[129]; /* DirectSound attenuation (1/100 dB) per channel gain Q15 >> 8 */

#endif
