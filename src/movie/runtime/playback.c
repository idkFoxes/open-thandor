#include <thandor/movie/runtime/playback.h>

/* Implementation ownership: movie/runtime/playback. */

/* Address: 0x004A8040.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode flm buffer from frame provider carry-flag result.
   Local calls: Movie_EncodeFrame4x4Keyframe, Movie_EncodeFrame4x4Delta.
*/
undefined8 __fastcall
Movie_EncodeFlmBufferFromFrameProviderCf
          (undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint *param_5,
          undefined *param_6)

{
  dword dVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_EDX;
  undefined4 uVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  bool bVar7;
  undefined1 uVar8;
  undefined8 uVar9;
  ulonglong uVar10;
  int iVar11;
  
  puVar5 = param_5;
  for (iVar2 = 0x80; iVar2 != 0; iVar2 = iVar2 + -1) {
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
  puVar5[-0x54] = param_4;
  puVar5[-0x53] = extraout_EDX;
  puVar5[-0x52] = 0;
  puVar5[-0x51] = 0;
  bVar7 = (uint *)0xfffffdff < puVar5 + -0x80;
  uVar9 = (*(code *)param_6)(0);
  uVar3 = (undefined4)((ulonglong)uVar9 >> 0x20);
  iVar2 = (int)uVar9;
  if (!bVar7) {
    puVar4 = (uint *)(iVar2 + *(int *)(iVar2 + *(int *)(iVar2 + 0xb8) + 0xc));
    uVar9 = Movie_EncodeFrame4x4Keyframe(extraout_ECX + 1,uVar3,uVar3,param_4,puVar5,puVar4);
    uVar8 = CARRY4((uint)puVar5,(uint)uVar9);
    puVar5 = (uint *)((int)puVar5 + (uint)uVar9);
    iVar11 = iVar2;
    while( true ) {
      uVar9 = (*(code *)param_6)(0,iVar11);
      uVar3 = (undefined4)((ulonglong)uVar9 >> 0x20);
      iVar2 = (int)uVar9;
      if ((bool)uVar8) break;
      uVar10 = Movie_EncodeFrame4x4Delta
                         (extraout_ECX_00 + 1,uVar3,uVar3,param_4,puVar5,(int)puVar4,
                          (ulonglong *)(*(int *)(iVar2 + *(int *)(iVar2 + 0xb8) + 0xc) + iVar2));
      uVar8 = CARRY4((uint)puVar5,(uint)uVar10);
      puVar5 = (uint *)((int)puVar5 + (uint)uVar10);
      (*(code *)param_6)();
    }
    (*(code *)param_6)();
    uVar6 = (int)puVar5 - (int)param_5;
    param_5[0x2e] = extraout_ECX_01;
    param_5[0x3f] = 0x10;
    param_5[1] = uVar6;
    param_5[0x30] = uVar6;
    if (iVar2 == -1) {
      param_5[0x30] = param_5[0x30] - 0x200;
      return CONCAT44(param_2,uVar6);
    }
  }
  return CONCAT44(param_2,iVar2);
}

/* Address: 0x00563FF0.
   Ownership: movie/runtime/playback.
   Purpose: Increments the playback schedule counter, derives the target from exact groups of eight frames and the
   configured span, advances intermediate eight-frame boundaries and the final target, then invokes the existing
   playback tick helper. EAX is preserved.
   Local calls: MoviePlayback_AdvanceToFrameAndPresent.
   Cross-module calls: InGameRuntime_UpdateSimulationAndNetworkTick [gameplay/session/runtime].
*/
undefined8 __cdecl MoviePlayback_AdvanceScheduledFrameAndTick(void)

{
  undefined4 in_EAX;
  uint targetFrame;
  uint targetFrame_00;
  uint extraout_ECX;
  undefined4 in_EDX;
  undefined8 uVar1;
  
  g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + 1;
  if (g_MoviePlaybackScheduleSpan != 0) {
    targetFrame = (uint)(g_MoviePlaybackScheduleCounter * 8) / g_MoviePlaybackScheduleSpan + 1 +
                  g_MoviePlaybackBaseFrameGroup * 8;
    for (targetFrame_00 = 8; targetFrame_00 < targetFrame; targetFrame_00 = targetFrame_00 + 8) {
      if (g_MoviePlaybackCurrentFrame < targetFrame_00) {
        uVar1 = MoviePlayback_AdvanceToFrameAndPresent(targetFrame_00);
        targetFrame = (uint)uVar1;
        targetFrame_00 = extraout_ECX;
      }
    }
    if (targetFrame != g_MoviePlaybackCurrentFrame) {
      MoviePlayback_AdvanceToFrameAndPresent(targetFrame);
    }
  }
  InGameRuntime_UpdateSimulationAndNetworkTick();
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004A8590.
   Ownership: movie/runtime/playback.
   Purpose: Opens an FLM from a mounted package or loose path, validates magic/version, chooses one embedded audio
   track, builds a one-subresource gfx-compatible MovieRuntime, and optionally starts the refill worker. CF clear
   means success; EAX returns frameCount and ECX returns frameIntervalMilliseconds.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], Package_FindEntryAcrossMounts
   [assets/package/runtime], Random_NextPrimary [core/math/random].
*/
dword Movie_Open(MovieOpenFlags movieOpenFlags,word *path)

{
  MovieFileHeader *pMVar1;
  uint uVar2;
  MovieSubresourceCount MVar3;
  longlong lVar4;
  MovieAudioGainQ15 MVar5;
  PckEntryHeader *pPVar6;
  MovieRuntime *pMVar7;
  MovieRuntime *pMVar8;
  HANDLE pvVar9;
  dword dVar10;
  int iVar11;
  MovieFileHeader *byteCount;
  uint uVar12;
  dword bytes;
  FileIoByteCount byteCount_00;
  AssetAllocationSizeBytes extraout_ECX;
  AssetAllocationSizeBytes extraout_ECX_00;
  AssetAllocationSizeBytes AVar13;
  uint extraout_EDX;
  uint extraout_EDX_00;
  SoundSampleAsset *arg0;
  SoundSampleAsset *memory;
  DirectSoundVoiceSet *extraout_EDX_01;
  DirectSoundVoiceSet *extraout_EDX_02;
  DirectSoundVoiceSet *pDVar14;
  AssetDimension extraout_EDX_03;
  SoundSampleAsset *memory_00;
  MovieRuntime *unaff_EBX;
  AssetMagic *pAVar15;
  undefined1 uVar16;
  undefined1 uVar17;
  bool bVar18;
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
  if (((movieOpenFlags & 0x80000000) == 0) &&
     (uVar16 = 0, g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    unaff_EBX = (MovieRuntime *)
                (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    uVar17 = 0;
    if ((bool)uVar16) goto Movie_OpenResolvePackageOrFallbackStream;
  }
  else {
Movie_OpenResolvePackageOrFallbackStream:
    uVar17 = false;
    movieOpenFlags = movieOpenFlags & 0x7fffffff;
    pPVar6 = Package_FindEntryAcrossMounts(path);
    if (!(bool)uVar17) {
      uVar17 = CARRY4(extraout_EDX,pPVar6->runtimePayloadOffset);
      (*g_FileSystemSeekCf)
                (FILESYSTEM_SEEK_BEGIN,extraout_EDX + pPVar6->runtimePayloadOffset,unaff_EBX);
      if (!(bool)uVar17) {
        local_1c = local_1c + 1;
        goto Movie_OpenReadHeaderFromResolvedStream;
      }
    }
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    unaff_EBX = (MovieRuntime *)
                (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    if (((bool)uVar17) && (unaff_EBX = (MovieRuntime *)(*g_FileSystemOpenCf)(0,path), (bool)uVar17))
    {
      return (dword)unaff_EBX;
    }
    uVar17 = 0;
  }
Movie_OpenReadHeaderFromResolvedStream:
  pAVar15 = (AssetMagic *)g_PackageScratchBuffer;
  pMVar7 = (MovieRuntime *)(*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,unaff_EBX);
  if ((bool)uVar17) goto Movie_OpenCloseStandaloneStreamAndReturnFailure;
  pMVar7 = (MovieRuntime *)0x30;
  if ((*pAVar15 != 0x6d6c66) ||
     (dVar10 = *(int *)((int)pAVar15 + 0xc0) + 0x200, *(int *)((int)pAVar15 + 0xc) != 0x20001))
  goto Movie_OpenCloseStandaloneStreamAndReturnFailure;
  bVar18 = dVar10 < 0x3c0000;
  if ((0x3c0000 < dVar10) && (bVar18 = false, movieOpenFlags != 0)) {
    dVar10 = 0x3c0000;
  }
  pMVar7 = (*g_MemoryApi.alloc)(dVar10);
  if (bVar18) goto Movie_OpenCloseStandaloneStreamAndReturnFailure;
  pMVar8 = pMVar7;
  for (iVar11 = 0x80; iVar11 != 0; iVar11 = iVar11 + -1) {
    (pMVar8->textureCommon).magic = *pAVar15;
    pAVar15 = pAVar15 + 1;
    pMVar8 = (MovieRuntime *)&(pMVar8->textureCommon).allocationSizeBytes;
  }
  pMVar1 = pMVar7->fileHeader;
  byteCount = pMVar1;
  if (((MovieFileHeader *)0x3a2000 < pMVar1) && (movieOpenFlags != 0)) {
    byteCount = (MovieFileHeader *)0x3a2000;
  }
  local_2c = (int)pMVar1 - (int)byteCount;
  bVar18 = CARRY4((uint)pMVar8,(uint)byteCount);
  local_28 = (byte *)((int)(pMVar8->textureCommon).buildMetadata.names.sourceName +
                     (int)(byteCount[-1].reserved100_1FF + 0x90));
  local_20 = unaff_EBX;
  local_18 = pMVar7;
  pMVar8 = (MovieRuntime *)(*g_FileSystemReadExactCf)((FileIoByteCount)byteCount,pMVar8,unaff_EBX);
  if ((bVar18) || (pMVar8 = (MovieRuntime *)(*g_FileSystemGetPositionCf)(unaff_EBX), bVar18))
  goto Movie_OpenReleaseHeaderAllocationAfterFailure;
  uVar2 = pMVar7->reservedBC;
  uVar12 = 0;
  local_24 = pMVar8;
  if ((uVar2 == 0) || (0xe < uVar2)) {
Movie_OpenAllocateAndInitializeRuntime:
    lVar4 = (longlong)(int)pMVar7->subresourceCount * (longlong)(int)pMVar7->paletteBankCount;
    iVar11 = (int)lVar4;
    bVar18 = iVar11 != lVar4;
    pMVar8 = (*g_MemoryApi.alloc)(iVar11 * 4 + 0x220);
    if (!bVar18) {
      AVar13 = extraout_ECX;
      pDVar14 = extraout_EDX_01;
      g_ActiveMovie = pMVar8;
      if ((local_1c == 0) && (local_2c == 0)) {
        (*g_FileSystemClose)(unaff_EBX);
        AVar13 = extraout_ECX_00;
        pDVar14 = extraout_EDX_02;
      }
      (pMVar8->textureCommon).magic = ASSET_MAGIC_GFX;
      (pMVar8->textureCommon).allocationSizeBytes = AVar13;
      (pMVar8->textureCommon).formatVersion = 1;
      (pMVar8->textureCommon).converterVersion = 0;
      pMVar8->audioVoiceSet = pDVar14;
      pMVar8->activeAudioBuffer = (IDirectSoundBuffer *)0x0;
      MVar3 = pMVar7->subresourceCount;
      dVar10 = (*g_LocaleGetPackedCurrentTime)();
      (pMVar8->textureCommon).buildMetadata.timestamps.dateValue0 = dVar10;
      (pMVar8->textureCommon).buildMetadata.timestamps.dateValue1 = dVar10;
      (pMVar8->textureCommon).buildMetadata.timestamps.dateValue2 = dVar10;
      dVar10 = (*g_LocaleGetPackedCurrentDate)();
      (pMVar8->textureCommon).buildMetadata.timestamps.timeValue0 = dVar10;
      (pMVar8->textureCommon).buildMetadata.timestamps.timeValue1 = dVar10;
      (pMVar8->textureCommon).buildMetadata.timestamps.timeValue2 = dVar10;
      (*g_LocaleCopyDefaultComputerLabelUtf16)
                ((pMVar8->textureCommon).buildMetadata.names.producerName);
      (*g_LocaleCopyDefaultComputerLabelUtf16)
                ((pMVar8->textureCommon).buildMetadata.names.sourceName);
      pMVar8->reserved100_1FF[0] = 0;
      pMVar8->subresourceTableOffset = 0x200;
      pMVar8->paletteBankCount = 0;
      pMVar8->subresourceCount = 1;
      pMVar8->fileHeader = (MovieFileHeader *)pMVar7;
      pMVar8->currentFrameIndex = 0;
      pMVar8->videoStreamOffset = 0x200;
      (pMVar8->sourceEntry).dataOffset = 0x220;
      (pMVar8->sourceEntry).pixelWidth = MVar3;
      (pMVar8->sourceEntry).pixelHeight = extraout_EDX_03;
      (pMVar8->sourceEntry).logicalWidth = MVar3;
      (pMVar8->sourceEntry).logicalHeight = extraout_EDX_03;
      (pMVar8->sourceEntry).paletteIndex = -1;
      (pMVar8->sourceEntry).originX = 0;
      (pMVar8->sourceEntry).originY = 0;
      pMVar8->remainingVideoBytes = local_2c;
      pMVar8->streamHandle = local_20;
      pMVar8->loadedVideoEnd = local_28;
      MVar5 = g_MovieDefaultAudioGainQ15;
      pMVar8->streamHandleIsSharedPackage = local_1c;
      pMVar8->openFlags = movieOpenFlags;
      pMVar8->streamFileOffset = (MovieStreamFileOffset)local_24;
      pMVar8->audioGainQ15 = MVar5;
      lpThreadId = &local_2c;
      pMVar8->workerActive = 0;
      pMVar8->streamState = MOVIE_STREAM_IDLE;
      pMVar8->refillSemaphore = (void *)0x0;
      if ((local_2c != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
        pMVar8->workerActive = pMVar8->workerActive + 1;
        dwCreationFlags = 0;
        lpParameter = (LPVOID)0x0;
        lpStartAddress = Movie_StreamWorkerThread;
        dwStackSize = 0;
        lpThreadAttributes = (LPSECURITY_ATTRIBUTES)0x0;
        pvVar9 = CreateSemaphoreA((LPSECURITY_ATTRIBUTES)0x0,0,1,(LPCSTR)0x0);
        pMVar8->refillSemaphore = pvVar9;
        pvVar9 = CreateThread(lpThreadAttributes,dwStackSize,lpStartAddress,lpParameter,
                              dwCreationFlags,lpThreadId);
        if (pvVar9 == (HANDLE)0x0) {
          pMVar8->workerActive = pMVar8->workerActive - 1;
        }
        else {
          CloseHandle(pvVar9);
        }
      }
      return pMVar7->subresourceTableOffset;
    }
  }
  else {
    if (1 < uVar2) {
      dVar10 = Random_NextPrimary();
      uVar12 = (dVar10 & 0xffff) % extraout_EDX_00;
    }
    iVar11 = 0;
    pMVar8 = pMVar7;
    for (; uVar12 != 0; uVar12 = uVar12 - 1) {
      iVar11 = iVar11 + pMVar8->currentFrameIndex;
      pMVar8 = (MovieRuntime *)&(pMVar8->textureCommon).allocationSizeBytes;
    }
    bVar18 = false;
    if (pMVar8->currentFrameIndex == 0) goto Movie_OpenAllocateAndInitializeRuntime;
    pMVar8 = (MovieRuntime *)
             (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_CURRENT,iVar11 + local_2c,unaff_EBX);
    if ((!bVar18) && (pMVar8 = (*g_MemoryApi.alloc)(bytes), !bVar18)) {
      pMVar8 = (MovieRuntime *)(*g_FileSystemReadExactCf)(byteCount_00,pMVar8,unaff_EBX);
      memory_00 = arg0;
      if ((!bVar18) &&
         (pMVar8 = (MovieRuntime *)(*g_SoundCreateSampleVoiceSet)(arg0), memory_00 = memory, !bVar18
         )) {
        (*g_MemoryApi.free)(memory);
        goto Movie_OpenAllocateAndInitializeRuntime;
      }
      (*g_MemoryApi.free)(memory_00);
    }
  }
Movie_OpenReleaseHeaderAllocationAfterFailure:
  pMVar7 = pMVar8;
  (*g_MemoryApi.free)(local_18);
Movie_OpenCloseStandaloneStreamAndReturnFailure:
  if (local_1c == 0) {
    (*g_FileSystemClose)(unaff_EBX);
  }
  return (dword)pMVar7;
}

/* Address: 0x004A8A20.
   Ownership: movie/runtime/playback.
   Purpose: Returns the active movie source entry's logical width in EAX and logical height in EDX. Both are zero
   when no movie is open.
*/
MovieFrameDimensionsEdxEax8 Movie_GetFrameDimensions(void)

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
void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15)

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
  FileIoByteCount byteCount;
  int extraout_ECX;
  undefined1 uVar2;
  
  do {
    do {
      if ((((g_ActiveMovie == (MovieRuntime *)0x0) ||
           (MsgWaitForMultipleObjects(1,&g_ActiveMovie->refillSemaphore,0,0x100,0),
           pMVar1 = g_ActiveMovie, g_ActiveMovie == (MovieRuntime *)0x0)) ||
          (g_ActiveMovie->streamState == MOVIE_STREAM_SHUTDOWN)) ||
         ((g_ActiveMovie->workerActive == 0 || (g_ActiveMovie->remainingVideoBytes == 0))))
      goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
    } while (g_ActiveMovie->streamState == MOVIE_STREAM_IDLE);
    if ((uint)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200) {
      handle = g_ActiveMovie->streamHandle;
      uVar2 = g_ActiveMovie->remainingVideoBytes < 0x1e000;
      (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,g_ActiveMovie->streamFileOffset,handle);
      (*g_FileSystemReadExactCf)(byteCount,pMVar1->loadedVideoEnd,handle);
      if ((bool)uVar2) {
        if (pMVar1->streamState != MOVIE_STREAM_SHUTDOWN) {
          pMVar1->streamState = MOVIE_STREAM_READ_FAILED;
        }
Movie_StreamWorkerThread_ClearWorkerActiveAndReturn:
        if (g_ActiveMovie != (MovieRuntime *)0x0) {
          g_ActiveMovie->workerActive = 0;
        }
        return 0;
      }
      pMVar1->remainingVideoBytes = pMVar1->remainingVideoBytes - extraout_ECX;
      pMVar1->streamFileOffset = pMVar1->streamFileOffset + extraout_ECX;
      pMVar1->loadedVideoEnd = pMVar1->loadedVideoEnd + extraout_ECX;
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
void Movie_Close(void)

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
void EndMovieUiRuntime_HandleModeTransitionCf(void *endMovieRuntime)

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
void EndMovieUiRuntime_DispatchCommandByFlagsCf
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
undefined8 __fastcall
Movie_EncodeFrame4x4Keyframe
          (undefined4 param_1,undefined4 param_2,uint param_3,uint param_4,uint *param_5,
          uint *param_6)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar5;
  uint *puVar6;
  uint *puVar7;
  undefined1 uVar8;
  undefined1 uVar9;
  undefined1 uVar10;
  undefined1 uVar11;
  undefined1 uVar12;
  undefined1 uVar13;
  undefined1 uVar14;
  undefined1 uVar15;
  ushort uVar16;
  ushort uVar19;
  uint uVar17;
  ushort uVar20;
  undefined8 uVar18;
  ushort uVar21;
  undefined8 uVar22;
  uint uStack_24;
  uint uStack_20;
  
  uStack_20 = param_3 >> 2;
  puVar7 = param_5;
  uStack_24 = param_4 >> 2;
  do {
    do {
      uVar3 = *param_6;
      uVar17 = param_6[1];
      uVar1 = param_6[2];
      uVar2 = param_6[3];
      uVar8 = (undefined1)(uVar3 >> 0x18);
      uVar16 = CONCAT11(uVar8,uVar8);
      uVar9 = (undefined1)(uVar3 >> 0x10);
      uVar8 = (undefined1)(uVar3 >> 8);
      uVar10 = (undefined1)(uVar17 >> 0x18);
      uVar19 = CONCAT11(uVar10,uVar10);
      uVar11 = (undefined1)(uVar17 >> 0x10);
      uVar10 = (undefined1)(uVar17 >> 8);
      uVar12 = (undefined1)(uVar1 >> 0x18);
      uVar20 = CONCAT11(uVar12,uVar12);
      uVar13 = (undefined1)(uVar1 >> 0x10);
      uVar12 = (undefined1)(uVar1 >> 8);
      uVar14 = (undefined1)(uVar2 >> 0x18);
      uVar21 = CONCAT11(uVar14,uVar14);
      uVar15 = (undefined1)(uVar2 >> 0x10);
      uVar14 = (undefined1)(uVar2 >> 8);
      uVar22 = CONCAT26((uVar21 >> 6) + (uVar20 >> 6) + (uVar16 >> 6) + (uVar19 >> 6),
                        CONCAT24(((ushort)(CONCAT35(CONCAT21(uVar21,uVar15),CONCAT14(uVar15,uVar2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar20,uVar13),CONCAT14(uVar13,uVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar16,uVar9),CONCAT14(uVar9,uVar3)) >>
                                          0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar19,uVar11),CONCAT14(uVar11,uVar17))
                                          >> 0x20) >> 6),
                                 CONCAT22((CONCAT11(uVar14,uVar14) >> 6) +
                                          (CONCAT11(uVar12,uVar12) >> 6) +
                                          (CONCAT11(uVar8,uVar8) >> 6) +
                                          (CONCAT11(uVar10,uVar10) >> 6),
                                          (CONCAT11((char)uVar2,(char)uVar2) >> 6) +
                                          (CONCAT11((char)uVar1,(char)uVar1) >> 6) +
                                          (CONCAT11((char)uVar3,(char)uVar3) >> 6) +
                                          (CONCAT11((char)uVar17,(char)uVar17) >> 6))));
      MovieColor_ComputeLuma5FromRgb888(*param_6);
      MovieColor_ComputeLuma5FromRgb888(param_6[1]);
      MovieColor_ComputeLuma5FromRgb888(param_6[2]);
      MovieColor_ComputeLuma5FromRgb888(param_6[3]);
      puVar6 = param_6 + param_4;
      uVar3 = *puVar6;
      uVar17 = puVar6[1];
      uVar1 = puVar6[2];
      uVar2 = puVar6[3];
      uVar8 = (undefined1)(uVar3 >> 0x18);
      uVar16 = CONCAT11(uVar8,uVar8);
      uVar9 = (undefined1)(uVar3 >> 0x10);
      uVar8 = (undefined1)(uVar3 >> 8);
      uVar10 = (undefined1)(uVar17 >> 0x18);
      uVar19 = CONCAT11(uVar10,uVar10);
      uVar11 = (undefined1)(uVar17 >> 0x10);
      uVar10 = (undefined1)(uVar17 >> 8);
      uVar12 = (undefined1)(uVar1 >> 0x18);
      uVar20 = CONCAT11(uVar12,uVar12);
      uVar13 = (undefined1)(uVar1 >> 0x10);
      uVar12 = (undefined1)(uVar1 >> 8);
      uVar14 = (undefined1)(uVar2 >> 0x18);
      uVar21 = CONCAT11(uVar14,uVar14);
      uVar15 = (undefined1)(uVar2 >> 0x10);
      uVar14 = (undefined1)(uVar2 >> 8);
      uVar22 = CONCAT26((short)((ulonglong)uVar22 >> 0x30) + (uVar16 >> 6) + (uVar19 >> 6) +
                        (uVar20 >> 6) + (uVar21 >> 6),
                        CONCAT24((short)((ulonglong)uVar22 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar16,uVar9),CONCAT14(uVar9,uVar3)) >>
                                          0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar19,uVar11),CONCAT14(uVar11,uVar17))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar20,uVar13),CONCAT14(uVar13,uVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar21,uVar15),CONCAT14(uVar15,uVar2))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar22 >> 0x10) +
                                          (CONCAT11(uVar8,uVar8) >> 6) +
                                          (CONCAT11(uVar10,uVar10) >> 6) +
                                          (CONCAT11(uVar12,uVar12) >> 6) +
                                          (CONCAT11(uVar14,uVar14) >> 6),
                                          (short)uVar22 +
                                          (CONCAT11((char)uVar3,(char)uVar3) >> 6) +
                                          (CONCAT11((char)uVar17,(char)uVar17) >> 6) +
                                          (CONCAT11((char)uVar1,(char)uVar1) >> 6) +
                                          (CONCAT11((char)uVar2,(char)uVar2) >> 6))));
      MovieColor_ComputeLuma5FromRgb888(*puVar6);
      MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
      MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
      MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
      puVar6 = puVar6 + param_4;
      uVar3 = *puVar6;
      uVar17 = puVar6[1];
      uVar1 = puVar6[2];
      uVar2 = puVar6[3];
      uVar8 = (undefined1)(uVar3 >> 0x18);
      uVar16 = CONCAT11(uVar8,uVar8);
      uVar9 = (undefined1)(uVar3 >> 0x10);
      uVar8 = (undefined1)(uVar3 >> 8);
      uVar10 = (undefined1)(uVar17 >> 0x18);
      uVar19 = CONCAT11(uVar10,uVar10);
      uVar11 = (undefined1)(uVar17 >> 0x10);
      uVar10 = (undefined1)(uVar17 >> 8);
      uVar12 = (undefined1)(uVar1 >> 0x18);
      uVar20 = CONCAT11(uVar12,uVar12);
      uVar13 = (undefined1)(uVar1 >> 0x10);
      uVar12 = (undefined1)(uVar1 >> 8);
      uVar14 = (undefined1)(uVar2 >> 0x18);
      uVar21 = CONCAT11(uVar14,uVar14);
      uVar15 = (undefined1)(uVar2 >> 0x10);
      uVar14 = (undefined1)(uVar2 >> 8);
      uVar22 = CONCAT26((short)((ulonglong)uVar22 >> 0x30) + (uVar16 >> 6) + (uVar19 >> 6) +
                        (uVar20 >> 6) + (uVar21 >> 6),
                        CONCAT24((short)((ulonglong)uVar22 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar16,uVar9),CONCAT14(uVar9,uVar3)) >>
                                          0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar19,uVar11),CONCAT14(uVar11,uVar17))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar20,uVar13),CONCAT14(uVar13,uVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar21,uVar15),CONCAT14(uVar15,uVar2))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar22 >> 0x10) +
                                          (CONCAT11(uVar8,uVar8) >> 6) +
                                          (CONCAT11(uVar10,uVar10) >> 6) +
                                          (CONCAT11(uVar12,uVar12) >> 6) +
                                          (CONCAT11(uVar14,uVar14) >> 6),
                                          (short)uVar22 +
                                          (CONCAT11((char)uVar3,(char)uVar3) >> 6) +
                                          (CONCAT11((char)uVar17,(char)uVar17) >> 6) +
                                          (CONCAT11((char)uVar1,(char)uVar1) >> 6) +
                                          (CONCAT11((char)uVar2,(char)uVar2) >> 6))));
      MovieColor_ComputeLuma5FromRgb888(*puVar6);
      MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
      MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
      MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
      puVar6 = puVar6 + param_4;
      uVar3 = *puVar6;
      uVar17 = puVar6[1];
      uVar1 = puVar6[2];
      uVar2 = puVar6[3];
      uVar8 = (undefined1)(uVar3 >> 0x18);
      uVar16 = CONCAT11(uVar8,uVar8);
      uVar9 = (undefined1)(uVar3 >> 0x10);
      uVar8 = (undefined1)(uVar3 >> 8);
      uVar10 = (undefined1)(uVar17 >> 0x18);
      uVar19 = CONCAT11(uVar10,uVar10);
      uVar11 = (undefined1)(uVar17 >> 0x10);
      uVar10 = (undefined1)(uVar17 >> 8);
      uVar12 = (undefined1)(uVar1 >> 0x18);
      uVar20 = CONCAT11(uVar12,uVar12);
      uVar13 = (undefined1)(uVar1 >> 0x10);
      uVar12 = (undefined1)(uVar1 >> 8);
      uVar14 = (undefined1)(uVar2 >> 0x18);
      uVar21 = CONCAT11(uVar14,uVar14);
      uVar15 = (undefined1)(uVar2 >> 0x10);
      uVar14 = (undefined1)(uVar2 >> 8);
      uVar18 = CONCAT26((short)((ulonglong)uVar22 >> 0x30) + (uVar16 >> 6) + (uVar19 >> 6) +
                        (uVar20 >> 6) + (uVar21 >> 6),
                        CONCAT24((short)((ulonglong)uVar22 >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar16,uVar9),CONCAT14(uVar9,uVar3)) >>
                                          0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar19,uVar11),CONCAT14(uVar11,uVar17))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar20,uVar13),CONCAT14(uVar13,uVar1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(uVar21,uVar15),CONCAT14(uVar15,uVar2))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)uVar22 >> 0x10) +
                                          (CONCAT11(uVar8,uVar8) >> 6) +
                                          (CONCAT11(uVar10,uVar10) >> 6) +
                                          (CONCAT11(uVar12,uVar12) >> 6) +
                                          (CONCAT11(uVar14,uVar14) >> 6),
                                          (short)uVar22 +
                                          (CONCAT11((char)uVar3,(char)uVar3) >> 6) +
                                          (CONCAT11((char)uVar17,(char)uVar17) >> 6) +
                                          (CONCAT11((char)uVar1,(char)uVar1) >> 6) +
                                          (CONCAT11((char)uVar2,(char)uVar2) >> 6))));
      MovieColor_ComputeLuma5FromRgb888(*puVar6);
      MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
      MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
      uVar22 = MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
      iVar5 = (int)((ulonglong)uVar22 >> 0x20);
      iVar4 = (int)uVar22;
      if ((extraout_ECX <= iVar4) && (uVar22 = CONCAT44(iVar5,extraout_ECX), iVar5 < iVar4)) {
        uVar22 = CONCAT44(iVar4,extraout_ECX);
      }
      iVar4 = (int)((ulonglong)uVar22 >> 0x20);
      uVar16 = (ushort)uVar18 >> 6;
      uVar19 = (ushort)((ulonglong)uVar18 >> 0x10) >> 6;
      uVar20 = (ushort)((ulonglong)uVar18 >> 0x20) >> 6;
      uVar21 = (ushort)((ulonglong)uVar18 >> 0x36);
      uVar17 = CONCAT13((uVar21 != 0) * (uVar21 < 0x100) * (char)uVar21 - (0xff < uVar21),
                        CONCAT12((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20 - (0xff < uVar20),
                                 CONCAT11((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 -
                                          (0xff < uVar19),
                                          (uVar16 != 0) * (uVar16 < 0x100) * (char)uVar16 -
                                          (0xff < uVar16))));
      uVar3 = (int)uVar22 + -8 + iVar4 >> 1;
      if ((int)uVar3 < 0) {
        uVar3 = 0;
      }
      else if (0x18 < (int)uVar3) {
        uVar3 = 0x18;
      }
      if ((uint)(iVar4 - (int)uVar22) < 0xc) {
        *puVar7 = uVar3;
        MovieColor_ComputeChromaCodeFromRgb888(uVar17);
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        uVar22 = MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar7[1] = (uint)((ulonglong)uVar22 >> 0x20);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        uVar22 = MovieColor_ComputeLuma5FromRgb888(*puVar6);
        iVar4 = (int)uVar22 - extraout_ECX_00;
        if (iVar4 < 0) {
          iVar4 = 0;
        }
        else if (7 < iVar4) {
          iVar4 = 7;
        }
        *puVar7 = *puVar7 | (uint)((ulonglong)uVar22 >> 0x20) | iVar4 << 5;
      }
      else {
        uVar3 = uVar3 - 4;
        if ((int)uVar3 < 0) {
          uVar3 = 0;
        }
        else if (0x10 < (int)uVar3) {
          uVar3 = 0x10;
        }
        *puVar7 = uVar3;
        MovieColor_ComputeChromaCodeFromRgb888(uVar17);
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        uVar22 = MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar7[1] = (uint)((ulonglong)uVar22 >> 0x20);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        MovieColor_ComputeLuma5FromRgb888(*puVar6);
        puVar6 = puVar6 + -param_4;
        MovieColor_ComputeLuma5FromRgb888(puVar6[3]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar6[1]);
        uVar22 = MovieColor_ComputeLuma5FromRgb888(*puVar6);
        uVar3 = (int)uVar22 - extraout_ECX_01;
        if ((int)uVar3 < 0) {
          uVar3 = 0;
        }
        else if (0xf < (int)uVar3) {
          uVar3 = 0xf;
        }
        *puVar7 = *puVar7 | (uint)((ulonglong)uVar22 >> 0x20) | (uVar3 >> 1) << 5;
      }
      param_6 = puVar6 + 4;
      puVar7 = puVar7 + 2;
      uStack_24 = uStack_24 - 1;
    } while (uStack_24 != 0);
    param_6 = param_6 + param_4 * 3;
    uStack_20 = uStack_20 - 1;
    uStack_24 = param_4 >> 2;
  } while (uStack_20 != 0);
  return CONCAT44(param_2,(int)puVar7 - (int)param_5);
}

