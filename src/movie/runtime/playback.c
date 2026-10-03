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
#include <thandor/platform/debug/hooks.h>

/* Implementation ownership: movie/runtime/playback. */

/* MMX lane helpers for the 4x4 block encoders (not in the original, which does this inline with MMX
   instructions; the helpers are inlined into Movie_EncodeFrame4x4Keyframe/Delta). Per 4-pixel row the original
   does MOVD mm,[pixel]; PUNPCKLBW mm,mm (each byte duplicated into a word); PSRLW mm,6; PADDW into MM7 -- four 16-bit channel
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
  for (lane = 0; lane < 4; lane++) {
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
  for (lane = 0; lane < 4; lane++) {
    average = (uint16_t)((uint16_t)(channelSums >> (lane * 16)) >> 6);
    color = color | ((uint32_t)(average > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : average) << (lane * 8));
  }
  return color;
}

/* Pixels of a provider frame, a gfx texture source: those of its first subresource entry. */
#define MOVIE_FRAME_PIXELS(frame) \
  ((uint32_t *)((uint8_t *)(frame) + \
                ((GraphicsTextureSourceEntry *)((uint8_t *)(frame) + \
                  ((GraphicsTextureSourceAsset *)(frame))->tableDescriptor.subresourceTableOffset))->dataOffset))

/* Encodes a whole FLM movie into outputBuffer: writes the 0x200-byte MovieFileHeader, encodes the first frame
   the provider returns as a keyframe and every further frame as a delta against it (the delta encoder keeps
   the first frame's pixels up to date as its reference), then fills in the frame count and sizes. The
   provider is called with NULL for the next frame and with a frame to release it; it ends the sequence with CF
   set and 0xFFFFFFFF. Returns true and stores the total byte count in *outByteCount, or returns false
   (*outByteCount untouched, the buffer already written) when the provider yields no frame at all or ends
   with any other error. A leftover of the movie tools: no caller found in src/.
*/
bool Movie_EncodeFlmBufferFromFrameProvider
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderProc *frameProvider,uint32_t *outByteCount)

