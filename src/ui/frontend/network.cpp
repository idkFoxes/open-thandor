/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/network.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/network.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>

/* Views the network backend callbacks and the copy loops need (genuine reinterpretations, in one place). */
/* The 16-byte UiTransferEndpointDescriptor is the image of a sockaddr_in (family and port, IPv4 address, 8 zero
   bytes); the backend callbacks take it as a WinSockAddress. */
static inline WinSockAddress *FrontendNetwork_SocketAddress(UiTransferEndpointDescriptor *endpoint)
{
  return reinterpret_cast<WinSockAddress *>(endpoint);
}
/* The backend callbacks take their text as char *, also where the buffer holds UTF-16 code units (the original
   passes the same bytes; the backend formats/parses wide text). */
template <class T> static inline char *FrontendNetwork_TextBytes(T *text)
{
  return reinterpret_cast<char *>(text);
}
/* A record or text buffer as dwords: the original copies names, endpoints and player records dword by dword. */
template <class T> static inline uint32_t *FrontendNetwork_Dwords(T *record)
{
  return reinterpret_cast<uint32_t *>(record);
}
/* The typed row pointer arrays of the session and player lists as the untyped row slots of a UiPointerListControl
   (same 4-byte Ptr32 entries). */
template <class T> static inline Ptr32<void> *FrontendNetwork_RowSlots(Ptr32<T> *rows)
{
  return reinterpret_cast<Ptr32<void> *>(rows);
}

/* Module data. */

uint16_t g_FrontendLocalPlayerNameUtf16[20] = {};

Ptr32<FrontendSessionDiscoveryRecord> *g_FrontendSessionListRows = nullptr;

Ptr32<FrontendPlayerRuntimeRecord> g_FrontendPlayerRuntimeRecordPointers32[32] = {};

uint32_t g_FrontendNetworkState = 0;

char g_SpielerSpielNetzwerkHostKeywordsAscii[31] = "SPIELER=\"SPIEL=\"NETZWERK=\"HOST";

/* Original quirk: the string's terminating NUL is the
   first byte of the Package_FindEntry output buffer g_LevelPackageFoundEntry; the code passes explicit lengths. */
char g_NameClientKarteKeywordsAscii[21] = {'N', 'A', 'M', 'E', '=', '"', 'C', 'L', 'I', 'E', 'N', 'T', '=', '"', 'K', 'A', 'R', 'T', 'E', '=', '"'}; /* "NAME=\"CLIENT=\"KARTE=\"" without its NUL */

UiTransferEndpointDescriptor g_FrontendNetworkEndpointScratch = {};

uint16_t g_FrontendNetworkRuntimeCountTextUtf16[4] = {};

uint16_t g_FrontendNetworkPlayerCountTextUtf16[4] = {};

uint16_t g_FrontendNetworkEndpointTextUtf16[512] = {};

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
      return nullptr;
    }
  }
  return nullptr;
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
static void FrontendNetworkSetupPage_ApplyNameOption()
{
  uint8_t *option;
  uint8_t *closingQuote;
  uint16_t *playerNameText;

  option = g_CommandLineFindOption(6,g_NameClientKarteKeywordsAscii);
  if (option == nullptr) {
    return;
  }
  closingQuote = CommandLineOption_FindClosingQuote(option + 6,19);
  if (closingQuote == nullptr) {
    return;
  }
  playerNameText = FrontendUi_Image(g_FrontendRootNode)->playerNameEdit.textBuffer;
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
static void FrontendNetworkSetupPage_ApplyClientOption()
{
  uint8_t *option;
  uint8_t *closingQuote;
  Bool8 endpointParseFailed;

  option = g_CommandLineFindOption(8,g_NameClientKarteKeywordsAscii + 6);
  if (option == nullptr) {
    return;
  }
  closingQuote = CommandLineOption_FindClosingQuote(option + 8,FRONTEND_CLIENT_OPTION_SCAN_LIMIT);
  if (closingQuote == nullptr) {
    return;
  }
  *closingQuote = 0;
  if (closingQuote[1] != 0) {
    return;
  }
  *option = 'c';
  Text_CopyNarrowToUtf16
            (PACKAGE_SCRATCH_BUFFER_BYTES,reinterpret_cast<uint16_t *>(g_PackageScratchBuffer) /* the scratch bytes as UTF-16 text */,option + 8);
  endpointParseFailed = g_NetworkBackendSlot6
                    (&g_FrontendSelectedNetworkEndpoint,FrontendNetwork_TextBytes(g_PackageScratchBuffer));
  if (endpointParseFailed) {
    return;
  }
  g_NetworkBackendSlot6(&g_FrontendNetworkEndpointScratch,FrontendNetwork_TextBytes(g_PackageScratchBuffer));
  g_FrontendSessionToken = FRONTEND_SEQUENCE_TOKEN_HIGH_WORD;
  g_FrontendSelectedPlayerToken = 0xffffffff;
  UiTransfer_SendPlayerDescriptor();
  g_NetworkBackendSlot7
            (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),
             FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch));
  RichTextCommandStream_CopyExpanded
            (128,FrontendUi_Image(g_FrontendRootNode)->hostAddressEdit.textBuffer,
             g_FrontendNetworkEndpointTextUtf16,nullptr);
}

