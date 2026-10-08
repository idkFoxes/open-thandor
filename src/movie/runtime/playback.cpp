/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/playback.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <atomic>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/playback.h>
#include <algorithm>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* KERNEL32 ReOpenFile (Vista and later), not in thandor/generated/imports.h: a second handle to the file of an
   open handle, with its own file position (see Movie_StartStreamWorker). */
extern "C" __declspec(dllimport) HANDLE __stdcall ReOpenFile(HANDLE hOriginalFile, DWORD dwDesiredAccess,
                                                             DWORD dwShareMode, DWORD dwFlagsAndAttributes);

/* Module data. */

THANDOR_ALIGN(4) MovieAudioGainQ15 g_MovieDefaultAudioGainQ15 = 32768;

THANDOR_ALIGN(16) MovieAudioGainQ15 g_MovieAlternateAudioGainQ15 = 32768;

MovieRuntime *g_ActiveMovie = nullptr;

/* Largest frame width and height Movie_Open accepts (the stock movies are at most 800x600) */
static constexpr int MOVIE_MAX_FRAME_DIMENSION = 4096;

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

/* Not in the original: the refill worker advances remainingVideoBytes and loadedVideoEnd while the main thread
   reads them in Movie_AdvanceFrame (the original used plain fields, a data race). The worker stores them with
   release after the read has filled the buffer, the readers load them with acquire, so the main thread sees the
   new bytes before it sees the larger end. On x86 these are the same plain moves as before. */
static std::atomic_ref<MovieStreamByteCount> Movie_RemainingVideoBytes(MovieRuntime *movie)
{
  return std::atomic_ref<MovieStreamByteCount>(movie->remainingVideoBytes);
}

static uint8_t *Movie_LoadLoadedVideoEnd(MovieRuntime *movie)
{
  return std::atomic_ref<Ptr32<uint8_t>>(movie->loadedVideoEnd).load(std::memory_order_acquire).get();
}

static void Movie_StoreLoadedVideoEnd(MovieRuntime *movie,uint8_t *loadedEnd)
{
  std::atomic_ref<Ptr32<uint8_t>>(movie->loadedVideoEnd).store(Ptr32<uint8_t>(loadedEnd),std::memory_order_release);
}

/* Not in the original: the size of the FLM buffer Movie_Open allocates (header plus video stream, capped when
   streaming). Movie_Open has checked that the sum fits in 32 bits. */
static uint32_t Movie_StreamBufferBytes(const MovieFileHeader *header,MovieOpenFlags openFlags)
{
  uint32_t bufferBytes = header->videoStreamBytes + MOVIE_FILE_HEADER_BYTES;

  if ((MOVIE_STREAM_BUFFER_MAX_BYTES < bufferBytes) && Any(openFlags)) {
    bufferBytes = MOVIE_STREAM_BUFFER_MAX_BYTES;
  }
  return bufferBytes;
}

/* Not in the original: checks the FLM header values Movie_Open sizes its buffers with. entryBytes is the size of
   the file or package entry (0 when unknown). The original trusted them. Bounded here because they come from the
   file: the 32-bit size sums could wrap (a too small buffer that the read then overruns), a width or height
   below 4 wraps the decoder's block counters, and a video stream longer than the file cannot be read. */
static bool Movie_IsHeaderValid(const MovieFileHeader *header,uint32_t entryBytes)
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
  ArenaScoped audioSample; /* the track's sample, freed on return as before */
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
  allocError = audioSample.allocate(trackBytes);
  if (allocError != 0) {
    return allocError;
  }
  loadError = g_FileSystemReadExact(trackBytes,audioSample.get(),handle);
  if (loadError == 0) {
    loadError = g_SoundCreateSampleVoiceSet(audioSample.as<SoundSampleAsset>(),trackBytes,&voiceSet);
    if (loadError == 0) {
      *outVoiceSet = voiceSet;
    }
  }
  return loadError;
}

static uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