{
  MovieFileHeader *header;
  uint32_t packedTime;
  uint32_t packedDate;
  uint32_t byteCount;
  int clearCount;
  uint32_t frameCount;
  void *firstFrame;
  uint32_t *firstFramePixels;
  void *frame;
  uint32_t *outputCursor;
  FrameProviderResult providerResult;

  header = (MovieFileHeader *)outputBuffer;
  outputCursor = outputBuffer;
  for (clearCount = MOVIE_FILE_HEADER_BYTES / 4; clearCount != 0; clearCount--) {
    *outputCursor = 0;
    outputCursor++;
  }
  /* outputCursor now points at the video stream, right after the cleared header */
  header->common.magic = ASSET_MAGIC_FLM;
  header->common.allocationSizeBytes = MOVIE_FILE_HEADER_BYTES; /* the final size is stored at the end */
  header->common.formatVersion = 1;
  header->common.converterVersion = MOVIE_FLM_CONVERTER_VERSION;
  packedTime = g_LocaleGetPackedCurrentTime();
  header->common.buildMetadata.timestamps.timeValue0 = packedTime;
  header->common.buildMetadata.timestamps.timeValue1 = packedTime;
  header->common.buildMetadata.timestamps.timeValue2 = packedTime;
  packedDate = g_LocaleGetPackedCurrentDate();
  header->common.buildMetadata.timestamps.dateValue0 = packedDate;
  header->common.buildMetadata.timestamps.dateValue1 = packedDate;
  header->common.buildMetadata.timestamps.dateValue2 = packedDate;
  g_LocaleCopyDefaultComputerLabelUtf16(header->common.buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(header->common.buildMetadata.names.sourceName);
  header->unusedText[0] = 0;
  header->widthPixels = frameWidthPixels;
  header->heightPixels = frameHeightPixels;
  header->frameCount = 0;
  header->audioTrackCount = 0;
  providerResult = frameProvider(NULL);
  if (providerResult.noFrame) {
    return false;
  }
  frameCount = 1;
  firstFrame = providerResult.frameOrError;
  /* a frame is a gfx texture source; its pixels are those of the first subresource entry */
  firstFramePixels = MOVIE_FRAME_PIXELS(firstFrame);
  byteCount = Movie_EncodeFrame4x4Keyframe(frameHeightPixels,frameWidthPixels,outputCursor,firstFramePixels);
  outputCursor = (uint32_t *)((uint8_t *)outputCursor + byteCount);
  providerResult = frameProvider(NULL);
  while (!providerResult.noFrame) {
    frame = providerResult.frameOrError;
    frameCount++;
    byteCount = Movie_EncodeFrame4x4Delta
                      (frameHeightPixels,frameWidthPixels,outputCursor,firstFramePixels,MOVIE_FRAME_PIXELS(frame));
    outputCursor = (uint32_t *)((uint8_t *)outputCursor + byteCount);
    frameProvider(frame); /* release */
    providerResult = frameProvider(NULL);
  }
  frameProvider(firstFrame); /* release */
  byteCount = (uint32_t)((uint8_t *)outputCursor - (uint8_t *)outputBuffer);
  header->frameCount = frameCount;
  header->frameIntervalMilliseconds = 16;
  header->common.allocationSizeBytes = byteCount;
  /* video stream bytes once the header is subtracted below */
  header->videoStreamBytes = byteCount;
  /* the provider ends the sequence with the error 0xFFFFFFFF; any other error fails the encode */
  if (providerResult.frameOrError != (void *)(intptr_t)-1) {
    return false;
  }
  header->videoStreamBytes = header->videoStreamBytes - MOVIE_FILE_HEADER_BYTES;
  *outByteCount = byteCount;
  return true;
}


/* Per-tick callback of a movie played inside a session: counts the tick, works out which frame the movie
   should show by now (8 frames per g_MoviePlaybackScheduleSpan ticks, offset by the base frame group), catches
   up to it, presenting at least every 8th frame on the way, then runs the regular simulation and network tick
   so the session keeps going under the movie.
*/
void MoviePlayback_AdvanceScheduledFrameAndTick(void)

{
  uint32_t targetFrame;
  uint32_t boundaryFrame;

  g_MoviePlaybackScheduleCounter++;
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
   follow the whole video stream. Returns 0 on success and stores the voice set in *outVoiceSet (NULL when the
   header has no usable track); otherwise returns the error of the failing seek/alloc/read/voice-set call. The
   loaded sample is freed on both the success and the failure path, as in the original. */
static uint32_t
Movie_OpenLoadRandomAudioTrack(MovieFileHeader *header,MovieStreamByteCount remainingVideoBytes,void *handle,
                               DirectSoundVoiceSet **outVoiceSet)

{
  uint32_t audioTrackCount;
  uint32_t selectedTrack;
  uint32_t trackIndex;
  uint32_t trackOffset;
  uint32_t trackBytes;
  void *audioSample;
  uint32_t seekError;
  uint32_t allocError;
  uint32_t loadError;
  DirectSoundVoiceSet *voiceSet;

  *outVoiceSet = NULL;
  audioTrackCount = header->audioTrackCount;
  if ((audioTrackCount == 0) || (MOVIE_MAX_AUDIO_TRACKS < audioTrackCount)) {
    return 0;
  }
  selectedTrack = 0;
  if (1 < audioTrackCount) {
    selectedTrack = (Random_NextPrimary() & 0xffff) % audioTrackCount; /* DIV: unsigned */
  }
  trackOffset = 0;
  /* the tracks are stored back to back, so skip the sizes of all tracks before the selected one */
  for (trackIndex = 0; trackIndex < selectedTrack; trackIndex++) {
    trackOffset = trackOffset + header->audioTrackBytes[trackIndex];
  }
  trackBytes = header->audioTrackBytes[selectedTrack];
  if (trackBytes == 0) {
    return 0;
  }
  seekError = g_FileSystemSeek(FILESYSTEM_SEEK_CURRENT,trackOffset + remainingVideoBytes,handle);
  if (seekError != 0) {
    return seekError;
  }
  allocError = g_MemoryApi.alloc(trackBytes,&audioSample);
  if (allocError != 0) {
    return allocError;
  }
  loadError = g_FileSystemReadExact(trackBytes,audioSample,handle);
  if (loadError == 0) {
    loadError = g_SoundCreateSampleVoiceSet((SoundSampleAsset *)audioSample,&voiceSet);
    if (loadError == 0) {
      *outVoiceSet = voiceSet;
    }
  }
  g_MemoryApi.free(audioSample);
  return loadError;
}


/* Failure exit of Movie_Open once a file or package handle is open: closes the handle unless it is a shared
   package handle, stores error in *outError (when not NULL) and returns false. */
static bool Movie_OpenFail(void *handle,MovieSharedStreamHandleFlag isSharedPackageHandle,uint32_t error,
                           uint32_t *outError)

{
  if (isSharedPackageHandle == 0) {
    g_FileSystemClose(handle);
  }
  if (outError != NULL) {
    *outError = error;
  }
  return false;
}


/* Opens an FLM movie as g_ActiveMovie: from the loose movie directory (unless MOVIE_OPEN_PACKAGE_ONLY), a
   mounted package, the executable directory or the plain path, in that order. Loads the header and the video
   stream (only its start when streaming), picks one of the embedded audio tracks at random and builds a
   MovieRuntime that looks like a one-frame gfx texture, so the ARGB frame can be drawn like any other texture.
   A partly loaded stream gets the refill worker thread. Returns true on success and stores the header's
   frame timer rate (frameIntervalMilliseconds, the value callers pass to TimerRegisterPeriodic) in
   *outPlaybackRateHz; returns false and stores the error code of the failing step in *outError. Either
   pointer may be NULL. (The original also returned frameCount in EAX on success; no caller uses it.)
*/
bool Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path,uint32_t *outPlaybackRateHz,uint32_t *outError)

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
  HANDLE workerThread;
  uint32_t streamBufferBytes;
  uint32_t runtimeBytes;
  uint32_t packedTime;
  uint32_t packedDate;
  uint32_t initialVideoBytes;
  uint32_t status;
  bool looseFileOpened;
  uint32_t openError;
  uint32_t allocError;
  void *allocPayload;
  DirectSoundVoiceSet *audioVoiceSet;
  PckEntryHeader *packageEntry;
  EngineFileHandle packageFileHandle;
  MovieStreamByteCount remainingByteCount;
  uint8_t *loadedEnd;
  MovieStreamFileOffset streamPosition;
  MovieSharedStreamHandleFlag isSharedPackageHandle;

  isSharedPackageHandle = 0;
  looseFileOpened = false;
  if (((movieOpenFlags & MOVIE_OPEN_PACKAGE_ONLY) == 0) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    looseFileOpened = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle) == 0;
  }
  if (!looseFileOpened) {
    movieOpenFlags = movieOpenFlags & ~MOVIE_OPEN_PACKAGE_ONLY;
    packageEntry = Package_FindEntryAcrossMounts(path,&packageFileHandle);
    if ((packageEntry != NULL) &&
       (g_FileSystemSeek
            (FILESYSTEM_SEEK_BEGIN,packageEntry->runtimePayloadOffset + PCK_ENTRY_HEADER_BYTES,
             (void *)packageFileHandle) == 0)) {
      isSharedPackageHandle++;
      handle = (void *)packageFileHandle;
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      openError = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
      if (openError != 0) {
        openError = g_FileSystemOpen(0,path,&handle);
        if (openError != 0) {
          /* Nothing is open yet: no close. */
          if (outError != NULL) {
            *outError = openError;
          }
          return false;
        }
      }
    }
  }
  status = g_FileSystemReadExact(MOVIE_FILE_HEADER_BYTES,g_PackageScratchBuffer,handle);
  if (status != 0) {
    return Movie_OpenFail(handle,isSharedPackageHandle,status,outError);
  }
  header = (MovieFileHeader *)g_PackageScratchBuffer;
  if ((header->common.magic != ASSET_MAGIC_FLM) ||
      ((uint32_t)header->common.converterVersion != MOVIE_FLM_CONVERTER_VERSION)) {
    return Movie_OpenFail(handle,isSharedPackageHandle,FATAL_ERROR_MOVIE_INVALID,outError);
  }
  /* the buffer holds the header and the whole video stream, or a bounded window of it when streaming */
  streamBufferBytes = header->videoStreamBytes + MOVIE_FILE_HEADER_BYTES;
  if ((MOVIE_STREAM_BUFFER_MAX_BYTES < streamBufferBytes) && (movieOpenFlags != 0)) {
    streamBufferBytes = MOVIE_STREAM_BUFFER_MAX_BYTES;
  }
  allocError = g_MemoryApi.alloc(streamBufferBytes,&allocPayload);
  if (allocError != 0) {
    return Movie_OpenFail(handle,isSharedPackageHandle,allocError,outError);
  }
  copySource = (uint32_t *)g_PackageScratchBuffer;
  copyDestination = (uint32_t *)allocPayload;
  for (copyCount = MOVIE_FILE_HEADER_BYTES / 4; copyCount != 0; copyCount--) {
    *copyDestination = *copySource;
    copySource++;
    copyDestination++;
  }
  header = (MovieFileHeader *)allocPayload;
  initialVideoBytes = header->videoStreamBytes;
  if ((MOVIE_INITIAL_VIDEO_MAX_BYTES < initialVideoBytes) && (movieOpenFlags != 0)) {
    initialVideoBytes = MOVIE_INITIAL_VIDEO_MAX_BYTES;
  }
  remainingByteCount = header->videoStreamBytes - initialVideoBytes;
  loadedEnd = (uint8_t *)(header + 1) + initialVideoBytes;
  status = g_FileSystemReadExact(initialVideoBytes,header + 1,handle);
  if (status != 0) {
    g_MemoryApi.free(header);
    return Movie_OpenFail(handle,isSharedPackageHandle,status,outError);
  }
  if (!g_FileSystemGetPosition(handle,&streamPosition)) {
    /* a failed position query fails the open with the position value as the error (0, JC 0x004a89f1) */
    g_MemoryApi.free(header);
    return Movie_OpenFail(handle,isSharedPackageHandle,streamPosition,outError);
  }
  status = Movie_OpenLoadRandomAudioTrack(header,remainingByteCount,handle,&audioVoiceSet);
  if (status != 0) {
    g_MemoryApi.free(header);
    return Movie_OpenFail(handle,isSharedPackageHandle,status,outError);
  }
  runtimeBytes = header->widthPixels * header->heightPixels * 4 + MOVIE_RUNTIME_PIXELS_OFFSET;
  allocError = g_MemoryApi.alloc(runtimeBytes,&allocPayload);
  if (allocError != 0) {
    g_MemoryApi.free(header);
    return Movie_OpenFail(handle,isSharedPackageHandle,allocError,outError);
  }
  movie = (MovieRuntime *)allocPayload;
  g_ActiveMovie = movie;
  if ((isSharedPackageHandle == 0) && (remainingByteCount == 0)) {
    g_FileSystemClose(handle);
  }
  movie->textureCommon.magic = ASSET_MAGIC_GFX;
  movie->textureCommon.allocationSizeBytes = runtimeBytes;
  movie->textureCommon.formatVersion = 1;
  movie->textureCommon.converterVersion = 0;
  movie->audioVoiceSet = audioVoiceSet;
  movie->activeAudioBuffer = NULL;
  frameWidth = header->widthPixels;
  frameHeight = header->heightPixels;
  packedTime = g_LocaleGetPackedCurrentTime();
  movie->textureCommon.buildMetadata.timestamps.timeValue0 = packedTime;
  movie->textureCommon.buildMetadata.timestamps.timeValue1 = packedTime;
  movie->textureCommon.buildMetadata.timestamps.timeValue2 = packedTime;
  packedDate = g_LocaleGetPackedCurrentDate();
  movie->textureCommon.buildMetadata.timestamps.dateValue0 = packedDate;
  movie->textureCommon.buildMetadata.timestamps.dateValue1 = packedDate;
  movie->textureCommon.buildMetadata.timestamps.dateValue2 = packedDate;
  g_LocaleCopyDefaultComputerLabelUtf16(movie->textureCommon.buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(movie->textureCommon.buildMetadata.names.sourceName);
  movie->unusedText[0] = 0;
  movie->subresourceTableOffset = offsetof(MovieRuntime,sourceEntry); /* right after the gfx header */
  movie->paletteBankCount = 0;
  movie->subresourceCount = 1;
  movie->fileHeader = header;
  movie->currentFrameIndex = 0;
  movie->videoStreamOffset = MOVIE_FILE_HEADER_BYTES;
  movie->sourceEntry.dataOffset = MOVIE_RUNTIME_PIXELS_OFFSET;
  movie->sourceEntry.pixelWidth = frameWidth;
  movie->sourceEntry.pixelHeight = frameHeight;
  movie->sourceEntry.logicalWidth = frameWidth;
  movie->sourceEntry.logicalHeight = frameHeight;
  movie->sourceEntry.paletteIndex = -1;
  movie->sourceEntry.originX = 0;
  movie->sourceEntry.originY = 0;
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
  movie->refillSemaphore = NULL;
  if ((remainingByteCount != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
    movie->workerActive++;
    movie->refillSemaphore = CreateSemaphoreA(NULL,0,1,NULL);
    /* The original passes the address of its remainingByteCount local as lpThreadId. */
    workerThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Movie_StreamWorkerThread,NULL,0,
                                &remainingByteCount);
    if (workerThread == NULL) {
      movie->workerActive--;
    }
    else {
      CloseHandle(workerThread);
    }
  }
  if (outPlaybackRateHz != NULL) {
    *outPlaybackRateHz = header->frameIntervalMilliseconds;
  }
  return true;
}


