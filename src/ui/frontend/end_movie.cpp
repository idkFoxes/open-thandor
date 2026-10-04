/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/end_movie.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/end_movie.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

uint32_t g_EndMoviePendingTicks = 0;

static uint16_t g_FrontendEndMoviePathUtf16[17] = {'f', 'l', 'm', '\\', 'e', 'n', 'd', 'e', '0', '0', '0', '0', '.', 'f', 'l', 'm', 0}; /* L"flm\\ende0000.flm" */

static uint16_t g_EndGameElapsedTimeScratchUtf16[64] = {0};

/* The end movie's keyboard fallback returns nothing; the root keyboard fallback slot returns Bool8, but its only
   caller (UiKeyboard_DispatchPendingEvents) ignores the result, so false is returned. */
static Bool8 EndMovieSlot_KeyboardFallback(UiKeyboardStateMask keyboardStateMask,UiActionId keyCode,UiRootNode *uiRoot)

{
  EndMovieUiRuntime_DispatchCommandByFlags(keyboardStateMask,keyCode,uiRoot);
  return false;
}

/* Campaign level records (CampaignLevelRecord) as in OldUnitRuntime_RebuildScenarioReplayTables: finds the
   current level's record and points g_EndMoviePath at flm\endeNNNN.flm with its end movie number for the
   outcome g_EndMovieSelectionIndex (separate numbers for a nonzero / zero variant index). */
static void FrontendEndMovie_SelectCampaignMoviePath(CampaignAsset *campaign)
{
  int remainingRecords;
  CampaignLevelRecord *levelRecord;
  int32_t endMovieNumber;

  remainingRecords = campaign->levelRecordCount;
  levelRecord = campaign->levels;
  do {
    if (campaign->currentLevelId == levelRecord->levelId) {
      if (g_EndMovieVariantIndex == 0) {
        endMovieNumber = levelRecord->endMovieNumbers[g_EndMovieSelectionIndex];
      }
      else {
        endMovieNumber = levelRecord->endMovieNumbersVariant[g_EndMovieSelectionIndex];
      }
      /* four zero-padded digits over the "0000" of flm\ende0000.flm */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,endMovieNumber,(uint16_t *)(g_FrontendEndMoviePathUtf16 + 8));
      g_EndMoviePath = (uint16_t *)g_FrontendEndMoviePathUtf16;
      return;
    }
    levelRecord++;
    remainingRecords--;
  } while (remainingRecords != 0);
}

/* Fills the whole framebuffer with opaque black and presents it (skipped when the buffer cannot be
   accessed). */
static void FrontendEndMovie_ClearAndPresentBlackFrame()
{
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
               ,0,UI_ARGB_OPAQUE_BLACK,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
}

/* Results page after the end movie: scores of every faction that took part, elapsed time and level title;
   then draws frames until a results button sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED. Nothing is shown
   when no faction took part. */
static void FrontendEndMovie_ShowResultsPage(InGameRuntimeRoot *runtimeRoot)
{
  int activeFactionCount;
  int factionIndex;
  uint64_t elapsedTimeUnits;
  uint16_t *resultsText;
  TextResourceId levelTitleResourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendResultsEightColumnTemplate *firstChart;
  uint32_t previousColumnCount;
  int remainingColumns;
  uint32_t *copySource;
  uint32_t *copyDestination;

  /* scores of every faction 1..7 that took part (lifecycle state not 0) */
  activeFactionCount = 0;
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      activeFactionCount++;
      GameFactionRuntime_RecomputeProgressAndScoreMetrics(factionIndex,&runtimeRoot->worldRuntime);
    }
  }
  if (activeFactionCount == 0) {
    return;
  }
  /* row counts of the three results lists */
  firstChart = (FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart1);
  firstChart->rowCount = activeFactionCount;
  ((FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart2))->rowCount = activeFactionCount;
  ((FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart3))->rowCount = activeFactionCount;
  /* elapsed minutes of the 80 Hz clock, rounded up, shown as hours and minutes */
  elapsedTimeUnits = (uint64_t)(g_GameFactionRuntimeImage.tail.periodicClockTick + 4799) / 4800;
  g_LocaleFormatTimeFieldsUtf16
            ((uint32_t)(elapsedTimeUnits / 60),(uint32_t)(elapsedTimeUnits % 60),
             g_EndGameElapsedTimeScratchUtf16);
  resultsText = TextResource_Resolve(TEXT_ID_RESULTS_TITLE_TEMPLATE);
  levelTitleResourceId = g_InGameLevelTitleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE;
  RichTextCommandStream_PatchPayloadBySelector(1,g_EndGameElapsedTimeScratchUtf16,resultsText);
  RichTextCommandStream_PatchPayloadBySelector(0,TextResource_Resolve(levelTitleResourceId),resultsText);
  /* the continue button; 0x1025 is the second results button, local games hide it */
  UiNodeList_UnsuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_SECONDARY_EXIT,(UiNodeBase *)runtimeRoot);
  }
  /* a host with other players waits for them instead of offering continue */
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
     && (1 < g_FrontendPlayerRuntimeBlockCount)) {
    UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
  }
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    playerBlock->factionAssignment.readyOrWaitState = 0;
    remainingPlayerBlocks--;
    playerBlock++;
  } while (remainingPlayerBlocks != 0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: remove column 2 (the third) of resultsChart1; the shift copies the count - 3 later ones */
    previousColumnCount = firstChart->columnTypeCount;
    firstChart->columnTypeCount = firstChart->columnTypeCount - 1;
    remainingColumns = previousColumnCount - 3;
    if (2 < previousColumnCount && remainingColumns != 0) {
      copySource = &firstChart->columnTypes[3];
      copyDestination = &firstChart->columnTypes[2];
      for (; remainingColumns != 0; remainingColumns--) {
        *copyDestination = *copySource;
        copySource++;
        copyDestination++;
      }
    }
  }
  do {
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(0xffffffff);
    }
  } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED) == 0);
}

