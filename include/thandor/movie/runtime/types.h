/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_TYPES_H
#define THANDOR_MOVIE_RUNTIME_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct MovieRuntime MovieRuntime, *PMovieRuntime;
typedef struct MovieFileHeader MovieFileHeader, *PMovieFileHeader;
typedef struct FrameProviderResult FrameProviderResult, *PFrameProviderResult;

using MovieFrameIndex = uint32_t;

using MovieWorkerActiveFlag = uint32_t;

using MoviePaletteBankCount = uint32_t;

using MovieAudioGainQ15 = uint32_t;

using MovieSubresourceCount = uint32_t;

using MovieSubresourceTableOffset = uint32_t;

using MovieOpenFlags = uint32_t;

using MovieSharedStreamHandleFlag = uint32_t;

using MovieFrameCount = uint32_t;

using MovieStreamOffset = uint32_t;

using MovieStreamFileOffset = uint32_t;

using MovieFrameIntervalMilliseconds = uint32_t;

using MoviePixelDimension = uint32_t;

typedef struct MovieFrameDimensions {
    AssetDimension width;
    AssetDimension height;
} MovieFrameDimensions;

using MovieAudioTrackCount = uint32_t;

using MovieStreamByteCount = uint32_t;

enum {
    MOVIE_STREAM_IDLE=0,
    MOVIE_STREAM_FILL_REQUESTED=1,
    MOVIE_STREAM_READ_FAILED=2,
    MOVIE_STREAM_SHUTDOWN=3
};
using MovieStreamState = int;

struct MovieFileHeader {
    struct GeneratedAssetCommonPrefix common; 
    MoviePixelDimension widthPixels; 
    MoviePixelDimension heightPixels; 
    MovieFrameCount frameCount; 
    MovieAudioTrackCount audioTrackCount; 
    MovieStreamByteCount videoStreamBytes; 
    uint32_t audioTrackBytes[14]; 
    MovieFrameIntervalMilliseconds frameIntervalMilliseconds;
    char unusedText[256]; // The asset header's text at +0x100: empty in every stock .flm, never read.
};

struct MovieRuntime {
    struct GeneratedAssetCommonPrefix textureCommon; 
    MovieSubresourceCount subresourceCount; 
    MoviePaletteBankCount paletteBankCount; 
    MovieSubresourceTableOffset subresourceTableOffset; 
    uint32_t reservedBC; 
    Ptr32<struct MovieFileHeader> fileHeader; 
    MovieFrameIndex currentFrameIndex; 
    MovieStreamOffset videoStreamOffset; 
    Ptr32<struct DirectSoundVoiceSet> audioVoiceSet; 
    Ptr32<struct IDirectSoundBuffer> activeAudioBuffer; 
    Ptr32<void> streamHandle; 
    MovieSharedStreamHandleFlag streamHandleIsSharedPackage; 
    Ptr32<uint8_t> loadedVideoEnd; 
    MovieStreamByteCount remainingVideoBytes; 
    MovieStreamFileOffset streamFileOffset; 
    MovieOpenFlags openFlags; 
    MovieAudioGainQ15 audioGainQ15; 
    MovieWorkerActiveFlag workerActive; 
    MovieStreamState streamState; 
    Ptr32<void> refillSemaphore; 
    uint32_t reservedFC;
    char unusedText[256]; // The gfx asset header's text at +0x100 (see GraphicsTextureSourceAsset); Movie_Open clears its first byte.
    struct GraphicsTextureSourceEntry sourceEntry;
    uint32_t argbPixels[1]; 
};

struct FrameProviderResult {
    Ptr32<void> frameOrError; // frame pointer when noFrame is false; error-coded value when it is true
    Bool8 noFrame; // true when no frame is returned
};
using MovieFrameProviderProc = FrameProviderResult (void * frameToReleaseOrNull);

#endif /* THANDOR_MOVIE_RUNTIME_TYPES_H */
