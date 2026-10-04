/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/flm_decoder.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/flm_decoder.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* Entries per chroma code (one per 5-bit luma) and the number of chroma codes */
#define MOVIE_CHROMA_LUMA_ROW 32
#define MOVIE_CHROMA_CODES 1024
/* Entries behind the last row: a colour block reads up to base luma 24 + 7 * 2 = 38, past its row */
#define MOVIE_CHROMA_LUMA_PADDING 16

/* filled at startup by Movie_BuildChromaLumaTable; row = chroma code, column = luma. The original's
   precomputed table was read past the end of a row (into the next one) by malformed colour blocks, which is
   kept; the padding bounds the last row, whose spill reads the zero padding here. */
static uint32_t g_MovieChromaLumaToArgb[MOVIE_CHROMA_CODES * MOVIE_CHROMA_LUMA_ROW + MOVIE_CHROMA_LUMA_PADDING] = {};

/* Implementation ownership: movie/runtime/flm_decoder. */

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
    destinationRow[pixelIndex & 3] = g_MovieChromaLumaToArgb[tableIndex + lumaStep * lumaStepScale];
    if ((pixelIndex & 3) == 3) {
      destinationRow = destinationRow + widthPixels;
    }
  }
}

/* Not in the original: the stream dword at cursor, or the bytes left before end (fewer than four) padded with
   zeros. The token and the skip counts of the short skip tokens only use the bytes they occupy, so the padding
   never changes a token that fits before end. */
static uint32_t Movie_LoadStreamDword(const uint8_t *cursor,const uint8_t *end)
{
  uint32_t value = 0;
  size_t available = (size_t)(end - cursor);

  memcpy(&value,cursor,available < 4 ? available : 4);
  return value;
}

/* Decodes one FLM frame over the previous one in the ARGB image, 4x4 blocks in row order. A colour block
   (token 0..24 = base luma) holds a 10-bit chroma code and sixteen 3-bit luma steps that index
   g_MovieChromaLumaToArgb; the skip tokens leave runs of blocks unchanged. Returns the encoded bytes consumed,
   rounded up to eight, so the caller can advance the stream. heightPixels and widthPixels must be at least 4
   (Movie_Open checks the header).
   The original read the stream without an end. Bounded here because the stream comes from the file: a token
   that does not fit before encodedEnd stops the frame (the rest of the image keeps the previous frame) and the
   result counts only the bytes up to encodedEnd. Every token of a well-formed frame lies before the end, so
   its result is unchanged.
*/
uint32_t Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          const uint8_t *encodedFrame,const uint8_t *encodedEnd)

{
  uint32_t blockWord0;
  uint32_t token;
  uint32_t tokenBytes;
  const uint8_t *streamCursor;
  uint32_t blocksLeftInRow;
  uint32_t blockRowsLeft;
  uint32_t skipRemaining;
  static Bool8 s_truncationLogged;

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
        if (streamCursor >= encodedEnd) {
          goto truncated;
        }
        blockWord0 = Movie_LoadStreamDword(streamCursor,encodedEnd);
        token = blockWord0 & MOVIE_TOKEN_MASK;
        tokenBytes = token < MOVIE_TOKEN_SKIP_SHORT ? 8 :
                     (token == MOVIE_TOKEN_SKIP_SHORT ? 1 : (token < MOVIE_TOKEN_SKIP_LONG ? 2 : 4));
        if ((size_t)(encodedEnd - streamCursor) < tokenBytes) {
          goto truncated;
        }
        if (token < MOVIE_TOKEN_SKIP_SHORT) {
          Movie_DecodeColorBlock(destinationArgb,widthPixels,blockWord0,
                                 Movie_LoadStreamDword(streamCursor + 4,encodedEnd));
        }
        /* skip tokens: this block plus skipRemaining further blocks keep the previous frame */
        else if (token == MOVIE_TOKEN_SKIP_SHORT) {
          skipRemaining = (blockWord0 & 0xff) >> 5;
        }
        else if (token < MOVIE_TOKEN_SKIP_LONG) {
          skipRemaining = ((blockWord0 & 0xffff) >> 5) + MOVIE_SKIP_SHORT_MAX_BLOCKS;
        }
        else {
          skipRemaining = (blockWord0 >> 5) + MOVIE_SKIP_MEDIUM_MAX_BLOCKS;
        }
        streamCursor = streamCursor + tokenBytes;
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
truncated:
  if (!s_truncationLogged) {
    s_truncationLogged = true;
    Thandor_Log("Movie_DecodeFrame4x4Delta: frame data ends inside a frame; frame truncated");
  }
  return (uint32_t)(encodedEnd - encodedFrame);
}

/* Not in the original: the original executable carries g_MovieChromaLumaToArgb precomputed
   (1024 x 32 ARGB values). An FLM colour is a 10-bit chroma code and a 5-bit luma:
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
   Movie_DecodeFrame4x4Delta looks up) from the two tables above. Called once at startup by WinMain (src/platform/bootstrap/main.cpp),
   before any movie is decoded. */
void Movie_BuildChromaLumaTable()
{
  int saturation;
  int hue;
  int luma;

  for (saturation = 0; saturation < 32; saturation++) {
    for (hue = 0; hue < 32; hue++) {
      int cosTerm = k_MovieChromaCosTerm[saturation][hue];
      int sinTerm = k_MovieChromaSinTerm[saturation][hue];
      uint32_t *row = &g_MovieChromaLumaToArgb[(saturation * 32 + hue) * MOVIE_CHROMA_LUMA_ROW];
      for (luma = 0; luma < 32; luma++) {
        int grey = luma * 8;
        row[luma] = ARGB8888_ALPHA_MASK | MovieColor_ClampChannel(grey - 2 * cosTerm) << 16 |
                    MovieColor_ClampChannel(grey + cosTerm - sinTerm) << 8 |
                    MovieColor_ClampChannel(grey + cosTerm + sinTerm);
      }
    }
  }
}