/* Not in the original (split out of Movie_StreamWorkerThread, also used by the synchronous fallback in
   Movie_AdvanceFrame): while the buffer holds less than MOVIE_REFILL_LIMIT_BYTES, appends the next
   MOVIE_REFILL_CHUNK_BYTES (at most the rest) of the video stream from streamFileOffset. A loose file is closed
   once fully read; a shared package handle stays open for other entries. Returns false when the seek or the read
   fails. The original ignored the seek result (a failed seek read the chunk from the wrong position); a failed
   seek now ends the stream like a failed read. */
static bool Movie_RefillChunk(MovieRuntime *movie)
{
  void *handle;
  uint32_t byteCount;
  uint32_t remainingBytes;
  uint8_t *loadedEnd;

  remainingBytes = Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire);
  loadedEnd = Movie_LoadLoadedVideoEnd(movie);
  byteCount = remainingBytes;
  if ((uint32_t)Thandor_ByteDistance(loadedEnd, movie->fileHeader.get()) < MOVIE_REFILL_LIMIT_BYTES) {
    handle = movie->streamHandle;
    if (MOVIE_REFILL_CHUNK_BYTES < byteCount) {
      byteCount = MOVIE_REFILL_CHUNK_BYTES;
    }
    if ((g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,movie->streamFileOffset,handle) != 0) ||
        (g_FileSystemReadExact(byteCount,loadedEnd,handle) != 0)) {
      return false;
    }
    remainingBytes = remainingBytes - byteCount;
    Movie_RemainingVideoBytes(movie).store(remainingBytes,std::memory_order_release);
    movie->streamFileOffset = movie->streamFileOffset + byteCount;
    Movie_StoreLoadedVideoEnd(movie,loadedEnd + byteCount);
    /* a loose file is closed once fully read; a package handle stays open for other entries */
    if ((remainingBytes == 0) && (movie->streamHandleIsSharedPackage == 0)) {
      g_FileSystemClose(handle);
    }
  }
  return true;
}

/* Not in the original (split out of Movie_Open): starts the refill worker of a partly loaded movie. Without a
   worker (semaphore or thread creation failed, or no private package handle) refillSemaphore stays NULL and
   Movie_AdvanceFrame refills synchronously on the main thread instead, so the movie still plays to its end.
   The original ignored a failed semaphore creation and, on a failed thread creation, left the movie waiting
   forever for refills that never came.
   A movie in a mounted package was streamed on the package's shared handle, whose file position the main
   thread also moves (other entries of the same package); the worker now gets a private handle to the same
   file (ReOpenFile), so the two threads never share a file position. That handle is the movie's own and is
   closed like a loose file. */
static void Movie_StartStreamWorker(MovieRuntime *movie)
{
  HANDLE privateHandle;
  HANDLE refillSemaphore;
  HANDLE workerThread;
  DWORD workerThreadId;

  if (movie->streamHandleIsSharedPackage != 0) {
    privateHandle = ReOpenFile(movie->streamHandle,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,0);
    if (privateHandle == INVALID_HANDLE_VALUE) {
      Thandor_Log("Movie_Open: no private package handle; movie refilled on the main thread");
      return;
    }
    movie->streamHandle = privateHandle;
    movie->streamHandleIsSharedPackage = 0;
  }
  refillSemaphore = CreateSemaphoreA(nullptr,0,1,nullptr);
  if (refillSemaphore == nullptr) {
    Thandor_Log("Movie_Open: CreateSemaphore failed; movie refilled on the main thread");
    return;
  }
  movie->refillSemaphore = refillSemaphore;
  movie->workerActive++;
  /* (The original passed the address of its remainingByteCount local as lpThreadId.) */
  workerThread = CreateThread(nullptr,0,(LPTHREAD_START_ROUTINE)Movie_StreamWorkerThread,nullptr,0,
                              &workerThreadId);
  if (workerThread == nullptr) {
    Thandor_Log("Movie_Open: CreateThread failed; movie refilled on the main thread");
    movie->workerActive--;
    CloseHandle(refillSemaphore);
    movie->refillSemaphore = nullptr;
    return;
  }
  CloseHandle(workerThread);
}

