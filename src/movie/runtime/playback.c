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

/* MMX lane helpers for the 4x4 block encoders. Per 4-pixel row the original does MOVD mm,[pixel];
   PUNPCKLBW mm,mm (each byte duplicated into a word); PSRLW mm,6; PADDW into MM7 -- four 16-bit channel
   sums, one lane per pixel byte, wrapping at 16 bits. After four rows PSRLW MM7,6; PACKUSWB MM7,MM7;
   MOVD packs the four averages back into one pixel. */

/* One byte lane of one pixel after PUNPCKLBW mm,mm and PSRLW mm,6. */
static __inline uint16_t Movie_DuplicatedByteLaneShr6(PackedRgb24 pixel,int lane)
{
  uint8_t value = (uint8_t)(pixel >> (lane * 8));
  return (uint16_t)((((uint16_t)value << 8) | value) >> 6);
}

/* PADDW of one 4-pixel row into the four 16-bit channel sums. */
static __inline uint64_t
Movie_AddRowToChannelSums(uint64_t channelSums,PackedRgb24 pixel0,PackedRgb24 pixel1,PackedRgb24 pixel2,
                          PackedRgb24 pixel3)
{
  uint64_t result = 0;
  uint16_t sum;
  int lane;
  for (lane = 0; lane < 4; lane = lane + 1) {
    sum = (uint16_t)(channelSums >> (lane * 16));
    sum = (uint16_t)(sum + Movie_DuplicatedByteLaneShr6(pixel0,lane) + Movie_DuplicatedByteLaneShr6(pixel1,lane) +
                   Movie_DuplicatedByteLaneShr6(pixel2,lane) + Movie_DuplicatedByteLaneShr6(pixel3,lane));
    result = result | ((uint64_t)sum << (lane * 16));
  }
  return result;
}

/* PSRLW mm,6; PACKUSWB mm,mm; MOVD: the four channel averages as one pixel. The saturation to 0xFF can
   never trigger (16 lanes of at most 0x3FF, shifted right by 6), so PACKUSWB's signed input view does not
   matter either. */
static __inline PackedRgb24 Movie_PackChannelAverages(uint64_t channelSums)
{
  PackedRgb24 color = 0;
  uint16_t average;
  int lane;
  for (lane = 0; lane < 4; lane = lane + 1) {
    average = (uint16_t)((uint16_t)(channelSums >> (lane * 16)) >> 6);
    color = color | ((uint32_t)(average > 0xff ? 0xff : average) << (lane * 8));
  }
  return color;
}

/* Address: 0x004A8040.
   Ownership: movie/runtime/playback.
   Purpose: Handles movie encode flm buffer from frame provider carry-flag result.
   Local calls: Movie_EncodeFrame4x4Keyframe, Movie_EncodeFrame4x4Delta.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Movie_EncodeFlmBufferFromFrameProviderCf
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderCfProc *frameProvider)

{
  uint32_t packedTimeOrDate;
  void *frameToReleaseOrNull;
  uint32_t byteCount;
  int clearCount;
  uint32_t frameCount;
  uint32_t *sourcePixels;
  uint32_t *outputCursor;
  FrameProviderResult providerResult;
  StatusResult successResult;
  StatusResult failureResult;
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
  packedTimeOrDate = g_LocaleGetPackedCurrentTime();
  outputCursor[-0x7c] = packedTimeOrDate;
  outputCursor[-0x7a] = packedTimeOrDate;
  outputCursor[-0x78] = packedTimeOrDate;
  packedTimeOrDate = g_LocaleGetPackedCurrentDate();
  outputCursor[-0x7b] = packedTimeOrDate;
  outputCursor[-0x79] = packedTimeOrDate;
  outputCursor[-0x77] = packedTimeOrDate;
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(outputCursor + -0x74));
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(outputCursor + -100));
  *(uint8_t *)(outputCursor + -0x40) = 0;
  outputCursor[-0x54] = frameWidthPixels;
  outputCursor[-0x53] = frameHeightPixels;
  outputCursor[-0x52] = 0;
  outputCursor[-0x51] = 0;
  providerResult = frameProvider((void *)0x0);
  frameToReleaseOrNull = providerResult.frameOrError;
  if (!providerResult.noFrame) {
    frameCount = 1;
    sourcePixels = (uint32_t *)((int)frameToReleaseOrNull +
                           *(int *)((int)frameToReleaseOrNull +
                                   *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc));
    byteCount = Movie_EncodeFrame4x4Keyframe(frameHeightPixels,frameWidthPixels,outputCursor,sourcePixels);
    outputCursor = (uint32_t *)((int)outputCursor + byteCount);
    firstFrame = frameToReleaseOrNull;
    while( true ) {
      providerResult = frameProvider((void *)0x0);
      frameToReleaseOrNull = providerResult.frameOrError;
      if (providerResult.noFrame) break;
      frameCount = frameCount + 1;
      byteCount = Movie_EncodeFrame4x4Delta
                        (frameHeightPixels,frameWidthPixels,outputCursor,sourcePixels,
                         (uint32_t *)(*(int *)((int)frameToReleaseOrNull +
                                          *(int *)((int)frameToReleaseOrNull + 0xb8) + 0xc) +
                                 (int)frameToReleaseOrNull));
      outputCursor = (uint32_t *)((int)outputCursor + byteCount);
      frameProvider(frameToReleaseOrNull);
    }
    frameProvider(firstFrame);
    byteCount = (int)outputCursor - (int)outputBuffer;
    outputBuffer[0x2e] = frameCount;
    outputBuffer[0x3f] = 0x10;
    outputBuffer[1] = byteCount;
    outputBuffer[0x30] = byteCount;
    if (frameToReleaseOrNull == (void *)0xffffffff) {
      outputBuffer[0x30] = outputBuffer[0x30] - 0x200;
      successResult.failed = false;
      successResult.valueOrError = byteCount;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)frameToReleaseOrNull;
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
  uint32_t targetFrame;
  uint32_t boundaryFrame;
  
  g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + 1;
  if (g_MoviePlaybackScheduleSpan != 0) {
    targetFrame = (uint32_t)(g_MoviePlaybackScheduleCounter * 8) / g_MoviePlaybackScheduleSpan + 1 +
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


/* Part of Movie_Open (0x004A870D-0x004A87AF): picks one of the header's audio tracks at random and turns it
   into a sample voice set. The stream stands right after the initially loaded video bytes; the audio tracks
   follow the whole video stream. CF clear returns the voice set in valueOrError (0 when the header has no
   usable track); CF set returns the error of the failing seek/alloc/read/voice-set call. The loaded sample is
   freed on both the success and the failure path, as in the original. */
static StatusResult
Movie_OpenLoadRandomAudioTrackCf(MovieFileHeader *header,MovieStreamByteCount remainingVideoBytes,void *handle)