/* End of a mission: plays the end movie chosen by the current scenario's record in the loaded campaign
   (flm\endeNNNN.flm for the outcome g_EndMovieSelectionIndex and variant g_EndMovieVariantIndex) on the
   in-game root, then shows the results page (per-faction scores, elapsed time, level title) until a results
   button sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED. Without an in-game root or end movie path, or when the
   movie cannot be opened, it only installs the results-screen callbacks.
*/
void Frontend_PlaySelectedEndMovie()

{
  UiRootCallbacks *rootCallbacks;
  InGameRuntimeRoot *runtimeRoot;
  uint32_t playbackRateHz;
  Bool8 movieOpened;
  MovieRuntime *endMovieRuntime;
  Bool8 endMovieAdvanced;

  runtimeRoot = g_InGameRuntimeRoot;
  g_GraphicsCursorSetFrame(0);
  g_CursorVisibilityToken--;
  if ((runtimeRoot != nullptr) && (g_EndMoviePath != nullptr)) {
    rootCallbacks = runtimeRoot->rootUi.callbacks;
    rootCallbacks->keyboardFallback = UI_SLOT(EndMovieSlot_KeyboardFallback);
    rootCallbacks->frameUpdate = UI_SLOT(EndMovieUiRuntime_HandleModeTransition);
    if (g_FrontendLoadedCampaignAsset != 0) {
      FrontendEndMovie_SelectCampaignMoviePath((CampaignAsset *)g_FrontendLoadedCampaignAsset);
    }
    Movie_Close();
    /* clear both buffers to black */
    FrontendEndMovie_ClearAndPresentBlackFrame();
    FrontendEndMovie_ClearAndPresentBlackFrame();
    movieOpened = Movie_Open(MOVIE_OPEN_STREAM,g_EndMoviePath,&playbackRateHz,nullptr);
    runtimeRoot = g_InGameRuntimeRoot;
    if (movieOpened) {
      g_EndMoviePendingTicks = 0;
      g_TimerRegisterPeriodic(playbackRateHz,FrontendSession_PeriodicTick);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(runtimeRoot,primaryPageStack));
      endMovieRuntime = runtimeRoot->activeEndMovieRuntime;
      endMovieAdvanced = Movie_AdvanceFrame(&endMovieRuntime,nullptr);
      runtimeRoot->activeEndMovieRuntime = endMovieRuntime;
      if (endMovieAdvanced) {
        runtimeRoot->endMoviePlaybackState = 0;
        g_EndMoviePendingTicks = 0;
        /* one movie frame per timer tick until the movie ends (or the end-movie flag is cleared elsewhere) */
        do {
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks--;
            if (!Movie_AdvanceFrame(nullptr,nullptr)) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)runtimeRoot);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0);
      }
      g_CursorVisibilityToken++;
      UiPageStack_SetActiveIndex(1,&runtimeRoot->endMoviePageStack);
      FrontendEndMovie_ShowResultsPage(runtimeRoot);
      g_TimerUnregisterPeriodic(FrontendSession_PeriodicTick);
      Movie_Close();
    }
    else {
      g_CursorVisibilityToken++;
    }
  }
  else {
    g_CursorVisibilityToken++;
  }
  rootCallbacks = g_InGameRuntimeRoot->rootUi.callbacks;
  rootCallbacks->keyboardFallback = UI_SLOT(InGameHotkeys_DispatchCommandByFlags);
  rootCallbacks->frameUpdate = UI_SLOT(InGameUiRoot_UpdateFrame);
  return;
}
