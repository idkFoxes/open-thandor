/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/backend/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: audio/backend/runtime. */

/* Address: 0x00583410.
   Shuts DirectSound down: restores the primary buffer's volume and pan saved by DirectSound_Init,
   releases the primary buffer and the device, and frees the voice-set registry.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_Shutdown(void)

{
  /* The original calls the silent stub here (CALL 0x004175F0), not DirectSound_StopAllVoices, so
     playing voices are not stopped. */
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


/* Address: 0x00417570.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend placeholder. Returns pointer value 0xFFFFFFFF with CF clear and consumes one
   SoundSampleAsset argument.
*/
SampleVoiceSetResult __thandor_eax_cf_preserve_ecx_edx
SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  /* Returned through SoundCreateSampleVoiceSetProc, so it must use that {EAX, CF} result type. */
  SampleVoiceSetResult result;
  result.voiceSet = (DirectSoundVoiceSet *)0xffffffff;
  result.failed = false;
  return result;
}


/* Address: 0x00417580.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend no-op release. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}


/* Address: 0x00417590.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend placeholder. Returns pointer value 0xFFFFFFFF with CF clear and consumes the five raw-
   PCM arguments.
*/
PcmVoiceSetResult __thandor_eax_cf_preserve_ecx_edx
SoundBackendDisabled_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  PcmVoiceSetResult result; /* slot type SoundCreatePcmVoiceSetProc */
  result.voiceSet = (DirectSoundVoiceSet *)0xffffffff;
  result.failed = false;
  return result;
}


/* Address: 0x004175A0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend no-op release. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}


/* Address: 0x004175B0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend one-shot placeholder. It only clears CF and leaves EAX unchanged.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  SoundPlayResult result; /* slot type SoundPlayVoiceProc; EAX is left unchanged */
  memset(&result, 0, sizeof result);
  return result;
}


/* Address: 0x004175C0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend looping placeholder. It only clears CF and leaves EAX unchanged.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  SoundPlayResult result; /* slot type SoundPlayVoiceProc; EAX is left unchanged */
  memset(&result, 0, sizeof result);
  return result;
}


/* Address: 0x004175D0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend stop no-op. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice)

{
  return;
}


/* Address: 0x004175E0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend playing query. It always sets CF, meaning not playing.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  return true;
}


/* Address: 0x00417600.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend query placeholder. Returns EDX:EAX equal to zero.
*/
uint64_t SoundBackendDisabled_QueryVoiceRegs(IDirectSoundBuffer *voice)

{
  return 0;
}

/* Address: 0x00417610.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend gain-update no-op.
*/
void __thandor_void_preserve_eax_ecx_edx
SoundBackendDisabled_SetVoiceGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  return;
}


