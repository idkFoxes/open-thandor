/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/playback.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/playback.h>
#include <algorithm>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(4) MovieAudioGainQ15 g_MovieDefaultAudioGainQ15 = 32768;

THANDOR_ALIGN(16) MovieAudioGainQ15 g_MovieAlternateAudioGainQ15 = 32768;

MovieRuntime *g_ActiveMovie = nullptr;

/* Largest frame width and height Movie_Open accepts (the stock movies are at most 800x600) */
#define MOVIE_MAX_FRAME_DIMENSION 4096

/* Not in the original: streamState and workerActive are shared between the main thread and the refill worker.
   They are accessed as atomics through std::atomic_ref, because MovieRuntime has a fixed layout (it doubles as
   a gfx texture). The original used plain fields. */
static std::atomic_ref<MovieStreamState> Movie_StreamState(MovieRuntime *movie)
{
  return std::atomic_ref<MovieStreamState>(movie->streamState);
}

static std::atomic_ref<MovieWorkerActiveFlag> Movie_WorkerActive(MovieRuntime *movie)
{
  return std::atomic_ref<MovieWorkerActiveFlag>(movie->workerActive);
}

/* Not in the original: the size of the FLM buffer Movie_Open allocates (header plus video stream, capped when
   streaming). Movie_Open has checked that the sum fits in 32 bits. */
static uint32_t Movie_StreamBufferBytes(const MovieFileHeader *header,MovieOpenFlags openFlags)
{
  uint32_t bufferBytes = header->videoStreamBytes + MOVIE_FILE_HEADER_BYTES;

  if ((MOVIE_STREAM_BUFFER_MAX_BYTES < bufferBytes) && (openFlags != 0)) {
    bufferBytes = MOVIE_STREAM_BUFFER_MAX_BYTES;
  }
  return bufferBytes;
}

/* Not in the original: checks the FLM header values Movie_Open sizes its buffers with. entryBytes is the size of
   the file or package entry (0 when unknown). The original trusted them. Bounded here because they come from the
   file: the 32-bit size sums could wrap (a too small buffer that the read then overruns), a width or height
   below 4 wraps the decoder's block counters, and a video stream longer than the file cannot be read. */
static Bool8 Movie_IsHeaderValid(const MovieFileHeader *header,uint32_t entryBytes)
{
  uint64_t videoEnd = (uint64_t)header->videoStreamBytes + MOVIE_FILE_HEADER_BYTES;

  if ((header->widthPixels < 4) || (header->heightPixels < 4) ||
      (MOVIE_MAX_FRAME_DIMENSION < header->widthPixels) || (MOVIE_MAX_FRAME_DIMENSION < header->heightPixels) ||
      (UINT32_MAX < videoEnd) || ((entryBytes != 0) && (entryBytes < videoEnd))) {
    Thandor_Log("Movie_Open: invalid FLM header (%ux%u pixels, %u video bytes, entry %u bytes); movie skipped",
                (unsigned)header->widthPixels,(unsigned)header->heightPixels,(unsigned)header->videoStreamBytes,
                (unsigned)entryBytes);
    return false;
  }
  return true;
}

/* Part of Movie_Open: picks one of the header's audio tracks at random and turns it
   into a sample voice set. The stream stands right after the initially loaded video bytes; the audio tracks
   follow the whole video stream. Returns 0 on success and stores the voice set in *outVoiceSet (NULL when the
   header has no usable track); otherwise returns the error of the failing seek/alloc/read/voice-set call. The
   loaded sample is freed on both the success and the failure path, as in the original. */
static uint32_t
Movie_OpenLoadRandomAudioTrack(MovieFileHeader *header,MovieStreamByteCount remainingVideoBytes,void *handle,
                               SoundVoiceSet **outVoiceSet)

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
  SoundVoiceSet *voiceSet;

  *outVoiceSet = nullptr;
  audioTrackCount = header->audioTrackCount;
  if ((audioTrackCount == 0) || (MOVIE_MAX_AUDIO_TRACKS < audioTrackCount)) {
    return 0;
  }
  selectedTrack = 0;
  if (1 < audioTrackCount) {
    selectedTrack = (Random_NextPrimary() & 0xffff) % audioTrackCount; /* unsigned division */
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
    loadError = g_SoundCreateSampleVoiceSet(static_cast<SoundSampleAsset *>(audioSample),&voiceSet);
    if (loadError == 0) {
      *outVoiceSet = voiceSet;
    }
  }
  g_MemoryApi.free(audioSample);
  return loadError;
}

static uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

/* Failure exit of Movie_Open once a file or package handle is open: closes the handle unless it is a shared
   package handle, stores error in *outError (when not NULL) and returns false. */
static Bool8 Movie_OpenFail(void *handle,MovieSharedStreamHandleFlag isSharedPackageHandle,uint32_t error,
                           uint32_t *outError)

{
  if (isSharedPackageHandle == 0) {
    g_FileSystemClose(handle);
  }
  if (outError != nullptr) {
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
   pointer may be NULL. (The original also returned frameCount on success; no caller uses it.)
*/
Bool8 Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path,uint32_t *outPlaybackRateHz,uint32_t *outError)

{
  MovieFileHeader *header;
  MovieRuntime *movie;
  void *handle;
  uint32_t *copySource;
  uint32_t *copyDestination;
  MovieSubresourceCount frameWidth;
  MoviePaletteBankCount frameHeight;
  MovieAudioGainQ15 defaultAudioGain;
  HANDLE workerThread;
  uint32_t streamBufferBytes;
  uint32_t runtimeBytes;
  uint32_t packedTime;
  uint32_t packedDate;
  uint32_t initialVideoBytes;
  uint32_t entryBytes;
  uint32_t status;
  Bool8 looseFileOpened;
  uint32_t openError;
  uint32_t allocError;
  void *allocPayload;
  SoundVoiceSet *audioVoiceSet;
  PckEntryHeader *packageEntry;
  EngineFileHandle packageFileHandle;
  MovieStreamByteCount remainingByteCount;
  uint8_t *loadedEnd;
  MovieStreamFileOffset streamPosition;
  MovieSharedStreamHandleFlag isSharedPackageHandle;

  isSharedPackageHandle = 0;
  looseFileOpened = false;
  entryBytes = 0;
  if (((movieOpenFlags & MOVIE_OPEN_PACKAGE_ONLY) == 0) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    looseFileOpened = g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16,&handle) == 0;
  }
  if (!looseFileOpened) {
    movieOpenFlags = movieOpenFlags & ~MOVIE_OPEN_PACKAGE_ONLY;
    packageEntry = Package_FindEntryAcrossMounts(path,&packageFileHandle);
    if ((packageEntry != nullptr) &&
       (g_FileSystemSeek
            (FILESYSTEM_SEEK_BEGIN,packageEntry->runtimePayloadOffset + PCK_ENTRY_HEADER_BYTES,
             THANDOR_PTR((uintptr_t)packageFileHandle)) == 0)) {
      isSharedPackageHandle++;
      handle = THANDOR_PTR((uintptr_t)packageFileHandle);
      /* a movie entry is stored; the larger size is taken so a slightly inconsistent entry still plays */
      entryBytes = packageEntry->packedSize < packageEntry->unpackedSize ? packageEntry->unpackedSize :
                   packageEntry->packedSize;
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                (g_FileSystemCombinedPathScratchUtf16,path,
                 g_ExecutableDirectoryUtf16);
      openError = g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16,&handle);
      if (openError != 0) {
        openError = g_FileSystemOpen(0,path,&handle);
        if (openError != 0) {
          /* Nothing is open yet: no close. */
          if (outError != nullptr) {
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
  header = reinterpret_cast<MovieFileHeader *>(g_PackageScratchBuffer);
  if ((header->common.magic != ASSET_MAGIC_FLM) ||
      ((uint32_t)header->common.converterVersion != MOVIE_FLM_CONVERTER_VERSION)) {
    return Movie_OpenFail(handle,isSharedPackageHandle,FATAL_ERROR_MOVIE_INVALID,outError);
  }
  if ((isSharedPackageHandle == 0) && !g_FileSystemGetSize(handle,&entryBytes)) {
    entryBytes = 0;
  }
  if (!Movie_IsHeaderValid(header,entryBytes)) {
    return Movie_OpenFail(handle,isSharedPackageHandle,FATAL_ERROR_MOVIE_INVALID,outError);
  }
  /* the buffer holds the header and the whole video stream, or a bounded window of it when streaming */
  streamBufferBytes = Movie_StreamBufferBytes(header,movieOpenFlags);
  allocError = g_MemoryApi.alloc(streamBufferBytes,&allocPayload);
  if (allocError != 0) {
    return Movie_OpenFail(handle,isSharedPackageHandle,allocError,outError);
  }
  copySource = reinterpret_cast<uint32_t *>(g_PackageScratchBuffer);
  copyDestination = static_cast<uint32_t *>(allocPayload);
  std::copy_n(copySource,MOVIE_FILE_HEADER_BYTES / 4,copyDestination);
  header = static_cast<MovieFileHeader *>(allocPayload);
  initialVideoBytes = header->videoStreamBytes;
  if ((MOVIE_INITIAL_VIDEO_MAX_BYTES < initialVideoBytes) && (movieOpenFlags != 0)) {
    initialVideoBytes = MOVIE_INITIAL_VIDEO_MAX_BYTES;
  }
  remainingByteCount = header->videoStreamBytes - initialVideoBytes;
  loadedEnd = Thandor_At(header + 1, initialVideoBytes);
  status = g_FileSystemReadExact(initialVideoBytes,header + 1,handle);
  if (status != 0) {
    g_MemoryApi.free(header);
    return Movie_OpenFail(handle,isSharedPackageHandle,status,outError);
  }
  if (!g_FileSystemGetPosition(handle,&streamPosition)) {
    /* Original quirk: a failed position query fails the open with the position value as the error (0) */
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
  movie = static_cast<MovieRuntime *>(allocPayload);
  g_ActiveMovie = movie;
  if ((isSharedPackageHandle == 0) && (remainingByteCount == 0)) {
    g_FileSystemClose(handle);
  }
  movie->textureCommon.magic = ASSET_MAGIC_GFX;
  movie->textureCommon.allocationSizeBytes = runtimeBytes;
  movie->textureCommon.formatVersion = 1;
  movie->textureCommon.converterVersion = 0;
  movie->audioVoiceSet = audioVoiceSet;
  movie->activeAudioBuffer = nullptr;
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
  movie->refillSemaphore = nullptr;
  if ((remainingByteCount != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
    movie->workerActive++;
    movie->refillSemaphore = CreateSemaphoreA(nullptr,0,1,nullptr);
    /* The original passes the address of its remainingByteCount local as lpThreadId. */
    workerThread = CreateThread(nullptr,0,(LPTHREAD_START_ROUTINE)Movie_StreamWorkerThread,nullptr,0,
                                &remainingByteCount);
    if (workerThread == nullptr) {
      movie->workerActive--;
    }
    else {
      CloseHandle(workerThread);
    }
  }
  if (outPlaybackRateHz != nullptr) {
    *outPlaybackRateHz = header->frameIntervalMilliseconds;
  }
  return true;
}

/* Returns the frame size of the active movie, so callers can place and scale the movie texture. Both are
   zero when no movie is open.
*/
MovieFrameDimensions Movie_GetFrameDimensions()

{
  MovieFrameDimensions dimensions;

  dimensions.width = 0;
  dimensions.height = 0;
  if (g_ActiveMovie != nullptr) {
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
  if (g_ActiveMovie != nullptr) {
    g_ActiveMovie->audioGainQ15 = gainQ15;
  }
}

/* Background thread of a streamed movie: whenever Movie_AdvanceFrame signals the refill semaphore (or every
   256 ms), appends the next MOVIE_REFILL_CHUNK_BYTES of video to the buffer while it stays below
   MOVIE_REFILL_LIMIT_BYTES, so playback does not stall on disk reads. Ends when the movie is closed, fully
   loaded or a read fails (MOVIE_STREAM_READ_FAILED), and clears workerActive on the way out.
*/
static uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext)

{
  void *handle;
  HANDLE refillSemaphore;
  MovieRuntime *movie;
  uint32_t byteCount;
  MovieStreamState expectedState;

  /* The original keeps the movie in a local: it re-reads g_ActiveMovie only at the loop top, after the
     wait and at the exit. */
  while ((movie = g_ActiveMovie) != nullptr) {
    refillSemaphore = movie->refillSemaphore;
    MsgWaitForMultipleObjects(1,&refillSemaphore,FALSE,256,0);
    movie = g_ActiveMovie;
    if ((movie == nullptr) || (Movie_StreamState(movie).load() == MOVIE_STREAM_SHUTDOWN) ||
        (Movie_WorkerActive(movie).load() == 0) || (movie->remainingVideoBytes == 0)) break;
    if (Movie_StreamState(movie).load() == MOVIE_STREAM_IDLE) continue;
    byteCount = movie->remainingVideoBytes;
    if ((uint32_t)Thandor_ByteDistance(movie->loadedVideoEnd.get(), movie->fileHeader.get()) < MOVIE_REFILL_LIMIT_BYTES) {
      handle = movie->streamHandle;
      if (MOVIE_REFILL_CHUNK_BYTES < byteCount) {
        byteCount = MOVIE_REFILL_CHUNK_BYTES;
      }
      g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,movie->streamFileOffset,handle);
      if (g_FileSystemReadExact(byteCount,movie->loadedVideoEnd,handle) != 0) {
        /* only FILL_REQUESTED becomes READ_FAILED; a SHUTDOWN stored by Movie_Close stays */
        expectedState = MOVIE_STREAM_FILL_REQUESTED;
        Movie_StreamState(movie).compare_exchange_strong(expectedState,MOVIE_STREAM_READ_FAILED);
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
    if (movie->remainingVideoBytes == 0) break;
    /* The original checked for SHUTDOWN and then stored IDLE, so a Movie_Close between the two lost its
       SHUTDOWN and waited forever for this worker. Changed to one compare-exchange: it fails (and the worker
       leaves) exactly when Movie_Close has stored SHUTDOWN. */
    expectedState = MOVIE_STREAM_FILL_REQUESTED;
    if (!Movie_StreamState(movie).compare_exchange_strong(expectedState,MOVIE_STREAM_IDLE)) break;
  }
  if (g_ActiveMovie != nullptr) {
    Movie_WorkerActive(g_ActiveMovie).store(0);
  }
  return 0;
}

/* Resets currentFrameIndex and videoStreamOffset of g_ActiveMovie to the first frame and stops its audio voice,
   so the movie plays again from the start. It does not rebuild a discarded streaming prefix. Called by
   FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState when the movie of a frontend page has ended (when the
   page enables movie playback), which makes it loop.
*/
void Movie_Rewind()

{
  MovieRuntime *activeMovie;
  
  activeMovie = g_ActiveMovie;
  if (g_ActiveMovie != nullptr) {
    g_ActiveMovie->currentFrameIndex = 0;
    activeMovie->videoStreamOffset = MOVIE_FILE_HEADER_BYTES; /* the first frame follows the header */
    if (activeMovie->activeAudioBuffer != nullptr) {
      g_SoundStopVoice(activeMovie->activeAudioBuffer);
      activeMovie->activeAudioBuffer = nullptr;
    }
  }
}

/* Closes g_ActiveMovie. With the arena allocator a refill worker may run: it is told to stop and waited for
   (with the process dropped from real-time to normal priority so the worker gets CPU time while this thread
   spins), then the semaphore is closed. Frees the FLM buffer, the soundtrack voice set, a still-open own
   stream handle and the MovieRuntime.
*/
void Movie_Close()

{
  MovieRuntime *movie;
  HANDLE currentProcessHandle;
  HANDLE hProcess;

  movie = g_ActiveMovie;
  if (g_ActiveMovie != nullptr) {
    if (g_MemoryApi.alloc == ArenaHeap_Alloc) {
      Movie_StreamState(g_ActiveMovie).store(MOVIE_STREAM_SHUTDOWN);
      currentProcessHandle = GetCurrentProcess();
      SetPriorityClass(currentProcessHandle,NORMAL_PRIORITY_CLASS);
      /* wait for the worker to leave (an atomic read: the worker thread clears the flag; the original spun on
         a plain read and relied on the compiler not to keep it in a register) */
      while (Movie_WorkerActive(movie).load() != 0) {
        Thandor_SleepMs(0);
      }
      if (movie->refillSemaphore != nullptr) {
        CloseHandle(movie->refillSemaphore);
        movie->refillSemaphore = nullptr;
      }
      hProcess = GetCurrentProcess();
      SetPriorityClass(hProcess,DebugHook_ProcessPriorityClass(REALTIME_PRIORITY_CLASS));
    }
    g_ActiveMovie = nullptr;
    g_MemoryApi.free(movie->fileHeader);
    if (movie->audioVoiceSet != nullptr) {
      g_SoundReleaseSampleVoiceSet(movie->audioVoiceSet);
    }
    if ((movie->remainingVideoBytes != 0) && (movie->streamHandleIsSharedPackage == 0)) {
      g_FileSystemClose(movie->streamHandle);
    }
    g_MemoryApi.free(movie);
  }
}

/* Periodic timer callback registered at the movie's playback rate: counts one more frame that is due in
   g_IntroMoviePendingTicks. The intro loop consumes the count and decodes at most three pending frames per
   iteration, which keeps the movie in time on slow machines.
*/
void IntroMovie_TimerTick()

{
  g_IntroMoviePendingTicks++;
}

/* Not in the original (split out of Movie_AdvanceFrame): stores endCode in *outEndCode when given and
   returns false, the "no frame" result of Movie_AdvanceFrame. */
static Bool8 Movie_ReportAdvanceEnd(uint32_t *outEndCode,uint32_t endCode)
{
  if (outEndCode != nullptr) {
    *outEndCode = endCode;
  }
  return false;
}

/* Not in the original (split out of Movie_AdvanceFrame): once the read position of a streamed movie is a
   whole MOVIE_COMPACT_SHIFT_BYTES past the header, moves the unplayed bytes down by that shift (dword by
   dword) to make room for further refills. */
static void Movie_CompactStreamBuffer(MovieRuntime *movie)
{
  uint32_t readOffset;
  uint32_t loadedSize;
  uint32_t *copySource;
  uint32_t *copyDestination;

  readOffset = movie->videoStreamOffset;
  loadedSize = (uint32_t)Thandor_ByteDistance(movie->loadedVideoEnd.get(), movie->fileHeader.get());
  if ((MOVIE_COMPACT_SHIFT_BYTES + MOVIE_FILE_HEADER_BYTES - 1 < readOffset) && (readOffset < loadedSize)) {
    movie->videoStreamOffset = movie->videoStreamOffset - MOVIE_COMPACT_SHIFT_BYTES;
    copyDestination =
        reinterpret_cast<uint32_t *>(Thandor_Bytes(movie->fileHeader.get()) + readOffset - MOVIE_COMPACT_SHIFT_BYTES);
    movie->loadedVideoEnd = movie->loadedVideoEnd - MOVIE_COMPACT_SHIFT_BYTES;
    copySource = Thandor_At<uint32_t>(copyDestination, MOVIE_COMPACT_SHIFT_BYTES);
    /* the ranges overlap (destination below source): a forward copy, as the dword loop was */
    std::copy_n(copySource,(loadedSize - readOffset) >> 2,copyDestination);
  }
}

/* Decodes the next frame of g_ActiveMovie into its ARGB image, returns true and stores the movie in *outMovie.
   Asks the worker for more data when the buffer has room, starts the soundtrack with the first frame, and
   waits (returns true without decoding) while a streamed movie has less than one refill chunk buffered. A
   streamed movie drops played bytes from the buffer front in MOVIE_COMPACT_SHIFT_BYTES steps. Returns false
   after the last frame, on a read failure of the worker or when no movie is open; *outEndCode then gets
   FATAL_ERROR_MOVIE_INVALID (no movie / read failure) or the unplayed bytes left in the buffer (after the
   last frame). Either output may be NULL; only the one for the returned case is written.
   Original quirk: after a worker read failure it closes an unrelated value left over by its caller instead
   of the stream handle; the C closes NULL, which has the same effect on the movie (see the body).
*/
Bool8 Movie_AdvanceFrame(MovieRuntime **outMovie,uint32_t *outEndCode)

{
  MovieFileHeader *flmHeader;
  MovieFrameIndex previousFrameIndex;
  MovieFrameIndex nextFrameIndex;
  MovieRuntime *movie;
  uint32_t bufferedBytes;
  uint32_t consumedBytes;
  uint8_t *streamCursor;
  SoundVoice *playedVoice;

  movie = g_ActiveMovie;
  if (movie == nullptr) {
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  if (Movie_StreamState(movie).load() == MOVIE_STREAM_READ_FAILED) {
    /* The original passes g_FileSystemClose a value it never sets on this path, so it closes whatever
       its caller left there -- never the movie stream handle: a UI/runtime object pointer in the
       frontend/in-game/briefing callers, g_FramebufferHeight in the Game_PlayIntroMovies frame loop, the
       outer caller's value via MoviePlayback_AdvanceToFrameAndPresent. Closing NULL keeps the effect (the stream handle stays
       open; remainingVideoBytes = 0 also keeps Movie_Close from closing it) without the stray
       CloseHandle on an unrelated value. */
    g_FileSystemClose(nullptr);
    movie->remainingVideoBytes = 0;
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  if ((Movie_StreamState(movie).load() == MOVIE_STREAM_IDLE) && (Movie_WorkerActive(movie).load() != 0) &&
      (movie->remainingVideoBytes != 0) &&
      ((uint32_t)Thandor_ByteDistance(movie->loadedVideoEnd.get(), movie->fileHeader.get()) < MOVIE_REFILL_LIMIT_BYTES)) {
    /* only the worker leaves FILL_REQUESTED, so IDLE cannot change between the check and this store */
    Movie_StreamState(movie).store(MOVIE_STREAM_FILL_REQUESTED);
    ReleaseSemaphore(movie->refillSemaphore,1,nullptr);
  }
  flmHeader = movie->fileHeader;
  previousFrameIndex = movie->currentFrameIndex;
  streamCursor = Thandor_At(flmHeader, movie->videoStreamOffset);
  if ((previousFrameIndex == 0) && (movie->audioVoiceSet != nullptr)) {
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
       the movie pointer minus MOVIE_RUNTIME_PIXELS_OFFSET here, because its working pointer is only
       advanced to the pixels further down. Callers keep the value as the movie only after the first-frame call, which cannot
       get here (with remainingVideoBytes != 0 the first 0x3A2000 bytes are loaded). */
    if (outMovie != nullptr) {
      *outMovie = reinterpret_cast<MovieRuntime *>(Thandor_Bytes(movie) - MOVIE_RUNTIME_PIXELS_OFFSET);
    }
    return true;
  }
  /* the decoder stops at the end of the FLM buffer (bytes past loadedVideoEnd of a streamed movie are read as
     in the original) */
  consumedBytes = Movie_DecodeFrame4x4Delta
                    (flmHeader->heightPixels,flmHeader->widthPixels,movie->argbPixels,streamCursor,
                     Thandor_At(flmHeader, Movie_StreamBufferBytes(flmHeader,movie->openFlags)));
  movie->currentFrameIndex = nextFrameIndex;
  movie->videoStreamOffset = movie->videoStreamOffset + consumedBytes;
  DebugHook_MovieFrameDone(movie,consumedBytes);
  if ((movie->openFlags != 0) && (Movie_StreamState(movie).load() == MOVIE_STREAM_IDLE)) {
    Movie_CompactStreamBuffer(movie);
  }
  if (outMovie != nullptr) {
    *outMovie = movie;
  }
  return true;
}
