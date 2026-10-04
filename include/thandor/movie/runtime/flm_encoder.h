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

/* Encoder-only constants (the decoder does not need them). */
/* Largest luma level of a colour block with doubled steps (MOVIE_BLOCK_DOUBLE_STEPS) */
#define MOVIE_BLOCK_WIDE_LEVEL_MAX 15
/* 0x8000 * sqrt(3): the blue-green axis of the chroma vector (MovieColor_ComputeChromaCodeFromRgb888) */
#define MOVIE_CHROMA_SQRT3_Q15 0xddb4
/* 2^16 / 3: (r + g + b) * this >> 19 is the channel average scaled to 5 bits (MovieColor_ComputeLuma5FromRgb888) */
#define MOVIE_LUMA_THIRD_Q16 0x5555

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
