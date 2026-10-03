/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/data.h
 */

#ifndef THANDOR_AUDIO_BACKEND_DATA_H
#define THANDOR_AUDIO_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern SoundPlayVoiceProc *g_SoundPlayLooping; /* 0041734C g_SoundPlayLooping */

extern SoundStopVoiceProc *g_SoundStopVoice; /* 00417350 g_SoundStopVoice */

extern SoundIsVoicePlayingProc *g_SoundIsVoicePlaying; /* 00417358 g_SoundIsVoicePlaying */

extern SoundSetVoiceGainsProc *g_SoundSetVoiceGains; /* 00417360 g_SoundSetVoiceGains */

extern short g_SoundSampleCoefficientBlock[256]; /* 00417364 g_SoundSampleCoefficientBlock */

extern DirectSoundCreate *pDirectSoundCreate; /* 00573FC8 pDirectSoundCreate */

extern char dynapi_4[7]; /* 005744C2 dynapi_4 */

extern char dynapi_20[18]; /* 0057459A dynapi_20 */

extern IDirectSound *g_DirectSound; /* 00582EE0 g_DirectSound */

extern IDirectSoundBuffer *g_PrimarySoundBuffer; /* 00582EE4 g_PrimarySoundBuffer */

extern TH_LEGACY_LONG g_PrimaryVolume; /* 00582EE8 g_PrimaryVolume */

extern TH_LEGACY_LONG g_PrimaryPan; /* 00582EEC g_PrimaryPan */

extern WAVEFORMATEX WaveFormat_PCM_22050_Stereo16; /* 00582EF0 WaveFormat_PCM_22050_Stereo16 (20 bytes, followed by 12 bytes of NOP fill) */

extern DSBUFFERDESC_DX6 PrimarySoundBufferDesc; /* 00582F10 PrimarySoundBufferDesc */

extern DirectSoundVoiceSet **g_DirectSoundVoiceSetRegistry; /* 00582F24 g_DirectSoundVoiceSetRegistry */

extern int32_t g_DirectSoundGainAttenuation[129]; /* 00582F2E g_DirectSoundGainAttenuation: DirectSound attenuation (1/100 dB) per channel gain Q15 >> 8 */

#endif
