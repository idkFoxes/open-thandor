/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/backend/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

__declspec(align(8)) SoundCreateSampleVoiceSetProc *g_SoundCreateSampleVoiceSet = (void *)SoundBackendDisabled_CreateSampleVoiceSet;

__declspec(align(4)) SoundReleaseSampleVoiceSetProc *g_SoundReleaseSampleVoiceSet = (void *)SoundBackendDisabled_ReleaseSampleVoiceSet;

__declspec(align(4)) SoundStopAllVoicesProc *g_SoundStopAllVoices = (void *)SoundBackendDisabled_StopAllVoices;

static short g_SoundSampleCoefficientBlock[256] = {0};

static DirectSoundCreate *pDirectSoundCreate = 0;

static char dynapi_4[7] = "DSOUND";

static char dynapi_20[18] = "DirectSoundCreate";

static IDirectSound *g_DirectSound = 0;

static IDirectSoundBuffer *g_PrimarySoundBuffer = 0;

static TH_LEGACY_LONG g_PrimaryVolume = 0;

static TH_LEGACY_LONG g_PrimaryPan = 0;

/* 20 bytes (12 bytes of NOP fill followed in the original) */
static WAVEFORMATEX WaveFormat_PCM_22050_Stereo16 = {0};

static DSBUFFERDESC_DX6 PrimarySoundBufferDesc = {0};

static DirectSoundVoiceSet **g_DirectSoundVoiceSetRegistry = 0;

/* DirectSound attenuation (1/100 dB) indexed by channel gain Q15 >> 8 (0..128; the spatial gains are
   clamped to SPATIAL_SOUND_GAIN_Q15_FULL); [128] = 0. 14 bytes of NOP fill followed in the original. */
static const int32_t g_DirectSoundGainAttenuation[129] = {
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

SoundPlayVoiceProc *g_SoundPlayLooping = (void *)SoundBackendDisabled_PlayLooping;

SoundStopVoiceProc *g_SoundStopVoice = (void *)SoundBackendDisabled_StopVoice;

SoundIsVoicePlayingProc *g_SoundIsVoicePlaying = (void *)SoundBackendDisabled_IsVoicePlaying;

SoundSetVoiceGainsProc *g_SoundSetVoiceGains = (void *)SoundBackendDisabled_SetVoiceGains;

SoundPlayVoiceProc *g_SoundPlayOneShot = (void *)SoundBackendDisabled_PlayOneShot;

/* Implementation ownership: audio/backend/runtime. */

/* Shuts DirectSound down: restores the primary buffer's volume and pan saved by DirectSound_Init,
   releases the primary buffer and the device, and frees the voice-set registry.
*/
void DirectSound_Shutdown(void)

{
  /* The original calls the silent stub here, not DirectSound_StopAllVoices, so playing voices are not
     stopped. */
  SoundBackendDisabled_StopAllVoices();
  if (g_PrimarySoundBuffer != NULL) {
    g_PrimarySoundBuffer->lpVtbl->SetVolume(g_PrimarySoundBuffer,g_PrimaryVolume);
    g_PrimarySoundBuffer->lpVtbl->SetPan(g_PrimarySoundBuffer,g_PrimaryPan);
    g_PrimarySoundBuffer->lpVtbl->Release(g_PrimarySoundBuffer);
    g_PrimarySoundBuffer = NULL;
  }
  if (g_DirectSound != NULL) {
    g_DirectSound->lpVtbl->Release(g_DirectSound);
    g_DirectSound = NULL;
  }
  g_MemoryApi.free(g_DirectSoundVoiceSetRegistry);
  g_DirectSoundVoiceSetRegistry = NULL;
  return;
}


/* Silent-backend stub in slot g_SoundCreateSampleVoiceSet (until DirectSound_Init switches the slots to
   DirectSound). Always succeeds (returns 0) with the dummy voice set 0xFFFFFFFF in
   *outVoiceSet, so callers holding a sample keep a non-NULL handle even without sound.
*/
uint32_t SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet)

{
  *outVoiceSet = (DirectSoundVoiceSet *)(intptr_t)-1;
  return 0;
}


/* Silent-backend stub in slot g_SoundReleaseSampleVoiceSet: nothing to release.
*/
void SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}


