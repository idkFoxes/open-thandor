/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/backend/runtime.h>

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
    (*g_PrimarySoundBuffer->lpVtbl->SetVolume)(g_PrimarySoundBuffer,g_PrimaryVolume);
    (*g_PrimarySoundBuffer->lpVtbl->SetPan)(g_PrimarySoundBuffer,g_PrimaryPan);
    (*g_PrimarySoundBuffer->lpVtbl->Release)(g_PrimarySoundBuffer);
    g_PrimarySoundBuffer = (IDirectSoundBuffer *)0x0;
  }
  if (g_DirectSound != (IDirectSound *)0x0) {
    (*g_DirectSound->lpVtbl->Release)(g_DirectSound);
    g_DirectSound = (IDirectSound *)0x0;
  }
  (*g_MemoryApi.free)(g_DirectSoundVoiceSetRegistry);
  g_DirectSoundVoiceSetRegistry = (DirectSoundVoiceSet **)0x0;
  return;
}


/* Address: 0x00417570.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend placeholder. Returns pointer value 0xFFFFFFFF with CF clear and consumes one
   SoundSampleAsset argument.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  return false;
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
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  return false;
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
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return false;
}


/* Address: 0x004175C0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend looping placeholder. It only clears CF and leaves EAX unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  return false;
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
qword SoundBackendDisabled_QueryVoiceRegs(IDirectSoundBuffer *voice)

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
   Ownership: audio/backend/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime],
   Memory_ZeroDwords [core/memory/allocator], CosineDerivedLookupTables_InitCf [core/math/fixed].
*/
bool __thandor_void_preserve_ecx_edx DirectSound_Init(void)