/* Address: 0x004A7770.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode frame4x4 delta.
   Local calls: MovieColor_ComputeLuma5FromRgb888, MovieColor_ComputeChromaCodeFromRgb888.
*/
ulonglong __fastcall
Movie_EncodeFrame4x4Delta
          (undefined4 param_1,undefined4 param_2,uint param_3,uint param_4,uint *param_5,int param_6
          ,ulonglong *param_7)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  ulonglong *puVar8;
  uint uVar9;
  int iVar10;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar11;
  uint *puVar12;
  ulonglong *puVar13;
  uint *puVar14;
  undefined1 uVar16;
  undefined1 uVar17;
  ulonglong uVar15;
  undefined1 uVar18;
  undefined1 uVar19;
  undefined1 uVar20;
  undefined1 uVar21;
  undefined1 uVar22;
  undefined1 uVar23;
  ushort uVar24;
  ushort uVar27;
  uint uVar25;
  ushort uVar28;
  undefined8 uVar26;
  ushort uVar29;
  undefined8 uVar30;
  uint uStack_28;
  uint uStack_24;
  uint uStack_1c;
  
  iVar1 = param_4 * 4;
  uStack_24 = param_3 >> 2;
  uStack_1c = 0;
  puVar13 = param_7;
  puVar14 = param_5;
  uStack_28 = param_4 >> 2;
  do {
    do {
      puVar8 = (ulonglong *)((int)puVar13 + (param_6 - (int)param_7));
      uVar15 = _DAT_004a6d90 &
               (*puVar13 & _DAT_004a6d90 ^ *puVar8 | puVar13[1] & _DAT_004a6d90 ^ puVar8[1] |
                *(ulonglong *)(iVar1 + (int)puVar13) & _DAT_004a6d90 ^
                *(ulonglong *)((int)puVar8 + iVar1) |
                *(ulonglong *)(iVar1 + 8 + (int)puVar13) & _DAT_004a6d90 ^
                *(ulonglong *)((int)puVar8 + iVar1 + 8) |
                puVar13[param_4] & _DAT_004a6d90 ^ puVar8[param_4] |
                puVar13[param_4 + 1] & _DAT_004a6d90 ^ puVar8[param_4 + 1] |
               *(ulonglong *)((int)puVar13 + param_4 * 0xc) & _DAT_004a6d90 ^
               *(ulonglong *)((int)puVar8 + param_4 * 0xc) |
               *(ulonglong *)((int)puVar13 + param_4 * 0xc + 8) & _DAT_004a6d90 ^
               *(ulonglong *)((int)puVar8 + param_4 * 0xc + 8));
      uVar6 = uStack_1c + 1;
      if ((int)(uVar15 >> 0x20) != 0 || (int)uVar15 != 0) {
        if (uStack_1c != 0) {
          if (uStack_1c < 9) {
            *(byte *)puVar14 = ((char)uStack_1c + -1) * ' ' | 0x19;
            puVar14 = (uint *)((int)puVar14 + 1);
          }
          else if (uStack_1c < 0x809) {
            *(ushort *)puVar14 = ((short)uStack_1c + -9) * 0x20 | 0x1a;
            puVar14 = (uint *)((int)puVar14 + 2);
          }
          else {
            *puVar14 = (uStack_1c - 0x809) * 0x20 | 0x1b;
            puVar14 = puVar14 + 1;
          }
          uStack_1c = 0;
        }
        puVar8 = (ulonglong *)((int)puVar13 + (param_6 - (int)param_7));
        uVar15 = puVar13[1];
        uVar30 = *(undefined8 *)((int)puVar13 + param_4 * 4);
        uVar26 = *(undefined8 *)((int)puVar13 + (param_4 + 2) * 4);
        *puVar8 = *puVar13;
        puVar8[1] = uVar15;
        *(undefined8 *)((int)puVar8 + iVar1) = uVar30;
        *(undefined8 *)((int)puVar8 + iVar1 + 8) = uVar26;
        uVar15 = puVar13[param_4 + 1];
        uVar30 = *(undefined8 *)((int)puVar13 + param_4 * 0xc);
        uVar26 = *(undefined8 *)((int)puVar13 + (param_4 * 3 + 2) * 4);
        puVar8[param_4] = puVar13[param_4];
        puVar8[param_4 + 1] = uVar15;
        *(undefined8 *)((int)puVar8 + param_4 * 0xc) = uVar30;
        *(undefined8 *)((int)puVar8 + param_4 * 0xc + 8) = uVar26;
        uVar2 = (undefined4)*puVar13;
        uVar3 = *(undefined4 *)((int)puVar13 + 4);
        uVar4 = (undefined4)puVar13[1];
        uVar5 = *(undefined4 *)((int)puVar13 + 0xc);
        uVar16 = (undefined1)((uint)uVar2 >> 0x18);
        uVar24 = CONCAT11(uVar16,uVar16);
        uVar17 = (undefined1)((uint)uVar2 >> 0x10);
        uVar16 = (undefined1)((uint)uVar2 >> 8);
        uVar18 = (undefined1)((uint)uVar3 >> 0x18);
        uVar27 = CONCAT11(uVar18,uVar18);
        uVar19 = (undefined1)((uint)uVar3 >> 0x10);
        uVar18 = (undefined1)((uint)uVar3 >> 8);
        uVar20 = (undefined1)((uint)uVar4 >> 0x18);
        uVar28 = CONCAT11(uVar20,uVar20);
        uVar21 = (undefined1)((uint)uVar4 >> 0x10);
        uVar20 = (undefined1)((uint)uVar4 >> 8);
        uVar22 = (undefined1)((uint)uVar5 >> 0x18);
        uVar29 = CONCAT11(uVar22,uVar22);
        uVar23 = (undefined1)((uint)uVar5 >> 0x10);
        uVar22 = (undefined1)((uint)uVar5 >> 8);
        uVar30 = CONCAT26((uVar29 >> 6) + (uVar28 >> 6) + (uVar24 >> 6) + (uVar27 >> 6),
                          CONCAT24(((ushort)(CONCAT35(CONCAT21(uVar29,uVar23),CONCAT14(uVar23,uVar5)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar28,uVar21),CONCAT14(uVar21,uVar4)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar24,uVar17),CONCAT14(uVar17,uVar2)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar27,uVar19),CONCAT14(uVar19,uVar3)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((CONCAT11(uVar22,uVar22) >> 6) +
                                            (CONCAT11(uVar20,uVar20) >> 6) +
                                            (CONCAT11(uVar16,uVar16) >> 6) +
                                            (CONCAT11(uVar18,uVar18) >> 6),
                                            (CONCAT11((char)uVar5,(char)uVar5) >> 6) +
                                            (CONCAT11((char)uVar4,(char)uVar4) >> 6) +
                                            (CONCAT11((char)uVar2,(char)uVar2) >> 6) +
                                            (CONCAT11((char)uVar3,(char)uVar3) >> 6))));
        MovieColor_ComputeLuma5FromRgb888((uint)*puVar13);
        MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 4));
        MovieColor_ComputeLuma5FromRgb888((uint)puVar13[1]);
        MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 0xc));
        puVar12 = (uint *)((int)puVar13 + param_4 * 4);
        uVar6 = *puVar12;
        uVar9 = puVar12[1];
        uVar25 = puVar12[2];
        uVar7 = puVar12[3];
        uVar16 = (undefined1)(uVar6 >> 0x18);
        uVar24 = CONCAT11(uVar16,uVar16);
        uVar17 = (undefined1)(uVar6 >> 0x10);
        uVar16 = (undefined1)(uVar6 >> 8);
        uVar18 = (undefined1)(uVar9 >> 0x18);
        uVar27 = CONCAT11(uVar18,uVar18);
        uVar19 = (undefined1)(uVar9 >> 0x10);
        uVar18 = (undefined1)(uVar9 >> 8);
        uVar20 = (undefined1)(uVar25 >> 0x18);
        uVar28 = CONCAT11(uVar20,uVar20);
        uVar21 = (undefined1)(uVar25 >> 0x10);
        uVar20 = (undefined1)(uVar25 >> 8);
        uVar22 = (undefined1)(uVar7 >> 0x18);
        uVar29 = CONCAT11(uVar22,uVar22);
        uVar23 = (undefined1)(uVar7 >> 0x10);
        uVar22 = (undefined1)(uVar7 >> 8);
        uVar30 = CONCAT26((short)((ulonglong)uVar30 >> 0x30) + (uVar24 >> 6) + (uVar27 >> 6) +
                          (uVar28 >> 6) + (uVar29 >> 6),
                          CONCAT24((short)((ulonglong)uVar30 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar24,uVar17),CONCAT14(uVar17,uVar6)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar27,uVar19),CONCAT14(uVar19,uVar9)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar28,uVar21),
                                                      CONCAT14(uVar21,uVar25)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar29,uVar23),CONCAT14(uVar23,uVar7)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar30 >> 0x10) +
                                            (CONCAT11(uVar16,uVar16) >> 6) +
                                            (CONCAT11(uVar18,uVar18) >> 6) +
                                            (CONCAT11(uVar20,uVar20) >> 6) +
                                            (CONCAT11(uVar22,uVar22) >> 6),
                                            (short)uVar30 +
                                            (CONCAT11((char)uVar6,(char)uVar6) >> 6) +
                                            (CONCAT11((char)uVar9,(char)uVar9) >> 6) +
                                            (CONCAT11((char)uVar25,(char)uVar25) >> 6) +
                                            (CONCAT11((char)uVar7,(char)uVar7) >> 6))));
        MovieColor_ComputeLuma5FromRgb888(*puVar12);
        MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
        MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
        puVar12 = puVar12 + param_4;
        uVar6 = *puVar12;
        uVar9 = puVar12[1];
        uVar25 = puVar12[2];
        uVar7 = puVar12[3];
        uVar16 = (undefined1)(uVar6 >> 0x18);
        uVar24 = CONCAT11(uVar16,uVar16);
        uVar17 = (undefined1)(uVar6 >> 0x10);
        uVar16 = (undefined1)(uVar6 >> 8);
        uVar18 = (undefined1)(uVar9 >> 0x18);
        uVar27 = CONCAT11(uVar18,uVar18);
        uVar19 = (undefined1)(uVar9 >> 0x10);
        uVar18 = (undefined1)(uVar9 >> 8);
        uVar20 = (undefined1)(uVar25 >> 0x18);
        uVar28 = CONCAT11(uVar20,uVar20);
        uVar21 = (undefined1)(uVar25 >> 0x10);
        uVar20 = (undefined1)(uVar25 >> 8);
        uVar22 = (undefined1)(uVar7 >> 0x18);
        uVar29 = CONCAT11(uVar22,uVar22);
        uVar23 = (undefined1)(uVar7 >> 0x10);
        uVar22 = (undefined1)(uVar7 >> 8);
        uVar30 = CONCAT26((short)((ulonglong)uVar30 >> 0x30) + (uVar24 >> 6) + (uVar27 >> 6) +
                          (uVar28 >> 6) + (uVar29 >> 6),
                          CONCAT24((short)((ulonglong)uVar30 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar24,uVar17),CONCAT14(uVar17,uVar6)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar27,uVar19),CONCAT14(uVar19,uVar9)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar28,uVar21),
                                                      CONCAT14(uVar21,uVar25)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar29,uVar23),CONCAT14(uVar23,uVar7)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar30 >> 0x10) +
                                            (CONCAT11(uVar16,uVar16) >> 6) +
                                            (CONCAT11(uVar18,uVar18) >> 6) +
                                            (CONCAT11(uVar20,uVar20) >> 6) +
                                            (CONCAT11(uVar22,uVar22) >> 6),
                                            (short)uVar30 +
                                            (CONCAT11((char)uVar6,(char)uVar6) >> 6) +
                                            (CONCAT11((char)uVar9,(char)uVar9) >> 6) +
                                            (CONCAT11((char)uVar25,(char)uVar25) >> 6) +
                                            (CONCAT11((char)uVar7,(char)uVar7) >> 6))));
        MovieColor_ComputeLuma5FromRgb888(*puVar12);
        MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
        MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
        MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
        puVar12 = puVar12 + param_4;
        uVar6 = *puVar12;
        uVar9 = puVar12[1];
        uVar25 = puVar12[2];
        uVar7 = puVar12[3];
        uVar16 = (undefined1)(uVar6 >> 0x18);
        uVar24 = CONCAT11(uVar16,uVar16);
        uVar17 = (undefined1)(uVar6 >> 0x10);
        uVar16 = (undefined1)(uVar6 >> 8);
        uVar18 = (undefined1)(uVar9 >> 0x18);
        uVar27 = CONCAT11(uVar18,uVar18);
        uVar19 = (undefined1)(uVar9 >> 0x10);
        uVar18 = (undefined1)(uVar9 >> 8);
        uVar20 = (undefined1)(uVar25 >> 0x18);
        uVar28 = CONCAT11(uVar20,uVar20);
        uVar21 = (undefined1)(uVar25 >> 0x10);
        uVar20 = (undefined1)(uVar25 >> 8);
        uVar22 = (undefined1)(uVar7 >> 0x18);
        uVar29 = CONCAT11(uVar22,uVar22);
        uVar23 = (undefined1)(uVar7 >> 0x10);
        uVar22 = (undefined1)(uVar7 >> 8);
        uVar26 = CONCAT26((short)((ulonglong)uVar30 >> 0x30) + (uVar24 >> 6) + (uVar27 >> 6) +
                          (uVar28 >> 6) + (uVar29 >> 6),
                          CONCAT24((short)((ulonglong)uVar30 >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar24,uVar17),CONCAT14(uVar17,uVar6)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar27,uVar19),CONCAT14(uVar19,uVar9)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar28,uVar21),
                                                      CONCAT14(uVar21,uVar25)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(uVar29,uVar23),CONCAT14(uVar23,uVar7)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)uVar30 >> 0x10) +
                                            (CONCAT11(uVar16,uVar16) >> 6) +
                                            (CONCAT11(uVar18,uVar18) >> 6) +
                                            (CONCAT11(uVar20,uVar20) >> 6) +
                                            (CONCAT11(uVar22,uVar22) >> 6),
                                            (short)uVar30 +
                                            (CONCAT11((char)uVar6,(char)uVar6) >> 6) +
                                            (CONCAT11((char)uVar9,(char)uVar9) >> 6) +
                                            (CONCAT11((char)uVar25,(char)uVar25) >> 6) +
                                            (CONCAT11((char)uVar7,(char)uVar7) >> 6))));
        MovieColor_ComputeLuma5FromRgb888(*puVar12);
        MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
        MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
        uVar30 = MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
        iVar11 = (int)((ulonglong)uVar30 >> 0x20);
        iVar10 = (int)uVar30;
        if ((extraout_ECX <= iVar10) && (uVar30 = CONCAT44(iVar11,extraout_ECX), iVar11 < iVar10)) {
          uVar30 = CONCAT44(iVar10,extraout_ECX);
        }
        iVar10 = (int)((ulonglong)uVar30 >> 0x20);
        uVar24 = (ushort)uVar26 >> 6;
        uVar27 = (ushort)((ulonglong)uVar26 >> 0x10) >> 6;
        uVar28 = (ushort)((ulonglong)uVar26 >> 0x20) >> 6;
        uVar29 = (ushort)((ulonglong)uVar26 >> 0x36);
        uVar25 = CONCAT13((uVar29 != 0) * (uVar29 < 0x100) * (char)uVar29 - (0xff < uVar29),
                          CONCAT12((uVar28 != 0) * (uVar28 < 0x100) * (char)uVar28 - (0xff < uVar28)
                                   ,CONCAT11((uVar27 != 0) * (uVar27 < 0x100) * (char)uVar27 -
                                             (0xff < uVar27),
                                             (uVar24 != 0) * (uVar24 < 0x100) * (char)uVar24 -
                                             (0xff < uVar24))));
        uVar9 = (int)uVar30 + -8 + iVar10 >> 1;
        if ((int)uVar9 < 0) {
          uVar9 = 0;
        }
        else if (0x18 < uVar9) {
          uVar9 = 0x18;
        }
        uVar6 = uStack_1c;
        if ((uint)(iVar10 - (int)uVar30) < 0xc) {
          *puVar14 = uVar9;
          MovieColor_ComputeChromaCodeFromRgb888(uVar25);
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar12 = puVar12 + -param_4;
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          uVar30 = MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar14[1] = (uint)((ulonglong)uVar30 >> 0x20);
          puVar12 = puVar12 + -param_4;
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar13 = (ulonglong *)(puVar12 + -param_4);
          MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 0xc));
          MovieColor_ComputeLuma5FromRgb888((uint)puVar13[1]);
          MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 4));
          uVar30 = MovieColor_ComputeLuma5FromRgb888((uint)*puVar13);
          iVar10 = (int)uVar30 - extraout_ECX_00;
          if (iVar10 < 0) {
            iVar10 = 0;
          }
          else if (7 < iVar10) {
            iVar10 = 7;
          }
          *puVar14 = *puVar14 | (uint)((ulonglong)uVar30 >> 0x20) | iVar10 << 5;
          puVar14 = puVar14 + 2;
        }
        else {
          uVar9 = uVar9 - 4;
          if ((int)uVar9 < 0) {
            uVar9 = 0;
          }
          else if (0x10 < (int)uVar9) {
            uVar9 = 0x10;
          }
          *puVar14 = uVar9;
          MovieColor_ComputeChromaCodeFromRgb888(uVar25);
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar12 = puVar12 + -param_4;
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          uVar30 = MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar14[1] = (uint)((ulonglong)uVar30 >> 0x20);
          puVar12 = puVar12 + -param_4;
          MovieColor_ComputeLuma5FromRgb888(puVar12[3]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[2]);
          MovieColor_ComputeLuma5FromRgb888(puVar12[1]);
          MovieColor_ComputeLuma5FromRgb888(*puVar12);
          puVar13 = (ulonglong *)(puVar12 + -param_4);
          MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 0xc));
          MovieColor_ComputeLuma5FromRgb888((uint)puVar13[1]);
          MovieColor_ComputeLuma5FromRgb888(*(uint *)((int)puVar13 + 4));
          uVar30 = MovieColor_ComputeLuma5FromRgb888((uint)*puVar13);
          uVar9 = (int)uVar30 - extraout_ECX_01;
          if ((int)uVar9 < 0) {
            uVar9 = 0;
          }
          else if (0xf < (int)uVar9) {
            uVar9 = 0xf;
          }
          *puVar14 = *puVar14 | (uint)((ulonglong)uVar30 >> 0x20) | (uVar9 >> 1) << 5;
          puVar14 = puVar14 + 2;
        }
      }
      uStack_1c = uVar6;
      puVar13 = puVar13 + 2;
      uStack_28 = uStack_28 - 1;
    } while (uStack_28 != 0);
    puVar13 = (ulonglong *)((int)puVar13 + param_4 * 0xc);
    uStack_24 = uStack_24 - 1;
    uStack_28 = param_4 >> 2;
  } while (uStack_24 != 0);
  if (uStack_1c != 0) {
    if (uStack_1c < 9) {
      *(byte *)puVar14 = ((char)uStack_1c + -1) * ' ' | 0x19;
      puVar14 = (uint *)((int)puVar14 + 1);
    }
    else if (uStack_1c < 0x809) {
      *(ushort *)puVar14 = ((short)uStack_1c + -9) * 0x20 | 0x1a;
      puVar14 = (uint *)((int)puVar14 + 2);
    }
    else {
      *puVar14 = (uStack_1c - 0x809) * 0x20 | 0x1b;
      puVar14 = puVar14 + 1;
    }
  }
  return CONCAT44(param_2,(int)puVar14 + (7 - (int)param_5)) & 0xfffffffffffffff8;
}

