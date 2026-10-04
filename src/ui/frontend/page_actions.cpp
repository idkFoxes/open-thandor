/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/page_actions.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/page_actions.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

FrontendUiActionHandlerPage20Prefix g_FrontendUiActionHandlersPage20 = {
    .handlers00_54 = {
        /*  0 */ THANDOR_FN(FrontendSessionAction_ResetNetworkAndReturnToMainPage),
        /*  1 */ THANDOR_FN(FrontendNetworkSetupPage_InitializeFromCommandLine),
        /*  2 */ THANDOR_FN(FrontendNetworkSettings_PublishSelectedPlayerDescriptor),
        /*  3 */ THANDOR_FN(FrontendTransferPage_OpenAndRequestMailbox),
        /*  4 */ THANDOR_FN(FrontendNetworkSetupPage_InitializeSingleLocalPlayer),
        /*  5 */ THANDOR_FN(FrontendPlayerSetup_OpenLocalPageAndResetRoster),
        /*  6 */ THANDOR_FN(FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag),
        /*  7 */ THANDOR_FN(FrontendNetworkSettings_SetPlayerCount),
        /*  8 */ THANDOR_FN(FrontendNetworkSettings_SetGameName),
        /*  9 */ THANDOR_FN(FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick),
        /* 10 */ THANDOR_FN(FrontendTransferPage_ResetSessionOpenAndRequestMailbox),
        /* 11 */ THANDOR_FN(FrontendPlayerSetup_ExpireSelectedRuntimeBlock),
        /* 12 */ THANDOR_FN(FrontendHostLobby_UpdateKickButtonForSelection),
        /* 13 */ THANDOR_FN(FrontendTransferPage_ValidateInputAndRequestMailbox),
        /* 14 */ THANDOR_FN(FrontendRecentText_TrimAndSortTopFive),
        /* 15 */ THANDOR_FN(FrontendNetworkSetup_OpenSelectedBackend),
        /* 16 */ THANDOR_FN(FrontendOptionsAction_ReturnToMainOrOptionsPage),
        /* 17 */ THANDOR_FN(FrontendDisplaySettingsAction_OpenPageAndListModes),
        /* 18 */ THANDOR_FN(FrontendGraphicsSettings_OpenAndSynchronize),
        /* 19 */ THANDOR_FN(FrontendAudioSettings_OpenAndSynchronize),
        /* 20 */ THANDOR_FN(FrontendShadingSettings_SetEnabled),
        /* 21 */ THANDOR_FN(FrontendShadingSettings_ApplyLevel),
        /* 22 */ THANDOR_FN(FrontendModelSettings_SetLodDepthThresholdQ8),
        /* 23 */ THANDOR_FN(FrontendTextureSettings_SetQuality),
        /* 24 */ THANDOR_FN(FrontendAudioSettings_SetEffectsEnabled),
        /* 25 */ THANDOR_FN(FrontendAudioSettings_SetMusicEnabled),
        /* 26 */ THANDOR_FN(FrontendAudioSettings_SetReverseStereo),
        /* 27 */ THANDOR_FN(FrontendAudioSettings_SetEffectsGain),
        /* 28 */ THANDOR_FN(FrontendAudioSettings_SetMovieDefaultGain),
        /* 29 */ THANDOR_FN(FrontendAudioSettings_SetMusicGain),
        /* 30 */ THANDOR_FN(nullptr), /* the original's colour depth choices, gone (32-bit colour only) */
        /* 31 */ THANDOR_FN(nullptr), /* the original's colour depth choices, gone (32-bit colour only) */
        /* 32 */ THANDOR_FN(nullptr), /* the original's colour depth choices, gone (32-bit colour only) */
        /* 33 */ THANDOR_FN(nullptr), /* the original's colour depth choices, gone (32-bit colour only) */
        /* 34 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 35 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 36 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 37 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 38 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 39 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 40 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 41 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 42 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 43 */ THANDOR_FN(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 44 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectAdapter),
        /* 45 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectAdapter),
        /* 46 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectAdapter),
        /* 47 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectAdapter),
        /* 48 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectAdapter),
        /* 49 */ THANDOR_FN(FrontendDisplaySettings_ApplyMode),
        /* 50 */ THANDOR_FN(FrontendNetworkSettings_SetPlayerName),
        /* 51 */ THANDOR_FN(FrontendQuitDialogAction_ReturnToMainPage),
        /* 52 */ THANDOR_FN(FrontendCallback_ReturnToMainPageOrDispatchState4),
        /* 53 */ THANDOR_FN(FrontendScenarioPage_OpenSaveRecordsAndRefresh),
        /* 54 */ THANDOR_FN(FrontendScenarioPage_OpenLevelRecordsAndRefresh),
        /* 55 */ THANDOR_FN(FrontendScenarioPage_OpenCampaignRecordsAndRefresh),
        /* 56 */ THANDOR_FN(FrontendScenarioSelection_ActivateSelectedRecord),
        /* 57 */ THANDOR_FN(FrontendScenarioSelection_SelectOrStartSavedGame),
        /* 58 */ THANDOR_FN(FrontendScenarioSelection_SelectOrStartLevel),
        /* 59 */ THANDOR_FN(FrontendScenarioSelection_SelectOrStartCampaign),
        /* 60 */ THANDOR_FN(FrontendGameplaySettings_SetAutomaticZoomOff),
        /* 61 */ THANDOR_FN(FrontendGameplaySettings_SetAutomaticRotationOff),
        /* 62 */ THANDOR_FN(FrontendGameplaySettings_SetLinkRotationZoom),
        /* 63 */ THANDOR_FN(FrontendGameplaySettings_SetLinkRotationTilt),
        /* 64 */ THANDOR_FN(FrontendFactionSetupAction_ReturnToMainPage),
        /* 65 */ THANDOR_FN(FrontendScenarioAction_StartFieldGridLoad),
        /* 66 */ THANDOR_FN(FrontendPlayerConsensus_SubmitSelectedValue),
        /* 67 */ THANDOR_FN(FrontendSessionAction_ApplyGameSpeedAndReturnToMainPage),
        /* 68 */ THANDOR_FN(FrontendFactionSetupAction_CycleFactionColour),
        /* 69 */ THANDOR_FN(FrontendFactionSetupAction_ToggleFactionActive),
        /* 70 */ THANDOR_FN(FrontendFactionSetupAction_ChooseFaction),
        /* 71 */ THANDOR_FN(FrontendSessionAction_ApplySpeedOrToggleReady),
        /* 72 */ THANDOR_FN(FrontendSessionAction_CloseMovieAndReturnToMainPage),
        /* 73 */ THANDOR_FN(FrontendGameplaySettings_SetRightButtonDoesNotScroll),
        /* 74 */ THANDOR_FN(FrontendGameplaySettings_SetGameSpeedPercent),
        /* 75 */ THANDOR_FN(FrontendGameplaySettings_SetCameraScrollStep),
        /* 76 */ THANDOR_FN(FrontendPlayerMessage_SubmitSevenSlotText),
        /* 77 */ THANDOR_FN(FrontendNetworkSettings_SetNetworkSpeed),
        /* 78 */ THANDOR_FN(FrontendAudioSettings_SetMovieAlternateGain),
        /* 79 */ THANDOR_FN(FrontendSessionAction_ReleaseCampaignAndReturnToMainPage),
        /* 80 */ THANDOR_FN(FrontendCallback_NoOpArg1),
        /* 81 */ THANDOR_FN(FrontendGameplaySettings_SetHidePanel),
        /* 82 */ THANDOR_FN(FrontendScenarioPage_OpenSaveRecordsAndRefresh),
        /* 83 */ THANDOR_FN(FrontendScenarioPage_OpenLevelRecordsAndRefresh),
        /* 84 */ THANDOR_FN(FrontendScenarioPage_OpenCampaignRecordsAndRefresh)
    },
    .scenarioCatalogRebuildCallbacks = {
        /* 0 */ THANDOR_FN(ScenarioCatalog_RebuildSaveRecordListPage),
        /* 1 */ THANDOR_FN(ScenarioCatalog_RebuildLevelRecordListPage),
        /* 2 */ THANDOR_FN(ScenarioCatalog_RebuildCampaignRecordListPage)
    },
    /* not in the original: actions 0x2058..0x205A, the display mode kind choices */
    .handlers58_5A = {
        /* 88 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectDisplayModeKind),
        /* 89 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectDisplayModeKind),
        /* 90 */ THANDOR_FN(FrontendDisplaySettingsAction_SelectDisplayModeKind)
    }};