/* Returns the frame size of the active movie, so callers can place and scale the movie texture. Both are
   zero when no movie is open.
*/
MovieFrameDimensions Movie_GetFrameDimensions(void)

{
  MovieFrameDimensions dimensions;

  dimensions.width = 0;
  dimensions.height = 0;
  if (g_ActiveMovie != NULL) {
    dimensions.width = (g_ActiveMovie->sourceEntry).pixelWidth;
    dimensions.height = (g_ActiveMovie->sourceEntry).pixelHeight;
  }
  return dimensions;
}


/* Sets the Q15 volume the active movie's soundtrack starts with (Movie_AdvanceFrame plays it on the first frame
   with this gain on both channels). Does nothing when no movie is open.
*/
void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15)

{
  if (g_ActiveMovie != NULL) {
    g_ActiveMovie->audioGainQ15 = gainQ15;
  }
  return;
}


/* Background thread of a streamed movie: whenever Movie_AdvanceFrame signals the refill semaphore (or every
   256 ms), appends the next MOVIE_REFILL_CHUNK_BYTES of video to the buffer while it stays below
   MOVIE_REFILL_LIMIT_BYTES, so playback does not stall on disk reads. Ends when the movie is closed, fully
   loaded or a read fails (MOVIE_STREAM_READ_FAILED), and clears workerActive on the way out.
*/
uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext)

{
  void *handle;
  MovieRuntime *movie;
  uint32_t byteCount;

  /* The original keeps the movie in ESI: it re-reads g_ActiveMovie only at the loop top, after the wait
     and at the exit. */
  while ((movie = g_ActiveMovie) != NULL) {
    MsgWaitForMultipleObjects(1,&movie->refillSemaphore,FALSE,256,0);
    movie = g_ActiveMovie;
    if ((movie == NULL) || (movie->streamState == MOVIE_STREAM_SHUTDOWN) ||
        (movie->workerActive == 0) || (movie->remainingVideoBytes == 0)) break;
    if (movie->streamState == MOVIE_STREAM_IDLE) continue;
    byteCount = movie->remainingVideoBytes;
    if ((uint32_t)(movie->loadedVideoEnd - (uint8_t *)movie->fileHeader) < MOVIE_REFILL_LIMIT_BYTES) {
      handle = movie->streamHandle;
      if (MOVIE_REFILL_CHUNK_BYTES < byteCount) {
        byteCount = MOVIE_REFILL_CHUNK_BYTES;
      }
      g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,movie->streamFileOffset,handle);
      if (g_FileSystemReadExact(byteCount,movie->loadedVideoEnd,handle) != 0) {
        if (movie->streamState != MOVIE_STREAM_SHUTDOWN) {
          movie->streamState = MOVIE_STREAM_READ_FAILED;
        }
        break;
      }
      movie->remainingVideoBytes = movie->remainingVideoBytes - byteCount;
      movie->streamFileOffset = movie->streamFileOffset + byteCount;
      movie->loadedVideoEnd = movie->loadedVideoEnd + byteCount;
      /* a loose file is closed once fully read; a package handle stays open for other entries */
      if ((movie->remainingVideoBytes == 0) && (movie->streamHandleIsSharedPackage == 0)) {
        g_FileSystemClose(handle);
      }
    }
    if ((movie->streamState == MOVIE_STREAM_SHUTDOWN) || (movie->remainingVideoBytes == 0)) break;
    movie->streamState = MOVIE_STREAM_IDLE;
  }
  if (g_ActiveMovie != NULL) {
    g_ActiveMovie->workerActive = 0;
  }
  return 0;
}


