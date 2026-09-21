/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/scenario.c
 * Reverse engineering by idkFoxes 2026
 */

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

void __thandor_void_preserve_eax_ecx_edx
FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  FrontendNetworkListsRuntimeView5650 *frontendRoot;
  undefined4 uVar4;
  int iVar5;
  dword arg0;
  uint uVar6;
  ScenarioCatalogRecordCount SVar7;
  uint uVar8;
  MovieAdvanceFrameEaxCf5 MVar9;
  StatusValueEaxCf5 SVar10;
  TextResourceResolveEaxCf5 TVar11;
  UiSelectableGroupIndexEcxCf5 UVar12;
  word wVar13;
  
  uVar4 = g_FrontendNetworkState;
  frontendRoot = g_FrontendRootNode;
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)(g_FrontendRootNode->opaqueGap0000_4B67 + 0x350));
                    // WARNING: Switch is manually overridden
  switch(uVar4) {
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
    if ((*(int *)(frontendRoot->opaqueGap0000_4B67 + 0x904) != 0) &&
       (MVar9 = Movie_AdvanceFrame(), MVar9.carry)) {
      Movie_Rewind();
    }
    if (*(int *)(frontendRoot->opaqueGap0000_4B67 + 0x224) != 0) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick
                ((SoftwareMaskRuntimeView *)(frontendRoot->opaqueGap0000_4B67 + 0x1d4));
    }
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0xb0));
  }
  else {
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0xb0));
  }
  (*g_FrontendModelPointerContextUpdateCallback)
            (g_CursorOverrideY,g_CursorOverrideX,
             (FrontendModelPointerContextRuntimeState118 *)
             (frontendRoot->opaqueGap0000_4B67 + 0x368));
  iVar5 = (**(code **)(*(int *)(frontendRoot->opaqueGap0000_4B67 + 0xc) + 0x2c))
                    (g_CursorOverrideY,g_CursorOverrideX,frontendRoot);
  if (iVar5 == -1) {
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    arg0 = (**(code **)(*(int *)(iVar5 + 0xc) + 0x28))(g_CursorOverrideY,g_CursorOverrideX,iVar5);
    (*g_GraphicsCursorSetFrame)(arg0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = 0x100;
  }
  SVar10 = UiPageStack_ActivePageNotInListCf
                     ((UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0x508));
  if (SVar10.valueOrError == 10) {
    UVar12 = UiSelectableGroup_NoneSelectedCf(3);
    if (((!UVar12.carryNoneSelected) && (UVar12.selectedIndexOrCount == 1)) &&
       (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0)) {
      SVar7 = g_ScenarioCatalog->levelRecordCount;
      iVar5 = (int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset;
      if (SVar7 != 0) {
        uVar6 = 1;
        uVar8 = 0;
        do {
          wVar13 = 0x8001;
          pFVar2 = g_FrontendPlayerRuntimeBlocks;
          FVar1 = g_FrontendPlayerRuntimeBlockCount;
          do {
            FVar1 = FVar1 - 1;
            if (FVar1 == 0) {
              wVar13 = 0x8000;
              break;
            }
            pFVar3 = pFVar2 + 1;
            pFVar2 = pFVar2 + 1;
          } while ((*(uint *)(pFVar3->reserved78_7F + uVar8 * 4 + 0xc) & uVar6) != 0);
          TVar11 = TextResource_Resolve(*(int *)(iVar5 + 0x70) + 0x2230);
          *TVar11.eax = wVar13;
          iVar5 = iVar5 + 0x100;
          uVar6 = uVar6 * 2;
          if (uVar6 == 0) {
            uVar8 = uVar8 + 1;
            uVar6 = 1;
            if (2 < uVar8) {
              return;
            }
          }
          SVar7 = SVar7 - 1;
        } while (SVar7 != 0);
      }
    }
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
void __thandor_void_preserve_eax_ecx_edx
FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  sdword *psVar1;
  UiNodeBase **ppUVar2;
  UiNodeVtable **ppUVar3;
  UiAnchorFractionQ31 *control;
  UiTextResourceId UVar4;
  FrontendLoadedLevelRuntimeImage370 *pFVar5;
  dword dVar6;
  int iVar7;
  FrontendPlayerRuntimeBlockCount FVar8;
  int iVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  int iVar11;
  FactionRuntimeLifecycleObservedState *pFVar12;
  RichTextExtentRegs RVar13;
  TextResourceResolveEaxCf5 TVar14;
  TextResourceResolveEaxCf5 TVar15;
  MovieOpenEaxCf5 MVar16;
  MovieAdvanceFrameEaxCf5 MVar17;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    dVar6 = PersistentSettings_ReadDword(100,0x44);
    frontendRoot[0x1e].base.leftAnchorQ31 = dVar6;
  }
  UiPageStack_SetActiveIndex(0xc,(UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  FVar8 = g_FrontendPlayerRuntimeBlockCount;
  pFVar10 = g_FrontendPlayerRuntimeBlocks;
  if ((int)g_FramebufferWidth < 0x281) {
    psVar1 = &frontendRoot[10].base.layoutHeight;
    *psVar1 = *psVar1 | 0x2000;
    FVar8 = g_FrontendPlayerRuntimeBlockCount;
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    pFVar5 = g_FrontendLoadedLevelAsset;
    if (g_LocalPlayerRuntimeId == pFVar10->playerRuntimeId) break;
    pFVar10 = pFVar10 + 1;
    FVar8 = FVar8 - 1;
  } while (FVar8 != 0);
  UVar4 = (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex;
  frontendRoot[0x19].base.left =
       (pFVar10->factionAssignment).factionAssignmentIndex + 0x230017 +
       (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10;
  TVar14 = TextResource_Resolve(UVar4 + 0x2230);
  *TVar14.eax = 0x8000;
  TVar15 = TextResource_Resolve(0x219b);
  RichTextCommandStream_PatchPayloadBySelector(0,TVar14.eax,TVar15.eax);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,(pFVar5->header).titleTextResourceIndex,
             (word *)&g_FrontendMissionBriefingLevelDigitsUtf16);
  WidePath_SetExtensionCode(0x6d6c66,(word *)&g_FrontendMissionBriefingMoviePathUtf16);
  MVar16 = Movie_Open(0x80000000,(word *)&g_FrontendMissionBriefingMoviePathUtf16);
  if (MVar16.carry) {
    frontendRoot[0x1a].base.top = 0;
  }
  else {
    MVar17 = Movie_AdvanceFrame();
    frontendRoot[0x1a].base.top = MVar17.eax;
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
  FVar8 = g_FrontendPlayerRuntimeBlockCount;
  pFVar10 = g_FrontendPlayerRuntimeBlocks;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    psVar1 = &frontendRoot[0x15].base.right;
    *psVar1 = *psVar1 | 8;
    FVar8 = g_FrontendPlayerRuntimeBlockCount;
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    (pFVar10->factionAssignment).readyOrWaitState = 0;
    FVar8 = FVar8 - 1;
    pFVar10 = pFVar10 + 1;
  } while (FVar8 != 0);
  TVar14 = TextResource_Resolve(frontendRoot[0x19].base.left);
  RVar13 = RichTextCommandStream_MeasureWrappedBlockRegs
                     (g_UiTextStyleNormal,TVar14.eax,(UiPixelExtent)frontendRoot[0x19].base.vtable);
  frontendRoot[0x18].base.bottomAnchorQ31 = RVar13.widthPixels + 6;
  frontendRoot[0x18].base.layoutWidth = RVar13.heightPixels + 6;
  control = &frontendRoot[0x16].base.topAnchorQ31;
  UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)control);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)control);
  UiNodeList_UnsuppressActionId(0x204a,&frontendRoot->base);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(0x204a,&frontendRoot->base);
  }
  pFVar12 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  iVar9 = 7;
  iVar7 = 1;
  iVar11 = 0;
  do {
    pFVar12 = pFVar12 + 1;
    FVar8 = g_FrontendPlayerRuntimeBlockCount;
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
    if (*pFVar12 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        if (iVar7 == (pFVar10->factionAssignment).factionAssignmentIndex)
        goto FrontendMissionBriefing_AdvanceFactionAvailabilityScan;
        pFVar10 = pFVar10 + 1;
        FVar8 = FVar8 - 1;
      } while (FVar8 != 0);
      iVar11 = iVar11 + 1;
    }
FrontendMissionBriefing_AdvanceFactionAvailabilityScan:
    iVar7 = iVar7 + 1;
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) {
      if (((g_FrontendLoadedCampaignAsset == 0) ||
          (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) ==
           *(int *)(g_FrontendLoadedCampaignAsset + 0xb4))) && (iVar11 != 0)) {
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

