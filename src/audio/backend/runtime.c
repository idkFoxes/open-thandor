#include <thandor/audio/backend/runtime.h>

/* Implementation ownership: audio/backend/runtime. */

/* Address: 0x00583410.
   Ownership: audio/backend/runtime.
   Purpose: Handles direct sound shutdown.
   Local calls: SoundBackendDisabled_StopAllVoices.
*/
void __cdecl DirectSound_Shutdown(void)

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
DirectSoundVoiceSet * SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  return (DirectSoundVoiceSet *)0xffffffff;
}

/* Address: 0x00417580.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend no-op release. CF is cleared.
*/
void SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}

/* Address: 0x00417590.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend placeholder. Returns pointer value 0xFFFFFFFF with CF clear and consumes the five raw-
   PCM arguments.
*/
DirectSoundVoiceSet *
SoundBackendDisabled_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  return (DirectSoundVoiceSet *)0xffffffff;
}

/* Address: 0x004175A0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend no-op release. CF is cleared.
*/
void SoundBackendDisabled_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}

/* Address: 0x004175B0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend one-shot placeholder. It only clears CF and leaves EAX unchanged.
*/
IDirectSoundBuffer *
SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *in_EAX;
  
  return in_EAX;
}

/* Address: 0x004175C0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend looping placeholder. It only clears CF and leaves EAX unchanged.
*/
IDirectSoundBuffer *
SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *in_EAX;
  
  return in_EAX;
}

/* Address: 0x004175D0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend stop no-op. CF is cleared.
*/
void SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice)

{
  return;
}

/* Address: 0x004175E0.
   Ownership: audio/backend/runtime.
   Purpose: Disabled-backend playing query. It always sets CF, meaning not playing.
*/
void SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  return;
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
void SoundBackendDisabled_SetVoiceGains
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
dword __cdecl DirectSound_Init(void)

{
  HINSTANCE module;
  HINSTANCE pHVar1;
  dword dVar2;
  TH_LEGACY_HRESULT TVar3;
  DirectSoundVoiceSet **ppDVar4;
  int iVar5;
  HINSTANCE extraout_EDX;
  bool bVar6;
  sdword local_1c;
  
  bVar6 = &stack0xffffffe8 < (undefined1 *)0x4;
  local_1c = 0;
  module = DynDLL_Load(dynapi_4);
  pHVar1 = module;
  if ((((!bVar6) && (pHVar1 = DynAPI_Resolve(&pDirectSoundCreate,module,dynapi_20), !bVar6)) &&
      (pHVar1 = DynAPI_Resolve(&pDirectSoundEnumerateA,module,dynapi_21), !bVar6)) &&
     ((pHVar1 = DynAPI_Resolve(&pDirectSoundCaptureCreate,module,dynapi_22), !bVar6 &&
      (pHVar1 = DynAPI_Resolve(&pDirectSoundCaptureEnumerateA,module,dynapi_23), !bVar6)))) {
    dVar2 = (*pDirectSoundCreate)((TH_LEGACY_GUID *)0x0,&g_DirectSound,(TH_LEGACY_LPVOID)0x0);
    if (dVar2 != 0) {
      return dVar2;
    }
    TVar3 = (*g_DirectSound->lpVtbl->SetCooperativeLevel)(g_DirectSound,g_MainWindow,3);
    if (TVar3 == 0) {
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
      TVar3 = (*g_DirectSound->lpVtbl->CreateSoundBuffer)
                        (g_DirectSound,&PrimarySoundBufferDesc,&g_PrimarySoundBuffer,
                         (TH_LEGACY_LPVOID)0x0);
      if (TVar3 == 0) {
        local_1c = 2;
        TVar3 = (*g_PrimarySoundBuffer->lpVtbl->SetFormat)
                          (g_PrimarySoundBuffer,&WaveFormat_PCM_22050_Stereo16);
        if (TVar3 == 0) {
          local_1c = 3;
          TVar3 = (*g_PrimarySoundBuffer->lpVtbl->GetVolume)(g_PrimarySoundBuffer,&g_PrimaryVolume);
          if (TVar3 == 0) {
            local_1c = 4;
            TVar3 = (*g_PrimarySoundBuffer->lpVtbl->GetPan)(g_PrimarySoundBuffer,&g_PrimaryPan);
            if (TVar3 == 0) {
              local_1c = 5;
              TVar3 = (*g_PrimarySoundBuffer->lpVtbl->SetVolume)(g_PrimarySoundBuffer,0);
              if (TVar3 == 0) {
                local_1c = 6;
                TVar3 = (*g_PrimarySoundBuffer->lpVtbl->SetPan)(g_PrimarySoundBuffer,0);
                if (TVar3 == 0) {
                  local_1c = 7;
                  TVar3 = (*g_PrimarySoundBuffer->lpVtbl->Play)(g_PrimarySoundBuffer,0,0,1);
                  bVar6 = false;
                  if (TVar3 == 0) {
                    ppDVar4 = (*g_MemoryApi.alloc)(0x400);
                    if (!bVar6) {
                      g_DirectSoundVoiceSetRegistry = ppDVar4;
                      for (iVar5 = 0x100; iVar5 != 0; iVar5 = iVar5 + -1) {
                        *ppDVar4 = (DirectSoundVoiceSet *)0x0;
                        ppDVar4 = ppDVar4 + 1;
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
                      dVar2 = CosineDerivedLookupTables_InitCf();
                      return dVar2;
                    }
                    return (dword)ppDVar4;
                  }
                }
              }
            }
          }
        }
      }
    }
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,local_1c,g_PackageLastErrorPath);
    pHVar1 = extraout_EDX;
  }
  return (dword)pHVar1;
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
DirectSoundVoiceSet * DirectSound_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset)

