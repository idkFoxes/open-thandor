/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/playback.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/movie/runtime/playback.h>

/* Implementation ownership: movie/runtime/playback. */

/* Address: 0x004A8040.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode flm buffer from frame provider carry-flag result.
   Local calls: Movie_EncodeFrame4x4Keyframe, Movie_EncodeFrame4x4Delta.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Movie_EncodeFlmBufferFromFrameProviderCf
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *outputBuffer,MovieFrameProviderCfProc *frameProvider)

{
  dword dVar1;
  void *frameToReleaseOrNull;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint *sourcePixels;
  uint *puVar5;
  MovieFrameProviderEaxCf5 MVar6;
  StatusValueEaxCf5 SVar7;
  StatusValueEaxCf5 SVar8;
  void *frameToReleaseOrNull_00;
  
  puVar5 = outputBuffer;
  for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  puVar5[-0x80] = 0x6d6c66;
  puVar5[-0x7f] = 0x200;
  puVar5[-0x7e] = 1;
  puVar5[-0x7d] = 0x20001;
  dVar1 = (*g_LocaleGetPackedCurrentTime)();
  puVar5[-0x7c] = dVar1;
  puVar5[-0x7a] = dVar1;
  puVar5[-0x78] = dVar1;
  dVar1 = (*g_LocaleGetPackedCurrentDate)();
  puVar5[-0x7b] = dVar1;
  puVar5[-0x79] = dVar1;
  puVar5[-0x77] = dVar1;
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(puVar5 + -0x74));
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(puVar5 + -100));
  *(undefined1 *)(puVar5 + -0x40) = 0;
  puVar5[-0x54] = frameWidthPixels;
  puVar5[-0x53] = frameHeightPixels;
  puVar5[-0x52] = 0;
  puVar5[-0x51] = 0;
  MVar6 = (*frameProvider)((void *)0x0);
  frameToReleaseOrNull = MVar6.frameOrError;
  if (!MVar6.carry) {
    uVar4 = 1;
    sourcePixels = (uint *)((int)frameToReleaseOrNull +
                           *(int *)((int)frameToReleaseOrNull +
                                   *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc));
    uVar2 = Movie_EncodeFrame4x4Keyframe(frameHeightPixels,frameWidthPixels,puVar5,sourcePixels);
    puVar5 = (uint *)((int)puVar5 + uVar2);
    frameToReleaseOrNull_00 = frameToReleaseOrNull;
    while( true ) {
      MVar6 = (*frameProvider)((void *)0x0);
      frameToReleaseOrNull = MVar6.frameOrError;
      if (MVar6.carry) break;
      uVar4 = uVar4 + 1;
      uVar2 = Movie_EncodeFrame4x4Delta
                        (frameHeightPixels,frameWidthPixels,puVar5,sourcePixels,
                         (uint *)(*(int *)((int)frameToReleaseOrNull +
                                          *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc) +
                                 (int)frameToReleaseOrNull));
      puVar5 = (uint *)((int)puVar5 + uVar2);
      (*frameProvider)(frameToReleaseOrNull);
    }
    (*frameProvider)(frameToReleaseOrNull_00);
    uVar2 = (int)puVar5 - (int)outputBuffer;
    outputBuffer[0x2e] = uVar4;
    outputBuffer[0x3f] = 0x10;
    outputBuffer[1] = uVar2;
    outputBuffer[0x30] = uVar2;
    if (frameToReleaseOrNull == (void *)0xffffffff) {
      outputBuffer[0x30] = outputBuffer[0x30] - 0x200;
      SVar7.carry = false;
      SVar7.valueOrError = uVar2;
      return SVar7;
    }
  }
  SVar8.carry = true;
  SVar8.valueOrError = (dword)frameToReleaseOrNull;
  return SVar8;
}


/* Address: 0x00563FF0.
   Ownership: movie/runtime/playback.
   Purpose: Increments the playback schedule counter, derives the target from exact groups of eight frames and the
   configured span, advances intermediate eight-frame boundaries and the final target, then invokes the existing
   playback tick helper. EAX is preserved.
   Local calls: MoviePlayback_AdvanceToFrameAndPresent.
   Cross-module calls: InGameRuntime_UpdateSimulationAndNetworkTick [gameplay/session/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx MoviePlayback_AdvanceScheduledFrameAndTick(void)

{
  uint targetFrame;
  uint targetFrame_00;
  
  g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + 1;
  if (g_MoviePlaybackScheduleSpan != 0) {
    targetFrame = (uint)(g_MoviePlaybackScheduleCounter * 8) / g_MoviePlaybackScheduleSpan + 1 +
                  g_MoviePlaybackBaseFrameGroup * 8;
    for (targetFrame_00 = 8; targetFrame_00 < targetFrame; targetFrame_00 = targetFrame_00 + 8) {
      if (g_MoviePlaybackCurrentFrame < targetFrame_00) {
        MoviePlayback_AdvanceToFrameAndPresent(targetFrame_00);
      }
    }
    if (targetFrame != g_MoviePlaybackCurrentFrame) {
      MoviePlayback_AdvanceToFrameAndPresent(targetFrame);
    }
  }
  InGameRuntime_UpdateSimulationAndNetworkTick();
  return;
}


/* Address: 0x004A8590.
   Ownership: movie/runtime/playback.
   Purpose: Opens an FLM from a mounted package or loose path, validates magic/version, chooses one embedded audio
   track, builds a one-subresource gfx-compatible MovieRuntime, and optionally starts the refill worker. CF clear
   means success; EAX returns frameCount and ECX returns frameIntervalMilliseconds.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], Package_FindEntryAcrossMounts
   [assets/package/runtime], Random_NextPrimary [core/math/random].
*/
MovieOpenEaxCf5 __thandor_eax_cf_preserve_edx Movie_Open(MovieOpenFlags movieOpenFlags,word *path)

