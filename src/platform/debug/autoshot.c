/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/autoshot.c
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/platform/system/win32.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/autoshot.h>

/* Test aid: OPEN_THANDOR_AUTOSHOT=<milliseconds> saves the game's own framebuffer to
   shots\shot_NNNN.bmp at that interval (checked from the message pump), so automated runs can be
   looked at without capturing the desktop. */
void DebugAutoShot_Tick(void)
{
  static int interval = -1;
  static unsigned last;
  static unsigned number;
  unsigned now;
  GraphicsCapturedTextureSourceAsset *capture;
  if (interval < 0) {
    const char *value = getenv("OPEN_THANDOR_AUTOSHOT");
    interval = (value != NULL) ? atoi(value) : 0;
    if (interval > 0) {
      CreateDirectoryA("shots", NULL);
    }
    last = Thandor_TickCount();
  }
  if (interval <= 0 || g_GraphicsFramebufferCaptureRegion == NULL || g_FramebufferWidth == 0) {
    return;
  }
  now = Thandor_TickCount();
  if (now - last < (unsigned)interval) {
    return;
  }
  last = now;
  capture = g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
#ifdef THANDOR_TEST_AIDS
  if (capture == NULL) {
    Thandor_Log("autoshot failed: backend access state %d, frame heartbeat %u",
                (int)g_GraphicsBackendAccessState, g_ThandorFrameHeartbeat);
  }
#endif
  if (capture != NULL) {
    GraphicsTextureSourceEntry *entry = &capture->sourceEntry;
    const uint32_t *pixels = (const uint32_t *)((uint8_t *)capture + entry->dataOffset);
    uint32_t width = entry->pixelWidth;
    uint32_t height = entry->pixelHeight;
    char name[64];
    FILE *file;
    sprintf(name, "shots\\shot_%04u.bmp", number++);
    file = fopen(name, "wb");
    if (file != NULL) {
      /* "BM" + the rest of BITMAPFILEHEADER (14 bytes) and a BITMAPINFOHEADER (40 bytes): 32-bit top-down */
      uint32_t header[13];
      uint32_t imageBytes = width * height * 4;
      memset(header, 0, sizeof header);
      fwrite("BM", 1, 2, file);
      header[0] = 54 + imageBytes; /* bfSize */
      header[2] = 54;              /* bfOffBits */
      header[3] = 40;              /* biSize */
      header[4] = width;
      header[5] = (uint32_t)-(int)height; /* negative height: rows top to bottom */
      header[6] = 1 | (32 << 16);  /* biPlanes 1, biBitCount 32 */
      header[8] = imageBytes;      /* biSizeImage */
      fwrite(header, 4, 13, file);
      fwrite(pixels, 4, width * height, file);
      fclose(file);
      Thandor_Log("autoshot %s (%ux%u)", name, width, height);
    }
    g_MemoryApi.free(capture);
  }
}