/* Address: 0x00583140.
   Binds DSOUND.DLL, opens the default DirectSound device in exclusive mode and starts the looping primary
   buffer as 22050 Hz 16-bit stereo, then allocates the 256-entry voice-set registry and switches the
   g_Sound* backend slots from the silent stubs to DirectSound. Without a sound device it succeeds and
   leaves the silent backend in place; a failing setup step reports FATAL_ERROR_DIRECTSOUND_SETUP.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx DirectSound_Init(void)

{
  HINSTANCE module;
  TH_LEGACY_HRESULT directSoundResult;
  int remainingCount;
  DirectSoundVoiceSet **registryCursor;
  DllLoadResult dllLoadResult;
  DynApiResolveResult resolveResult;
  ArenaAllocResult registryAlloc;
  int32_t failedStage; /* number of setup steps passed, shown in the error message */

  failedStage = 0;
  dllLoadResult = DynDLL_Load(dynapi_4);
  module = dllLoadResult.moduleOrError;
  if ((((!dllLoadResult.failed) &&
       (resolveResult = DynAPI_Resolve(&pDirectSoundCreate,module,dynapi_20), !resolveResult.failed)) &&
      (resolveResult = DynAPI_Resolve(&pDirectSoundEnumerateA,module,dynapi_21), !resolveResult.failed)) &&
     ((resolveResult = DynAPI_Resolve(&pDirectSoundCaptureCreate,module,dynapi_22), !resolveResult.failed &&
      (resolveResult = DynAPI_Resolve(&pDirectSoundCaptureEnumerateA,module,dynapi_23), !resolveResult.failed)))) {
    directSoundResult = pDirectSoundCreate(NULL,&g_DirectSound,NULL);
    Thandor_Log("DirectSoundCreate -> 0x%08X", (uint32_t)directSoundResult);
    if (directSoundResult != 0) {
      /* no DirectSound device: not an error, the game runs silent */
      return StatusValue_Ok((uint32_t)directSoundResult);
    }
    directSoundResult = g_DirectSound->lpVtbl->SetCooperativeLevel(g_DirectSound,g_MainWindow,DSSCL_EXCLUSIVE);
    if (directSoundResult == 0) {
      failedStage = 1;
      /* 0x14 bytes: the packed 18-byte WAVEFORMATEX and the two bytes behind it */
      Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
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
      if (directSoundResult == 0) {
        failedStage = 2;
        directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetFormat
                          (g_PrimarySoundBuffer,&WaveFormat_PCM_22050_Stereo16);
        if (directSoundResult == 0) {
          failedStage = 3;
          directSoundResult = g_PrimarySoundBuffer->lpVtbl->GetVolume(g_PrimarySoundBuffer,&g_PrimaryVolume);
          if (directSoundResult == 0) {
            failedStage = 4;
            directSoundResult = g_PrimarySoundBuffer->lpVtbl->GetPan(g_PrimarySoundBuffer,&g_PrimaryPan);
            if (directSoundResult == 0) {
              failedStage = 5;
              directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetVolume(g_PrimarySoundBuffer,DSBVOLUME_MAX);
              if (directSoundResult == 0) {
                failedStage = 6;
                directSoundResult = g_PrimarySoundBuffer->lpVtbl->SetPan(g_PrimarySoundBuffer,DSBPAN_CENTER);
                if (directSoundResult == 0) {
                  failedStage = 7;
                  directSoundResult = g_PrimarySoundBuffer->lpVtbl->Play(g_PrimarySoundBuffer,0,0,DSBPLAY_LOOPING);
                  if (directSoundResult == 0) {
                    registryAlloc = g_MemoryApi.alloc(0x400); /* 256 voice-set pointers */
                    if (!registryAlloc.failed) {
                      registryCursor = (DirectSoundVoiceSet **)registryAlloc.payloadOrError;
                      g_DirectSoundVoiceSetRegistry = (DirectSoundVoiceSet **)registryAlloc.payloadOrError;
                      for (remainingCount = 256; remainingCount != 0; remainingCount--) {
                        *registryCursor = NULL;
                        registryCursor++;
                      }
                      g_SoundCreateSampleVoiceSet = DirectSound_CreateSampleVoiceSet;
                      g_SoundReleaseSampleVoiceSet = DirectSound_ReleaseSampleVoiceSet;
                      g_SoundCreatePcmVoiceSet = DirectSound_CreatePcmVoiceSet;
                      g_SoundReleasePcmVoiceSet = DirectSound_ReleasePcmVoiceSet;
                      g_SoundPlayOneShot = DirectSound_PlayOneShot;
                      g_SoundPlayLooping = DirectSound_PlayLooping;
                      g_SoundStopVoice = DirectSound_StopVoice;
                      g_SoundStopAllVoices = DirectSound_StopAllVoices;
                      g_SoundIsVoicePlaying = DirectSound_IsVoicePlaying;
                      g_SoundQueryVoiceRegs = DirectSound_QueryVoiceRegsStub;
                      g_SoundSetVoiceGains = DirectSound_SetVoiceGains;
                      CosineDerivedLookupTables_Init();
                      return StatusValue_Ok(0);
                    }
                    return StatusValue_Fail(registryAlloc.payloadOrError);
                  }
                }
              }
            }
          }
        }
      }
    }
    Thandor_Log("DirectSound_Init failed at stage %d, HRESULT 0x%08X", failedStage, (uint32_t)directSoundResult);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
    return StatusValue_Fail(FATAL_ERROR_DIRECTSOUND_SETUP);
  }
  Thandor_Log("DirectSound_Init: DSOUND.DLL or an export could not be resolved");
  return StatusValue_Fail(dllLoadResult.failed ? (uint32_t)dllLoadResult.moduleOrError
                                              : (uint32_t)resolveResult.procedureOrError);
}