/* Failure exit of Movie_Open once a file or package handle is open: closes the handle unless it is a shared
   package handle, stores error in *outError (when not NULL) and returns false. */
static bool Movie_OpenFail(void *handle,MovieSharedStreamHandleFlag isSharedPackageHandle,uint32_t error,
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
bool Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path,uint32_t *outPlaybackRateHz,uint32_t *outError)

{
  MovieFileHeader *header;
  MovieRuntime *movie;
  void *handle;
  uint32_t *copySource;
  uint32_t *copyDestination;
  MovieSubresourceCount frameWidth;
  MoviePaletteBankCount frameHeight;
  MovieAudioGainQ15 defaultAudioGain;
  uint32_t streamBufferBytes;
  uint32_t runtimeBytes;
  uint32_t packedTime;
  uint32_t packedDate;
  uint32_t initialVideoBytes;
  uint32_t entryBytes;
  uint32_t status;
  bool looseFileOpened;
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

  /* Not in the original: only one movie can be open at a time (g_ActiveMovie, the refill worker and Movie_Close
     know only one); debug builds assert it, the others log it. Original quirk: a second open replaces
     g_ActiveMovie and leaks the open movie. Kept, because callers may still draw the old movie as a texture
     (closing it here would free it under them), and a caller can reach it with a movie whose first frame
     failed (the in-game notification movies leave it open and open the next one). */
  if (g_ActiveMovie != nullptr) {
    Thandor_Log("Movie_Open: a movie is already open; it is replaced without being closed");
  }
  assert(g_ActiveMovie == nullptr);
  isSharedPackageHandle = 0;
  looseFileOpened = false;
  entryBytes = 0;
  if (!Any(movieOpenFlags & MovieOpenFlags::MOVIE_OPEN_PACKAGE_ONLY) && (g_LooseMoviePathPrefix.firstTwoCodeUnits != 0)) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,path,g_LooseMoviePathPrefix.codeUnits);
    looseFileOpened = g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,g_FileSystemCombinedPathScratchUtf16,&handle) == 0;
  }
  if (!looseFileOpened) {
    movieOpenFlags = movieOpenFlags & ~MovieOpenFlags::MOVIE_OPEN_PACKAGE_ONLY;
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
      openError = g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,g_FileSystemCombinedPathScratchUtf16,&handle);
      if (openError != 0) {
        openError = g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,path,&handle);
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
  if ((MOVIE_INITIAL_VIDEO_MAX_BYTES < initialVideoBytes) && Any(movieOpenFlags)) {
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
  movie->streamState = MovieStreamState::MOVIE_STREAM_IDLE;
  movie->refillSemaphore = nullptr;
  if ((remainingByteCount != 0) && (g_MemoryApi.alloc == ArenaHeap_Alloc)) {
    Movie_StartStreamWorker(movie);
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
   loaded or a seek or read fails (MovieStreamState::MOVIE_STREAM_READ_FAILED), and clears workerActive on the way
   out.
*/
static uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext)

{
  HANDLE refillSemaphore;
  MovieRuntime *movie;
  MovieStreamState expectedState;

  /* The original keeps the movie in a local: it re-reads g_ActiveMovie only at the loop top, after the
     wait and at the exit. */
  while ((movie = g_ActiveMovie) != nullptr) {
    refillSemaphore = movie->refillSemaphore;
    MsgWaitForMultipleObjects(1,&refillSemaphore,FALSE,256,0);
    movie = g_ActiveMovie;
    if ((movie == nullptr) || (Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_SHUTDOWN) ||
        (Movie_WorkerActive(movie).load() == 0) ||
        (Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire) == 0)) break;
    if (Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_IDLE) continue;
    if (!Movie_RefillChunk(movie)) {
      /* only FILL_REQUESTED becomes READ_FAILED; a SHUTDOWN stored by Movie_Close stays */
      expectedState = MovieStreamState::MOVIE_STREAM_FILL_REQUESTED;
      Movie_StreamState(movie).compare_exchange_strong(expectedState,MovieStreamState::MOVIE_STREAM_READ_FAILED);
      break;
    }
    if (Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire) == 0) break;
    /* The original checked for SHUTDOWN and then stored IDLE, so a Movie_Close between the two lost its
       SHUTDOWN and waited forever for this worker. Changed to one compare-exchange: it fails (and the worker
       leaves) exactly when Movie_Close has stored SHUTDOWN. */
    expectedState = MovieStreamState::MOVIE_STREAM_FILL_REQUESTED;
    if (!Movie_StreamState(movie).compare_exchange_strong(expectedState,MovieStreamState::MOVIE_STREAM_IDLE)) break;
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
      Movie_StreamState(g_ActiveMovie).store(MovieStreamState::MOVIE_STREAM_SHUTDOWN);
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
      /* back to the game's priority; the original restored real-time, high here (see ProcessEntry) */
      hProcess = GetCurrentProcess();
      SetPriorityClass(hProcess,DebugHook_ProcessPriorityClass(HIGH_PRIORITY_CLASS));
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
  /* Not in the original: an atomic increment, because the timer thread runs this while the intro loop counts
     the ticks down (the original used a plain read-modify-write, a data race). atomic_ref keeps the plain
     uint32_t global. */
  std::atomic_ref<uint32_t>(g_IntroMoviePendingTicks).fetch_add(1);
}