{
  DirectSoundVoiceSet **ppDVar1;
  TH_LEGACY_HRESULT TVar2;
  int extraout_EAX;
  IDirectSoundBuffer **ppIVar3;
  int extraout_ECX;
  DirectSoundVoiceSet **ppDVar4;
  IDirectSoundBuffer **ppIVar5;
  SoundSampleAsset *encodedBlock;
  short *outputStereoPcm;
  DirectSoundVoiceSet **ppDVar6;
  bool bVar7;
  DirectSoundVoiceSet *pDVar8;
  int iVar9;
  TH_LEGACY_DWORD TStack_2c;
  TH_LEGACY_LPVOID pvStack_28;
  TH_LEGACY_DWORD TStack_24;
  short *psStack_20;
  IDirectSoundBuffer *pIStack_1c;
  
  pIStack_1c = (IDirectSoundBuffer *)0x0;
  iVar9 = 100;
  Memory_ZeroDwords(0x14,&WaveFormat_PCM_22050_Stereo16);
  Memory_ZeroDwords(0x14,&PrimarySoundBufferDesc);
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
    if (TVar2 == 0) {
      iVar9 = iVar9 + 1;
      TVar2 = (*pIStack_1c->lpVtbl->Lock)
                        (pIStack_1c,0,0,&psStack_20,&TStack_24,&pvStack_28,&TStack_2c,2);
      if (TVar2 == 0) {
        iVar9 = iVar9 + 1;
        encodedBlock = sampleAsset + 1;
        outputStereoPcm = psStack_20;
        do {
          SoundSample_DecodePackedCoefficientBlock((short *)0x417364,(byte *)encodedBlock);
          SoundSample_DecodeCoefficientBlockToPcmMmx(outputStereoPcm,(short *)0x417364);
          encodedBlock = (SoundSampleAsset *)(encodedBlock->reserved04_0B + extraout_EAX + -4);
          outputStereoPcm = outputStereoPcm + 0x200;
        } while (extraout_ECX != 1);
        TVar2 = (*pIStack_1c->lpVtbl->Unlock)(pIStack_1c,psStack_20,TStack_24,pvStack_28,TStack_2c);
        bVar7 = false;
        if ((TVar2 == 0) && (ppIVar3 = (*g_MemoryApi.alloc)(0x20), !bVar7)) {
          iVar9 = 8;
          ppIVar5 = ppIVar3;
          do {
            *ppIVar5 = (IDirectSoundBuffer *)0x0;
            ppDVar6 = g_DirectSoundVoiceSetRegistry;
            ppIVar5 = ppIVar5 + 1;
            iVar9 = iVar9 + -1;
          } while (iVar9 != 0);
          ppDVar4 = (DirectSoundVoiceSet **)0x100;
          *ppIVar3 = pIStack_1c;
          ppDVar1 = ppDVar6;
          while( true ) {
            if (ppDVar1 == (DirectSoundVoiceSet **)0x0) {
              return (DirectSoundVoiceSet *)ppIVar3;
            }
            if (*ppDVar6 == (DirectSoundVoiceSet *)0x0) break;
            ppDVar6 = ppDVar6 + 1;
            ppDVar4 = (DirectSoundVoiceSet **)((int)ppDVar4 + -1);
            ppDVar1 = ppDVar4;
          }
          *ppDVar6 = (DirectSoundVoiceSet *)ppIVar3;
          return (DirectSoundVoiceSet *)ppIVar3;
        }
      }
    }
  }
  if (pIStack_1c != (IDirectSoundBuffer *)0x0) {
    (*pIStack_1c->lpVtbl->Release)(pIStack_1c);
  }
  pDVar8 = (DirectSoundVoiceSet *)0x40;
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar9,g_PackageLastErrorPath);
  return pDVar8;
}