/* Shows the network game page; on small screens (width up to FRONTEND_COMPACT_LAYOUT_MAX_WIDTH) the menu room
   behind it stops rendering. */
void FrontendNetworkGamePage_Show(FrontendUiImage *frontendUi)
{
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,UiLayoutContainerControl_AsPageStack(&frontendUi->frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    frontendUi->menuRoomModelView.contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
}

/* Empties the session list of the network game page and hides its Join button until a session is chosen. */
void FrontendNetworkGamePage_ClearSessionList(FrontendUiImage *frontendUi)
{
  UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&frontendUi->frontendRoot.root.base);
  UiPointerList_InitializeColumnLayout
            (0,FrontendNetwork_RowSlots(g_FrontendSessionListRows),UiListControl_AsPointerList(&frontendUi->sessionList));
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
              (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),
               FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch));
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,&frontendUi->frontendRoot.root.base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&frontendUi->frontendRoot.root.base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendUi->frontendRoot.root.base);
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,UiLayoutContainerControl_AsPageStack(&frontendUi->frontendPageStack));
    if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
      frontendUi->menuRoomModelView.contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    }
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
    UiPointerList_InitializeColumnLayout
              (1,FrontendNetwork_RowSlots(g_FrontendPlayerRuntimeRecordPointers32),
               UiListControl_AsPointerList(&frontendUi->hostLobbyPlayerList));
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,g_FrontendNetworkRuntimeCountTextUtf16
              );
    /* the local player becomes the only player record: name, local endpoint, command sync pending */
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    sequenceToken = g_UiTransferSequenceToken;
    g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
    firstPlayerRecord->peerSequenceToken = sequenceToken;
    localPlayerNameDwordCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
    localPlayerRecordDwordCursor = FrontendNetwork_Dwords(&firstPlayerRecord->playerName);
    for (dwordsRemaining = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); dwordsRemaining != 0;
         dwordsRemaining--) {
      *localPlayerRecordDwordCursor = *localPlayerNameDwordCursor;
      localPlayerNameDwordCursor++;
      localPlayerRecordDwordCursor++;
    }
    /* the cursor continues into firstPlayerRecord->endpoint and then commandSyncPending */
    endpointSourceDwordCursor = FrontendNetwork_Dwords(&g_NetworkLocalEndpoint);
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
              (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),
               FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch));
    RichTextCommandStream_CopyExpanded
              (128,frontendUi->hostAddressEdit.textBuffer,
               g_FrontendNetworkEndpointTextUtf16,nullptr);
    FrontendNetworkGamePage_Show(frontendUi);
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,&frontendUi->frontendRoot.root.base);
    FrontendNetworkGamePage_ClearSessionList(frontendUi);
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
  /* networkProtocolList is a text list: the same prefix as a pointer list */
  backendIndex = UiPointerList_GetSelectedIndexAndConfirmed
                          (reinterpret_cast<UiPointerListControl *>(&frontendUi->networkProtocolList),nullptr);
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
      FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
      return;
    }
  }
  /* backend backendIndex is open */
  UiPointerList_SelectColumnListIndex
            (backendIndex,reinterpret_cast<UiPointerListControl *>(&frontendUi->networkProtocolList));
  localEndpointCursor = FrontendNetwork_Dwords(&g_NetworkLocalEndpoint);
  endpointDestinationDwordCursor = FrontendNetwork_Dwords(&g_FrontendNetworkEndpointScratch);
  /* copies the 16-byte local endpoint dword by dword */
  for (dwordsRemaining = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); dwordsRemaining != 0;
       dwordsRemaining--) {
    *endpointDestinationDwordCursor = *localEndpointCursor;
    localEndpointCursor++;
    endpointDestinationDwordCursor++;
  }
  FrontendNetworkSetupPage_ApplyNameOption();
  g_NetworkBackendSlot7
            (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),
             FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch));
  FrontendNetworkGamePage_Show(frontendUi);
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
  /* hosting needs a player name */
  if (g_FrontendLocalPlayerNameUtf16[0] == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_HOST_GAME,&frontendUi->frontendRoot.root.base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,&frontendUi->frontendRoot.root.base);
  }
  FrontendNetworkGamePage_ClearSessionList(frontendUi);
  UiTransfer_SendDiscoveryProbe();
  /* -HOST opens the host setup at once; otherwise -CLIENT="host address" may join a host */
  hostOption = g_CommandLineFindOption(5,g_SpielerSpielNetzwerkHostKeywordsAscii + 26);
  if (hostOption != nullptr) {
    *hostOption = 'h';
    FrontendNetworkSetupPage_InitializeFromCommandLine
              (&frontendUi->networkGameHostButton.selectable.base);
  }
  else {
    FrontendNetworkSetupPage_ApplyClientOption();
  }
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
  g_FrontendRootInitializationTemplate.bottomBarStatusText.text =
       FrontendUi_Image(root)->bottomBarStatusText.text;
  sourceCursor = reinterpret_cast<int32_t *>(FrontendUi_Image(root)->hostAddressEdit.textBuffer) /* the text dword by dword */;
  destinationCursor =
       reinterpret_cast<int32_t *>(g_FrontendRootInitializationTemplate.hostAddressEdit.textBuffer) /* the text dword by dword */;
  g_InGameRuntimeDefaultImageTemplate.worldViewCyclingInfoText.text = g_FrontendRootInitializationTemplate.bottomBarStatusText.text;
  for (dwordsRemaining = 32; dwordsRemaining != 0; dwordsRemaining--) { /* 0x40 code units */
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
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
                    (&g_FrontendNetworkEndpointScratch,FrontendNetwork_TextBytes(hostAddressEdit->textBuffer));
  if (endpointParseFailed) {
    hostAddressEdit->editStateFlags =
         hostAddressEdit->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
    return;
  }
  hostAddressEdit->editStateFlags =
       hostAddressEdit->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  UiTransfer_SendDiscoveryProbe();
  g_NetworkBackendSlot7
            (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch)
            );
}