/* Address: 0x004A8A60.
   Ownership: movie/runtime/playback.
   Purpose: Starts embedded audio on the first frame, decodes the next 4x4 delta frame when enough bytes are
   buffered, compacts the bounded stream buffer, and requests asynchronous refill. CF clear returns the active
   MovieRuntime in EAX; CF set reports end-of-movie or read failure.
   Local calls: Movie_DecodeFrame4x4Delta.
*/
MovieRuntime * Movie_AdvanceFrame(void)

{
  MovieFileHeader *pMVar1;
  MovieRuntime *pMVar2;
  MovieRuntime *pMVar3;
  IDirectSoundBuffer *pIVar4;
  dword dVar5;
  MovieFrameIndex extraout_ECX;
  MovieFrameIndex MVar6;
  MovieFrameIndex extraout_ECX_00;
  uint uVar7;
  uint uVar8;
  void *unaff_EBX;
  undefined4 *puVar9;
  byte *pbVar10;
  
  pMVar2 = g_ActiveMovie;
  pMVar3 = (MovieRuntime *)0x30;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_ActiveMovie->streamState == MOVIE_STREAM_READ_FAILED) {
      pMVar3 = (MovieRuntime *)(*g_FileSystemClose)(unaff_EBX);
      pMVar2->remainingVideoBytes = 0;
    }
    else {
      if (((g_ActiveMovie->streamState == MOVIE_STREAM_IDLE) && (g_ActiveMovie->workerActive != 0))
         && (g_ActiveMovie->remainingVideoBytes != 0)) {
        if ((uint)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200)
        {
          g_ActiveMovie->streamState = MOVIE_STREAM_FILL_REQUESTED;
          ReleaseSemaphore(pMVar2->refillSemaphore,1,(LPLONG)0x0);
        }
      }
      pMVar1 = pMVar2->fileHeader;
      MVar6 = pMVar2->currentFrameIndex;
      pbVar10 = (pMVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                (pMVar2->videoStreamOffset - 0x28);
      if ((MVar6 == 0) && (pMVar2->audioVoiceSet != (DirectSoundVoiceSet *)0x0)) {
        pIVar4 = (*g_SoundPlayOneShot)
                           (pMVar2->audioGainQ15,pMVar2->audioGainQ15,pMVar2->audioVoiceSet);
        pMVar2->activeAudioBuffer = pIVar4;
        MVar6 = extraout_ECX;
      }
      pMVar3 = (MovieRuntime *)(pMVar2->loadedVideoEnd + -(int)pbVar10);
      if (MVar6 + 1 <= pMVar1->frameCount) {
        if ((pMVar2->remainingVideoBytes != 0) && (pMVar3 < (MovieRuntime *)0x1e000)) {
          return (MovieRuntime *)&pMVar2[-1].textureCommon.allocationSizeBytes;
        }
        dVar5 = Movie_DecodeFrame4x4Delta
                          (pMVar1->heightPixels,pMVar1->widthPixels,pMVar2->argbPixels,pbVar10);
        pMVar2->currentFrameIndex = extraout_ECX_00;
        pMVar2->videoStreamOffset = pMVar2->videoStreamOffset + dVar5;
        if ((pMVar2->openFlags != 0) && (pMVar2->streamState == MOVIE_STREAM_IDLE)) {
          uVar8 = pMVar2->videoStreamOffset;
          uVar7 = (int)pMVar2->loadedVideoEnd - (int)pMVar2->fileHeader;
          if ((0x1e01ff < uVar8) && (uVar8 < uVar7)) {
            pMVar2->videoStreamOffset = pMVar2->videoStreamOffset - 0x1e0000;
            pbVar10 = pMVar2->fileHeader[-0xf00].common.buildMetadata.assetRelativeAddressAnchor28 +
                      (uVar8 - 0x28);
            pMVar2->loadedVideoEnd = pMVar2->loadedVideoEnd + -0x1e0000;
            puVar9 = (undefined4 *)(pbVar10 + 0x1e0000);
            for (uVar8 = uVar7 - uVar8 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
              *(undefined4 *)pbVar10 = *puVar9;
              puVar9 = puVar9 + 1;
              pbVar10 = pbVar10 + 4;
            }
          }
        }
        return pMVar2;
      }
    }
  }
  return pMVar3;
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
undefined8 MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame)