/* Not in the original (split out of Movie_AdvanceFrame): stores endCode in *outEndCode when given and
   returns false, the "no frame" result of Movie_AdvanceFrame. */
static bool Movie_ReportAdvanceEnd(uint32_t *outEndCode,uint32_t endCode)
{
  if (outEndCode != nullptr) {
    *outEndCode = endCode;
  }
  return false;
}

/* Not in the original (split out of Movie_AdvanceFrame): once the read position of a streamed movie is a
   whole MOVIE_COMPACT_SHIFT_BYTES past the header, moves the unplayed bytes down by that shift to make room
   for further refills. Called only in MOVIE_STREAM_IDLE, when no refill runs. */
static void Movie_CompactStreamBuffer(MovieRuntime *movie)
{
  uint32_t readOffset;
  uint32_t loadedSize;
  uint8_t *copySource;
  uint8_t *copyDestination;
  uint8_t *loadedEnd;

  readOffset = movie->videoStreamOffset;
  loadedEnd = Movie_LoadLoadedVideoEnd(movie);
  loadedSize = (uint32_t)Thandor_ByteDistance(loadedEnd, movie->fileHeader.get());
  if ((MOVIE_COMPACT_SHIFT_BYTES + MOVIE_FILE_HEADER_BYTES - 1 < readOffset) && (readOffset < loadedSize)) {
    movie->videoStreamOffset = movie->videoStreamOffset - MOVIE_COMPACT_SHIFT_BYTES;
    copyDestination = Thandor_Bytes(movie->fileHeader.get()) + readOffset - MOVIE_COMPACT_SHIFT_BYTES;
    Movie_StoreLoadedVideoEnd(movie,loadedEnd - MOVIE_COMPACT_SHIFT_BYTES);
    copySource = copyDestination + MOVIE_COMPACT_SHIFT_BYTES;
    /* The original copied (loadedSize - readOffset) / 4 dwords forward, dropping up to 3 tail bytes of an
       unplayed size that is not a multiple of 4; bounded here because those bytes stay stale in front of
       loadedVideoEnd and are decoded. Now the exact byte count is moved; std::copy_n is a forward copy, which
       is defined for these overlapping ranges because the destination starts below the source (a memmove for
       bytes). Every stock FLM has a video stream size and frame sizes that are multiples of 8, so the count is
       a multiple of 4 and the same bytes move as before. */
    std::copy_n(copySource,loadedSize - readOffset,copyDestination);
  }
}

