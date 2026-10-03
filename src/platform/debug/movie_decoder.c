/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/movie_decoder.c
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/playback.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/original_code.h>
#include <thandor/platform/debug/movie_decoder.h>

/* Debug tool: OPEN_THANDOR_MOVIEDUMP=1 logs every decoded frame (consumed bytes, stream state,
   pixel checksum) and writes every tenth frame to moviedump\frame_NNNN.bmp. */
void DebugMovieDecoder_DumpFrame(MovieRuntime *movie, uint32_t consumedBytes)
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
  /* cheap checksum over every seventh pixel, enough to spot diverging frames in the log */
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

/* Debug tool: OPEN_THANDOR_MOVIECMP=1 decodes every frame a second time with the original machine code
   of Movie_DecodeFrame4x4Delta (0x004A81C0, copied from thandor_original.exe; it uses no absolute data)
   into a shadow buffer and logs where the two results differ. Call before the C decoder runs. */
typedef uint32_t (__stdcall *OriginalMovieDecodeProc)(uint32_t heightPixels, uint32_t widthPixels,
                                                      uint32_t *destinationArgb, uint8_t *encodedFrame);
static uint32_t *s_MovieCompareShadow;
static uint32_t s_MovieCompareConsumed;

void DebugMovieDecoder_CompareBefore(MovieRuntime *movie, uint32_t height, uint32_t width, uint8_t *encoded)
{
  static int enabled = -1;
  static OriginalMovieDecodeProc original;
  if (enabled < 0) {
    const char *value = getenv("OPEN_THANDOR_MOVIECMP");
    enabled = (value != NULL) && (value[0] == '1');
    if (enabled) {
      /* the decoder's code runs from 0x004A81C0 up to and including its RET 0x10 at 0x004A8586 */
      original = (OriginalMovieDecodeProc)Thandor_LoadOriginalCodeCopy(0x4a81c0, 0x4a8589 - 0x4a81c0);
      if (original == NULL) {
        Thandor_Log("moviecmp: could not load the original decoder");
        enabled = 0;
      }
    }
  }
  if (!enabled) {
    return;
  }
  if (s_MovieCompareShadow == NULL || movie->currentFrameIndex == 0) {
    free(s_MovieCompareShadow);
    s_MovieCompareShadow = (uint32_t *)malloc(width * height * 4);
    memcpy(s_MovieCompareShadow, movie->argbPixels, width * height * 4);
  }
  s_MovieCompareConsumed = original(height, width, s_MovieCompareShadow, encoded);
  __asm emms
}

void DebugMovieDecoder_CompareAfter(MovieRuntime *movie, uint32_t height, uint32_t width, uint32_t consumed)
{
  uint32_t i;
  uint32_t differing = 0;
  uint32_t first = 0;
  if (s_MovieCompareShadow == NULL) {
    return;
  }
  for (i = 0; i < width * height; i++) {
    if (s_MovieCompareShadow[i] != movie->argbPixels[i]) {
      if (differing++ == 0) first = i;
    }
  }
  if (differing != 0 || consumed != s_MovieCompareConsumed) {
    Thandor_Log("moviecmp frame %u: %u pixels differ (first at %u,%u: C %08x original %08x), consumed C %u original %u",
                movie->currentFrameIndex, differing, first % width, first / width, movie->argbPixels[first],
                s_MovieCompareShadow[first], consumed, s_MovieCompareConsumed);
    memcpy(s_MovieCompareShadow, movie->argbPixels, width * height * 4); /* resync */
  }
}
