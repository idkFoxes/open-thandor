/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/flm_encoder.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_FLM_ENCODER_H
#define THANDOR_MOVIE_RUNTIME_FLM_ENCODER_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/movie/runtime/types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/flm_encoder. */

/* Functions are grouped by semantic ownership. */

Bool8 Movie_EncodeFlmBufferFromFrameProvider
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderProc *frameProvider,uint32_t *outByteCount);

uint32_t Movie_EncodeFrame4x4Keyframe(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels);

uint32_t Movie_EncodeFrame4x4Delta(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels);

uint32_t MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888);

uint32_t MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888);

#endif /* THANDOR_MOVIE_RUNTIME_FLM_ENCODER_H */