{
  MovieFileHeader *pMVar1;
  uint uVar2;
  MovieSubresourceCount MVar3;
  MoviePaletteBankCount MVar4;
  MovieAudioGainQ15 MVar5;
  MovieRuntime *pMVar6;
  MovieRuntime *pMVar7;
  HANDLE pvVar8;
  dword dVar9;
  int iVar10;
  MovieFileHeader *byteCount;
  uint uVar11;
  MovieRuntime *pMVar12;
  MovieRuntime *handle;
  AssetMagic *pAVar13;
  bool bVar14;
  FileSystemOpenEaxCf5 FVar15;
  FileSystemSeekEaxCf5 FVar16;
  FileSystemReadEaxCf5 FVar17;
  ArenaAllocEaxCf5 AVar18;
  SoundCreateSampleVoiceSetEaxCf5 SVar19;
  MovieOpenEaxCf5 MVar20;
  MovieOpenEaxCf5 MVar21;
  PackageFindEntryEaxEbxCf9 PVar22;
  LPSECURITY_ATTRIBUTES lpThreadAttributes;
  SIZE_T dwStackSize;
  code *lpStartAddress;
  LPVOID lpParameter;
  DWORD dwCreationFlags;
  MovieStreamByteCount *lpThreadId;
  MovieStreamByteCount local_2c;
  byte *local_28;
  MovieRuntime *local_24;
  MovieRuntime *local_20;
  MovieSharedStreamHandleFlag local_1c;
  MovieRuntime *local_18;
  
  local_1c = 0;
  if (((movieOpenFlags & 0x80000000) == 0) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    FVar15 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (MovieRuntime *)FVar15.eax;
    if (FVar15.carry) goto Movie_OpenResolvePackageOrFallbackStream;
  }
  else {
Movie_OpenResolvePackageOrFallbackStream:
    movieOpenFlags = movieOpenFlags & 0x7fffffff;
    PVar22 = Package_FindEntryAcrossMounts(path);
    if ((PVar22.carry) ||
       (FVar16 = (*g_FileSystemSeekCf)
                           (FILESYSTEM_SEEK_BEGIN,*(int *)(PVar22.eax + 0x1ec) + 0x200,
                            (MovieRuntime *)PVar22.ebx), FVar16.carry)) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      FVar15 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (MovieRuntime *)FVar15.eax;
      if (FVar15.carry) {
        FVar15 = (*g_FileSystemOpenCf)(0,path);
        pMVar6 = (MovieRuntime *)FVar15.eax;
        handle = pMVar6;
        if (FVar15.carry) goto LAB_004a8a0b;
      }
    }
    else {
      local_1c = local_1c + 1;
      handle = (MovieRuntime *)PVar22.ebx;
    }
  }
  pAVar13 = (AssetMagic *)g_PackageScratchBuffer;
  FVar17 = (*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,handle);
  pMVar6 = (MovieRuntime *)FVar17.eax;
  if (!FVar17.carry) {
    pMVar6 = (MovieRuntime *)0x30;
    if ((*pAVar13 == 0x6d6c66) &&
       (dVar9 = *(int *)((int)pAVar13 + 0xc0) + 0x200, *(int *)((int)pAVar13 + 0xc) == 0x20001)) {
      if ((0x3c0000 < dVar9) && (movieOpenFlags != 0)) {
        dVar9 = 0x3c0000;
      }
      AVar18 = (*g_MemoryApi.alloc)(dVar9);
      pMVar6 = (MovieRuntime *)AVar18.eax;
      if (!AVar18.carry) {
        pMVar7 = pMVar6;
        for (iVar10 = 0x80; iVar10 != 0; iVar10 = iVar10 + -1) {
          (pMVar7->textureCommon).magic = *pAVar13;
          pAVar13 = pAVar13 + 1;
          pMVar7 = (MovieRuntime *)&(pMVar7->textureCommon).allocationSizeBytes;
        }
        pMVar1 = pMVar6->fileHeader;
        byteCount = pMVar1;
        if (((MovieFileHeader *)0x3a2000 < pMVar1) && (movieOpenFlags != 0)) {
          byteCount = (MovieFileHeader *)0x3a2000;
        }
        local_2c = (int)pMVar1 - (int)byteCount;
        local_28 = byteCount[-1].reserved100_1FF +
                   (int)(&(pMVar7->textureCommon).buildMetadata + 1) + 0x50;
        local_20 = handle;
        local_18 = pMVar6;
        FVar17 = (*g_FileSystemReadExactCf)((FileIoByteCount)byteCount,pMVar7,handle);
        bVar14 = FVar17.carry;
        pMVar7 = (MovieRuntime *)FVar17.eax;
        if ((bVar14) || (pMVar7 = (MovieRuntime *)(*g_FileSystemGetPositionCf)(handle), bVar14))
        goto Movie_OpenReleaseHeaderAllocationAfterFailure;
        uVar2 = pMVar6->reservedBC;
        uVar11 = 0;
        local_24 = pMVar7;
        if ((uVar2 == 0) || (0xe < uVar2)) {
LAB_004a87c0:
          pMVar12 = (MovieRuntime *)0x0;
Movie_OpenAllocateAndInitializeRuntime:
          dVar9 = pMVar6->subresourceCount * pMVar6->paletteBankCount * 4 + 0x220;
          AVar18 = (*g_MemoryApi.alloc)(dVar9);
          pMVar7 = (MovieRuntime *)AVar18.eax;
          if (!AVar18.carry) {
            g_ActiveMovie = pMVar7;
            if ((local_1c == 0) && (local_2c == 0)) {
              (*g_FileSystemClose)(handle);
            }
            (pMVar7->textureCommon).magic = ASSET_MAGIC_GFX;
            (pMVar7->textureCommon).allocationSizeBytes = dVar9;
            (pMVar7->textureCommon).formatVersion = 1;
            (pMVar7->textureCommon).converterVersion = 0;
            pMVar7->audioVoiceSet = (DirectSoundVoiceSet *)pMVar12;
            pMVar7->activeAudioBuffer = (IDirectSoundBuffer *)0x0;
            MVar3 = pMVar6->subresourceCount;
            MVar4 = pMVar6->paletteBankCount;
            dVar9 = (*g_LocaleGetPackedCurrentTime)();
            (pMVar7->textureCommon).buildMetadata.timestamps.dateValue0 = dVar9;
            (pMVar7->textureCommon).buildMetadata.timestamps.dateValue1 = dVar9;
            (pMVar7->textureCommon).buildMetadata.timestamps.dateValue2 = dVar9;
            dVar9 = (*g_LocaleGetPackedCurrentDate)();
            (pMVar7->textureCommon).buildMetadata.timestamps.timeValue0 = dVar9;
            (pMVar7->textureCommon).buildMetadata.timestamps.timeValue1 = dVar9;
            (pMVar7->textureCommon).buildMetadata.timestamps.timeValue2 = dVar9;
            (*g_LocaleCopyDefaultComputerLabelUtf16)
                      ((pMVar7->textureCommon).buildMetadata.names.producerName);
            (*g_LocaleCopyDefaultComputerLabelUtf16)
                      ((pMVar7->textureCommon).buildMetadata.names.sourceName);
            pMVar7->reserved100_1FF[0] = 0;
            pMVar7->subresourceTableOffset = 0x200;
            pMVar7->paletteBankCount = 0;
            pMVar7->subresourceCount = 1;
            pMVar7->fileHeader = (MovieFileHeader *)pMVar6;
            pMVar7->currentFrameIndex = 0;
            pMVar7->videoStreamOffset = 0x200;
            (pMVar7->sourceEntry).dataOffset = 0x220;
            (pMVar7->sourceEntry).pixelWidth = MVar3;
            (pMVar7->sourceEntry).pixelHeight = MVar4;
            (pMVar7->sourceEntry).logicalWidth = MVar3;
            (pMVar7->sourceEntry).logicalHeight = MVar4;
            (pMVar7->sourceEntry).paletteIndex = -1;
            (pMVar7->sourceEntry).originX = 0;
            (pMVar7->sourceEntry).originY = 0;
            pMVar7->remainingVideoBytes = local_2c;
            pMVar7->streamHandle = local_20;
            pMVar7->loadedVideoEnd = local_28;
            MVar5 = g_MovieDefaultAudioGainQ15;
            pMVar7->streamHandleIsSharedPackage = local_1c;
            pMVar7->openFlags = movieOpenFlags;
            pMVar7->streamFileOffset = (MovieStreamFileOffset)local_24;
            pMVar7->audioGainQ15 = MVar5;
            lpThreadId = &local_2c;
            pMVar7->workerActive = 0;
            pMVar7->streamState = MOVIE_STREAM_IDLE;
            pMVar7->refillSemaphore = (void *)0x0;
            if ((local_2c != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
              pMVar7->workerActive = pMVar7->workerActive + 1;
              dwCreationFlags = 0;
              lpParameter = (LPVOID)0x0;
              lpStartAddress = Movie_StreamWorkerThread;
              dwStackSize = 0;
              lpThreadAttributes = (LPSECURITY_ATTRIBUTES)0x0;
              pvVar8 = CreateSemaphoreA((LPSECURITY_ATTRIBUTES)0x0,0,1,(LPCSTR)0x0);
              pMVar7->refillSemaphore = pvVar8;
              pvVar8 = CreateThread(lpThreadAttributes,dwStackSize,lpStartAddress,lpParameter,
                                    dwCreationFlags,lpThreadId);
              if (pvVar8 == (HANDLE)0x0) {
                pMVar7->workerActive = pMVar7->workerActive - 1;
              }
              else {
                CloseHandle(pvVar8);
              }
            }
            MVar20.carry = false;
            MVar20.eax = pMVar6->subresourceTableOffset;
            return MVar20;
          }
        }
        else {
          if (1 < uVar2) {
            dVar9 = Random_NextPrimary();
            uVar11 = (dVar9 & 0xffff) % uVar2;
          }
          iVar10 = 0;
          pMVar7 = pMVar6;
          for (; uVar11 != 0; uVar11 = uVar11 - 1) {
            iVar10 = iVar10 + pMVar7->currentFrameIndex;
            pMVar7 = (MovieRuntime *)&(pMVar7->textureCommon).allocationSizeBytes;
          }
          dVar9 = pMVar7->currentFrameIndex;
          if (dVar9 == 0) goto LAB_004a87c0;
          FVar16 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_CURRENT,iVar10 + local_2c,handle);
          pMVar7 = (MovieRuntime *)FVar16.eax;
          if (!FVar16.carry) {
            AVar18 = (*g_MemoryApi.alloc)(dVar9);
            pMVar7 = (MovieRuntime *)AVar18.eax;
            if (AVar18.carry) goto Movie_OpenReleaseHeaderAllocationAfterFailure;
            FVar17 = (*g_FileSystemReadExactCf)(dVar9,pMVar7,handle);
            pMVar12 = (MovieRuntime *)FVar17.eax;
            if (FVar17.carry) {
LAB_004a89e8:
              (*g_MemoryApi.free)(pMVar7);
              pMVar7 = pMVar12;
              goto Movie_OpenReleaseHeaderAllocationAfterFailure;
            }
            SVar19 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)pMVar7);
            pMVar12 = (MovieRuntime *)SVar19.eax;
            if (SVar19.carry) goto LAB_004a89e8;
            (*g_MemoryApi.free)(pMVar7);
            goto Movie_OpenAllocateAndInitializeRuntime;
          }
        }
Movie_OpenReleaseHeaderAllocationAfterFailure:
        pMVar6 = pMVar7;
        (*g_MemoryApi.free)(local_18);
      }
    }
  }
  if (local_1c == 0) {
    (*g_FileSystemClose)(handle);
  }
LAB_004a8a0b:
  MVar21.carry = true;
  MVar21.eax = (dword)pMVar6;
  return MVar21;
}


/* Address: 0x004A8A20.
   Ownership: movie/runtime/playback.
   Purpose: Returns the active movie source entry's logical width in EAX and logical height in EDX. Both are zero
   when no movie is open.
*/
MovieFrameDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx Movie_GetFrameDimensions(void)

{
  AssetDimension frameWidth;
  AssetDimension frameHeight;
  
  frameWidth = 0;
  frameHeight = 0;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    frameWidth = (g_ActiveMovie->sourceEntry).pixelWidth;
    frameHeight = (g_ActiveMovie->sourceEntry).pixelHeight;
  }
  return CONCAT44(frameHeight,frameWidth);
}


/* Address: 0x004A8A40.
   Ownership: movie/runtime/playback.
   Purpose: Updates the active movie's equal-channel Q15 one-shot audio gain. Does nothing when no movie is open.
*/
void __thandor_preserve_eax Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15)

{
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    g_ActiveMovie->audioGainQ15 = gainQ15;
  }
  return;
}


/* Address: 0x004A8C00.
   Ownership: movie/runtime/playback.
   Purpose: Semaphore-driven refill worker. It appends at most 0x1E000 video bytes while the bounded buffer is
   below 0x3A2200, advances streamFileOffset/loadedVideoEnd, records read failure as state 2, and clears
   workerActive before returning zero.
*/
dword Movie_StreamWorkerThread(void *unusedThreadContext)

{
  void *handle;
  MovieRuntime *pMVar1;
  uint byteCount;
  FileSystemReadEaxCf5 FVar2;
  
  do {
    do {
      if (g_ActiveMovie == (MovieRuntime *)0x0) {
Movie_StreamWorkerThread_ClearWorkerActiveAndReturn:
        if (g_ActiveMovie != (MovieRuntime *)0x0) {
          g_ActiveMovie->workerActive = 0;
        }
        return 0;
      }
      MsgWaitForMultipleObjects(1,&g_ActiveMovie->refillSemaphore,0,0x100,0);
      pMVar1 = g_ActiveMovie;
      if ((((g_ActiveMovie == (MovieRuntime *)0x0) ||
           (g_ActiveMovie->streamState == MOVIE_STREAM_SHUTDOWN)) ||
          (g_ActiveMovie->workerActive == 0)) || (g_ActiveMovie->remainingVideoBytes == 0))
      goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
    } while (g_ActiveMovie->streamState == MOVIE_STREAM_IDLE);
    byteCount = g_ActiveMovie->remainingVideoBytes;
    if ((uint)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200) {
      handle = g_ActiveMovie->streamHandle;
      if (0x1e000 < byteCount) {
        byteCount = 0x1e000;
      }
      (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,g_ActiveMovie->streamFileOffset,handle);
      FVar2 = (*g_FileSystemReadExactCf)(byteCount,pMVar1->loadedVideoEnd,handle);
      if (FVar2.carry) {
        if (pMVar1->streamState != MOVIE_STREAM_SHUTDOWN) {
          pMVar1->streamState = MOVIE_STREAM_READ_FAILED;
        }
        goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
      }
      pMVar1->remainingVideoBytes = pMVar1->remainingVideoBytes - byteCount;
      pMVar1->streamFileOffset = pMVar1->streamFileOffset + byteCount;
      pMVar1->loadedVideoEnd = pMVar1->loadedVideoEnd + byteCount;
      if ((pMVar1->remainingVideoBytes == 0) && (pMVar1->streamHandleIsSharedPackage == 0)) {
        (*g_FileSystemClose)(handle);
      }
    }
    if ((pMVar1->streamState == MOVIE_STREAM_SHUTDOWN) || (pMVar1->remainingVideoBytes == 0))
    goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
    pMVar1->streamState = MOVIE_STREAM_IDLE;
  } while( true );
}


/* Address: 0x004A8D50.
   Ownership: movie/runtime/playback.
   Purpose: Resets currentFrameIndex and videoStreamOffset to the first frame and stops the active movie audio
   voice. It does not rebuild a discarded streaming prefix.
*/
void Movie_Rewind(void)