/* Address: 0x00583490.
   Turns a .sam sound asset into a voice set: checks the 0x200-byte header, creates a 22050 Hz 16-bit
   stereo secondary buffer of decodedBlockCount * 0x400 bytes, decodes every packed block into it and
   registers a new eight-voice set holding the buffer in voices[0]. On failure CF is set, EAX holds the
   error code and the failing stage number is left in g_PackageLastErrorPath.
*/
SampleVoiceSetResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  TH_LEGACY_HRESULT directSoundResult;
  uint32_t encodedBlockSize;
  uint32_t remainingBlocks;
  int remainingCount;
  int registryRemaining;
  IDirectSoundBuffer **voiceSetOrErrorCode;
  IDirectSoundBuffer **voiceCursor;
  SoundSampleAsset *encodedBlock;
  short *outputStereoPcm;
  DirectSoundVoiceSet **registryCursor;
  ArenaAllocResult voiceSetAlloc;
  SampleVoiceSetResult successResult;
  SampleVoiceSetResult failureResult;
  int32_t failedStage;
  TH_LEGACY_DWORD wrapByteCount;
  TH_LEGACY_LPVOID wrapRegion;
  uint32_t lockedByteCount;
  short *lockedPcm;
  IDirectSoundBuffer *soundBuffer;

  soundBuffer = NULL;
  failedStage = DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER;
  /* 0x14 bytes: the packed 18-byte WAVEFORMATEX and the two bytes behind it */
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(sizeof PrimarySoundBufferDesc,&PrimarySoundBufferDesc);
  voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_SOUND_SAMPLE_INVALID;
  if ((sampleAsset->magic == ASSET_MAGIC_SAM) && (sampleAsset->formatVersion == SOUND_SAMPLE_FORMAT_VERSION)) {
    WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat_PCM_22050_Stereo16.nChannels = 2;
    /* the original writes only the low word (the dword was zeroed above) */
    THANDOR_PART(uint16_t, WaveFormat_PCM_22050_Stereo16.nSamplesPerSec, 0) = 22050;
    WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 88200; /* 22050 * 4 */
    WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4; /* 2 channels * 2 bytes */
    WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 16;
    /* decodedBlockCount * SOUND_SAMPLE_DECODED_BLOCK_BYTES */
    PrimarySoundBufferDesc.dwBufferBytes = sampleAsset->decodedBlockCount << 10;
    PrimarySoundBufferDesc.dwSize = sizeof PrimarySoundBufferDesc;
    PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
    PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
    directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                      (g_DirectSound,&PrimarySoundBufferDesc,&soundBuffer,NULL);
    voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
    if (directSoundResult == 0) {
      failedStage = DIRECTSOUND_VOICE_STAGE_LOCK;
      directSoundResult = soundBuffer->lpVtbl->Lock
                        (soundBuffer,0,0,&lockedPcm,&lockedByteCount,&wrapRegion,&wrapByteCount,
                         DSBLOCK_ENTIREBUFFER);
      voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
      if (directSoundResult == 0) {
        failedStage = DIRECTSOUND_VOICE_STAGE_FILL;
        encodedBlock = sampleAsset + 1; /* the packed blocks follow the 0x200-byte header */
        remainingBlocks = lockedByteCount >> 10; /* / SOUND_SAMPLE_DECODED_BLOCK_BYTES */
        outputStereoPcm = lockedPcm;
        do {
          encodedBlockSize = SoundSample_DecodePackedCoefficientBlock((short *)THANDOR_ADDR(g_SoundSampleCoefficientBlock,0),(uint8_t *)encodedBlock);
          SoundSample_DecodeCoefficientBlockToPcmMmx(outputStereoPcm,(short *)THANDOR_ADDR(g_SoundSampleCoefficientBlock,0));
          /* advance by encodedBlockSize bytes */
          encodedBlock = (SoundSampleAsset *)(encodedBlock->reserved04_0B + (encodedBlockSize - 4));
          outputStereoPcm = outputStereoPcm + 0x200; /* SOUND_SAMPLE_DECODED_BLOCK_BYTES in shorts */
          remainingBlocks = remainingBlocks - 1;
        } while (remainingBlocks != 0);
        directSoundResult = soundBuffer->lpVtbl->Unlock(soundBuffer,lockedPcm,lockedByteCount,wrapRegion,wrapByteCount);
        voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
        if (directSoundResult == 0) {
          voiceSetAlloc = g_MemoryApi.alloc(sizeof(DirectSoundVoiceSet));
          voiceSetOrErrorCode = (IDirectSoundBuffer **)voiceSetAlloc.payloadOrError;
          if (!voiceSetAlloc.failed) {
            remainingCount = DIRECTSOUND_VOICES_PER_SET;
            voiceCursor = voiceSetOrErrorCode;
            do {
              *voiceCursor = NULL;
              voiceCursor++;
              remainingCount--;
            } while (remainingCount != 0);
            registryCursor = g_DirectSoundVoiceSetRegistry;
            *voiceSetOrErrorCode = soundBuffer;
            /* Register the set in the first free registry slot; a full or missing registry is not an error. */
            if (registryCursor != NULL) {
              for (registryRemaining = DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryRemaining != 0;
                   registryRemaining--) {
                if (*registryCursor == NULL) {
                  *registryCursor = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
                  break;
                }
                registryCursor++;
              }
            }
            successResult.failed = false;
            successResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
            return successResult;
          }
        }
      }
    }
  }
  if (soundBuffer != NULL) {
    soundBuffer->lpVtbl->Release(soundBuffer);
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
  return failureResult;
}


