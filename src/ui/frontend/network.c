/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/network.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/network.h>

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
FrontendNetworkSetupPage_InitializeBackendMode(FrontendNetworkSettingsControlView250 *networkPage)

{
  byte bVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  UiListRowIndex returnValue;
  dword errorOrValue;
  int iVar3;
  word *destination;
  byte *commandLineOptionBytes;
  byte *secondaryCommandLineOptionBytes;
  UiTransferEndpointDescriptor *pUVar4;
  dword *localPlayerNameDwordCursor;
  dword *endpointSourceDwordCursor;
  dword *endpointDestinationDwordCursor;
  byte *pbVar5;
  dword *localPlayerRecordDwordCursor;
  bool bVar6;
  NetworkBackendSetSessionEaxCf5 NVar7;
  NetworkBackendOpenBindEaxCf5 NVar8;
  CommandLineFindOptionEbxCf5 CVar9;
  UiListRowIndex UVar10;
  dword dVar11;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    (*g_NetworkBackendSlot7)
              (&g_FrontendNetworkEndpointTextUtf16,
               (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
    UiNodeList_UnsuppressActionId(0x2001,(UiNodeBase *)&networkPage->commonState);
    UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&networkPage->commonState);
    UiNodeList_SuppressActionId(0x200b,(UiNodeBase *)&networkPage->commonState);
    UiPageStack_SetActiveIndex(3,(UiPageStackControl *)((int)networkPage + 0x508));
    if ((int)g_FramebufferWidth < 0x281) {
      *(uint *)((int)networkPage + 0x3b4) = *(uint *)((int)networkPage + 0x3b4) | 0x2000;
    }
    g_FrontendNetworkState = 2;
    UiPointerList_InitializeColumnLayout
              (1,(void **)&g_FrontendPlayerRuntimeRecordPointers32,
               (UiPointerListControl *)((int)networkPage + 0x55ec));
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(word *)&g_FrontendNetworkRuntimeCountTextUtf16
              );
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    dVar11 = g_UiTransferSequenceToken;
    g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff;
    pFVar2->peerSequenceToken = dVar11;
    localPlayerNameDwordCursor = &g_FrontendLocalPlayerNameUtf16;
    localPlayerRecordDwordCursor = (dword *)&pFVar2->playerName;
    for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(dword *)((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 =
           *localPlayerNameDwordCursor;
      localPlayerNameDwordCursor = localPlayerNameDwordCursor + 1;
      localPlayerRecordDwordCursor =
           (dword *)(((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 + 2);
    }
    endpointSourceDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
    for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
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
              (0x80,(word *)((int)networkPage + 0x4dc8),(word *)&g_FrontendNetworkEndpointTextUtf16)
    ;
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)((int)networkPage + 0x508));
    if ((int)g_FramebufferWidth < 0x281) {
      *(uint *)((int)networkPage + 0x3b4) = *(uint *)((int)networkPage + 0x3b4) | 0x2000;
    }
    g_FrontendNetworkState = 1;
    UiNodeList_UnsuppressActionId(0x2001,(UiNodeBase *)&networkPage->commonState);
    UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&networkPage->commonState);
    UiPointerList_InitializeColumnLayout
              (0,g_FrontendSessionListRows,(UiPointerListControl *)((int)networkPage + 0x4b68));
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_CLIENT;
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    (pFVar2->playerName).textUtf16[0] = 0;
    (pFVar2->playerName).textUtf16[1] = 0;
    pFVar2->playerRuntimeId = 0;
    (pFVar2->factionAssignment).roleStateFlags = 0;
    pFVar2->runtimeState64 = 0;
    pFVar2->snapshotTransferFlags = 0;
    UiTransfer_SendPlayerDescriptorPacket20002Cf();
    return;
  }
  UiRuntimeRecordRing_Clear();
  UiTransferMailbox_RandomizeSequenceToken();
  returnValue = UiPointerList_GetSelectedIndexVariantACf
                          ((UiPointerListControl *)((int)networkPage + 0x4a70));
  UVar10 = returnValue;
  NVar7 = (*g_NetworkBackendSlot0)(returnValue);
  if (!NVar7.carry) {
    NVar8 = (*g_NetworkBackendSlot2)(0x3a1);
    if (!NVar8.carry) {
FrontendNetworkSetup_CommitSelectedBackendAndInitializeClientPage:
      UiPointerList_SelectIndexVariantA
                (returnValue,(UiPointerListControl *)((int)networkPage + 0x4a70));
      pUVar4 = &g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (dword *)&g_FrontendNetworkEndpointScratch;
      for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
        *endpointDestinationDwordCursor = (dword)pUVar4->addressHeader;
        pUVar4 = (UiTransferEndpointDescriptor *)&pUVar4->ipv4AddressNetworkOrder;
        endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
      }
      CVar9 = (*g_CommandLineFindOption)(6,s_NAME__CLIENT__KARTE___00545e91);
      commandLineOptionBytes = CVar9.ebx;
      if (!CVar9.carry) {
        pbVar5 = commandLineOptionBytes + 6;
        iVar3 = 0x13;
        while( true ) {
          bVar1 = *pbVar5;
          if ((bVar1 == 0) || (bVar1 < 0x20))
          goto FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery;
          if (bVar1 == 0x22) break;
          iVar3 = iVar3 + -1;
          if (iVar3 == 0) goto FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery;
          pbVar5 = pbVar5 + 1;
        }
        destination = (word *)(g_FrontendRootNode + 0x4eb4);
        *pbVar5 = 0;
        if (pbVar5[1] == 0) {
          *commandLineOptionBytes = 0x6e;
          Text_CopyNarrowToUtf16Cf(0x28,destination,commandLineOptionBytes + 6);
          Text_CopyNarrowToUtf16Cf
                    (0x28,(word *)&g_FrontendLocalPlayerNameUtf16,commandLineOptionBytes + 6);
          *pbVar5 = 0x22;
        }
      }
FrontendNetworkSetup_ActivateClientBrowserAndPublishDiscovery:
      (*g_NetworkBackendSlot7)
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)((int)networkPage + 0x508));
      if ((int)g_FramebufferWidth < 0x281) {
        *(uint *)((int)networkPage + 0x3b4) = *(uint *)((int)networkPage + 0x3b4) | 0x2000;
      }
      g_FrontendNetworkState = 1;
      if ((short)g_FrontendLocalPlayerNameUtf16 == 0) {
        UiNodeList_SuppressActionId(0x2001,(UiNodeBase *)&networkPage->commonState);
      }
      else {
        UiNodeList_UnsuppressActionId(0x2001,(UiNodeBase *)&networkPage->commonState);
      }
      UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&networkPage->commonState);
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,(UiPointerListControl *)((int)networkPage + 0x4b68));
      UiTransfer_SendPacketType10000Value2931Cf();
      CVar9 = (*g_CommandLineFindOption)(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
      if (CVar9.carry) {
        CVar9 = (*g_CommandLineFindOption)(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
        secondaryCommandLineOptionBytes = CVar9.ebx;
        if (!CVar9.carry) {
          iVar3 = 0x7fffff;
          pbVar5 = secondaryCommandLineOptionBytes + 8;
          while( true ) {
            bVar1 = *pbVar5;
            if (bVar1 == 0) {
              return;
            }
            if (bVar1 < 0x20) {
              return;
            }
            if (bVar1 == 0x22) break;
            iVar3 = iVar3 + -1;
            if (iVar3 == 0) {
              return;
            }
            pbVar5 = pbVar5 + 1;
          }
          *pbVar5 = 0;
          if (pbVar5[1] == 0) {
            *secondaryCommandLineOptionBytes = 99;
            Text_CopyNarrowToUtf16Cf
                      (0x800000,(word *)g_PackageScratchBuffer,secondaryCommandLineOptionBytes + 8);
            bVar6 = (*g_NetworkBackendSlot6)
                              (&g_FrontendSelectedNetworkEndpoint,(char *)g_PackageScratchBuffer);
            if (!bVar6) {
              (*g_NetworkBackendSlot6)
                        (&g_FrontendNetworkEndpointScratch,(char *)g_PackageScratchBuffer);
              g_FrontendSessionToken = 0x12340000;
              g_FrontendSelectedPlayerToken = 0xffffffff;
              UiTransfer_SendPlayerDescriptorPacket20002Cf();
              iVar3 = g_FrontendRootNode;
              (*g_NetworkBackendSlot7)
                        (&g_FrontendNetworkEndpointTextUtf16,
                         (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
              RichTextCommandStream_CopyExpandedCf
                        (0x80,(word *)(iVar3 + 0x4dc8),(word *)&g_FrontendNetworkEndpointTextUtf16);
            }
          }
        }
      }
      else {
        *CVar9.ebx = 0x68;
        FrontendNetworkSetupPage_InitializeFromCommandLine
                  ((FrontendNetworkSettingsControlView250 *)((int)networkPage + 0x4920));
      }
      return;
    }
    (*g_NetworkBackendSlot1)(UVar10);
  }
  returnValue = 0;
  do {
    dVar11 = returnValue;
    NVar7 = (*g_NetworkBackendSlot0)(returnValue);
    errorOrValue = NVar7.eax;
    if (!NVar7.carry) {
      NVar8 = (*g_NetworkBackendSlot2)(0x3a1);
      errorOrValue = NVar8.eax;
      if (!NVar8.carry) goto FrontendNetworkSetup_CommitSelectedBackendAndInitializeClientPage;
      (*g_NetworkBackendSlot1)(dVar11);
    }
    returnValue = returnValue + 1;
    if (g_NetworkBackendInstanceCount <= returnValue) {
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
   Purpose: Handles frontend teardown save root state snapshot80.
*/
void __thandor_void_preserve_eax_ecx FrontendTeardown_SaveRootStateSnapshot80(UiRootNode *root)

{
  int dwordsRemaining;
  sdword *sourceCursor;
  sdword *destinationCursor;
  
  g_SharedWorldStatePointer = root[0xd0].base.topOffset;
  sourceCursor = &root[0xe2].base.right;
  destinationCursor = &g_FrontendRootStateSnapshot80;
  g_InGameWorldStatePointerMirror = g_SharedWorldStatePointer;
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
  bool bVar1;
  
  bVar1 = (*g_NetworkBackendSlot6)
                    (&g_FrontendNetworkEndpointScratch,(char *)transferPageControl->textPrefix6C);
  if (bVar1) {
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
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&source[-0xfc].layoutHeight);
  if ((int)g_FramebufferWidth < 0x281) {
    source[-0x100].leftOffset = source[-0x100].leftOffset | 0x2000;
  }
  g_FrontendNetworkState = 1;
  UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&source[-0x10d].nodeFlags);
  UiPointerList_InitializeColumnLayout
            (0,g_FrontendSessionListRows,(UiPointerListControl *)&source[-0xf].nodeFlags);
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
FrontendNetworkSetupPage_InitializeFromCommandLine
          (FrontendNetworkSettingsControlView250 *networkPage)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  word *destination;
  int iVar4;
  byte *pbVar5;
  byte *pbVar6;
  TextResourceResolveEaxCf5 TVar7;
  CommandLineFindOptionEbxCf5 CVar8;
  
  iVar4 = 0;
  CVar8 = (*g_CommandLineFindOption)(9,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72);
  pbVar5 = CVar8.ebx;
  if ((((!CVar8.carry) && (pbVar5[10] == 0x22)) &&
      (uVar2 = pbVar5[9] - 0x30, 0x2f < pbVar5[9] && uVar2 != 0)) &&
     (((pbVar5[0xb] == 0 && (uVar2 < 9)) && (1 < uVar2)))) {
    *pbVar5 = 0x73;
    iVar4 = 1;
    *(uint *)(g_FrontendRootNode + 0x5140) = uVar2;
  }
  CVar8 = (*g_CommandLineFindOption)(7,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 9);
  pbVar5 = CVar8.ebx;
  if (!CVar8.carry) {
    pbVar6 = pbVar5 + 7;
    iVar3 = 0x13;
    while( true ) {
      bVar1 = *pbVar6;
      if ((bVar1 == 0) || (bVar1 < 0x20))
      goto FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage;
      if (bVar1 == 0x22) break;
      iVar3 = iVar3 + -1;
      if (iVar3 == 0) goto FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage;
      pbVar6 = pbVar6 + 1;
    }
    destination = (word *)(g_FrontendRootNode + 0x50c0);
    *pbVar6 = 0;
    if (pbVar6[1] == 0) {
      *pbVar5 = 0x73;
      Text_CopyNarrowToUtf16Cf(0x28,destination,pbVar5 + 7);
      *pbVar6 = 0x22;
      iVar4 = iVar4 + 2;
    }
  }
FrontendNetworkSetup_ApplyCommandLinePlayerCountAndActivateHostPage:
  CVar8 = (*g_CommandLineFindOption)(10,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x10);
  pbVar5 = CVar8.ebx;
  if (((((!CVar8.carry) && (pbVar5[0xb] == 0x22)) &&
       (uVar2 = pbVar5[10] - 0x30, 0x2f < pbVar5[10] && uVar2 != 0)) &&
      ((pbVar5[0xc] == 0 && (uVar2 < 8)))) && (uVar2 != 0)) {
    *pbVar5 = 0x6e;
    *(uint *)(g_FrontendRootNode + 0x5204) = uVar2;
    iVar4 = iVar4 + 4;
    g_SessionNetworkTickInterval = uVar2 * 2;
  }
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)((int)networkPage + -0x4418));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)((int)networkPage + -0x456c) = *(uint *)((int)networkPage + -0x456c) | 0x2000;
  }
  g_FrontendNetworkState = 0;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,*(sdword *)((int)networkPage + 0x820),
             (word *)&g_FrontendNetworkPlayerCountTextUtf16);
  uVar2 = g_SessionNetworkTickInterval >> 1;
  *(uint *)((int)networkPage + 0x8e4) = uVar2;
  TVar7 = TextResource_Resolve(uVar2 + 0x210d);
  RichTextCommandStream_CopyExpandedCf
            (0x40,(word *)&g_FrontendNetworkPlayerCountLabelUtf16,TVar7.eax);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)((int)networkPage + 0x734));
  FrontendNetworkSettings_SetGameName((UiTextEditControl *)((int)networkPage + 0x734));
  if (iVar4 == 7) {
    FrontendNetworkSetupPage_InitializeSingleLocalPlayer
              ((FrontendNetworkSettingsControlView250 *)((int)networkPage + 0x6d4));
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
FrontendNetworkSetupPage_InitializeSingleLocalPlayer
          (FrontendNetworkSettingsControlView250 *networkPage)

{
  dword dVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  int iVar3;
  undefined4 *puVar4;
  dword *localEndpointDwordCursor;
  dword *localPlayerRecordDwordCursor;
  bool bVar5;
  
  UiNodeList_SuppressActionId(0x200b,(UiNodeBase *)((int)networkPage + -0x4ff4));
  UiPageStack_SetActiveIndex(3,(UiPageStackControl *)((int)networkPage + -0x4aec));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)((int)networkPage + -0x4c40) = *(uint *)((int)networkPage + -0x4c40) | 0x2000;
  }
  g_FrontendNetworkState = 2;
  UiPointerList_InitializeColumnLayout
            (1,(void **)&g_FrontendPlayerRuntimeRecordPointers32,
             (UiPointerListControl *)((int)networkPage + 0x5f8));
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,1,(word *)&g_FrontendNetworkRuntimeCountTextUtf16);
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  dVar1 = g_UiTransferSequenceToken;
  g_FrontendPlayerRuntimeBlocks->heartbeatExpiryTicks = 0xffffffff;
  pFVar2->peerSequenceToken = dVar1;
  pFVar2->playerRuntimeId = 0;
  puVar4 = &g_FrontendLocalPlayerNameUtf16;
  localPlayerRecordDwordCursor = (dword *)&pFVar2->playerName;
  for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined4 *)((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 = *puVar4
    ;
    puVar4 = puVar4 + 1;
    localPlayerRecordDwordCursor =
         (dword *)(((FrontendPlayerNameUtf16_28 *)localPlayerRecordDwordCursor)->textUtf16 + 2);
  }
  localEndpointDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
  for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
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
  bVar5 = PcxPreview_Load64x64PaletteAndPixelsCf
                    ((PcxPreview64 *)(localPlayerRecordDwordCursor + 0x18),
                     (word *)(localPlayerRecordDwordCursor + -0xe));
  if (!bVar5) {
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