{
  MovieRuntime *activeMovie;
  
  activeMovie = g_ActiveMovie;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    g_ActiveMovie->currentFrameIndex = 0;
    activeMovie->videoStreamOffset = 0x200;
    if (activeMovie->activeAudioBuffer != (IDirectSoundBuffer *)0x0) {
      (*g_SoundStopVoice)(activeMovie->activeAudioBuffer);
      activeMovie->activeAudioBuffer = (IDirectSoundBuffer *)0x0;
    }
  }
  return;
}

/* Address: 0x004A8D90.
   Ownership: movie/runtime/playback.
   Purpose: Signals worker shutdown, waits for workerActive to clear, closes the semaphore and owned stream,
   releases the selected audio voice set, frees the FLM buffer and MovieRuntime, and clears g_ActiveMovie.
*/
void __thandor_void_preserve_eax_ecx_edx Movie_Close(void)

{
  MovieRuntime *memory;
  HANDLE currentProcessHandle;
  HANDLE hProcess;
  
  memory = g_ActiveMovie;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_MemoryApi.alloc == ArenaHeap_Alloc) {
      g_ActiveMovie->streamState = MOVIE_STREAM_SHUTDOWN;
      currentProcessHandle = GetCurrentProcess();
      SetPriorityClass(currentProcessHandle,0x20);
      do {
      } while (memory->workerActive != 0);
      if (memory->refillSemaphore != (void *)0x0) {
        CloseHandle(memory->refillSemaphore);
        memory->refillSemaphore = (void *)0x0;
      }
      hProcess = GetCurrentProcess();
      SetPriorityClass(hProcess,0x100);
    }
    g_ActiveMovie = (MovieRuntime *)0x0;
    (*g_MemoryApi.free)(memory->fileHeader);
    if (memory->audioVoiceSet != (DirectSoundVoiceSet *)0x0) {
      (*g_SoundReleaseSampleVoiceSet)(memory->audioVoiceSet);
    }
    if ((memory->remainingVideoBytes != 0) && (memory->streamHandleIsSharedPackage == 0)) {
      (*g_FileSystemClose)(memory->streamHandle);
    }
    (*g_MemoryApi.free)(memory);
  }
  return;
}


/* Address: 0x005657D0.
   Ownership: movie/runtime/playback.
   Purpose: End-movie UI callback. Checks frontend/session mode flags and invokes the corresponding transition or
   close helper.
   Cross-module calls: FrontendClientSession_DecrementTimeoutsAndCompactPlayers [ui/frontend/session],
   FrontendHostSession_TickShutdownOrReadyConsensus [ui/frontend/session].
*/
void __thandor_preserve_eax EndMovieUiRuntime_HandleModeTransitionCf(void *endMovieRuntime)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendClientSession_DecrementTimeoutsAndCompactPlayers();
    }
  }
  else {
    FrontendHostSession_TickShutdownOrReadyConsensus();
  }
  return;
}


/* Address: 0x00565810.
   Ownership: movie/runtime/playback.
   Purpose: End-movie command dispatcher selected by command code and modifier flags. Typed parameters: p0
   modifierFlags→UiKeyboardStateMask_V297, p1 commandCode→UiActionId_V338. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
EndMovieUiRuntime_DispatchCommandByFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime)

{
  uint uVar1;
  UiCommandDispatchRecord *pUVar2;
  UiCommandDispatchRecord *pUVar3;
  
  pUVar3 = g_EndMovieCommandDispatchRecords_00_Code00000071_Modifier30;
  do {
    while( true ) {
      while( true ) {
        while( true ) {
          do {
            pUVar2 = pUVar3;
            uVar1 = pUVar2->modifierClassFlags;
            if (pUVar2->commandCode == 0) {
              return;
            }
            pUVar3 = pUVar2 + 1;
          } while (pUVar2->commandCode != commandCode);
          if (uVar1 != 0) break;
          if ((modifierFlags & 0x3c) == 0) {
            (*(code *)pUVar2->continuationEntryAddress)();
            return;
          }
        }
        if ((uVar1 & 0x30) != 0) break;
        if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) == 0)) {
          (*(code *)pUVar2->continuationEntryAddress)();
          return;
        }
      }
      if ((uVar1 & 0xc) != 0) break;
      if (((modifierFlags & 0xc) == 0) && ((modifierFlags & 0x30) != 0)) {
        (*(code *)pUVar2->continuationEntryAddress)();
        return;
      }
    }
  } while (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0));
  (*(code *)pUVar2->continuationEntryAddress)();
  return;
}


/* Address: 0x005739C0.
   Ownership: movie/runtime/playback.
   Purpose: Periodic timer callback that increments g_IntroMoviePendingTicks. The intro loop decodes at most three
   pending frames per iteration.
*/
void IntroMovie_TimerTick(void)

{
  g_IntroMoviePendingTicks = g_IntroMoviePendingTicks + 1;
  return;
}

/* Address: 0x004A7030.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode frame4x4 keyframe.
   Local calls: MovieColor_ComputeLuma5FromRgb888, MovieColor_ComputeChromaCodeFromRgb888.
*/
uint __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Keyframe
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *encodedOutput,uint *sourcePixels)

