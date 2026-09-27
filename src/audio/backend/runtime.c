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
   Ownership: audio/backend/runtime.
   Purpose: Handles direct sound shutdown.
   Local calls: SoundBackendDisabled_StopAllVoices.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_Shutdown(void)

{
  SoundBackendDisabled_StopAllVoices();
  if (g_PrimarySoundBuffer != (IDirectSoundBuffer *)0x0) {
    g_PrimarySoundBuffer->lpVtbl->SetVolume(g_PrimarySoundBuffer,g_PrimaryVolume);
    g_PrimarySoundBuffer->lpVtbl->SetPan(g_PrimarySoundBuffer,g_PrimaryPan);
    g_PrimarySoundBuffer->lpVtbl->Release(g_PrimarySoundBuffer);
    g_PrimarySoundBuffer = (IDirectSoundBuffer *)0x0;
  }
  if (g_DirectSound != (IDirectSound *)0x0) {
    g_DirectSound->lpVtbl->Release(g_DirectSound);
    g_DirectSound = (IDirectSound *)0x0;
  }
  g_MemoryApi.free(g_DirectSoundVoiceSetRegistry);
  g_DirectSoundVoiceSetRegistry = (DirectSoundVoiceSet **)0x0;
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
   Ownership: audio/backend/runtime.
   Purpose: Validates a fixed 0x200-byte sam header, creates a 22050 Hz stereo 16-bit secondary buffer sized
   decodedBlockCount*0x400, decodes each payload block into the locked buffer, allocates an eight-pointer
   DirectSoundVoiceSet, stores the original buffer in voices[0], and registers the set in the 256-entry registry.
   CF clear returns the set pointer in EAX; CF set returns an engine/DirectSound error code in EAX.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator], SoundSample_DecodePackedCoefficientBlock
   [audio/codec/sam], SoundSample_DecodeCoefficientBlockToPcmMmx [audio/codec/sam].
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
  
  soundBuffer = (IDirectSoundBuffer *)0x0;
  failedStage = 100;
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
  voiceSetOrErrorCode = (IDirectSoundBuffer **)0x4a;
  if ((sampleAsset->magic == ASSET_MAGIC_SAM) && (sampleAsset->formatVersion == 0x10000)) {
    WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat_PCM_22050_Stereo16.nChannels = 2;
    THANDOR_PART(uint16_t, WaveFormat_PCM_22050_Stereo16.nSamplesPerSec, 0) = 0x5622;
    WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 0x15888;
    WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4;
    WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 0x10;
    PrimarySoundBufferDesc.dwBufferBytes = sampleAsset->decodedBlockCount << 10;
    PrimarySoundBufferDesc.dwSize = 0x14;
    PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
    PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
    directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                      (g_DirectSound,&PrimarySoundBufferDesc,&soundBuffer,(TH_LEGACY_LPVOID)0x0);
    voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
    if (directSoundResult == 0) {
      failedStage = 0x65;
      directSoundResult = soundBuffer->lpVtbl->Lock
                        (soundBuffer,0,0,&lockedPcm,&lockedByteCount,&wrapRegion,&wrapByteCount,2);
      voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
      if (directSoundResult == 0) {
        failedStage = 0x66;
        encodedBlock = sampleAsset + 1;
        remainingBlocks = lockedByteCount >> 10;
        outputStereoPcm = lockedPcm;
        do {
          encodedBlockSize = SoundSample_DecodePackedCoefficientBlock((short *)THANDOR_ADDR(g_SoundSampleCoefficientBlock,0),(uint8_t *)encodedBlock);
          SoundSample_DecodeCoefficientBlockToPcmMmx(outputStereoPcm,(short *)THANDOR_ADDR(g_SoundSampleCoefficientBlock,0));
          encodedBlock = (SoundSampleAsset *)(encodedBlock->reserved04_0B + (encodedBlockSize - 4));
          outputStereoPcm = outputStereoPcm + 0x200;
          remainingBlocks = remainingBlocks - 1;
        } while (remainingBlocks != 0);
        directSoundResult = soundBuffer->lpVtbl->Unlock(soundBuffer,lockedPcm,lockedByteCount,wrapRegion,wrapByteCount);
        voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
        if (directSoundResult == 0) {
          voiceSetAlloc = g_MemoryApi.alloc(0x20);
          voiceSetOrErrorCode = (IDirectSoundBuffer **)voiceSetAlloc.payloadOrError;
          if (!voiceSetAlloc.failed) {
            remainingCount = 8;
            voiceCursor = voiceSetOrErrorCode;
            do {
              *voiceCursor = (IDirectSoundBuffer *)0x0;
              voiceCursor = voiceCursor + 1;
              remainingCount = remainingCount + -1;
            } while (remainingCount != 0);
            registryCursor = g_DirectSoundVoiceSetRegistry;
            *voiceSetOrErrorCode = soundBuffer;
            /* Register the set in the first free registry slot; a full or missing registry is not an error. */
            if (registryCursor != (DirectSoundVoiceSet **)0x0) {
              for (registryRemaining = 0x100; registryRemaining != 0; registryRemaining = registryRemaining + -1) {
                if (*registryCursor == (DirectSoundVoiceSet *)0x0) {
                  *registryCursor = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
                  break;
                }
                registryCursor = registryCursor + 1;
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
  if (soundBuffer != (IDirectSoundBuffer *)0x0) {
    soundBuffer->lpVtbl->Release(soundBuffer);
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
  return failureResult;
}


/* Address: 0x00583690.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes its pointer from the 256-entry registry. Null is accepted. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  DirectSoundVoiceSet **registryGuard;
  int voicesRemaining;
  DirectSoundVoiceSet **registryRemaining;
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;
  IDirectSoundBuffer *voiceBuffer;
  
  voicesRemaining = 8;
  voiceCursor = voiceSet->voices;
  if (voiceSet != (DirectSoundVoiceSet *)0x0) {
    do {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != (IDirectSoundBuffer *)0x0) {
        voiceBuffer->lpVtbl->Release(voiceBuffer);
      }
      voiceCursor = voiceCursor + 1;
      voicesRemaining = voicesRemaining + -1;
    } while (voicesRemaining != 0);
    g_MemoryApi.free(voiceSet);
    registryRemaining = (DirectSoundVoiceSet **)0x100;
    registryCursor = g_DirectSoundVoiceSetRegistry;
    registryGuard = g_DirectSoundVoiceSetRegistry;
    while (registryGuard != (DirectSoundVoiceSet **)0x0) {
      if (voiceSet == *registryCursor) {
        *registryCursor = (DirectSoundVoiceSet *)0x0;
        return;
      }
      registryCursor = registryCursor + 1;
      registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining + -1);
      registryGuard = registryRemaining;
    }
  }
  return;
}


/* Address: 0x00583720.
   Ownership: audio/backend/runtime.
   Purpose: CF clear returns the set pointer in EAX; CF set returns an error code.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
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
  
  soundBuffer = (IDirectSoundBuffer *)0x0;
  failedStage = 100;
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
  WaveFormat_PCM_22050_Stereo16.nChannels = (AudioChannelCount)channelCount;
  WaveFormat_PCM_22050_Stereo16.wBitsPerSample = (AudioBitsPerSample)bitsPerSample;
  blockAlignOrDwordCount = bitsPerSample * channelCount >> 3;
  PrimarySoundBufferDesc.dwBufferBytes = bufferByteCount;
  WaveFormat_PCM_22050_Stereo16.nBlockAlign = (AudioBlockAlignBytes)blockAlignOrDwordCount;
  WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = blockAlignOrDwordCount * sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
  PrimarySoundBufferDesc.dwSize = 0x14;
  PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
  PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
  directSoundResult = g_DirectSound->lpVtbl->CreateSoundBuffer
                    (g_DirectSound,&PrimarySoundBufferDesc,&soundBuffer,(TH_LEGACY_LPVOID)0x0);
  voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
  pendingStage = failedStage;
  if (directSoundResult == 0) {
    directSoundResult = soundBuffer->lpVtbl->Lock
                      (soundBuffer,0,0,&lockedData,&lockedByteCount,&wrapRegion,&wrapByteCount,2);
    voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
    pendingStage = 0x65;
    if (directSoundResult == 0) {
      failedStage = 0x66;
      destCursor = lockedData;
      for (blockAlignOrDwordCount = lockedByteCount >> 2; blockAlignOrDwordCount != 0; blockAlignOrDwordCount = blockAlignOrDwordCount - 1) {
        *destCursor = *pcmData;
        pcmData = pcmData + 1;
        destCursor = destCursor + 1;
      }
      directSoundResult = soundBuffer->lpVtbl->Unlock(soundBuffer,lockedData,lockedByteCount,wrapRegion,wrapByteCount);
      voiceSetOrErrorCode = (IDirectSoundBuffer **)0x29;
      pendingStage = 0x66;
      if (directSoundResult == 0) {
        voiceSetAlloc = g_MemoryApi.alloc(0x20);
        voiceSetOrErrorCode = (IDirectSoundBuffer **)voiceSetAlloc.payloadOrError;
        pendingStage = failedStage;
        if (!voiceSetAlloc.failed) {
          remainingCount = 8;
          voiceCursor = voiceSetOrErrorCode;
          do {
            *voiceCursor = (IDirectSoundBuffer *)0x0;
            voiceCursor = voiceCursor + 1;
            remainingCount = remainingCount + -1;
          } while (remainingCount != 0);
          registryCursor = g_DirectSoundVoiceSetRegistry;
          *voiceSetOrErrorCode = soundBuffer;
          /* Register the set in the first free registry slot; a full or missing registry is not an error. */
          if (registryCursor != (DirectSoundVoiceSet **)0x0) {
            for (registryRemaining = 0x100; registryRemaining != 0; registryRemaining = registryRemaining + -1) {
              if (*registryCursor == (DirectSoundVoiceSet *)0x0) {
                *registryCursor = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
                break;
              }
              registryCursor = registryCursor + 1;
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
  if (soundBuffer != (IDirectSoundBuffer *)0x0) {
    soundBuffer->lpVtbl->Release(soundBuffer);
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.voiceSet = (DirectSoundVoiceSet *)voiceSetOrErrorCode;
  return failureResult;
}


/* Address: 0x005838D0.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes it from the registry. This is ownership-equivalent to the sample-set release service.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *voiceBuffer;
  DirectSoundVoiceSet **registryGuard;
  int voicesRemaining;
  DirectSoundVoiceSet **registryRemaining;
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;
  
  voicesRemaining = 8;
  voiceCursor = voiceSet->voices;
  if (voiceSet != (DirectSoundVoiceSet *)0x0) {
    do {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != (IDirectSoundBuffer *)0x0) {
        voiceBuffer->lpVtbl->Release(voiceBuffer);
      }
      voiceCursor = voiceCursor + 1;
      voicesRemaining = voicesRemaining + -1;
    } while (voicesRemaining != 0);
    g_MemoryApi.free(voiceSet);
    registryRemaining = (DirectSoundVoiceSet **)0x100;
    registryCursor = g_DirectSoundVoiceSetRegistry;
    registryGuard = g_DirectSoundVoiceSetRegistry;
    while (registryGuard != (DirectSoundVoiceSet **)0x0) {
      if (voiceSet == *registryCursor) {
        *registryCursor = (DirectSoundVoiceSet *)0x0;
        return;
      }
      registryCursor = registryCursor + 1;
      registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining + -1);
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

/* Shared body of PlayOneShot/PlayLooping (0x00583930 / 0x00583A70), rewritten from the assembly:
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

  result.soundBuffer = (IDirectSoundBuffer *)0x0;
  result.failed = true;
  if (voiceSet == (DirectSoundVoiceSet *)0x0) {
    return result;
  }
  for (slot = 0; slot < 8; slot = slot + 1) {
    voice = voiceSet->voices[slot];
    if (voice == (IDirectSoundBuffer *)0x0) {
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
    if ((status & 1) == 0) {
      break;
    }
  }
  if (slot == 8) {
    return result;
  }
  voice->lpVtbl->Play(voice,0,0,playFlags);
  DirectSound_ApplyChannelGains(leftChannelGainQ15,rightChannelGainQ15,voice);
  result.soundBuffer = voice;
  result.failed = false;
  return result;
}


/* Address: 0x00583940.
   Ownership: audio/backend/runtime.
   Purpose: Finds a non-playing voice or duplicates voices[0] into an empty slot, starts playback without
   DSBPLAY_LOOPING, converts both 0..0x8000 gain inputs through the 129-entry attenuation table, and applies
   overall volume plus signed pan. CF clear returns the selected IDirectSoundBuffer in EAX; CF set returns zero
   when all eight voices are busy or duplication fails.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,0);
}


/* Address: 0x00583A70.
   Ownership: audio/backend/runtime.
   Purpose: Finds a non-playing voice or duplicates voices[0], starts playback with DSBPLAY_LOOPING, and applies
   the same gain-to-volume/pan conversion as the one-shot path. CF clear returns the selected IDirectSoundBuffer.
*/
SoundPlayResult __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return DirectSound_PlayVoiceSet(leftChannelGainQ15,rightChannelGainQ15,voiceSet,1 /* DSBPLAY_LOOPING */);
}


/* Address: 0x00583B90.
   Ownership: audio/backend/runtime.
   Purpose: Calls IDirectSoundBuffer::Stop for a non-null voice. Null is accepted and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopVoice(IDirectSoundBuffer *voice)

{
  if (voice != (IDirectSoundBuffer *)0x0) {
    voice->lpVtbl->Stop(voice);
  }
  return;
}


/* Address: 0x00583BC0.
   Ownership: audio/backend/runtime.
   Purpose: Calls GetStatus and tests DSBSTATUS_PLAYING bit 0. CF clear means playing. CF set means null or not
   playing. EAX is preserved rather than used as a scalar return.
*/
bool __thandor_cf_preserve_eax_ecx_edx DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  bool notPlaying;
  TH_LEGACY_DWORD voiceStatusFlags;
  
  notPlaying = true;
  if (voice != (IDirectSoundBuffer *)0x0) {
    voice->lpVtbl->GetStatus(voice,&voiceStatusFlags);
    notPlaying = (voiceStatusFlags & 1) == 0;
  }
  return notPlaying;
}


/* Address: 0x00583C00.
   Ownership: audio/backend/runtime.
   Purpose: Walks all 256 registered DirectSoundVoiceSet pointers and calls Stop for every non-null voice in each
   eight-pointer set. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopAllVoices(void)

{
  IDirectSoundBuffer *voiceBuffer;
  DirectSoundVoiceSet **registryGuard;
  IDirectSoundBuffer **voiceGuard;
  DirectSoundVoiceSet **registryRemaining;
  IDirectSoundBuffer **voicesRemaining;
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;
  
  registryRemaining = (DirectSoundVoiceSet **)0x100;
  registryCursor = g_DirectSoundVoiceSetRegistry;
  registryGuard = g_DirectSoundVoiceSetRegistry;
  while (registryGuard != (DirectSoundVoiceSet **)0x0) {
    voicesRemaining = (IDirectSoundBuffer **)0x8;
    voiceCursor = (*registryCursor)->voices;
    voiceGuard = voiceCursor;
    while (voiceGuard != (IDirectSoundBuffer **)0x0) {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != (IDirectSoundBuffer *)0x0) {
        voiceBuffer->lpVtbl->Stop(voiceBuffer);
      }
      voiceCursor = voiceCursor + 1;
      voicesRemaining = (IDirectSoundBuffer **)((int)voicesRemaining + -1);
      voiceGuard = voicesRemaining;
    }
    registryCursor = registryCursor + 1;
    registryRemaining = (DirectSoundVoiceSet **)((int)registryRemaining + -1);
    registryGuard = registryRemaining;
  }
  return;
}


/* Address: 0x00583C60.
   Ownership: audio/backend/runtime.
   Purpose: Consumes one voice pointer and returns EDX:EAX equal to zero. No executable call site references this
   service slot, so the higher-level query semantics remain unresolved.
*/
uint64_t DirectSound_QueryVoiceRegsStub(IDirectSoundBuffer *voice)

{
  return 0;
}

/* Address: 0x00583C70.
   Ownership: audio/backend/runtime.
   Purpose: Updates a non-null playing voice with the same two-gain conversion used by both play services. The
   louder input selects the DirectSound volume attenuation; the attenuation difference becomes signed pan. Null is
   accepted and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_SetVoiceGains
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  if (voice != (IDirectSoundBuffer *)0x0) {
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