{
  uint32_t audioTrackCount;
  uint32_t selectedTrack;
  uint32_t track;
  uint32_t trackOffset;
  uint32_t trackBytes;
  void *audioSample;
  FileSystemSeekResult seekResult;
  ArenaAllocResult allocResult;
  FileSystemReadResult readResult;
  SampleVoiceSetResult voiceSetResult;
  StatusResult result;

  result.failed = false;
  result.valueOrError = 0;
  audioTrackCount = header->audioTrackCount;
  if ((audioTrackCount == 0) || (0xe < audioTrackCount)) {
    return result;
  }
  selectedTrack = 0;
  if (1 < audioTrackCount) {
    selectedTrack = (Random_NextPrimary() & 0xffff) % audioTrackCount; /* DIV: unsigned */
  }
  trackOffset = 0;
  for (track = 0; track < selectedTrack; track = track + 1) {
    trackOffset = trackOffset + header->audioTrackBytes[track];
  }
  trackBytes = header->audioTrackBytes[selectedTrack];
  if (trackBytes == 0) {
    return result;
  }
  seekResult = g_FileSystemSeekCf(FILESYSTEM_SEEK_CURRENT,trackOffset + remainingVideoBytes,handle);
  result.failed = seekResult.failed;
  result.valueOrError = seekResult.positionOrError;
  if (seekResult.failed) {
    return result;
  }
  allocResult = g_MemoryApi.alloc(trackBytes);
  result.failed = allocResult.failed;
  result.valueOrError = allocResult.payloadOrError;
  if (allocResult.failed) {
    return result;
  }
  audioSample = (void *)allocResult.payloadOrError;
  readResult = g_FileSystemReadExactCf(trackBytes,audioSample,handle);
  result.failed = readResult.failed;
  result.valueOrError = readResult.valueOrError;
  if (!readResult.failed) {
    voiceSetResult = g_SoundCreateSampleVoiceSet((SoundSampleAsset *)audioSample);
    result.failed = voiceSetResult.failed;
    result.valueOrError = (uint32_t)voiceSetResult.voiceSet;
  }
  g_MemoryApi.free(audioSample);
  return result;
}


/* Address: 0x004A8590.
   Ownership: movie/runtime/playback.
   Purpose: Opens an FLM from a mounted package or loose path, validates magic/version, chooses one embedded audio
   track, builds a one-subresource gfx-compatible MovieRuntime, and optionally starts the refill worker. CF clear
   means success; EAX returns frameCount and ECX returns frameIntervalMilliseconds.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], Package_FindEntryAcrossMounts
   [assets/package/runtime], Random_NextPrimary [core/math/random].
*/
MovieOpenResult __thandor_eax_cf_preserve_edx Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path)

