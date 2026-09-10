#include <thandor/ui/frontend/scenario.h>

/* Implementation ownership: ui/frontend/scenario. */

/* Address: 0x00547D60.
   Ownership: ui/frontend/scenario.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Body boundaries remain exact and are not split into speculative ABI functions. Frontend root callback contract:
   one UiRootNode* callback-context argument at stack +4, callee cleanup 4, void return. EAX and EDX are
   preserved/incidental state, not a semantic qword result.
   Cross-module calls: RecentTextHistory_SortAndBuildPointerList [ui/support/runtime],
   FrontendSessionList_DecrementExpiryAndCompactRows [ui/frontend/session],
   FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks [ui/frontend/player],
   FrontendTransfer_TickRequestTimeoutAndResetPage [network/protocol/transfer],
   FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers [ui/frontend/player],
   FrontendNetwork_TickDisconnectTimeoutAndResetSession [network/backend/runtime].
*/
void FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  UiRootNode *frontendRoot;
  UiNodeBase *control;
  GraphicsCursorFrameIndex arg0;
  uint extraout_EAX;
  uint uVar4;
  word *pwVar5;
  word extraout_CX;
  int extraout_ECX;
  undefined4 extraout_EDX;
  int extraout_EDX_00;
  uint uVar6;
  int iVar7;
  bool bVar8;
  
  frontendRoot = g_FrontendRootNode;
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&g_FrontendRootNode[9].base.rightAnchorQ31);
                    
  switch(extraout_EDX) {
  case 1:
    FrontendSessionList_DecrementExpiryAndCompactRows(frontendRoot);
    break;
  case 2:
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(frontendRoot);
    break;
  case 3:
    FrontendTransfer_TickRequestTimeoutAndResetPage(frontendRoot);
    break;
  case 4:
    FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers();
    break;
  case 5:
    FrontendNetwork_TickDisconnectTimeoutAndResetSession();
  }
  if ((g_FrontendRuntimeFlags & 0x10) == 0) {
    bVar8 = false;
    if ((frontendRoot[0x1a].base.top != 0) && (Movie_AdvanceFrame(), bVar8)) {
      Movie_Rewind();
    }
    if (frontendRoot[6].base.top != 0) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick
                ((SoftwareMaskRuntimeView *)&frontendRoot[5].base.bottom);
    }
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRoot + 2));
  }
  else {
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)(frontendRoot + 2));
  }
  (*g_FrontendModelPointerContextUpdateCallback)
            (g_CursorOverrideY,g_CursorOverrideX,
             (FrontendModelPointerContextRuntimeState118 *)&frontendRoot[9].callbacks);
  control = (*((frontendRoot->base).vtable)->hitTest)
                      (g_CursorOverrideY,g_CursorOverrideX,&frontendRoot->base);
  if (control == (UiNodeBase *)0xffffffff) {
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    arg0 = (*control->vtable->pointerMove)(g_CursorOverrideY,g_CursorOverrideX,control);
    (*g_GraphicsCursorSetFrame)(arg0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = 0x100;
  }
  UiPageStack_ActivePageNotInListCf((UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  bVar8 = extraout_EAX < 10;
  if ((((extraout_EAX == 10) && (UiSelectableGroup_NoneSelectedCf(3), !bVar8)) &&
      (extraout_ECX == 1)) &&
     ((g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0 &&
      (iVar7 = (int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset,
      g_ScenarioCatalog->levelRecordCount != 0)))) {
    uVar4 = 1;
    uVar6 = 0;
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    FVar1 = g_FrontendPlayerRuntimeBlockCount;
joined_r0x00547f48:
    do {
      FVar1 = FVar1 - 1;
      if (FVar1 != 0) {
        pFVar3 = pFVar2 + 1;
        pFVar2 = pFVar2 + 1;
        if ((*(uint *)(pFVar3->reserved78_7F + uVar6 * 4 + 0xc) & uVar4) != 0)
        goto joined_r0x00547f48;
      }
      pwVar5 = TextResource_Resolve(*(int *)(iVar7 + 0x70) + 0x2230);
      *pwVar5 = extraout_CX;
      iVar7 = iVar7 + 0x100;
      uVar4 = uVar4 * 2;
      if (uVar4 == 0) {
        uVar6 = uVar6 + 1;
        uVar4 = 1;
        if (2 < uVar6) {
          return;
        }
      }
      pFVar2 = g_FrontendPlayerRuntimeBlocks;
      FVar1 = g_FrontendPlayerRuntimeBlockCount;
    } while (extraout_EDX_00 != 1);
  }
  return;
}

/* Address: 0x0054C9F0.
   Ownership: ui/frontend/scenario.
   Purpose: Activates mission page 12, resolves the current player/faction briefing, patches localized mission-
   description template 0x219B, constructs the FLM path, opens the movie, and updates page actions.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], WidePath_SetExtensionCode [core/text/path], Movie_Open [movie/runtime/playback].
*/
void FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  sdword *psVar1;
  UiNodeBase **ppUVar2;
  UiNodeVtable **ppUVar3;
  dword dVar4;
  uint uVar5;
  word *pwVar6;
  MovieRuntime *pMVar7;
  int iVar8;
  FrontendPlayerRuntimeBlockCount FVar9;
  uint uVar10;
  int iVar11;
  void *replacementPayload;
  FrontendPlayerRuntimeRecord *pFVar12;
  int iVar13;
  FactionRuntimeLifecycleObservedState *pFVar14;
  undefined1 uVar15;
  RichTextExtentRegs RVar16;
  undefined8 uVar17;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    dVar4 = PersistentSettings_ReadDword(100,0x44);
    frontendRoot[0x1e].base.leftAnchorQ31 = dVar4;
  }
  UiPageStack_SetActiveIndex(0xc,(UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  FVar9 = g_FrontendPlayerRuntimeBlockCount;
  pFVar12 = g_FrontendPlayerRuntimeBlocks;
  if ((int)g_FramebufferWidth < 0x281) {
    psVar1 = &frontendRoot[10].base.layoutHeight;
    *psVar1 = *psVar1 | 0x2000;
    FVar9 = g_FrontendPlayerRuntimeBlockCount;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    iVar8 = g_FrontendLoadedLevelAsset;
    if (g_LocalPlayerRuntimeId == pFVar12->playerRuntimeId) break;
    pFVar12 = pFVar12 + 1;
    FVar9 = FVar9 - 1;
  } while (FVar9 != 0);
  uVar5 = (pFVar12->factionAssignment).factionAssignmentIndex + 0x230017;
  uVar10 = *(int *)(g_FrontendLoadedLevelAsset + 0x170) * 0x10;
  iVar11 = *(int *)(g_FrontendLoadedLevelAsset + 0x170);
  uVar15 = CARRY4(uVar5,uVar10);
  frontendRoot[0x19].base.left = uVar5 + uVar10;
  pwVar6 = TextResource_Resolve(iVar11 + 0x2230);
  *pwVar6 = 0x8000;
  pwVar6 = TextResource_Resolve(0x219b);
  RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,pwVar6);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,*(sdword *)(iVar8 + 0x170),
             (word *)&g_FrontendMissionBriefingLevelDigitsUtf16);
  WidePath_SetExtensionCode(0x6d6c66,(word *)&g_FrontendMissionBriefingMoviePathUtf16);
  Movie_Open(0x80000000,(word *)&g_FrontendMissionBriefingMoviePathUtf16);
  if ((bool)uVar15) {
    frontendRoot[0x1a].base.top = 0;
  }
  else {
    pMVar7 = Movie_AdvanceFrame();
    frontendRoot[0x1a].base.top = (sdword)pMVar7;
    frontendRoot[0x1a].base.right = 0;
  }
  if ((((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
      (g_FrontendLoadedCampaignAsset == 0)) && (g_FrontendScenarioInitializationCount == 0)) {
    frontendRoot[0x12].base.nextSibling =
         (UiNodeBase *)((uint)frontendRoot[0x12].base.nextSibling & 0xfffffff7);
    ppUVar2 = &frontendRoot[0x12].base.firstChild;
    *ppUVar2 = (UiNodeBase *)((uint)*ppUVar2 & 0xfffffbff);
  }
  else {
    frontendRoot[0x12].base.nextSibling =
         (UiNodeBase *)((uint)frontendRoot[0x12].base.nextSibling | 8);
    ppUVar2 = &frontendRoot[0x12].base.firstChild;
    *ppUVar2 = (UiNodeBase *)((uint)*ppUVar2 | 0x400);
    if ((g_FrontendScenarioInitializationCount != 0) || (g_FrontendLoadedCampaignAsset != 0)) {
      ppUVar2 = &frontendRoot[0x13].base.parent;
      *ppUVar2 = (UiNodeBase *)((uint)*ppUVar2 & 0xfffffff7);
      ppUVar3 = &frontendRoot[0x13].base.vtable;
      *ppUVar3 = (UiNodeVtable *)((uint)*ppUVar3 & 0xfffffbff);
      psVar1 = &frontendRoot[0x14].base.left;
      *psVar1 = *psVar1 & 0xfffffff7;
      psVar1 = &frontendRoot[0x14].base.top;
      *psVar1 = *psVar1 & 0xfffffbff;
      psVar1 = &frontendRoot[0x14].base.left;
      *psVar1 = *psVar1 | 8;
      psVar1 = &frontendRoot[0x14].base.top;
      *psVar1 = *psVar1 | 0x400;
      goto FrontendMissionBriefing_InitializePlayerReadinessAndLayout;
    }
  }
  ppUVar2 = &frontendRoot[0x13].base.parent;
  *ppUVar2 = (UiNodeBase *)((uint)*ppUVar2 | 8);
  ppUVar3 = &frontendRoot[0x13].base.vtable;
  *ppUVar3 = (UiNodeVtable *)((uint)*ppUVar3 | 0x400);
  psVar1 = &frontendRoot[0x14].base.left;
  *psVar1 = *psVar1 | 8;
  psVar1 = &frontendRoot[0x14].base.top;
  *psVar1 = *psVar1 | 0x400;
FrontendMissionBriefing_InitializePlayerReadinessAndLayout:
  psVar1 = &frontendRoot[0x15].base.right;
  *psVar1 = *psVar1 & 0xfffffff7;
  FVar9 = g_FrontendPlayerRuntimeBlockCount;
  pFVar12 = g_FrontendPlayerRuntimeBlocks;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    psVar1 = &frontendRoot[0x15].base.right;
    *psVar1 = *psVar1 | 8;
    FVar9 = g_FrontendPlayerRuntimeBlockCount;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    (pFVar12->factionAssignment).readyOrWaitState = 0;
    FVar9 = FVar9 - 1;
    pFVar12 = pFVar12 + 1;
  } while (FVar9 != 0);
  pwVar6 = TextResource_Resolve(frontendRoot[0x19].base.left);
  RVar16 = RichTextCommandStream_MeasureWrappedBlockRegs
                     (g_UiTextStyleNormal,pwVar6,(UiPixelExtent)frontendRoot[0x19].base.vtable);
  frontendRoot[0x18].base.bottomAnchorQ31 = RVar16.widthPixels + 6;
  frontendRoot[0x18].base.layoutWidth = RVar16.heightPixels + 6;
  uVar17 = UiScrollableControl_RebuildViewportAndScrollbars
                     ((UiScrollableControl *)&frontendRoot[0x16].base.topAnchorQ31);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar17);
  UiNodeList_UnsuppressActionId(0x204a,&frontendRoot->base);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(0x204a,&frontendRoot->base);
  }
  pFVar14 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  iVar11 = 7;
  iVar8 = 1;
  iVar13 = 0;
  do {
    pFVar14 = pFVar14 + 1;
    FVar9 = g_FrontendPlayerRuntimeBlockCount;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
    if (*pFVar14 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        if (iVar8 == (pFVar12->factionAssignment).factionAssignmentIndex)
        goto FrontendMissionBriefing_AdvanceFactionAvailabilityScan;
        pFVar12 = pFVar12 + 1;
        FVar9 = FVar9 - 1;
      } while (FVar9 != 0);
      iVar13 = iVar13 + 1;
    }
FrontendMissionBriefing_AdvanceFactionAvailabilityScan:
    iVar8 = iVar8 + 1;
    iVar11 = iVar11 + -1;
    if (iVar11 == 0) {
      if (((g_FrontendLoadedCampaignAsset == 0) ||
          (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) ==
           *(int *)(g_FrontendLoadedCampaignAsset + 0xb4))) && (iVar13 != 0)) {
        psVar1 = &frontendRoot[0x1b].base.top;
        *psVar1 = *psVar1 & 0xfffffff7;
      }
      else {
        psVar1 = &frontendRoot[0x1b].base.top;
        *psVar1 = *psVar1 | 8;
        UiNodeList_SuppressActionId(0x204a,&frontendRoot->base);
      }
      return;
    }
  } while( true );
}