/* Silent-backend stub in slot g_SoundPlayOneShot: plays nothing and reports success (returns true)
   with a NULL voice in *outVoice; the original never writes the voice, so its callers store an unrelated
   leftover value of their own as the voice (e.g. a random effect index or a music gain). Those handles only ever go back to the silent stubs (the backend is chosen once at startup)
   or through a NULL test before one (Movie_Rewind), so NULL here behaves the same.
*/
bool SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  if (outVoice != NULL) {
    *outVoice = NULL;
  }
  return true;
}


/* Silent-backend stub in slot g_SoundPlayLooping: plays nothing and reports success (returns true)
   with a NULL voice in *outVoice; the original never writes the voice (callers store an unrelated leftover
   value instead: a music gain, or the non-zero gain). As for
   SoundBackendDisabled_PlayOneShot those values only reach the silent stubs again, so NULL behaves the same
   (the spatial pool merely calls this stub again instead of the gain stub on the next frame).
*/
bool SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  if (outVoice != NULL) {
    *outVoice = NULL;
  }
  return true;
}


/* Silent-backend stub in slot g_SoundStopVoice: nothing plays, so nothing to stop.
*/
void SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice)

{
  return;
}


/* Silent-backend stub in slot g_SoundIsVoicePlaying: always returns true, meaning the voice is not
   playing.
*/
bool SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  return true;
}


/* Silent-backend stub in slot g_SoundSetVoiceGains: ignores the new left/right gains.
*/
void SoundBackendDisabled_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  return;
}


/* Resolves the DSOUND.DLL export DirectSound_Init uses; returns 0 or the DynAPI_Resolve error. */
static uint32_t DirectSound_ResolveExports(HINSTANCE module)
{
  /* dynapi_20: "DirectSoundCreate" */
  return DynAPI_Resolve((void **)&pDirectSoundCreate,module,dynapi_20);
}

/* DirectSound_Init's device setup after DirectSoundCreate: exclusive cooperative level, then the primary
   buffer (create, 22050 Hz 16-bit stereo format, save volume/pan, full volume, centre pan, looping play).
   Returns 0 or the failing step's HRESULT; *passedStages receives the number of steps that succeeded
   before the failing one (the stage number in DirectSound_Init's error message). */
