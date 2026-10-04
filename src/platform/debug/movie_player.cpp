/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/movie_player.cpp
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/font.h>
#include <thandor/platform/debug/movie_player.h>

/* Debug tool: movie test player.
   OPEN_THANDOR_MOVIE=<name>  plays flm\<name>.flm,
   OPEN_THANDOR_MOVIE=all     plays every name listed in movies.txt (one per line, working dir).
   Each movie runs at its own rate for at most 10 seconds; a key or mouse click skips to the next.
   The name and frame counter are drawn top left. OPEN_THANDOR_MOVIE_STRETCH=1 draws full screen
   with the end-movie bilinear stretch. The process exits after the last movie. */

/* Fills the framebuffer with opaque black and presents it (called twice to clear both page buffers). */
static void DebugMovie_ClearScreen()
{
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0,
               ARGB8888_ALPHA_MASK,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
}

/* Plays one movie; returns after the end, 10 seconds, or a key/click. */
static void DebugMovie_PlayOne(const char *name, int index, int count, int stretch)
{
  uint16_t path[64];
  char label[128];
  int pathLength = 0;
  int nameIndex;
  unsigned start;
  uint32_t playbackRateHz;
  uint32_t openError;
  MovieRuntime *movie;
  /* UTF-16 path "flm\<name>.flm"; the name is cut so ".flm" and the terminator still fit */
  path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm'; path[pathLength++] = '\\';
  for (nameIndex = 0; name[nameIndex] != 0 && pathLength < 56; nameIndex++) {
    path[pathLength++] = (uint16_t)name[nameIndex];
  }
  path[pathLength++] = '.'; path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm';
  path[pathLength] = 0;
  DebugMovie_ClearScreen();
  DebugMovie_ClearScreen();
  if (!Movie_Open(1,path,&playbackRateHz,&openError)) {
    Thandor_Log("debug movie %d/%d %s: Movie_Open failed (eax=%08x)", index, count, name, openError);
    sprintf(label, "Video %d/%d: %s.flm - OEFFNEN FEHLGESCHLAGEN", index, count, name);
    if (!g_GraphicsFramebufferBeginAccess()) {
      DebugFont_DrawText(8, 8, label);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
    Thandor_SleepMs(1500);
    return;
  }
  if (!Movie_AdvanceFrame(&movie,nullptr)) {
    Thandor_Log("debug movie %d/%d %s: first frame failed", index, count, name);
    Movie_Close();
    return;
  }
  Thandor_Log("debug movie %d/%d %s: playing, %u frames at %u Hz", index, count, name,
              g_ActiveMovie->fileHeader->frameCount, playbackRateHz);
  g_IntroMoviePendingTicks = 0;
  UiFrame_FlushInputAndResetPendingTicks();
  g_TimerRegisterPeriodic(playbackRateHz,IntroMovie_TimerTick);
  start = Thandor_TickCount();
  for (;;) {
    uint32_t keyCode;
    uint32_t keyStateMask;
    CursorPointerEvent cursor;
    g_Win32PumpMessages();
    if (g_KeyboardReadEvent(&keyCode,&keyStateMask)) break;
    if (g_GraphicsCursorConsumeEvent(&cursor) && RIGHT_PRESS < cursor.eventType) break;
    if (Thandor_TickCount() - start > 10000) break;
    if (g_IntroMoviePendingTicks != 0) {
      int burst = 3;
      int ended = 0;
      do {
        if (!Movie_AdvanceFrame(nullptr,nullptr)) { ended = 1; break; }
        g_IntroMoviePendingTicks--;
      } while ((g_IntroMoviePendingTicks != 0) && (--burst != 0));
      if (ended) break;
      if (g_GraphicsFramebufferBeginAccess()) break;
      if (stretch) {
        g_GraphicsTextureSourceStretchDirectColorBilinear
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,0,
                   (GraphicsTextureSourceAsset *)movie,g_FramebufferAccess);
      }
      else {
        MovieFrameDimensions size = Movie_GetFrameDimensions();
        uint32_t height = g_FramebufferHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,
                   ((int)((height - (height >> 2)) - (int)size.height) >> 1) + (height >> 3),
                   (int)(g_FramebufferWidth - (int)size.width) >> 1,0,
                   (GraphicsTextureSourceAsset *)movie,g_FramebufferAccess);
      }
      sprintf(label, "Video %d/%d: %s.flm  Frame %u/%u", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount);
      DebugFont_DrawText(8, 8, label);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
  }
  Thandor_Log("debug movie %d/%d %s: stopped at frame %u/%u after %u ms", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount,
              Thandor_TickCount() - start);
  g_TimerUnregisterPeriodic(IntroMovie_TimerTick);
  Movie_Close();
}

