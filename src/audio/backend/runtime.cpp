/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/backend/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* The sound slots (g_Sound*) the game plays through, with the silent backend that fills them until SdlAudio_Init
   installs the SDL3 audio backend. The original's DirectSound backend is replaced by platform/sdl3/audio.cpp. */

/* Module data. */

SoundCreateSampleVoiceSetProc *g_SoundCreateSampleVoiceSet = &SoundBackendDisabled_CreateSampleVoiceSet;

SoundReleaseSampleVoiceSetProc *g_SoundReleaseSampleVoiceSet = &SoundBackendDisabled_ReleaseSampleVoiceSet;

SoundStopAllVoicesProc *g_SoundStopAllVoices = &SoundBackendDisabled_StopAllVoices;

/* The original's DirectSound attenuation (1/100 dB), which the SDL3 mixer turns into linear channel gains
   (platform/sdl3/audio.cpp), indexed by channel gain Q15 >> 8 (0..128; the spatial gains are
   clamped to SPATIAL_SOUND_GAIN_Q15_FULL); [128] = 0. 14 bytes of NOP fill followed in the original. */
const int32_t g_DirectSoundGainAttenuation[129] = {
        /*   0 */ -10000, -7000, -6000, -5415, -5000, -4678, -4415, -4193,
        /*   8 */ -4000, -3830, -3678, -3540, -3415, -3300, -3193, -3093,
        /*  16 */ -3000, -2913, -2830, -2752, -2678, -2608, -2541, -2476,
        /*  24 */ -2415, -2356, -2300, -2245, -2193, -2142, -2093, -2046,
        /*  32 */ -2000, -1956, -1913, -1871, -1830, -1791, -1752, -1715,
        /*  40 */ -1678, -1642, -1608, -1574, -1541, -1508, -1476, -1445,
        /*  48 */ -1415, -1385, -1356, -1328, -1300, -1272, -1245, -1219,
        /*  56 */ -1193, -1167, -1142, -1117, -1093, -1069, -1046, -1023,
        /*  64 */ -1000, -978, -956, -934, -913, -891, -871, -850,
        /*  72 */ -830, -810, -791, -771, -752, -733, -715, -696,
        /*  80 */ -678, -660, -642, -625, -608, -591, -574, -557,
        /*  88 */ -541, -524, -508, -492, -476, -461, -445, -430,
        /*  96 */ -415, -400, -385, -371, -356, -342, -328, -314,
        /* 104 */ -300, -286, -272, -259, -245, -232, -219, -206,
        /* 112 */ -193, -180, -167, -154, -142, -130, -117, -105,
        /* 120 */ -93, -81, -69, -57, -46, -34, -23, -11,
        /* 128 */ 0,
};

SoundPlayVoiceProc *g_SoundPlayLooping = &SoundBackendDisabled_PlayLooping;

SoundStopVoiceProc *g_SoundStopVoice = &SoundBackendDisabled_StopVoice;

SoundIsVoicePlayingProc *g_SoundIsVoicePlaying = &SoundBackendDisabled_IsVoicePlaying;

SoundSetVoiceGainsProc *g_SoundSetVoiceGains = &SoundBackendDisabled_SetVoiceGains;

SoundPlayVoiceProc *g_SoundPlayOneShot = &SoundBackendDisabled_PlayOneShot;
