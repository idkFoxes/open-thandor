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
   Ownership: ui/frontend/network.
   Purpose: Initializes the network setup page for dedicated host, client, or local backend modes, selects an
   available adapter, applies command-line client and host options, seeds the first player descriptor, and updates
   page actions.
   Local calls: FrontendNetworkSetupPage_InitializeFromCommandLine.
   Cross-module calls: UiNodeList_UnsuppressActionId [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPageStack_SetActiveIndex [ui/controls/layout], UiPointerList_InitializeColumnLayout
   [ui/controls/lists], RichTextCommandStream_CopyExpandedCf [assets/text/richtext],
   UiTransfer_SendPlayerDescriptorPacket20002Cf [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx
FrontendNetworkSetupPage_InitializeBackendMode(FrontendUiImage *frontendUi)

{
  byte optionChar;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  UiListRowIndex backendIndex;
  dword errorOrValue;
  int remainingOrRootNode;
  word *destination;
  byte *commandLineOptionBytes;
  byte *secondaryCommandLineOptionBytes;
  UiTransferEndpointDescriptor *localEndpointCursor;
  dword *localPlayerNameDwordCursor;
  dword *endpointSourceDwordCursor;
  dword *endpointDestinationDwordCursor;
  byte *optionTextCursor;
  dword *localPlayerRecordDwordCursor;
  bool endpointParseFailed;
  NetworkBackendSetSessionEaxCf5 setSessionResult;
  NetworkBackendOpenBindEaxCf5 openBindResult;
  CommandLineFindOptionEbxCf5 findOptionResult;
  UiListRowIndex selectedBackendIndex;
  dword sequenceTokenOrBackendIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    (*g_NetworkBackendSlot7)
              (&g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    UiNodeList_UnsuppressActionId(0x2001,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(0x2002,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(0x200b,FRONTEND_UI(frontendUi,frontendRoot));
    UiPageStack_SetActiveIndex(3,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < 0x281) {
      FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) =
           FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) | 0x2000;
    }
    g_FrontendNetworkState = 2;
    UiPointerList_InitializeColumnLayout
              (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
               (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(word *)&g_FrontendNetworkRuntimeCountTextUtf16
              );
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    sequenceTokenOrBackendIndex = g_UiTransferSequenceToken;
    g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff;
    firstPlayerRecord->peerSequenceToken = sequenceTokenOrBackendIndex;
    localPlayerNameDwordCursor = (void *)g_FrontendLocalPlayerNameUtf16;
    localPlayerRecordDwordCursor = (dword *)&firstPlayerRecord->playerName;
    for (remainingOrRootNode = 10; remainingOrRootNode != 0; remainingOrRootNode = remainingOrRootNode + -1) {
      *(dword *)((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 =
           *localPlayerNameDwordCursor;
      localPlayerNameDwordCursor = localPlayerNameDwordCursor + 1;
      localPlayerRecordDwordCursor =
           (dword *)(((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 + 2);
    }
    endpointSourceDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
    for (remainingOrRootNode = 4; remainingOrRootNode != 0; remainingOrRootNode = remainingOrRootNode + -1) {
      *localPlayerRecordDwordCursor = *endpointSourceDwordCursor;
      endpointSourceDwordCursor = endpointSourceDwordCursor + 1;
      localPlayerRecordDwordCursor = localPlayerRecordDwordCursor + 1;
    }
    *localPlayerRecordDwordCursor = 1;
    g_FrontendPendingSessionPlayerCount = 0;
    g_FrontendPlayerRuntimeCount = 1;
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_HOST;
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    (*g_NetworkBackendSlot7)
              (&g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    RichTextCommandStream_CopyExpandedCf
              (0x80,((UiRequiredTextEditControl *)FRONTEND_UI(frontendUi,hostAddressEdit))->textPrefix6C,
               (word *)&g_FrontendNetworkEndpointTextUtf16);
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
    if ((int)g_FramebufferWidth < 0x281) {
      FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) =
           FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) | 0x2000;
    }
    g_FrontendNetworkState = 1;
    UiNodeList_UnsuppressActionId(0x2001,FRONTEND_UI(frontendUi,frontendRoot));
    UiNodeList_SuppressActionId(0x2002,FRONTEND_UI(frontendUi,frontendRoot));
    UiPointerList_InitializeColumnLayout
              (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_CLIENT;
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    (firstPlayerRecord->playerName).textUtf16[0] = 0;
    (firstPlayerRecord->playerName).textUtf16[1] = 0;
    firstPlayerRecord->playerRuntimeId = 0;
    (firstPlayerRecord->factionAssignment).roleStateFlags = 0;
    firstPlayerRecord->runtimeState64 = 0;
    firstPlayerRecord->snapshotTransferFlags = 0;
    UiTransfer_SendPlayerDescriptorPacket20002Cf();
    return;
  }
  UiRuntimeRecordRing_Clear();
  UiTransferMailbox_RandomizeSequenceToken();
  backendIndex = UiPointerList_GetSelectedIndexVariantACf
                          ((UiPointerListControl *)FRONTEND_UI(frontendUi,networkProtocolList));
  selectedBackendIndex = backendIndex;
  setSessionResult = (*g_NetworkBackendSlot0)(backendIndex);
  if (!setSessionResult.carry) {
    openBindResult = (*g_NetworkBackendSlot2)(0x3a1);
    if (!openBindResult.carry) {
FrontendNetworkSetup_CommitSelectedBackendAndInitializeClientPage:
      UiPointerList_SelectIndexVariantA
                (backendIndex,(UiPointerListControl *)FRONTEND_UI(frontendUi,networkProtocolList));
      localEndpointCursor = &g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (dword *)&g_FrontendNetworkEndpointScratch;
      for (remainingOrRootNode = 4; remainingOrRootNode != 0; remainingOrRootNode = remainingOrRootNode + -1) {
        *endpointDestinationDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, dword, localEndpointCursor->addressHeader);
        localEndpointCursor = (UiTransferEndpointDescriptor *)&localEndpointCursor->ipv4AddressNetworkOrder;
        endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
      }
      findOptionResult = (*g_CommandLineFindOption)(6,s_NAME__CLIENT__KARTE___00545e91);
      commandLineOptionBytes = findOptionResult.ebx;
      if (!findOptionResult.carry) {
        optionTextCursor = commandLineOptionBytes + 6;
        remainingOrRootNode = 0x13;
        while( true ) {
          optionChar = *optionTextCursor;
          if ((optionChar == 0) || (optionChar < 0x20))
          goto FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery;
          if (optionChar == 0x22) break;
          remainingOrRootNode = remainingOrRootNode + -1;
          if (remainingOrRootNode == 0) goto FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery;
          optionTextCursor = optionTextCursor + 1;
        }
        destination = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,playerNameEdit))->textPrefix6C;
        *optionTextCursor = 0;
        if (optionTextCursor[1] == 0) {
          *commandLineOptionBytes = 0x6e;
          Text_CopyNarrowToUtf16Cf(0x28,destination,commandLineOptionBytes + 6);
          Text_CopyNarrowToUtf16Cf
                    (0x28,g_FrontendLocalPlayerNameUtf16,commandLineOptionBytes + 6);
          *optionTextCursor = 0x22;
        }
      }
FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery:
      (*g_NetworkBackendSlot7)
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
      if ((int)g_FramebufferWidth < 0x281) {
        FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) =
           FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) | 0x2000;
      }
      g_FrontendNetworkState = 1;
      if (g_FrontendLocalPlayerNameUtf16[0] == 0) {
        UiNodeList_SuppressActionId(0x2001,FRONTEND_UI(frontendUi,frontendRoot));
      }
      else {
        UiNodeList_UnsuppressActionId(0x2001,FRONTEND_UI(frontendUi,frontendRoot));
      }
      UiNodeList_SuppressActionId(0x2002,FRONTEND_UI(frontendUi,frontendRoot));
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
      UiTransfer_SendPacketType10000Value2931Cf();
      findOptionResult = (*g_CommandLineFindOption)(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
      if (findOptionResult.carry) {
        findOptionResult = (*g_CommandLineFindOption)(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
        secondaryCommandLineOptionBytes = findOptionResult.ebx;
        if (!findOptionResult.carry) {
          remainingOrRootNode = 0x7fffff;
          optionTextCursor = secondaryCommandLineOptionBytes + 8;
          while( true ) {
            optionChar = *optionTextCursor;
            if (optionChar == 0) {
              return;
            }
            if (optionChar < 0x20) {
              return;
            }
            if (optionChar == 0x22) break;
            remainingOrRootNode = remainingOrRootNode + -1;
            if (remainingOrRootNode == 0) {
              return;
            }
            optionTextCursor = optionTextCursor + 1;
          }
          *optionTextCursor = 0;
          if (optionTextCursor[1] == 0) {
            *secondaryCommandLineOptionBytes = 99;
            Text_CopyNarrowToUtf16Cf
                      (0x800000,(word *)g_PackageScratchBuffer,secondaryCommandLineOptionBytes + 8);
            endpointParseFailed = (*g_NetworkBackendSlot6)
                              (&g_FrontendSelectedNetworkEndpoint,(char *)g_PackageScratchBuffer);
            if (!endpointParseFailed) {
              (*g_NetworkBackendSlot6)
                        (&g_FrontendNetworkEndpointScratch,(char *)g_PackageScratchBuffer);
              g_FrontendSessionToken = 0x12340000;
              g_FrontendSelectedPlayerToken = 0xffffffff;
              UiTransfer_SendPlayerDescriptorPacket20002Cf();
              remainingOrRootNode = g_FrontendRootNode;
              (*g_NetworkBackendSlot7)
                        (&g_FrontendNetworkEndpointTextUtf16,
                         (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
              RichTextCommandStream_CopyExpandedCf
                        (0x80,
                         ((UiRequiredTextEditControl *)FRONTEND_UI(remainingOrRootNode,hostAddressEdit))
                         ->textPrefix6C,(word *)&g_FrontendNetworkEndpointTextUtf16);
            }
          }
        }
      }
      else {
        *findOptionResult.ebx = 0x68;
        FrontendNetworkSetupPage_InitializeFromCommandLine
                  (FRONTEND_UI(frontendUi,networkGameHostButton));
      }
      return;
    }
    (*g_NetworkBackendSlot1)(); /* cleanup takes no arguments; Ghidra passed a stale register */
  }
  backendIndex = 0;
  do {
    sequenceTokenOrBackendIndex = backendIndex;
    setSessionResult = (*g_NetworkBackendSlot0)(backendIndex);
    errorOrValue = setSessionResult.eax;
    if (!setSessionResult.carry) {
      openBindResult = (*g_NetworkBackendSlot2)(0x3a1);
      errorOrValue = openBindResult.eax;
      if (!openBindResult.carry) goto FrontendNetworkSetup_CommitSelectedBackendAndInitializeClientPage;
      (*g_NetworkBackendSlot1)(); /* cleanup takes no arguments; Ghidra passed a stale register */
    }
    backendIndex = backendIndex + 1;
    if (g_NetworkBackendInstanceCount <= backendIndex) {
      (*g_FatalErrorRuntimeDispatchCf)(errorOrValue,true);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
      }
      return;
    }
  } while( true );
}


/* Address: 0x005474A0.
   Ownership: ui/frontend/network.
   Purpose: On frontend teardown, saves the status text resource id and the host address text of the frontend
   template copy back into the frontend template (and the status id into the in-game template).
*/
void __thandor_void_preserve_eax_ecx FrontendTeardown_SaveRootStateSnapshot80(UiRootNode *root)

{
  int dwordsRemaining;
  sdword *sourceCursor;
  sdword *destinationCursor;
  
  /* root is the frontend template copy; both values are written back into the frontend template
     (bottomBarStatusText's text resource id, mirrored into the in-game template's worldViewCyclingInfoText,
     and the 0x40-code-unit hostAddressEdit text). */
  g_FrontendTemplateStatusTextResourceId =
       (TextResourceId)((UiSingleLineTextControl *)FRONTEND_UI(root,bottomBarStatusText))->text;
  sourceCursor = (sdword *)((UiRequiredTextEditControl *)FRONTEND_UI(root,hostAddressEdit))->textPrefix6C;
  destinationCursor = (sdword *)g_FrontendHostAddressTextTemplate;
  g_InGameTemplateWorldViewInfoTextResourceId = g_FrontendTemplateStatusTextResourceId;
  for (dwordsRemaining = 0x20; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
    *destinationCursor = *sourceCursor;
    sourceCursor = sourceCursor + 1;
    destinationCursor = destinationCursor + 1;
  }
  return;
}


/* Address: 0x0054C7D0.
   Ownership: ui/frontend/network.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[13]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[13] (0x200D). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 transferPageControl→UiTextEditControl *. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiTransfer_SendPacketType10000Value2931Cf [network/protocol/transfer].
*/
void __thandor_preserve_eax_edx
FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *transferPageControl)

{
  bool endpointParseFailed;
  
  endpointParseFailed = (*g_NetworkBackendSlot6)
                    (&g_FrontendNetworkEndpointScratch,(char *)transferPageControl->textPrefix6C);
  if (endpointParseFailed) {
    transferPageControl->editStateFlags =
         transferPageControl->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
    return;
  }
  transferPageControl->editStateFlags =
       transferPageControl->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  UiTransfer_SendPacketType10000Value2931Cf();
  (*g_NetworkBackendSlot7)
            (&g_FrontendNetworkEndpointTextUtf16,(WinSockAddress *)&g_FrontendNetworkEndpointScratch
            );
  return;
}


/* Address: 0x0054CE10.
   Ownership: ui/frontend/network.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[3]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[3] (0x2003). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiTransfer_SendPacketType10000Value2931Cf [network/protocol/transfer].
*/
void __thandor_preserve_eax FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source)

{
  /* source is the frontend template's hostGameSetupBackButton (+0x4F94). */
  FrontendUiImage *frontendUi;

  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(source,-0x4f94);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,sdword) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,sdword) | 0x2000;
  }
  g_FrontendNetworkState = 1;
  UiNodeList_SuppressActionId(0x2002,FRONTEND_UI(frontendUi,frontendRoot));
  UiPointerList_InitializeColumnLayout
            (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
  UiTransfer_SendPacketType10000Value2931Cf();
  return;
}


/* Address: 0x0054C830.
   Ownership: ui/frontend/network.
   Purpose: Initializes the network setup page from SPIELER, SPIEL, and NETZWERK command-line options, formats the
   selected counts, applies compact-layout state, and refreshes game-name validity. Queued UI action handler for
   FRONTEND_PAGE20[1] (0x2001). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSetupPage_InitializeSingleLocalPlayer.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_CopyExpandedCf
   [assets/text/richtext], UiTextControl_UpdateNonEmptyValidity [ui/controls/text],
   FrontendNetworkSettings_SetGameName [ui/frontend/settings].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendNetworkSetupPage_InitializeFromCommandLine(UiNodeBase *hostButton)

{
  /* hostButton is the frontend template's networkGameHostButton (+0x4920). */
  FrontendUiImage *frontendUi;
  byte optionChar;
  uint parsedCountOrTickSetting;
  int remainingChars;
  word *destination;
  int appliedOptionMask;
  byte *optionText;
  byte *optionTextCursor;
  TextResourceResolveEaxCf5 resolvedText;
  CommandLineFindOptionEbxCf5 findOptionResult;
  
  appliedOptionMask = 0;
  findOptionResult = (*g_CommandLineFindOption)(9,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72);
  optionText = findOptionResult.ebx;
  if ((((!findOptionResult.carry) && (optionText[10] == 0x22)) &&
      (parsedCountOrTickSetting = optionText[9] - 0x30, 0x2f < optionText[9] && parsedCountOrTickSetting != 0)) &&
     (((optionText[0xb] == 0 && (parsedCountOrTickSetting < 9)) && (1 < parsedCountOrTickSetting)))) {
    *optionText = 0x73;
    appliedOptionMask = 1;
    ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,maxPlayersSlider))->value =
         parsedCountOrTickSetting;
  }
  findOptionResult = (*g_CommandLineFindOption)(7,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 9);
  optionText = findOptionResult.ebx;
  if (!findOptionResult.carry) {
    optionTextCursor = optionText + 7;
    remainingChars = 0x13;
    while( true ) {
      optionChar = *optionTextCursor;
      if ((optionChar == 0) || (optionChar < 0x20))
      goto FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage;
      if (optionChar == 0x22) break;
      remainingChars = remainingChars + -1;
      if (remainingChars == 0) goto FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage;
      optionTextCursor = optionTextCursor + 1;
    }
    destination = ((UiRequiredTextEditControl *)FRONTEND_UI(g_FrontendRootNode,gameNameEdit))->textPrefix6C;
    *optionTextCursor = 0;
    if (optionTextCursor[1] == 0) {
      *optionText = 0x73;
      Text_CopyNarrowToUtf16Cf(0x28,destination,optionText + 7);
      *optionTextCursor = 0x22;
      appliedOptionMask = appliedOptionMask + 2;
    }
  }
FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage:
  findOptionResult = (*g_CommandLineFindOption)(10,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x10);
  optionText = findOptionResult.ebx;
  if (((((!findOptionResult.carry) && (optionText[0xb] == 0x22)) &&
       (parsedCountOrTickSetting = optionText[10] - 0x30, 0x2f < optionText[10] && parsedCountOrTickSetting != 0)) &&
      ((optionText[0xc] == 0 && (parsedCountOrTickSetting < 8)))) && (parsedCountOrTickSetting != 0)) {
    *optionText = 0x6e;
    ((UiRangeSliderControl *)FRONTEND_UI(g_FrontendRootNode,networkSpeedSlider))->value =
         parsedCountOrTickSetting;
    appliedOptionMask = appliedOptionMask + 4;
    g_SessionNetworkTickInterval = parsedCountOrTickSetting * 2;
  }
  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(hostButton,-0x4920);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) | 0x2000;
  }
  g_FrontendNetworkState = 0;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
             ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,maxPlayersSlider))->value,
             (word *)&g_FrontendNetworkPlayerCountTextUtf16);
  parsedCountOrTickSetting = g_SessionNetworkTickInterval >> 1;
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,networkSpeedSlider))->value = parsedCountOrTickSetting;
  resolvedText = TextResource_Resolve(parsedCountOrTickSetting + 0x210d);
  RichTextCommandStream_CopyExpandedCf
            (0x40,(word *)&g_FrontendNetworkPlayerCountLabelUtf16,resolvedText.eax);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  FrontendNetworkSettings_SetGameName((UiTextEditControl *)FRONTEND_UI(frontendUi,gameNameEdit));
  if (appliedOptionMask == 7) {
    FrontendNetworkSetupPage_InitializeSingleLocalPlayer(FRONTEND_UI(frontendUi,hostGameCreateButton));
  }
  return;
}