/* Address: 0x00583690.
   Frees a voice set made by DirectSound_CreateSampleVoiceSet: releases its eight voices (the data
   buffer and its duplicates), frees the set and clears its registry slot. NULL is accepted.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  DirectSoundVoiceSet **registryGuard;
  int voicesRemaining;
  DirectSoundVoiceSet **registryRemaining; /* a count; Ghidra shares the register with registryGuard */
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;
  IDirectSoundBuffer *voiceBuffer;

  voicesRemaining = DIRECTSOUND_VOICES_PER_SET;
  voiceCursor = voiceSet->voices;
  if (voiceSet != NULL) {
    do {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != NULL) {
        voiceBuffer->lpVtbl->Release(voiceBuffer);
      }
      voiceCursor++;
      voicesRemaining--;
    } while (voicesRemaining != 0);
    g_MemoryApi.free(voiceSet);
    registryRemaining = (DirectSoundVoiceSet **)DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY;
    registryCursor = g_DirectSoundVoiceSetRegistry;
    registryGuard = g_DirectSoundVoiceSetRegistry;
    /* first pass tests the registry pointer, later passes the remaining count */
    while (registryGuard != NULL) {
      if (voiceSet == *registryCursor) {
        *registryCursor = NULL;
        return;
      }
      registryCursor++;
      registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining - 1);
      registryGuard = registryRemaining;
    }
  }
  return;
}


/* Address: 0x00583720.
   Wraps raw PCM data in a voice set: creates a secondary buffer of bufferByteCount bytes in the given
   rate/bits/channels format, copies the data into it dword by dword and registers a new eight-voice set
   holding the buffer in voices[0]. On failure CF is set, EAX holds the error code and the failing stage
   number is left in g_PackageLastErrorPath.
*/
PcmVoiceSetResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          uint32_t *pcmData)