{
  PackedRgb24 PVar1;
  PackedRgb24 PVar2;
  PackedRgb24 PVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  PackedRgb24 *pPVar23;
  uint *puVar24;
  undefined1 uVar25;
  undefined1 uVar26;
  undefined1 uVar27;
  undefined1 uVar28;
  undefined1 uVar29;
  undefined1 uVar30;
  undefined1 uVar31;
  undefined1 uVar32;
  ushort uVar33;
  ushort uVar36;
  PackedRgb24 PVar34;
  ushort uVar37;
  undefined8 uVar35;
  ushort uVar38;
  uint uStack_24;
  uint uStack_20;
  
  uStack_20 = frameHeightPixels >> 2;
  puVar24 = encodedOutput;
  uStack_24 = frameWidthPixels >> 2;
  do {
    do {
      PVar34 = *sourcePixels;
      PVar1 = sourcePixels[1];
      PVar2 = sourcePixels[2];
      PVar3 = sourcePixels[3];
      uVar25 = (undefined1)(PVar34 >> 0x18);
      uVar33 = CONCAT11(uVar25,uVar25);
      uVar26 = (undefined1)(PVar34 >> 0x10);
      uVar25 = (undefined1)(PVar34 >> 8);
      uVar27 = (undefined1)(PVar1 >> 0x18);
      uVar36 = CONCAT11(uVar27,uVar27);
      uVar28 = (undefined1)(PVar1 >> 0x10);
      uVar27 = (undefined1)(PVar1 >> 8);
      uVar29 = (undefined1)(PVar2 >> 0x18);
      uVar37 = CONCAT11(uVar29,uVar29);
      uVar30 = (undefined1)(PVar2 >> 0x10);
      uVar29 = (undefined1)(PVar2 >> 8);
      uVar31 = (undefined1)(PVar3 >> 0x18);
      uVar38 = CONCAT11(uVar31,uVar31);
      uVar32 = (undefined1)(PVar3 >> 0x10);
      uVar31 = (undefined1)(PVar3 >> 8);
      uVar35 = CONCAT26((uVar38 >> 6) + (uVar37 >> 6) + (uVar33 >> 6) + (uVar36 >> 6),
                        CONCAT24(((ushort)(CONCAT35(CONCAT21(uVar38,uVar32),CONCAT14(uVar32,PVar3))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar37,uVar30),CONCAT14(uVar30,PVar2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar33,uVar26),CONCAT14(uVar26,PVar34))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar36,uVar28),CONCAT14(uVar28,PVar1))
                                          >> 0x20) >> 6),
                                 CONCAT22((CONCAT11(uVar31,uVar31) >> 6) +
                                          (CONCAT11(uVar29,uVar29) >> 6) +
                                          (CONCAT11(uVar25,uVar25) >> 6) +
                                          (CONCAT11(uVar27,uVar27) >> 6),
                                          (CONCAT11((char)PVar3,(char)PVar3) >> 6) +
                                          (CONCAT11((char)PVar2,(char)PVar2) >> 6) +
                                          (CONCAT11((char)PVar34,(char)PVar34) >> 6) +
                                          (CONCAT11((char)PVar1,(char)PVar1) >> 6))));
      uVar4 = MovieColor_ComputeLuma5FromRgb888(*sourcePixels);
      uVar5 = MovieColor_ComputeLuma5FromRgb888(sourcePixels[1]);
      uVar7 = uVar5;
      if (((int)uVar4 <= (int)uVar5) && (uVar7 = uVar4, (int)uVar4 < (int)uVar5)) {
        uVar4 = uVar5;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(sourcePixels[2]);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(sourcePixels[3]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      pPVar23 = sourcePixels + frameWidthPixels;
      PVar34 = *pPVar23;
      PVar1 = pPVar23[1];
      PVar2 = pPVar23[2];
      PVar3 = pPVar23[3];
      uVar25 = (undefined1)(PVar34 >> 0x18);
      uVar33 = CONCAT11(uVar25,uVar25);
      uVar26 = (undefined1)(PVar34 >> 0x10);
      uVar25 = (undefined1)(PVar34 >> 8);
      uVar27 = (undefined1)(PVar1 >> 0x18);
      uVar36 = CONCAT11(uVar27,uVar27);
      uVar28 = (undefined1)(PVar1 >> 0x10);
      uVar27 = (undefined1)(PVar1 >> 8);
      uVar29 = (undefined1)(PVar2 >> 0x18);
      uVar37 = CONCAT11(uVar29,uVar29);
      uVar30 = (undefined1)(PVar2 >> 0x10);
      uVar29 = (undefined1)(PVar2 >> 8);
      uVar31 = (undefined1)(PVar3 >> 0x18);
      uVar38 = CONCAT11(uVar31,uVar31);
      uVar32 = (undefined1)(PVar3 >> 0x10);
      uVar31 = (undefined1)(PVar3 >> 8);
      uVar35 = CONCAT26((short)((ulonglong)uVar35 >> 0x30) + (uVar33 >> 6) + (uVar36 >> 6) +
                        (uVar37 >> 6) + (uVar38 >> 6),
                        CONCAT24((short)((ulonglong)uVar35 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar33,uVar26),CONCAT14(uVar26,PVar34))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar36,uVar28),CONCAT14(uVar28,PVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar37,uVar30),CONCAT14(uVar30,PVar2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar38,uVar32),CONCAT14(uVar32,PVar3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar35 >> 0x10) +
                                          (CONCAT11(uVar25,uVar25) >> 6) +
                                          (CONCAT11(uVar27,uVar27) >> 6) +
                                          (CONCAT11(uVar29,uVar29) >> 6) +
                                          (CONCAT11(uVar31,uVar31) >> 6),
                                          (short)uVar35 +
                                          (CONCAT11((char)PVar34,(char)PVar34) >> 6) +
                                          (CONCAT11((char)PVar1,(char)PVar1) >> 6) +
                                          (CONCAT11((char)PVar2,(char)PVar2) >> 6) +
                                          (CONCAT11((char)PVar3,(char)PVar3) >> 6))));
      uVar6 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      pPVar23 = pPVar23 + frameWidthPixels;
      PVar34 = *pPVar23;
      PVar1 = pPVar23[1];
      PVar2 = pPVar23[2];
      PVar3 = pPVar23[3];
      uVar25 = (undefined1)(PVar34 >> 0x18);
      uVar33 = CONCAT11(uVar25,uVar25);
      uVar26 = (undefined1)(PVar34 >> 0x10);
      uVar25 = (undefined1)(PVar34 >> 8);
      uVar27 = (undefined1)(PVar1 >> 0x18);
      uVar36 = CONCAT11(uVar27,uVar27);
      uVar28 = (undefined1)(PVar1 >> 0x10);
      uVar27 = (undefined1)(PVar1 >> 8);
      uVar29 = (undefined1)(PVar2 >> 0x18);
      uVar37 = CONCAT11(uVar29,uVar29);
      uVar30 = (undefined1)(PVar2 >> 0x10);
      uVar29 = (undefined1)(PVar2 >> 8);
      uVar31 = (undefined1)(PVar3 >> 0x18);
      uVar38 = CONCAT11(uVar31,uVar31);
      uVar32 = (undefined1)(PVar3 >> 0x10);
      uVar31 = (undefined1)(PVar3 >> 8);
      uVar35 = CONCAT26((short)((ulonglong)uVar35 >> 0x30) + (uVar33 >> 6) + (uVar36 >> 6) +
                        (uVar37 >> 6) + (uVar38 >> 6),
                        CONCAT24((short)((ulonglong)uVar35 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar33,uVar26),CONCAT14(uVar26,PVar34))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar36,uVar28),CONCAT14(uVar28,PVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar37,uVar30),CONCAT14(uVar30,PVar2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar38,uVar32),CONCAT14(uVar32,PVar3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar35 >> 0x10) +
                                          (CONCAT11(uVar25,uVar25) >> 6) +
                                          (CONCAT11(uVar27,uVar27) >> 6) +
                                          (CONCAT11(uVar29,uVar29) >> 6) +
                                          (CONCAT11(uVar31,uVar31) >> 6),
                                          (short)uVar35 +
                                          (CONCAT11((char)PVar34,(char)PVar34) >> 6) +
                                          (CONCAT11((char)PVar1,(char)PVar1) >> 6) +
                                          (CONCAT11((char)PVar2,(char)PVar2) >> 6) +
                                          (CONCAT11((char)PVar3,(char)PVar3) >> 6))));
      uVar6 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      pPVar23 = pPVar23 + frameWidthPixels;
      PVar34 = *pPVar23;
      PVar1 = pPVar23[1];
      PVar2 = pPVar23[2];
      PVar3 = pPVar23[3];
      uVar25 = (undefined1)(PVar34 >> 0x18);
      uVar33 = CONCAT11(uVar25,uVar25);
      uVar26 = (undefined1)(PVar34 >> 0x10);
      uVar25 = (undefined1)(PVar34 >> 8);
      uVar27 = (undefined1)(PVar1 >> 0x18);
      uVar36 = CONCAT11(uVar27,uVar27);
      uVar28 = (undefined1)(PVar1 >> 0x10);
      uVar27 = (undefined1)(PVar1 >> 8);
      uVar29 = (undefined1)(PVar2 >> 0x18);
      uVar37 = CONCAT11(uVar29,uVar29);
      uVar30 = (undefined1)(PVar2 >> 0x10);
      uVar29 = (undefined1)(PVar2 >> 8);
      uVar31 = (undefined1)(PVar3 >> 0x18);
      uVar38 = CONCAT11(uVar31,uVar31);
      uVar32 = (undefined1)(PVar3 >> 0x10);
      uVar31 = (undefined1)(PVar3 >> 8);
      uVar35 = CONCAT26((short)((ulonglong)uVar35 >> 0x30) + (uVar33 >> 6) + (uVar36 >> 6) +
                        (uVar37 >> 6) + (uVar38 >> 6),
                        CONCAT24((short)((ulonglong)uVar35 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar33,uVar26),CONCAT14(uVar26,PVar34))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar36,uVar28),CONCAT14(uVar28,PVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar37,uVar30),CONCAT14(uVar30,PVar2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar38,uVar32),CONCAT14(uVar32,PVar3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar35 >> 0x10) +
                                          (CONCAT11(uVar25,uVar25) >> 6) +
                                          (CONCAT11(uVar27,uVar27) >> 6) +
                                          (CONCAT11(uVar29,uVar29) >> 6) +
                                          (CONCAT11(uVar31,uVar31) >> 6),
                                          (short)uVar35 +
                                          (CONCAT11((char)PVar34,(char)PVar34) >> 6) +
                                          (CONCAT11((char)PVar1,(char)PVar1) >> 6) +
                                          (CONCAT11((char)PVar2,(char)PVar2) >> 6) +
                                          (CONCAT11((char)PVar3,(char)PVar3) >> 6))));
      uVar6 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
      uVar5 = uVar6;
      if (((int)uVar7 <= (int)uVar6) && (uVar5 = uVar7, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
      uVar7 = uVar6;
      if (((int)uVar5 <= (int)uVar6) && (uVar7 = uVar5, (int)uVar4 < (int)uVar6)) {
        uVar4 = uVar6;
      }
      uVar33 = (ushort)uVar35 >> 6;
      uVar36 = (ushort)((ulonglong)uVar35 >> 0x10) >> 6;
      uVar37 = (ushort)((ulonglong)uVar35 >> 0x20) >> 6;
      uVar38 = (ushort)((ulonglong)uVar35 >> 0x36);
      PVar34 = CONCAT13((uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 - (0xff < uVar38),
                        CONCAT12((uVar37 != 0) * (uVar37 < 0x100) * (char)uVar37 - (0xff < uVar37),
                                 CONCAT11((uVar36 != 0) * (uVar36 < 0x100) * (char)uVar36 -
                                          (0xff < uVar36),
                                          (uVar33 != 0) * (uVar33 < 0x100) * (char)uVar33 -
                                          (0xff < uVar33))));
      uVar5 = (int)((uVar7 - 8) + uVar4) >> 1;
      if ((int)uVar5 < 0) {
        uVar5 = 0;
      }
      else if (0x18 < (int)uVar5) {
        uVar5 = 0x18;
      }
      if (uVar4 - uVar7 < 0xc) {
        *puVar24 = uVar5;
        uVar7 = MovieColor_ComputeChromaCodeFromRgb888(PVar34);
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        iVar8 = uVar4 - uVar5;
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        else if (7 < iVar8) {
          iVar8 = 7;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        iVar9 = uVar4 - uVar5;
        if (iVar9 < 0) {
          iVar9 = 0;
        }
        else if (7 < iVar9) {
          iVar9 = 7;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        iVar10 = uVar4 - uVar5;
        if (iVar10 < 0) {
          iVar10 = 0;
        }
        else if (7 < iVar10) {
          iVar10 = 7;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        iVar11 = uVar4 - uVar5;
        if (iVar11 < 0) {
          iVar11 = 0;
        }
        else if (7 < iVar11) {
          iVar11 = 7;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        iVar12 = uVar4 - uVar5;
        if (iVar12 < 0) {
          iVar12 = 0;
        }
        else if (7 < iVar12) {
          iVar12 = 7;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        iVar13 = uVar4 - uVar5;
        if (iVar13 < 0) {
          iVar13 = 0;
        }
        else if (7 < iVar13) {
          iVar13 = 7;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        uVar4 = uVar4 - uVar5;
        if ((int)uVar4 < 0) {
          uVar4 = 0;
        }
        else if (7 < (int)uVar4) {
          uVar4 = 7;
        }
        uVar6 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        puVar24[1] = (uVar7 & 0x7fe0) << 0x10 | iVar8 << 0x12 | iVar9 << 0xf | iVar10 << 0xc |
                     iVar11 << 9 | iVar12 << 6 | iVar13 << 3 | uVar4;
        iVar8 = uVar6 - uVar5;
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        else if (7 < iVar8) {
          iVar8 = 7;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        iVar9 = uVar7 - uVar5;
        if (iVar9 < 0) {
          iVar9 = 0;
        }
        else if (7 < iVar9) {
          iVar9 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        iVar10 = uVar7 - uVar5;
        if (iVar10 < 0) {
          iVar10 = 0;
        }
        else if (7 < iVar10) {
          iVar10 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        iVar11 = uVar7 - uVar5;
        if (iVar11 < 0) {
          iVar11 = 0;
        }
        else if (7 < iVar11) {
          iVar11 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        iVar12 = uVar7 - uVar5;
        if (iVar12 < 0) {
          iVar12 = 0;
        }
        else if (7 < iVar12) {
          iVar12 = 7;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        iVar13 = uVar7 - uVar5;
        if (iVar13 < 0) {
          iVar13 = 0;
        }
        else if (7 < iVar13) {
          iVar13 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        iVar14 = uVar7 - uVar5;
        if (iVar14 < 0) {
          iVar14 = 0;
        }
        else if (7 < iVar14) {
          iVar14 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        iVar15 = uVar7 - uVar5;
        if (iVar15 < 0) {
          iVar15 = 0;
        }
        else if (7 < iVar15) {
          iVar15 = 7;
        }
        uVar7 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        iVar16 = uVar7 - uVar5;
        if (iVar16 < 0) {
          iVar16 = 0;
        }
        else if (7 < iVar16) {
          iVar16 = 7;
        }
        *puVar24 = *puVar24 |
                   iVar8 << 0x1d | iVar9 << 0x1a | iVar10 << 0x17 | iVar11 << 0x14 | iVar12 << 0x11
                   | iVar13 << 0xe | iVar14 << 0xb | iVar15 << 8 | iVar16 << 5;
      }
      else {
        uVar5 = uVar5 - 4;
        if ((int)uVar5 < 0) {
          uVar5 = 0;
        }
        else if (0x10 < (int)uVar5) {
          uVar5 = 0x10;
        }
        *puVar24 = uVar5;
        uVar7 = MovieColor_ComputeChromaCodeFromRgb888(PVar34);
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        uVar4 = uVar4 - uVar5;
        if ((int)uVar4 < 0) {
          uVar4 = 0;
        }
        else if (0xf < (int)uVar4) {
          uVar4 = 0xf;
        }
        uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        uVar6 = uVar6 - uVar5;
        if ((int)uVar6 < 0) {
          uVar6 = 0;
        }
        else if (0xf < (int)uVar6) {
          uVar6 = 0xf;
        }
        uVar17 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        uVar17 = uVar17 - uVar5;
        if ((int)uVar17 < 0) {
          uVar17 = 0;
        }
        else if (0xf < (int)uVar17) {
          uVar17 = 0xf;
        }
        uVar18 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        uVar18 = uVar18 - uVar5;
        if ((int)uVar18 < 0) {
          uVar18 = 0;
        }
        else if (0xf < (int)uVar18) {
          uVar18 = 0xf;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar19 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        uVar19 = uVar19 - uVar5;
        if ((int)uVar19 < 0) {
          uVar19 = 0;
        }
        else if (0xf < (int)uVar19) {
          uVar19 = 0xf;
        }
        uVar20 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        uVar20 = uVar20 - uVar5;
        if ((int)uVar20 < 0) {
          uVar20 = 0;
        }
        else if (0xf < (int)uVar20) {
          uVar20 = 0xf;
        }
        uVar21 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        uVar21 = uVar21 - uVar5;
        if ((int)uVar21 < 0) {
          uVar21 = 0;
        }
        else if (0xf < (int)uVar21) {
          uVar21 = 0xf;
        }
        uVar22 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        puVar24[1] = (uVar7 & 0x7fe0) * 0x10000 + 0x80000000 | (uVar4 >> 1) << 0x12 |
                     (uVar6 >> 1) << 0xf | (uVar17 >> 1) << 0xc | (uVar18 >> 1) << 9 |
                     (uVar19 >> 1) << 6 | (uVar20 >> 1) << 3 | uVar21 >> 1;
        uVar22 = uVar22 - uVar5;
        if ((int)uVar22 < 0) {
          uVar22 = 0;
        }
        else if (0xf < (int)uVar22) {
          uVar22 = 0xf;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar7 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        uVar7 = uVar7 - uVar5;
        if ((int)uVar7 < 0) {
          uVar7 = 0;
        }
        else if (0xf < (int)uVar7) {
          uVar7 = 0xf;
        }
        uVar4 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        uVar4 = uVar4 - uVar5;
        if ((int)uVar4 < 0) {
          uVar4 = 0;
        }
        else if (0xf < (int)uVar4) {
          uVar4 = 0xf;
        }
        uVar6 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        uVar6 = uVar6 - uVar5;
        if ((int)uVar6 < 0) {
          uVar6 = 0;
        }
        else if (0xf < (int)uVar6) {
          uVar6 = 0xf;
        }
        uVar17 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        uVar17 = uVar17 - uVar5;
        if ((int)uVar17 < 0) {
          uVar17 = 0;
        }
        else if (0xf < (int)uVar17) {
          uVar17 = 0xf;
        }
        pPVar23 = pPVar23 + -frameWidthPixels;
        uVar18 = MovieColor_ComputeLuma5FromRgb888(pPVar23[3]);
        uVar18 = uVar18 - uVar5;
        if ((int)uVar18 < 0) {
          uVar18 = 0;
        }
        else if (0xf < (int)uVar18) {
          uVar18 = 0xf;
        }
        uVar19 = MovieColor_ComputeLuma5FromRgb888(pPVar23[2]);
        uVar19 = uVar19 - uVar5;
        if ((int)uVar19 < 0) {
          uVar19 = 0;
        }
        else if (0xf < (int)uVar19) {
          uVar19 = 0xf;
        }
        uVar20 = MovieColor_ComputeLuma5FromRgb888(pPVar23[1]);
        uVar20 = uVar20 - uVar5;
        if ((int)uVar20 < 0) {
          uVar20 = 0;
        }
        else if (0xf < (int)uVar20) {
          uVar20 = 0xf;
        }
        uVar21 = MovieColor_ComputeLuma5FromRgb888(*pPVar23);
        uVar21 = uVar21 - uVar5;
        if ((int)uVar21 < 0) {
          uVar21 = 0;
        }
        else if (0xf < (int)uVar21) {
          uVar21 = 0xf;
        }
        *puVar24 = *puVar24 |
                   (uVar22 >> 1) << 0x1d | (uVar7 >> 1) << 0x1a | (uVar4 >> 1) << 0x17 |
                   (uVar6 >> 1) << 0x14 | (uVar17 >> 1) << 0x11 | (uVar18 >> 1) << 0xe |
                   (uVar19 >> 1) << 0xb | (uVar20 >> 1) << 8 | (uVar21 >> 1) << 5;
      }
      sourcePixels = pPVar23 + 4;
      puVar24 = puVar24 + 2;
      uStack_24 = uStack_24 - 1;
    } while (uStack_24 != 0);
    sourcePixels = sourcePixels + frameWidthPixels * 3;
    uStack_20 = uStack_20 - 1;
    uStack_24 = frameWidthPixels >> 2;
  } while (uStack_20 != 0);
  return (int)puVar24 - (int)encodedOutput;
}


/* Address: 0x004A7770.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode frame4x4 delta.
   Local calls: MovieColor_ComputeLuma5FromRgb888, MovieColor_ComputeChromaCodeFromRgb888.
*/
uint __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Delta
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *encodedOutput,uint *previousFramePixels,uint *currentFramePixels)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  PackedRgb24 PVar7;
  PackedRgb24 PVar8;
  PackedRgb24 PVar9;
  ulonglong *puVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  uint uVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  uint uVar25;
  uint uVar26;
  uint uVar27;
  uint uVar28;
  uint uVar29;
  uint uVar30;
  PackedRgb24 *pPVar31;
  ulonglong *puVar32;
  uint *puVar33;
  undefined1 uVar35;
  undefined1 uVar36;
  ulonglong uVar34;
  undefined1 uVar37;
  undefined1 uVar38;
  undefined1 uVar39;
  undefined1 uVar40;
  undefined1 uVar41;
  undefined1 uVar42;
  ushort uVar43;
  ushort uVar46;
  PackedRgb24 PVar44;
  ushort uVar47;
  undefined8 uVar45;
  ushort uVar48;
  uint uStack_28;
  uint uStack_24;
  uint uStack_1c;
  
  iVar1 = frameWidthPixels * 4;
  uStack_24 = frameHeightPixels >> 2;
  uStack_1c = 0;
  puVar32 = (ulonglong *)currentFramePixels;
  puVar33 = encodedOutput;
  uStack_28 = frameWidthPixels >> 2;
  do {
    do {
      puVar10 = (ulonglong *)(((int)previousFramePixels + (int)puVar32) - (int)currentFramePixels);
      uVar34 = g_MovieDeltaRgbHighNibbleMask2Pixels &
               (*puVar32 & g_MovieDeltaRgbHighNibbleMask2Pixels ^ *puVar10 |
                puVar32[1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^ puVar10[1] |
                *(ulonglong *)(iVar1 + (int)puVar32) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(ulonglong *)((int)puVar10 + iVar1) |
                *(ulonglong *)(iVar1 + 8 + (int)puVar32) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(ulonglong *)((int)puVar10 + iVar1 + 8) |
                puVar32[frameWidthPixels] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                puVar10[frameWidthPixels] |
                puVar32[frameWidthPixels + 1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                puVar10[frameWidthPixels + 1] |
               *(ulonglong *)((int)puVar32 + frameWidthPixels * 0xc) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(ulonglong *)((int)puVar10 + frameWidthPixels * 0xc) |
               *(ulonglong *)((int)puVar32 + frameWidthPixels * 0xc + 8) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(ulonglong *)((int)puVar10 + frameWidthPixels * 0xc + 8));
      uVar14 = uStack_1c + 1;
      if ((int)(uVar34 >> 0x20) != 0 || (int)uVar34 != 0) {
        if (uStack_1c != 0) {
          if (uStack_1c < 9) {
            *(byte *)puVar33 = ((char)uStack_1c + -1) * ' ' | 0x19;
            puVar33 = (uint *)((int)puVar33 + 1);
          }
          else if (uStack_1c < 0x809) {
            *(ushort *)puVar33 = ((short)uStack_1c + -9) * 0x20 | 0x1a;
            puVar33 = (uint *)((int)puVar33 + 2);
          }
          else {
            *puVar33 = (uStack_1c - 0x809) * 0x20 | 0x1b;
            puVar33 = puVar33 + 1;
          }
          uStack_1c = 0;
        }
        puVar10 = (ulonglong *)(((int)previousFramePixels + (int)puVar32) - (int)currentFramePixels)
        ;
        uVar34 = puVar32[1];
        uVar45 = *(undefined8 *)((int)puVar32 + frameWidthPixels * 4);
        uVar2 = *(undefined8 *)((int)puVar32 + (frameWidthPixels + 2) * 4);
        *puVar10 = *puVar32;
        puVar10[1] = uVar34;
        *(undefined8 *)((int)puVar10 + iVar1) = uVar45;
        *(undefined8 *)((int)puVar10 + iVar1 + 8) = uVar2;
        uVar34 = puVar32[frameWidthPixels + 1];
        uVar45 = *(undefined8 *)((int)puVar32 + frameWidthPixels * 0xc);
        uVar2 = *(undefined8 *)((int)puVar32 + (frameWidthPixels * 3 + 2) * 4);
        puVar10[frameWidthPixels] = puVar32[frameWidthPixels];
        puVar10[frameWidthPixels + 1] = uVar34;
        *(undefined8 *)((int)puVar10 + frameWidthPixels * 0xc) = uVar45;
        *(undefined8 *)((int)puVar10 + frameWidthPixels * 0xc + 8) = uVar2;
        uVar3 = (undefined4)*puVar32;
        uVar4 = *(undefined4 *)((int)puVar32 + 4);
        uVar5 = (undefined4)puVar32[1];
        uVar6 = *(undefined4 *)((int)puVar32 + 0xc);
        uVar35 = (undefined1)((uint)uVar3 >> 0x18);
        uVar43 = CONCAT11(uVar35,uVar35);
        uVar36 = (undefined1)((uint)uVar3 >> 0x10);
        uVar35 = (undefined1)((uint)uVar3 >> 8);
        uVar37 = (undefined1)((uint)uVar4 >> 0x18);
        uVar46 = CONCAT11(uVar37,uVar37);
        uVar38 = (undefined1)((uint)uVar4 >> 0x10);
        uVar37 = (undefined1)((uint)uVar4 >> 8);
        uVar39 = (undefined1)((uint)uVar5 >> 0x18);
        uVar47 = CONCAT11(uVar39,uVar39);
        uVar40 = (undefined1)((uint)uVar5 >> 0x10);
        uVar39 = (undefined1)((uint)uVar5 >> 8);
        uVar41 = (undefined1)((uint)uVar6 >> 0x18);
        uVar48 = CONCAT11(uVar41,uVar41);
        uVar42 = (undefined1)((uint)uVar6 >> 0x10);
        uVar41 = (undefined1)((uint)uVar6 >> 8);
        uVar45 = CONCAT26((uVar48 >> 6) + (uVar47 >> 6) + (uVar43 >> 6) + (uVar46 >> 6),
                          CONCAT24(((ushort)(CONCAT35(CONCAT21(uVar48,uVar42),CONCAT14(uVar42,uVar6)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar47,uVar40),CONCAT14(uVar40,uVar5)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar43,uVar36),CONCAT14(uVar36,uVar3)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar46,uVar38),CONCAT14(uVar38,uVar4)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((CONCAT11(uVar41,uVar41) >> 6) +
                                            (CONCAT11(uVar39,uVar39) >> 6) +
                                            (CONCAT11(uVar35,uVar35) >> 6) +
                                            (CONCAT11(uVar37,uVar37) >> 6),
                                            (CONCAT11((char)uVar6,(char)uVar6) >> 6) +
                                            (CONCAT11((char)uVar5,(char)uVar5) >> 6) +
                                            (CONCAT11((char)uVar3,(char)uVar3) >> 6) +
                                            (CONCAT11((char)uVar4,(char)uVar4) >> 6))));
        uVar11 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*puVar32);
        uVar12 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 4));
        uVar14 = uVar12;
        if (((int)uVar11 <= (int)uVar12) && (uVar14 = uVar11, (int)uVar11 < (int)uVar12)) {
          uVar11 = uVar12;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)puVar32[1]);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 0xc));
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        pPVar31 = (PackedRgb24 *)((int)puVar32 + frameWidthPixels * 4);
        PVar44 = *pPVar31;
        PVar7 = pPVar31[1];
        PVar8 = pPVar31[2];
        PVar9 = pPVar31[3];
        uVar35 = (undefined1)(PVar44 >> 0x18);
        uVar43 = CONCAT11(uVar35,uVar35);
        uVar36 = (undefined1)(PVar44 >> 0x10);
        uVar35 = (undefined1)(PVar44 >> 8);
        uVar37 = (undefined1)(PVar7 >> 0x18);
        uVar46 = CONCAT11(uVar37,uVar37);
        uVar38 = (undefined1)(PVar7 >> 0x10);
        uVar37 = (undefined1)(PVar7 >> 8);
        uVar39 = (undefined1)(PVar8 >> 0x18);
        uVar47 = CONCAT11(uVar39,uVar39);
        uVar40 = (undefined1)(PVar8 >> 0x10);
        uVar39 = (undefined1)(PVar8 >> 8);
        uVar41 = (undefined1)(PVar9 >> 0x18);
        uVar48 = CONCAT11(uVar41,uVar41);
        uVar42 = (undefined1)(PVar9 >> 0x10);
        uVar41 = (undefined1)(PVar9 >> 8);
        uVar45 = CONCAT26((short)((ulonglong)uVar45 >> 0x30) + (uVar43 >> 6) + (uVar46 >> 6) +
                          (uVar47 >> 6) + (uVar48 >> 6),
                          CONCAT24((short)((ulonglong)uVar45 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar43,uVar36),
                                                      CONCAT14(uVar36,PVar44)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar46,uVar38),CONCAT14(uVar38,PVar7)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar47,uVar40),CONCAT14(uVar40,PVar8)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar48,uVar42),CONCAT14(uVar42,PVar9)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar45 >> 0x10) +
                                            (CONCAT11(uVar35,uVar35) >> 6) +
                                            (CONCAT11(uVar37,uVar37) >> 6) +
                                            (CONCAT11(uVar39,uVar39) >> 6) +
                                            (CONCAT11(uVar41,uVar41) >> 6),
                                            (short)uVar45 +
                                            (CONCAT11((char)PVar44,(char)PVar44) >> 6) +
                                            (CONCAT11((char)PVar7,(char)PVar7) >> 6) +
                                            (CONCAT11((char)PVar8,(char)PVar8) >> 6) +
                                            (CONCAT11((char)PVar9,(char)PVar9) >> 6))));
        uVar13 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        pPVar31 = pPVar31 + frameWidthPixels;
        PVar44 = *pPVar31;
        PVar7 = pPVar31[1];
        PVar8 = pPVar31[2];
        PVar9 = pPVar31[3];
        uVar35 = (undefined1)(PVar44 >> 0x18);
        uVar43 = CONCAT11(uVar35,uVar35);
        uVar36 = (undefined1)(PVar44 >> 0x10);
        uVar35 = (undefined1)(PVar44 >> 8);
        uVar37 = (undefined1)(PVar7 >> 0x18);
        uVar46 = CONCAT11(uVar37,uVar37);
        uVar38 = (undefined1)(PVar7 >> 0x10);
        uVar37 = (undefined1)(PVar7 >> 8);
        uVar39 = (undefined1)(PVar8 >> 0x18);
        uVar47 = CONCAT11(uVar39,uVar39);
        uVar40 = (undefined1)(PVar8 >> 0x10);
        uVar39 = (undefined1)(PVar8 >> 8);
        uVar41 = (undefined1)(PVar9 >> 0x18);
        uVar48 = CONCAT11(uVar41,uVar41);
        uVar42 = (undefined1)(PVar9 >> 0x10);
        uVar41 = (undefined1)(PVar9 >> 8);
        uVar45 = CONCAT26((short)((ulonglong)uVar45 >> 0x30) + (uVar43 >> 6) + (uVar46 >> 6) +
                          (uVar47 >> 6) + (uVar48 >> 6),
                          CONCAT24((short)((ulonglong)uVar45 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar43,uVar36),
                                                      CONCAT14(uVar36,PVar44)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar46,uVar38),CONCAT14(uVar38,PVar7)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar47,uVar40),CONCAT14(uVar40,PVar8)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar48,uVar42),CONCAT14(uVar42,PVar9)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar45 >> 0x10) +
                                            (CONCAT11(uVar35,uVar35) >> 6) +
                                            (CONCAT11(uVar37,uVar37) >> 6) +
                                            (CONCAT11(uVar39,uVar39) >> 6) +
                                            (CONCAT11(uVar41,uVar41) >> 6),
                                            (short)uVar45 +
                                            (CONCAT11((char)PVar44,(char)PVar44) >> 6) +
                                            (CONCAT11((char)PVar7,(char)PVar7) >> 6) +
                                            (CONCAT11((char)PVar8,(char)PVar8) >> 6) +
                                            (CONCAT11((char)PVar9,(char)PVar9) >> 6))));
        uVar13 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        pPVar31 = pPVar31 + frameWidthPixels;
        PVar44 = *pPVar31;
        PVar7 = pPVar31[1];
        PVar8 = pPVar31[2];
        PVar9 = pPVar31[3];
        uVar35 = (undefined1)(PVar44 >> 0x18);
        uVar43 = CONCAT11(uVar35,uVar35);
        uVar36 = (undefined1)(PVar44 >> 0x10);
        uVar35 = (undefined1)(PVar44 >> 8);
        uVar37 = (undefined1)(PVar7 >> 0x18);
        uVar46 = CONCAT11(uVar37,uVar37);
        uVar38 = (undefined1)(PVar7 >> 0x10);
        uVar37 = (undefined1)(PVar7 >> 8);
        uVar39 = (undefined1)(PVar8 >> 0x18);
        uVar47 = CONCAT11(uVar39,uVar39);
        uVar40 = (undefined1)(PVar8 >> 0x10);
        uVar39 = (undefined1)(PVar8 >> 8);
        uVar41 = (undefined1)(PVar9 >> 0x18);
        uVar48 = CONCAT11(uVar41,uVar41);
        uVar42 = (undefined1)(PVar9 >> 0x10);
        uVar41 = (undefined1)(PVar9 >> 8);
        uVar45 = CONCAT26((short)((ulonglong)uVar45 >> 0x30) + (uVar43 >> 6) + (uVar46 >> 6) +
                          (uVar47 >> 6) + (uVar48 >> 6),
                          CONCAT24((short)((ulonglong)uVar45 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar43,uVar36),
                                                      CONCAT14(uVar36,PVar44)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar46,uVar38),CONCAT14(uVar38,PVar7)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar47,uVar40),CONCAT14(uVar40,PVar8)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar48,uVar42),CONCAT14(uVar42,PVar9)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar45 >> 0x10) +
                                            (CONCAT11(uVar35,uVar35) >> 6) +
                                            (CONCAT11(uVar37,uVar37) >> 6) +
                                            (CONCAT11(uVar39,uVar39) >> 6) +
                                            (CONCAT11(uVar41,uVar41) >> 6),
                                            (short)uVar45 +
                                            (CONCAT11((char)PVar44,(char)PVar44) >> 6) +
                                            (CONCAT11((char)PVar7,(char)PVar7) >> 6) +
                                            (CONCAT11((char)PVar8,(char)PVar8) >> 6) +
                                            (CONCAT11((char)PVar9,(char)PVar9) >> 6))));
        uVar13 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
        uVar14 = uVar13;
        if (((int)uVar12 <= (int)uVar13) && (uVar14 = uVar12, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
        uVar12 = uVar13;
        if (((int)uVar14 <= (int)uVar13) && (uVar12 = uVar14, (int)uVar11 < (int)uVar13)) {
          uVar11 = uVar13;
        }
        uVar14 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
        uVar13 = uVar14;
        if (((int)uVar12 <= (int)uVar14) && (uVar13 = uVar12, (int)uVar11 < (int)uVar14)) {
          uVar11 = uVar14;
        }
        uVar43 = (ushort)uVar45 >> 6;
        uVar46 = (ushort)((ulonglong)uVar45 >> 0x10) >> 6;
        uVar47 = (ushort)((ulonglong)uVar45 >> 0x20) >> 6;
        uVar48 = (ushort)((ulonglong)uVar45 >> 0x36);
        PVar44 = CONCAT13((uVar48 != 0) * (uVar48 < 0x100) * (char)uVar48 - (0xff < uVar48),
                          CONCAT12((uVar47 != 0) * (uVar47 < 0x100) * (char)uVar47 - (0xff < uVar47)
                                   ,CONCAT11((uVar46 != 0) * (uVar46 < 0x100) * (char)uVar46 -
                                             (0xff < uVar46),
                                             (uVar43 != 0) * (uVar43 < 0x100) * (char)uVar43 -
                                             (0xff < uVar43))));
        uVar12 = (int)((uVar13 - 8) + uVar11) >> 1;
        if ((int)uVar12 < 0) {
          uVar12 = 0;
        }
        else if (0x18 < uVar12) {
          uVar12 = 0x18;
        }
        uVar14 = uStack_1c;
        if (uVar11 - uVar13 < 0xc) {
          *puVar33 = uVar12;
          uVar11 = MovieColor_ComputeChromaCodeFromRgb888(PVar44);
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          iVar15 = uVar13 - uVar12;
          if (iVar15 < 0) {
            iVar15 = 0;
          }
          else if (7 < iVar15) {
            iVar15 = 7;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          iVar16 = uVar13 - uVar12;
          if (iVar16 < 0) {
            iVar16 = 0;
          }
          else if (7 < iVar16) {
            iVar16 = 7;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          iVar17 = uVar13 - uVar12;
          if (iVar17 < 0) {
            iVar17 = 0;
          }
          else if (7 < iVar17) {
            iVar17 = 7;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          iVar18 = uVar13 - uVar12;
          if (iVar18 < 0) {
            iVar18 = 0;
          }
          else if (7 < iVar18) {
            iVar18 = 7;
          }
          pPVar31 = pPVar31 + -frameWidthPixels;
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          iVar19 = uVar13 - uVar12;
          if (iVar19 < 0) {
            iVar19 = 0;
          }
          else if (7 < iVar19) {
            iVar19 = 7;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          iVar20 = uVar13 - uVar12;
          if (iVar20 < 0) {
            iVar20 = 0;
          }
          else if (7 < iVar20) {
            iVar20 = 7;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          uVar13 = uVar13 - uVar12;
          if ((int)uVar13 < 0) {
            uVar13 = 0;
          }
          else if (7 < (int)uVar13) {
            uVar13 = 7;
          }
          uVar21 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          puVar33[1] = (uVar11 & 0x7fe0) << 0x10 | iVar15 << 0x12 | iVar16 << 0xf | iVar17 << 0xc |
                       iVar18 << 9 | iVar19 << 6 | iVar20 << 3 | uVar13;
          iVar15 = uVar21 - uVar12;
          if (iVar15 < 0) {
            iVar15 = 0;
          }
          else if (7 < iVar15) {
            iVar15 = 7;
          }
          pPVar31 = pPVar31 + -frameWidthPixels;
          uVar11 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          iVar16 = uVar11 - uVar12;
          if (iVar16 < 0) {
            iVar16 = 0;
          }
          else if (7 < iVar16) {
            iVar16 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          iVar17 = uVar11 - uVar12;
          if (iVar17 < 0) {
            iVar17 = 0;
          }
          else if (7 < iVar17) {
            iVar17 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          iVar18 = uVar11 - uVar12;
          if (iVar18 < 0) {
            iVar18 = 0;
          }
          else if (7 < iVar18) {
            iVar18 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          iVar19 = uVar11 - uVar12;
          if (iVar19 < 0) {
            iVar19 = 0;
          }
          else if (7 < iVar19) {
            iVar19 = 7;
          }
          puVar32 = (ulonglong *)(pPVar31 + -frameWidthPixels);
          uVar11 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 0xc));
          iVar20 = uVar11 - uVar12;
          if (iVar20 < 0) {
            iVar20 = 0;
          }
          else if (7 < iVar20) {
            iVar20 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)puVar32[1]);
          iVar22 = uVar11 - uVar12;
          if (iVar22 < 0) {
            iVar22 = 0;
          }
          else if (7 < iVar22) {
            iVar22 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 4));
          iVar23 = uVar11 - uVar12;
          if (iVar23 < 0) {
            iVar23 = 0;
          }
          else if (7 < iVar23) {
            iVar23 = 7;
          }
          uVar11 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*puVar32);
          iVar24 = uVar11 - uVar12;
          if (iVar24 < 0) {
            iVar24 = 0;
          }
          else if (7 < iVar24) {
            iVar24 = 7;
          }
          *puVar33 = *puVar33 |
                     iVar15 << 0x1d | iVar16 << 0x1a | iVar17 << 0x17 | iVar18 << 0x14 |
                     iVar19 << 0x11 | iVar20 << 0xe | iVar22 << 0xb | iVar23 << 8 | iVar24 << 5;
          puVar33 = puVar33 + 2;
        }
        else {
          uVar12 = uVar12 - 4;
          if ((int)uVar12 < 0) {
            uVar12 = 0;
          }
          else if (0x10 < (int)uVar12) {
            uVar12 = 0x10;
          }
          *puVar33 = uVar12;
          uVar11 = MovieColor_ComputeChromaCodeFromRgb888(PVar44);
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          uVar13 = uVar13 - uVar12;
          if ((int)uVar13 < 0) {
            uVar13 = 0;
          }
          else if (0xf < (int)uVar13) {
            uVar13 = 0xf;
          }
          uVar21 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          uVar21 = uVar21 - uVar12;
          if ((int)uVar21 < 0) {
            uVar21 = 0;
          }
          else if (0xf < (int)uVar21) {
            uVar21 = 0xf;
          }
          uVar25 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          uVar25 = uVar25 - uVar12;
          if ((int)uVar25 < 0) {
            uVar25 = 0;
          }
          else if (0xf < (int)uVar25) {
            uVar25 = 0xf;
          }
          uVar26 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          uVar26 = uVar26 - uVar12;
          if ((int)uVar26 < 0) {
            uVar26 = 0;
          }
          else if (0xf < (int)uVar26) {
            uVar26 = 0xf;
          }
          pPVar31 = pPVar31 + -frameWidthPixels;
          uVar27 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          uVar27 = uVar27 - uVar12;
          if ((int)uVar27 < 0) {
            uVar27 = 0;
          }
          else if (0xf < (int)uVar27) {
            uVar27 = 0xf;
          }
          uVar28 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          uVar28 = uVar28 - uVar12;
          if ((int)uVar28 < 0) {
            uVar28 = 0;
          }
          else if (0xf < (int)uVar28) {
            uVar28 = 0xf;
          }
          uVar29 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          uVar29 = uVar29 - uVar12;
          if ((int)uVar29 < 0) {
            uVar29 = 0;
          }
          else if (0xf < (int)uVar29) {
            uVar29 = 0xf;
          }
          uVar30 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          puVar33[1] = (uVar11 & 0x7fe0) * 0x10000 + 0x80000000 | (uVar13 >> 1) << 0x12 |
                       (uVar21 >> 1) << 0xf | (uVar25 >> 1) << 0xc | (uVar26 >> 1) << 9 |
                       (uVar27 >> 1) << 6 | (uVar28 >> 1) << 3 | uVar29 >> 1;
          uVar30 = uVar30 - uVar12;
          if ((int)uVar30 < 0) {
            uVar30 = 0;
          }
          else if (0xf < (int)uVar30) {
            uVar30 = 0xf;
          }
          pPVar31 = pPVar31 + -frameWidthPixels;
          uVar11 = MovieColor_ComputeLuma5FromRgb888(pPVar31[3]);
          uVar11 = uVar11 - uVar12;
          if ((int)uVar11 < 0) {
            uVar11 = 0;
          }
          else if (0xf < (int)uVar11) {
            uVar11 = 0xf;
          }
          uVar13 = MovieColor_ComputeLuma5FromRgb888(pPVar31[2]);
          uVar13 = uVar13 - uVar12;
          if ((int)uVar13 < 0) {
            uVar13 = 0;
          }
          else if (0xf < (int)uVar13) {
            uVar13 = 0xf;
          }
          uVar21 = MovieColor_ComputeLuma5FromRgb888(pPVar31[1]);
          uVar21 = uVar21 - uVar12;
          if ((int)uVar21 < 0) {
            uVar21 = 0;
          }
          else if (0xf < (int)uVar21) {
            uVar21 = 0xf;
          }
          uVar25 = MovieColor_ComputeLuma5FromRgb888(*pPVar31);
          uVar25 = uVar25 - uVar12;
          if ((int)uVar25 < 0) {
            uVar25 = 0;
          }
          else if (0xf < (int)uVar25) {
            uVar25 = 0xf;
          }
          puVar32 = (ulonglong *)(pPVar31 + -frameWidthPixels);
          uVar26 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 0xc));
          uVar26 = uVar26 - uVar12;
          if ((int)uVar26 < 0) {
            uVar26 = 0;
          }
          else if (0xf < (int)uVar26) {
            uVar26 = 0xf;
          }
          uVar27 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)puVar32[1]);
          uVar27 = uVar27 - uVar12;
          if ((int)uVar27 < 0) {
            uVar27 = 0;
          }
          else if (0xf < (int)uVar27) {
            uVar27 = 0xf;
          }
          uVar28 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)puVar32 + 4));
          uVar28 = uVar28 - uVar12;
          if ((int)uVar28 < 0) {
            uVar28 = 0;
          }
          else if (0xf < (int)uVar28) {
            uVar28 = 0xf;
          }
          uVar29 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*puVar32);
          uVar29 = uVar29 - uVar12;
          if ((int)uVar29 < 0) {
            uVar29 = 0;
          }
          else if (0xf < (int)uVar29) {
            uVar29 = 0xf;
          }
          *puVar33 = *puVar33 |
                     (uVar30 >> 1) << 0x1d | (uVar11 >> 1) << 0x1a | (uVar13 >> 1) << 0x17 |
                     (uVar21 >> 1) << 0x14 | (uVar25 >> 1) << 0x11 | (uVar26 >> 1) << 0xe |
                     (uVar27 >> 1) << 0xb | (uVar28 >> 1) << 8 | (uVar29 >> 1) << 5;
          puVar33 = puVar33 + 2;
        }
      }
      uStack_1c = uVar14;
      puVar32 = puVar32 + 2;
      uStack_28 = uStack_28 - 1;
    } while (uStack_28 != 0);
    puVar32 = (ulonglong *)((int)puVar32 + frameWidthPixels * 0xc);
    uStack_24 = uStack_24 - 1;
    uStack_28 = frameWidthPixels >> 2;
  } while (uStack_24 != 0);
  if (uStack_1c != 0) {
    if (uStack_1c < 9) {
      *(byte *)puVar33 = ((char)uStack_1c + -1) * ' ' | 0x19;
      puVar33 = (uint *)((int)puVar33 + 1);
    }
    else if (uStack_1c < 0x809) {
      *(ushort *)puVar33 = ((short)uStack_1c + -9) * 0x20 | 0x1a;
      puVar33 = (uint *)((int)puVar33 + 2);
    }
    else {
      *puVar33 = (uStack_1c - 0x809) * 0x20 | 0x1b;
      puVar33 = puVar33 + 1;
    }
  }
  return (int)puVar33 + (7 - (int)encodedOutput) & 0xfffffff8;
}