{
  HINSTANCE module;
  TH_LEGACY_HRESULT TVar1;
  int iVar2;
  DirectSoundVoiceSet **ppDVar3;
  DynDllLoadEaxCf5 DVar4;
  DynApiResolveEaxCf5 DVar5;
  ArenaAllocEaxCf5 AVar6;
  sdword local_1c;
  
  local_1c = 0;
  DVar4 = DynDLL_Load(dynapi_4);
  module = DVar4.moduleOrError;
  if ((((!DVar4.carry) &&
       (DVar5 = DynAPI_Resolve(&pDirectSoundCreate,module,dynapi_20), !DVar5.carry)) &&
      (DVar5 = DynAPI_Resolve(&pDirectSoundEnumerateA,module,dynapi_21), !DVar5.carry)) &&
     ((DVar5 = DynAPI_Resolve(&pDirectSoundCaptureCreate,module,dynapi_22), !DVar5.carry &&
      (DVar5 = DynAPI_Resolve(&pDirectSoundCaptureEnumerateA,module,dynapi_23), !DVar5.carry)))) {
    TVar1 = (*pDirectSoundCreate)((TH_LEGACY_GUID *)0x0,&g_DirectSound,(TH_LEGACY_LPVOID)0x0);
    if (TVar1 != 0) {
      return false;
    }
    TVar1 = (*g_DirectSound->lpVtbl->SetCooperativeLevel)(g_DirectSound,g_MainWindow,3);
    if (TVar1 == 0) {
      local_1c = 1;
      Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
      Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
      WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
      WaveFormat_PCM_22050_Stereo16.nChannels = 2;
      WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = 0x5622;
      WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4;
      WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 0x15888;
      WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 0x10;
      PrimarySoundBufferDesc.dwSize = 0x14;
      PrimarySoundBufferDesc.dwFlags = 0xc1;
      TVar1 = (*g_DirectSound->lpVtbl->CreateSoundBuffer)
                        (g_DirectSound,&PrimarySoundBufferDesc,&g_PrimarySoundBuffer,
                         (TH_LEGACY_LPVOID)0x0);
      if (TVar1 == 0) {
        local_1c = 2;
        TVar1 = (*g_PrimarySoundBuffer->lpVtbl->SetFormat)
                          (g_PrimarySoundBuffer,&WaveFormat_PCM_22050_Stereo16);
        if (TVar1 == 0) {
          local_1c = 3;
          TVar1 = (*g_PrimarySoundBuffer->lpVtbl->GetVolume)(g_PrimarySoundBuffer,&g_PrimaryVolume);
          if (TVar1 == 0) {
            local_1c = 4;
            TVar1 = (*g_PrimarySoundBuffer->lpVtbl->GetPan)(g_PrimarySoundBuffer,&g_PrimaryPan);
            if (TVar1 == 0) {
              local_1c = 5;
              TVar1 = (*g_PrimarySoundBuffer->lpVtbl->SetVolume)(g_PrimarySoundBuffer,0);
              if (TVar1 == 0) {
                local_1c = 6;
                TVar1 = (*g_PrimarySoundBuffer->lpVtbl->SetPan)(g_PrimarySoundBuffer,0);
                if (TVar1 == 0) {
                  local_1c = 7;
                  TVar1 = (*g_PrimarySoundBuffer->lpVtbl->Play)(g_PrimarySoundBuffer,0,0,1);
                  if (TVar1 == 0) {
                    AVar6 = (*g_MemoryApi.alloc)(0x400);
                    if (!AVar6.carry) {
                      ppDVar3 = (DirectSoundVoiceSet **)AVar6.eax;
                      g_DirectSoundVoiceSetRegistry = (DirectSoundVoiceSet **)AVar6.eax;
                      for (iVar2 = 0x100; iVar2 != 0; iVar2 = iVar2 + -1) {
                        *ppDVar3 = (DirectSoundVoiceSet *)0x0;
                        ppDVar3 = ppDVar3 + 1;
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
                      CosineDerivedLookupTables_InitCf();
                      return false;
                    }
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,local_1c,g_PackageLastErrorPath);
  }
  return true;
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
SoundCreateSampleVoiceSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  DirectSoundVoiceSet **ppDVar1;
  TH_LEGACY_HRESULT TVar2;
  dword dVar3;
  uint uVar4;
  int iVar5;
  DirectSoundVoiceSet **ppDVar6;
  IDirectSoundBuffer **ppIVar7;
  IDirectSoundBuffer **ppIVar8;
  SoundSampleAsset *encodedBlock;
  short *outputStereoPcm;
  DirectSoundVoiceSet **ppDVar9;
  ArenaAllocEaxCf5 AVar10;
  SoundCreateSampleVoiceSetEaxCf5 SVar11;
  sdword sStack_30;
  TH_LEGACY_DWORD TStack_2c;
  TH_LEGACY_LPVOID pvStack_28;
  uint uStack_24;
  short *psStack_20;
  IDirectSoundBuffer *pIStack_1c;
  
  pIStack_1c = (IDirectSoundBuffer *)0x0;
  sStack_30 = 100;
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
  ppIVar7 = (IDirectSoundBuffer **)0x4a;
  if ((sampleAsset->magic == ASSET_MAGIC_SAM) && (sampleAsset->formatVersion == 0x10000)) {
    WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat_PCM_22050_Stereo16.nChannels = 2;
    WaveFormat_PCM_22050_Stereo16.nSamplesPerSec._0_2_ = 0x5622;
    WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = 0x15888;
    WaveFormat_PCM_22050_Stereo16.nBlockAlign = 4;
    WaveFormat_PCM_22050_Stereo16.wBitsPerSample = 0x10;
    PrimarySoundBufferDesc.dwBufferBytes = sampleAsset->decodedBlockCount << 10;
    PrimarySoundBufferDesc.dwSize = 0x14;
    PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
    PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
    TVar2 = (*g_DirectSound->lpVtbl->CreateSoundBuffer)
                      (g_DirectSound,&PrimarySoundBufferDesc,&pIStack_1c,(TH_LEGACY_LPVOID)0x0);
    ppIVar7 = (IDirectSoundBuffer **)0x29;
    if (TVar2 == 0) {
      sStack_30 = 0x65;
      TVar2 = (*pIStack_1c->lpVtbl->Lock)
                        (pIStack_1c,0,0,&psStack_20,&uStack_24,&pvStack_28,&TStack_2c,2);
      ppIVar7 = (IDirectSoundBuffer **)0x29;
      if (TVar2 == 0) {
        sStack_30 = 0x66;
        encodedBlock = sampleAsset + 1;
        uVar4 = uStack_24 >> 10;
        outputStereoPcm = psStack_20;
        do {
          dVar3 = SoundSample_DecodePackedCoefficientBlock((short *)0x417364,(byte *)encodedBlock);
          SoundSample_DecodeCoefficientBlockToPcmMmx(outputStereoPcm,(short *)0x417364);
          encodedBlock = (SoundSampleAsset *)(encodedBlock->reserved04_0B + (dVar3 - 4));
          outputStereoPcm = outputStereoPcm + 0x200;
          uVar4 = uVar4 - 1;
        } while (uVar4 != 0);
        TVar2 = (*pIStack_1c->lpVtbl->Unlock)(pIStack_1c,psStack_20,uStack_24,pvStack_28,TStack_2c);
        ppIVar7 = (IDirectSoundBuffer **)0x29;
        if (TVar2 == 0) {
          AVar10 = (*g_MemoryApi.alloc)(0x20);
          ppIVar7 = (IDirectSoundBuffer **)AVar10.eax;
          if (!AVar10.carry) {
            iVar5 = 8;
            ppIVar8 = ppIVar7;
            do {
              *ppIVar8 = (IDirectSoundBuffer *)0x0;
              ppDVar9 = g_DirectSoundVoiceSetRegistry;
              ppIVar8 = ppIVar8 + 1;
              iVar5 = iVar5 + -1;
            } while (iVar5 != 0);
            ppDVar6 = (DirectSoundVoiceSet **)0x100;
            *ppIVar7 = pIStack_1c;
            ppDVar1 = ppDVar9;
            do {
              if (ppDVar1 == (DirectSoundVoiceSet **)0x0) {
LAB_00583652:
                return (SoundCreateSampleVoiceSetEaxCf5)((uint5)AVar10 & 0xffffffff);
              }
              if (*ppDVar9 == (DirectSoundVoiceSet *)0x0) {
                *ppDVar9 = (DirectSoundVoiceSet *)ppIVar7;
                goto LAB_00583652;
              }
              ppDVar9 = ppDVar9 + 1;
              ppDVar6 = (DirectSoundVoiceSet **)((int)ppDVar6 + -1);
              ppDVar1 = ppDVar6;
            } while( true );
          }
        }
      }
    }
  }
  if (pIStack_1c != (IDirectSoundBuffer *)0x0) {
    (*pIStack_1c->lpVtbl->Release)(pIStack_1c);
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,sStack_30,g_PackageLastErrorPath);
  SVar11.carry = true;
  SVar11.eax = (DirectSoundVoiceSet *)ppIVar7;
  return SVar11;
}


/* Address: 0x00583690.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes its pointer from the 256-entry registry. Null is accepted. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  DirectSoundVoiceSet **ppDVar1;
  int voicesRemaining;
  DirectSoundVoiceSet **ppDVar2;
  IDirectSoundBuffer **voiceCursor;
  DirectSoundVoiceSet **registryCursor;
  IDirectSoundBuffer *voiceBuffer;
  
  voicesRemaining = 8;
  voiceCursor = voiceSet->voices;
  if (voiceSet != (DirectSoundVoiceSet *)0x0) {
    do {
      voiceBuffer = *voiceCursor;
      if (voiceBuffer != (IDirectSoundBuffer *)0x0) {
        (*voiceBuffer->lpVtbl->Release)(voiceBuffer);
      }
      voiceCursor = voiceCursor + 1;
      voicesRemaining = voicesRemaining + -1;
    } while (voicesRemaining != 0);
    (*g_MemoryApi.free)(voiceSet);
    ppDVar2 = (DirectSoundVoiceSet **)0x100;
    registryCursor = g_DirectSoundVoiceSetRegistry;
    ppDVar1 = g_DirectSoundVoiceSetRegistry;
    while (ppDVar1 != (DirectSoundVoiceSet **)0x0) {
      if (voiceSet == *registryCursor) {
        *registryCursor = (DirectSoundVoiceSet *)0x0;
        return;
      }
      registryCursor = registryCursor + 1;
      ppDVar2 = (DirectSoundVoiceSet **)((int)ppDVar2 + -1);
      ppDVar1 = ppDVar2;
    }
  }
  return;
}


/* Address: 0x00583720.
   Ownership: audio/backend/runtime.
   Purpose: CF clear returns the set pointer in EAX; CF set returns an error code.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
SoundCreatePcmVoiceSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          dword *pcmData)

{
  DirectSoundVoiceSet **ppDVar1;
  sdword sVar2;
  uint uVar3;
  TH_LEGACY_HRESULT TVar4;
  int iVar5;
  DirectSoundVoiceSet **ppDVar6;
  IDirectSoundBuffer **ppIVar7;
  IDirectSoundBuffer **ppIVar8;
  dword *pdVar9;
  DirectSoundVoiceSet **ppDVar10;
  ArenaAllocEaxCf5 AVar11;
  SoundCreatePcmVoiceSetEaxCf5 SVar12;
  sdword sStack_30;
  TH_LEGACY_DWORD TStack_2c;
  TH_LEGACY_LPVOID pvStack_28;
  uint uStack_24;
  dword *pdStack_20;
  IDirectSoundBuffer *pIStack_1c;
  
  pIStack_1c = (IDirectSoundBuffer *)0x0;
  sStack_30 = 100;
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
  WaveFormat_PCM_22050_Stereo16.nChannels = (AudioChannelCount)channelCount;
  WaveFormat_PCM_22050_Stereo16.wBitsPerSample = (AudioBitsPerSample)bitsPerSample;
  uVar3 = bitsPerSample * channelCount >> 3;
  PrimarySoundBufferDesc.dwBufferBytes = bufferByteCount;
  WaveFormat_PCM_22050_Stereo16.nBlockAlign = (AudioBlockAlignBytes)uVar3;
  WaveFormat_PCM_22050_Stereo16.nSamplesPerSec = sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.nAvgBytesPerSec = uVar3 * sampleRateHz;
  WaveFormat_PCM_22050_Stereo16.wFormatTag = WAVE_FORMAT_PCM;
  PrimarySoundBufferDesc.dwSize = 0x14;
  PrimarySoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
  PrimarySoundBufferDesc.lpwfxFormat = &WaveFormat_PCM_22050_Stereo16;
  TVar4 = (*g_DirectSound->lpVtbl->CreateSoundBuffer)
                    (g_DirectSound,&PrimarySoundBufferDesc,&pIStack_1c,(TH_LEGACY_LPVOID)0x0);
  ppIVar7 = (IDirectSoundBuffer **)0x29;
  sVar2 = sStack_30;
  if (TVar4 == 0) {
    TVar4 = (*pIStack_1c->lpVtbl->Lock)
                      (pIStack_1c,0,0,&pdStack_20,&uStack_24,&pvStack_28,&TStack_2c,2);
    ppIVar7 = (IDirectSoundBuffer **)0x29;
    sVar2 = 0x65;
    if (TVar4 == 0) {
      sStack_30 = 0x66;
      pdVar9 = pdStack_20;
      for (uVar3 = uStack_24 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *pdVar9 = *pcmData;
        pcmData = pcmData + 1;
        pdVar9 = pdVar9 + 1;
      }
      TVar4 = (*pIStack_1c->lpVtbl->Unlock)(pIStack_1c,pdStack_20,uStack_24,pvStack_28,TStack_2c);
      ppIVar7 = (IDirectSoundBuffer **)0x29;
      sVar2 = 0x66;
      if (TVar4 == 0) {
        AVar11 = (*g_MemoryApi.alloc)(0x20);
        ppIVar7 = (IDirectSoundBuffer **)AVar11.eax;
        sVar2 = sStack_30;
        if (!AVar11.carry) {
          iVar5 = 8;
          ppIVar8 = ppIVar7;
          do {
            *ppIVar8 = (IDirectSoundBuffer *)0x0;
            ppDVar10 = g_DirectSoundVoiceSetRegistry;
            ppIVar8 = ppIVar8 + 1;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          ppDVar6 = (DirectSoundVoiceSet **)0x100;
          *ppIVar7 = pIStack_1c;
          ppDVar1 = ppDVar10;
          do {
            if (ppDVar1 == (DirectSoundVoiceSet **)0x0) {
LAB_00583892:
              return (SoundCreatePcmVoiceSetEaxCf5)((uint5)AVar11 & 0xffffffff);
            }
            if (*ppDVar10 == (DirectSoundVoiceSet *)0x0) {
              *ppDVar10 = (DirectSoundVoiceSet *)ppIVar7;
              goto LAB_00583892;
            }
            ppDVar10 = ppDVar10 + 1;
            ppDVar6 = (DirectSoundVoiceSet **)((int)ppDVar6 + -1);
            ppDVar1 = ppDVar6;
          } while( true );
        }
      }
    }
  }
  sStack_30 = sVar2;
  if (pIStack_1c != (IDirectSoundBuffer *)0x0) {
    (*pIStack_1c->lpVtbl->Release)(pIStack_1c);
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,sStack_30,g_PackageLastErrorPath);
  SVar12.carry = true;
  SVar12.eax = (DirectSoundVoiceSet *)ppIVar7;
  return SVar12;
}


/* Address: 0x005838D0.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes it from the registry. This is ownership-equivalent to the sample-set release service.
*/
void __thandor_void_preserve_eax_ecx_edx
DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *This;
  DirectSoundVoiceSet **ppDVar1;
  int iVar2;
  DirectSoundVoiceSet **ppDVar3;
  IDirectSoundBuffer **ppIVar4;
  DirectSoundVoiceSet **ppDVar5;
  
  iVar2 = 8;
  ppIVar4 = voiceSet->voices;
  if (voiceSet != (DirectSoundVoiceSet *)0x0) {
    do {
      This = *ppIVar4;
      if (This != (IDirectSoundBuffer *)0x0) {
        (*This->lpVtbl->Release)(This);
      }
      ppIVar4 = ppIVar4 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    (*g_MemoryApi.free)(voiceSet);
    ppDVar3 = (DirectSoundVoiceSet **)0x100;
    ppDVar5 = g_DirectSoundVoiceSetRegistry;
    ppDVar1 = g_DirectSoundVoiceSetRegistry;
    while (ppDVar1 != (DirectSoundVoiceSet **)0x0) {
      if (voiceSet == *ppDVar5) {
        *ppDVar5 = (DirectSoundVoiceSet *)0x0;
        return;
      }
      ppDVar5 = ppDVar5 + 1;
      ppDVar3 = (DirectSoundVoiceSet **)((int)ppDVar3 + -1);
      ppDVar1 = ppDVar3;
    }
  }
  return;
}


/* Address: 0x00583940.
   Ownership: audio/backend/runtime.
   Purpose: Finds a non-playing voice or duplicates voices[0] into an empty slot, starts playback without
   DSBPLAY_LOOPING, converts both 0..0x8000 gain inputs through the 129-entry attenuation table, and applies
   overall volume plus signed pan. CF clear returns the selected IDirectSoundBuffer in EAX; CF set returns zero
   when all eight voices are busy or duplication fails.
*/
SoundPlayVoiceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  IDirectSound_Vtbl *pIVar1;
  DirectSoundVoiceSet *pDVar2;
  IDirectSoundBuffer_Vtbl *This;
  TH_LEGACY_HRESULT TVar3;
  DirectSoundVoiceSet *pDVar4;
  SoundPlayVoiceEaxCf5 SVar5;
  SoundPlayVoiceEaxCf5 SVar7;
  SoundPlayVoiceEaxCf5 SVar8;
  int *piVar9;
  IDirectSound *pIVar10;
  DirectSoundVoiceSet *arg2;
  uint uStack_1c;
  SoundPlayVoiceEaxCf5 SVar6;
  
  pDVar4 = (DirectSoundVoiceSet *)0x8;
  arg2 = voiceSet;
  pDVar2 = voiceSet;
  while( true ) {
    if (pDVar2 == (DirectSoundVoiceSet *)0x0) {
      SVar5.eax = (IDirectSoundBuffer *)0x0;
      SVar5.carry = true;
      return SVar5;
    }
    This = (IDirectSoundBuffer_Vtbl *)arg2->voices[0];
    if (This == (IDirectSoundBuffer_Vtbl *)0x0) break;
    (*((IDirectSoundBuffer_Vtbl *)This->QueryInterface)->GetStatus)
              ((IDirectSoundBuffer *)This,&uStack_1c);
    if ((uStack_1c & 1) == 0) goto DirectSound_PlayOneShot_UseIdleOrDuplicatedVoice;
    arg2 = (DirectSoundVoiceSet *)(arg2->voices + 1);
    pDVar4 = (DirectSoundVoiceSet *)((int)pDVar4[-1].voices + 0x1f);
    pDVar2 = pDVar4;
  }
  pIVar10 = g_DirectSound;
  TVar3 = (*g_DirectSound->lpVtbl->DuplicateSoundBuffer)
                    (g_DirectSound,voiceSet->voices[0],arg2->voices);
  if (TVar3 != 0) {
    SVar6.eax = (IDirectSoundBuffer *)0x0;
    SVar6.carry = true;
    return SVar6;
  }
  pIVar1 = pIVar10->lpVtbl;
  This = (IDirectSoundBuffer_Vtbl *)arg2;
  (**(code **)(pIVar1->QueryInterface + 0x34))(pIVar1,0,pIVar1);
DirectSound_PlayOneShot_UseIdleOrDuplicatedVoice:
  (*((IDirectSoundBuffer_Vtbl *)This->QueryInterface)->Play)((IDirectSoundBuffer *)This,0,0,0);
  if (leftChannelGainQ15 <= rightChannelGainQ15) {
    piVar9 = (int *)g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
    (*((IDirectSoundBuffer *)This)->lpVtbl->SetVolume)
              ((IDirectSoundBuffer *)This,(TH_LEGACY_LONG)piVar9);
    (**(code **)(*piVar9 + 0x40))();
    SVar7.carry = false;
    SVar7.eax = (IDirectSoundBuffer *)piVar9;
    return SVar7;
  }
  piVar9 = (int *)g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8];
  (*((IDirectSoundBuffer *)This)->lpVtbl->SetVolume)
            ((IDirectSoundBuffer *)This,(TH_LEGACY_LONG)piVar9);
  (**(code **)(*piVar9 + 0x40))();
  SVar8.carry = false;
  SVar8.eax = (IDirectSoundBuffer *)piVar9;
  return SVar8;
}


/* Address: 0x00583A70.
   Ownership: audio/backend/runtime.
   Purpose: Finds a non-playing voice or duplicates voices[0], starts playback with DSBPLAY_LOOPING, and applies
   the same gain-to-volume/pan conversion as the one-shot path. CF clear returns the selected IDirectSoundBuffer.
*/
SoundPlayVoiceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectSound_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  IDirectSound_Vtbl *pIVar1;
  IDirectSoundBuffer_Vtbl *This;
  TH_LEGACY_HRESULT TVar2;
  int iVar3;
  DirectSoundVoiceSet *arg2;
  SoundPlayVoiceEaxCf5 SVar4;
  SoundPlayVoiceEaxCf5 SVar6;
  SoundPlayVoiceEaxCf5 SVar7;
  int *piVar8;
  IDirectSound *pIVar9;
  uint uStack_1c;
  SoundPlayVoiceEaxCf5 SVar5;
  
  iVar3 = 8;
  arg2 = voiceSet;
  while (This = (IDirectSoundBuffer_Vtbl *)arg2->voices[0], This != (IDirectSoundBuffer_Vtbl *)0x0)
  {
    (*((IDirectSoundBuffer_Vtbl *)This->QueryInterface)->GetStatus)
              ((IDirectSoundBuffer *)This,&uStack_1c);
    if ((uStack_1c & 1) == 0) goto DirectSound_PlayLooping_UseIdleOrDuplicatedVoice;
    arg2 = (DirectSoundVoiceSet *)(arg2->voices + 1);
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      SVar4.eax = (IDirectSoundBuffer *)0x0;
      SVar4.carry = true;
      return SVar4;
    }
  }
  pIVar9 = g_DirectSound;
  TVar2 = (*g_DirectSound->lpVtbl->DuplicateSoundBuffer)
                    (g_DirectSound,voiceSet->voices[0],arg2->voices);
  if (TVar2 != 0) {
    SVar5.eax = (IDirectSoundBuffer *)0x0;
    SVar5.carry = true;
    return SVar5;
  }
  pIVar1 = pIVar9->lpVtbl;
  This = (IDirectSoundBuffer_Vtbl *)arg2;
  (**(code **)(pIVar1->QueryInterface + 0x34))(pIVar1,0,pIVar1);
DirectSound_PlayLooping_UseIdleOrDuplicatedVoice:
  (*((IDirectSoundBuffer_Vtbl *)This->QueryInterface)->Play)((IDirectSoundBuffer *)This,0,0,1);
  if (rightChannelGainQ15 < leftChannelGainQ15) {
    piVar8 = (int *)g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8];
    (*((IDirectSoundBuffer *)This)->lpVtbl->SetVolume)
              ((IDirectSoundBuffer *)This,(TH_LEGACY_LONG)piVar8);
    (**(code **)(*piVar8 + 0x40))();
    SVar7.carry = false;
    SVar7.eax = (IDirectSoundBuffer *)piVar8;
    return SVar7;
  }
  piVar8 = (int *)g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
  (*((IDirectSoundBuffer *)This)->lpVtbl->SetVolume)
            ((IDirectSoundBuffer *)This,(TH_LEGACY_LONG)piVar8);
  (**(code **)(*piVar8 + 0x40))();
  SVar6.carry = false;
  SVar6.eax = (IDirectSoundBuffer *)piVar8;
  return SVar6;
}


/* Address: 0x00583B90.
   Ownership: audio/backend/runtime.
   Purpose: Calls IDirectSoundBuffer::Stop for a non-null voice. Null is accepted and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopVoice(IDirectSoundBuffer *voice)

{
  if (voice != (IDirectSoundBuffer *)0x0) {
    (*voice->lpVtbl->Stop)(voice);
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
  bool bVar1;
  TH_LEGACY_DWORD voiceStatusFlags;
  
  bVar1 = true;
  if (voice != (IDirectSoundBuffer *)0x0) {
    (*voice->lpVtbl->GetStatus)(voice,&voiceStatusFlags);
    bVar1 = (voiceStatusFlags & 1) == 0;
  }
  return bVar1;
}


/* Address: 0x00583C00.
   Ownership: audio/backend/runtime.
   Purpose: Walks all 256 registered DirectSoundVoiceSet pointers and calls Stop for every non-null voice in each
   eight-pointer set. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx DirectSound_StopAllVoices(void)

{
  IDirectSoundBuffer *This;
  DirectSoundVoiceSet **ppDVar1;
  IDirectSoundBuffer **ppIVar2;
  DirectSoundVoiceSet **ppDVar3;
  IDirectSoundBuffer **ppIVar4;
  IDirectSoundBuffer **ppIVar5;
  DirectSoundVoiceSet **ppDVar6;
  
  ppDVar3 = (DirectSoundVoiceSet **)0x100;
  ppDVar6 = g_DirectSoundVoiceSetRegistry;
  ppDVar1 = g_DirectSoundVoiceSetRegistry;
  while (ppDVar1 != (DirectSoundVoiceSet **)0x0) {
    ppIVar4 = (IDirectSoundBuffer **)0x8;
    ppIVar5 = (*ppDVar6)->voices;
    ppIVar2 = ppIVar5;
    while (ppIVar2 != (IDirectSoundBuffer **)0x0) {
      This = *ppIVar5;
      if (This != (IDirectSoundBuffer *)0x0) {
        (*This->lpVtbl->Stop)(This);
      }
      ppIVar5 = ppIVar5 + 1;
      ppIVar4 = (IDirectSoundBuffer **)((int)ppIVar4 + -1);
      ppIVar2 = ppIVar4;
    }
    ppDVar6 = ppDVar6 + 1;
    ppDVar3 = (DirectSoundVoiceSet **)((int)ppDVar3 + -1);
    ppDVar1 = ppDVar3;
  }
  return;
}


/* Address: 0x00583C60.
   Ownership: audio/backend/runtime.
   Purpose: Consumes one voice pointer and returns EDX:EAX equal to zero. No executable call site references this
   service slot, so the higher-level query semantics remain unresolved.
*/
qword DirectSound_QueryVoiceRegsStub(IDirectSoundBuffer *voice)

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
  int panAttenuation;
  int negativePanAttenuation;
  
  if (rightChannelGainQ15 < leftChannelGainQ15) {
    negativePanAttenuation =
         -(g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8] -
          g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
    if (voice != (IDirectSoundBuffer *)0x0) {
      (*voice->lpVtbl->SetVolume)(voice,g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
      (*voice->lpVtbl->SetPan)(voice,negativePanAttenuation);
      return;
    }
  }
  else {
    panAttenuation =
         g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8] -
         g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
    if (voice != (IDirectSoundBuffer *)0x0) {
      (*voice->lpVtbl->SetVolume)(voice,g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8]);
      (*voice->lpVtbl->SetPan)(voice,panAttenuation);
    }
  }
  return;
}


/* Address: 0x004175F0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend stop-all no-op. CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx SoundBackendDisabled_StopAllVoices(void)

{
  return;
}