{
  undefined4 in_EAX;
  uint extraout_ECX;
  undefined4 in_EDX;
  bool bVar1;
  
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      bVar1 = true;
      Movie_AdvanceFrame();
      if (bVar1)
      goto 
      MoviePlayback_AdvanceToFrameAndPresent_ReturnAfterAlreadyReachedAdvanceFailureOrPresentation;
    } while (extraout_ECX < targetFrame);
    g_MoviePlaybackCurrentFrame = extraout_ECX;
    UiRootStack_InvalidateAll();
    UiFrame_Draw();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
MoviePlayback_AdvanceToFrameAndPresent_ReturnAfterAlreadyReachedAdvanceFailureOrPresentation:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004A81C0.
   Ownership: movie/runtime/playback.
   Purpose: Decodes one FLM frame into an existing ARGB image in 4x4 blocks. Tokens 0-24 encode one block through
   g_MovieChromaLumaToArgb, while tokens 25-31 skip runs and preserve pixels from the previous frame. Returns
   encoded bytes consumed rounded up to eight.
*/
dword Movie_DecodeFrame4x4Delta
                (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,
                dword *destinationArgb,byte *encodedFrame)

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
undefined8 MovieColor_ComputeChromaCodeFromRgb888(uint param_1)

{
  uint uVar1;
  undefined4 in_EDX;
  FixedLengthAngleEdxEax8 FVar2;
  
  uVar1 = param_1 >> 8 & 0xff;
  FVar2 = FixedMath_Vector2AngleAndLengthRegs
                    (((param_1 & 0xff) - uVar1) * 0xddb4,
                     (uVar1 + (param_1 & 0xff) + (param_1 >> 0x10 & 0xff) * -2) * 0x8000);
  return CONCAT44(in_EDX,(uint)FVar2 >> 9 & 0x7c00 | (uint)(FVar2 >> 0x26) & 0x3e0);
}

/* Address: 0x004A7000.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie color compute luma5 from rgb888.
*/
undefined8 MovieColor_ComputeLuma5FromRgb888(uint param_1)

{
  undefined4 in_EDX;
  
  return CONCAT44(in_EDX,((param_1 & 0xff) + (param_1 >> 8 & 0xff) + (param_1 >> 0x10 & 0xff)) *
                         0x5555 + 0x40000 >> 0x13);
}