/* Address: 0x004A8A60.
   Ownership: movie/runtime/playback.
   Purpose: Starts embedded audio on the first frame, decodes the next 4x4 delta frame when enough bytes are
   buffered, compacts the bounded stream buffer, and requests asynchronous refill. CF clear returns the active
   MovieRuntime in EAX; CF set reports end-of-movie or read failure.
   Local calls: Movie_DecodeFrame4x4Delta.
*/
MovieAdvanceFrameEaxCf5 __thandor_eax_cf_preserve_ecx_edx Movie_AdvanceFrame(void)

{
  MovieFileHeader *pMVar1;
  MovieFrameIndex MVar2;
  MovieRuntime *pMVar3;
  uint uVar4;
  dword dVar5;
  uint uVar6;
  void *unaff_EBX;
  undefined4 *puVar7;
  byte *pbVar8;
  SoundPlayVoiceEaxCf5 SVar9;
  MovieAdvanceFrameEaxCf5 MVar10;
  MovieAdvanceFrameEaxCf5 MVar11;
  MovieAdvanceFrameEaxCf5 MVar12;
  
  pMVar3 = g_ActiveMovie;
  uVar4 = 0x30;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_ActiveMovie->streamState == MOVIE_STREAM_READ_FAILED) {
      (*g_FileSystemClose)(unaff_EBX);
      pMVar3->remainingVideoBytes = 0;
    }
    else {
      if (((g_ActiveMovie->streamState == MOVIE_STREAM_IDLE) && (g_ActiveMovie->workerActive != 0))
         && (g_ActiveMovie->remainingVideoBytes != 0)) {
        if ((uint)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200)
        {
          g_ActiveMovie->streamState = MOVIE_STREAM_FILL_REQUESTED;
          ReleaseSemaphore(pMVar3->refillSemaphore,1,(LPLONG)0x0);
        }
      }
      pMVar1 = pMVar3->fileHeader;
      MVar2 = pMVar3->currentFrameIndex;
      pbVar8 = (pMVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
               (pMVar3->videoStreamOffset - 0x28);
      if ((MVar2 == 0) && (pMVar3->audioVoiceSet != (DirectSoundVoiceSet *)0x0)) {
        SVar9 = (*g_SoundPlayOneShot)
                          (pMVar3->audioGainQ15,pMVar3->audioGainQ15,pMVar3->audioVoiceSet);
        pMVar3->activeAudioBuffer = SVar9.eax;
      }
      uVar6 = MVar2 + 1;
      uVar4 = (int)pMVar3->loadedVideoEnd - (int)pbVar8;
      if (uVar6 <= pMVar1->frameCount) {
        if ((pMVar3->remainingVideoBytes != 0) && (uVar4 < 0x1e000)) {
          MVar11.carry = false;
          MVar11.eax = (dword)&pMVar3[-1].textureCommon.allocationSizeBytes;
          return MVar11;
        }
        dVar5 = Movie_DecodeFrame4x4Delta
                          (pMVar1->heightPixels,pMVar1->widthPixels,pMVar3->argbPixels,pbVar8);
        pMVar3->currentFrameIndex = uVar6;
        pMVar3->videoStreamOffset = pMVar3->videoStreamOffset + dVar5;
        if ((pMVar3->openFlags != 0) && (pMVar3->streamState == MOVIE_STREAM_IDLE)) {
          uVar4 = pMVar3->videoStreamOffset;
          uVar6 = (int)pMVar3->loadedVideoEnd - (int)pMVar3->fileHeader;
          if ((0x1e01ff < uVar4) && (uVar4 < uVar6)) {
            pMVar3->videoStreamOffset = pMVar3->videoStreamOffset - 0x1e0000;
            pbVar8 = pMVar3->fileHeader[-0xf00].common.buildMetadata.assetRelativeAddressAnchor28 +
                     (uVar4 - 0x28);
            pMVar3->loadedVideoEnd = pMVar3->loadedVideoEnd + -0x1e0000;
            puVar7 = (undefined4 *)(pbVar8 + 0x1e0000);
            for (uVar4 = uVar6 - uVar4 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
              *(undefined4 *)pbVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              pbVar8 = pbVar8 + 4;
            }
          }
        }
        MVar10.carry = false;
        MVar10.eax = (dword)pMVar3;
        return MVar10;
      }
    }
  }
  MVar12.carry = true;
  MVar12.eax = uVar4;
  return MVar12;
}