/* Action 0x2003 of the back button on the host game setup page (g_FrontendUiActionHandlersPage20 slot 3):
   returns to the network game page, stops the menu room rendering behind it on small screens, resumes
   browsing with the join button hidden and an empty session list, and sends a new session discovery probe.
*/
void FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source)

{
  /* source is the frontend template's hostGameSetupBackButton. */
  FrontendUiImage *frontendUi;

  frontendUi = reinterpret_cast<FrontendUiImage *>(Thandor_Bytes(source) - offsetof(FrontendUiImage,hostGameSetupBackButton));
  FrontendNetworkGamePage_Show(frontendUi);
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
  FrontendNetworkGamePage_ClearSessionList(frontendUi);
  UiTransfer_SendDiscoveryProbe();
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
  if (option != nullptr) {
    maxPlayers = CommandLineOption_ParseQuotedDigit(option,9);
    if (maxPlayers < 9 && 1 < maxPlayers) {
      *option = 's';
      appliedOptionMask = 1;
      FrontendUi_Image(g_FrontendRootNode)->maxPlayersSlider.value = maxPlayers;
    }
  }
  /* -SPIEL="game name".
     Original quirk: when more text follows the closing quote, the quote is left overwritten with a terminator. */
  option = g_CommandLineFindOption(7,g_SpielerSpielNetzwerkHostKeywordsAscii + 9);
  if (option != nullptr) {
    closingQuote = CommandLineOption_FindClosingQuote(option + 7,19);
    if (closingQuote != nullptr) {
      gameNameText = FrontendUi_Image(g_FrontendRootNode)->gameNameEdit.textBuffer;
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
  if (option != nullptr) {
    optionNetworkSpeed = CommandLineOption_ParseQuotedDigit(option,10);
    if (optionNetworkSpeed < 8 && optionNetworkSpeed != 0) {
      *option = 'n';
      FrontendUi_Image(g_FrontendRootNode)->networkSpeedSlider.value =
           optionNetworkSpeed;
      appliedOptionMask = appliedOptionMask + 4;
      g_SessionNetworkTickInterval = optionNetworkSpeed * 2;
    }
  }
  frontendUi = reinterpret_cast<FrontendUiImage *>(Thandor_Bytes(hostButton) - offsetof(FrontendUiImage,networkGameHostButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_GAME_SETUP,UiLayoutContainerControl_AsPageStack(&frontendUi->frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    frontendUi->menuRoomModelView.contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
             frontendUi->maxPlayersSlider.value,
             g_FrontendNetworkPlayerCountTextUtf16);
  networkSpeed = g_SessionNetworkTickInterval >> 1;
  frontendUi->networkSpeedSlider.value = networkSpeed;
  /* the speed caption */
  resolvedText = TextResource_Resolve(networkSpeed + TEXT_ID_NETWORK_SPEED_BASE);
  RichTextCommandStream_CopyExpanded
            (64,g_FrontendNetworkSpeedLabelUtf16,resolvedText,nullptr);
  /* gameNameEdit is a UiRequiredTextEditControl, the same layout as a UiTextEditControl */
  UiTextControl_UpdateNonEmptyValidity(reinterpret_cast<UiTextEditControl *>(&frontendUi->gameNameEdit));
  FrontendNetworkSettings_SetGameName(reinterpret_cast<UiTextEditControl *>(&frontendUi->gameNameEdit));
  if (appliedOptionMask == 7) {
    FrontendNetworkSetupPage_InitializeSingleLocalPlayer(&frontendUi->hostGameCreateButton.selectable.base);
  }
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
  
  frontendUi = reinterpret_cast<FrontendUiImage *>(Thandor_Bytes(createButton) - offsetof(FrontendUiImage,hostGameCreateButton));
  UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendUi->frontendRoot.root.base);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_HOST_LOBBY,UiLayoutContainerControl_AsPageStack(&frontendUi->frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    frontendUi->menuRoomModelView.contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOSTING;
  UiPointerList_InitializeColumnLayout
            (1,FrontendNetwork_RowSlots(g_FrontendPlayerRuntimeRecordPointers32),
             UiListControl_AsPointerList(&frontendUi->hostLobbyPlayerList));
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,g_FrontendNetworkRuntimeCountTextUtf16);
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  sequenceToken = g_UiTransferSequenceToken;
  g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff; /* the local player never times out */
  firstPlayerRecord->peerSequenceToken = sequenceToken;
  firstPlayerRecord->playerRuntimeId = 0;
  localPlayerNameCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
  localPlayerRecordDwordCursor = FrontendNetwork_Dwords(&firstPlayerRecord->playerName);
  for (remainingDwords = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *localPlayerRecordDwordCursor = *localPlayerNameCursor;
    localPlayerNameCursor++;
    localPlayerRecordDwordCursor++;
  }
  localEndpointDwordCursor = FrontendNetwork_Dwords(&g_NetworkLocalEndpoint);
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
                    (reinterpret_cast<PcxPreview64 *>(localPlayerRecordDwordCursor + 24),
                     reinterpret_cast<uint16_t *>(localPlayerRecordDwordCursor + -14));
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
}

/* Handler of action 0x200F (slot 15 of g_FrontendUiActionHandlersPage20.handlers00_54), a choice in the network
   game page's protocol list: closes the current backend and opens the chosen one on NETWORK_GAME_UDP_PORT. On
   success the local endpoint is copied to g_FrontendNetworkEndpointScratch and formatted into
   g_FrontendNetworkEndpointTextUtf16, the session list is emptied, Join hidden and a discovery probe sent. A failure is reported and the backend opened once more without a report; if that
   fails too, the menu returns to the main page and the random generator to the primary stream.
*/
void FrontendNetworkSetup_OpenSelectedBackend(FrontendNetworkSetupPageBackendListPtr backendList)

{
  UiListRowIndex selectedBackendIndex;
  int remainingDwords;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  uint32_t backendError; /* 0 or a FATAL_ERROR_NETWORK_* code */

  selectedBackendIndex = UiPointerList_GetSelectedIndexAndConfirmed(backendList,nullptr);
  if (g_NetworkBackendInstanceCount <= selectedBackendIndex) {
    return;
  }
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  backendError = g_NetworkBackendSlot0(selectedBackendIndex);
  FatalError_ReportIfFailed(backendError,backendError != 0); /* reports and returns: the flag is ours */
  if (backendError == 0) {
    backendError = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    FatalError_ReportIfFailed(backendError,backendError != 0);
    if (backendError == 0) {
      endpointSourceDwordCursor = FrontendNetwork_Dwords(&g_NetworkLocalEndpoint);
      endpointDestinationDwordCursor = FrontendNetwork_Dwords(&g_FrontendNetworkEndpointScratch);
      for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor++;
        endpointDestinationDwordCursor++;
      }
      g_NetworkBackendSlot7
                (FrontendNetwork_TextBytes(g_FrontendNetworkEndpointTextUtf16),
                 FrontendNetwork_SocketAddress(&g_FrontendNetworkEndpointScratch));
      UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->rootNode);
      UiPointerList_InitializeColumnLayout
                (0,FrontendNetwork_RowSlots(g_FrontendSessionListRows),&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->sessionList);
      UiTransfer_SendDiscoveryProbe();
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  if (g_NetworkBackendSlot0(selectedBackendIndex) == 0) {
    if (g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT) == 0) {
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
  Random_SelectPrimaryStream();
}

/* Change handler of the network game page's player-name edit (playerNameEdit, action 0x2032, slot 50 of
   g_FrontendUiActionHandlersPage20). An empty name hides Host and Join; a valid one shows Host, lets the session
   list decide about Join, and is saved as PERSISTENT_SETTING_PLAYER_NAME and copied to the local player's name
   (20 UTF-16 code units).
*/
void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control)

{
  UiNodeBase *parentCursor;
  UiTextEditControl *rootNode;
  int remainingDwords;
  uint32_t *sourceDwordCursor;
  uint32_t *playerNameDwordCursor;

  parentCursor = control->base.parent;
  rootNode = control;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = UiNode_As<UiTextEditControl>((rootNode->base).parent.get());
    parentCursor = rootNode->base.parent;
  }
  UiTextControl_UpdateNonEmptyValidity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_HOST_GAME,&rootNode->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,&rootNode->base);
    FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick(&FrontendUi_Image(rootNode)->sessionList);
    PersistentSettings_WriteBlock(PERSISTENT_SETTINGS_NAME_BYTES,FrontendNetwork_Dwords(control->textBuffer),
                                  PERSISTENT_SETTING_PLAYER_NAME);
    sourceDwordCursor = FrontendNetwork_Dwords(control->textBuffer);
    playerNameDwordCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
    for (remainingDwords = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); remainingDwords != 0;
         remainingDwords--) {
      *playerNameDwordCursor = *sourceDwordCursor;
      sourceDwordCursor++;
      playerNameDwordCursor++;
    }
  }
}