/* Resets currentFrameIndex and videoStreamOffset of g_ActiveMovie to the first frame and stops its audio voice,
   so the movie plays again from the start. It does not rebuild a discarded streaming prefix. Called by
   FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState when the movie of a frontend page has ended (when the
   page enables movie playback), which makes it loop.
*/
void Movie_Rewind(void)

{
  MovieRuntime *activeMovie;
  
  activeMovie = g_ActiveMovie;
  if (g_ActiveMovie != NULL) {
    g_ActiveMovie->currentFrameIndex = 0;
    activeMovie->videoStreamOffset = MOVIE_FILE_HEADER_BYTES; /* the first frame follows the header */
    if (activeMovie->activeAudioBuffer != NULL) {
      g_SoundStopVoice(activeMovie->activeAudioBuffer);
      activeMovie->activeAudioBuffer = NULL;
    }
  }
  return;
}

/* Closes g_ActiveMovie. With the arena allocator a refill worker may run: it is told to stop and waited for
   (with the process dropped from real-time to normal priority so the worker gets CPU time while this thread
   spins), then the semaphore is closed. Frees the FLM buffer, the soundtrack voice set, a still-open own
   stream handle and the MovieRuntime.
*/
void Movie_Close(void)

{
  MovieRuntime *movie;
  HANDLE currentProcessHandle;
  HANDLE hProcess;

  movie = g_ActiveMovie;
  if (g_ActiveMovie != NULL) {
    if (g_MemoryApi.alloc == ArenaHeap_Alloc) {
      g_ActiveMovie->streamState = MOVIE_STREAM_SHUTDOWN;
      currentProcessHandle = GetCurrentProcess();
      SetPriorityClass(currentProcessHandle,NORMAL_PRIORITY_CLASS);
      do {
      } while (movie->workerActive != 0);
      if (movie->refillSemaphore != NULL) {
        CloseHandle(movie->refillSemaphore);
        movie->refillSemaphore = NULL;
      }
      hProcess = GetCurrentProcess();
      SetPriorityClass(hProcess,REALTIME_PRIORITY_CLASS);
    }
    g_ActiveMovie = NULL;
    g_MemoryApi.free(movie->fileHeader);
    if (movie->audioVoiceSet != NULL) {
      g_SoundReleaseSampleVoiceSet(movie->audioVoiceSet);
    }
    if ((movie->remainingVideoBytes != 0) && (movie->streamHandleIsSharedPackage == 0)) {
      g_FileSystemClose(movie->streamHandle);
    }
    g_MemoryApi.free(movie);
  }
  return;
}


/* Update callback of the end-movie UI in a network game: keeps the frontend session alive while the end
   movie plays by running the session tick of the local role. Does nothing in a local game. Note that the
   SESSION_NETWORK_ROLE_CLIENT bit selects the host tick and the HOST bit the client tick: either the enum or
   the two tick functions are named the wrong way round.
*/
void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendHostSession_TickPeerTimeoutsAndDropPlayers();
    }
  }
  else {
    FrontendClientSession_TickHostTimeout();
  }
  return;
}


/* Keyboard handler of the end-movie UI: looks the key up in the end-movie command table, whose records also
   say which Ctrl/Alt combination they need, and runs the matching action: save a numbered PCX screenshot, or
   skip the end movie (marks the local player done with the results; in a network game as a queued command).
   endMovieRuntime is the active UI root, i.e. the in-game runtime root (g_InGameRuntimeRoot) whose callbacks
   Frontend_PlaySelectedEndMovie replaced. Skipping is ignored while its resultsContinueButton is suppressed
   (a network host still waiting for its clients, or the local player already marked ready; see
   FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton).
*/
void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime)