/* Decodes the next frame of g_ActiveMovie into its ARGB image, returns true and stores the movie in *outMovie.
   Asks the worker for more data when the buffer has room, starts the soundtrack with the first frame, and
   waits (returns true without decoding) while a streamed movie has less than one refill chunk buffered. A
   streamed movie drops played bytes from the buffer front in MOVIE_COMPACT_SHIFT_BYTES steps. Returns false
   after the last frame, on a seek or read failure of a refill or when no movie is open; *outEndCode then gets
   FATAL_ERROR_MOVIE_INVALID (no movie / read failure) or the unplayed bytes left in the buffer (after the
   last frame). Either output may be NULL; only the one for the returned case is written.
   Original quirk: after a worker read failure it closes an unrelated value left over by its caller instead
   of the stream handle; the C closes NULL, which has the same effect on the movie (see the body).
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
  SoundVoice *playedVoice;

  movie = g_ActiveMovie;
  if (movie == nullptr) {
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  /* Not in the original: a partly loaded movie without a refill worker (see Movie_StartStreamWorker) refills
     here on the main thread; a failed seek or read ends it like a failed worker read. With a worker
     refillSemaphore is never NULL, so this does not run. */
  if ((movie->refillSemaphore == nullptr) && (Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire) != 0) &&
      (Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_IDLE) && !Movie_RefillChunk(movie)) {
    Movie_StreamState(movie).store(MovieStreamState::MOVIE_STREAM_READ_FAILED);
  }
  if (Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_READ_FAILED) {
    /* The original passes g_FileSystemClose a value it never sets on this path, so it closes whatever
       its caller left there -- never the movie stream handle: a UI/runtime object pointer in the
       frontend/in-game/briefing callers, g_FramebufferHeight in the Game_PlayIntroMovies frame loop, the
       outer caller's value via MoviePlayback_AdvanceToFrameAndPresent. Closing NULL keeps the effect (the stream handle stays
       open; remainingVideoBytes = 0 also keeps Movie_Close from closing it) without the stray
       CloseHandle on an unrelated value. */
    g_FileSystemClose(nullptr);
    Movie_RemainingVideoBytes(movie).store(0,std::memory_order_release);
    return Movie_ReportAdvanceEnd(outEndCode,FATAL_ERROR_MOVIE_INVALID);
  }
  if ((Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_IDLE) && (Movie_WorkerActive(movie).load() != 0) &&
      (Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire) != 0) &&
      ((uint32_t)Thandor_ByteDistance(Movie_LoadLoadedVideoEnd(movie), movie->fileHeader.get()) < MOVIE_REFILL_LIMIT_BYTES)) {
    /* only the worker leaves FILL_REQUESTED, so IDLE cannot change between the check and this store */
    Movie_StreamState(movie).store(MovieStreamState::MOVIE_STREAM_FILL_REQUESTED);
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
  bufferedBytes = (uint32_t)(Movie_LoadLoadedVideoEnd(movie) - streamCursor);
  if (nextFrameIndex > flmHeader->frameCount) {
    /* past the last frame: the end code is the unplayed byte count */
    return Movie_ReportAdvanceEnd(outEndCode,bufferedBytes);
  }
  if ((Movie_RemainingVideoBytes(movie).load(std::memory_order_acquire) != 0) &&
      (bufferedBytes < MOVIE_REFILL_CHUNK_BYTES)) {
    /* Not enough bytes buffered yet: success without decoding. Original quirk: the original returns
       the movie pointer minus MOVIE_RUNTIME_PIXELS_OFFSET here, because its working pointer is only
       advanced to the pixels further down. Callers keep the value as the movie only after the first-frame call, which cannot
       get here (with remainingVideoBytes != 0 the first 0x3A2000 bytes are loaded). */
    if (outMovie != nullptr) {
      /* computed in uintptr_t: the value lies before the allocation, so pointer arithmetic would be UB */
      *outMovie = reinterpret_cast<MovieRuntime *>(reinterpret_cast<uintptr_t>(movie) - MOVIE_RUNTIME_PIXELS_OFFSET);
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
  if (Any(movie->openFlags) && (Movie_StreamState(movie).load() == MovieStreamState::MOVIE_STREAM_IDLE)) {
    Movie_CompactStreamBuffer(movie);
  }
  if (outMovie != nullptr) {
    *outMovie = movie;
  }
  return true;
}