{
  int32_t pendingStage;
  uint32_t blockAlignOrDwordCount;
  TH_LEGACY_HRESULT directSoundResult;
  int remainingCount;
  int registryRemaining;
  IDirectSoundBuffer **voiceSetOrErrorCode;
  IDirectSoundBuffer **voiceCursor;
  uint32_t *destCursor;
  DirectSoundVoiceSet **registryCursor;
  ArenaAllocResult voiceSetAlloc;
  PcmVoiceSetResult successResult;
  PcmVoiceSetResult failureResult;
  int32_t failedStage;
  TH_LEGACY_DWORD wrapByteCount;
  TH_LEGACY_LPVOID wrapRegion;
  uint32_t lockedByteCount;
  uint32_t *lockedData;
  IDirectSoundBuffer *soundBuffer;
  
  soundBuffer = NULL;
  failedStage = DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER;
  /* 0x14 bytes: the packed 18-byte WAVEFORMATEX and the two bytes behind it */
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(sizeof PrimarySoundBufferDesc,&PrimarySoundBufferDesc);
  /* the shared WaveFormat_PCM_22050_Stereo16 buffer is reused with the caller's format */
  WaveFormat_PCM_22050_Stereo16.nChannels = (AudioChannelCount)channelCount;
  WaveFormat_PCM_22050_Stereo16.wBitsPerSample = (AudioBitsPerSample)bitsPerSample;
  blockAlignOrDwordCount = bitsPerSample * channelCount >> 3; /* bytes per frame */
  PrimarySoundBufferDesc.dwBufferBytes = bufferByteCount;
  WaveFormat_PCM_22050_Stereo16.nBlockAlign = (AudioBlockAlignBytes)blockAlignOrDwordCount;
  WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = blockAlignOrDwordCount * sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
  PrimarySoundBufferDesc.dwSize = sizeof PrimarySoundBufferDesc;
  PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
  PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
  directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                    (g_DirectSound,&PrimarySoundBufferDesc,&soundBuffer,NULL);
  voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
  pendingStage = failedStage;
  if (directSoundResult == 0) {
    directSoundResult = soundBuffer->lpVtbl->Lock
                      (soundBuffer,0,0,&lockedData,&lockedByteCount,&wrapRegion,&wrapByteCount,
                       DSBLOCK_ENTIREBUFFER);
    voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
    pendingStage = DIRECTSOUND_VOICE_STAGE_LOCK;
    if (directSoundResult == 0) {
      failedStage = DIRECTSOUND_VOICE_STAGE_FILL;
      destCursor = lockedData;
      /* copies lockedByteCount / 4 dwords; a trailing 1..3 bytes stay uncopied */
      for (blockAlignOrDwordCount = lockedByteCount >> 2; blockAlignOrDwordCount != 0; blockAlignOrDwordCount = blockAlignOrDwordCount - 1) {
        *destCursor = *pcmData;
        pcmData++;
        destCursor++;
      }
      directSoundResult = soundBuffer->lpVtbl->Unlock(soundBuffer,lockedData,lockedByteCount,wrapRegion,wrapByteCount);
      voiceSetOrErrorCode = (IDirectSoundBuffer **)FATAL_ERROR_DIRECTSOUND_SETUP;
      pendingStage = DIRECTSOUND_VOICE_STAGE_FILL;
      if (directSoundResult == 0) {
        voiceSetAlloc = g_MemoryApi.alloc(sizeof(DirectSoundVoiceSet));
        voiceSetOrErrorCode = (IDirectSoundBuffer **)voiceSetAlloc.payloadOrError;
        pendingStage = failedStage;
        if (!voiceSetAlloc.failed) {
          remainingCount = DIRECTSOUND_VOICES_PER_SET;
          voiceCursor = voiceSetOrErrorCode;
          do {
            *voiceCursor = NULL;
            voiceCursor++;
            remainingCount--;
          } while (remainingCount != 0);
          registryCursor = g_DirectSoundVoiceSetRegistry;
          *voiceSetOrErrorCode = soundBuffer;
          /* Register the set in the first free registry slot; a full or missing registry is not an error. */
          if (registryCursor != NULL) {
            for (registryRemaining = DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY; registryRemaining != 0;
                 registryRemaining--) {
              if (*registryCursor == NULL) {
                *registryCursor = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
                break;
              }
              registryCursor++;
            }
          }
          successResult.failed = false;
          successResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
          return successResult;
        }
      }
    }
  }
  failedStage = pendingStage;
  if (soundBuffer != NULL) {
    soundBuffer->lpVtbl->Release(soundBuffer);
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
  return failureResult;
}


/* Address: 0x005838D0.
   Frees a voice set made by DirectSound_CreatePcmVoiceSet; the same code as
   DirectSound_ReleaseSampleVoiceSet (release the eight voices, free the set, clear its registry slot).
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *voiceBuffer;
  DirectSoundVoiceSet **registryGuard;
  int voicesRemaining;
  DirectSoundVoiceSet **registryRemaining; /* a count; Ghidra shares the register with registryGuard */
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;

  voicesRemaining = DIRECTSOUND_VOICES_PER_SET;
  voiceCursor = voiceSet->voices;
  if (voiceSet != NULL) {
    do {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != NULL) {
        voiceBuffer->lpVtbl->Release(voiceBuffer);
      }
      voiceCursor++;
      voicesRemaining--;
    } while (voicesRemaining != 0);
    g_MemoryApi.free(voiceSet);
    registryRemaining = (DirectSoundVoiceSet **)DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY;
    registryCursor = g_DirectSoundVoiceSetRegistry;
    registryGuard = g_DirectSoundVoiceSetRegistry;
    /* first pass tests the registry pointer, later passes the remaining count */
    while (registryGuard != NULL) {
      if (voiceSet == *registryCursor) {
        *registryCursor = NULL;
        return;
      }
      registryCursor++;
      registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining - 1);
      registryGuard = registryRemaining;
    }
  }
  return;
}


/*
Volume and pan as in 0x00583AD0/0x00583C70: the attenuation of the louder channel is the volume,
and attenuation[first] - attenuation[second] is the pan. The original leaves the pan argument on
the stack for the following SetPan call, which Ghidra could not follow.
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

/* Shared body of PlayOneShot/PlayLooping (0x00583940 / 0x00583A70), rewritten from the assembly:
   the first idle voice of the set plays; an empty slot is filled with DuplicateSoundBuffer of
   voice 0 rewound to position 0; with all eight voices busy CF is set. */
