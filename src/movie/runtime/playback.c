/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/playback.c
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/playback.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
  dword packedTimeOrDate;
  void *frameToReleaseOrNull;
  uint byteCount;
  int clearCount;
  uint frameCount;
  uint *sourcePixels;
  uint *outputCursor;
  MovieFrameProviderEaxCf5 providerResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  void *firstFrame;
  
  outputCursor = outputBuffer;
  for (clearCount = 0x80; clearCount != 0; clearCount = clearCount + -1) {
    *outputCursor = 0;
    outputCursor = outputCursor + 1;
  }
  outputCursor[-0x80] = 0x6d6c66;
  outputCursor[-0x7f] = 0x200;
  outputCursor[-0x7e] = 1;
  outputCursor[-0x7d] = 0x20001;
  packedTimeOrDate = (*g_LocaleGetPackedCurrentTime)();
  outputCursor[-0x7c] = packedTimeOrDate;
  outputCursor[-0x7a] = packedTimeOrDate;
  outputCursor[-0x78] = packedTimeOrDate;
  packedTimeOrDate = (*g_LocaleGetPackedCurrentDate)();
  outputCursor[-0x7b] = packedTimeOrDate;
  outputCursor[-0x79] = packedTimeOrDate;
  outputCursor[-0x77] = packedTimeOrDate;
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(outputCursor + -0x74));
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(outputCursor + -100));
  *(undefined1 *)(outputCursor + -0x40) = 0;
  outputCursor[-0x54] = frameWidthPixels;
  outputCursor[-0x53] = frameHeightPixels;
  outputCursor[-0x52] = 0;
  outputCursor[-0x51] = 0;
  providerResult = (*frameProvider)((void *)0x0);
  frameToReleaseOrNull = providerResult.frameOrError;
  if (!providerResult.carry) {
    frameCount = 1;
    sourcePixels = (uint *)((int)frameToReleaseOrNull +
                           *(int *)((int)frameToReleaseOrNull +
                                   *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc));
    byteCount = Movie_EncodeFrame4x4Keyframe(frameHeightPixels,frameWidthPixels,outputCursor,sourcePixels);
    outputCursor = (uint *)((int)outputCursor + byteCount);
    firstFrame = frameToReleaseOrNull;
    while( true ) {
      providerResult = (*frameProvider)((void *)0x0);
      frameToReleaseOrNull = providerResult.frameOrError;
      if (providerResult.carry) break;
      frameCount = frameCount + 1;
      byteCount = Movie_EncodeFrame4x4Delta
                        (frameHeightPixels,frameWidthPixels,outputCursor,sourcePixels,
                         (uint *)(*(int *)((int)frameToReleaseOrNull +
                                          *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc) +
                                 (int)frameToReleaseOrNull));
      outputCursor = (uint *)((int)outputCursor + byteCount);
      (*frameProvider)(frameToReleaseOrNull);
    }
    (*frameProvider)(firstFrame);
    byteCount = (int)outputCursor - (int)outputBuffer;
    outputBuffer[0x2e] = frameCount;
    outputBuffer[0x3f] = 0x10;
    outputBuffer[1] = byteCount;
    outputBuffer[0x30] = byteCount;
    if (frameToReleaseOrNull == (void *)0xffffffff) {
      outputBuffer[0x30] = outputBuffer[0x30] - 0x200;
      successResult.carry = false;
      successResult.valueOrError = byteCount;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.valueOrError = (dword)frameToReleaseOrNull;
  return failureResult;
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
  uint boundaryFrame;
  
  g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + 1;
  if (g_MoviePlaybackScheduleSpan != 0) {
    targetFrame = (uint)(g_MoviePlaybackScheduleCounter * 8) / g_MoviePlaybackScheduleSpan + 1 +
                  g_MoviePlaybackBaseFrameGroup * 8;
    for (boundaryFrame = 8; boundaryFrame < targetFrame; boundaryFrame = boundaryFrame + 8) {
      if (g_MoviePlaybackCurrentFrame < boundaryFrame) {
        MoviePlayback_AdvanceToFrameAndPresent(boundaryFrame);
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
  MovieFileHeader *streamByteCount;
  uint audioTrackCount;
  MovieSubresourceCount frameWidth;
  MoviePaletteBankCount frameHeight;
  MovieAudioGainQ15 defaultAudioGain;
  MovieRuntime *flmBufferOrError;
  MovieRuntime *runtimeOrScratch;
  HANDLE semaphoreOrThread;
  dword sizeOrValue;
  int copyCountOrTrackOffset;
  MovieFileHeader *byteCount;
  uint tracksToSkip;
  MovieRuntime *voiceSetOrError;
  MovieRuntime *handle;
  AssetMagic *headerSource;
  bool readFailed;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  ArenaAllocEaxCf5 allocResult;
  SoundCreateSampleVoiceSetEaxCf5 voiceSetResult;
  MovieOpenEaxCf5 successResult;
  MovieOpenEaxCf5 failureResult;
  PackageFindEntryEaxEbxCf9 packageEntry;
  LPSECURITY_ATTRIBUTES lpThreadAttributes;
  SIZE_T dwStackSize;
  LPTHREAD_START_ROUTINE lpStartAddress;
  LPVOID lpParameter;
  DWORD dwCreationFlags;
  MovieStreamByteCount *lpThreadId;
  MovieStreamByteCount remainingByteCount;
  byte *loadedEnd;
  MovieRuntime *streamPosition;
  MovieRuntime *savedStreamHandle;
  MovieSharedStreamHandleFlag isSharedPackageHandle;
  MovieRuntime *headerAllocation;
  
  isSharedPackageHandle = 0;
  if (((movieOpenFlags & 0x80000000) == 0) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (MovieRuntime *)openResult.eax;
    if (openResult.carry) goto Movie_OpenResolvePackageOrFallbackStream;
  }
  else {
Movie_OpenResolvePackageOrFallbackStream:
    movieOpenFlags = movieOpenFlags & 0x7fffffff;
    packageEntry = Package_FindEntryAcrossMounts(path);
    if ((packageEntry.carry) ||
       (seekResult = (*g_FileSystemSeekCf)
                           (FILESYSTEM_SEEK_BEGIN,*(int *)(packageEntry.eax + 0x1ec) + 0x200,
                            (MovieRuntime *)packageEntry.ebx), seekResult.carry)){
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (MovieRuntime *)openResult.eax;
      if (openResult.carry) {
        openResult = (*g_FileSystemOpenCf)(0,path);
        flmBufferOrError = (MovieRuntime *)openResult.eax;
        handle = flmBufferOrError;
        if (openResult.carry) goto LAB_004a8a0b;
      }
    }
    else {
      isSharedPackageHandle = isSharedPackageHandle + 1;
      handle = (MovieRuntime *)packageEntry.ebx;
    }
  }
  headerSource = (AssetMagic *)g_PackageScratchBuffer;
  readResult = (*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,handle);
  flmBufferOrError = (MovieRuntime *)readResult.eax;
  if (!readResult.carry) {
    flmBufferOrError = (MovieRuntime *)0x30;
    if ((*headerSource == 0x6d6c66) &&
       (sizeOrValue = *(int *)((int)headerSource + 0xc0) + 0x200, *(int *)((int)headerSource + 0xc) == 0x20001)) {
      if ((0x3c0000 < sizeOrValue) && (movieOpenFlags != 0)) {
        sizeOrValue = 0x3c0000;
      }
      allocResult = (*g_MemoryApi.alloc)(sizeOrValue);
      flmBufferOrError = (MovieRuntime *)allocResult.eax;
      if (!allocResult.carry) {
        runtimeOrScratch = flmBufferOrError;
        for (copyCountOrTrackOffset = 0x80; copyCountOrTrackOffset != 0; copyCountOrTrackOffset = copyCountOrTrackOffset + -1) {
          (runtimeOrScratch->textureCommon).magic = *headerSource;
          headerSource = headerSource + 1;
          runtimeOrScratch = (MovieRuntime *)&(runtimeOrScratch->textureCommon).allocationSizeBytes;
        }
        streamByteCount = flmBufferOrError->fileHeader;
        byteCount = streamByteCount;
        if (((MovieFileHeader *)0x3a2000 < streamByteCount) && (movieOpenFlags != 0)) {
          byteCount = (MovieFileHeader *)0x3a2000;
        }
        remainingByteCount = (int)streamByteCount - (int)byteCount;
        loadedEnd = byteCount[-1].reserved100_1FF +
                   (int)(&(runtimeOrScratch->textureCommon).buildMetadata + 1) + 0x50;
        savedStreamHandle = handle;
        headerAllocation = flmBufferOrError;
        readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)byteCount,runtimeOrScratch,handle);
        readFailed = readResult.carry;
        runtimeOrScratch = (MovieRuntime *)readResult.eax;
        if ((readFailed) || (runtimeOrScratch = (MovieRuntime *)(*g_FileSystemGetPositionCf)(handle), readFailed))
        goto Movie_OpenReleaseHeaderAllocationAfterFailure;
        audioTrackCount = flmBufferOrError->reservedBC;
        tracksToSkip = 0;
        streamPosition = runtimeOrScratch;
        if ((audioTrackCount == 0) || (0xe < audioTrackCount)) {
LAB_004a87c0:
          voiceSetOrError = (MovieRuntime *)0x0;
Movie_OpenAllocateAndInitializeRuntime:
          sizeOrValue = flmBufferOrError->subresourceCount * flmBufferOrError->paletteBankCount * 4 + 0x220;
          allocResult = (*g_MemoryApi.alloc)(sizeOrValue);
          runtimeOrScratch = (MovieRuntime *)allocResult.eax;
          if (!allocResult.carry) {
            g_ActiveMovie = runtimeOrScratch;
            if ((isSharedPackageHandle == 0) && (remainingByteCount == 0)) {
              (*g_FileSystemClose)(handle);
            }
            (runtimeOrScratch->textureCommon).magic = ASSET_MAGIC_GFX;
            (runtimeOrScratch->textureCommon).allocationSizeBytes = sizeOrValue;
            (runtimeOrScratch->textureCommon).formatVersion = 1;
            (runtimeOrScratch->textureCommon).converterVersion = 0;
            runtimeOrScratch->audioVoiceSet = (DirectSoundVoiceSet *)voiceSetOrError;
            runtimeOrScratch->activeAudioBuffer = (IDirectSoundBuffer *)0x0;
            frameWidth = flmBufferOrError->subresourceCount;
            frameHeight = flmBufferOrError->paletteBankCount;
            sizeOrValue = (*g_LocaleGetPackedCurrentTime)();
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.dateValue0 = sizeOrValue;
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.dateValue1 = sizeOrValue;
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.dateValue2 = sizeOrValue;
            sizeOrValue = (*g_LocaleGetPackedCurrentDate)();
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.timeValue0 = sizeOrValue;
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.timeValue1 = sizeOrValue;
            (runtimeOrScratch->textureCommon).buildMetadata.timestamps.timeValue2 = sizeOrValue;
            (*g_LocaleCopyDefaultComputerLabelUtf16)
                      ((runtimeOrScratch->textureCommon).buildMetadata.names.producerName);
            (*g_LocaleCopyDefaultComputerLabelUtf16)
                      ((runtimeOrScratch->textureCommon).buildMetadata.names.sourceName);
            runtimeOrScratch->reserved100_1FF[0] = 0;
            runtimeOrScratch->subresourceTableOffset = 0x200;
            runtimeOrScratch->paletteBankCount = 0;
            runtimeOrScratch->subresourceCount = 1;
            runtimeOrScratch->fileHeader = (MovieFileHeader *)flmBufferOrError;
            runtimeOrScratch->currentFrameIndex = 0;
            runtimeOrScratch->videoStreamOffset = 0x200;
            (runtimeOrScratch->sourceEntry).dataOffset = 0x220;
            (runtimeOrScratch->sourceEntry).pixelWidth = frameWidth;
            (runtimeOrScratch->sourceEntry).pixelHeight = frameHeight;
            (runtimeOrScratch->sourceEntry).logicalWidth = frameWidth;
            (runtimeOrScratch->sourceEntry).logicalHeight = frameHeight;
            (runtimeOrScratch->sourceEntry).paletteIndex = -1;
            (runtimeOrScratch->sourceEntry).originX = 0;
            (runtimeOrScratch->sourceEntry).originY = 0;
            runtimeOrScratch->remainingVideoBytes = remainingByteCount;
            runtimeOrScratch->streamHandle = savedStreamHandle;
            runtimeOrScratch->loadedVideoEnd = loadedEnd;
            defaultAudioGain = g_MovieDefaultAudioGainQ15;
            runtimeOrScratch->streamHandleIsSharedPackage = isSharedPackageHandle;
            runtimeOrScratch->openFlags = movieOpenFlags;
            runtimeOrScratch->streamFileOffset = (MovieStreamFileOffset)streamPosition;
            runtimeOrScratch->audioGainQ15 = defaultAudioGain;
            lpThreadId = &remainingByteCount;
            runtimeOrScratch->workerActive = 0;
            runtimeOrScratch->streamState = MOVIE_STREAM_IDLE;
            runtimeOrScratch->refillSemaphore = (void *)0x0;
            if ((remainingByteCount != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
              runtimeOrScratch->workerActive = runtimeOrScratch->workerActive + 1;
              dwCreationFlags = 0;
              lpParameter = (LPVOID)0x0;
              lpStartAddress = (LPTHREAD_START_ROUTINE)Movie_StreamWorkerThread;
              dwStackSize = 0;
              lpThreadAttributes = (LPSECURITY_ATTRIBUTES)0x0;
              semaphoreOrThread = CreateSemaphoreA((LPSECURITY_ATTRIBUTES)0x0,0,1,(LPCSTR)0x0);
              runtimeOrScratch->refillSemaphore = semaphoreOrThread;
              semaphoreOrThread = CreateThread(lpThreadAttributes,dwStackSize,lpStartAddress,lpParameter,
                                    dwCreationFlags,lpThreadId);
              if (semaphoreOrThread == (HANDLE)0x0) {
                runtimeOrScratch->workerActive = runtimeOrScratch->workerActive - 1;
              }
              else {
                CloseHandle(semaphoreOrThread);
              }
            }
            successResult.carry = false;
            successResult.eax = flmBufferOrError->subresourceTableOffset;
            successResult.playbackRateHzEcx = *(dword *)((byte *)flmBufferOrError + 0xfc); /* MOV ECX,[ESI+0xFC] */
            return successResult;
          }
        }
        else {
          if (1 < audioTrackCount) {
            sizeOrValue = Random_NextPrimary();
            tracksToSkip = (sizeOrValue & 0xffff) % audioTrackCount;
          }
          copyCountOrTrackOffset = 0;
          runtimeOrScratch = flmBufferOrError;
          for (; tracksToSkip != 0; tracksToSkip = tracksToSkip - 1) {
            copyCountOrTrackOffset = copyCountOrTrackOffset + runtimeOrScratch->currentFrameIndex;
            runtimeOrScratch = (MovieRuntime *)&(runtimeOrScratch->textureCommon).allocationSizeBytes;
          }
          sizeOrValue = runtimeOrScratch->currentFrameIndex;
          if (sizeOrValue == 0) goto LAB_004a87c0;
          seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_CURRENT,copyCountOrTrackOffset + remainingByteCount,handle);
          runtimeOrScratch = (MovieRuntime *)seekResult.eax;
          if (!seekResult.carry) {
            allocResult = (*g_MemoryApi.alloc)(sizeOrValue);
            runtimeOrScratch = (MovieRuntime *)allocResult.eax;
            if (allocResult.carry) goto Movie_OpenReleaseHeaderAllocationAfterFailure;
            readResult = (*g_FileSystemReadExactCf)(sizeOrValue,runtimeOrScratch,handle);
            voiceSetOrError = (MovieRuntime *)readResult.eax;
            if (readResult.carry) {
LAB_004a89e8:
              (*g_MemoryApi.free)(runtimeOrScratch);
              runtimeOrScratch = voiceSetOrError;
              goto Movie_OpenReleaseHeaderAllocationAfterFailure;
            }
            voiceSetResult = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)runtimeOrScratch);
            voiceSetOrError = (MovieRuntime *)voiceSetResult.eax;
            if (voiceSetResult.carry) goto LAB_004a89e8;
            (*g_MemoryApi.free)(runtimeOrScratch);
            goto Movie_OpenAllocateAndInitializeRuntime;
          }
        }
Movie_OpenReleaseHeaderAllocationAfterFailure:
        flmBufferOrError = runtimeOrScratch;
        (*g_MemoryApi.free)(headerAllocation);
      }
    }
  }
  if (isSharedPackageHandle == 0) {
    (*g_FileSystemClose)(handle);
  }
LAB_004a8a0b:
  failureResult.carry = true;
  failureResult.eax = (dword)flmBufferOrError;
  return failureResult;
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
dword __stdcall Movie_StreamWorkerThread(void *unusedThreadContext)

{
  void *handle;
  MovieRuntime *movie;
  uint byteCount;
  FileSystemReadEaxCf5 readResult;
  
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
      movie = g_ActiveMovie;
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
      readResult = (*g_FileSystemReadExactCf)(byteCount,movie->loadedVideoEnd,handle);
      if (readResult.carry) {
        if (movie->streamState != MOVIE_STREAM_SHUTDOWN) {
          movie->streamState = MOVIE_STREAM_READ_FAILED;
        }
        goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
      }
      movie->remainingVideoBytes = movie->remainingVideoBytes - byteCount;
      movie->streamFileOffset = movie->streamFileOffset + byteCount;
      movie->loadedVideoEnd = movie->loadedVideoEnd + byteCount;
      if ((movie->remainingVideoBytes == 0) && (movie->streamHandleIsSharedPackage == 0)) {
        (*g_FileSystemClose)(handle);
      }
    }
    if ((movie->streamState == MOVIE_STREAM_SHUTDOWN) || (movie->remainingVideoBytes == 0))
    goto Movie_StreamWorkerThread_ClearWorkerActiveAndReturn;
    movie->streamState = MOVIE_STREAM_IDLE;
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
  MovieRuntime *movie;
  HANDLE currentProcessHandle;
  HANDLE hProcess;
  
  movie = g_ActiveMovie;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_MemoryApi.alloc == ArenaHeap_Alloc) {
      g_ActiveMovie->streamState = MOVIE_STREAM_SHUTDOWN;
      currentProcessHandle = GetCurrentProcess();
      SetPriorityClass(currentProcessHandle,0x20);
      do {
      } while (movie->workerActive != 0);
      if (movie->refillSemaphore != (void *)0x0) {
        CloseHandle(movie->refillSemaphore);
        movie->refillSemaphore = (void *)0x0;
      }
      hProcess = GetCurrentProcess();
      SetPriorityClass(hProcess,0x100);
    }
    g_ActiveMovie = (MovieRuntime *)0x0;
    (*g_MemoryApi.free)(movie->fileHeader);
    if (movie->audioVoiceSet != (DirectSoundVoiceSet *)0x0) {
      (*g_SoundReleaseSampleVoiceSet)(movie->audioVoiceSet);
    }
    if ((movie->remainingVideoBytes != 0) && (movie->streamHandleIsSharedPackage == 0)) {
      (*g_FileSystemClose)(movie->streamHandle);
    }
    (*g_MemoryApi.free)(movie);
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
  /* Rewritten from the assembly (0x00565810-0x00565A29). The decompiled version jumped to the
     continuation labels inside the original machine code. EBX is the end-movie runtime. */
  UiCommandDispatchRecord *record = g_EndMovieCommandDispatchRecords_00_Code00000071_Modifier30;
  dword target = 0;

  for (;; record++) {
    uint flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & 0x3c) != 0) continue;
    }
    else if ((flags & 0x30) == 0) {
      if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) != 0)) continue;
    }
    else if ((flags & 0xc) == 0) {
      if (((modifierFlags & 0xc) != 0) || ((modifierFlags & 0x30) == 0)) continue;
    }
    else {
      if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0)) continue;
    }
    target = (dword)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5658f0: { /* screenshot */
    GraphicsFramebufferCaptureEaxCf5 capture =
         (*g_GraphicsFramebufferCaptureRegion)(g_FramebufferHeight,g_FramebufferWidth,0,0);
    PcxEncodeEaxEcxCf9 pcx;
    word *digitHigh = (word *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xc);
    word *digitLow = (word *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xe);
    if (capture.carry) {
      break;
    }
    pcx = (*g_PcxFunctionExport3)(g_PcxFunctionModule,capture.eax);
    if (pcx.carry) {
      (*g_MemoryApi.free)(capture.eax);
      break;
    }
    FileSystem_WriteBufferToPathCf(pcx.encodedByteCount,pcx.encodedBytesOrError,
                                   (word *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0));
    (*g_MemoryApi.free)(pcx.encodedBytesOrError);
    (*g_MemoryApi.free)(capture.eax);
    *digitLow = *digitLow + 1;
    if (*digitLow > 0x39) {
      *digitHigh = *digitHigh + 1;
      *digitLow = *digitLow - 10;
      if (*digitHigh > 0x39) {
        *digitHigh = *digitHigh - 10;
      }
    }
    break;
  }
  case 0x565990: /* skip the end movie */
    if (((*(dword *)((byte *)endMovieRuntime + 0x6ec) & 8) != 0) ||
        ((g_UiCommandRuntimeFlags & 0x800) != 0)) {
      break;
    }
    if ((g_SessionNetworkRoleFlags & 1) != 0) {
      if ((g_SessionNetworkRoleFlags & 3) != 0) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x470,0,0,0);
      }
      else {
        FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(g_LocalPlayerRuntimeId);
      }
    }
    else if ((g_SessionNetworkRoleFlags & 3) != 0) {
      InGameCommandQueue_AppendLocalPlayerCommand(0x310,0,0x1000,0);
    }
    else {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,0x1000,0);
    }
    break;
  default:
    Thandor_Log("EndMovie dispatch: unhandled continuation %08x",target);
    break;
  }
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
  PackedRgb24 pixel1;
  PackedRgb24 pixel2;
  PackedRgb24 pixel3;
  uint maxLumaOrLevel;
  uint minLumaOrBaseLuma;
  uint sampleLumaOrLevel;
  uint minLumaChromaOrLevel;
  int level0;
  int level1;
  int level2;
  int level3;
  int level4;
  int level5;
  int level6;
  int level7;
  int level8;
  uint wideLevel0;
  uint wideLevel1;
  uint wideLevel2;
  uint wideLevel3;
  uint wideLevel4;
  uint wideLevel5;
  PackedRgb24 *blockRowPixels;
  uint *outputCursor;
  undefined1 pixel0Byte3Or1;
  undefined1 pixel0Byte2;
  undefined1 pixel1Byte3Or1;
  undefined1 pixel1Byte2;
  undefined1 pixel2Byte3Or1;
  undefined1 pixel2Byte2;
  undefined1 pixel3Byte3Or1;
  undefined1 pixel3Byte2;
  ushort pair0OrAverage0;
  ushort pair1OrAverage1;
  PackedRgb24 pixel0OrAverageColor;
  ushort pair2OrAverage2;
  undefined8 channelSums;
  ushort pair3OrAverage3;
  uint blocksLeftInRow;
  uint blockRowsLeft;
  
  blockRowsLeft = frameHeightPixels >> 2;
  outputCursor = encodedOutput;
  blocksLeftInRow = frameWidthPixels >> 2;
  do {
    do {
      pixel0OrAverageColor = *sourcePixels;
      pixel1 = sourcePixels[1];
      pixel2 = sourcePixels[2];
      pixel3 = sourcePixels[3];
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
      pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
      pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
      pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
      pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
      pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
      pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
      pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
      pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
      channelSums = CONCAT26((pair3OrAverage3 >> 6) + (pair2OrAverage2 >> 6) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6),
                        CONCAT24(((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),CONCAT14(pixel0Byte2,pixel0OrAverageColor))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1))
                                          >> 0x20) >> 6),
                                 CONCAT22((CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6) +
                                          (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                          (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                          (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6),
                                          (CONCAT11((char)pixel3,(char)pixel3) >> 6) +
                                          (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                          (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                          (CONCAT11((char)pixel1,(char)pixel1) >> 6))));
      maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*sourcePixels);
      minLumaOrBaseLuma = MovieColor_ComputeLuma5FromRgb888(sourcePixels[1]);
      minLumaChromaOrLevel = minLumaOrBaseLuma;
      if (((int)maxLumaOrLevel <= (int)minLumaOrBaseLuma) && (minLumaChromaOrLevel = maxLumaOrLevel, (int)maxLumaOrLevel < (int)minLumaOrBaseLuma)) {
        maxLumaOrLevel = minLumaOrBaseLuma;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(sourcePixels[2]);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(sourcePixels[3]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      blockRowPixels = sourcePixels + frameWidthPixels;
      pixel0OrAverageColor = *blockRowPixels;
      pixel1 = blockRowPixels[1];
      pixel2 = blockRowPixels[2];
      pixel3 = blockRowPixels[3];
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
      pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
      pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
      pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
      pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
      pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
      pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
      pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
      pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
      channelSums = CONCAT26((short)((ulonglong)channelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                        (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                        CONCAT24((short)((ulonglong)channelSums >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),CONCAT14(pixel0Byte2,pixel0OrAverageColor))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)channelSums >> 0x10) +
                                          (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                          (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                          (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                          (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                          (short)channelSums +
                                          (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                          (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                          (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                          (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      blockRowPixels = blockRowPixels + frameWidthPixels;
      pixel0OrAverageColor = *blockRowPixels;
      pixel1 = blockRowPixels[1];
      pixel2 = blockRowPixels[2];
      pixel3 = blockRowPixels[3];
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
      pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
      pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
      pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
      pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
      pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
      pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
      pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
      pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
      channelSums = CONCAT26((short)((ulonglong)channelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                        (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                        CONCAT24((short)((ulonglong)channelSums >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),CONCAT14(pixel0Byte2,pixel0OrAverageColor))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)channelSums >> 0x10) +
                                          (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                          (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                          (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                          (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                          (short)channelSums +
                                          (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                          (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                          (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                          (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      blockRowPixels = blockRowPixels + frameWidthPixels;
      pixel0OrAverageColor = *blockRowPixels;
      pixel1 = blockRowPixels[1];
      pixel2 = blockRowPixels[2];
      pixel3 = blockRowPixels[3];
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
      pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
      pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
      pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
      pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
      pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
      pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
      pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
      pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
      pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
      pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
      pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
      pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
      channelSums = CONCAT26((short)((ulonglong)channelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                        (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                        CONCAT24((short)((ulonglong)channelSums >> 0x20) +
                                 ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),CONCAT14(pixel0Byte2,pixel0OrAverageColor))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2))
                                          >> 0x20) >> 6) +
                                 ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3))
                                          >> 0x20) >> 6),
                                 CONCAT22((short)((ulonglong)channelSums >> 0x10) +
                                          (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                          (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                          (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                          (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                          (short)channelSums +
                                          (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                          (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                          (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                          (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
      minLumaOrBaseLuma = sampleLumaOrLevel;
      if (((int)minLumaChromaOrLevel <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaChromaOrLevel, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
      minLumaChromaOrLevel = sampleLumaOrLevel;
      if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaChromaOrLevel = minLumaOrBaseLuma, (int)maxLumaOrLevel < (int)sampleLumaOrLevel)) {
        maxLumaOrLevel = sampleLumaOrLevel;
      }
      pair0OrAverage0 = (ushort)channelSums >> 6;
      pair1OrAverage1 = (ushort)((ulonglong)channelSums >> 0x10) >> 6;
      pair2OrAverage2 = (ushort)((ulonglong)channelSums >> 0x20) >> 6;
      pair3OrAverage3 = (ushort)((ulonglong)channelSums >> 0x36);
      pixel0OrAverageColor = CONCAT13((pair3OrAverage3 != 0) * (pair3OrAverage3 < 0x100) * (char)pair3OrAverage3 - (0xff < pair3OrAverage3),
                        CONCAT12((pair2OrAverage2 != 0) * (pair2OrAverage2 < 0x100) * (char)pair2OrAverage2 - (0xff < pair2OrAverage2),
                                 CONCAT11((pair1OrAverage1 != 0) * (pair1OrAverage1 < 0x100) * (char)pair1OrAverage1 -
                                          (0xff < pair1OrAverage1),
                                          (pair0OrAverage0 != 0) * (pair0OrAverage0 < 0x100) * (char)pair0OrAverage0 -
                                          (0xff < pair0OrAverage0))));
      minLumaOrBaseLuma = (int)((minLumaChromaOrLevel - 8) + maxLumaOrLevel) >> 1;
      if ((int)minLumaOrBaseLuma < 0) {
        minLumaOrBaseLuma = 0;
      }
      else if (0x18 < (int)minLumaOrBaseLuma) {
        minLumaOrBaseLuma = 0x18;
      }
      if (maxLumaOrLevel - minLumaChromaOrLevel < 0xc) {
        *outputCursor = minLumaOrBaseLuma;
        minLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(pixel0OrAverageColor);
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        level0 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level0 < 0) {
          level0 = 0;
        }
        else if (7 < level0) {
          level0 = 7;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        level1 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level1 < 0) {
          level1 = 0;
        }
        else if (7 < level1) {
          level1 = 7;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        level2 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level2 < 0) {
          level2 = 0;
        }
        else if (7 < level2) {
          level2 = 7;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        level3 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level3 < 0) {
          level3 = 0;
        }
        else if (7 < level3) {
          level3 = 7;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        level4 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level4 < 0) {
          level4 = 0;
        }
        else if (7 < level4) {
          level4 = 7;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        level5 = maxLumaOrLevel - minLumaOrBaseLuma;
        if (level5 < 0) {
          level5 = 0;
        }
        else if (7 < level5) {
          level5 = 7;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        maxLumaOrLevel = maxLumaOrLevel - minLumaOrBaseLuma;
        if ((int)maxLumaOrLevel < 0) {
          maxLumaOrLevel = 0;
        }
        else if (7 < (int)maxLumaOrLevel) {
          maxLumaOrLevel = 7;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        outputCursor[1] = (minLumaChromaOrLevel & 0x7fe0) << 0x10 | level0 << 0x12 | level1 << 0xf | level2 << 0xc |
                     level3 << 9 | level4 << 6 | level5 << 3 | maxLumaOrLevel;
        level0 = sampleLumaOrLevel - minLumaOrBaseLuma;
        if (level0 < 0) {
          level0 = 0;
        }
        else if (7 < level0) {
          level0 = 7;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        level1 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level1 < 0) {
          level1 = 0;
        }
        else if (7 < level1) {
          level1 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        level2 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level2 < 0) {
          level2 = 0;
        }
        else if (7 < level2) {
          level2 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        level3 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level3 < 0) {
          level3 = 0;
        }
        else if (7 < level3) {
          level3 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        level4 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level4 < 0) {
          level4 = 0;
        }
        else if (7 < level4) {
          level4 = 7;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        level5 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level5 < 0) {
          level5 = 0;
        }
        else if (7 < level5) {
          level5 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        level6 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level6 < 0) {
          level6 = 0;
        }
        else if (7 < level6) {
          level6 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        level7 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level7 < 0) {
          level7 = 0;
        }
        else if (7 < level7) {
          level7 = 7;
        }
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        level8 = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if (level8 < 0) {
          level8 = 0;
        }
        else if (7 < level8) {
          level8 = 7;
        }
        *outputCursor = *outputCursor |
                   level0 << 0x1d | level1 << 0x1a | level2 << 0x17 | level3 << 0x14 | level4 << 0x11
                   | level5 << 0xe | level6 << 0xb | level7 << 8 | level8 << 5;
      }
      else {
        minLumaOrBaseLuma = minLumaOrBaseLuma - 4;
        if ((int)minLumaOrBaseLuma < 0) {
          minLumaOrBaseLuma = 0;
        }
        else if (0x10 < (int)minLumaOrBaseLuma) {
          minLumaOrBaseLuma = 0x10;
        }
        *outputCursor = minLumaOrBaseLuma;
        minLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(pixel0OrAverageColor);
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        maxLumaOrLevel = maxLumaOrLevel - minLumaOrBaseLuma;
        if ((int)maxLumaOrLevel < 0) {
          maxLumaOrLevel = 0;
        }
        else if (0xf < (int)maxLumaOrLevel) {
          maxLumaOrLevel = 0xf;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        sampleLumaOrLevel = sampleLumaOrLevel - minLumaOrBaseLuma;
        if ((int)sampleLumaOrLevel < 0) {
          sampleLumaOrLevel = 0;
        }
        else if (0xf < (int)sampleLumaOrLevel) {
          sampleLumaOrLevel = 0xf;
        }
        wideLevel0 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        wideLevel0 = wideLevel0 - minLumaOrBaseLuma;
        if ((int)wideLevel0 < 0) {
          wideLevel0 = 0;
        }
        else if (0xf < (int)wideLevel0) {
          wideLevel0 = 0xf;
        }
        wideLevel1 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        wideLevel1 = wideLevel1 - minLumaOrBaseLuma;
        if ((int)wideLevel1 < 0) {
          wideLevel1 = 0;
        }
        else if (0xf < (int)wideLevel1) {
          wideLevel1 = 0xf;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        wideLevel2 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        wideLevel2 = wideLevel2 - minLumaOrBaseLuma;
        if ((int)wideLevel2 < 0) {
          wideLevel2 = 0;
        }
        else if (0xf < (int)wideLevel2) {
          wideLevel2 = 0xf;
        }
        wideLevel3 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        wideLevel3 = wideLevel3 - minLumaOrBaseLuma;
        if ((int)wideLevel3 < 0) {
          wideLevel3 = 0;
        }
        else if (0xf < (int)wideLevel3) {
          wideLevel3 = 0xf;
        }
        wideLevel4 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        wideLevel4 = wideLevel4 - minLumaOrBaseLuma;
        if ((int)wideLevel4 < 0) {
          wideLevel4 = 0;
        }
        else if (0xf < (int)wideLevel4) {
          wideLevel4 = 0xf;
        }
        wideLevel5 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        outputCursor[1] = (minLumaChromaOrLevel & 0x7fe0) * 0x10000 + 0x80000000 | (maxLumaOrLevel >> 1) << 0x12 |
                     (sampleLumaOrLevel >> 1) << 0xf | (wideLevel0 >> 1) << 0xc | (wideLevel1 >> 1) << 9 |
                     (wideLevel2 >> 1) << 6 | (wideLevel3 >> 1) << 3 | wideLevel4 >> 1;
        wideLevel5 = wideLevel5 - minLumaOrBaseLuma;
        if ((int)wideLevel5 < 0) {
          wideLevel5 = 0;
        }
        else if (0xf < (int)wideLevel5) {
          wideLevel5 = 0xf;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        minLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        minLumaChromaOrLevel = minLumaChromaOrLevel - minLumaOrBaseLuma;
        if ((int)minLumaChromaOrLevel < 0) {
          minLumaChromaOrLevel = 0;
        }
        else if (0xf < (int)minLumaChromaOrLevel) {
          minLumaChromaOrLevel = 0xf;
        }
        maxLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        maxLumaOrLevel = maxLumaOrLevel - minLumaOrBaseLuma;
        if ((int)maxLumaOrLevel < 0) {
          maxLumaOrLevel = 0;
        }
        else if (0xf < (int)maxLumaOrLevel) {
          maxLumaOrLevel = 0xf;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        sampleLumaOrLevel = sampleLumaOrLevel - minLumaOrBaseLuma;
        if ((int)sampleLumaOrLevel < 0) {
          sampleLumaOrLevel = 0;
        }
        else if (0xf < (int)sampleLumaOrLevel) {
          sampleLumaOrLevel = 0xf;
        }
        wideLevel0 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        wideLevel0 = wideLevel0 - minLumaOrBaseLuma;
        if ((int)wideLevel0 < 0) {
          wideLevel0 = 0;
        }
        else if (0xf < (int)wideLevel0) {
          wideLevel0 = 0xf;
        }
        blockRowPixels = blockRowPixels + -frameWidthPixels;
        wideLevel1 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        wideLevel1 = wideLevel1 - minLumaOrBaseLuma;
        if ((int)wideLevel1 < 0) {
          wideLevel1 = 0;
        }
        else if (0xf < (int)wideLevel1) {
          wideLevel1 = 0xf;
        }
        wideLevel2 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        wideLevel2 = wideLevel2 - minLumaOrBaseLuma;
        if ((int)wideLevel2 < 0) {
          wideLevel2 = 0;
        }
        else if (0xf < (int)wideLevel2) {
          wideLevel2 = 0xf;
        }
        wideLevel3 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        wideLevel3 = wideLevel3 - minLumaOrBaseLuma;
        if ((int)wideLevel3 < 0) {
          wideLevel3 = 0;
        }
        else if (0xf < (int)wideLevel3) {
          wideLevel3 = 0xf;
        }
        wideLevel4 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        wideLevel4 = wideLevel4 - minLumaOrBaseLuma;
        if ((int)wideLevel4 < 0) {
          wideLevel4 = 0;
        }
        else if (0xf < (int)wideLevel4) {
          wideLevel4 = 0xf;
        }
        *outputCursor = *outputCursor |
                   (wideLevel5 >> 1) << 0x1d | (minLumaChromaOrLevel >> 1) << 0x1a | (maxLumaOrLevel >> 1) << 0x17 |
                   (sampleLumaOrLevel >> 1) << 0x14 | (wideLevel0 >> 1) << 0x11 | (wideLevel1 >> 1) << 0xe |
                   (wideLevel2 >> 1) << 0xb | (wideLevel3 >> 1) << 8 | (wideLevel4 >> 1) << 5;
      }
      sourcePixels = blockRowPixels + 4;
      outputCursor = outputCursor + 2;
      blocksLeftInRow = blocksLeftInRow - 1;
    } while (blocksLeftInRow != 0);
    sourcePixels = sourcePixels + frameWidthPixels * 3;
    blockRowsLeft = blockRowsLeft - 1;
    blocksLeftInRow = frameWidthPixels >> 2;
  } while (blockRowsLeft != 0);
  return (int)outputCursor - (int)encodedOutput;
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
  int rowStrideBytes;
  undefined8 copiedQword;
  undefined4 pixel0Raw;
  undefined4 pixel1Raw;
  undefined4 pixel2Raw;
  undefined4 pixel3Raw;
  PackedRgb24 pixel1;
  PackedRgb24 pixel2;
  PackedRgb24 pixel3;
  ulonglong *previousBlock;
  uint maxLumaChromaOrLevel;
  uint minLumaOrBaseLuma;
  uint sampleLumaOrLevel;
  uint minLumaOrSkipCount;
  int level0;
  int level1;
  int level2;
  int level3;
  int level4;
  int level5;
  uint wideLevel0;
  int level6;
  int level7;
  int level8;
  uint wideLevel1;
  uint wideLevel2;
  uint wideLevel3;
  uint wideLevel4;
  uint wideLevel5;
  uint wideLevel6;
  PackedRgb24 *blockRowPixels;
  ulonglong *currentBlockCursor;
  uint *outputCursor;
  undefined1 pixel0Byte3Or1;
  undefined1 pixel0Byte2;
  ulonglong changedBitsOrQword;
  undefined1 pixel1Byte3Or1;
  undefined1 pixel1Byte2;
  undefined1 pixel2Byte3Or1;
  undefined1 pixel2Byte2;
  undefined1 pixel3Byte3Or1;
  undefined1 pixel3Byte2;
  ushort pair0OrAverage0;
  ushort pair1OrAverage1;
  PackedRgb24 pixel0OrAverageColor;
  ushort pair2OrAverage2;
  undefined8 copiedQwordOrChannelSums;
  ushort pair3OrAverage3;
  uint blocksLeftInRow;
  uint blockRowsLeft;
  uint pendingSkipCount;
  
  rowStrideBytes = frameWidthPixels * 4;
  blockRowsLeft = frameHeightPixels >> 2;
  pendingSkipCount = 0;
  currentBlockCursor = (ulonglong *)currentFramePixels;
  outputCursor = encodedOutput;
  blocksLeftInRow = frameWidthPixels >> 2;
  do {
    do {
      previousBlock = (ulonglong *)(((int)previousFramePixels + (int)currentBlockCursor) - (int)currentFramePixels);
      changedBitsOrQword = g_MovieDeltaRgbHighNibbleMask2Pixels &
               (*currentBlockCursor & g_MovieDeltaRgbHighNibbleMask2Pixels ^ *previousBlock |
                currentBlockCursor[1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^ previousBlock[1] |
                *(ulonglong *)(rowStrideBytes + (int)currentBlockCursor) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(ulonglong *)((int)previousBlock + rowStrideBytes) |
                *(ulonglong *)(rowStrideBytes + 8 + (int)currentBlockCursor) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(ulonglong *)((int)previousBlock + rowStrideBytes + 8) |
                currentBlockCursor[frameWidthPixels] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                previousBlock[frameWidthPixels] |
                currentBlockCursor[frameWidthPixels + 1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                previousBlock[frameWidthPixels + 1] |
               *(ulonglong *)((int)currentBlockCursor + frameWidthPixels * 0xc) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(ulonglong *)((int)previousBlock + frameWidthPixels * 0xc) |
               *(ulonglong *)((int)currentBlockCursor + frameWidthPixels * 0xc + 8) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(ulonglong *)((int)previousBlock + frameWidthPixels * 0xc + 8));
      minLumaOrSkipCount = pendingSkipCount + 1;
      if ((int)(changedBitsOrQword >> 0x20) != 0 || (int)changedBitsOrQword != 0) {
        if (pendingSkipCount != 0) {
          if (pendingSkipCount < 9) {
            *(byte *)outputCursor = ((char)pendingSkipCount + -1) * ' ' | 0x19;
            outputCursor = (uint *)((int)outputCursor + 1);
          }
          else if (pendingSkipCount < 0x809) {
            *(ushort *)outputCursor = ((short)pendingSkipCount + -9) * 0x20 | 0x1a;
            outputCursor = (uint *)((int)outputCursor + 2);
          }
          else {
            *outputCursor = (pendingSkipCount - 0x809) * 0x20 | 0x1b;
            outputCursor = outputCursor + 1;
          }
          pendingSkipCount = 0;
        }
        previousBlock = (ulonglong *)(((int)previousFramePixels + (int)currentBlockCursor) - (int)currentFramePixels)
        ;
        changedBitsOrQword = currentBlockCursor[1];
        copiedQwordOrChannelSums = *(undefined8 *)((int)currentBlockCursor + frameWidthPixels * 4);
        copiedQword = *(undefined8 *)((int)currentBlockCursor + (frameWidthPixels + 2) * 4);
        *previousBlock = *currentBlockCursor;
        previousBlock[1] = changedBitsOrQword;
        *(undefined8 *)((int)previousBlock + rowStrideBytes) = copiedQwordOrChannelSums;
        *(undefined8 *)((int)previousBlock + rowStrideBytes + 8) = copiedQword;
        changedBitsOrQword = currentBlockCursor[frameWidthPixels + 1];
        copiedQwordOrChannelSums = *(undefined8 *)((int)currentBlockCursor + frameWidthPixels * 0xc);
        copiedQword = *(undefined8 *)((int)currentBlockCursor + (frameWidthPixels * 3 + 2) * 4);
        previousBlock[frameWidthPixels] = currentBlockCursor[frameWidthPixels];
        previousBlock[frameWidthPixels + 1] = changedBitsOrQword;
        *(undefined8 *)((int)previousBlock + frameWidthPixels * 0xc) = copiedQwordOrChannelSums;
        *(undefined8 *)((int)previousBlock + frameWidthPixels * 0xc + 8) = copiedQword;
        pixel0Raw = (undefined4)*currentBlockCursor;
        pixel1Raw = *(undefined4 *)((int)currentBlockCursor + 4);
        pixel2Raw = (undefined4)currentBlockCursor[1];
        pixel3Raw = *(undefined4 *)((int)currentBlockCursor + 0xc);
        pixel0Byte3Or1 = (undefined1)((uint)pixel0Raw >> 0x18);
        pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
        pixel0Byte2 = (undefined1)((uint)pixel0Raw >> 0x10);
        pixel0Byte3Or1 = (undefined1)((uint)pixel0Raw >> 8);
        pixel1Byte3Or1 = (undefined1)((uint)pixel1Raw >> 0x18);
        pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
        pixel1Byte2 = (undefined1)((uint)pixel1Raw >> 0x10);
        pixel1Byte3Or1 = (undefined1)((uint)pixel1Raw >> 8);
        pixel2Byte3Or1 = (undefined1)((uint)pixel2Raw >> 0x18);
        pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
        pixel2Byte2 = (undefined1)((uint)pixel2Raw >> 0x10);
        pixel2Byte3Or1 = (undefined1)((uint)pixel2Raw >> 8);
        pixel3Byte3Or1 = (undefined1)((uint)pixel3Raw >> 0x18);
        pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
        pixel3Byte2 = (undefined1)((uint)pixel3Raw >> 0x10);
        pixel3Byte3Or1 = (undefined1)((uint)pixel3Raw >> 8);
        copiedQwordOrChannelSums = CONCAT26((pair3OrAverage3 >> 6) + (pair2OrAverage2 >> 6) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6),
                          CONCAT24(((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3Raw)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2Raw)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),CONCAT14(pixel0Byte2,pixel0Raw)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1Raw)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6) +
                                            (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                            (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                            (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6),
                                            (CONCAT11((char)pixel3Raw,(char)pixel3Raw) >> 6) +
                                            (CONCAT11((char)pixel2Raw,(char)pixel2Raw) >> 6) +
                                            (CONCAT11((char)pixel0Raw,(char)pixel0Raw) >> 6) +
                                            (CONCAT11((char)pixel1Raw,(char)pixel1Raw) >> 6))));
        maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*currentBlockCursor);
        minLumaOrBaseLuma = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 4));
        minLumaOrSkipCount = minLumaOrBaseLuma;
        if (((int)maxLumaChromaOrLevel <= (int)minLumaOrBaseLuma) && (minLumaOrSkipCount = maxLumaChromaOrLevel, (int)maxLumaChromaOrLevel < (int)minLumaOrBaseLuma)) {
          maxLumaChromaOrLevel = minLumaOrBaseLuma;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)currentBlockCursor[1]);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 0xc));
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        blockRowPixels = (PackedRgb24 *)((int)currentBlockCursor + frameWidthPixels * 4);
        pixel0OrAverageColor = *blockRowPixels;
        pixel1 = blockRowPixels[1];
        pixel2 = blockRowPixels[2];
        pixel3 = blockRowPixels[3];
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
        pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
        pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
        pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
        pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
        pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
        pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
        pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
        pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
        copiedQwordOrChannelSums = CONCAT26((short)((ulonglong)copiedQwordOrChannelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                          (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                          CONCAT24((short)((ulonglong)copiedQwordOrChannelSums >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),
                                                      CONCAT14(pixel0Byte2,pixel0OrAverageColor)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)copiedQwordOrChannelSums >> 0x10) +
                                            (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                            (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                            (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                            (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                            (short)copiedQwordOrChannelSums +
                                            (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                            (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                            (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                            (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        blockRowPixels = blockRowPixels + frameWidthPixels;
        pixel0OrAverageColor = *blockRowPixels;
        pixel1 = blockRowPixels[1];
        pixel2 = blockRowPixels[2];
        pixel3 = blockRowPixels[3];
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
        pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
        pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
        pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
        pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
        pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
        pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
        pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
        pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
        copiedQwordOrChannelSums = CONCAT26((short)((ulonglong)copiedQwordOrChannelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                          (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                          CONCAT24((short)((ulonglong)copiedQwordOrChannelSums >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),
                                                      CONCAT14(pixel0Byte2,pixel0OrAverageColor)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)copiedQwordOrChannelSums >> 0x10) +
                                            (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                            (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                            (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                            (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                            (short)copiedQwordOrChannelSums +
                                            (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                            (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                            (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                            (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        blockRowPixels = blockRowPixels + frameWidthPixels;
        pixel0OrAverageColor = *blockRowPixels;
        pixel1 = blockRowPixels[1];
        pixel2 = blockRowPixels[2];
        pixel3 = blockRowPixels[3];
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 0x18);
        pair0OrAverage0 = CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1);
        pixel0Byte2 = (undefined1)(pixel0OrAverageColor >> 0x10);
        pixel0Byte3Or1 = (undefined1)(pixel0OrAverageColor >> 8);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 0x18);
        pair1OrAverage1 = CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1);
        pixel1Byte2 = (undefined1)(pixel1 >> 0x10);
        pixel1Byte3Or1 = (undefined1)(pixel1 >> 8);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 0x18);
        pair2OrAverage2 = CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1);
        pixel2Byte2 = (undefined1)(pixel2 >> 0x10);
        pixel2Byte3Or1 = (undefined1)(pixel2 >> 8);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 0x18);
        pair3OrAverage3 = CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1);
        pixel3Byte2 = (undefined1)(pixel3 >> 0x10);
        pixel3Byte3Or1 = (undefined1)(pixel3 >> 8);
        copiedQwordOrChannelSums = CONCAT26((short)((ulonglong)copiedQwordOrChannelSums >> 0x30) + (pair0OrAverage0 >> 6) + (pair1OrAverage1 >> 6) +
                          (pair2OrAverage2 >> 6) + (pair3OrAverage3 >> 6),
                          CONCAT24((short)((ulonglong)copiedQwordOrChannelSums >> 0x20) +
                                   ((ushort)(CONCAT35(CONCAT21(pair0OrAverage0,pixel0Byte2),
                                                      CONCAT14(pixel0Byte2,pixel0OrAverageColor)) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair1OrAverage1,pixel1Byte2),CONCAT14(pixel1Byte2,pixel1)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair2OrAverage2,pixel2Byte2),CONCAT14(pixel2Byte2,pixel2)
                                                     ) >> 0x20) >> 6) +
                                   ((ushort)(CONCAT35(CONCAT21(pair3OrAverage3,pixel3Byte2),CONCAT14(pixel3Byte2,pixel3)
                                                     ) >> 0x20) >> 6),
                                   CONCAT22((short)((ulonglong)copiedQwordOrChannelSums >> 0x10) +
                                            (CONCAT11(pixel0Byte3Or1,pixel0Byte3Or1) >> 6) +
                                            (CONCAT11(pixel1Byte3Or1,pixel1Byte3Or1) >> 6) +
                                            (CONCAT11(pixel2Byte3Or1,pixel2Byte3Or1) >> 6) +
                                            (CONCAT11(pixel3Byte3Or1,pixel3Byte3Or1) >> 6),
                                            (short)copiedQwordOrChannelSums +
                                            (CONCAT11((char)pixel0OrAverageColor,(char)pixel0OrAverageColor) >> 6) +
                                            (CONCAT11((char)pixel1,(char)pixel1) >> 6) +
                                            (CONCAT11((char)pixel2,(char)pixel2) >> 6) +
                                            (CONCAT11((char)pixel3,(char)pixel3) >> 6))));
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
        minLumaOrSkipCount = sampleLumaOrLevel;
        if (((int)minLumaOrBaseLuma <= (int)sampleLumaOrLevel) && (minLumaOrSkipCount = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
        minLumaOrBaseLuma = sampleLumaOrLevel;
        if (((int)minLumaOrSkipCount <= (int)sampleLumaOrLevel) && (minLumaOrBaseLuma = minLumaOrSkipCount, (int)maxLumaChromaOrLevel < (int)sampleLumaOrLevel)) {
          maxLumaChromaOrLevel = sampleLumaOrLevel;
        }
        minLumaOrSkipCount = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
        sampleLumaOrLevel = minLumaOrSkipCount;
        if (((int)minLumaOrBaseLuma <= (int)minLumaOrSkipCount) && (sampleLumaOrLevel = minLumaOrBaseLuma, (int)maxLumaChromaOrLevel < (int)minLumaOrSkipCount)) {
          maxLumaChromaOrLevel = minLumaOrSkipCount;
        }
        pair0OrAverage0 = (ushort)copiedQwordOrChannelSums >> 6;
        pair1OrAverage1 = (ushort)((ulonglong)copiedQwordOrChannelSums >> 0x10) >> 6;
        pair2OrAverage2 = (ushort)((ulonglong)copiedQwordOrChannelSums >> 0x20) >> 6;
        pair3OrAverage3 = (ushort)((ulonglong)copiedQwordOrChannelSums >> 0x36);
        pixel0OrAverageColor = CONCAT13((pair3OrAverage3 != 0) * (pair3OrAverage3 < 0x100) * (char)pair3OrAverage3 - (0xff < pair3OrAverage3),
                          CONCAT12((pair2OrAverage2 != 0) * (pair2OrAverage2 < 0x100) * (char)pair2OrAverage2 - (0xff < pair2OrAverage2)
                                   ,CONCAT11((pair1OrAverage1 != 0) * (pair1OrAverage1 < 0x100) * (char)pair1OrAverage1 -
                                             (0xff < pair1OrAverage1),
                                             (pair0OrAverage0 != 0) * (pair0OrAverage0 < 0x100) * (char)pair0OrAverage0 -
                                             (0xff < pair0OrAverage0))));
        minLumaOrBaseLuma = (int)((sampleLumaOrLevel - 8) + maxLumaChromaOrLevel) >> 1;
        if ((int)minLumaOrBaseLuma < 0) {
          minLumaOrBaseLuma = 0;
        }
        else if (0x18 < minLumaOrBaseLuma) {
          minLumaOrBaseLuma = 0x18;
        }
        minLumaOrSkipCount = pendingSkipCount;
        if (maxLumaChromaOrLevel - sampleLumaOrLevel < 0xc) {
          *outputCursor = minLumaOrBaseLuma;
          maxLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(pixel0OrAverageColor);
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          level0 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level0 < 0) {
            level0 = 0;
          }
          else if (7 < level0) {
            level0 = 7;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          level1 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level1 < 0) {
            level1 = 0;
          }
          else if (7 < level1) {
            level1 = 7;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          level2 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level2 < 0) {
            level2 = 0;
          }
          else if (7 < level2) {
            level2 = 7;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          level3 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level3 < 0) {
            level3 = 0;
          }
          else if (7 < level3) {
            level3 = 7;
          }
          blockRowPixels = blockRowPixels + -frameWidthPixels;
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          level4 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level4 < 0) {
            level4 = 0;
          }
          else if (7 < level4) {
            level4 = 7;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          level5 = sampleLumaOrLevel - minLumaOrBaseLuma;
          if (level5 < 0) {
            level5 = 0;
          }
          else if (7 < level5) {
            level5 = 7;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          sampleLumaOrLevel = sampleLumaOrLevel - minLumaOrBaseLuma;
          if ((int)sampleLumaOrLevel < 0) {
            sampleLumaOrLevel = 0;
          }
          else if (7 < (int)sampleLumaOrLevel) {
            sampleLumaOrLevel = 7;
          }
          wideLevel0 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          outputCursor[1] = (maxLumaChromaOrLevel & 0x7fe0) << 0x10 | level0 << 0x12 | level1 << 0xf | level2 << 0xc |
                       level3 << 9 | level4 << 6 | level5 << 3 | sampleLumaOrLevel;
          level0 = wideLevel0 - minLumaOrBaseLuma;
          if (level0 < 0) {
            level0 = 0;
          }
          else if (7 < level0) {
            level0 = 7;
          }
          blockRowPixels = blockRowPixels + -frameWidthPixels;
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          level1 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level1 < 0) {
            level1 = 0;
          }
          else if (7 < level1) {
            level1 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          level2 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level2 < 0) {
            level2 = 0;
          }
          else if (7 < level2) {
            level2 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          level3 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level3 < 0) {
            level3 = 0;
          }
          else if (7 < level3) {
            level3 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          level4 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level4 < 0) {
            level4 = 0;
          }
          else if (7 < level4) {
            level4 = 7;
          }
          currentBlockCursor = (ulonglong *)(blockRowPixels + -frameWidthPixels);
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 0xc));
          level5 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level5 < 0) {
            level5 = 0;
          }
          else if (7 < level5) {
            level5 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)currentBlockCursor[1]);
          level6 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level6 < 0) {
            level6 = 0;
          }
          else if (7 < level6) {
            level6 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 4));
          level7 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level7 < 0) {
            level7 = 0;
          }
          else if (7 < level7) {
            level7 = 7;
          }
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*currentBlockCursor);
          level8 = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if (level8 < 0) {
            level8 = 0;
          }
          else if (7 < level8) {
            level8 = 7;
          }
          *outputCursor = *outputCursor |
                     level0 << 0x1d | level1 << 0x1a | level2 << 0x17 | level3 << 0x14 |
                     level4 << 0x11 | level5 << 0xe | level6 << 0xb | level7 << 8 | level8 << 5;
          outputCursor = outputCursor + 2;
        }
        else {
          minLumaOrBaseLuma = minLumaOrBaseLuma - 4;
          if ((int)minLumaOrBaseLuma < 0) {
            minLumaOrBaseLuma = 0;
          }
          else if (0x10 < (int)minLumaOrBaseLuma) {
            minLumaOrBaseLuma = 0x10;
          }
          *outputCursor = minLumaOrBaseLuma;
          maxLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(pixel0OrAverageColor);
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          sampleLumaOrLevel = sampleLumaOrLevel - minLumaOrBaseLuma;
          if ((int)sampleLumaOrLevel < 0) {
            sampleLumaOrLevel = 0;
          }
          else if (0xf < (int)sampleLumaOrLevel) {
            sampleLumaOrLevel = 0xf;
          }
          wideLevel0 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          wideLevel0 = wideLevel0 - minLumaOrBaseLuma;
          if ((int)wideLevel0 < 0) {
            wideLevel0 = 0;
          }
          else if (0xf < (int)wideLevel0) {
            wideLevel0 = 0xf;
          }
          wideLevel1 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          wideLevel1 = wideLevel1 - minLumaOrBaseLuma;
          if ((int)wideLevel1 < 0) {
            wideLevel1 = 0;
          }
          else if (0xf < (int)wideLevel1) {
            wideLevel1 = 0xf;
          }
          wideLevel2 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          wideLevel2 = wideLevel2 - minLumaOrBaseLuma;
          if ((int)wideLevel2 < 0) {
            wideLevel2 = 0;
          }
          else if (0xf < (int)wideLevel2) {
            wideLevel2 = 0xf;
          }
          blockRowPixels = blockRowPixels + -frameWidthPixels;
          wideLevel3 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          wideLevel3 = wideLevel3 - minLumaOrBaseLuma;
          if ((int)wideLevel3 < 0) {
            wideLevel3 = 0;
          }
          else if (0xf < (int)wideLevel3) {
            wideLevel3 = 0xf;
          }
          wideLevel4 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          wideLevel4 = wideLevel4 - minLumaOrBaseLuma;
          if ((int)wideLevel4 < 0) {
            wideLevel4 = 0;
          }
          else if (0xf < (int)wideLevel4) {
            wideLevel4 = 0xf;
          }
          wideLevel5 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          wideLevel5 = wideLevel5 - minLumaOrBaseLuma;
          if ((int)wideLevel5 < 0) {
            wideLevel5 = 0;
          }
          else if (0xf < (int)wideLevel5) {
            wideLevel5 = 0xf;
          }
          wideLevel6 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          outputCursor[1] = (maxLumaChromaOrLevel & 0x7fe0) * 0x10000 + 0x80000000 | (sampleLumaOrLevel >> 1) << 0x12 |
                       (wideLevel0 >> 1) << 0xf | (wideLevel1 >> 1) << 0xc | (wideLevel2 >> 1) << 9 |
                       (wideLevel3 >> 1) << 6 | (wideLevel4 >> 1) << 3 | wideLevel5 >> 1;
          wideLevel6 = wideLevel6 - minLumaOrBaseLuma;
          if ((int)wideLevel6 < 0) {
            wideLevel6 = 0;
          }
          else if (0xf < (int)wideLevel6) {
            wideLevel6 = 0xf;
          }
          blockRowPixels = blockRowPixels + -frameWidthPixels;
          maxLumaChromaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[3]);
          maxLumaChromaOrLevel = maxLumaChromaOrLevel - minLumaOrBaseLuma;
          if ((int)maxLumaChromaOrLevel < 0) {
            maxLumaChromaOrLevel = 0;
          }
          else if (0xf < (int)maxLumaChromaOrLevel) {
            maxLumaChromaOrLevel = 0xf;
          }
          sampleLumaOrLevel = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[2]);
          sampleLumaOrLevel = sampleLumaOrLevel - minLumaOrBaseLuma;
          if ((int)sampleLumaOrLevel < 0) {
            sampleLumaOrLevel = 0;
          }
          else if (0xf < (int)sampleLumaOrLevel) {
            sampleLumaOrLevel = 0xf;
          }
          wideLevel0 = MovieColor_ComputeLuma5FromRgb888(blockRowPixels[1]);
          wideLevel0 = wideLevel0 - minLumaOrBaseLuma;
          if ((int)wideLevel0 < 0) {
            wideLevel0 = 0;
          }
          else if (0xf < (int)wideLevel0) {
            wideLevel0 = 0xf;
          }
          wideLevel1 = MovieColor_ComputeLuma5FromRgb888(*blockRowPixels);
          wideLevel1 = wideLevel1 - minLumaOrBaseLuma;
          if ((int)wideLevel1 < 0) {
            wideLevel1 = 0;
          }
          else if (0xf < (int)wideLevel1) {
            wideLevel1 = 0xf;
          }
          currentBlockCursor = (ulonglong *)(blockRowPixels + -frameWidthPixels);
          wideLevel2 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 0xc));
          wideLevel2 = wideLevel2 - minLumaOrBaseLuma;
          if ((int)wideLevel2 < 0) {
            wideLevel2 = 0;
          }
          else if (0xf < (int)wideLevel2) {
            wideLevel2 = 0xf;
          }
          wideLevel3 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)currentBlockCursor[1]);
          wideLevel3 = wideLevel3 - minLumaOrBaseLuma;
          if ((int)wideLevel3 < 0) {
            wideLevel3 = 0;
          }
          else if (0xf < (int)wideLevel3) {
            wideLevel3 = 0xf;
          }
          wideLevel4 = MovieColor_ComputeLuma5FromRgb888(*(PackedRgb24 *)((int)currentBlockCursor + 4));
          wideLevel4 = wideLevel4 - minLumaOrBaseLuma;
          if ((int)wideLevel4 < 0) {
            wideLevel4 = 0;
          }
          else if (0xf < (int)wideLevel4) {
            wideLevel4 = 0xf;
          }
          wideLevel5 = MovieColor_ComputeLuma5FromRgb888((PackedRgb24)*currentBlockCursor);
          wideLevel5 = wideLevel5 - minLumaOrBaseLuma;
          if ((int)wideLevel5 < 0) {
            wideLevel5 = 0;
          }
          else if (0xf < (int)wideLevel5) {
            wideLevel5 = 0xf;
          }
          *outputCursor = *outputCursor |
                     (wideLevel6 >> 1) << 0x1d | (maxLumaChromaOrLevel >> 1) << 0x1a | (sampleLumaOrLevel >> 1) << 0x17 |
                     (wideLevel0 >> 1) << 0x14 | (wideLevel1 >> 1) << 0x11 | (wideLevel2 >> 1) << 0xe |
                     (wideLevel3 >> 1) << 0xb | (wideLevel4 >> 1) << 8 | (wideLevel5 >> 1) << 5;
          outputCursor = outputCursor + 2;
        }
      }
      pendingSkipCount = minLumaOrSkipCount;
      currentBlockCursor = currentBlockCursor + 2;
      blocksLeftInRow = blocksLeftInRow - 1;
    } while (blocksLeftInRow != 0);
    currentBlockCursor = (ulonglong *)((int)currentBlockCursor + frameWidthPixels * 0xc);
    blockRowsLeft = blockRowsLeft - 1;
    blocksLeftInRow = frameWidthPixels >> 2;
  } while (blockRowsLeft != 0);
  if (pendingSkipCount != 0) {
    if (pendingSkipCount < 9) {
      *(byte *)outputCursor = ((char)pendingSkipCount + -1) * ' ' | 0x19;
      outputCursor = (uint *)((int)outputCursor + 1);
    }
    else if (pendingSkipCount < 0x809) {
      *(ushort *)outputCursor = ((short)pendingSkipCount + -9) * 0x20 | 0x1a;
      outputCursor = (uint *)((int)outputCursor + 2);
    }
    else {
      *outputCursor = (pendingSkipCount - 0x809) * 0x20 | 0x1b;
      outputCursor = outputCursor + 1;
    }
  }
  return (int)outputCursor + (7 - (int)encodedOutput) & 0xfffffff8;
}


/* Address: 0x004A8A60.
   Ownership: movie/runtime/playback.
   Purpose: Starts embedded audio on the first frame, decodes the next 4x4 delta frame when enough bytes are
   buffered, compacts the bounded stream buffer, and requests asynchronous refill. CF clear returns the active
   MovieRuntime in EAX; CF set reports end-of-movie or read failure.
   Local calls: Movie_DecodeFrame4x4Delta.
*/
/* Debug tool: OPEN_THANDOR_MOVIEDUMP=1 logs every decoded frame (consumed bytes, stream state,
   pixel checksum) and writes every tenth frame to moviedump\frame_NNNN.bmp. */
static void Movie_DebugDumpFrame(MovieRuntime *movie, dword consumedBytes)
{
  static int enabled = -1;
  dword width = movie->sourceEntry.pixelWidth;
  dword height = movie->sourceEntry.pixelHeight;
  dword sum = 0;
  dword i;
  if (enabled < 0) {
    const char *value = getenv("OPEN_THANDOR_MOVIEDUMP");
    enabled = (value != NULL) && (value[0] == '1');
    if (enabled) {
      CreateDirectoryA("moviedump", NULL);
    }
  }
  if (!enabled) {
    return;
  }
  for (i = 0; i < width * height; i += 7) {
    sum = sum * 31 + movie->argbPixels[i];
  }
  Thandor_Log("movie frame %u/%u: consumed=%x offset=%x loadedEnd-header=%x remaining=%x state=%d worker=%d sum=%08x",
              movie->currentFrameIndex, movie->fileHeader->frameCount, consumedBytes,
              movie->videoStreamOffset, (dword)(movie->loadedVideoEnd - (byte *)movie->fileHeader),
              movie->remainingVideoBytes, (int)movie->streamState, (int)movie->workerActive, sum);
  if ((movie->currentFrameIndex % 10) == 1) {
    char name[64];
    FILE *file;
    sprintf(name, "moviedump\\frame_%04u.bmp", movie->currentFrameIndex);
    file = fopen(name, "wb");
    if (file != NULL) {
      dword imageBytes = width * height * 4;
      dword header[13];
      int y;
      memset(header, 0, sizeof header);
      fwrite("BM", 1, 2, file);
      header[0] = 54 + imageBytes; /* file size */
      header[2] = 54;              /* pixel data offset */
      header[3] = 40;              /* BITMAPINFOHEADER */
      header[4] = width;
      header[5] = (dword)-(int)height; /* top-down */
      header[6] = 1 | (32 << 16);  /* planes, bit count */
      header[8] = imageBytes;
      fwrite(header, 4, 13, file);
      for (y = 0; y < (int)height; y++) {
        fwrite(movie->argbPixels + y * width, 4, width, file);
      }
      fclose(file);
    }
  }
}

MovieAdvanceFrameEaxCf5 __thandor_eax_cf_preserve_ecx_edx Movie_AdvanceFrame(void)

{
  MovieFileHeader *flmHeader;
  MovieFrameIndex previousFrameIndex;
  MovieRuntime *movie;
  uint byteCountOrStatus;
  dword consumedBytes;
  uint nextFrameOrLoadedSize;
  void *unaff_EBX = (void *)0; /* the original closes a stale caller EBX here */
  undefined4 *copySource;
  byte *streamCursor;
  SoundPlayVoiceEaxCf5 playResult;
  MovieAdvanceFrameEaxCf5 successResult;
  MovieAdvanceFrameEaxCf5 bufferingResult;
  MovieAdvanceFrameEaxCf5 failureResult;
  
  movie = g_ActiveMovie;
  byteCountOrStatus = 0x30;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_ActiveMovie->streamState == MOVIE_STREAM_READ_FAILED) {
      (*g_FileSystemClose)(unaff_EBX);
      movie->remainingVideoBytes = 0;
    }
    else {
      if (((g_ActiveMovie->streamState == MOVIE_STREAM_IDLE) && (g_ActiveMovie->workerActive != 0))
         && (g_ActiveMovie->remainingVideoBytes != 0)) {
        if ((uint)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200)
        {
          g_ActiveMovie->streamState = MOVIE_STREAM_FILL_REQUESTED;
          ReleaseSemaphore(movie->refillSemaphore,1,(LPLONG)0x0);
        }
      }
      flmHeader = movie->fileHeader;
      previousFrameIndex = movie->currentFrameIndex;
      streamCursor = (flmHeader->common).buildMetadata.assetRelativeAddressAnchor28 +
               (movie->videoStreamOffset - 0x28);
      if ((previousFrameIndex == 0) && (movie->audioVoiceSet != (DirectSoundVoiceSet *)0x0)) {
        playResult = (*g_SoundPlayOneShot)
                          (movie->audioGainQ15,movie->audioGainQ15,movie->audioVoiceSet);
        movie->activeAudioBuffer = playResult.eax;
      }
      nextFrameOrLoadedSize = previousFrameIndex + 1;
      byteCountOrStatus = (int)movie->loadedVideoEnd - (int)streamCursor;
      if (nextFrameOrLoadedSize <= flmHeader->frameCount) {
        if ((movie->remainingVideoBytes != 0) && (byteCountOrStatus < 0x1e000)) {
          bufferingResult.carry = false;
          bufferingResult.eax = (dword)&movie[-1].textureCommon.allocationSizeBytes;
          return bufferingResult;
        }
        consumedBytes = Movie_DecodeFrame4x4Delta
                          (flmHeader->heightPixels,flmHeader->widthPixels,movie->argbPixels,streamCursor);
        movie->currentFrameIndex = nextFrameOrLoadedSize;
        movie->videoStreamOffset = movie->videoStreamOffset + consumedBytes;
        Movie_DebugDumpFrame(movie, consumedBytes);
        if ((movie->openFlags != 0) && (movie->streamState == MOVIE_STREAM_IDLE)) {
          byteCountOrStatus = movie->videoStreamOffset;
          nextFrameOrLoadedSize = (int)movie->loadedVideoEnd - (int)movie->fileHeader;
          if ((0x1e01ff < byteCountOrStatus) && (byteCountOrStatus < nextFrameOrLoadedSize)) {
            movie->videoStreamOffset = movie->videoStreamOffset - 0x1e0000;
            streamCursor = movie->fileHeader[-0xf00].common.buildMetadata.assetRelativeAddressAnchor28 +
                     (byteCountOrStatus - 0x28);
            movie->loadedVideoEnd = movie->loadedVideoEnd + -0x1e0000;
            copySource = (undefined4 *)(streamCursor + 0x1e0000);
            for (byteCountOrStatus = nextFrameOrLoadedSize - byteCountOrStatus >> 2; byteCountOrStatus != 0; byteCountOrStatus = byteCountOrStatus - 1) {
              *(undefined4 *)streamCursor = *copySource;
              copySource = copySource + 1;
              streamCursor = streamCursor + 4;
            }
          }
        }
        successResult.carry = false;
        successResult.eax = (dword)movie;
        return successResult;
      }
    }
  }
  failureResult.carry = true;
  failureResult.eax = byteCountOrStatus;
  return failureResult;
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
  uint frameIndex;
  MovieAdvanceFrameEaxCf5 advanceResult;
  
  frameIndex = g_MoviePlaybackCurrentFrame;
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      frameIndex = frameIndex + 1;
      advanceResult = Movie_AdvanceFrame();
      if (advanceResult.carry) {
        return;
      }
    } while (frameIndex < targetFrame);
    g_MoviePlaybackCurrentFrame = frameIndex;
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
  uint blockWord0;
  uint blockWord1;
  dword pixel1;
  dword pixel2;
  dword pixel3;
  uint tokenOrColorBase;
  uint *streamCursor;
  dword *destinationRow;
  uint blocksLeftInRow;
  uint blockRowsLeft;
  uint skipRemaining;
  
  blockRowsLeft = heightPixels >> 2;
  skipRemaining = 0;
  streamCursor = (uint *)encodedFrame;
  blocksLeftInRow = widthPixels >> 2;
  do {
    do {
      blockWord0 = *streamCursor;
      if (skipRemaining == 0) {
        tokenOrColorBase = blockWord0 & 0x1f;
        blockWord1 = streamCursor[1];
        if (tokenOrColorBase < 0x19) {
          if ((int)blockWord1 < 0) {
            tokenOrColorBase = (blockWord1 & 0x7fe00000) >> 0x10 | *streamCursor & 0x1f;
            blockWord0 = *streamCursor;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 8 & 7) * 2];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0xb & 7) * 2];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0xe & 7) * 2];
            *destinationArgb = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 5 & 7) * 2];
            destinationArgb[1] = pixel1;
            destinationArgb[2] = pixel2;
            destinationArgb[3] = pixel3;
            destinationRow = destinationArgb + widthPixels;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x14 & 7) * 2];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x17 & 7) * 2];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x1a & 7) * 2];
            *destinationRow = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x11 & 7) * 2];
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationRow = destinationRow + widthPixels;
            blockWord1 = streamCursor[1];
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 & 7) * 2];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 3 & 7) * 2];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 6 & 7) * 2];
            *destinationRow = *(dword *)((int)g_MovieChromaLumaToArgb[0] +
                                (blockWord0 >> 0x1a & 0xfffffff8) + tokenOrColorBase * 4);
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationRow = destinationRow + widthPixels;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0xc & 7) * 2];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0xf & 7) * 2];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0x12 & 7) * 2];
            streamCursor = streamCursor + 2;
            *destinationRow = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 9 & 7) * 2];
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationArgb = destinationRow + widthPixels * -3;
          }
          else {
            tokenOrColorBase = (blockWord1 & 0x7fe00000) >> 0x10 | *streamCursor & 0x1f;
            blockWord0 = *streamCursor;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 8 & 7)];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0xb & 7)];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0xe & 7)];
            *destinationArgb = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 5 & 7)];
            destinationArgb[1] = pixel1;
            destinationArgb[2] = pixel2;
            destinationArgb[3] = pixel3;
            destinationRow = destinationArgb + widthPixels;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x14 & 7)];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x17 & 7)];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x1a & 7)];
            *destinationRow = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x11 & 7)];
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationRow = destinationRow + widthPixels;
            blockWord1 = streamCursor[1];
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 & 7)];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 3 & 7)];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 6 & 7)];
            *destinationRow = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord0 >> 0x1d)];
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationRow = destinationRow + widthPixels;
            pixel1 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0xc & 7)];
            pixel2 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0xf & 7)];
            pixel3 = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 0x12 & 7)];
            streamCursor = streamCursor + 2;
            *destinationRow = g_MovieChromaLumaToArgb[0][tokenOrColorBase + (blockWord1 >> 9 & 7)];
            destinationRow[1] = pixel1;
            destinationRow[2] = pixel2;
            destinationRow[3] = pixel3;
            destinationArgb = destinationRow + widthPixels * -3;
          }
        }
        else if (tokenOrColorBase == 0x19) {
          streamCursor = (uint *)((int)streamCursor + 1);
          skipRemaining = (blockWord0 & 0xff) >> 5;
        }
        else if (tokenOrColorBase < 0x1b) {
          streamCursor = (uint *)((int)streamCursor + 2);
          skipRemaining = ((blockWord0 & 0xffff) >> 5) + 8;
        }
        else {
          streamCursor = streamCursor + 1;
          skipRemaining = (blockWord0 >> 5) + 0x808;
        }
      }
      else {
        skipRemaining = skipRemaining - 1;
      }
      destinationArgb = destinationArgb + 4;
      blocksLeftInRow = blocksLeftInRow - 1;
    } while (blocksLeftInRow != 0);
    destinationArgb = destinationArgb + widthPixels * 3;
    blockRowsLeft = blockRowsLeft - 1;
    blocksLeftInRow = widthPixels >> 2;
  } while (blockRowsLeft != 0);
  return (int)streamCursor + (7 - (int)encodedFrame) & 0xfffffff8;
}


/* Address: 0x004A6FB0.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie color compute chroma code from rgb888.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed].
*/
uint __thandor_eax_preserve_ecx_edx MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888)

{
  uint middleChannel;
  FixedLengthAngleEaxEdx8 angleAndLength;
  
  middleChannel = rgb888 >> 8 & 0xff;
  angleAndLength = FixedMath_Vector2AngleAndLengthRegs
                    (((rgb888 & 0xff) - middleChannel) * 0xddb4,
                     (middleChannel + (rgb888 & 0xff) + (rgb888 >> 0x10 & 0xff) * -2) * 0x8000);
  return angleAndLength.length >> 9 & 0x7c00 | angleAndLength.angle >> 6 & 0x3e0;
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