{
  MovieFileHeader *header;
  MovieRuntime *movie;
  void *handle;
  uint32_t *copySource;
  uint32_t *copyDestination;
  int copyCount;
  MovieSubresourceCount frameWidth;
  MoviePaletteBankCount frameHeight;
  MovieAudioGainQ15 defaultAudioGain;
  HANDLE semaphoreOrThread;
  uint32_t sizeOrValue;
  uint32_t initialVideoBytes;
  uint32_t status;
  bool looseFileOpened;
  FileSystemOpenResult openResult;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  ArenaAllocResult allocResult;
  StatusResult audioResult;
  MovieOpenResult successResult;
  MovieOpenResult failureResult;
  PackageEntryLookupResult packageEntry;
  MovieStreamByteCount remainingByteCount;
  uint8_t *loadedEnd;
  MovieStreamFileOffset streamPosition;
  MovieSharedStreamHandleFlag isSharedPackageHandle;

  isSharedPackageHandle = 0;
  looseFileOpened = false;
  if (((movieOpenFlags & 0x80000000) == 0) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    openResult = g_FileSystemOpenCf(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)openResult.handleOrError;
    looseFileOpened = !openResult.failed;
  }
  if (!looseFileOpened) {
    movieOpenFlags = movieOpenFlags & 0x7fffffff;
    packageEntry = Package_FindEntryAcrossMounts(path);
    if ((!packageEntry.notFound) &&
       (seekResult = g_FileSystemSeekCf
                           (FILESYSTEM_SEEK_BEGIN,*(int *)(packageEntry.entry + 0x1ec) + 0x200,
                            (void *)packageEntry.fileHandle), !seekResult.failed)) {
      isSharedPackageHandle = isSharedPackageHandle + 1;
      handle = (void *)packageEntry.fileHandle;
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      openResult = g_FileSystemOpenCf(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
      if (openResult.failed) {
        openResult = g_FileSystemOpenCf(0,path);
        if (openResult.failed) {
          /* Nothing is open yet: no close. */
          failureResult.failed = true;
          failureResult.frameCountOrError = openResult.handleOrError;
          return failureResult;
        }
      }
      handle = (void *)openResult.handleOrError;
    }
  }
  readResult = g_FileSystemReadExactCf(0x200,g_PackageScratchBuffer,handle);
  status = readResult.valueOrError;
  if (!readResult.failed) {
    header = (MovieFileHeader *)g_PackageScratchBuffer;
    status = 0x30;
    if ((header->common.magic == ASSET_MAGIC_FLM) && ((uint32_t)header->common.converterVersion == 0x20001)) {
      sizeOrValue = header->videoStreamBytes + 0x200;
      if ((0x3c0000 < sizeOrValue) && (movieOpenFlags != 0)) {
        sizeOrValue = 0x3c0000;
      }
      allocResult = g_MemoryApi.alloc(sizeOrValue);
      status = allocResult.payloadOrError;
      if (!allocResult.failed) {
        copySource = (uint32_t *)g_PackageScratchBuffer;
        copyDestination = (uint32_t *)allocResult.payloadOrError;
        for (copyCount = 0x80; copyCount != 0; copyCount = copyCount + -1) {
          *copyDestination = *copySource;
          copySource = copySource + 1;
          copyDestination = copyDestination + 1;
        }
        header = (MovieFileHeader *)allocResult.payloadOrError;
        initialVideoBytes = header->videoStreamBytes;
        if ((0x3a2000 < initialVideoBytes) && (movieOpenFlags != 0)) {
          initialVideoBytes = 0x3a2000;
        }
        remainingByteCount = header->videoStreamBytes - initialVideoBytes;
        loadedEnd = (uint8_t *)(header + 1) + initialVideoBytes;
        readResult = g_FileSystemReadExactCf(initialVideoBytes,header + 1,handle);
        status = readResult.valueOrError;
        if (!readResult.failed) {
          /* The original also fails on CF of g_FileSystemGetPositionCf (JC 0x004a89f1), but
             FileSystemGetPositionCfProc has no CF result (it returns 0 on failure). */
          streamPosition = g_FileSystemGetPositionCf(handle);
          audioResult = Movie_OpenLoadRandomAudioTrackCf(header,remainingByteCount,handle);
          status = audioResult.valueOrError;
          if (!audioResult.failed) {
            sizeOrValue = header->widthPixels * header->heightPixels * 4 + 0x220;
            allocResult = g_MemoryApi.alloc(sizeOrValue);
            status = allocResult.payloadOrError;
            if (!allocResult.failed) {
              movie = (MovieRuntime *)allocResult.payloadOrError;
              g_ActiveMovie = movie;
              if ((isSharedPackageHandle == 0) && (remainingByteCount == 0)) {
                g_FileSystemClose(handle);
              }
              (movie->textureCommon).magic = ASSET_MAGIC_GFX;
              (movie->textureCommon).allocationSizeBytes = sizeOrValue;
              (movie->textureCommon).formatVersion = 1;
              (movie->textureCommon).converterVersion = 0;
              movie->audioVoiceSet = (DirectSoundVoiceSet *)audioResult.valueOrError;
              movie->activeAudioBuffer = (IDirectSoundBuffer *)0x0;
              frameWidth = header->widthPixels;
              frameHeight = header->heightPixels;
              sizeOrValue = g_LocaleGetPackedCurrentTime();
              (movie->textureCommon).buildMetadata.timestamps.dateValue0 = sizeOrValue;
              (movie->textureCommon).buildMetadata.timestamps.dateValue1 = sizeOrValue;
              (movie->textureCommon).buildMetadata.timestamps.dateValue2 = sizeOrValue;
              sizeOrValue = g_LocaleGetPackedCurrentDate();
              (movie->textureCommon).buildMetadata.timestamps.timeValue0 = sizeOrValue;
              (movie->textureCommon).buildMetadata.timestamps.timeValue1 = sizeOrValue;
              (movie->textureCommon).buildMetadata.timestamps.timeValue2 = sizeOrValue;
              g_LocaleCopyDefaultComputerLabelUtf16
                        ((movie->textureCommon).buildMetadata.names.producerName);
              g_LocaleCopyDefaultComputerLabelUtf16
                        ((movie->textureCommon).buildMetadata.names.sourceName);
              movie->reserved100_1FF[0] = 0;
              movie->subresourceTableOffset = 0x200;
              movie->paletteBankCount = 0;
              movie->subresourceCount = 1;
              movie->fileHeader = header;
              movie->currentFrameIndex = 0;
              movie->videoStreamOffset = 0x200;
              (movie->sourceEntry).dataOffset = 0x220;
              (movie->sourceEntry).pixelWidth = frameWidth;
              (movie->sourceEntry).pixelHeight = frameHeight;
              (movie->sourceEntry).logicalWidth = frameWidth;
              (movie->sourceEntry).logicalHeight = frameHeight;
              (movie->sourceEntry).paletteIndex = -1;
              (movie->sourceEntry).originX = 0;
              (movie->sourceEntry).originY = 0;
              movie->remainingVideoBytes = remainingByteCount;
              movie->streamHandle = handle;
              movie->loadedVideoEnd = loadedEnd;
              defaultAudioGain = g_MovieDefaultAudioGainQ15;
              movie->streamHandleIsSharedPackage = isSharedPackageHandle;
              movie->openFlags = movieOpenFlags;
              movie->streamFileOffset = streamPosition;
              movie->audioGainQ15 = defaultAudioGain;
              movie->workerActive = 0;
              movie->streamState = MOVIE_STREAM_IDLE;
              movie->refillSemaphore = (void *)0x0;
              if ((remainingByteCount != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
                movie->workerActive = movie->workerActive + 1;
                semaphoreOrThread = CreateSemaphoreA((LPSECURITY_ATTRIBUTES)0x0,0,1,(LPCSTR)0x0);
                movie->refillSemaphore = semaphoreOrThread;
                /* The original passes the address of its remainingByteCount local as lpThreadId. */
                semaphoreOrThread = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,
                                      (LPTHREAD_START_ROUTINE)Movie_StreamWorkerThread,(LPVOID)0x0,0,
                                      &remainingByteCount);
                if (semaphoreOrThread == (HANDLE)0x0) {
                  movie->workerActive = movie->workerActive - 1;
                }
                else {
                  CloseHandle(semaphoreOrThread);
                }
              }
              successResult.failed = false;
              successResult.frameCountOrError = header->frameCount;
              successResult.playbackRateHz = header->frameIntervalMilliseconds; /* MOV ECX,[ESI+0xFC] */
              return successResult;
            }
          }
        }
        g_MemoryApi.free(header);
      }
    }
  }
  if (isSharedPackageHandle == 0) {
    g_FileSystemClose(handle);
  }
  failureResult.failed = true;
  failureResult.frameCountOrError = status;
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
  return ((MovieFrameDimensionsEdxEax8)frameHeight << 32) | frameWidth; /* EDX:EAX */
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
uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext)

{
  void *handle;
  MovieRuntime *movie;
  uint32_t byteCount;
  FileSystemReadResult readResult;
  
  /* The original keeps the movie in ESI: it re-reads g_ActiveMovie only at the loop top, after the wait
     and at the exit. */
  for (;;) {
    movie = g_ActiveMovie;
    if (movie == (MovieRuntime *)0x0) break;
    MsgWaitForMultipleObjects(1,&movie->refillSemaphore,0,0x100,0);
    movie = g_ActiveMovie;
    if ((movie == (MovieRuntime *)0x0) || (movie->streamState == MOVIE_STREAM_SHUTDOWN) ||
        (movie->workerActive == 0) || (movie->remainingVideoBytes == 0)) break;
    if (movie->streamState == MOVIE_STREAM_IDLE) continue;
    byteCount = movie->remainingVideoBytes;
    if ((uint32_t)(movie->loadedVideoEnd - (uint8_t *)movie->fileHeader) < 0x3a2200) {
      handle = movie->streamHandle;
      if (0x1e000 < byteCount) {
        byteCount = 0x1e000;
      }
      g_FileSystemSeekCf(FILESYSTEM_SEEK_BEGIN,movie->streamFileOffset,handle);
      readResult = g_FileSystemReadExactCf(byteCount,movie->loadedVideoEnd,handle);
      if (readResult.failed) {
        if (movie->streamState != MOVIE_STREAM_SHUTDOWN) {
          movie->streamState = MOVIE_STREAM_READ_FAILED;
        }
        break;
      }
      movie->remainingVideoBytes = movie->remainingVideoBytes - byteCount;
      movie->streamFileOffset = movie->streamFileOffset + byteCount;
      movie->loadedVideoEnd = movie->loadedVideoEnd + byteCount;
      if ((movie->remainingVideoBytes == 0) && (movie->streamHandleIsSharedPackage == 0)) {
        g_FileSystemClose(handle);
      }
    }
    if ((movie->streamState == MOVIE_STREAM_SHUTDOWN) || (movie->remainingVideoBytes == 0)) break;
    movie->streamState = MOVIE_STREAM_IDLE;
  }
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    g_ActiveMovie->workerActive = 0;
  }
  return 0;
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
      g_SoundStopVoice(activeMovie->activeAudioBuffer);
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
    g_MemoryApi.free(movie->fileHeader);
    if (movie->audioVoiceSet != (DirectSoundVoiceSet *)0x0) {
      g_SoundReleaseSampleVoiceSet(movie->audioVoiceSet);
    }
    if ((movie->remainingVideoBytes != 0) && (movie->streamHandleIsSharedPackage == 0)) {
      g_FileSystemClose(movie->streamHandle);
    }
    g_MemoryApi.free(movie);
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
  uint32_t target = 0;

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
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
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5658f0: { /* screenshot */
    FramebufferCaptureResult capture =
         g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
    PcxEncodeResult pcx;
    uint16_t *digitHigh = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xc);
    uint16_t *digitLow = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xe);
    if (capture.failed) {
      break;
    }
    pcx = g_PcxFunctionExport3(g_PcxFunctionModule,capture.capture);
    if (pcx.failed) {
      g_MemoryApi.free(capture.capture);
      break;
    }
    FileSystem_WriteBufferToPathCf(pcx.encodedByteCount,pcx.encodedBytesOrError,
                                   (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0));
    g_MemoryApi.free(pcx.encodedBytesOrError);
    g_MemoryApi.free(capture.capture);
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
    if (((*(uint32_t *)((uint8_t *)endMovieRuntime + 0x6ec) & 8) != 0) ||
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
uint32_t __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Keyframe
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels)