/* Address: 0x00564080.
   Ownership: movie/runtime/playback.
   Purpose: Advances Movie_AdvanceFrame until the requested target is reached or CF reports failure. On success it
   publishes the current frame index, invalidates all UI roots, draws, and presents the framebuffer. EAX and CF
   behavior remain intact. Typed parameters: p0 targetFrame→MovieFrameIndex_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: Movie_AdvanceFrame.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout], UiFrame_Draw [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame)

{
  uint uVar1;
  MovieAdvanceFrameEaxCf5 MVar2;
  
  uVar1 = g_MoviePlaybackCurrentFrame;
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      uVar1 = uVar1 + 1;
      MVar2 = Movie_AdvanceFrame();
      if (MVar2.carry) {
        return;
      }
    } while (uVar1 < targetFrame);
    g_MoviePlaybackCurrentFrame = uVar1;
    UiRootStack_InvalidateAll();
    UiFrame_Draw();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  return;
}


/* Address: 0x004A81C0.
   Ownership: movie/runtime/playback.
   Purpose: Decodes one FLM frame into an existing ARGB image in 4x4 blocks. Tokens 0-24 encode one block through
   g_MovieChromaLumaToArgb, while tokens 25-31 skip runs and preserve pixels from the previous frame. Returns
   encoded bytes consumed rounded up to eight.
*/
dword __thandor_eax_preserve_ecx_edx
Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,dword *destinationArgb,
          byte *encodedFrame)