/* Address: 0x00583690.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes its pointer from the 256-entry registry. Null is accepted. CF is cleared.
*/
void DirectSound_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

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
DirectSoundVoiceSet *
DirectSound_CreatePcmVoiceSet
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  DirectSoundVoiceSet **ppDVar1;
  sdword sVar2;
  uint uVar3;
  TH_LEGACY_HRESULT TVar4;
  IDirectSoundBuffer **ppIVar5;
  int iVar6;
  DirectSoundVoiceSet **ppDVar7;
  IDirectSoundBuffer **ppIVar8;
  undefined4 *puVar9;
  DirectSoundVoiceSet **ppDVar10;
  bool bVar11;
  DirectSoundVoiceSet *pDVar12;
  sdword sStack_30;
  TH_LEGACY_DWORD TStack_2c;
  TH_LEGACY_LPVOID pvStack_28;
  uint uStack_24;
  undefined4 *puStack_20;
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
  sVar2 = sStack_30;
  if (TVar4 == 0) {
    TVar4 = (*pIStack_1c->lpVtbl->Lock)
                      (pIStack_1c,0,0,&puStack_20,&uStack_24,&pvStack_28,&TStack_2c,2);
    sVar2 = 0x65;
    if (TVar4 == 0) {
      sStack_30 = 0x66;
      puVar9 = puStack_20;
      for (uVar3 = uStack_24 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar9 = *(undefined4 *)pcmData;
        pcmData = (undefined4 *)((int)pcmData + 4);
        puVar9 = puVar9 + 1;
      }
      TVar4 = (*pIStack_1c->lpVtbl->Unlock)(pIStack_1c,puStack_20,uStack_24,pvStack_28,TStack_2c);
      bVar11 = false;
      sVar2 = 0x66;
      if ((TVar4 == 0) && (ppIVar5 = (*g_MemoryApi.alloc)(0x20), sVar2 = sStack_30, !bVar11)) {
        iVar6 = 8;
        ppIVar8 = ppIVar5;
        do {
          *ppIVar8 = (IDirectSoundBuffer *)0x0;
          ppDVar10 = g_DirectSoundVoiceSetRegistry;
          ppIVar8 = ppIVar8 + 1;
          iVar6 = iVar6 + -1;
        } while (iVar6 != 0);
        ppDVar7 = (DirectSoundVoiceSet **)0x100;
        *ppIVar5 = pIStack_1c;
        ppDVar1 = ppDVar10;
        while( true ) {
          if (ppDVar1 == (DirectSoundVoiceSet **)0x0) {
            return (DirectSoundVoiceSet *)ppIVar5;
          }
          if (*ppDVar10 == (DirectSoundVoiceSet *)0x0) break;
          ppDVar10 = ppDVar10 + 1;
          ppDVar7 = (DirectSoundVoiceSet **)((int)ppDVar7 + -1);
          ppDVar1 = ppDVar7;
        }
        *ppDVar10 = (DirectSoundVoiceSet *)ppIVar5;
        return (DirectSoundVoiceSet *)ppIVar5;
      }
    }
  }
  sStack_30 = sVar2;
  if (pIStack_1c != (IDirectSoundBuffer *)0x0) {
    (*pIStack_1c->lpVtbl->Release)(pIStack_1c);
  }
  pDVar12 = (DirectSoundVoiceSet *)0x40;
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,sStack_30,g_PackageLastErrorPath);
  return pDVar12;
}