{
  uint32_t maxLumaOrLevel;
  uint32_t minLumaOrBaseLuma;
  uint32_t sampleLumaOrLevel;
  uint32_t minLumaChromaOrLevel;
  int level0;
  int level1;
  int level2;
  int level3;
  int level4;
  int level5;
  int level6;
  int level7;
  int level8;
  uint32_t wideLevel0;
  uint32_t wideLevel1;
  uint32_t wideLevel2;
  uint32_t wideLevel3;
  uint32_t wideLevel4;
  uint32_t wideLevel5;
  PackedRgb24 *blockRowPixels;
  uint32_t *outputCursor;
  PackedRgb24 averageColor;
  uint64_t channelSums;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;

  blockRowsLeft = frameHeightPixels >> 2;
  outputCursor = encodedOutput;
  blocksLeftInRow = frameWidthPixels >> 2;
  do {
    do {
      channelSums = Movie_AddRowToChannelSums(0,sourcePixels[0],sourcePixels[1],sourcePixels[2],sourcePixels[3]);
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
      channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                              blockRowPixels[3]);
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
      channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                              blockRowPixels[3]);
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
      channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                              blockRowPixels[3]);
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
      averageColor = Movie_PackChannelAverages(channelSums);
      minLumaOrBaseLuma = (int)((minLumaChromaOrLevel - 8) + maxLumaOrLevel) >> 1;
      if ((int)minLumaOrBaseLuma < 0) {
        minLumaOrBaseLuma = 0;
      }
      else if (0x18 < (int)minLumaOrBaseLuma) {
        minLumaOrBaseLuma = 0x18;
      }
      if (maxLumaOrLevel - minLumaChromaOrLevel < 0xc) {
        *outputCursor = minLumaOrBaseLuma;
        minLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
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
        minLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
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
uint32_t __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Delta
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels)