/* Address: 0x0054CE80.
   Ownership: ui/frontend/network.
   Purpose: Initializes the frontend network-setup page for one local player, seeds the first 0x13B0-byte player
   block and endpoint data, loads the 64x64 player preview when available, and refreshes action 0x2006. Queued UI
   action handler for FRONTEND_PAGE20[4] (0x2004). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   PcxPreview_Load64x64PaletteAndPixelsCf [ui/support/runtime],
   FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction [ui/frontend/player].
*/
void __thandor_void_preserve_eax_ecx
FrontendNetworkSetupPage_InitializeSingleLocalPlayer(UiNodeBase *createButton)

{
  /* createButton is the frontend template's hostGameCreateButton (+0x4FF4). */
  FrontendUiImage *frontendUi;
  dword sequenceToken;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  int remainingDwords;
  undefined4 *localPlayerNameCursor;
  dword *localEndpointDwordCursor;
  dword *localPlayerRecordDwordCursor;
  bool previewLoadFailed;
  
  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(createButton,-0x4ff4);
  UiNodeList_SuppressActionId(0x200b,FRONTEND_UI(frontendUi,frontendRoot));
  UiPageStack_SetActiveIndex(3,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint) | 0x2000;
  }
  g_FrontendNetworkState = 2;
  UiPointerList_InitializeColumnLayout
            (1,(void **)g_FrontendPlayerRuntimeRecordPointers32,
             (UiPointerListControl *)FRONTEND_UI(frontendUi,hostLobbyPlayerList));
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(word *)&g_FrontendNetworkRuntimeCountTextUtf16);
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  sequenceToken = g_UiTransferSequenceToken;
  g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff;
  firstPlayerRecord->peerSequenceToken = sequenceToken;
  firstPlayerRecord->playerRuntimeId = 0;
  localPlayerNameCursor = (void *)g_FrontendLocalPlayerNameUtf16;
  localPlayerRecordDwordCursor = (dword *)&firstPlayerRecord->playerName;
  for (remainingDwords = 10; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
    *(undefined4 *)((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 = *localPlayerNameCursor
    ;
    localPlayerNameCursor = localPlayerNameCursor + 1;
    localPlayerRecordDwordCursor =
         (dword *)(((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 + 2);
  }
  localEndpointDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
  for (remainingDwords = 4; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
    *localPlayerRecordDwordCursor = *localEndpointDwordCursor;
    localEndpointDwordCursor = localEndpointDwordCursor + 1;
    localPlayerRecordDwordCursor = localPlayerRecordDwordCursor + 1;
  }
  *localPlayerRecordDwordCursor = 1;
  g_FrontendPendingSessionPlayerCount = 0;
  g_FrontendPlayerRuntimeCount = 1;
  g_LocalPlayerRuntimeId = 0;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_HOST;
  localPlayerRecordDwordCursor[6] = 0;
  previewLoadFailed = PcxPreview_Load64x64PaletteAndPixelsCf
                    ((PcxPreview64 *)(localPlayerRecordDwordCursor + 0x18),
                     (word *)(localPlayerRecordDwordCursor + -0xe));
  if (!previewLoadFailed) {
    localPlayerRecordDwordCursor[6] = 3;
  }
  localPlayerRecordDwordCursor[0x10] = 0;
  localPlayerRecordDwordCursor[0x11] = 0x6d0030;
  localPlayerRecordDwordCursor[0x12] = 0x73;
  localPlayerRecordDwordCursor[9] = 0x100;
  localPlayerRecordDwordCursor[10] = 0;
  localPlayerRecordDwordCursor[0xb] = 0;
  localPlayerRecordDwordCursor[10] = 0x440043;
  FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
  return;
}

