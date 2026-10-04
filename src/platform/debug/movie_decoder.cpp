/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/movie_decoder.cpp
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/movie/runtime/playback.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
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
      CreateDirectoryA((LPCSTR)"moviedump", NULL);
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
        fwrite(movie->argbPixels + (int32_t)(y * width), 4, width, file);
      }
      fclose(file);
    }
  }
}