/* Handler of the host game setup page's player-count slider (maxPlayersSlider, action 0x2007, slot 7 of
   g_FrontendUiActionHandlersPage20): saves the value as PERSISTENT_SETTING_NETWORK_PLAYER_COUNT and formats it
   into the slider's number text.
*/
void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_NETWORK_PLAYER_COUNT);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,value,
             g_FrontendNetworkPlayerCountTextUtf16);
}

/* Change handler of the host game setup page's game-name edit: the create button (FRONTEND_ACTION_CREATE_HOSTED_GAME)
   is only offered while the name is valid (non-empty), and a valid name is saved as PERSISTENT_SETTING_GAME_NAME.
*/
void FrontendNetworkSettings_SetGameName(UiTextEditControl *control)

{
  UiTextEditControl *rootNode;
  UiNodeBase *parentCursor;
  
  parentCursor = control->base.parent;
  rootNode = control;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = UiNode_As<UiTextEditControl>((rootNode->base).parent.get());
    parentCursor = rootNode->base.parent;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_CREATE_HOSTED_GAME,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_CREATE_HOSTED_GAME,&rootNode->base);
    PersistentSettings_WriteBlock(PERSISTENT_SETTINGS_NAME_BYTES,FrontendNetwork_Dwords(control->textBuffer),
                                  PERSISTENT_SETTING_GAME_NAME);
  }
}

