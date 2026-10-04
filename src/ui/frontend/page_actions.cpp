/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/page_actions.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/page_actions.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Slot adapters for the handlers below whose own signature differs from the handler slot's void (void *)
   (calling through the slot type directly would be undefined behaviour). Each does the conversion the call
   through the untyped table did implicitly: the uint32_t handlers (which ignore the value) received the low 32
   bits of the source pointer, the intptr_t handler the source address, and the Bool8 result is dropped, as the
   action queue never read it. */

static void UiActionSlot_FrontendSessionAction_ApplyGameSpeedAndReturnToMainPage(void *source)

{
  FrontendSessionAction_ApplyGameSpeedAndReturnToMainPage((uint32_t)(uintptr_t)source);
}

static void UiActionSlot_FrontendSessionAction_ReleaseCampaignAndReturnToMainPage(void *source)

{
  FrontendSessionAction_ReleaseCampaignAndReturnToMainPage((uint32_t)(uintptr_t)source);
}

static void UiActionSlot_FrontendFactionSetupAction_ReturnToMainPage(void *source)

{
  FrontendFactionSetupAction_ReturnToMainPage((uint32_t)(uintptr_t)source);
}

static void UiActionSlot_FrontendQuitDialogAction_ReturnToMainPage(void *source)

{
  FrontendQuitDialogAction_ReturnToMainPage((uint32_t)(uintptr_t)source);
}

static void UiActionSlot_FrontendCallback_ReturnToMainPageOrDispatchState4(void *source)

{
  FrontendCallback_ReturnToMainPageOrDispatchState4((uint32_t)(uintptr_t)source);
}

static void UiActionSlot_FrontendScenarioSelection_ActivateSelectedRecord(void *source)

{
  FrontendScenarioSelection_ActivateSelectedRecord((intptr_t)source);
}

static void UiActionSlot_FrontendNetworkSettings_PublishSelectedPlayerDescriptor(void *source)

{
  FrontendNetworkSettings_PublishSelectedPlayerDescriptor((FrontendNetworkSettingsControlView *)source);
}

/* Module data. */