/* Debug tool: OPEN_THANDOR_MOVIEEXPORT=<name>[,<name>...] decodes flm\<name>.flm frame by frame (as fast
   as the stream allows) and writes moviedump\<name>.rgb (32-bit BGRA frames, top-down) and moviedump\<name>.txt
   (width height frames rate); the soundtrack is not exported. The process exits afterwards. */
void DebugMovie_ExportOne(const char *name)
{
  uint16_t path[64];
  char fileName[128];
  int pathLength = 0;
  int nameIndex;
  uint32_t frames = 0;
  uint32_t width;
  uint32_t height;
  uint32_t playbackRateHz;
  uint32_t openError;
  Bool8 frameDecoded;
  FILE *video;
  FILE *info;
  /* L"flm\<name>.flm", the name cut so that the extension and terminator still fit */
  path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm'; path[pathLength++] = '\\';
  for (nameIndex = 0; name[nameIndex] != 0 && pathLength < 56; nameIndex++) {
    path[pathLength++] = (uint16_t)name[nameIndex];
  }
  path[pathLength++] = '.'; path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm';
  path[pathLength] = 0;
  CreateDirectoryA((LPCSTR)"moviedump", nullptr);
  if (!Movie_Open(1,path,&playbackRateHz,&openError)) {
    Thandor_Log("movie export %s: Movie_Open failed (eax=%08x)", name, openError);
    return;
  }
  width = g_ActiveMovie->fileHeader->widthPixels;
  height = g_ActiveMovie->fileHeader->heightPixels;
  /* the soundtrack is not exported: the SDL3 audio backend keeps no sound buffer to read it from */
  sprintf(fileName, "moviedump\\%s.rgb", name);
  video = fopen(fileName, "wb");
  for (;;) {
    int attempts = 0;
    do {
      frameDecoded = Movie_AdvanceFrame(nullptr,nullptr);
      if (frameDecoded) break;
      Thandor_SleepMs(5); /* the refill worker may not have loaded the next frame yet */
    } while (++attempts < 200 && g_ActiveMovie != nullptr &&
             g_ActiveMovie->currentFrameIndex < g_ActiveMovie->fileHeader->frameCount);
    if (!frameDecoded) break;
    if (video != nullptr) fwrite(g_ActiveMovie->argbPixels, 4, width * height, video);
    frames++;
  }
  if (video != nullptr) fclose(video);
  sprintf(fileName, "moviedump\\%s.txt", name);
  info = fopen(fileName, "w");
  if (info != nullptr) {
    fprintf(info, "%u %u %u %u\n", width, height, frames, playbackRateHz);
    fclose(info);
  }
  Thandor_Log("movie export %s: %ux%u, %u frames at %u Hz", name, width, height, frames, playbackRateHz);
  Movie_Close();
}

/* Debug tool: OPEN_THANDOR_MOVIE=<name> plays flm\<name>.flm, OPEN_THANDOR_MOVIE=all plays every name
   listed in movies.txt (up to 256, one per line) one after another, each with a frame counter overlay.
   OPEN_THANDOR_MOVIE_STRETCH=1 stretches the frames to the screen. The process exits afterwards. */
void DebugMovie_Run(const char *which)
{
  const char *stretchValue = getenv("OPEN_THANDOR_MOVIE_STRETCH");
  int stretch = (stretchValue != nullptr) && (stretchValue[0] == '1');
  if (strcmp(which, "all") == 0) {
    static char names[256][24];
    int count = 0;
    int i;
    FILE *list = fopen("movies.txt", "r");
    if (list == nullptr) {
      Thandor_Log("debug movie: movies.txt not found");
      ExitProcess(1);
    }
    while ((count < 256) && (fgets(names[count], sizeof names[count], list) != nullptr)) {
      char *end = names[count] + strlen(names[count]);
      while ((end > names[count]) && ((end[-1] == '\n') || (end[-1] == '\r') || (end[-1] == ' '))) *--end = 0;
      if (names[count][0] != 0) count++;
    }
    fclose(list);
    /* OPEN_THANDOR_MOVIE_START=<n> resumes the list at movie n (1-based). */
    i = (getenv("OPEN_THANDOR_MOVIE_START") != nullptr) ? atoi(getenv("OPEN_THANDOR_MOVIE_START")) - 1 : 0;
    if (i < 0) i = 0;
    for (; i < count; i++) {
      DebugMovie_PlayOne(names[i], i + 1, count, stretch);
    }
  }
  else {
    DebugMovie_PlayOne(which, 1, 1, stretch);
  }
  Thandor_Log("debug movie: finished");
  ExitProcess(0);
}