{
  /* Rewritten from the assembly (0x00565810-0x00565A29). The decompiled version jumped to the
     continuation labels inside the original machine code. EBX is the end-movie runtime (the in-game root). */
  UiCommandDispatchRecord *record = g_EndMovieCommandDispatchRecords;
  uint32_t target = 0;

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    /* the record's modifier class demands exactly none, Ctrl, Alt or Ctrl+Alt (Shift is ignored) */
    if (flags == 0) {
      if ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) continue;
    }
    else if ((flags & KEYBOARD_STATE_ALT) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
    }
    else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    else {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5658f0: { /* screenshot */
    GraphicsCapturedTextureSourceAsset *capture =
         g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
    void *pcxBytes;
    uint32_t pcxByteCount;
    uint32_t pcxError;
    uint16_t *digitHigh = &g_ScreenshotFileNameUtf16[6];
    uint16_t *digitLow = &g_ScreenshotFileNameUtf16[7];
    if (capture == NULL) {
      break;
    }
    if (!Pcx_EncodeCapture(capture,&pcxBytes,&pcxByteCount,&pcxError)) {
      g_MemoryApi.free(capture);
      break;
    }
    FileSystem_WriteBufferToPath(pcxByteCount,pcxBytes,g_ScreenshotFileNameUtf16);
    g_MemoryApi.free(pcxBytes);
    g_MemoryApi.free(capture);
    /* two-digit counter in the file name, wrapping from 99 to 00 */
    (*digitLow)++;
    if (*digitLow > '9') {
      (*digitHigh)++;
      *digitLow = *digitLow - 10;
      if (*digitHigh > '9') {
        *digitHigh = *digitHigh - 10;
      }
    }
    break;
  }
  case 0x565990: /* skip the end movie */
    /* +0x6EC bit 3: nodeFlags of the results continue button */
    if (((INGAME_UI(endMovieRuntime,resultsContinueButton)->nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
        ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0)) {
      break;
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != 0) {
      /* always true here: the direct call below is unreachable in the original as well */
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_RESULTS_READY,0,0,0);
      }
      else {
        FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(g_LocalPlayerRuntimeId);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,
                                                  UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    else {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,
                                                     UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    break;
  default:
    Thandor_Log("EndMovie dispatch: unhandled continuation %08x",target);
    break;
  }
  return;
}


/* Periodic timer callback registered at the movie's playback rate: counts one more frame that is due in
   g_IntroMoviePendingTicks. The intro loop consumes the count and decodes at most three pending frames per
   iteration, which keeps the movie in time on slow machines.
*/
void IntroMovie_TimerTick(void)

{
  g_IntroMoviePendingTicks++;
  return;
}

/* Defined further down with the delta encoder; it writes the same 8-byte colour block that the keyframe
   encoder writes inline in the original. */
static void MovieDeltaEncode_Block(uint32_t *output,const PackedRgb24 *blockPixels,uint32_t rowStridePixels);

/* Encodes a whole frame as FLM 4x4 colour blocks of 8 bytes each (no skip tokens), for the first frame in
   Movie_EncodeFlmBufferFromFrameProvider, its only caller. A block stores the chroma code of its average
   colour and 16 per-pixel luma levels above a base luma, the base being the low 5 bits (a token 0..24) of
   the first dword. A block whose luma range is below 12 uses 3-bit levels in steps of 1 above
   (min + max - 8) / 2; otherwise bit 31 of the second dword is set and the levels step by 2 above a base
   4 lower. The levels are packed from the bottom right pixel backwards. Returns the bytes written.
*/
uint32_t Movie_EncodeFrame4x4Keyframe(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels)

{
  uint32_t *outputCursor;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;

  blockRowsLeft = frameHeightPixels >> 2;
  outputCursor = encodedOutput;
  blocksLeftInRow = frameWidthPixels >> 2;
  do {
    do {
      MovieDeltaEncode_Block(outputCursor,sourcePixels,frameWidthPixels);
      sourcePixels = sourcePixels + 4;
      outputCursor = outputCursor + 2;
      blocksLeftInRow--;
    } while (blocksLeftInRow != 0);
    /* past the other three pixel rows of this block row */
    sourcePixels = sourcePixels + frameWidthPixels * 3;
    blockRowsLeft--;
    blocksLeftInRow = frameWidthPixels >> 2;
  } while (blockRowsLeft != 0);
  return (uint32_t)((uint8_t *)outputCursor - (uint8_t *)encodedOutput);
}


/* Whether a 4x4 block of the current frame differs from the same block of the reference frame under
   g_MovieDeltaRgbHighNibbleMask2Pixels (two pixels per 64-bit word, two words per block row). */
static int MovieDeltaEncode_BlockChanged(const uint8_t *currentBlock,const uint8_t *referenceBlock,int rowStrideBytes)
{
  const uint64_t *currentRow;
  const uint64_t *referenceRow;
  uint64_t changedBits = 0;
  int row;

  for (row = 0; row < 4; row++) {
    currentRow = (const uint64_t *)(currentBlock + row * rowStrideBytes);
    referenceRow = (const uint64_t *)(referenceBlock + row * rowStrideBytes);
    changedBits = changedBits |
                  (currentRow[0] & g_MovieDeltaRgbHighNibbleMask2Pixels ^ referenceRow[0]) |
                  (currentRow[1] & g_MovieDeltaRgbHighNibbleMask2Pixels ^ referenceRow[1]);
  }
  return (g_MovieDeltaRgbHighNibbleMask2Pixels & changedBits) != 0;
}

/* Copies a 4x4 block of the current frame into the reference frame. */
static void MovieDeltaEncode_CopyBlock(uint8_t *referenceBlock,const uint8_t *currentBlock,int rowStrideBytes)
{
  const uint64_t *currentRow;
  uint64_t *referenceRow;
  int row;

  for (row = 0; row < 4; row++) {
    currentRow = (const uint64_t *)(currentBlock + row * rowStrideBytes);
    referenceRow = (uint64_t *)(referenceBlock + row * rowStrideBytes);
    referenceRow[0] = currentRow[0];
    referenceRow[1] = currentRow[1];
  }
}

/* Writes the MOVIE_TOKEN_SKIP_* token for a run of skippedBlocks (> 0) unchanged blocks; returns the output
   position after it. */
static uint8_t *MovieDeltaEncode_WriteSkipToken(uint8_t *output,uint32_t skippedBlocks)
{
  if (skippedBlocks < MOVIE_SKIP_SHORT_MAX_BLOCKS + 1) {
    *output = ((char)skippedBlocks - 1) * (MOVIE_TOKEN_MASK + 1) | MOVIE_TOKEN_SKIP_SHORT;
    return output + 1;
  }
  if (skippedBlocks < MOVIE_SKIP_MEDIUM_MAX_BLOCKS + 1) {
    *(uint16_t *)output = ((short)skippedBlocks - (MOVIE_SKIP_SHORT_MAX_BLOCKS + 1)) * (MOVIE_TOKEN_MASK + 1) | MOVIE_TOKEN_SKIP_MEDIUM;
    return output + 2;
  }
  *(uint32_t *)output = (skippedBlocks - (MOVIE_SKIP_MEDIUM_MAX_BLOCKS + 1)) * (MOVIE_TOKEN_MASK + 1) | MOVIE_TOKEN_SKIP_LONG;
  return output + 4;
}

/* Widens [*minLuma, *maxLuma] (signed compare) to include luma. */
static __inline void MovieDeltaEncode_ExtendLumaRange(int luma,int *minLuma,int *maxLuma)
{
  if (luma < *minLuma) {
    *minLuma = luma;
  }
  else if (*maxLuma < luma) {
    *maxLuma = luma;
  }
}

/* Scans a 4x4 block row by row, left to right: sums its channels and finds its smallest and largest luma.
   Returns the block's average colour. */
static PackedRgb24 MovieDeltaEncode_ScanBlock(const PackedRgb24 *blockPixels,uint32_t rowStridePixels,
                                              int *outMinLuma,int *outMaxLuma)
{
  const PackedRgb24 *rowPixels;
  uint64_t channelSums = 0;
  int minLuma;
  int maxLuma;
  int row;
  int column;

  channelSums = Movie_AddRowToChannelSums(channelSums,blockPixels[0],blockPixels[1],blockPixels[2],blockPixels[3]);
  minLuma = (int)MovieColor_ComputeLuma5FromRgb888(blockPixels[0]);
  maxLuma = minLuma;
  for (column = 1; column < 4; column++) {
    MovieDeltaEncode_ExtendLumaRange((int)MovieColor_ComputeLuma5FromRgb888(blockPixels[column]),&minLuma,&maxLuma);
  }
  for (row = 1; row < 4; row++) {
    rowPixels = blockPixels + row * rowStridePixels;
    channelSums = Movie_AddRowToChannelSums(channelSums,rowPixels[0],rowPixels[1],rowPixels[2],rowPixels[3]);
    for (column = 0; column < 4; column++) {
      MovieDeltaEncode_ExtendLumaRange((int)MovieColor_ComputeLuma5FromRgb888(rowPixels[column]),&minLuma,&maxLuma);
    }
  }
  *outMinLuma = minLuma;
  *outMaxLuma = maxLuma;
  return Movie_PackChannelAverages(channelSums);
}

/* The 16 luma levels of a 4x4 block, taken from the bottom-right pixel backwards (row 3 right to left, then
   rows 2, 1 and 0): luma minus baseLuma, clamped to 0..maxLevel and shifted right by levelShift, 3 bits each.
   The first seven go to bits 18..0 of the colour word (*outColorWordLevels), the other nine to bits 31..5 of
   the luma word (*outLumaWordLevels). */
static void MovieDeltaEncode_PackLevels(const PackedRgb24 *blockPixels,uint32_t rowStridePixels,int baseLuma,
                                        int maxLevel,int levelShift,uint32_t *outLumaWordLevels,
                                        uint32_t *outColorWordLevels)
{
  const PackedRgb24 *rowPixels;
  uint32_t lumaWordLevels = 0;
  uint32_t colorWordLevels = 0;
  uint32_t field;
  int pixelIndex = 0;
  int level;
  int row;
  int column;

  for (row = 3; row >= 0; row--) {
    rowPixels = blockPixels + row * rowStridePixels;
    for (column = 3; column >= 0; column--) {
      level = (int)MovieColor_ComputeLuma5FromRgb888(rowPixels[column]) - baseLuma;
      if (level < 0) {
        level = 0;
      }
      else if (maxLevel < level) {
        level = maxLevel;
      }
      field = (uint32_t)level >> levelShift;
      if (pixelIndex < 7) {
        colorWordLevels = colorWordLevels | field << (18 - 3 * pixelIndex);
      }
      else {
        lumaWordLevels = lumaWordLevels | field << (29 - 3 * (pixelIndex - 7));
      }
      pixelIndex++;
    }
  }
  *outLumaWordLevels = lumaWordLevels;
  *outColorWordLevels = colorWordLevels;
}

/* Encodes one changed 4x4 block into two output words as Movie_EncodeFrame4x4Keyframe does: base luma and
   levels in the first, chroma code and levels in the second. A block whose luma range is below 12 gets 3-bit
   levels in single steps, any other one levels 0..MOVIE_BLOCK_WIDE_LEVEL_MAX halved (MOVIE_BLOCK_DOUBLE_STEPS)
   above a base lowered by 4. */
static void MovieDeltaEncode_Block(uint32_t *output,const PackedRgb24 *blockPixels,uint32_t rowStridePixels)
{
  PackedRgb24 averageColor;
  uint32_t chromaCode;
  uint32_t lumaWordLevels;
  uint32_t colorWordLevels;
  int minLuma;
  int maxLuma;
  int baseLuma;

  averageColor = MovieDeltaEncode_ScanBlock(blockPixels,rowStridePixels,&minLuma,&maxLuma);
  baseLuma = ((minLuma - 8) + maxLuma) >> 1;
  if (baseLuma < 0) {
    baseLuma = 0;
  }
  else if (MOVIE_TOKEN_BASE_LUMA_MAX < baseLuma) {
    baseLuma = MOVIE_TOKEN_BASE_LUMA_MAX;
  }
  if (maxLuma - minLuma < 12) {
    chromaCode = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
    MovieDeltaEncode_PackLevels(blockPixels,rowStridePixels,baseLuma,7,0,&lumaWordLevels,&colorWordLevels);
    output[0] = (uint32_t)baseLuma | lumaWordLevels;
    output[1] = (chromaCode & MOVIE_COLOR_CHROMA_MASK) << 16 | colorWordLevels;
  }
  else {
    baseLuma = baseLuma - 4;
    if (baseLuma < 0) {
      baseLuma = 0;
    }
    else if (16 < baseLuma) {
      baseLuma = 16;
    }
    chromaCode = MovieColor_ComputeChromaCodeFromRgb888(averageColor);
    MovieDeltaEncode_PackLevels(blockPixels,rowStridePixels,baseLuma,MOVIE_BLOCK_WIDE_LEVEL_MAX,1,&lumaWordLevels,
                                &colorWordLevels);
    output[0] = (uint32_t)baseLuma | lumaWordLevels;
    output[1] = (chromaCode & MOVIE_COLOR_CHROMA_MASK) * (1 << 16) + MOVIE_BLOCK_DOUBLE_STEPS | colorWordLevels;
  }
}

/* Encodes currentFramePixels as an FLM delta frame against previousFramePixels (called by
   Movie_EncodeFlmBufferFromFrameProvider for every frame after the first). A 4x4 block that does not differ
   from the reference under g_MovieDeltaRgbHighNibbleMask2Pixels is skipped, runs of skipped blocks being
   written as MOVIE_TOKEN_SKIP_* tokens; a changed block is copied into the reference, so the reference keeps
   up with what the decoder shows, and encoded as in Movie_EncodeFrame4x4Keyframe. Returns the bytes written,
   rounded up to a multiple of 8.
*/
uint32_t Movie_EncodeFrame4x4Delta(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels)

{
  int rowStrideBytes;
  const uint8_t *currentBlock;
  uint8_t *referenceBlock;
  uint8_t *outputCursor;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;
  uint32_t pendingSkipCount;

  rowStrideBytes = frameWidthPixels * 4;
  blockRowsLeft = frameHeightPixels >> 2;
  pendingSkipCount = 0;
  currentBlock = (const uint8_t *)currentFramePixels;
  referenceBlock = (uint8_t *)previousFramePixels;
  outputCursor = (uint8_t *)encodedOutput;
  do {
    blocksLeftInRow = frameWidthPixels >> 2;
    do {
      if (!MovieDeltaEncode_BlockChanged(currentBlock,referenceBlock,rowStrideBytes)) {
        pendingSkipCount++;
      }
      else {
        if (pendingSkipCount != 0) {
          outputCursor = MovieDeltaEncode_WriteSkipToken(outputCursor,pendingSkipCount);
          pendingSkipCount = 0;
        }
        MovieDeltaEncode_CopyBlock(referenceBlock,currentBlock,rowStrideBytes);
        MovieDeltaEncode_Block((uint32_t *)outputCursor,(const PackedRgb24 *)currentBlock,frameWidthPixels);
        outputCursor = outputCursor + 8;
      }
      currentBlock = currentBlock + 16;
      referenceBlock = referenceBlock + 16;
      blocksLeftInRow--;
    } while (blocksLeftInRow != 0);
    /* past the other three pixel rows of this block row */
    currentBlock = currentBlock + frameWidthPixels * 12;
    referenceBlock = referenceBlock + frameWidthPixels * 12;
    blockRowsLeft--;
  } while (blockRowsLeft != 0);
  if (pendingSkipCount != 0) {
    outputCursor = MovieDeltaEncode_WriteSkipToken(outputCursor,pendingSkipCount);
  }
  return (uint32_t)(outputCursor - (uint8_t *)encodedOutput + 7) & ~7u;
}


/* Not in the original (split out of Movie_AdvanceFrame): stores endCode in *outEndCode when given and
   returns false, the "no frame" result of Movie_AdvanceFrame. */
static bool Movie_ReportAdvanceEnd(uint32_t *outEndCode,uint32_t endCode)
{
  if (outEndCode != NULL) {
    *outEndCode = endCode;
  }
  return false;
}

/* Not in the original (split out of Movie_AdvanceFrame): once the read position of a streamed movie is a
   whole MOVIE_COMPACT_SHIFT_BYTES past the header, moves the unplayed bytes down by that shift (a REP MOVSD
   in the original) to make room for further refills. */
static void Movie_CompactStreamBuffer(MovieRuntime *movie)
{
  uint32_t readOffset;
  uint32_t loadedSize;
  uint32_t dwordsLeft;
  uint32_t *copySource;
  uint32_t *copyDestination;

  readOffset = movie->videoStreamOffset;
  loadedSize = (uint32_t)(movie->loadedVideoEnd - (uint8_t *)movie->fileHeader);
  if ((MOVIE_COMPACT_SHIFT_BYTES + MOVIE_FILE_HEADER_BYTES - 1 < readOffset) && (readOffset < loadedSize)) {
    movie->videoStreamOffset = movie->videoStreamOffset - MOVIE_COMPACT_SHIFT_BYTES;
    copyDestination = (uint32_t *)((uint8_t *)movie->fileHeader + readOffset - MOVIE_COMPACT_SHIFT_BYTES);
    movie->loadedVideoEnd = movie->loadedVideoEnd - MOVIE_COMPACT_SHIFT_BYTES;
    copySource = (uint32_t *)((uint8_t *)copyDestination + MOVIE_COMPACT_SHIFT_BYTES);
    for (dwordsLeft = (loadedSize - readOffset) >> 2; dwordsLeft != 0; dwordsLeft--) {
      *copyDestination = *copySource;
      copySource++;
      copyDestination++;
    }
  }
}

/* Decodes the next frame of g_ActiveMovie into its ARGB image, returns true and stores the movie in *outMovie.
   Asks the worker for more data when the buffer has room, starts the soundtrack with the first frame, and
   waits (returns true without decoding) while a streamed movie has less than one refill chunk buffered. A
   streamed movie drops played bytes from the buffer front in MOVIE_COMPACT_SHIFT_BYTES steps. Returns false
   after the last frame, on a read failure of the worker or when no movie is open; *outEndCode then gets
   FATAL_ERROR_MOVIE_INVALID (no movie / read failure) or the unplayed bytes left in the buffer (after the
   last frame). Either output may be NULL; only the one for the returned case is written.
   Original quirk: after a worker read failure it closes the caller's leftover EBX instead of the stream
   handle (0x004A8BDD); the C closes NULL, which has the same effect on the movie (see the body).
*/
bool Movie_AdvanceFrame(MovieRuntime **outMovie,uint32_t *outEndCode)

{
  MovieFileHeader *flmHeader;
  MovieFrameIndex previousFrameIndex;
  MovieFrameIndex nextFrameIndex;
  MovieRuntime *movie;
  uint32_t bufferedBytes;
  uint32_t consumedBytes;
  uint8_t *streamCursor;
  IDirectSoundBuffer *playedVoice;

  movie = g_ActiveMovie;
  if (movie == NULL) {
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  if (movie->streamState == MOVIE_STREAM_READ_FAILED) {
    /* 0x004A8BDD PUSH EBX; CALL g_FileSystemClose. On this path the function never loads EBX, so the
       original closes whatever EBX its caller left there -- never the movie stream handle: a UI/runtime
       object pointer in the frontend/in-game/briefing callers, g_FramebufferHeight in the
       Game_PlayIntroMovies frame loop (0x00573B2C MOV EBX,ECX), the outer caller's EBX via
       MoviePlayback_AdvanceToFrameAndPresent. Closing NULL keeps the effect (the stream handle stays
       open; remainingVideoBytes = 0 also keeps Movie_Close from closing it) without the stray
       CloseHandle on an unrelated value. */
    g_FileSystemClose(NULL);
    movie->remainingVideoBytes = 0;
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  if ((movie->streamState == MOVIE_STREAM_IDLE) && (movie->workerActive != 0) &&
      (movie->remainingVideoBytes != 0) &&
      ((uint32_t)(movie->loadedVideoEnd - (uint8_t *)movie->fileHeader) < MOVIE_REFILL_LIMIT_BYTES)) {
    movie->streamState = MOVIE_STREAM_FILL_REQUESTED;
    ReleaseSemaphore(movie->refillSemaphore,1,NULL);
  }
  flmHeader = movie->fileHeader;
  previousFrameIndex = movie->currentFrameIndex;
  streamCursor = (uint8_t *)flmHeader + movie->videoStreamOffset;
  if ((previousFrameIndex == 0) && (movie->audioVoiceSet != NULL)) {
    /* stored whether or not it plays (NULL on failure) */
    g_SoundPlayOneShot(movie->audioGainQ15,movie->audioGainQ15,movie->audioVoiceSet,&playedVoice);
    movie->activeAudioBuffer = playedVoice;
  }
  nextFrameIndex = previousFrameIndex + 1;
  bufferedBytes = (uint32_t)(movie->loadedVideoEnd - streamCursor);
  if (nextFrameIndex > flmHeader->frameCount) {
    /* past the last frame: the end code is the unplayed byte count */
    return Movie_ReportAdvanceEnd(outEndCode,bufferedBytes);
  }
  if ((movie->remainingVideoBytes != 0) && (bufferedBytes < MOVIE_REFILL_CHUNK_BYTES)) {
    /* Not enough bytes buffered yet: success without decoding. Original quirk: the original returns
       ESI - 0x220 here (0x004A8BD0 LEA EAX,[ESI-0x220]) because ESI is only advanced to the pixels at
       0x004A8B48. Callers keep the value as the movie only after the first-frame call, which cannot
       get here (with remainingVideoBytes != 0 the first 0x3A2000 bytes are loaded). */
    if (outMovie != NULL) {
      *outMovie = (MovieRuntime *)((uint8_t *)movie - MOVIE_RUNTIME_PIXELS_OFFSET);
    }
    return true;
  }
  DebugHook_MovieBeforeDecode(movie,flmHeader->heightPixels,flmHeader->widthPixels,streamCursor);
  consumedBytes = Movie_DecodeFrame4x4Delta
                    (flmHeader->heightPixels,flmHeader->widthPixels,movie->argbPixels,streamCursor);
  DebugHook_MovieAfterDecode(movie,flmHeader->heightPixels,flmHeader->widthPixels,consumedBytes);
  movie->currentFrameIndex = nextFrameIndex;
  movie->videoStreamOffset = movie->videoStreamOffset + consumedBytes;
  DebugHook_MovieFrameDone(movie,consumedBytes);
  if ((movie->openFlags != 0) && (movie->streamState == MOVIE_STREAM_IDLE)) {
    Movie_CompactStreamBuffer(movie);
  }
  if (outMovie != NULL) {
    *outMovie = movie;
  }
  return true;
}


/* Decodes movie frames until g_MoviePlaybackCurrentFrame reaches targetFrame, then redraws the whole UI (which
   shows the movie texture) and presents it once. Stops without drawing, and without updating the frame
   counter, when the movie ends or cannot deliver a frame.
*/
void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame)

{
  uint32_t frameIndex;

  frameIndex = g_MoviePlaybackCurrentFrame;
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      frameIndex++;
      if (!Movie_AdvanceFrame(NULL,NULL)) {
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


/* Not in the original (split out of Movie_DecodeFrame4x4Delta, which unrolls it twice): draws one 4x4 colour
   block from its two stream dwords. Table row = chroma code (second dword bits 21-30) * 32 + base luma (first
   dword bits 0-4). The sixteen 3-bit luma steps, in row order, sit at bits 5-31 of the first dword (steps
   0-8) and bits 0-20 of the second (steps 9-15). With bit 31 of the second dword set every step counts twice
   (luma range 0..14 instead of 0..7). */
static void Movie_DecodeColorBlock(uint32_t *blockTopLeft,MoviePixelDimension widthPixels,uint32_t blockWord0,
                                   uint32_t blockWord1)
{
  uint32_t tableIndex;
  uint32_t lumaStepScale;
  uint32_t lumaStep;
  uint32_t pixelIndex;
  uint32_t *destinationRow;

  tableIndex = (blockWord1 & MOVIE_COLOR_CHROMA_MASK << 16) >> 16 | blockWord0 & MOVIE_TOKEN_MASK;
  lumaStepScale = ((int)blockWord1 < 0) ? 2 : 1;
  destinationRow = blockTopLeft;
  for (pixelIndex = 0; pixelIndex < 16; pixelIndex++) {
    if (pixelIndex < 9) {
      lumaStep = blockWord0 >> (5 + pixelIndex * 3) & 7;
    }
    else {
      lumaStep = blockWord1 >> ((pixelIndex - 9) * 3) & 7;
    }
    destinationRow[pixelIndex & 3] = g_MovieChromaLumaToArgb[0][tableIndex + lumaStep * lumaStepScale];
    if ((pixelIndex & 3) == 3) {
      destinationRow = destinationRow + widthPixels;
    }
  }
}

/* Decodes one FLM frame over the previous one in the ARGB image, 4x4 blocks in row order. A colour block
   (token 0..24 = base luma) holds a 10-bit chroma code and sixteen 3-bit luma steps that index
   g_MovieChromaLumaToArgb; the skip tokens leave runs of blocks unchanged. Returns the encoded bytes consumed,
   rounded up to eight, so the caller can advance the stream.
*/
uint32_t Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          uint8_t *encodedFrame)

{
  uint32_t blockWord0;
  uint32_t token;
  uint8_t *streamCursor;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;
  uint32_t skipRemaining;

  blockRowsLeft = heightPixels >> 2;
  skipRemaining = 0;
  streamCursor = encodedFrame;
  blocksLeftInRow = widthPixels >> 2;
  do {
    do {
      if (skipRemaining != 0) {
        skipRemaining--;
      }
      else {
        blockWord0 = *(uint32_t *)streamCursor;
        token = blockWord0 & MOVIE_TOKEN_MASK;
        if (token < MOVIE_TOKEN_SKIP_SHORT) {
          Movie_DecodeColorBlock(destinationArgb,widthPixels,blockWord0,*(uint32_t *)(streamCursor + 4));
          streamCursor = streamCursor + 8;
        }
        /* skip tokens: this block plus skipRemaining further blocks keep the previous frame */
        else if (token == MOVIE_TOKEN_SKIP_SHORT) {
          streamCursor = streamCursor + 1;
          skipRemaining = (blockWord0 & 0xff) >> 5;
        }
        else if (token < MOVIE_TOKEN_SKIP_LONG) {
          streamCursor = streamCursor + 2;
          skipRemaining = ((blockWord0 & 0xffff) >> 5) + MOVIE_SKIP_SHORT_MAX_BLOCKS;
        }
        else {
          streamCursor = streamCursor + 4;
          skipRemaining = (blockWord0 >> 5) + MOVIE_SKIP_MEDIUM_MAX_BLOCKS;
        }
      }
      destinationArgb = destinationArgb + 4;
      blocksLeftInRow--;
    } while (blocksLeftInRow != 0);
    /* the block loop left destinationArgb on the first row of the block row; move to the next block row */
    destinationArgb = destinationArgb + widthPixels * 3;
    blockRowsLeft--;
    blocksLeftInRow = widthPixels >> 2;
  } while (blockRowsLeft != 0);
  return (uint32_t)(streamCursor - encodedFrame + 7) & ~7u;
}


/* FLM chroma code of a colour for the block encoders, already shifted left by 5 so the 5-bit luma fits below
   it: saturation in bits 10-14 and hue in bits 5-9, from the length and angle of the opponent-colour vector
   ((blue - green) * sqrt(3), green + blue - 2 * red), both scaled by 0x8000. The bytes of PackedRgb24 are
   blue, green, red from the lowest. Called by Movie_EncodeFrame4x4Keyframe and Movie_EncodeFrame4x4Delta.
*/
uint32_t MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888)

{
  uint32_t green;
  FixedLengthAngle angleAndLength;
  
  green = rgb888 >> 8 & ARGB8888_CHANNEL_MASK;
  /* 0xDDB4 = 0x8000 * sqrt(3) */
  angleAndLength = FixedMath_Vector2AngleAndLength
                    (((rgb888 & ARGB8888_CHANNEL_MASK) - green) * MOVIE_CHROMA_SQRT3_Q15,
                     (green + (rgb888 & ARGB8888_CHANNEL_MASK) + (rgb888 >> 16 & ARGB8888_CHANNEL_MASK) * -2) * (1 << 15));
  return angleAndLength.length >> 9 & MOVIE_COLOR_SATURATION_MASK | angleAndLength.angle >> 6 & MOVIE_COLOR_HUE_MASK;
}


/* FLM luma of a colour for the block encoders: the channel sum divided by 24 (the average divided by 8),
   rounded: ((red + green + blue) * 0x5555 + 2^18) >> 19. A channel sum of 757 or more (near white) gives
   32, one more than 5 bits; the encoders clamp their per-pixel levels, so it never reaches the stream. Called by
   Movie_EncodeFrame4x4Keyframe and Movie_EncodeFrame4x4Delta.
*/
uint32_t MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888)

{
  return (((rgb888 & ARGB8888_CHANNEL_MASK) + (rgb888 >> 8 & ARGB8888_CHANNEL_MASK) + (rgb888 >> 16 & ARGB8888_CHANNEL_MASK)) * MOVIE_LUMA_THIRD_Q16 + (1 << 18)) >> 19;
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

/* Not in the original: clamps one colour channel of a table entry to 0..255. */
static uint32_t MovieColor_ClampChannel(int value)
{
  return value < 0 ? 0 : (value > 255 ? 255 : (uint32_t)value);
}

/* Not in the original: fills g_MovieChromaLumaToArgb (the 1024 chroma codes x 32 lumas that
   Movie_DecodeFrame4x4Delta looks up) from the two tables above. Called once at startup by WinMain (src/platform/bootstrap/main.c),
   before any movie is decoded. */
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
        row[luma] = ARGB8888_ALPHA_MASK | MovieColor_ClampChannel(grey - 2 * cosTerm) << 16 |
                    MovieColor_ClampChannel(grey + cosTerm - sinTerm) << 8 |
                    MovieColor_ClampChannel(grey + cosTerm + sinTerm);
      }
    }
  }
}