/* Handler of the network game page's session list (sessionList, action 0x2009, slot 9 of
   g_FrontendUiActionHandlersPage20; also called by FrontendNetworkSettings_SetPlayerName). Join
   (FRONTEND_ACTION_JOIN_GAME) is offered only while the list has rows (rowCount), its selected row (selectedRowSlot)
   holds a session (advertisement.joinAvailableFlag) and the local player has a name; if then bit 2 of the
   list's listStateFlags is set
   (presumably a double click), it is cleared and the join request is sent at once, as if Join had been pressed.
*/
void FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick(UiListControl *sessionList)

{
  UiListStateFlags *dirtyFlagsSlot;
  UiNodeBase *parentCursor;
  UiNodeBase *rootNode;

  parentCursor = sessionList->base.parent;
  rootNode = &sessionList->base;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = rootNode->parent;
    parentCursor = rootNode->parent;
  }
  /* sessionList is a UiListControl of FrontendSessionDiscoveryRecord rows */
  if (((sessionList->rowCount == 0) ||
      (static_cast<FrontendSessionDiscoveryRecord *>(sessionList->selectedRowSlot->get())->advertisement.
       joinAvailableFlag == 0)) ||
     (g_FrontendLocalPlayerNameUtf16[0] == 0)) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,rootNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_JOIN_GAME,rootNode);
    if ((sessionList->listStateFlags & 4) != 0) {
      dirtyFlagsSlot = &sessionList->listStateFlags;
      *dirtyFlagsSlot = *dirtyFlagsSlot & ~4;
      FrontendNetworkSettings_PublishSelectedPlayerDescriptor(&FrontendUi_Image(rootNode)->networkGameJoinButton);
    }
  }
}

