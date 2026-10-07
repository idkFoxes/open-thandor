/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/mission_briefing.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/mission_briefing.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* the four level digits at index 7 are overwritten with the level number */
static uint16_t g_FrontendMissionBriefingMoviePathUtf16[16] = {'f', 'l', 'm', '\\', 'l', 'e', 'v', '0', '0', '0', '0', '.', 'f', 'l', 'm', 0}; /* L"flm\\lev0000.flm" */

/* True when one of the players' records has factionSlot as its faction assignment (the player list is assumed
   to hold at least one record). */
static Bool8 FrontendMissionBriefing_IsFactionTakenByPlayer(int factionSlot)
{
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeBlockCount playersRemaining;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  do {
    if (factionSlot == playerRecord->factionAssignment.factionAssignmentIndex) {
      return true;
    }
    playerRecord++;
    playersRemaining--;
  } while (playersRemaining != 0);
  return false;
}

/* Opens the mission briefing page (FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) for the loaded level: the
   briefing text of the local player's faction, the level title in template 0x219B, the level's briefing movie
   (level digits + .flm) as animated image, and the button set (Back/Begin from the menu; Exit instead of Back
   in a campaign or a re-initialised scenario). The opponent settings stay visible only while an active
   faction is left to the computer.
*/
void FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  FrontendUiImage *ui = FrontendUi_Image(frontendRoot);
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  UiScrollableControl *control;
  UiTextResourceId titleTextId;
  FrontendLoadedLevelAsset *loadedLevel;
  uint32_t savedGameSpeedPercent;
  int factionSlot;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  int unclaimedActiveFactions;
  RichTextExtent textExtent;
  uint16_t *briefingText;
  uint16_t *templateText;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    savedGameSpeedPercent = PersistentSettings_Read(100,PERSISTENT_SETTING_GAME_SPEED_PERCENT);
    ui->gameSpeedSlider.value = savedGameSpeedPercent;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MISSION_BRIEFING,
                             UiLayoutContainerControl_AsPageStack(&ui->frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags = &ui->menuRoomModelView.contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  /* find the local player's record (Original quirk: one past the last record if none matches) */
  loadedLevel = g_FrontendLoadedLevelAsset;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while (g_LocalPlayerRuntimeId != playerRecord->playerRuntimeId) {
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) break;
  }
  titleTextId = g_FrontendLoadedLevelAsset->header.titleTextResourceIndex;
  /* briefingText's text resource id (kept in its text pointer field): the faction's briefing entry of the
     level's text page */
  ui->briefingText.text =
       THANDOR_PTR32_BITS(playerRecord->factionAssignment.factionAssignmentIndex + TEXT_ID_LEVEL_BRIEFING_BASE +
       g_FrontendLoadedLevelAsset->header.titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE);
  briefingText = TextResource_Resolve(titleTextId + TEXT_ID_LEVEL_TITLE_BASE);
  *briefingText = FRONTEND_TEXT_STYLE_NORMAL;
  templateText = TextResource_Resolve(TEXT_ID_MISSION_BRIEFING_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,briefingText,templateText);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,loadedLevel->header.titleTextResourceIndex,
             &g_FrontendMissionBriefingMoviePathUtf16[7]);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLM,g_FrontendMissionBriefingMoviePathUtf16);
  /* briefingImage is an image action control whose template node is shorter than the class (UiNodeBase + 6 dwords) */
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,g_FrontendMissionBriefingMoviePathUtf16,nullptr,nullptr)) {
    reinterpret_cast<UiImageActionControl *>(&ui->briefingImage)->textureSource = nullptr;
  }
  else {
    if (Movie_AdvanceFrame(&firstFrameMovie,&movieEndCode)) {
      /* the movie runtime is the image's texture source (it starts with the texture asset prefix) */
      reinterpret_cast<UiImageActionControl *>(&ui->briefingImage)->textureSource =
           reinterpret_cast<GraphicsTextureSourceAsset *>(firstFrameMovie);
    }
    else {
      /* The original did not check the result and used the end code as the texture source; bounded here
         because the image would draw through that code as a pointer: no frame shows no image (logged), like a
         movie that does not open. */
      Thandor_Log("FrontendMissionBriefingPage_Initialize: briefing movie gave no frame (end code %u)",
                  (uint32_t)movieEndCode);
      reinterpret_cast<UiImageActionControl *>(&ui->briefingImage)->textureSource = nullptr;
    }
    reinterpret_cast<UiImageActionControl *>(&ui->briefingImage)->subresource = 0;
  }
  /* Back only when started from the menu by a non-client; Exit (and the Save button) in a campaign or a
     re-initialised scenario; neither for a client of a fresh scenario */
  if ((((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
      (g_FrontendLoadedCampaignAsset == 0)) && (g_FrontendScenarioInitializationCount == 0)) {
    ui->briefingBackButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->briefingBackButton.selectable.stateFlags &= ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    ui->briefingBackButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    ui->briefingBackButton.selectable.stateFlags |= FRONTEND_CONTROL_INACTIVE;
  }
  /* (when Back was shown, both of these are zero) */
  if ((g_FrontendScenarioInitializationCount != 0) || (g_FrontendLoadedCampaignAsset != 0)) {
    ui->briefingExitButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->briefingExitButton.selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    /* the original shows the Save button and switches it off again right away */
    ui->briefingSaveButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->briefingSaveButton.selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    ui->briefingSaveButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    ui->briefingSaveButton.selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
  }
  else {
    ui->briefingExitButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    ui->briefingExitButton.selectable.stateFlags |= FRONTEND_CONTROL_INACTIVE;
    ui->briefingSaveButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    ui->briefingSaveButton.selectable.stateFlags |= FRONTEND_CONTROL_INACTIVE;
  }
  /* in a network game with other players only the host starts the mission */
  ui->briefingBeginButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    ui->briefingBeginButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
  }
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    playerRecord->factionAssignment.readyOrWaitState = 0;
    playersRemaining--;
    playerRecord++;
  } while (playersRemaining != 0);
  briefingText = TextResource_Resolve((TextResourceId)(uintptr_t)ui->briefingText.text);
  textExtent = RichTextCommandStream_MeasureWrappedBlock
                     (g_UiTextStyleNormal,briefingText,ui->briefingText.wrapWidth);
  /* size the text control to the wrapped text plus a 6-pixel margin, then refit the scroller */
  ui->briefingText.base.rightOffset = textExtent.widthPixels + 6;
  ui->briefingText.base.bottomOffset = textExtent.heightPixels + 6;
  control = &ui->briefingTextScroller;
  UiScrollableControl_RebuildViewportAndScrollbars(control);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,control);
  /* The "computer opponent" slider (weak..strong) is the game speed percent; clients cannot change it. */
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  }
  /* count the active factions 1..7 that no player has taken (they are played by the computer) */
  unclaimedActiveFactions = 0;
  for (factionSlot = 1; factionSlot <= 7; factionSlot++) {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
        !FrontendMissionBriefing_IsFactionTakenByPlayer(factionSlot)) {
      unclaimedActiveFactions++;
    }
  }
  /* opponent settings: only with computer factions, and in a campaign only on its first level
     (CampaignAsset.firstLevelId) */
  if ((g_FrontendLoadedCampaignAsset == 0 ||
       reinterpret_cast<CampaignAsset *>(g_FrontendLoadedCampaignAsset)->currentLevelId ==
       reinterpret_cast<CampaignAsset *>(g_FrontendLoadedCampaignAsset)->firstLevelId) && unclaimedActiveFactions != 0) {
    ui->opponentSettingsGroup.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  else {
    ui->opponentSettingsGroup.base.nodeFlags |= UI_NODE_SUPPRESSED;
    UiNodeList_SuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  }
}