static TH_LEGACY_HRESULT DirectSound_StartPrimaryBuffer(int32_t *passedStages)
{
  TH_LEGACY_HRESULT directSoundResult;

  *passedStages = 0;
  directSoundResult =
       g_DirectSound->lpVtbl->SetCooperativeLevel(g_DirectSound,g_MainWindow,DSSCL_EXCLUSIVE);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 1;
  Memory_ZeroDwords(DIRECTSOUND_WAVE_FORMAT_CLEAR_BYTES,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(sizeof PrimarySoundBufferDesc,&PrimarySoundBufferDesc);
  WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
  WaveFormat_PCM_22050_Stereo16.nChannels = 2;
  WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = 22050;
  WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4; /* 2 channels * 2 bytes */
  WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 88200; /* 22050 * 4 */
  WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 16;
  PrimarySoundBufferDesc.dwSize = sizeof PrimarySoundBufferDesc;
  PrimarySoundBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
  directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                    (g_DirectSound,&PrimarySoundBufferDesc,&g_PrimarySoundBuffer,NULL);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 2;
  directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetFormat
                    (g_PrimarySoundBuffer,&WaveFormat_PCM_22050_Stereo16);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 3;
  directSoundResult = g_PrimarySoundBuffer->lpVtbl->GetVolume(g_PrimarySoundBuffer,&g_PrimaryVolume);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 4;
  directSoundResult = g_PrimarySoundBuffer->lpVtbl->GetPan(g_PrimarySoundBuffer,&g_PrimaryPan);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 5;
  directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetVolume(g_PrimarySoundBuffer,DSBVOLUME_MAX);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 6;
  directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetPan(g_PrimarySoundBuffer,DSBPAN_CENTER);
  if (directSoundResult != 0) {
    return directSoundResult;
  }
  *passedStages = 7;
  return g_PrimarySoundBuffer->lpVtbl->Play(g_PrimarySoundBuffer,0,0,DSBPLAY_LOOPING);
}

/* Binds DSOUND.DLL, opens the default DirectSound device in exclusive mode and starts the looping primary
   buffer as 22050 Hz 16-bit stereo, then allocates the 256-entry voice-set registry and switches the
   g_Sound* backend slots from the silent stubs to DirectSound. Without a sound device it succeeds and
   leaves the silent backend in place. Returns 0 on success, otherwise the error code: FATAL_ERROR_DLL_LOAD_FAILED
   or the DynAPI_Resolve error when DSOUND.DLL cannot be bound, the allocator's error for the registry, or
   FATAL_ERROR_DIRECTSOUND_SETUP for a failing setup step.
*/
uint32_t DirectSound_Init(void)

{
  HINSTANCE module;
  TH_LEGACY_HRESULT directSoundResult;
  int registryIndex;
  DirectSoundVoiceSet **registry;
  uint32_t resolveError;
  uint32_t registryAllocError;
  void *registryPayload;
  int32_t failedStage; /* number of setup steps passed, shown in the error message */

  module = DynDLL_Load(dynapi_4); /* "DSOUND" */
  if (module == NULL) {
    Thandor_Log("DirectSound_Init: DSOUND.DLL or an export could not be resolved");
    return FATAL_ERROR_DLL_LOAD_FAILED;
  }
  resolveError = DirectSound_ResolveExports(module);
  if (resolveError != 0) {
    Thandor_Log("DirectSound_Init: DSOUND.DLL or an export could not be resolved");
    return resolveError;
  }

  directSoundResult = pDirectSoundCreate(NULL,&g_DirectSound,NULL);
  Thandor_Log("DirectSoundCreate -> 0x%08X", (uint32_t)directSoundResult);
  if (directSoundResult != 0) {
    /* no DirectSound device: not an error, the game runs silent (the original returns the HRESULT as a
       success; no caller reads it) */
    return 0;
  }

  directSoundResult = DirectSound_StartPrimaryBuffer(&failedStage);
  if (directSoundResult != 0) {
    Thandor_Log("DirectSound_Init failed at stage %d, HRESULT 0x%08X", failedStage,
                (uint32_t)directSoundResult);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
    return FATAL_ERROR_DIRECTSOUND_SETUP;
  }

  registryAllocError = g_MemoryApi.alloc(DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY *
                                         sizeof(DirectSoundVoiceSet *),&registryPayload);
  if (registryAllocError != 0) {
    return registryAllocError;
  }
  registry = (DirectSoundVoiceSet **)registryPayload;
  g_DirectSoundVoiceSetRegistry = registry;
  for (registryIndex = 0; registryIndex < DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryIndex++) {
    registry[registryIndex] = NULL;
  }
  g_SoundCreateSampleVoiceSet = DirectSound_CreateSampleVoiceSet;
  g_SoundReleaseSampleVoiceSet = DirectSound_ReleaseSampleVoiceSet;
  g_SoundPlayOneShot = DirectSound_PlayOneShot;
  g_SoundPlayLooping = DirectSound_PlayLooping;
  g_SoundStopVoice = DirectSound_StopVoice;
  g_SoundStopAllVoices = DirectSound_StopAllVoices;
  g_SoundIsVoicePlaying = DirectSound_IsVoicePlaying;
  g_SoundSetVoiceGains = DirectSound_SetVoiceGains;
  CosineDerivedLookupTables_Init();
  return 0;
}


/* Failure exit of DirectSound_CreateSampleVoiceSet: releases
   the secondary buffer if one was created, writes the failing stage number to g_PackageLastErrorPath
   and returns errorCode. */
static uint32_t DirectSound_FailVoiceSet(IDirectSoundBuffer *soundBuffer,int32_t failedStage,uint32_t errorCode)
{
  if (soundBuffer != NULL) {
    soundBuffer->lpVtbl->Release(soundBuffer);
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  return errorCode;
}

/* Success tail of DirectSound_CreateSampleVoiceSet: clears the
   eight voices of the freshly allocated set, puts soundBuffer into voices[0] and registers the set in the
   first free registry slot (a full or missing registry is not an error). */
static void DirectSound_InitAndRegisterVoiceSet(DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer *soundBuffer)
{
  int voiceIndex;
  int registryIndex;
  DirectSoundVoiceSet **registry;

  for (voiceIndex = 0; voiceIndex < DIRECTSOUND_VOICES_PER_SET; voiceIndex++) {
    voiceSet->voices[voiceIndex] = NULL;
  }
  registry = g_DirectSoundVoiceSetRegistry;
  voiceSet->voices[0] = soundBuffer;
  if (registry != NULL) {
    for (registryIndex = 0; registryIndex < DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryIndex++) {
      if (registry[registryIndex] == NULL) {
        registry[registryIndex] = voiceSet;
        break;
      }
    }
  }
}

/* Turns a .sam sound asset into a voice set: checks the 0x200-byte header, creates a 22050 Hz 16-bit
   stereo secondary buffer of decodedBlockCount * 0x400 bytes, decodes every packed block into it and
   registers a new eight-voice set holding the buffer in voices[0]. Returns 0 and stores the set in
   *outVoiceSet; on failure returns the error code (FATAL_ERROR_SOUND_SAMPLE_INVALID,
   FATAL_ERROR_DIRECTSOUND_SETUP or the allocator's error), leaves *outVoiceSet untouched and leaves the
   failing stage number in g_PackageLastErrorPath.
*/
uint32_t DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet)

{
  TH_LEGACY_HRESULT directSoundResult;
  uint32_t encodedBlockSize;
  uint32_t remainingBlocks;
  SoundSampleAsset *encodedBlock;
  short *outputStereoPcm;
  uint32_t voiceSetAllocError;
  void *voiceSetPayload;
  DirectSoundVoiceSet *voiceSet;
  TH_LEGACY_DWORD wrapByteCount;
  TH_LEGACY_LPVOID wrapRegion;
  uint32_t lockedByteCount;
  short *lockedPcm;
  IDirectSoundBuffer *soundBuffer;

  soundBuffer = NULL;
  Memory_ZeroDwords(DIRECTSOUND_WAVE_FORMAT_CLEAR_BYTES,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(sizeof PrimarySoundBufferDesc,&PrimarySoundBufferDesc);
  if ((sampleAsset->magic != ASSET_MAGIC_SAM) || (sampleAsset->formatVersion != SOUND_SAMPLE_FORMAT_VERSION)) {
    return DirectSound_FailVoiceSet(soundBuffer,DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER,
                                    FATAL_ERROR_SOUND_SAMPLE_INVALID);
  }
  WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
  WaveFormat_PCM_22050_Stereo16.nChannels = 2;
  /* the original writes only the low word; the high word was zeroed above, so the dword is 22050 */
  WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = 22050;
  WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 88200; /* 22050 * 4 */
  WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4; /* 2 channels * 2 bytes */
  WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 16;
  PrimarySoundBufferDesc.dwBufferBytes = sampleAsset->decodedBlockCount * SOUND_SAMPLE_DECODED_BLOCK_BYTES;
  PrimarySoundBufferDesc.dwSize = sizeof PrimarySoundBufferDesc;
  PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
  PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
  directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                    (g_DirectSound,&PrimarySoundBufferDesc,&soundBuffer,NULL);
  if (directSoundResult != 0) {
    return DirectSound_FailVoiceSet(soundBuffer,DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER,
                                    FATAL_ERROR_DIRECTSOUND_SETUP);
  }
  directSoundResult = soundBuffer->lpVtbl->Lock
                    (soundBuffer,0,0,&lockedPcm,&lockedByteCount,&wrapRegion,&wrapByteCount,
                     DSBLOCK_ENTIREBUFFER);
  if (directSoundResult != 0) {
    return DirectSound_FailVoiceSet(soundBuffer,DIRECTSOUND_VOICE_STAGE_LOCK,FATAL_ERROR_DIRECTSOUND_SETUP);
  }
  encodedBlock = sampleAsset + 1; /* the packed blocks follow the 0x200-byte header */
  remainingBlocks = lockedByteCount / SOUND_SAMPLE_DECODED_BLOCK_BYTES;
  outputStereoPcm = lockedPcm;
  /* Original quirk: the loop body runs before the count is tested, so a buffer smaller than one decoded
     block (count 0) still decodes once and then wraps the count around. */
  do {
    encodedBlockSize =
         SoundSample_DecodePackedCoefficientBlock(g_SoundSampleCoefficientBlock,(uint8_t *)encodedBlock);
    SoundSample_DecodeCoefficientBlockToPcmMmx(outputStereoPcm,g_SoundSampleCoefficientBlock);
    encodedBlock = (SoundSampleAsset *)((uint8_t *)encodedBlock + encodedBlockSize);
    outputStereoPcm = outputStereoPcm + SOUND_SAMPLE_DECODED_BLOCK_BYTES / sizeof(short);
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  directSoundResult =
       soundBuffer->lpVtbl->Unlock(soundBuffer,lockedPcm,lockedByteCount,wrapRegion,wrapByteCount);
  if (directSoundResult != 0) {
    return DirectSound_FailVoiceSet(soundBuffer,DIRECTSOUND_VOICE_STAGE_FILL,FATAL_ERROR_DIRECTSOUND_SETUP);
  }
  voiceSetAllocError = g_MemoryApi.alloc(sizeof(DirectSoundVoiceSet),&voiceSetPayload);
  if (voiceSetAllocError != 0) {
    return DirectSound_FailVoiceSet(soundBuffer,DIRECTSOUND_VOICE_STAGE_FILL,voiceSetAllocError);
  }
  voiceSet = (DirectSoundVoiceSet *)voiceSetPayload;
  DirectSound_InitAndRegisterVoiceSet(voiceSet,soundBuffer);
  *outVoiceSet = voiceSet;
  return 0;
}


/* Frees a voice set made by DirectSound_CreateSampleVoiceSet: releases its eight voices (the data
   buffer and its duplicates), frees the set and clears its registry slot. NULL is accepted.
*/
void DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  int voiceIndex;
  int registryIndex;
  DirectSoundVoiceSet **registry;
  IDirectSoundBuffer *voiceBuffer;

  if (voiceSet != NULL) {
    for (voiceIndex = 0; voiceIndex < DIRECTSOUND_VOICES_PER_SET; voiceIndex++) {
      voiceBuffer = voiceSet->voices[voiceIndex];
      if (voiceBuffer != NULL) {
        voiceBuffer->lpVtbl->Release(voiceBuffer);
      }
    }
    g_MemoryApi.free(voiceSet);
    /* the original's loop counter holds the registry pointer on the first pass (a NULL registry searches
       nothing) and the remaining count afterwards */
    registry = g_DirectSoundVoiceSetRegistry;
    if (registry != NULL) {
      for (registryIndex = 0; registryIndex < DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryIndex++) {
        if (voiceSet == registry[registryIndex]) {
          registry[registryIndex] = NULL;
          return;
        }
      }
    }
  }
  return;
}


/* Shared tail of the play functions and DirectSound_SetVoiceGains. The attenuation of the louder channel is
   the volume and left - right attenuation is the pan.
*/
static void DirectSound_ApplyChannelGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)
{
  int32_t leftAttenuation = g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8];
  int32_t rightAttenuation = g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
  voice->lpVtbl->SetVolume
            (voice,rightChannelGainQ15 < leftChannelGainQ15 ? leftAttenuation : rightAttenuation);
  voice->lpVtbl->SetPan(voice,leftAttenuation - rightAttenuation);
}

