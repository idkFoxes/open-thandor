/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/flm_encoder.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/flm_encoder.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

static const uint64_t g_MovieDeltaRgbHighNibbleMask2Pixels = 0xF0F0F000F0F0F0ull;

/* Implementation ownership: movie/runtime/flm_encoder. */

/* MMX lane helpers for the 4x4 block encoders (not in the original, which does this inline with MMX
   instructions; the helpers are inlined into Movie_EncodeFrame4x4Keyframe/Delta). Per 4-pixel row the original
   loads each pixel (MOVD), duplicates each byte into a word (PUNPCKLBW with itself), shifts the words right by
   6 (PSRLW) and adds them into an accumulator (PADDW) -- four 16-bit channel sums, one lane per pixel byte,
   wrapping at 16 bits. After four rows PSRLW by 6, PACKUSWB and MOVD pack the four averages back into one
   pixel. */

/* One byte lane of one pixel after PUNPCKLBW with itself and PSRLW by 6. */
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

/* PSRLW by 6, PACKUSWB, MOVD: the four channel averages as one pixel. The saturation to 0xFF can
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
   provider is called with NULL for the next frame and with a frame to release it; it ends the sequence with
   noFrame set and frameOrError 0xFFFFFFFF. Returns true and stores the total byte count in *outByteCount, or returns false
   (*outByteCount untouched, the buffer already written) when the provider yields no frame at all or ends
   with any other error. A leftover of the movie tools: no caller found in src/.
*/
Bool8 Movie_EncodeFlmBufferFromFrameProvider
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
    rowPixels = blockPixels + (int32_t)(row * rowStridePixels);
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
    rowPixels = blockPixels + (int32_t)(row * rowStridePixels);
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