FrontendUiActionHandlerPage20Prefix g_FrontendUiActionHandlersPage20 = {
    .handlers00_54 = {
        /*  0 */ UI_SLOT(FrontendSessionAction_ResetNetworkAndReturnToMainPage),
        /*  1 */ UI_SLOT(FrontendNetworkSetupPage_InitializeFromCommandLine),
        /*  2 */ UI_SLOT(UiActionSlot_FrontendNetworkSettings_PublishSelectedPlayerDescriptor),
        /*  3 */ UI_SLOT(FrontendTransferPage_OpenAndRequestMailbox),
        /*  4 */ UI_SLOT(FrontendNetworkSetupPage_InitializeSingleLocalPlayer),
        /*  5 */ UI_SLOT(FrontendPlayerSetup_OpenLocalPageAndResetRoster),
        /*  6 */ UI_SLOT(FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag),
        /*  7 */ UI_SLOT(FrontendNetworkSettings_SetPlayerCount),
        /*  8 */ UI_SLOT(FrontendNetworkSettings_SetGameName),
        /*  9 */ UI_SLOT(FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick),
        /* 10 */ UI_SLOT(FrontendTransferPage_ResetSessionOpenAndRequestMailbox),
        /* 11 */ UI_SLOT(FrontendPlayerSetup_ExpireSelectedRuntimeBlock),
        /* 12 */ UI_SLOT(FrontendHostLobby_UpdateKickButtonForSelection),
        /* 13 */ UI_SLOT(FrontendTransferPage_ValidateInputAndRequestMailbox),
        /* 14 */ UI_SLOT(FrontendRecentText_TrimAndSortTopFive),
        /* 15 */ UI_SLOT(FrontendNetworkSetup_OpenSelectedBackend),
        /* 16 */ UI_SLOT(FrontendOptionsAction_ReturnToMainOrOptionsPage),
        /* 17 */ UI_SLOT(FrontendDisplaySettingsAction_OpenPageAndListModes),
        /* 18 */ UI_SLOT(FrontendGraphicsSettings_OpenAndSynchronize),
        /* 19 */ UI_SLOT(FrontendAudioSettings_OpenAndSynchronize),
        /* 20 */ UI_SLOT(FrontendShadingSettings_SetEnabled),
        /* 21 */ UI_SLOT(FrontendShadingSettings_ApplyLevel),
        /* 22 */ UI_SLOT(FrontendModelSettings_SetLodDepthThresholdQ8),
        /* 23 */ UI_SLOT(FrontendTextureSettings_SetQuality),
        /* 24 */ UI_SLOT(FrontendAudioSettings_SetEffectsEnabled),
        /* 25 */ UI_SLOT(FrontendAudioSettings_SetMusicEnabled),
        /* 26 */ UI_SLOT(FrontendAudioSettings_SetReverseStereo),
        /* 27 */ UI_SLOT(FrontendAudioSettings_SetEffectsGain),
        /* 28 */ UI_SLOT(FrontendAudioSettings_SetMovieDefaultGain),
        /* 29 */ UI_SLOT(FrontendAudioSettings_SetMusicGain),
        /* 30 */ nullptr, /* the original's colour depth choices, gone (32-bit colour only) */
        /* 31 */ nullptr, /* the original's colour depth choices, gone (32-bit colour only) */
        /* 32 */ nullptr, /* the original's colour depth choices, gone (32-bit colour only) */
        /* 33 */ nullptr, /* the original's colour depth choices, gone (32-bit colour only) */
        /* 34 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 35 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 36 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 37 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 38 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 39 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 40 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 41 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 42 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 43 */ UI_SLOT(FrontendDisplaySettingsAction_ApplyPendingResolution),
        /* 44 */ UI_SLOT(FrontendDisplaySettingsAction_SelectAdapter),
        /* 45 */ UI_SLOT(FrontendDisplaySettingsAction_SelectAdapter),
        /* 46 */ UI_SLOT(FrontendDisplaySettingsAction_SelectAdapter),
        /* 47 */ UI_SLOT(FrontendDisplaySettingsAction_SelectAdapter),
        /* 48 */ UI_SLOT(FrontendDisplaySettingsAction_SelectAdapter),
        /* 49 */ UI_SLOT(FrontendDisplaySettings_ApplyMode),
        /* 50 */ UI_SLOT(FrontendNetworkSettings_SetPlayerName),
        /* 51 */ UI_SLOT(UiActionSlot_FrontendQuitDialogAction_ReturnToMainPage),
        /* 52 */ UI_SLOT(UiActionSlot_FrontendCallback_ReturnToMainPageOrDispatchState4),
        /* 53 */ UI_SLOT(FrontendScenarioPage_OpenSaveRecordsAndRefresh),
        /* 54 */ UI_SLOT(FrontendScenarioPage_OpenLevelRecordsAndRefresh),
        /* 55 */ UI_SLOT(FrontendScenarioPage_OpenCampaignRecordsAndRefresh),
        /* 56 */ UI_SLOT(UiActionSlot_FrontendScenarioSelection_ActivateSelectedRecord),
        /* 57 */ UI_SLOT(FrontendScenarioSelection_SelectOrStartSavedGame),
        /* 58 */ UI_SLOT(FrontendScenarioSelection_SelectOrStartLevel),
        /* 59 */ UI_SLOT(FrontendScenarioSelection_SelectOrStartCampaign),
        /* 60 */ UI_SLOT(FrontendGameplaySettings_SetAutomaticZoomOff),
        /* 61 */ UI_SLOT(FrontendGameplaySettings_SetAutomaticRotationOff),
        /* 62 */ UI_SLOT(FrontendGameplaySettings_SetLinkRotationZoom),
        /* 63 */ UI_SLOT(FrontendGameplaySettings_SetLinkRotationTilt),
        /* 64 */ UI_SLOT(UiActionSlot_FrontendFactionSetupAction_ReturnToMainPage),
        /* 65 */ UI_SLOT(FrontendScenarioAction_StartFieldGridLoad),
        /* 66 */ UI_SLOT(FrontendPlayerConsensus_SubmitSelectedValue),
        /* 67 */ UI_SLOT(UiActionSlot_FrontendSessionAction_ApplyGameSpeedAndReturnToMainPage),
        /* 68 */ UI_SLOT(FrontendFactionSetupAction_CycleFactionColour),
        /* 69 */ UI_SLOT(FrontendFactionSetupAction_ToggleFactionActive),
        /* 70 */ UI_SLOT(FrontendFactionSetupAction_ChooseFaction),
        /* 71 */ UI_SLOT(FrontendSessionAction_ApplySpeedOrToggleReady),
        /* 72 */ UI_SLOT(FrontendSessionAction_CloseMovieAndReturnToMainPage),
        /* 73 */ UI_SLOT(FrontendGameplaySettings_SetRightButtonDoesNotScroll),
        /* 74 */ UI_SLOT(FrontendGameplaySettings_SetGameSpeedPercent),
        /* 75 */ UI_SLOT(FrontendGameplaySettings_SetCameraScrollStep),
        /* 76 */ UI_SLOT(FrontendPlayerMessage_SubmitSevenSlotText),
        /* 77 */ UI_SLOT(FrontendNetworkSettings_SetNetworkSpeed),
        /* 78 */ UI_SLOT(FrontendAudioSettings_SetMovieAlternateGain),
        /* 79 */ UI_SLOT(UiActionSlot_FrontendSessionAction_ReleaseCampaignAndReturnToMainPage),
        /* 80 */ UI_SLOT(FrontendCallback_NoOpArg1),
        /* 81 */ UI_SLOT(FrontendGameplaySettings_SetHidePanel),
        /* 82 */ UI_SLOT(FrontendScenarioPage_OpenSaveRecordsAndRefresh),
        /* 83 */ UI_SLOT(FrontendScenarioPage_OpenLevelRecordsAndRefresh),
        /* 84 */ UI_SLOT(FrontendScenarioPage_OpenCampaignRecordsAndRefresh)
    },
    .scenarioCatalogRebuildCallbacks = {
        /* 0 */ UI_SLOT(ScenarioCatalog_RebuildSaveRecordListPage),
        /* 1 */ UI_SLOT(ScenarioCatalog_RebuildLevelRecordListPage),
        /* 2 */ UI_SLOT(ScenarioCatalog_RebuildCampaignRecordListPage)
    },
    /* not in the original: actions 0x2058..0x205A, the display mode kind choices */
    .handlers58_5A = {
        /* 88 */ UI_SLOT(FrontendDisplaySettingsAction_SelectDisplayModeKind),
        /* 89 */ UI_SLOT(FrontendDisplaySettingsAction_SelectDisplayModeKind),
        /* 90 */ UI_SLOT(FrontendDisplaySettingsAction_SelectDisplayModeKind)
    }};

/* Handler of action 0x2050 (slot 80 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Save" button of
   the in-game variant of the mission briefing: does nothing.
*/
void FrontendCallback_NoOpArg1(void *source)

{
}

/* Handler of action 0x2034 (slot 52 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Choose game"
   page's "Cancel" button: returns to the main page with ROM action record 0 in a local game and with record 4
   (as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE) in a network game.
*/
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

{
  /* Original quirk: the record argument depends on the same role test that FrontendCommand_Issue repeats, so
     a local game calls the handler with record 0 and a network game queues record 4. */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
  }
  else {
    FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,4);
  }
}

/* Handler of action 0x2033 (slot 51 of g_FrontendUiActionHandlersPage20.handlers00_54), the quit dialog's "no"
   button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a
   network game).
*/
void FrontendQuitDialogAction_ReturnToMainPage(uint32_t callbackArgument)

{
  FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
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
    FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
    return;
  }
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,&frontendRootPage->primaryPageStack);
}