{
  uint uVar1;
  uint uVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  uint uVar6;
  uint *puVar7;
  dword *pdVar8;
  uint local_28;
  uint local_24;
  uint local_1c;
  
  local_24 = heightPixels >> 2;
  local_1c = 0;
  puVar7 = (uint *)encodedFrame;
  local_28 = widthPixels >> 2;
  do {
    do {
      uVar1 = *puVar7;
      if (local_1c == 0) {
        uVar6 = uVar1 & 0x1f;
        uVar2 = puVar7[1];
        if (uVar6 < 0x19) {
          if ((int)uVar2 < 0) {
            uVar6 = (uVar2 & 0x7fe00000) >> 0x10 | *puVar7 & 0x1f;
            uVar1 = *puVar7;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 8 & 7) * 2];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0xb & 7) * 2];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0xe & 7) * 2];
            *destinationArgb = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 5 & 7) * 2];
            destinationArgb[1] = dVar3;
            destinationArgb[2] = dVar4;
            destinationArgb[3] = dVar5;
            pdVar8 = destinationArgb + widthPixels;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x14 & 7) * 2];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x17 & 7) * 2];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x1a & 7) * 2];
            *pdVar8 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x11 & 7) * 2];
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            pdVar8 = pdVar8 + widthPixels;
            uVar2 = puVar7[1];
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 & 7) * 2];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 3 & 7) * 2];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 6 & 7) * 2];
            *pdVar8 = *(dword *)((int)g_MovieChromaLumaToArgb[0] +
                                (uVar1 >> 0x1a & 0xfffffff8) + uVar6 * 4);
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            pdVar8 = pdVar8 + widthPixels;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0xc & 7) * 2];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0xf & 7) * 2];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0x12 & 7) * 2];
            puVar7 = puVar7 + 2;
            *pdVar8 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 9 & 7) * 2];
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            destinationArgb = pdVar8 + widthPixels * -3;
          }
          else {
            uVar6 = (uVar2 & 0x7fe00000) >> 0x10 | *puVar7 & 0x1f;
            uVar1 = *puVar7;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 8 & 7)];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0xb & 7)];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0xe & 7)];
            *destinationArgb = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 5 & 7)];
            destinationArgb[1] = dVar3;
            destinationArgb[2] = dVar4;
            destinationArgb[3] = dVar5;
            pdVar8 = destinationArgb + widthPixels;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x14 & 7)];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x17 & 7)];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x1a & 7)];
            *pdVar8 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x11 & 7)];
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            pdVar8 = pdVar8 + widthPixels;
            uVar2 = puVar7[1];
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 & 7)];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 3 & 7)];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 6 & 7)];
            *pdVar8 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar1 >> 0x1d)];
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            pdVar8 = pdVar8 + widthPixels;
            dVar3 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0xc & 7)];
            dVar4 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0xf & 7)];
            dVar5 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 0x12 & 7)];
            puVar7 = puVar7 + 2;
            *pdVar8 = g_MovieChromaLumaToArgb[0][uVar6 + (uVar2 >> 9 & 7)];
            pdVar8[1] = dVar3;
            pdVar8[2] = dVar4;
            pdVar8[3] = dVar5;
            destinationArgb = pdVar8 + widthPixels * -3;
          }
        }
        else if (uVar6 == 0x19) {
          puVar7 = (uint *)((int)puVar7 + 1);
          local_1c = (uVar1 & 0xff) >> 5;
        }
        else if (uVar6 < 0x1b) {
          puVar7 = (uint *)((int)puVar7 + 2);
          local_1c = ((uVar1 & 0xffff) >> 5) + 8;
        }
        else {
          puVar7 = puVar7 + 1;
          local_1c = (uVar1 >> 5) + 0x808;
        }
      }
      else {
        local_1c = local_1c - 1;
      }
      destinationArgb = destinationArgb + 4;
      local_28 = local_28 - 1;
    } while (local_28 != 0);
    destinationArgb = destinationArgb + widthPixels * 3;
    local_24 = local_24 - 1;
    local_28 = widthPixels >> 2;
  } while (local_24 != 0);
  return (int)puVar7 + (7 - (int)encodedFrame) & 0xfffffff8;
}


/* Address: 0x004A6FB0.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie color compute chroma code from rgb888.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed].
*/
uint __thandor_eax_preserve_ecx_edx MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888)

{
  uint uVar1;
  FixedLengthAngleEaxEdx8 FVar2;
  
  uVar1 = rgb888 >> 8 & 0xff;
  FVar2 = FixedMath_Vector2AngleAndLengthRegs
                    (((rgb888 & 0xff) - uVar1) * 0xddb4,
                     (uVar1 + (rgb888 & 0xff) + (rgb888 >> 0x10 & 0xff) * -2) * 0x8000);
  return FVar2.length >> 9 & 0x7c00 | FVar2.angle >> 6 & 0x3e0;
}


/* Address: 0x004A7000.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie color compute luma5 from rgb888.
*/
uint __thandor_eax_preserve_ecx_edx MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888)

{
  return ((rgb888 & 0xff) + (rgb888 >> 8 & 0xff) + (rgb888 >> 0x10 & 0xff)) * 0x5555 + 0x40000 >>
         0x13;
}

