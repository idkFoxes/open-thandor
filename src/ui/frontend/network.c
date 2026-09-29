/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/network.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/network.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/frontend/network. */

/* Address: 0x0054C260.
   Opens the network part of the menu (FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) according to the session role:
   - host session already running (returning from a game): back to the host lobby with the local player as the
     only player record;
   - client session: back to the network game page, the player record reset, and the join request re-sent;
   - no session yet: open the selected network backend (or the first one that works) on NETWORK_GAME_UDP_PORT
     and show the network game page. The command-line options -NAME="..." (player name), -HOST (open the host
     setup at once) and -CLIENT="address" (join that host at once) are applied here; each is consumed by
     lower-casing its first letter. If no backend can be opened, the error is reported and the menu returns
     to the main page.
*/
void FrontendNetworkSetupPage_InitializeBackendMode(FrontendUiImage *frontendUi)

{
  uint8_t optionChar;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  UiListRowIndex backendIndex;
  uint32_t errorOrValue;
  int remainingOrRootNode; /* loop counter; once also holds g_FrontendRootNode */
  uint16_t *destination;
  uint8_t *commandLineOptionBytes;
  uint8_t *secondaryCommandLineOptionBytes;
  uint32_t *localEndpointCursor;
  uint32_t *localPlayerNameDwordCursor;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  uint8_t *optionTextCursor;
  uint32_t *localPlayerRecordDwordCursor;
  bool endpointParseFailed;
  NetworkSetSessionResult setSessionResult;
  NetworkOpenBindResult openBindResult;
  CommandLineOptionResult findOptionResult;
  uint32_t sequenceToken;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    /* host: back to the lobby */
    g_NetworkBackendSlot7
              (&g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,FRONTEND_UI(frontendUi,frontendRoot));
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
      ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    }
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
    UiPointerList_InitializeColumnLayout
              (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
               (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(uint16_t *)&g_FrontendNetworkRuntimeCountTextUtf16
              );
    /* the local player becomes the only player record: name, local endpoint, command sync pending */
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    sequenceToken = g_UiTransferSequenceToken;
    g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
    firstPlayerRecord->peerSequenceToken = sequenceToken;
    localPlayerNameDwordCursor = (void *)g_FrontendLocalPlayerNameUtf16;
    localPlayerRecordDwordCursor = (uint32_t *)&firstPlayerRecord->playerName;
    for (remainingOrRootNode = sizeof(FrontendPlayerNameUtf16_28) / sizeof(uint32_t); remainingOrRootNode != 0;
         remainingOrRootNode--) {
      *localPlayerRecordDwordCursor = *localPlayerNameDwordCursor;
      localPlayerNameDwordCursor++;
      localPlayerRecordDwordCursor++;
    }
    /* the cursor continues into firstPlayerRecord->endpoint and then commandSyncPending */
    endpointSourceDwordCursor = (uint32_t *)&g_NetworkLocalEndpointDescriptor16;
    for (remainingOrRootNode = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingOrRootNode != 0;
         remainingOrRootNode--) {
      *localPlayerRecordDwordCursor = *endpointSourceDwordCursor;
      endpointSourceDwordCursor++;
      localPlayerRecordDwordCursor++;
    }
    *localPlayerRecordDwordCursor = FRONTEND_COMMAND_SYNC_PENDING;
    g_FrontendPendingSessionPlayerCount = 0;
    g_FrontendPlayerRuntimeCount = 1;
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_HOST;
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    /* client: back to the network game page with the host address filled in, and ask to join again */
    g_NetworkBackendSlot7
              (&g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    RichTextCommandStream_CopyExpanded
              (0x80,((UiRequiredTextEditControl *)FRONTEND_UI(frontendUi,hostAddressEdit))->textBuffer,
               (uint16_t *)&g_FrontendNetworkEndpointTextUtf16);
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
      ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    }
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiPointerList_InitializeColumnLayout
              (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_CLIENT;
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    firstPlayerRecord->playerName.textUtf16[0] = 0;
    firstPlayerRecord->playerName.textUtf16[1] = 0;
    firstPlayerRecord->playerRuntimeId = 0;
    firstPlayerRecord->factionAssignment.roleStateFlags = 0;
    firstPlayerRecord->colourCycleFlags = 0;
    firstPlayerRecord->snapshotTransferFlags = 0;
    UiTransfer_SendPlayerDescriptor();
    return;
  }
  /* no session yet: open the backend selected in the protocol list, else the first one that opens */
  UiRuntimeRecordRing_Clear();
  UiTransferMailbox_RandomizeSequenceToken();
  backendIndex = UiPointerList_GetSelectedIndex
                          ((UiPointerListControl *)FRONTEND_UI(frontendUi,networkProtocolList));
  setSessionResult = g_NetworkBackendSlot0(backendIndex);
  if (!setSessionResult.failed) {
    openBindResult = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    if (!openBindResult.failed) {
backendOpened:
      UiPointerList_SelectTextListIndex
                (backendIndex,(UiPointerListControl *)FRONTEND_UI(frontendUi,networkProtocolList));
      localEndpointCursor = (uint32_t *)&g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (uint32_t *)&g_FrontendNetworkEndpointScratch;
      /* copies the 16-byte local endpoint dword by dword */
      for (remainingOrRootNode = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingOrRootNode != 0;
           remainingOrRootNode--) {
        *endpointDestinationDwordCursor = *localEndpointCursor;
        localEndpointCursor++;
        endpointDestinationDwordCursor++;
      }
      /* -NAME="player name" */
      findOptionResult = g_CommandLineFindOption(6,s_NAME__CLIENT__KARTE___00545e91);
      commandLineOptionBytes = findOptionResult.option;
      if (!findOptionResult.notFound) {
        optionTextCursor = commandLineOptionBytes + 6;
        remainingOrRootNode = 19;
        /* find the closing quote within 19 characters (stop at control characters) */
        for (;;) {
          optionChar = *optionTextCursor;
          if (optionChar == 0 || optionChar < ' ') break;
          if (optionChar == '"') break;
          remainingOrRootNode--;
          if (remainingOrRootNode == 0) break;
          optionTextCursor++;
        }
        if (optionChar == '"') {
          destination = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,playerNameEdit))->textBuffer;
          *optionTextCursor = 0;
          if (optionTextCursor[1] == 0) {
            *commandLineOptionBytes = 'n';
            Text_CopyNarrowToUtf16(0x28,destination,commandLineOptionBytes + 6);
            Text_CopyNarrowToUtf16
                      (0x28,g_FrontendLocalPlayerNameUtf16,commandLineOptionBytes + 6);
            *optionTextCursor = '"';
          }
        }
      }
      g_NetworkBackendSlot7
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
      if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
        ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      }
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
      /* hosting needs a player name */
      if (g_FrontendLocalPlayerNameUtf16[0] == 0) {
        UiNodeList_SuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
      }
      else {
        UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
      }
      UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
      UiTransfer_SendDiscoveryProbe();
      /* -HOST */
      findOptionResult = g_CommandLineFindOption(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
      if (findOptionResult.notFound) {
        /* -CLIENT="host address" */
        findOptionResult = g_CommandLineFindOption(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
        secondaryCommandLineOptionBytes = findOptionResult.option;
        if (!findOptionResult.notFound) {
          remainingOrRootNode = 0x7fffff;
          optionTextCursor = secondaryCommandLineOptionBytes + 8;
          for (;;) {
            optionChar = *optionTextCursor;
            if (optionChar == 0) {
              return;
            }
            if (optionChar < ' ') {
              return;
            }
            if (optionChar == '"') break;
            remainingOrRootNode--;
            if (remainingOrRootNode == 0) {
              return;
            }
            optionTextCursor++;
          }
          *optionTextCursor = 0;
          if (optionTextCursor[1] == 0) {
            *secondaryCommandLineOptionBytes = 'c';
            Text_CopyNarrowToUtf16
                      (PACKAGE_SCRATCH_BUFFER_BYTES,(uint16_t *)g_PackageScratchBuffer,
                       secondaryCommandLineOptionBytes + 8);
            endpointParseFailed = g_NetworkBackendSlot6
                              (&g_FrontendSelectedNetworkEndpoint,(char *)g_PackageScratchBuffer);
            if (!endpointParseFailed) {
              /* join that host directly, without picking a session from the list */
              g_NetworkBackendSlot6
                        (&g_FrontendNetworkEndpointScratch,(char *)g_PackageScratchBuffer);
              g_FrontendSessionToken = FRONTEND_SEQUENCE_TOKEN_HIGH_WORD;
              g_FrontendSelectedPlayerToken = 0xffffffff;
              UiTransfer_SendPlayerDescriptor();
              remainingOrRootNode = g_FrontendRootNode;
              g_NetworkBackendSlot7
                        (&g_FrontendNetworkEndpointTextUtf16,
                         (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
              RichTextCommandStream_CopyExpanded
                        (0x80,
                         ((UiRequiredTextEditControl *)FRONTEND_UI(remainingOrRootNode,hostAddressEdit))
                         ->textBuffer,(uint16_t *)&g_FrontendNetworkEndpointTextUtf16);
            }
          }
        }
      }
      else {
        *findOptionResult.option = 'h';
        FrontendNetworkSetupPage_InitializeFromCommandLine
                  (FRONTEND_UI(frontendUi,networkGameHostButton));
      }
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup takes no arguments; Ghidra passed a stale register */
  }
  backendIndex = 0;
  do {
    setSessionResult = g_NetworkBackendSlot0(backendIndex);
    errorOrValue = setSessionResult.valueOrError;
    if (!setSessionResult.failed) {
      openBindResult = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
      errorOrValue = openBindResult.valueOrError;
      if (!openBindResult.failed) goto backendOpened;
      g_NetworkBackendSlot1(); /* cleanup takes no arguments; Ghidra passed a stale register */
    }
    backendIndex++;
    if (g_NetworkBackendInstanceCount <= backendIndex) {
      FatalError_ReportIfFailed(errorOrValue,true);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
      }
      return;
    }
  } while (true);
}


/* Address: 0x005474A0.
   Frontend teardown: carries the status text id and the host address the player typed over into the frontend
   template, so the next frontend built from it (and the in-game template's info text) shows them again.
*/
void FrontendTeardown_SaveStatusTextAndHostAddress(UiRootNode *root)

{
  int dwordsRemaining;
  int32_t *sourceCursor;
  int32_t *destinationCursor;
  
  /* root is the frontend template copy; both values are written back into the frontend template
     (bottomBarStatusText's text resource id, mirrored into the in-game template's worldViewCyclingInfoText,
     and the 0x40-code-unit hostAddressEdit text). */
  g_FrontendTemplateStatusTextResourceId =
       (TextResourceId)((UiSingleLineTextControl *)FRONTEND_UI(root,bottomBarStatusText))->text;
  sourceCursor = (int32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(root,hostAddressEdit))->textBuffer;
  destinationCursor = (int32_t *)g_FrontendHostAddressTextTemplate;
  g_InGameTemplateWorldViewInfoTextResourceId = g_FrontendTemplateStatusTextResourceId;
  for (dwordsRemaining = 0x20; dwordsRemaining != 0; dwordsRemaining--) { /* 0x40 code units */
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
  return;
}


/* Address: 0x0054C7D0.
   Action 0x200D of the host address edit on the network game page (g_FrontendUiActionHandlersPage20 slot 13):
   the typed address is parsed by the active network backend into g_FrontendNetworkEndpointScratch. An
   unparsable address only clears the edit's valid flag; a valid one sends the session discovery probe
   and writes the parsed endpoint back as normalised text.
*/
void FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *hostAddressEdit)

{
  bool endpointParseFailed;

  endpointParseFailed = g_NetworkBackendSlot6
                    (&g_FrontendNetworkEndpointScratch,(char *)hostAddressEdit->textBuffer);
  if (endpointParseFailed) {
    hostAddressEdit->editStateFlags =
         hostAddressEdit->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
    return;
  }
  hostAddressEdit->editStateFlags =
       hostAddressEdit->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  UiTransfer_SendDiscoveryProbe();
  g_NetworkBackendSlot7
            (&g_FrontendNetworkEndpointTextUtf16,(WinSockAddress *)&g_FrontendNetworkEndpointScratch
            );
  return;
}


/* Address: 0x0054CE10.
   Action 0x2003 of the back button on the host game setup page (g_FrontendUiActionHandlersPage20 slot 3):
   returns to the network game page, stops the menu room rendering behind it on small screens, resumes
   browsing with the join button hidden and an empty session list, and sends a new session discovery probe.
*/
void FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source)

