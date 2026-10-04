/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/loading_movie.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/gameplay/session/loading_movie.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

uint32_t g_MoviePlaybackCurrentFrame = 0;

/* Implementation ownership: gameplay/session/loading_movie. */

/* Per-tick callback of a movie played inside a session: counts the tick, works out which frame the movie
   should show by now (8 frames per g_MoviePlaybackScheduleSpan ticks, offset by the base frame group), catches
   up to it, presenting at least every 8th frame on the way, then runs the regular simulation and network tick
   so the session keeps going under the movie.
*/
void MoviePlayback_AdvanceScheduledFrameAndTick()

{
  uint32_t targetFrame;
  uint32_t boundaryFrame;

  g_MoviePlaybackScheduleCounter++;
  if (g_MoviePlaybackScheduleSpan != 0) {
    targetFrame = (uint32_t)(g_MoviePlaybackScheduleCounter * 8) / g_MoviePlaybackScheduleSpan + 1 +
                  g_MoviePlaybackBaseFrameGroup * 8;
    for (boundaryFrame = 8; boundaryFrame < targetFrame; boundaryFrame = boundaryFrame + 8) {
      if (g_MoviePlaybackCurrentFrame < boundaryFrame) {
        MoviePlayback_AdvanceToFrameAndPresent(boundaryFrame);
      }
    }
    if (targetFrame != g_MoviePlaybackCurrentFrame) {
      MoviePlayback_AdvanceToFrameAndPresent(targetFrame);
    }
  }
  InGameRuntime_UpdateSimulationAndNetworkTick();
  return;
}

/* Decodes movie frames until g_MoviePlaybackCurrentFrame reaches targetFrame, then redraws the whole UI (which
   shows the movie texture) and presents it once. Stops without drawing, and without updating the frame
   counter, when the movie ends or cannot deliver a frame.
*/
void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame)

{
  uint32_t frameIndex;

  frameIndex = g_MoviePlaybackCurrentFrame;
  if (g_MoviePlaybackCurrentFrame < targetFrame) {
    do {
      frameIndex++;
      if (!Movie_AdvanceFrame(nullptr,nullptr)) {
        return;
      }
    } while (frameIndex < targetFrame);
    g_MoviePlaybackCurrentFrame = frameIndex;
    UiRootStack_InvalidateAll();
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
  return;
}
