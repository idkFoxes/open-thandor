/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/flm_decoder.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_FLM_DECODER_H
#define THANDOR_MOVIE_RUNTIME_FLM_DECODER_H

#include <thandor/movie/runtime/types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/flm_decoder. */

/* FLM frame tokens (low 5 bits of the next stream dword, Movie_DecodeFrame4x4Delta / Movie_EncodeFrame4x4*):
   0..24 start an 8-byte colour block with that base luma; the skip tokens keep blocks of the previous frame,
   the count (minus its bias) sits in the bits above the token */
#define MOVIE_TOKEN_SKIP_SHORT 0x19 /* 1 byte: skip 1..8 blocks */
#define MOVIE_TOKEN_SKIP_MEDIUM 0x1A /* 2 bytes: skip 9..0x808 blocks */
#define MOVIE_TOKEN_SKIP_LONG 0x1B /* 4 bytes: skip 0x809 or more blocks */
/* Largest run of kept blocks the short and the medium skip token encode */
#define MOVIE_SKIP_SHORT_MAX_BLOCKS 8
#define MOVIE_SKIP_MEDIUM_MAX_BLOCKS 0x808
/* Low 5 bits of a stream dword: the frame token (MOVIE_TOKEN_SKIP_*, or the base luma of a colour block) */
#define MOVIE_TOKEN_MASK 0x1f
/* Highest base luma of a colour block (tokens 0..24) */
#define MOVIE_TOKEN_BASE_LUMA_MAX 24
/* FLM colour: 5-bit luma in bits 0-4 below the 10-bit chroma code (hue in bits 5-9, saturation in 10-14);
   a colour block stores the chroma code in bits 21-30 of its second dword */
#define MOVIE_COLOR_LUMA_MASK 0x1f
#define MOVIE_COLOR_HUE_MASK 0x3e0
#define MOVIE_COLOR_SATURATION_MASK 0x7c00
#define MOVIE_COLOR_CHROMA_MASK 0x7fe0
/* Bit 31 of a colour block's second dword: every luma step counts twice (4-bit levels 0..15 halved) */
#define MOVIE_BLOCK_DOUBLE_STEPS 0x80000000
/* Largest luma level of a colour block with doubled steps */
#define MOVIE_BLOCK_WIDE_LEVEL_MAX 15
/* 0x8000 * sqrt(3): the blue-green axis of the chroma vector (MovieColor_ComputeChromaCodeFromRgb888) */
#define MOVIE_CHROMA_SQRT3_Q15 0xddb4
/* 2^16 / 3: (r + g + b) * this >> 19 is the channel average scaled to 5 bits (MovieColor_ComputeLuma5FromRgb888) */
#define MOVIE_LUMA_THIRD_Q16 0x5555

/* Functions are grouped by semantic ownership. */

/* Decodes one frame from encodedFrame, reading nothing at or after encodedEnd (see flm_decoder.cpp). */
uint32_t Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          const uint8_t *encodedFrame,const uint8_t *encodedEnd);

/* Not in the original: fills g_MovieChromaLumaToArgb (the original shipped it precomputed). */
void Movie_BuildChromaLumaTable(void);

#endif /* THANDOR_MOVIE_RUNTIME_FLM_DECODER_H */
