/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/network.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/network.h>
#include <thandor/thandor.h>

/* Module data. */

uint16_t g_FrontendLocalPlayerNameUtf16[20] = {0};

FrontendSessionDiscoveryRecord **g_FrontendSessionListRows = 0;

FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeRecordPointers32[32] = {0};

uint32_t g_FrontendNetworkState = 0;

char g_SpielerSpielNetzwerkHostKeywordsAscii[31] = "SPIELER=\"SPIEL=\"NETZWERK=\"HOST";

/* Original quirk: the string's terminating NUL is the
   first byte of the Package_FindEntry output buffer g_LevelPackageFoundEntry; the code passes explicit lengths. */
char g_NameClientKarteKeywordsAscii[21] = {'N', 'A', 'M', 'E', '=', '"', 'C', 'L', 'I', 'E', 'N', 'T', '=', '"', 'K', 'A', 'R', 'T', 'E', '=', '"'}; /* "NAME=\"CLIENT=\"KARTE=\"" without its NUL */

UiTransferEndpointDescriptor g_FrontendNetworkEndpointScratch = {0};

uint16_t g_FrontendNetworkRuntimeCountTextUtf16[4] = {0};

uint16_t g_FrontendNetworkPlayerCountTextUtf16[4] = {0};

uint16_t g_FrontendNetworkEndpointTextUtf16[512] = {0};

/* Implementation ownership: ui/frontend/network. */

/* Scans the value of a quoted command-line option (valueText follows the opening quote) for its closing quote:
   at most maxChars characters, stopping at the terminator or a control character. Returns the closing quote, or
   NULL when there is none within the limit. */
static uint8_t *CommandLineOption_FindClosingQuote(uint8_t *valueText, int maxChars)
{
  uint8_t *cursor;
  int remainingChars;

  remainingChars = maxChars;
  for (cursor = valueText; *cursor >= ' '; cursor++) {
    if (*cursor == '"') {
      return cursor;
    }
    remainingChars--;
    if (remainingChars == 0) {
      return NULL;
    }
  }
  return NULL;
}

/* Opens network backend backendIndex on NETWORK_GAME_UDP_PORT; a backend that can be selected but not opened is
   cleaned up again. Returns 0 or the FATAL_ERROR_NETWORK_* code of the failing step. */
static uint32_t FrontendNetworkSetupPage_OpenBackend(UiListRowIndex backendIndex)
{
  uint32_t backendError;

  backendError = g_NetworkBackendSlot0(backendIndex);
  if (backendError == 0) {
    backendError = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    if (backendError != 0) {
      g_NetworkBackendSlot1(); /* cleanup takes no arguments */
    }
  }
  return backendError;
}

/* -NAME="player name": fills the player name edit and the local player name.
   Original quirk: when more text follows the closing quote, the quote is left overwritten with a terminator. */
static void FrontendNetworkSetupPage_ApplyNameOption(void)
{
  uint8_t *option;
  uint8_t *closingQuote;
  uint16_t *playerNameText;

  option = g_CommandLineFindOption(6,g_NameClientKarteKeywordsAscii);
  if (option == NULL) {
    return;
  }
  closingQuote = CommandLineOption_FindClosingQuote(option + 6,19);
  if (closingQuote == NULL) {
    return;
  }
  playerNameText = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,playerNameEdit))->textBuffer;
  *closingQuote = 0;
  if (closingQuote[1] == 0) {
    *option = 'n';
    Text_CopyNarrowToUtf16(40,playerNameText,option + 6);
    Text_CopyNarrowToUtf16(40,g_FrontendLocalPlayerNameUtf16,option + 6);
    *closingQuote = '"';
  }
}

/* -CLIENT="host address": when the backend can parse the address, joins that host directly (without picking a
   session from the list) and shows the address in the host address edit.
   Original quirk: unlike -NAME and -SPIEL, the closing quote is never restored (it stays a terminator). */