/* Address: 0x005838D0.
   Ownership: audio/backend/runtime.
   Purpose: Releases every non-null IDirectSoundBuffer in the eight-voice set, frees the set allocation, and
   removes it from the registry. This is ownership-equivalent to the sample-set release service.
*/
void DirectSound_ReleasePcmVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  int *piVar1;
  DirectSoundVoiceSet **ppDVar2;
  int iVar3;
  DirectSoundVoiceSet **ppDVar4;
  undefined4 in_EDX;
  undefined4 unaff_EBX;
  undefined1 *puVar5;
  undefined4 unaff_ESI;
  DirectSoundVoiceSet **ppDVar6;
  
  puVar5 = &stack0xfffffff0;
  iVar3 = 8;
  if (voiceSet != (DirectSoundVoiceSet *)0x0) {
    do {
      piVar1 = *(int **)voiceSet;
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 8))
                  (piVar1,voiceSet,unaff_ESI,puVar5,&stack0xfffffff0,unaff_EBX,in_EDX,iVar3,piVar1);
      }
      voiceSet = (DirectSoundVoiceSet *)((int)voiceSet + 4);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    (*g_MemoryApi.free)(*(void **)(puVar5 + 0x14));
    ppDVar4 = (DirectSoundVoiceSet **)0x100;
    ppDVar6 = g_DirectSoundVoiceSetRegistry;
    ppDVar2 = g_DirectSoundVoiceSetRegistry;
    while (ppDVar2 != (DirectSoundVoiceSet **)0x0) {
      if (*(DirectSoundVoiceSet **)(puVar5 + 0x14) == *ppDVar6) {
        *ppDVar6 = (DirectSoundVoiceSet *)0x0;
        return;
      }
      ppDVar6 = ppDVar6 + 1;
      ppDVar4 = (DirectSoundVoiceSet **)((int)ppDVar4 + -1);
      ppDVar2 = ppDVar4;
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
IDirectSoundBuffer *
DirectSound_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  DirectSoundVoiceSet *pDVar1;
  IDirectSoundBuffer *This;
  TH_LEGACY_HRESULT TVar2;
  DirectSoundVoiceSet *pDVar3;
  int iVar4;
  IDirectSoundBuffer **arg2;
  uint uStack_1c;
  
  pDVar3 = (DirectSoundVoiceSet *)0x8;
  arg2 = voiceSet->voices;
  pDVar1 = voiceSet;
  while( true ) {
    if (pDVar1 == (DirectSoundVoiceSet *)0x0) {
      return (IDirectSoundBuffer *)0x0;
    }
    This = *arg2;
    if (This == (IDirectSoundBuffer *)0x0) break;
    (*This->lpVtbl->GetStatus)(This,&uStack_1c);
    if ((uStack_1c & 1) == 0) goto DirectSound_PlayOneShot_UseIdleOrDuplicatedVoice;
    arg2 = ((DirectSoundVoiceSet *)arg2)->voices + 1;
    pDVar3 = (DirectSoundVoiceSet *)((int)pDVar3[-1].voices + 0x1f);
    pDVar1 = pDVar3;
  }
  TVar2 = (*g_DirectSound->lpVtbl->DuplicateSoundBuffer)(g_DirectSound,voiceSet->voices[0],arg2);
  if (TVar2 != 0) {
    return (IDirectSoundBuffer *)0x0;
  }
  This = *arg2;
  (*This->lpVtbl->SetCurrentPosition)(This,0);
DirectSound_PlayOneShot_UseIdleOrDuplicatedVoice:
  (*This->lpVtbl->Play)(This,0,0,0);
  if (leftChannelGainQ15 <= rightChannelGainQ15) {
    iVar4 = g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8] -
            g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
    (*This->lpVtbl->SetVolume)(This,g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8]);
    (*This->lpVtbl->SetPan)(This,iVar4);
    return This;
  }
  iVar4 = -(g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8] -
           g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
  (*This->lpVtbl->SetVolume)(This,g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
  (*This->lpVtbl->SetPan)(This,iVar4);
  return This;
}

/* Address: 0x00583A70.
   Ownership: audio/backend/runtime.
   Purpose: Finds a non-playing voice or duplicates voices[0], starts playback with DSBPLAY_LOOPING, and applies
   the same gain-to-volume/pan conversion as the one-shot path. CF clear returns the selected IDirectSoundBuffer.
*/
IDirectSoundBuffer *
DirectSound_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet)