/* Shared body of DirectSound_PlayOneShot/DirectSound_PlayLooping: the first idle voice of the set plays; an empty slot is filled with DuplicateSoundBuffer of
   voice 0 rewound to position 0; with all eight voices busy it fails (false, NULL voice). */
static bool DirectSound_PlayVoiceSet
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,TH_LEGACY_DWORD playFlags,IDirectSoundBuffer **outVoice)
{
  IDirectSoundBuffer *voice;
  TH_LEGACY_DWORD status;
  int slot;

  if (outVoice != NULL) {
    *outVoice = NULL;
  }
  if (voiceSet == NULL) {
    return false;
  }
  for (slot = 0; slot < DIRECTSOUND_VOICES_PER_SET; slot++) {
    voice = voiceSet->voices[slot];
    if (voice == NULL) {
      if (g_DirectSound->lpVtbl->DuplicateSoundBuffer
                    (g_DirectSound,voiceSet->voices[0],&voiceSet->voices[slot]) != 0) {
        return false;
      }
      voice = voiceSet->voices[slot];
      voice->lpVtbl->SetCurrentPosition(voice,0);
      break;
    }
    status = 0;
    voice->lpVtbl->GetStatus(voice,&status);
    if ((status & DSBSTATUS_PLAYING) == 0) {
      break;
    }
  }
  if (slot == DIRECTSOUND_VOICES_PER_SET) {
    return false;
  }
  voice->lpVtbl->Play(voice,0,0,playFlags);
  DirectSound_ApplyChannelGains(leftChannelGainQ15,rightChannelGainQ15,voice);
  if (outVoice != NULL) {
    *outVoice = voice;
  }
  return true;
}