static SoundPlayResult DirectSound_PlayVoiceSet
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,TH_LEGACY_DWORD playFlags)
{
  SoundPlayResult result;
  IDirectSoundBuffer *voice;
  TH_LEGACY_DWORD status;
  int slot;

  result.soundBuffer = NULL;
  result.failed = true;
  if (voiceSet == NULL) {
    return result;
  }
  for (slot = 0; slot < DIRECTSOUND_VOICES_PER_SET; slot++) {
    voice = voiceSet->voices[slot];
    if (voice == NULL) {
      if (g_DirectSound->lpVtbl->DuplicateSoundBuffer
                    (g_DirectSound,voiceSet->voices[0],&voiceSet->voices[slot]) != 0) {
        return result;
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
    return result;
  }
  voice->lpVtbl->Play(voice,0,0,playFlags);
  DirectSound_ApplyChannelGains(leftChannelGainQ15,rightChannelGainQ15,voice);
  result.soundBuffer = voice;
  result.failed = false;
  return result;
}


/* Address: 0x00583940.
   Plays a sound once on the first idle voice of the set (duplicating voices[0] into an empty slot when
   needed) and sets its volume/pan from the two 0..0x8000 channel gains via the 129-entry attenuation
   table. CF clear returns the voice in EAX; CF set (EAX 0) when all eight voices are busy or the
   duplication fails.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,0);
}


/* Address: 0x00583A70.
   Like DirectSound_PlayOneShot, but the voice plays with DSBPLAY_LOOPING until it is stopped.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,DSBPLAY_LOOPING);
}


/* Address: 0x00583B90.
   Stops one voice (a buffer returned by the play functions); NULL is accepted.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopVoice(IDirectSoundBuffer *voice)

{
  if (voice != NULL) {
    voice->lpVtbl->Stop(voice);
  }
  return;
}


/* Address: 0x00583BC0.
   Tells whether a voice is still playing. The result is inverted like all CF flags here: CF clear
   (false) means playing, CF set (true) means NULL or stopped; EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice)

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


/* Address: 0x00583C00.
   Stops every voice of every voice set in the registry (empty registry slots and voices are skipped).
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopAllVoices(void)

{
  IDirectSoundBuffer *voiceBuffer;
  DirectSoundVoiceSet **registryGuard;
  IDirectSoundBuffer **voiceGuard;
  DirectSoundVoiceSet **registryRemaining; /* counts; Ghidra shares their registers with the guards */
  IDirectSoundBuffer **voicesRemaining;
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;

  registryRemaining = (DirectSoundVoiceSet **)DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY;
  registryCursor = g_DirectSoundVoiceSetRegistry;
  registryGuard = g_DirectSoundVoiceSetRegistry;
  while (registryGuard != NULL) {
    voicesRemaining = (IDirectSoundBuffer **)DIRECTSOUND_VOICES_PER_SET;
    /* voices is at offset 0, so an empty registry slot yields a NULL cursor and skips the set */
    voiceCursor = (*registryCursor)->voices;
    voiceGuard = voiceCursor;
    while (voiceGuard != NULL) {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != NULL) {
        voiceBuffer->lpVtbl->Stop(voiceBuffer);
      }
      voiceCursor++;
      voicesRemaining = (IDirectSoundBuffer **)((int)voicesRemaining - 1);
      voiceGuard = voicesRemaining;
    }
    registryCursor++;
    registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining - 1);
    registryGuard = registryRemaining;
  }
  return;
}


/* Address: 0x00583C60.
   Backend slot g_SoundQueryVoiceRegs: takes a voice and returns 0 in EDX:EAX. No call site uses the
   slot, so what it was meant to query is unknown.
*/
uint64_t DirectSound_QueryVoiceRegsStub(IDirectSoundBuffer *voice)

{
  return 0;
}

/* Address: 0x00583C70.
   Updates the volume and pan of a playing voice from two new channel gains, with the same conversion as
   the play functions (louder channel's attenuation = volume, attenuation difference = pan). NULL is
   accepted.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_SetVoiceGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  if (voice != NULL) {
    DirectSound_ApplyChannelGains(leftChannelGainQ15,rightChannelGainQ15,voice);
  }
}


/* Address: 0x004175F0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend stop-all no-op. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx SoundBackendDisabled_StopAllVoices(void)

{
  return;
}