static void FrontendNetworkSetupPage_ApplyClientOption(void)
{
  uint8_t *option;
  uint8_t *closingQuote;
  Bool8 endpointParseFailed;

  option = g_CommandLineFindOption(8,g_NameClientKarteKeywordsAscii + 6);
  if (option == NULL) {
    return;
  }
  closingQuote = CommandLineOption_FindClosingQuote(option + 8,FRONTEND_CLIENT_OPTION_SCAN_LIMIT);
  if (closingQuote == NULL) {
    return;
  }
  *closingQuote = 0;
  if (closingQuote[1] != 0) {
    return;
  }
  *option = 'c';
  Text_CopyNarrowToUtf16
            (PACKAGE_SCRATCH_BUFFER_BYTES,(uint16_t *)g_PackageScratchBuffer,option + 8);
  endpointParseFailed = g_NetworkBackendSlot6
                    (&g_FrontendSelectedNetworkEndpoint,(char *)g_PackageScratchBuffer);
  if (endpointParseFailed) {
    return;
  }
  g_NetworkBackendSlot6(&g_FrontendNetworkEndpointScratch,(char *)g_PackageScratchBuffer);
  g_FrontendSessionToken = FRONTEND_SEQUENCE_TOKEN_HIGH_WORD;
  g_FrontendSelectedPlayerToken = 0xffffffff;
  UiTransfer_SendPlayerDescriptor();
  g_NetworkBackendSlot7
            ((char *)g_FrontendNetworkEndpointTextUtf16,
             (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
  RichTextCommandStream_CopyExpanded
            (128,((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,hostAddressEdit))->textBuffer,
             g_FrontendNetworkEndpointTextUtf16,NULL);
}

/* Opens the network part of the menu (FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) according to the session role:
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
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  UiListRowIndex backendIndex;
  uint32_t backendError; /* 0 or a FATAL_ERROR_NETWORK_* code */
  int dwordsRemaining;
  uint32_t *localEndpointCursor;
  uint32_t *localPlayerNameDwordCursor;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  uint32_t *localPlayerRecordDwordCursor;
  uint8_t *hostOption;
  uint32_t sequenceToken;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    /* host: back to the lobby */
    g_NetworkBackendSlot7
              ((char *)g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,FRONTEND_UI(frontendUi,frontendRoot));
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
      ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    }
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
    UiPointerList_InitializeColumnLayout
              (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
               (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,g_FrontendNetworkRuntimeCountTextUtf16
              );
    /* the local player becomes the only player record: name, local endpoint, command sync pending */
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    sequenceToken = g_UiTransferSequenceToken;
    g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
    firstPlayerRecord->peerSequenceToken = sequenceToken;
    localPlayerNameDwordCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
    localPlayerRecordDwordCursor = (uint32_t *)&firstPlayerRecord->playerName;
    for (dwordsRemaining = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); dwordsRemaining != 0;
         dwordsRemaining--) {
      *localPlayerRecordDwordCursor = *localPlayerNameDwordCursor;
      localPlayerNameDwordCursor++;
      localPlayerRecordDwordCursor++;
    }
    /* the cursor continues into firstPlayerRecord->endpoint and then commandSyncPending */
    endpointSourceDwordCursor = (uint32_t *)&g_NetworkLocalEndpoint;
    for (dwordsRemaining = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); dwordsRemaining != 0;
         dwordsRemaining--) {
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
              ((char *)g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    RichTextCommandStream_CopyExpanded
              (128,((UiRequiredTextEditControl *)FRONTEND_UI(frontendUi,hostAddressEdit))->textBuffer,
               g_FrontendNetworkEndpointTextUtf16,NULL);
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
      ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    }
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
    UiPointerList_InitializeColumnLayout
              (0,(void **)g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
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
  if (FrontendNetworkSetupPage_OpenBackend(backendIndex) != 0) {
    /* the selected backend fails: try every backend from the first one (the selected one again included) */
    backendIndex = 0;
    do {
      backendError = FrontendNetworkSetupPage_OpenBackend(backendIndex);
      if (backendError == 0) {
        break;
      }
      backendIndex++;
    } while (g_NetworkBackendInstanceCount > backendIndex);
    if (backendError != 0) {
      /* no backend opens: report the last error and leave the network page */
      FatalError_ReportIfFailed(backendError,true);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
      }
      return;
    }
  }
  /* backend backendIndex is open */
  UiPointerList_SelectTextListIndex
            (backendIndex,(UiPointerListControl *)FRONTEND_UI(frontendUi,networkProtocolList));
  localEndpointCursor = (uint32_t *)&g_NetworkLocalEndpoint;
  endpointDestinationDwordCursor = (uint32_t *)&g_FrontendNetworkEndpointScratch;
  /* copies the 16-byte local endpoint dword by dword */
  for (dwordsRemaining = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); dwordsRemaining != 0;
       dwordsRemaining--) {
    *endpointDestinationDwordCursor = *localEndpointCursor;
    localEndpointCursor++;
    endpointDestinationDwordCursor++;
  }
  FrontendNetworkSetupPage_ApplyNameOption();
  g_NetworkBackendSlot7
            ((char *)g_FrontendNetworkEndpointTextUtf16,
             (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
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
            (0,(void **)g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
  UiTransfer_SendDiscoveryProbe();
  /* -HOST opens the host setup at once; otherwise -CLIENT="host address" may join a host */
  hostOption = g_CommandLineFindOption(5,g_SpielerSpielNetzwerkHostKeywordsAscii + 26);
  if (hostOption != NULL) {
    *hostOption = 'h';
    FrontendNetworkSetupPage_InitializeFromCommandLine
              (FRONTEND_UI(frontendUi,networkGameHostButton));
  }
  else {
    FrontendNetworkSetupPage_ApplyClientOption();
  }
  return;
}


/* Frontend teardown: carries the status text id and the host address the player typed over into the frontend
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
  FRONTEND_UI_FIELD(&g_FrontendRootInitializationTemplate,bottomBarStatusText,0x54,TextResourceId) =
       (TextResourceId)((UiSingleLineTextControl *)FRONTEND_UI(root,bottomBarStatusText))->text;
  sourceCursor = (int32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(root,hostAddressEdit))->textBuffer;
  destinationCursor =
       (int32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(&g_FrontendRootInitializationTemplate,hostAddressEdit))
       ->textBuffer;
  INGAME_UI_FIELD(&g_InGameRuntimeDefaultImageTemplate,worldViewCyclingInfoText,0x54,TextResourceId) =
       FRONTEND_UI_FIELD(&g_FrontendRootInitializationTemplate,bottomBarStatusText,0x54,TextResourceId);
  for (dwordsRemaining = 32; dwordsRemaining != 0; dwordsRemaining--) { /* 0x40 code units */
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
  return;
}


/* Action 0x200D of the host address edit on the network game page (g_FrontendUiActionHandlersPage20 slot 13):
   the typed address is parsed by the active network backend into g_FrontendNetworkEndpointScratch. An
   unparsable address only clears the edit's valid flag; a valid one sends the session discovery probe
   and writes the parsed endpoint back as normalised text.
*/
void FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *hostAddressEdit)