{
  int rowStrideBytes;
  uint64_t copiedQwordA;
  uint64_t copiedQwordB;
  uint64_t *previousBlock;
  uint32_t maxLumaChromaOrLevel;
  uint32_t minLumaOrBaseLuma;
  uint32_t sampleLumaOrLevel;
  uint32_t minLumaOrSkipCount;
  int level0;
  int level1;
  int level2;
  int level3;
  int level4;
  int level5;
  uint32_t wideLevel0;
  int level6;
  int level7;
  int level8;
  uint32_t wideLevel1;
  uint32_t wideLevel2;
  uint32_t wideLevel3;
  uint32_t wideLevel4;
  uint32_t wideLevel5;
  uint32_t wideLevel6;
  PackedRgb24 *blockRowPixels;
  uint64_t *currentBlockCursor;
  uint32_t *outputCursor;
  uint64_t changedBitsOrQword;
  PackedRgb24 averageColor;
  uint64_t channelSums;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;
  uint32_t pendingSkipCount;
  
  rowStrideBytes = frameWidthPixels * 4;
  blockRowsLeft = frameHeightPixels >> 2;
  pendingSkipCount = 0;
  currentBlockCursor = (uint64_t *)currentFramePixels;
  outputCursor = encodedOutput;
  blocksLeftInRow = frameWidthPixels >> 2;
  do {
    do {
      previousBlock = (uint64_t *)(((int)previousFramePixels + (int)currentBlockCursor) - (int)currentFramePixels);
      changedBitsOrQword = g_MovieDeltaRgbHighNibbleMask2Pixels &
               (*currentBlockCursor & g_MovieDeltaRgbHighNibbleMask2Pixels ^ *previousBlock |
                currentBlockCursor[1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^ previousBlock[1] |
                *(uint64_t *)(rowStrideBytes + (int)currentBlockCursor) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(uint64_t *)((int)previousBlock + rowStrideBytes) |
                *(uint64_t *)(rowStrideBytes + 8 + (int)currentBlockCursor) & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                *(uint64_t *)((int)previousBlock + rowStrideBytes + 8) |
                currentBlockCursor[frameWidthPixels] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                previousBlock[frameWidthPixels] |
                currentBlockCursor[frameWidthPixels + 1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^
                previousBlock[frameWidthPixels + 1] |
               *(uint64_t *)((int)currentBlockCursor + frameWidthPixels * 0xc) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(uint64_t *)((int)previousBlock + frameWidthPixels * 0xc) |
               *(uint64_t *)((int)currentBlockCursor + frameWidthPixels * 0xc + 8) &
               g_MovieDeltaRgbHighNibbleMask2Pixels ^
               *(uint64_t *)((int)previousBlock + frameWidthPixels * 0xc + 8));
      minLumaOrSkipCount = pendingSkipCount + 1;
      if ((int)(changedBitsOrQword >> 0x20) != 0 || (int)changedBitsOrQword != 0) {
        if (pendingSkipCount != 0) {
          if (pendingSkipCount < 9) {
            *(uint8_t *)outputCursor = ((char)pendingSkipCount + -1) * ' ' | 0x19;
            outputCursor = (uint32_t *)((int)outputCursor + 1);
          }
          else if (pendingSkipCount < 0x809) {
            *(uint16_t *)outputCursor = ((short)pendingSkipCount + -9) * 0x20 | 0x1a;
            outputCursor = (uint32_t *)((int)outputCursor + 2);
          }
          else {
            *outputCursor = (pendingSkipCount - 0x809) * 0x20 | 0x1b;
            outputCursor = outputCursor + 1;
          }
          pendingSkipCount = 0;
        }
        previousBlock = (uint64_t *)(((int)previousFramePixels + (int)currentBlockCursor) - (int)currentFramePixels)
        ;
        changedBitsOrQword = currentBlockCursor[1];
        copiedQwordA = *(uint64_t *)((int)currentBlockCursor + frameWidthPixels * 4);
        copiedQwordB = *(uint64_t *)((int)currentBlockCursor + (frameWidthPixels + 2) * 4);
        *previousBlock = *currentBlockCursor;
        previousBlock[1] = changedBitsOrQword;
        *(uint64_t *)((int)previousBlock + rowStrideBytes) = copiedQwordA;
        *(uint64_t *)((int)previousBlock + rowStrideBytes + 8) = copiedQwordB;
        changedBitsOrQword = currentBlockCursor[frameWidthPixels + 1];
        copiedQwordA = *(uint64_t *)((int)currentBlockCursor + frameWidthPixels * 0xc);
        copiedQwordB = *(uint64_t *)((int)currentBlockCursor + (frameWidthPixels * 3 + 2) * 4);
        previousBlock[frameWidthPixels] = currentBlockCursor[frameWidthPixels];
        previousBlock[frameWidthPixels + 1] = changedBitsOrQword;
        *(uint64_t *)((int)previousBlock + frameWidthPixels * 0xc) = copiedQwordA;
        *(uint64_t *)((int)previousBlock + frameWidthPixels * 0xc + 8) = copiedQwordB;
        blockRowPixels = (PackedRgb24 *)currentBlockCursor;
        channelSums = Movie_AddRowToChannelSums(0,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                                blockRowPixels[3]);
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
        channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                                blockRowPixels[3]);
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
        channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                                blockRowPixels[3]);
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
        channelSums = Movie_AddRowToChannelSums(channelSums,blockRowPixels[0],blockRowPixels[1],blockRowPixels[2],
                                                blockRowPixels[3]);
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
        averageColor = Movie_PackChannelAverages(channelSums);
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
          maxLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
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
          currentBlockCursor = (uint64_t *)(blockRowPixels + -frameWidthPixels);
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
          maxLumaChromaOrLevel = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
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
          currentBlockCursor = (uint64_t *)(blockRowPixels + -frameWidthPixels);
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
    currentBlockCursor = (uint64_t *)((int)currentBlockCursor + frameWidthPixels * 0xc);
    blockRowsLeft = blockRowsLeft - 1;
    blocksLeftInRow = frameWidthPixels >> 2;
  } while (blockRowsLeft != 0);
  if (pendingSkipCount != 0) {
    if (pendingSkipCount < 9) {
      *(uint8_t *)outputCursor = ((char)pendingSkipCount + -1) * ' ' | 0x19;
      outputCursor = (uint32_t *)((int)outputCursor + 1);
    }
    else if (pendingSkipCount < 0x809) {
      *(uint16_t *)outputCursor = ((short)pendingSkipCount + -9) * 0x20 | 0x1a;
      outputCursor = (uint32_t *)((int)outputCursor + 2);
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
static void Movie_DebugDumpFrame(MovieRuntime *movie, uint32_t consumedBytes)
{
  static int enabled = -1;
  uint32_t width = movie->sourceEntry.pixelWidth;
  uint32_t height = movie->sourceEntry.pixelHeight;
  uint32_t sum = 0;
  uint32_t i;
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
              movie->videoStreamOffset, (uint32_t)(movie->loadedVideoEnd - (uint8_t *)movie->fileHeader),
              movie->remainingVideoBytes, (int)movie->streamState, (int)movie->workerActive, sum);
  if ((movie->currentFrameIndex % 10) == 1) {
    char name[64];
    FILE *file;
    sprintf(name, "moviedump\\frame_%04u.bmp", movie->currentFrameIndex);
    file = fopen(name, "wb");
    if (file != NULL) {
      uint32_t imageBytes = width * height * 4;
      uint32_t header[13];
      int y;
      memset(header, 0, sizeof header);
      fwrite("BM", 1, 2, file);
      header[0] = 54 + imageBytes; /* file size */
      header[2] = 54;              /* pixel data offset */
      header[3] = 40;              /* BITMAPINFOHEADER */
      header[4] = width;
      header[5] = (uint32_t)-(int)height; /* top-down */
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

MovieFrameResult __thandor_eax_cf_preserve_ecx_edx Movie_AdvanceFrame(void)

{
  MovieFileHeader *flmHeader;
  MovieFrameIndex previousFrameIndex;
  MovieRuntime *movie;
  uint32_t byteCountOrStatus;
  uint32_t consumedBytes;
  uint32_t nextFrameOrLoadedSize;
  uint32_t *copySource;
  uint32_t *copyDestination;
  uint8_t *streamCursor;
  SoundPlayResult playResult;
  MovieFrameResult successResult;
  MovieFrameResult bufferingResult;
  MovieFrameResult failureResult;
  
  movie = g_ActiveMovie;
  byteCountOrStatus = 0x30;
  if (g_ActiveMovie != (MovieRuntime *)0x0) {
    if (g_ActiveMovie->streamState == MOVIE_STREAM_READ_FAILED) {
      /* 0x004A8BDD PUSH EBX; CALL g_FileSystemClose. On this path the function never loads EBX, so the
         original closes whatever EBX its caller left there -- never the movie stream handle: a UI/runtime
         object pointer in the frontend/in-game/briefing callers, g_FramebufferHeight in the
         Game_PlayIntroMovies frame loop (0x00573B2C MOV EBX,ECX), the outer caller's EBX via
         MoviePlayback_AdvanceToFrameAndPresent. Closing NULL keeps the effect (the stream handle stays
         open; remainingVideoBytes = 0 also keeps Movie_Close from closing it) without the stray
         CloseHandle on an unrelated value. */
      g_FileSystemClose((void *)0x0);
      movie->remainingVideoBytes = 0;
    }
    else {
      if (((g_ActiveMovie->streamState == MOVIE_STREAM_IDLE) && (g_ActiveMovie->workerActive != 0))
         && (g_ActiveMovie->remainingVideoBytes != 0)) {
        if ((uint32_t)((int)g_ActiveMovie->loadedVideoEnd - (int)g_ActiveMovie->fileHeader) < 0x3a2200)
        {
          g_ActiveMovie->streamState = MOVIE_STREAM_FILL_REQUESTED;
          ReleaseSemaphore(movie->refillSemaphore,1,(LPLONG)0x0);
        }
      }
      flmHeader = movie->fileHeader;
      previousFrameIndex = movie->currentFrameIndex;
      streamCursor = (uint8_t *)flmHeader + movie->videoStreamOffset;
      if ((previousFrameIndex == 0) && (movie->audioVoiceSet != (DirectSoundVoiceSet *)0x0)) {
        playResult = g_SoundPlayOneShot
                          (movie->audioGainQ15,movie->audioGainQ15,movie->audioVoiceSet);
        movie->activeAudioBuffer = playResult.soundBuffer;
      }
      nextFrameOrLoadedSize = previousFrameIndex + 1;
      byteCountOrStatus = (int)movie->loadedVideoEnd - (int)streamCursor;
      if (nextFrameOrLoadedSize <= flmHeader->frameCount) {
        if ((movie->remainingVideoBytes != 0) && (byteCountOrStatus < 0x1e000)) {
          /* Not enough bytes buffered yet: CF clear without decoding. The original returns ESI - 0x220
             here (0x004A8BD0 LEA EAX,[ESI-0x220]) because ESI is only advanced to the pixels at
             0x004A8B48. Callers keep EAX as the movie only after the first-frame call, which cannot
             get here (with remainingVideoBytes != 0 the first 0x3A2000 bytes are loaded). */
          bufferingResult.ended = false;
          bufferingResult.movieOrError = (uint32_t)((uint8_t *)movie - 0x220);
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
            copyDestination = (uint32_t *)((uint8_t *)movie->fileHeader + byteCountOrStatus - 0x1e0000);
            movie->loadedVideoEnd = movie->loadedVideoEnd + -0x1e0000;
            copySource = (uint32_t *)((uint8_t *)copyDestination + 0x1e0000);
            for (byteCountOrStatus = (nextFrameOrLoadedSize - byteCountOrStatus) >> 2; byteCountOrStatus != 0; byteCountOrStatus = byteCountOrStatus - 1) {
              *copyDestination = *copySource;
              copySource = copySource + 1;
              copyDestination = copyDestination + 1;
            }
          }
        }
        successResult.ended = false;
        successResult.movieOrError = (uint32_t)movie;
        return successResult;
      }
    }
  }
  failureResult.ended = true;
  failureResult.movieOrError = byteCountOrStatus;
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
  uint32_t frameIndex;
  MovieFrameResult advanceResult;
  
  frameIndex = g_MoviePlaybackCurrentFrame;
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      frameIndex = frameIndex + 1;
      advanceResult = Movie_AdvanceFrame();
      if (advanceResult.ended) {
        return;
      }
    } while (frameIndex < targetFrame);
    g_MoviePlaybackCurrentFrame = frameIndex;
    UiRootStack_InvalidateAll();
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
  return;
}


/* Address: 0x004A81C0.
   Ownership: movie/runtime/playback.
   Purpose: Decodes one FLM frame into an existing ARGB image in 4x4 blocks. Tokens 0-24 encode one block through
   g_MovieChromaLumaToArgb, while tokens 25-31 skip runs and preserve pixels from the previous frame. Returns
   encoded bytes consumed rounded up to eight.
*/
uint32_t __thandor_eax_preserve_ecx_edx
Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          uint8_t *encodedFrame)

{
  uint32_t blockWord0;
  uint32_t blockWord1;
  uint32_t pixel1;
  uint32_t pixel2;
  uint32_t pixel3;
  uint32_t tokenOrColorBase;
  uint32_t *streamCursor;
  uint32_t *destinationRow;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;
  uint32_t skipRemaining;
  
  blockRowsLeft = heightPixels >> 2;
  skipRemaining = 0;
  streamCursor = (uint32_t *)encodedFrame;
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
            *destinationRow = *(uint32_t *)((int)g_MovieChromaLumaToArgb[0] +
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
          streamCursor = (uint32_t *)((int)streamCursor + 1);
          skipRemaining = (blockWord0 & 0xff) >> 5;
        }
        else if (tokenOrColorBase < 0x1b) {
          streamCursor = (uint32_t *)((int)streamCursor + 2);
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
uint32_t __thandor_eax_preserve_ecx_edx MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888)

{
  uint32_t middleChannel;
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
uint32_t __thandor_eax_preserve_ecx_edx MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888)

{
  return ((rgb888 & 0xff) + (rgb888 >> 8 & 0xff) + (rgb888 >> 0x10 & 0xff)) * 0x5555 + 0x40000 >>
         0x13;
}


/* Not in the original: the original executable carries g_MovieChromaLumaToArgb precomputed
   (0x00486D90, 1024 x 32 ARGB values). An FLM colour is a 10-bit chroma code and a 5-bit luma:
   the chroma code is saturation (bits 5-9, 0..31) and hue (bits 0-4, 32 steps around the
   circle). Every entry is opaque grey luma * 8 plus a chroma offset whose three channels sum to
   zero, clamped to 0..255:
     red = luma * 8 - 2 * cosTerm,  green = luma * 8 + cosTerm - sinTerm,  blue = luma * 8 + cosTerm + sinTerm
   cosTerm and sinTerm are roughly saturation * 2.65 * cos(hue) and saturation * 4.6 * sin(hue),
   but not exactly any closed formula, so they are kept as the tables taken from the original. */
static const short k_MovieChromaCosTerm[32][32] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -2, -2, -2, -1, 0, 0, 0, 1, 1, 1, 2, 2},
    {5, 4, 4, 4, 3, 2, 1, 0, 0, -2, -3, -3, -4, -5, -5, -6, -6, -6, -5, -5, -4, -3, -3, -2, 0, 0, 1, 2, 3, 4, 4, 4},
    {7, 7, 7, 6, 5, 4, 2, 1, 0, -2, -4, -5, -6, -7, -8, -8, -8, -8, -8, -7, -6, -5, -4, -2, 0, 1, 2, 4, 5, 6, 7, 7},
    {10, 10, 9, 8, 7, 5, 3, 1, 0, -3, -5, -6, -8, -9, -10, -11, -11, -11, -10, -9, -8, -6, -5, -3, 0, 1, 3, 5, 7, 8, 9, 10},
    {13, 12, 11, 10, 9, 7, 4, 2, 0, -3, -6, -8, -10, -12, -13, -14, -14, -14, -13, -12, -10, -8, -6, -3, 0, 2, 4, 7, 9, 10, 11, 12},
    {15, 15, 14, 12, 10, 8, 5, 2, 0, -4, -7, -9, -12, -14, -15, -16, -16, -16, -15, -14, -12, -9, -7, -4, 0, 2, 5, 8, 10, 12, 14, 15},
    {18, 17, 16, 15, 12, 10, 6, 3, 0, -4, -8, -11, -14, -16, -18, -19, -19, -19, -18, -16, -14, -11, -8, -4, 0, 3, 6, 10, 12, 15, 16, 17},
    {21, 20, 19, 17, 14, 11, 7, 3, 0, -5, -9, -12, -16, -18, -20, -21, -22, -21, -20, -18, -16, -12, -9, -5, 0, 3, 7, 11, 14, 17, 19, 20},
    {23, 23, 21, 19, 16, 13, 8, 4, 0, -5, -10, -14, -17, -20, -23, -24, -24, -24, -23, -20, -17, -14, -10, -5, 0, 4, 8, 13, 16, 19, 21, 23},
    {26, 25, 24, 21, 18, 14, 9, 4, 0, -6, -11, -15, -19, -23, -25, -27, -27, -27, -25, -23, -19, -15, -11, -6, 0, 4, 9, 14, 18, 21, 24, 25},
    {29, 28, 26, 24, 20, 15, 10, 5, 0, -6, -12, -17, -21, -25, -28, -29, -30, -29, -28, -25, -21, -17, -12, -6, 0, 5, 10, 15, 20, 24, 26, 28},
    {31, 31, 29, 26, 22, 17, 11, 5, 0, -7, -13, -18, -23, -27, -30, -32, -32, -32, -30, -27, -23, -18, -13, -7, 0, 5, 11, 17, 22, 26, 29, 31},
    {34, 33, 31, 28, 24, 18, 12, 6, 0, -7, -14, -20, -25, -29, -33, -35, -35, -35, -33, -29, -25, -20, -14, -7, 0, 6, 12, 18, 24, 28, 31, 33},
    {37, 36, 34, 30, 26, 20, 13, 6, 0, -8, -15, -21, -27, -32, -35, -37, -38, -37, -35, -32, -27, -21, -15, -8, 0, 6, 13, 20, 26, 30, 34, 36},
    {39, 38, 36, 32, 27, 21, 14, 7, 0, -8, -16, -23, -29, -34, -37, -40, -40, -40, -37, -34, -29, -23, -16, -8, 0, 7, 14, 21, 27, 32, 36, 38},
    {42, 41, 39, 35, 29, 23, 15, 7, 0, -9, -17, -24, -31, -36, -40, -42, -43, -42, -40, -36, -31, -24, -17, -9, 0, 7, 15, 23, 29, 35, 39, 41},
    {45, 44, 41, 37, 31, 24, 17, 8, 0, -9, -18, -26, -33, -38, -42, -45, -46, -45, -42, -38, -33, -26, -18, -9, 0, 8, 17, 24, 31, 37, 41, 44},
    {47, 46, 44, 39, 33, 26, 18, 9, 0, -10, -19, -27, -34, -40, -45, -48, -48, -48, -45, -40, -34, -27, -19, -10, 0, 9, 18, 26, 33, 39, 44, 46},
    {50, 49, 46, 41, 35, 27, 19, 9, 0, -10, -20, -29, -36, -43, -47, -50, -51, -50, -47, -43, -36, -29, -20, -10, 0, 9, 19, 27, 35, 41, 46, 49},
    {53, 51, 48, 44, 37, 29, 20, 10, 0, -11, -21, -30, -38, -45, -50, -53, -54, -53, -50, -45, -38, -30, -21, -11, 0, 10, 20, 29, 37, 44, 48, 51},
    {55, 54, 51, 46, 39, 30, 21, 10, 0, -11, -22, -32, -40, -47, -52, -55, -56, -55, -52, -47, -40, -32, -22, -11, 0, 10, 21, 30, 39, 46, 51, 54},
    {58, 57, 53, 48, 41, 32, 22, 11, 0, -12, -23, -33, -42, -49, -55, -58, -59, -58, -55, -49, -42, -33, -23, -12, 0, 11, 22, 32, 41, 48, 53, 57},
    {61, 59, 56, 50, 43, 33, 23, 11, 0, -12, -24, -35, -44, -51, -57, -61, -62, -61, -57, -51, -44, -35, -24, -12, 0, 11, 23, 33, 43, 50, 56, 59},
    {63, 62, 58, 52, 44, 35, 24, 12, 0, -13, -25, -36, -46, -54, -60, -63, -64, -63, -60, -54, -46, -36, -25, -13, 0, 12, 24, 35, 44, 52, 58, 62},
    {66, 65, 61, 55, 46, 36, 25, 12, 0, -14, -26, -38, -48, -56, -62, -66, -67, -66, -62, -56, -48, -38, -26, -14, 0, 12, 25, 36, 46, 55, 61, 65},
    {69, 67, 63, 57, 48, 38, 26, 13, 0, -14, -27, -39, -50, -58, -65, -69, -70, -69, -65, -58, -50, -39, -27, -14, 0, 13, 26, 38, 48, 57, 63, 67},
    {71, 70, 66, 59, 50, 39, 27, 13, 0, -15, -28, -41, -51, -60, -67, -71, -72, -71, -67, -60, -51, -41, -28, -15, 0, 13, 27, 39, 50, 59, 66, 70},
    {74, 72, 68, 61, 52, 41, 28, 14, 0, -15, -29, -42, -53, -63, -69, -74, -75, -74, -69, -63, -53, -42, -29, -15, 0, 14, 28, 41, 52, 61, 68, 72},
    {77, 75, 71, 63, 54, 42, 29, 14, 0, -16, -30, -43, -55, -65, -72, -76, -78, -76, -72, -65, -55, -43, -30, -16, 0, 14, 29, 42, 54, 63, 71, 75},
    {79, 78, 73, 66, 56, 44, 30, 15, 0, -16, -31, -45, -57, -67, -74, -79, -80, -79, -74, -67, -57, -45, -31, -16, 0, 15, 30, 44, 56, 66, 73, 78},
    {82, 80, 76, 68, 58, 45, 31, 15, 0, -17, -32, -46, -59, -69, -77, -82, -83, -82, -77, -69, -59, -46, -32, -17, 0, 15, 31, 45, 58, 68, 76, 80},
};
static const short k_MovieChromaSinTerm[32][32] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 2, 2, 3, 4, 4, 4, 4, 4, 3, 2, 2, 1, 0, 0, -2, -3, -3, -4, -5, -5, -5, -5, -5, -5, -5, -4, -3, -3, -2},
    {0, 1, 3, 4, 6, 7, 8, 8, 9, 8, 8, 7, 6, 4, 3, 1, 0, -3, -5, -6, -7, -9, -9, -10, -10, -10, -9, -9, -7, -6, -5, -3},
    {0, 2, 5, 7, 9, 10, 12, 13, 13, 13, 12, 10, 9, 7, 5, 2, 0, -3, -6, -9, -10, -12, -14, -14, -14, -14, -14, -12, -10, -9, -6, -3},
    {0, 3, 6, 9, 12, 15, 16, 17, 18, 17, 16, 15, 12, 9, 6, 3, 0, -5, -8, -11, -14, -16, -18, -19, -19, -19, -18, -16, -14, -11, -8, -5},
    {0, 4, 8, 12, 16, 19, 20, 22, 23, 22, 20, 19, 16, 12, 8, 4, 0, -5, -10, -14, -17, -20, -22, -24, -24, -24, -22, -20, -17, -14, -10, -5},
    {0, 5, 10, 15, 19, 22, 25, 27, 27, 27, 25, 22, 19, 15, 10, 5, 0, -6, -11, -16, -20, -24, -26, -28, -28, -28, -26, -24, -20, -16, -11, -6},
    {0, 5, 12, 17, 22, 26, 29, 31, 32, 31, 29, 26, 22, 17, 12, 5, 0, -7, -13, -19, -24, -28, -31, -32, -33, -32, -31, -28, -24, -19, -13, -7},
    {0, 6, 13, 20, 25, 30, 34, 35, 36, 35, 34, 30, 25, 20, 13, 6, 0, -8, -15, -21, -27, -32, -35, -37, -37, -37, -35, -32, -27, -21, -15, -8},
    {0, 8, 15, 23, 28, 34, 38, 40, 41, 40, 38, 34, 28, 23, 15, 8, 0, -9, -17, -24, -30, -35, -39, -41, -42, -41, -39, -35, -30, -24, -17, -9},
    {0, 8, 17, 25, 32, 38, 42, 45, 46, 45, 42, 38, 32, 25, 17, 8, 0, -10, -18, -26, -33, -39, -43, -46, -47, -46, -43, -39, -33, -26, -18, -10},
    {0, 9, 19, 27, 35, 42, 46, 49, 50, 49, 46, 42, 35, 27, 19, 9, 0, -11, -20, -29, -37, -43, -48, -51, -51, -51, -48, -43, -37, -29, -20, -11},
    {0, 10, 20, 30, 38, 45, 50, 54, 55, 54, 50, 45, 38, 30, 20, 10, 0, -11, -22, -32, -40, -47, -52, -55, -56, -55, -52, -47, -40, -32, -22, -11},
    {0, 11, 22, 32, 42, 49, 55, 58, 60, 58, 55, 49, 42, 32, 22, 11, 0, -13, -24, -34, -43, -51, -57, -60, -61, -60, -57, -51, -43, -34, -24, -13},
    {0, 12, 24, 35, 45, 53, 59, 62, 64, 62, 59, 53, 45, 35, 24, 12, 0, -13, -25, -37, -47, -55, -61, -64, -65, -64, -61, -55, -47, -37, -25, -13},
    {0, 13, 25, 38, 48, 57, 63, 67, 69, 67, 63, 57, 48, 38, 25, 13, 0, -14, -27, -39, -50, -58, -65, -69, -70, -69, -65, -58, -50, -39, -27, -14},
    {0, 13, 27, 40, 51, 61, 68, 72, 73, 72, 68, 61, 51, 40, 27, 13, 0, -15, -29, -42, -53, -62, -69, -73, -74, -73, -69, -62, -53, -42, -29, -15},
    {0, 15, 30, 43, 55, 65, 72, 76, 78, 76, 72, 65, 55, 43, 30, 15, 0, -16, -31, -44, -57, -66, -73, -78, -79, -78, -73, -66, -57, -44, -31, -16},
    {0, 16, 31, 46, 58, 68, 76, 81, 83, 81, 76, 68, 58, 46, 31, 16, 0, -17, -33, -47, -59, -70, -78, -82, -84, -82, -78, -70, -59, -47, -33, -17},
    {0, 16, 33, 48, 61, 72, 80, 86, 87, 86, 80, 72, 61, 48, 33, 16, 0, -18, -35, -50, -63, -74, -82, -87, -88, -87, -82, -74, -63, -50, -35, -18},
    {0, 17, 35, 50, 65, 76, 84, 90, 92, 90, 84, 76, 65, 50, 35, 17, 0, -19, -36, -52, -66, -78, -86, -91, -93, -91, -86, -78, -66, -52, -36, -19},
    {0, 18, 36, 53, 68, 80, 89, 94, 96, 94, 89, 80, 68, 53, 36, 18, 0, -20, -38, -55, -69, -81, -91, -96, -97, -96, -91, -81, -69, -55, -38, -20},
    {0, 19, 38, 56, 71, 84, 93, 99, 101, 99, 93, 84, 71, 56, 38, 19, 0, -21, -40, -57, -73, -85, -95, -100, -102, -100, -95, -85, -73, -57, -40, -21},
    {0, 20, 40, 58, 75, 87, 97, 103, 106, 103, 97, 87, 75, 58, 40, 20, 0, -21, -41, -60, -76, -89, -99, -105, -107, -105, -99, -89, -76, -60, -41, -21},
    {0, 21, 42, 61, 77, 91, 102, 108, 110, 108, 102, 91, 77, 61, 42, 21, 0, -22, -43, -62, -79, -93, -103, -110, -111, -110, -103, -93, -79, -62, -43, -22},
    {0, 22, 43, 64, 81, 95, 106, 113, 115, 113, 106, 95, 81, 64, 43, 22, 0, -24, -45, -65, -82, -97, -107, -114, -116, -114, -107, -97, -82, -65, -45, -24},
    {0, 23, 45, 66, 84, 99, 110, 117, 120, 117, 110, 99, 84, 66, 45, 23, 0, -24, -47, -67, -86, -100, -112, -119, -121, -119, -112, -100, -86, -67, -47, -24},
    {0, 24, 47, 69, 87, 103, 114, 121, 124, 121, 114, 103, 87, 69, 47, 24, 0, -25, -48, -70, -89, -104, -116, -123, -125, -123, -116, -104, -89, -70, -48, -25},
    {0, 24, 49, 71, 91, 107, 118, 126, 129, 126, 118, 107, 91, 71, 49, 24, 0, -26, -50, -73, -92, -108, -120, -128, -130, -128, -120, -108, -92, -73, -50, -26},
    {0, 25, 50, 73, 94, 110, 123, 131, 133, 131, 123, 110, 94, 73, 50, 25, 0, -27, -52, -75, -96, -112, -125, -132, -134, -132, -125, -112, -96, -75, -52, -27},
    {0, 26, 52, 76, 97, 114, 127, 135, 138, 135, 127, 114, 97, 76, 52, 26, 0, -28, -54, -78, -99, -116, -129, -137, -139, -137, -129, -116, -99, -78, -54, -28},
    {0, 27, 54, 79, 101, 118, 132, 140, 143, 140, 132, 118, 101, 79, 54, 27, 0, -29, -55, -80, -102, -120, -133, -141, -144, -141, -133, -120, -102, -80, -55, -29},
};

static uint32_t MovieColor_ClampChannel(int value)
{
  return value < 0 ? 0 : (value > 255 ? 255 : (uint32_t)value);
}

/* Called once at startup. */
void Movie_BuildChromaLumaTable(void)
{
  int saturation;
  int hue;
  int luma;

  for (saturation = 0; saturation < 32; saturation++) {
    for (hue = 0; hue < 32; hue++) {
      int cosTerm = k_MovieChromaCosTerm[saturation][hue];
      int sinTerm = k_MovieChromaSinTerm[saturation][hue];
      uint32_t *row = g_MovieChromaLumaToArgb[saturation * 32 + hue];
      for (luma = 0; luma < 32; luma++) {
        int grey = luma * 8;
        row[luma] = 0xff000000u | MovieColor_ClampChannel(grey - 2 * cosTerm) << 16 |
                    MovieColor_ClampChannel(grey + cosTerm - sinTerm) << 8 |
                    MovieColor_ClampChannel(grey + cosTerm + sinTerm);
      }
    }
  }
}