/* Implementation ownership: ui/frontend/page_actions. */

/* Handler of action 0x2050 (slot 80 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Save" button of
   the in-game variant of the mission briefing: does nothing.
*/
void FrontendCallback_NoOpArg1(void *source)

{
  return;
}

/* Handler of action 0x2034 (slot 52 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Choose game"
   page's "Cancel" button: returns to the main page with ROM action record 0 in a local game and with record 4
   (as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE) in a network game.
*/
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

{
  /* the inner tests repeat the outer one, so only the local-direct and network-queued paths are reachable */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,4);
  }
  return;
}

/* Handler of action 0x2033 (slot 51 of g_FrontendUiActionHandlersPage20.handlers00_54), the quit dialog's "no"
   button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a
   network game).
*/
void FrontendQuitDialogAction_ReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}

/* Handler of action 0x2010 (slot 16 of g_FrontendUiActionHandlersPage20.handlers00_54), shared by the options
   page's "Ok" button and the display settings page's "Back" button: "Ok" returns to the main page (ROM action
   record 0, FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a network game), "Back" reopens the options page.
*/
void FrontendOptionsAction_ReturnToMainOrOptionsPage(UiNodeBase *sourceNode)

{
  FrontendModelPointerContextFlags *compactLayoutFlags;
  uintptr_t parentNodeAddress;
  FrontendRootPageState *frontendRootPage;

  parentNodeAddress = (uintptr_t)sourceNode->parent;
  frontendRootPage = (FrontendRootPageState *)sourceNode;
  while ((UiNodeBase *)parentNodeAddress != UI_NODE_NONE) {
    frontendRootPage = (FrontendRootPageState *)(frontendRootPage->rootNode).parent;
    parentNodeAddress = (uintptr_t)(frontendRootPage->rootNode).parent;
  }
  if (sourceNode == &frontendRootPage->returnToMainActionControl) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
    return;
  }
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,&frontendRootPage->primaryPageStack);
  return;
}