/* Handler of the network game page's Join button (FRONTEND_ACTION_JOIN_GAME, slot 2 of
   g_FrontendUiActionHandlersPage20; also called by FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick):
   takes the session token (advertisement.header.sequenceToken) and host endpoint (senderEndpoint, 16 bytes)
   of the selected row (selectedRowSlot) of the sibling sessionList and sends the join request (player
   descriptor packet 0x20002) to it. Returns the result of UiTransfer_SendPlayerDescriptor.
*/
Bool8 FrontendNetworkSettings_PublishSelectedPlayerDescriptor(UiFramedTextButtonControl *joinButton)

{
  int remainingDwords;
  uint32_t *selectedPlayerRecordDwordCursor;
  uint32_t *selectedEndpointDwordCursor;
  Bool8 sendCarry;
  
  /* joinButton is the frontend template's networkGameJoinButton; the session list is a sibling. */
  g_FrontendSessionToken =
       static_cast<FrontendSessionDiscoveryRecord *>
       (FrontendUi_Image(Thandor_Bytes(joinButton)
 - offsetof(FrontendUiImage,networkGameJoinButton))->sessionList.selectedRowSlot->get())->advertisement.header.sequenceToken;
  selectedPlayerRecordDwordCursor =
       FrontendNetwork_Dwords(&static_cast<FrontendSessionDiscoveryRecord *>
                     (FrontendUi_Image(Thandor_Bytes(joinButton)
 -
                                       offsetof(FrontendUiImage,networkGameJoinButton))->sessionList.selectedRowSlot->get())->senderEndpoint);

  selectedEndpointDwordCursor = FrontendNetwork_Dwords(&g_FrontendSelectedNetworkEndpoint);
  for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor++;
    selectedEndpointDwordCursor++;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  sendCarry = UiTransfer_SendPlayerDescriptor();
  return sendCarry;
}