/* Plays a sound once on the first idle voice of the set (duplicating voices[0] into an empty slot when
   needed) and sets its volume/pan from the two 0..0x8000 channel gains via the 129-entry attenuation
   table. Returns true and the voice in *outVoice (outVoice may be NULL); false with a NULL voice when the
   set is NULL, all eight voices are busy or the duplication fails.
*/
bool DirectSound_PlayOneShot(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,0,outVoice);
}


/* Like DirectSound_PlayOneShot, but the voice plays with DSBPLAY_LOOPING until it is stopped.
*/
bool DirectSound_PlayLooping(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,DSBPLAY_LOOPING,outVoice);
}


/* Stops one voice (a buffer returned by the play functions); NULL is accepted.
*/
void DirectSound_StopVoice(IDirectSoundBuffer *voice)

{
  if (voice != NULL) {
    voice->lpVtbl->Stop(voice);
  }
  return;
}


/* Tells whether a voice is still playing. The result is inverted like all failure flags here: false means
   playing, true means NULL or stopped.
*/
bool DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  bool notPlaying;
  TH_LEGACY_DWORD voiceStatusFlags;

  notPlaying = true;
  if (voice != NULL) {
    voice->lpVtbl->GetStatus(voice,&voiceStatusFlags);
    notPlaying = (voiceStatusFlags & DSBSTATUS_PLAYING) == 0;
  }
  return notPlaying;
}


