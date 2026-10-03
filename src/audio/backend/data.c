/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/audio/backend/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 00417340 g_SoundCreatePcmVoiceSet */
__declspec(align(16)) SoundCreatePcmVoiceSetProc *g_SoundCreatePcmVoiceSet = (void *)SoundBackendDisabled_CreatePcmVoiceSet;

/* 00417344 g_SoundReleasePcmVoiceSet */
__declspec(align(4)) SoundReleasePcmVoiceSetProc *g_SoundReleasePcmVoiceSet = (void *)SoundBackendDisabled_ReleasePcmVoiceSet;

/* 0041734C g_SoundPlayLooping */
__declspec(align(4)) SoundPlayVoiceProc *g_SoundPlayLooping = (void *)SoundBackendDisabled_PlayLooping;

/* 00417350 g_SoundStopVoice */
__declspec(align(16)) SoundStopVoiceProc *g_SoundStopVoice = (void *)SoundBackendDisabled_StopVoice;

/* 00417358 g_SoundIsVoicePlaying */
__declspec(align(8)) SoundIsVoicePlayingProc *g_SoundIsVoicePlaying = (void *)SoundBackendDisabled_IsVoicePlaying;

/* 0041735C g_SoundQueryVoice */
__declspec(align(4)) SoundQueryVoiceRegsProc *g_SoundQueryVoice = (void *)SoundBackendDisabled_QueryVoice;

/* 00417360 g_SoundSetVoiceGains */
__declspec(align(16)) SoundSetVoiceGainsProc *g_SoundSetVoiceGains = (void *)SoundBackendDisabled_SetVoiceGains;

/* 00417364 g_SoundSampleCoefficientBlock */
__declspec(align(4)) short g_SoundSampleCoefficientBlock[256] = {0};

/* 00573FC8 pDirectSoundCreate */
__declspec(align(8)) DirectSoundCreate *pDirectSoundCreate = 0;

/* 00573FCC pDirectSoundEnumerateA */
__declspec(align(4)) DirectSoundEnumerateA *pDirectSoundEnumerateA = 0;

/* 00573FD0 pDirectSoundCaptureCreate */
__declspec(align(16)) DirectSoundCaptureCreate *pDirectSoundCaptureCreate = 0;

/* 00573FD4 pDirectSoundCaptureEnumerateA */
__declspec(align(4)) DirectSoundCaptureEnumerateA *pDirectSoundCaptureEnumerateA = 0;

/* 005744C2 dynapi_4 */
__declspec(align(4)) char dynapi_4[7] = "DSOUND";

/* 0057459A dynapi_20 */
__declspec(align(4)) char dynapi_20[18] = "DirectSoundCreate";

/* 005745AC dynapi_21 */
__declspec(align(4)) char dynapi_21[22] = "DirectSoundEnumerateA";

/* 005745C2 dynapi_22 */
__declspec(align(4)) char dynapi_22[25] = "DirectSoundCaptureCreate";

/* 005745DC dynapi_23 */
__declspec(align(4)) char dynapi_23[29] = "DirectSoundCaptureEnumerateA";

/* 00582EE0 g_DirectSound */
__declspec(align(16)) IDirectSound *g_DirectSound = 0;

/* 00582EE4 g_PrimarySoundBuffer */
__declspec(align(4)) IDirectSoundBuffer *g_PrimarySoundBuffer = 0;

/* 00582EE8 g_PrimaryVolume */
__declspec(align(8)) TH_LEGACY_LONG g_PrimaryVolume = 0;

/* 00582EEC g_PrimaryPan */
__declspec(align(4)) TH_LEGACY_LONG g_PrimaryPan = 0;

/* 00582EF0 WaveFormat_PCM_22050_Stereo16 */
__declspec(align(16)) WAVEFORMATEX WaveFormat_PCM_22050_Stereo16 = {0};

/* 00582F10 PrimarySoundBufferDesc */
__declspec(align(16)) DSBUFFERDESC_DX6 PrimarySoundBufferDesc = {0};

/* 00582F24 g_DirectSoundVoiceSetRegistry */
__declspec(align(4)) DirectSoundVoiceSet **g_DirectSoundVoiceSetRegistry = 0;

/* 00582F2E g_DirectSoundGainAttenuation: indexed by channel gain Q15 >> 8 (0..128; the spatial gains are
   clamped to SPATIAL_SOUND_GAIN_Q15_FULL); [128] = 0. 14 bytes of NOP fill followed in the original. */
__declspec(align(4)) int32_t g_DirectSoundGainAttenuation[129] = {
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