{
  IDirectSoundBuffer *This;
  TH_LEGACY_HRESULT TVar1;
  int iVar2;
  IDirectSoundBuffer **arg2;
  uint uStack_1c;
  
  iVar2 = 8;
  arg2 = voiceSet->voices;
  while (This = *arg2, This != (IDirectSoundBuffer *)0x0) {
    (*This->lpVtbl->GetStatus)(This,&uStack_1c);
    if ((uStack_1c & 1) == 0) goto DirectSound_PlayLooping_UseIdleOrDuplicatedVoice;
    arg2 = ((DirectSoundVoiceSet *)arg2)->voices + 1;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      return (IDirectSoundBuffer *)0x0;
    }
  }
  TVar1 = (*g_DirectSound->lpVtbl->DuplicateSoundBuffer)(g_DirectSound,voiceSet->voices[0],arg2);
  if (TVar1 != 0) {
    return (IDirectSoundBuffer *)0x0;
  }
  This = *arg2;
  (*This->lpVtbl->SetCurrentPosition)(This,0);
DirectSound_PlayLooping_UseIdleOrDuplicatedVoice:
  (*This->lpVtbl->Play)(This,0,0,1);
  if (rightChannelGainQ15 < leftChannelGainQ15) {
    iVar2 = -(g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8] -
             g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
    (*This->lpVtbl->SetVolume)(This,g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8]);
    (*This->lpVtbl->SetPan)(This,iVar2);
    return This;
  }
  iVar2 = g_DirectSoundGainAttenuation[leftChannelGainQ15 >> 8] -
          g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8];
  (*This->lpVtbl->SetVolume)(This,g_DirectSoundGainAttenuation[rightChannelGainQ15 >> 8]);
  (*This->lpVtbl->SetPan)(This,iVar2);
  return This;
}

/* Address: 0x00583B90.
   Ownership: audio/backend/runtime.
   Purpose: Calls IDirectSoundBuffer::Stop for a non-null voice. Null is accepted and CF is cleared.
*/
void DirectSound_StopVoice(IDirectSoundBuffer *voice)

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
void DirectSound_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  TH_LEGACY_DWORD voiceStatusFlags;
  
  if (voice != (IDirectSoundBuffer *)0x0) {
    (*voice->lpVtbl->GetStatus)(voice,&voiceStatusFlags);
  }
  return;
}

/* Address: 0x00583C00.
   Ownership: audio/backend/runtime.
   Purpose: Walks all 256 registered DirectSoundVoiceSet pointers and calls Stop for every non-null voice in each
   eight-pointer set. CF is cleared.
*/
void __cdecl DirectSound_StopAllVoices(void)

{
  int *piVar1;
  bool bVar2;
  DirectSoundVoiceSet **ppDVar3;
  undefined4 uVar4;
  DirectSoundVoiceSet **ppDVar5;
  DirectSoundVoiceSet **ppDVar6;
  IDirectSoundBuffer **ppIVar7;
  undefined4 unaff_EBX;
  undefined4 uVar8;
  undefined4 unaff_EBP;
  IDirectSoundBuffer **ppIVar9;
  DirectSoundVoiceSet **ppDVar10;
  
  ppDVar5 = (DirectSoundVoiceSet **)0x100;
  ppDVar10 = g_DirectSoundVoiceSetRegistry;
  ppDVar3 = g_DirectSoundVoiceSetRegistry;
  while (ppDVar3 != (DirectSoundVoiceSet **)0x0) {
    ppIVar9 = (*ppDVar10)->voices;
    ppDVar3 = ppDVar5;
    uVar4 = unaff_EBX;
    for (ppIVar7 = ppIVar9; bVar2 = ppIVar7 != (IDirectSoundBuffer **)0x0, ppDVar6 = ppDVar5,
        ppIVar7 = (IDirectSoundBuffer **)0x8, uVar8 = unaff_EBX, bVar2;
        ppIVar7 = (IDirectSoundBuffer **)((int)ppIVar7 + -1)) {
      piVar1 = (int *)*ppIVar9;
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x48))
                  (piVar1,ppDVar10,ppIVar9,unaff_EBP,&stack0xffffffec,uVar8,ppIVar7,ppDVar6,piVar1);
      }
      ppIVar9 = ppIVar9 + 1;
      ppDVar3 = ppDVar6;
      uVar4 = uVar8;
    }
    ppDVar10 = ppDVar10 + 1;
    ppDVar5 = (DirectSoundVoiceSet **)((int)ppDVar3 + -1);
    unaff_EBX = uVar4;
    ppDVar3 = ppDVar5;
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
void DirectSound_SetVoiceGains
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
void __cdecl SoundBackendDisabled_StopAllVoices(void)

{
  return;
}