/* Stops every voice of every voice set in the registry (empty registry slots and voices are skipped).
*/
void DirectSound_StopAllVoices(void)

{
  IDirectSoundBuffer *voiceBuffer;
  DirectSoundVoiceSet **registry;
  DirectSoundVoiceSet *voiceSet;
  int registryIndex;
  int voiceIndex;

  /* The original's loop counters hold the registry pointer and the set's voices pointer on the first pass
     and the remaining counts afterwards: a NULL registry stops nothing, and since voices is at offset 0 an
     empty registry slot skips its set. */
  registry = g_DirectSoundVoiceSetRegistry;
  if (registry == NULL) {
    return;
  }
  for (registryIndex = 0; registryIndex < DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryIndex++) {
    voiceSet = registry[registryIndex];
    if (voiceSet == NULL) {
      continue;
    }
    for (voiceIndex = 0; voiceIndex < DIRECTSOUND_VOICES_PER_SET; voiceIndex++) {
      voiceBuffer = voiceSet->voices[voiceIndex];
      if (voiceBuffer != NULL) {
        voiceBuffer->lpVtbl->Stop(voiceBuffer);
      }
    }
  }
  return;
}


/* Updates the volume and pan of a playing voice from two new channel gains, with the same conversion as
   the play functions (louder channel's attenuation = volume, attenuation difference = pan). NULL is
   accepted.
*/
void DirectSound_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  if (voice != NULL) {
    DirectSound_ApplyChannelGains(leftChannelGainQ15,rightChannelGainQ15,voice);
  }
}


/* Stop-all entry of the disabled sound backend (the initial value of g_SoundStopAllVoices until
   DirectSound_Init installs DirectSound_StopAllVoices): there are no voices, so it does nothing.
*/
void SoundBackendDisabled_StopAllVoices(void)

{
}