{
  /* source is the frontend template's hostGameSetupBackButton (+0x4F94). */
  FrontendUiImage *frontendUi;

  frontendUi = (FrontendUiImage *)((uint8_t *)source - offsetof(FrontendUiImage,hostGameSetupBackButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
  UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
  UiPointerList_InitializeColumnLayout
            (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
  UiTransfer_SendDiscoveryProbe();
  return;
}


/* Address: 0x0054C830.
   Opens the host game setup page (action FRONTEND_ACTION_HOST_GAME). The command-line options
   -SPIELER="n" (maximum players, 2..8), -SPIEL="name" (game name) and -NETZWERK="n" (network speed 1..7,
   tick interval 2n) preset the page, each consumed by lower-casing its first letter; when all three are
   given, the game is created at once (FrontendNetworkSetupPage_InitializeSingleLocalPlayer).
*/
void FrontendNetworkSetupPage_InitializeFromCommandLine(UiNodeBase *hostButton)

{
  /* hostButton is the frontend template's networkGameHostButton (+0x4920). */
  FrontendUiImage *frontendUi;
  uint8_t optionChar;
  uint32_t digitValueOrSpeed; /* digit of -SPIELER / -NETZWERK, at the end the network speed */
  int remainingChars;
  uint16_t *destination;
  int appliedOptionMask; /* 1 = -SPIELER, 2 = -SPIEL, 4 = -NETZWERK */
  uint8_t *optionText;
  uint8_t *optionTextCursor;
  TextResolveResult resolvedText;
  CommandLineOptionResult findOptionResult;

  appliedOptionMask = 0;
  /* -SPIELER="n" */
  findOptionResult = g_CommandLineFindOption(9,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72);
  optionText = findOptionResult.option;
  if (!findOptionResult.notFound && optionText[10] == '"' &&
      (digitValueOrSpeed = optionText[9] - '0', '/' < optionText[9] && digitValueOrSpeed != 0) &&
      optionText[11] == 0 && digitValueOrSpeed < 9 && 1 < digitValueOrSpeed) {
    *optionText = 's';
    appliedOptionMask = 1;
    ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,maxPlayersSlider))->value =
         digitValueOrSpeed;
  }
  /* -SPIEL="game name" */
  findOptionResult = g_CommandLineFindOption(7,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 9);
  optionText = findOptionResult.option;
  if (!findOptionResult.notFound) {
    optionTextCursor = optionText + 7;
    remainingChars = 19;
    /* find the closing quote within 19 characters (stop at control characters) */
    for (;;) {
      optionChar = *optionTextCursor;
      if (optionChar == 0 || optionChar < ' ') break;
      if (optionChar == '"') break;
      remainingChars--;
      if (remainingChars == 0) break;
      optionTextCursor++;
    }
    if (optionChar == '"') {
      destination = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,gameNameEdit))->textBuffer;
      *optionTextCursor = 0;
      if (optionTextCursor[1] == 0) {
        *optionText = 's';
        Text_CopyNarrowToUtf16(0x28,destination,optionText + 7);
        *optionTextCursor = '"';
        appliedOptionMask = appliedOptionMask + 2;
      }
    }
  }
  /* -NETZWERK="n" */
  findOptionResult = g_CommandLineFindOption(10,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x10);
  optionText = findOptionResult.option;
  if (!findOptionResult.notFound && optionText[11] == '"' &&
      (digitValueOrSpeed = optionText[10] - '0', '/' < optionText[10] && digitValueOrSpeed != 0) &&
      optionText[12] == 0 && digitValueOrSpeed < 8 && digitValueOrSpeed != 0) {
    *optionText = 'n';
    ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,networkSpeedSlider))->value =
         digitValueOrSpeed;
    appliedOptionMask = appliedOptionMask + 4;
    g_SessionNetworkTickInterval = digitValueOrSpeed * 2;
  }
  frontendUi = (FrontendUiImage *)((uint8_t *)hostButton - offsetof(FrontendUiImage,networkGameHostButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_GAME_SETUP,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
             ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,maxPlayersSlider))->value,
             (uint16_t *)&g_FrontendNetworkPlayerCountTextUtf16);
  digitValueOrSpeed = g_SessionNetworkTickInterval >> 1;
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,networkSpeedSlider))->value = digitValueOrSpeed;
  /* the speed caption; despite its name, g_FrontendNetworkPlayerCountLabelUtf16 holds the network speed */
  resolvedText = TextResource_Resolve(digitValueOrSpeed + TEXT_ID_NETWORK_SPEED_BASE);
  RichTextCommandStream_CopyExpanded
            (0x40,(uint16_t *)&g_FrontendNetworkPlayerCountLabelUtf16,resolvedText.text);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  FrontendNetworkSettings_SetGameName((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  if (appliedOptionMask == 7) {
    FrontendNetworkSetupPage_InitializeSingleLocalPlayer(FRONTEND_UI(frontendUi,hostGameCreateButton));
  }
  return;
}


/* Address: 0x0054CE80.
   Action handler of the host game setup page's create button: opens the host lobby page and makes the local
   player the only player of a new hosted session (player block 0 with the local name and endpoint, id 0, no
   timeout, "CD" capability, its 64x64 preview image as snapshot payload when it loads), then refreshes the lobby.
*/
void FrontendNetworkSetupPage_InitializeSingleLocalPlayer(UiNodeBase *createButton)

{
  /* createButton is the frontend template's hostGameCreateButton (+0x4FF4). */
  FrontendUiImage *frontendUi;
  uint32_t sequenceToken;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  int remainingDwords;
  uint32_t *localPlayerNameCursor;
  uint32_t *localEndpointDwordCursor;
  uint32_t *localPlayerRecordDwordCursor;
  bool previewLoadFailed;
  
  frontendUi = (FrontendUiImage *)((uint8_t *)createButton - offsetof(FrontendUiImage,hostGameCreateButton));
  UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,FRONTEND_UI(frontendUi,frontendRoot));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
  UiPointerList_InitializeColumnLayout
            (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
             (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(uint16_t *)&g_FrontendNetworkRuntimeCountTextUtf16);
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  sequenceToken = g_UiTransferSequenceToken;
  g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
  firstPlayerRecord->peerSequenceToken = sequenceToken;
  firstPlayerRecord->playerRuntimeId = 0;
  localPlayerNameCursor = (void *)g_FrontendLocalPlayerNameUtf16;
  localPlayerRecordDwordCursor = (uint32_t *)&firstPlayerRecord->playerName;
  for (remainingDwords = sizeof(FrontendPlayerNameUtf16_28) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *localPlayerRecordDwordCursor = *localPlayerNameCursor;
    localPlayerNameCursor++;
    localPlayerRecordDwordCursor++;
  }
  localEndpointDwordCursor = (uint32_t *)&g_NetworkLocalEndpointDescriptor16;
  for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *localPlayerRecordDwordCursor = *localEndpointDwordCursor;
    localEndpointDwordCursor++;
    localPlayerRecordDwordCursor++;
  }
  /* the cursor now points at +0x50 of the player record; the indices below are dwords from there */
  *localPlayerRecordDwordCursor = FRONTEND_COMMAND_SYNC_PENDING; /* commandSyncPending */
  g_FrontendPendingSessionPlayerCount = 0;
  g_FrontendPlayerRuntimeCount = 1;
  g_LocalPlayerRuntimeId = 0;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_HOST;
  localPlayerRecordDwordCursor[6] = 0; /* snapshotTransferFlags */
  /* the preview goes into snapshotPayload (+0xB0), its name is the player name (+0x18) */
  previewLoadFailed = PcxPreview_Load64x64PaletteAndPixels
                    ((PcxPreview64 *)(localPlayerRecordDwordCursor + 0x18),
                     (uint16_t *)(localPlayerRecordDwordCursor + -0xe));
  if (!previewLoadFailed) {
    localPlayerRecordDwordCursor[6] = FRONTEND_SNAPSHOT_SOURCE_AVAILABLE | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
  }
  /* +0x90..+0x9B: two empty code units, then L"0ms" */
  localPlayerRecordDwordCursor[0x10] = 0;
  localPlayerRecordDwordCursor[0x11] = 0x6d0030;
  localPlayerRecordDwordCursor[0x12] = 0x73;
  localPlayerRecordDwordCursor[9] = FRONTEND_CAPABILITY_CD; /* capabilityFlags */
  localPlayerRecordDwordCursor[10] = 0;
  localPlayerRecordDwordCursor[0xb] = 0;
  localPlayerRecordDwordCursor[10] = 0x440043; /* L"CD" at +0x78 */
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
  return;
}

