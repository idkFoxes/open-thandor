/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/notifications.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/notifications.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* the notification movie path; the three digits at [9] are overwritten with
   the movie number before it is opened */
/* L"flm\\movie000.flm" */
static uint16_t g_FlmMovie000FlmPathUtf16[17] = {'f', 'l', 'm', '\\', 'm', 'o', 'v', 'i', 'e', '0', '0', '0', '.', 'f', 'l', 'm', 0};

uint32_t g_InGameSessionNotificationTimeoutTicks = 0;

/* Opens the movie of the notification queue head ("flm\movie%03d.flm"). When its first frame is ready, the movie
   becomes the notification button's texture source and the head's payload the active notification; a payload with
   a map target makes the button clickable. */
static void InGameNotification_StartQueueHeadMovie(InGameRuntimeRoot *inGameRoot)

{
  uint32_t notificationMovieNumber;
  MovieRuntime *notificationMovie;

  notificationMovieNumber = inGameRoot->notificationQueue[0].movieId;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,notificationMovieNumber,&g_FlmMovie000FlmPathUtf16[9]);
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,g_FlmMovie000FlmPathUtf16,nullptr,nullptr)) {
    return;
  }
  /* movies 100-299 and 700-899 play at the alternate movie gain */
  if ((99 < notificationMovieNumber) &&
      ((notificationMovieNumber < 300) || ((699 < notificationMovieNumber) && (notificationMovieNumber < 900)))) {
    Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
  }
  if (!Movie_AdvanceFrame(&notificationMovie,nullptr)) {
    return;
  }
  inGameRoot->notificationButtonTextureSource = (uintptr_t)notificationMovie;
  inGameRoot->notificationButtonSubresource = 0;
  inGameRoot->activeNotificationPayload = inGameRoot->notificationQueue[0].payload;
  if (inGameRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE) {
    inGameRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
  }
  g_InGameSessionNotificationTimeoutTicks = 0;
  if ((inGameRoot->activeNotificationPayload).payloadKind != NOTIFICATION_PAYLOAD_NONE) {
    inGameRoot->notificationButtonCursorFrame = PAYLOAD_ACTIVE;
  }
}

/* Pops the notification queue head: moves entries 1-3 forward and clears the last entry. */
static void InGameNotification_PopQueueHead(InGameRuntimeRoot *inGameRoot)

{
  int slot;

  for (slot = 0; slot < INGAME_NOTIFICATION_QUEUE_SLOTS - 1; slot++) {
    inGameRoot->notificationQueue[slot] = inGameRoot->notificationQueue[slot + 1];
  }
  memset(&inGameRoot->notificationQueue[INGAME_NOTIFICATION_QUEUE_SLOTS - 1],0,
         sizeof(inGameRoot->notificationQueue[INGAME_NOTIFICATION_QUEUE_SLOTS - 1]));
}

/* Periodic timer that plays the queued in-game notification movies: while one plays it advances a frame and, at
   the end, closes it and keeps the notification's map target clickable for 0x280 more ticks; otherwise it starts
   the movie of the queue head ("flm\movie%03d.flm"), makes its payload the active notification and pops the
   four-entry queue.
*/
void InGameRuntime_ProcessQueuedSessionNotificationTimer()

{
  GraphicsTextureSourceAsset *panelTextureSource;
  InGameRuntimeRoot *inGameRoot;

  panelTextureSource = g_InGamePanelTextureSource;
  inGameRoot = g_InGameRuntimeRoot;
  /* the target of the last notification stays clickable until the timeout runs out */
  if (g_InGameSessionNotificationTimeoutTicks != 0) {
    g_InGameSessionNotificationTimeoutTicks--;
    if ((g_InGameSessionNotificationTimeoutTicks == 0) &&
        (inGameRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE)) {
      inGameRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
    }
  }
  /* notificationButtonTextureSource holds the playing movie, or the panel texture source when none plays */
  if (panelTextureSource != (GraphicsTextureSourceAsset *)inGameRoot->notificationButtonTextureSource) {
    if (!Movie_AdvanceFrame(nullptr,nullptr)) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 640;
      inGameRoot->notificationButtonTextureSource = (uintptr_t)panelTextureSource;
      inGameRoot->notificationButtonSubresource = 37;
    }
  }
  else if (inGameRoot->notificationQueue[0].priority != 0) {
    InGameNotification_StartQueueHeadMovie(inGameRoot);
    InGameNotification_PopQueueHead(inGameRoot);
  }
  return;
}