{
  Bool8 endpointParseFailed;

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
            ((char *)g_FrontendNetworkEndpointTextUtf16,(WinSockAddress *)&g_FrontendNetworkEndpointScratch
            );
  return;
}


/* Action 0x2003 of the back button on the host game setup page (g_FrontendUiActionHandlersPage20 slot 3):
   returns to the network game page, stops the menu room rendering behind it on small screens, resumes
   browsing with the join button hidden and an empty session list, and sends a new session discovery probe.
*/
void FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source)

{
  /* source is the frontend template's hostGameSetupBackButton. */
  FrontendUiImage *frontendUi;

  frontendUi = (FrontendUiImage *)((uint8_t *)source - offsetof(FrontendUiImage,hostGameSetupBackButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
  UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
  UiPointerList_InitializeColumnLayout
            (0,(void **)g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
  UiTransfer_SendDiscoveryProbe();
  return;
}


/* One-digit command-line option value: option[digitOffset] is the digit, followed by the closing quote and the end
   of the option text. Returns the digit value (at least 1; characters above '9' give larger values), or 0 when
   the text does not have that form or the digit is '0'. */
static uint32_t CommandLineOption_ParseQuotedDigit(const uint8_t *option, int digitOffset)
{
  if (option[digitOffset + 1] != '"') {
    return 0;
  }
  if (option[digitOffset] <= '0') {
    return 0;
  }
  if (option[digitOffset + 2] != 0) {
    return 0;
  }
  return (uint32_t)(option[digitOffset] - '0');
}

/* Opens the host game setup page (action FRONTEND_ACTION_HOST_GAME). The command-line options
   -SPIELER="n" (maximum players, 2..8), -SPIEL="name" (game name) and -NETZWERK="n" (network speed 1..7,
   tick interval 2n) preset the page, each consumed by lower-casing its first letter; when all three are
   given, the game is created at once (FrontendNetworkSetupPage_InitializeSingleLocalPlayer).
*/
void FrontendNetworkSetupPage_InitializeFromCommandLine(UiNodeBase *hostButton)

{
  /* hostButton is the frontend template's networkGameHostButton. */
  FrontendUiImage *frontendUi;
  uint32_t maxPlayers;
  uint32_t optionNetworkSpeed;
  uint32_t networkSpeed;
  uint16_t *gameNameText;
  int appliedOptionMask; /* 1 = -SPIELER, 2 = -SPIEL, 4 = -NETZWERK */
  uint8_t *option;
  uint8_t *closingQuote;
  uint16_t *resolvedText;

  appliedOptionMask = 0;
  /* -SPIELER="n", n = 2..8 */
  option = g_CommandLineFindOption(9,g_SpielerSpielNetzwerkHostKeywordsAscii);
  if (option != NULL) {
    maxPlayers = CommandLineOption_ParseQuotedDigit(option,9);
    if (maxPlayers < 9 && 1 < maxPlayers) {
      *option = 's';
      appliedOptionMask = 1;
      ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,maxPlayersSlider))->value = maxPlayers;
    }
  }
  /* -SPIEL="game name".
     Original quirk: when more text follows the closing quote, the quote is left overwritten with a terminator. */
  option = g_CommandLineFindOption(7,g_SpielerSpielNetzwerkHostKeywordsAscii + 9);
  if (option != NULL) {
    closingQuote = CommandLineOption_FindClosingQuote(option + 7,19);
    if (closingQuote != NULL) {
      gameNameText = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,gameNameEdit))->textBuffer;
      *closingQuote = 0;
      if (closingQuote[1] == 0) {
        *option = 's';
        Text_CopyNarrowToUtf16(40,gameNameText,option + 7);
        *closingQuote = '"';
        appliedOptionMask = appliedOptionMask + 2;
      }
    }
  }
  /* -NETZWERK="n", n = 1..7 */
  option = g_CommandLineFindOption(10,g_SpielerSpielNetzwerkHostKeywordsAscii + 16);
  if (option != NULL) {
    optionNetworkSpeed = CommandLineOption_ParseQuotedDigit(option,10);
    if (optionNetworkSpeed < 8 && optionNetworkSpeed != 0) {
      *option = 'n';
      ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,networkSpeedSlider))->value =
           optionNetworkSpeed;
      appliedOptionMask = appliedOptionMask + 4;
      g_SessionNetworkTickInterval = optionNetworkSpeed * 2;
    }
  }
  frontendUi = (FrontendUiImage *)((uint8_t *)hostButton - offsetof(FrontendUiImage,networkGameHostButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_GAME_SETUP,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
             ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,maxPlayersSlider))->value,
             g_FrontendNetworkPlayerCountTextUtf16);
  networkSpeed = g_SessionNetworkTickInterval >> 1;
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,networkSpeedSlider))->value = networkSpeed;
  /* the speed caption */
  resolvedText = TextResource_Resolve(networkSpeed + TEXT_ID_NETWORK_SPEED_BASE);
  RichTextCommandStream_CopyExpanded
            (64,g_FrontendNetworkSpeedLabelUtf16,resolvedText,NULL);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  FrontendNetworkSettings_SetGameName((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  if (appliedOptionMask == 7) {
    FrontendNetworkSetupPage_InitializeSingleLocalPlayer(FRONTEND_UI(frontendUi,hostGameCreateButton));
  }
  return;
}


/* Action handler of the host game setup page's create button: opens the host lobby page and makes the local
   player the only player of a new hosted session (player block 0 with the local name and endpoint, id 0, no
   timeout, "CD" capability, its 64x64 preview image as snapshot payload when it loads), then refreshes the lobby.
*/
void FrontendNetworkSetupPage_InitializeSingleLocalPlayer(UiNodeBase *createButton)

{
  /* createButton is the frontend template's hostGameCreateButton. */
  FrontendUiImage *frontendUi;
  uint32_t sequenceToken;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  int remainingDwords;
  uint32_t *localPlayerNameCursor;
  uint32_t *localEndpointDwordCursor;
  uint32_t *localPlayerRecordDwordCursor;
  Bool8 previewLoadFailed;
  
  frontendUi = (FrontendUiImage *)((uint8_t *)createButton - offsetof(FrontendUiImage,hostGameCreateButton));
  UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,FRONTEND_UI(frontendUi,frontendRoot));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
  UiPointerList_InitializeColumnLayout
            (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
             (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,g_FrontendNetworkRuntimeCountTextUtf16);
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  sequenceToken = g_UiTransferSequenceToken;
  g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
  firstPlayerRecord->peerSequenceToken = sequenceToken;
  firstPlayerRecord->playerRuntimeId = 0;
  localPlayerNameCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
  localPlayerRecordDwordCursor = (uint32_t *)&firstPlayerRecord->playerName;
  for (remainingDwords = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *localPlayerRecordDwordCursor = *localPlayerNameCursor;
    localPlayerNameCursor++;
    localPlayerRecordDwordCursor++;
  }
  localEndpointDwordCursor = (uint32_t *)&g_NetworkLocalEndpoint;
  for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *localPlayerRecordDwordCursor = *localEndpointDwordCursor;
    localEndpointDwordCursor++;
    localPlayerRecordDwordCursor++;
  }
  /* the cursor now points at commandSyncPending of the player record; the indices below are dwords from there */
  *localPlayerRecordDwordCursor = FRONTEND_COMMAND_SYNC_PENDING; /* commandSyncPending */
  g_FrontendPendingSessionPlayerCount = 0;
  g_FrontendPlayerRuntimeCount = 1;
  g_LocalPlayerRuntimeId = 0;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_HOST;
  localPlayerRecordDwordCursor[6] = 0; /* snapshotTransferFlags */
  /* the preview goes into snapshotPayload, its name is the player name (playerName) */
  previewLoadFailed = PcxPreview_Load64x64PaletteAndPixels
                    ((PcxPreview64 *)(localPlayerRecordDwordCursor + 24),
                     (uint16_t *)(localPlayerRecordDwordCursor + -14));
  if (!previewLoadFailed) {
    localPlayerRecordDwordCursor[6] = FRONTEND_SNAPSHOT_SOURCE_AVAILABLE | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
  }
  /* pingRoundTripTicks 0, pingTextUtf16 L"0ms" */
  localPlayerRecordDwordCursor[16] = 0;
  localPlayerRecordDwordCursor[17] = L'm' << 16 | L'0';
  localPlayerRecordDwordCursor[18] = L's';
  localPlayerRecordDwordCursor[9] = FRONTEND_CAPABILITY_CD; /* capabilityFlags */
  localPlayerRecordDwordCursor[10] = 0;
  localPlayerRecordDwordCursor[11] = 0;
  localPlayerRecordDwordCursor[10] = L'D' << 16 | L'C'; /* L"CD" in capabilityLabelUtf16 */
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
  return;
}

