/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/settings.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/settings.h>

/* Implementation ownership: ui/frontend/settings. */

/* Address: 0x00549250.
   Ownership: ui/frontend/settings.
   Purpose: Activates task page 11, configures seven faction/player rows, selects the local player row, updates
   responsive visibility, and patches localized task-description template 0x218C.
   Local calls: FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/

void __thandor_void_preserve_eax_ecx_edx
FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView26C4 *frontendRootPage)

{
  UiNodeFlags *pUVar1;
  dword dVar2;
  UiNodeVtable *pUVar3;
  int iVar4;
  FrontendLoadedLevelRuntimeImage370 *pFVar5;
  dword dVar6;
  uint uVar7;
  uint uVar8;
  byte *pbVar9;
  dword dVar10;
  FrontendPlayerRuntimeRecord *pFVar11;
  TextResourceResolveEaxCf5 TVar12;
  TextResourceResolveEaxCf5 TVar13;
  FrontendPlayerRuntimeBlockCount FVar14;
  
  UiPageStack_SetActiveIndex(0xb,&frontendRootPage->primaryPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    pUVar1 = &(frontendRootPage->compactLayoutControl).nodeFlags;
    *pUVar1 = *pUVar1 | 0x2000;
  }
  pFVar5 = g_FrontendLoadedLevelAsset;
  dVar6 = (g_FrontendLoadedLevelAsset->runtimeTail2E0).activeFactionCount;
  dVar10 = (g_FrontendLoadedLevelAsset->runtimeTail2E0).assignableFactionCount;
  uVar7 = 0;
  do {
    dVar2 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar7 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar2) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar2) & 0xfffffbff;
    pbVar9 = frontendRootPage->taskRowControls04C + dVar2 + 8;
    pbVar9[0] = 0x9a;
    pbVar9[1] = 0x21;
    pbVar9[2] = 0;
    pbVar9[3] = 0;
    dVar2 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar7 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar2) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar2) & 0xfffffbff;
    dVar2 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar7 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar2) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar2) & 0xfffffbfd;
    dVar2 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar7 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar2) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar2) & 0xffffffbf;
    dVar2 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar7 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar2 - 4)) & 0xfffffff7;
    uVar8 = uVar7 + 1;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar2) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar2) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar7 + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    dVar6 = dVar6 - 1;
    dVar10 = dVar10 - 1;
    uVar7 = uVar8;
  } while (dVar10 != 0);
  for (; dVar6 != 0; dVar6 = dVar6 - 1) {
    dVar10 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) & 0xfffffbff;
    pbVar9 = frontendRootPage->taskRowControls04C + dVar10 + 8;
    pbVar9[0] = 0x9a;
    pbVar9[1] = 0x21;
    pbVar9[2] = 0;
    pbVar9[3] = 0;
    dVar10 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) & 0xfffffbff;
    dVar10 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) & 0xfffffffd;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) | 0x400;
    dVar10 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) & 0xffffffbf;
    dVar10 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar10 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar10) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar10) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar8 + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    uVar8 = uVar8 + 1;
  }
  for (; uVar8 < 7; uVar8 = uVar8 + 1) {
    dVar6 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) | 0x400;
    pbVar9 = frontendRootPage->taskRowControls04C + dVar6 + 8;
    pbVar9[0] = 0x99;
    pbVar9[1] = 0x21;
    pbVar9[2] = 0;
    pbVar9[3] = 0;
    dVar6 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) | 0x400;
    dVar6 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) & 0xfffffffd;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) | 0x400;
    dVar6 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) | 0x40;
    dVar6 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar8 + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (dVar6 - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) | 0x40;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar8 + 1] = 0;
  }
  do {
    dVar6 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar8];
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6 + 8) =
         *(int *)((int)&pFVar5->playerSlots[0].aiClassOrMode +
                 g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[uVar8 - 1]) + 0x2173 + uVar8;
    *(uint *)(frontendRootPage->taskRowControls04C + dVar6) =
         *(uint *)(frontendRootPage->taskRowControls04C + dVar6) & 0xfffffffd;
    iVar4 = g_LocalPlayerRuntimeId;
    uVar8 = uVar8 - 1;
  } while (uVar8 != 0);
  uVar7 = 1;
  dVar6 = 0;
  pFVar11 = g_FrontendPlayerRuntimeBlocks;
  FVar14 = g_FrontendPlayerRuntimeBlockCount;
  do {
    dVar10 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar7];
    (pFVar11->factionAssignment).factionAssignmentIndex = uVar7;
    pbVar9 = frontendRootPage->taskRowControls04C + (dVar10 - 0x4c);
    (pFVar11->factionAssignment).readyOrWaitState = 0;
    (pFVar11->factionAssignment).consensusValue = 0;
    pbVar9[0x54] = 0x98;
    pbVar9[0x55] = 0x21;
    pbVar9[0x56] = 0;
    pbVar9[0x57] = 0;
    if (iVar4 == pFVar11->playerRuntimeId) {
      dVar6 = uVar7 - 1;
    }
    uVar7 = uVar7 + 1;
    pFVar11 = pFVar11 + 1;
    if ((pFVar5->runtimeTail2E0).assignableFactionCount < uVar7) {
      uVar7 = uVar7 - (pFVar5->runtimeTail2E0).assignableFactionCount;
    }
    FVar14 = FVar14 - 1;
  } while (FVar14 != 0);
  *(uint *)(frontendRootPage->taskRowControls04C +
           g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar6 + 1]) =
       *(uint *)(frontendRootPage->taskRowControls04C +
                g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar6 + 1]) | 2;
  *(uint *)(frontendRootPage->taskRowControls04C +
           (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar6 + 1] - 4)) =
       *(uint *)(frontendRootPage->taskRowControls04C +
                (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar6 + 1] - 4)) &
       0xfffffff7;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls
            ((dword)pbVar9,dVar6,(UiRootNode *)frontendRootPage);
  pUVar3 = (frontendRootPage->rootNode).vtable;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    frontendRootPage->taskPageControlState55C[0x1cc] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] | 8;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x176] =
         frontendRootPage->taskPageControlState55C[0x176] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x177] =
         frontendRootPage->taskPageControlState55C[0x177] & 0xfffffbff;
  }
  else {
    frontendRootPage->taskPageControlState55C[0x1cc] = 0;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x1bf] =
         frontendRootPage->taskPageControlState55C[0x1bf] & 0xfffffffd;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] | 8;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] | 0x400;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] | 8;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] | 0x400;
    }
    else {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] & 0xfffffbff;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] & 0xfffffff7;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] & 0xfffffbff;
    }
  }
  pFVar5 = g_FrontendLoadedLevelAsset;
  (*pUVar3->layout)(&frontendRootPage->rootNode);
  TVar12 = TextResource_Resolve((pFVar5->header).titleTextResourceIndex + 0x2230);
  *TVar12.eax = 0x8000;
  TVar13 = TextResource_Resolve(0x218c);
  RichTextCommandStream_PatchPayloadBySelector(0,TVar12.eax,TVar13.eax);
  return;
}


/* Address: 0x0054BA90.
   Ownership: ui/frontend/settings.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[34]@00545938;
   g_UiActionPage20InitializedHandlers[35]@00545938; g_UiActionPage20InitializedHandlers[36]@00545938;
   g_UiActionPage20InitializedHandlers[37]@00545938; g_UiActionPage20InitializedHandlers[38]@00545938;
   g_UiActionPage20InitializedHandlers[39]@00545938; g_UiActionPage20InitializedHandlers[40]@00545938;
   g_UiActionPage20InitializedHandlers[41]@00545938; g_UiActionPage20InitializedHandlers[42]@00545938;
   g_UiActionPage20InitializedHandlers[43]@00545938. Queued UI action handler for FRONTEND_PAGE20[34],FRONTEND_PAGE
   20[35],FRONTEND_PAGE20[36],FRONTEND_PAGE20[37],FRONTEND_PAGE20[38],FRONTEND_PAGE20[39],FRONTEND_PAGE20[40],FRONT
   END_PAGE20[41],FRONTEND_PAGE20[42],FRONTEND_PAGE20[43]
   (0x2022,0x2023,0x2024,0x2025,0x2026,0x2027,0x2028,0x2029,0x202A,0x202B). Return datatype is preserved for non-
   queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
*/
void __thandor_preserve_eax_edx
FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *displaySettingsRoot)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       displaySettingsRoot[1].top;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       displaySettingsRoot[1].right;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
  return;
}


/* Address: 0x0054BAC0.
   Ownership: ui/frontend/settings.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[30]@00545938;
   g_UiActionPage20InitializedHandlers[31]@00545938; g_UiActionPage20InitializedHandlers[32]@00545938;
   g_UiActionPage20InitializedHandlers[33]@00545938. Queued UI action handler for
   FRONTEND_PAGE20[30],FRONTEND_PAGE20[31],FRONTEND_PAGE20[32],FRONTEND_PAGE20[33] (0x201E,0x201F,0x2020,0x2021).
   Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
*/
void __thandor_preserve_eax
FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *displaySettingsRoot)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = displaySettingsRoot[1].top;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
  return;
}


/* Address: 0x0054BAF0.
   Ownership: ui/frontend/settings.
   Purpose: Attempts the selected display mode, persists adapter/width/height/bits-per-pixel at offsets 0x00
   through 0x0C on success, and restores the prior mode on failure. Queued UI action handler for
   FRONTEND_PAGE20[49] (0x2031). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], PersistentSettings_WriteDword
   [core/settings/persistent], UiRootStack_Relayout [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendDisplaySettings_ApplyMode(void *control)

{
  dword arg3;
  dword arg2;
  dword arg0;
  undefined4 value;
  undefined4 value_00;
  undefined4 value_01;
  undefined4 value_02;
  int iVar1;
  GraphicsTextureSourceAsset **ppGVar2;
  DisplayModeEaxCf5 DVar3;
  DisplayModeEaxCf5 DVar4;
  
  value_02 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             bitsPerPixel;
  value_01 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             height;
  value_00 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             width;
  value = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
          adapterIndex;
  arg0 = g_ActiveGraphicsAdapterIndex;
  arg2 = g_FramebufferHeight;
  arg3 = g_FramebufferWidth;
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  iVar1 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
          g_SoftwarePixelFormatConfig.blueBitCount;
  DVar3 = (*g_GraphicsDisplayModeHook)
                    (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.bitsPerPixel,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width);
  if (DVar3.carry) {
    DVar4 = (*g_GraphicsDisplayModeHook)(arg0,iVar1 + 0xfU & 0xfffffff0,arg2,arg3);
    (*g_FatalErrorPrimaryDispatchCf)(DVar4.eax,DVar4.carry);
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    (*g_FatalErrorRuntimeDispatchCf)(DVar3.eax,true);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    adapterIndex = PersistentSettings_ReadDword(1,0);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
         PersistentSettings_ReadDword(0x280,4);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
         PersistentSettings_ReadDword(0x1e0,8);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    bitsPerPixel = PersistentSettings_ReadDword(0x10,0xc);
    FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
    return;
  }
  PersistentSettings_WriteDword(value,0);
  PersistentSettings_WriteDword(value_00,4);
  PersistentSettings_WriteDword(value_01,8);
  PersistentSettings_WriteDword(value_02,0xc);
  UiRootStack_Relayout();
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_FrontendMenuTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowClassTextureSource);
  ppGVar2 = g_FontTextureSources;
  iVar1 = 2;
  do {
    (*g_GraphicsTextureSourceConvertPaletteEntries)((GraphicsPaletteTextureSourceAsset *)*ppGVar2);
    ppGVar2 = ppGVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
  iVar1 = *(int *)((int)control + 8);
  while (iVar1 != -1) {
    control = *(void **)((int)control + 8);
    iVar1 = *(int *)((int)control + 8);
  }
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)((int)control + 0x3b4) = *(uint *)((int)control + 0x3b4) | 0x2000;
  }
  else {
    *(uint *)((int)control + 0x3b4) = *(uint *)((int)control + 0x3b4) & 0xffffdfff;
  }
  return;
}


/* Address: 0x0054CD70.
   Ownership: ui/frontend/settings.
   Purpose: When the player-name edit is accepted, writes 0x28 bytes to settings offset 0x60 and mirrors the same
   twenty UTF-16 code units to the active player-name buffer. Queued UI action handler for FRONTEND_PAGE20[50]
   (0x2032). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish.
   Cross-module calls: UiTextControl_UpdateNonEmptyValidity [ui/controls/text], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDwords
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx
FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control)

{
  UiNodeBase *pUVar1;
  UiTextEditControl *firstNode;
  int iVar2;
  dword *sourceDwordCursor;
  dword *pdVar3;
  
  pUVar1 = (control->base).parent;
  firstNode = control;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    firstNode = (UiTextEditControl *)(firstNode->base).parent;
    pUVar1 = (firstNode->base).parent;
  }
  UiTextControl_UpdateNonEmptyValidity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(0x2001,&firstNode->base);
    UiNodeList_SuppressActionId(0x2002,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2001,&firstNode->base);
    FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
              ((FrontendNetworkSettingsControlView250 *)&firstNode[0x96].activationSound);
    PersistentSettings_WriteDwords(0x28,(dword *)control->textPrefix6C,0x60);
    sourceDwordCursor = (dword *)control->textPrefix6C;
    pdVar3 = &g_FrontendLocalPlayerNameUtf16;
    for (iVar2 = 10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *pdVar3 = *sourceDwordCursor;
      sourceDwordCursor = sourceDwordCursor + 1;
      pdVar3 = pdVar3 + 1;
    }
  }
  return;
}


/* Address: 0x00548E70.
   Ownership: ui/frontend/settings.
   Purpose: Applies control+0x58 to the frontend game-speed service and persists gameSpeedPercent at settings
   offset 0x44. Queued UI action handler for FRONTEND_PAGE20[74] (0x204A). Return datatype is preserved for non-
   queue direct callers.
   Cross-module calls: FrontendSession_SetGameSpeedPercent [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_SetGameSpeedPercent(g_LocalPlayerRuntimeId,0,0,control->boundValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x300,0,0,control->boundValue);
  }
  PersistentSettings_WriteDword(control->boundValue,0x44);
  return;
}


/* Address: 0x0054A810.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x2049. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[73] (0x2049). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 4;
  }
  else {
    value = dVar1 & 0xfffffffb;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0054A850.
   Ownership: ui/frontend/settings.
   Purpose: Persists control+0x58 as cameraScrollStep at settings offset 0x48. Queued UI action handler for
   FRONTEND_PAGE20[75] (0x204B). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentSettings_WriteDword(control->boundValue,0x48);
  return;
}


/* Address: 0x0054A870.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203C. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[60] (0x203C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 1;
  }
  else {
    value = dVar1 & 0xfffffffe;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0054A8B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203D. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[61] (0x203D). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 2;
  }
  else {
    value = dVar1 & 0xfffffffd;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0054A8F0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203E. Updates bit 0 of optionFlags5C. Selecting it suppresses the paired
   action 0x203F; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[62] (0x203E).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x5c);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 1;
    UiNodeList_SuppressActionId(0x203f,(control->base).parent);
  }
  else {
    value = dVar1 & 0xfffffffe;
    UiNodeList_UnsuppressActionId(0x203f,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A950.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203F. Updates bit 1 of optionFlags5C. Selecting it suppresses the paired
   action 0x203E; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[63] (0x203F).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x5c);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 2;
    UiNodeList_SuppressActionId(0x203e,(control->base).parent);
  }
  else {
    value = dVar1 & 0xfffffffd;
    UiNodeList_UnsuppressActionId(0x203e,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A9B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x2051. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[81] (0x2051). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x5c);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 4;
  }
  else {
    value = dVar1 & 0xfffffffb;
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A9F0.
   Ownership: ui/frontend/settings.
   Purpose: Selects gameplay-settings page five, applies compact layout, restores optionFlags40 and optionFlags5C
   controls with mutual-exclusion suppression, and restores the camera-scroll setting.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  sdword *psVar1;
  dword dVar2;
  
  UiPageStack_SetActiveIndex(5,(UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  if ((int)g_FramebufferWidth < 0x281) {
    psVar1 = &frontendRoot[10].base.layoutHeight;
    *psVar1 = *psVar1 | 0x2000;
  }
  dVar2 = PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_SetSelected
            (dVar2 & 1,(UiSelectableControl *)&frontendRoot[0x7b].base.bottomAnchorQ31);
  UiSelectableControl_SetSelected
            (dVar2 & 2,(UiSelectableControl *)&frontendRoot[0x7c].base.layoutHeight);
  UiSelectableControl_SetSelected(dVar2 & 4,(UiSelectableControl *)&frontendRoot[0x75].base.bottom);
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  if ((dVar2 & 1) != 0) {
    UiNodeList_SuppressActionId(0x203f,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected
            (dVar2 & 1,(UiSelectableControl *)&frontendRoot[0x7e].base.nodeFlags);
  if ((dVar2 & 2) != 0) {
    UiNodeList_SuppressActionId(0x203e,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected(dVar2 & 2,(UiSelectableControl *)&frontendRoot[0x7f].callbacks);
  dVar2 = PersistentSettings_ReadDword(0x20,0x48);
  frontendRoot[0x7a].base.leftAnchorQ31 = dVar2;
  return;
}


/* Address: 0x0054B740.
   Ownership: ui/frontend/settings.
   Purpose: Recovered action-table target FRONTEND_PAGE20[18] (0x2012).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState167C *source)

{
  FrontendGraphicsRuntimeSettingsPageState167C *firstNode;
  dword dVar1;
  dword dVar2;
  int iVar3;
  UiNodeBase *pUVar4;
  
  UiPageStack_SetActiveIndex(7,(UiPageStackControl *)(source[-2].reserved4C_1067 + 0xa20));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)(source[-2].reserved4C_1067 + 0x8cc) =
         *(uint *)(source[-2].reserved4C_1067 + 0x8cc) | 0x2000;
  }
  dVar1 = PersistentSettings_ReadDword(1,0x1c);
  UiSelectableControl_SetSelected(dVar1,&source->shadingEnabledControl);
  pUVar4 = (source->base).parent;
  firstNode = source;
  while (pUVar4 != (UiNodeBase *)0xffffffff) {
    firstNode = (FrontendGraphicsRuntimeSettingsPageState167C *)(firstNode->base).parent;
    pUVar4 = (firstNode->base).parent;
  }
  if (dVar1 == 0) {
    UiNodeList_SuppressActionId(0x2015,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&firstNode->base);
  }
  dVar1 = PersistentSettings_ReadDword(0x20,0x10);
  dVar2 = PersistentSettings_ReadDword(0x10,0x18);
  iVar3 = dVar2 * 4;
  if (dVar1 == 0x20) {
    pUVar4 = (UiNodeBase *)&source->shadingResolutionRows;
    if (iVar3 == 0x40) {
      pUVar4 = (UiNodeBase *)((source->shadingResolutionRows).rows + 1);
    }
    else if (iVar3 == 0x80) {
      pUVar4 = (UiNodeBase *)((source->shadingResolutionRows).rows + 2);
    }
  }
  else if (dVar1 == 0x40) {
    pUVar4 = (UiNodeBase *)((source->shadingResolutionRows).rows + 3);
    if (iVar3 == 0x80) {
      pUVar4 = (UiNodeBase *)((source->shadingResolutionRows).rows + 4);
    }
  }
  else {
    pUVar4 = (UiNodeBase *)((source->shadingResolutionRows).rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,pUVar4);
  dVar1 = PersistentSettings_ReadDword(1,0x30);
  if (dVar1 == 0) {
    pUVar4 = (UiNodeBase *)((source->textureResolutionRows).rows + 2);
  }
  else if (dVar1 == 1) {
    pUVar4 = (UiNodeBase *)((source->textureResolutionRows).rows + 1);
  }
  else {
    pUVar4 = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,pUVar4);
  dVar1 = PersistentSettings_ReadDword(0x10000,0x34);
  source->polygonResolutionLodThresholdQ8 = dVar1;
  return;
}


/* Address: 0x0054B8D0.
   Ownership: ui/frontend/settings.
   Purpose: Recovered action-table target FRONTEND_PAGE20[19] (0x2013).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_OpenAndSynchronize
          (FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeFlags *pUVar1;
  UiNodeBase *pUVar2;
  dword dVar3;
  dword dVar4;
  
  UiPageStack_SetActiveIndex(8,&ADJ(settingsSourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    pUVar1 = &ADJ(settingsSourceNode).pageRoot.nodeFlags;
    *pUVar1 = *pUVar1 | 0x2000;
  }
  dVar3 = PersistentSettings_ReadDword(3,0x20);
  UiSelectableControl_SetSelected(dVar3 & 1,&ADJ(settingsSourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(dVar3 & 2,&ADJ(settingsSourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(dVar3 & 4,&ADJ(settingsSourceNode)->reverseStereoControl);
  dVar4 = PersistentSettings_ReadDword(0x8000,0x24);
  (ADJ(settingsSourceNode)->soundEffectsGainControl).currentValue = dVar4;
  dVar4 = PersistentSettings_ReadDword(0x8000,0x28);
  (ADJ(settingsSourceNode)->movieDefaultAudioGainControl).currentValue = dVar4;
  dVar4 = PersistentSettings_ReadDword(0x8000,0x2c);
  (ADJ(settingsSourceNode)->musicGainControl).currentValue = dVar4;
  pUVar2 = settingsSourceNode->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    pUVar2 = settingsSourceNode->parent;
  }
  if ((dVar3 & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x201c,settingsSourceNode);
    UiNodeList_SuppressActionId(0x204e,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x201c,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x204e,settingsSourceNode);
  }
  if ((dVar3 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,settingsSourceNode);
  }
  if ((dVar3 & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,settingsSourceNode);
  }
  return;
}


/* Address: 0x0054BCB0.
   Ownership: ui/frontend/settings.
   Purpose: Updates the user-facing Shading toggle, persists shadingEnabled at settings offset 0x1C, and mirrors it
   to frontend runtime bit 0x00020000. Queued UI action handler for FRONTEND_PAGE20[20] (0x2014). Return datatype
   is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx
FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  byte bVar1;
  UiNodeBase *parentCursor;
  
  bVar1 = UiSelectableControl_IsSelectedCf(control);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((bVar1 & 1) == 0) {
    UiNodeList_SuppressActionId(0x2015,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&control->base);
  }
  PersistentSettings_WriteDword(bVar1 & 1,0x1c);
  return;
}


/* Address: 0x0054BD10.
   Ownership: ui/frontend/settings.
   Purpose: Applies the selected Shading Level. Persists the selected grid value at 0x10, twice that value at 0x14,
   and the selected depth divided by four at 0x18. Queued UI action handler for FRONTEND_PAGE20[21] (0x2015).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  UiNodeVtable *value;
  UiNodeBase *pUVar1;
  uint value_00;
  UiNodeBase *selectedControl;
  
  value = control[1].base.vtable;
  value_00 = (uint)control[1].base.left >> 2;
  PersistentSettings_WriteDword((int)value * 2,0x14);
  PersistentSettings_WriteDword((PersistentSettingsDwordValue)value,0x10);
  PersistentSettings_WriteDword(value_00,0x18);
  pUVar1 = (control->base).parent;
  if (value == (UiNodeVtable *)0x20) {
    selectedControl = (UiNodeBase *)&pUVar1[1].parent;
    if (value_00 == 0x10) {
      selectedControl = (UiNodeBase *)&pUVar1[2].topOffset;
    }
    else if (value_00 == 0x20) {
      selectedControl = (UiNodeBase *)&pUVar1[3].layoutWidth;
    }
  }
  else if (value == (UiNodeVtable *)0x40) {
    selectedControl = (UiNodeBase *)&pUVar1[5].left;
    if (value_00 == 0x20) {
      selectedControl = (UiNodeBase *)&pUVar1[6].bottomOffset;
    }
  }
  else {
    selectedControl = (UiNodeBase *)&pUVar1[7].nodeFlags;
  }
  UiSelectableGroup_SelectExclusive(6,selectedControl);
  return;
}


/* Address: 0x0054BDE0.
   Ownership: ui/frontend/settings.
   Purpose: Persists the custom-slider Q8 model-LOD depth threshold at settings offset 0x34 and mirrors it to
   g_ModelLodDepthThresholdQ8. Queued UI action handler for FRONTEND_PAGE20[22] (0x2016). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x34);
  g_ModelLodDepthThresholdQ8 = value;
  return;
}


/* Address: 0x0054BE10.
   Ownership: ui/frontend/settings.
   Purpose: Maps the Texture Quality choices High, Medium, and Low to persisted values 0, 1, and 2 at settings
   offset 0x30, then rebuilds texture staging resources. Queued UI action handler for FRONTEND_PAGE20[23] (0x2017).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl;
  UiNodeBase *graphicsSettingsRoot;
  
  graphicsSettingsRoot = (control->base).parent;
  if ((UiSelectableControl *)&graphicsSettingsRoot[1].parent == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[1].parent;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[2].bottom == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[2].bottom;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[3].leftAnchorQ31 == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[3].leftAnchorQ31;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl);
  PersistentSettings_WriteDword(qualityLevel,0x30);
  g_TextureDownsampleShift = qualityLevel >> 1;
  (*g_GraphicsRebuildAllStagingTextures)();
  return;
}


/* Address: 0x0054BE90.
   Ownership: ui/frontend/settings.
   Purpose: Queued UI action handler for FRONTEND_PAGE20[24] (0x2018). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx
FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  dword dVar2;
  AudioMixerGainQ15 AVar3;
  MovieAudioGainQ15 MVar4;
  MovieAudioGainQ15 MVar5;
  bool bVar6;
  
  bVar6 = (bool)UiSelectableControl_IsSelectedCf(control);
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword((uint)bVar6 | dVar2 & 0xfffffffe,0x20);
  pUVar1 = (control->base).parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar1 = (control->base).parent;
  }
  if (bVar6) {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,&control->base);
    UiNodeList_UnsuppressActionId(0x204e,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,&control->base);
    UiNodeList_SuppressActionId(0x204e,&control->base);
  }
  if ((dVar2 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,&control->base);
  }
  if (bVar6 == 0 && (dVar2 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,&control->base);
  }
  AVar3 = 0;
  if (bVar6) {
    AVar3 = PersistentSettings_ReadDword(0x8000,0x24);
  }
  MVar4 = 0;
  g_UiSoundGainQ15 = AVar3;
  g_SoundEffectsGainQ15 = AVar3;
  if (bVar6) {
    MVar4 = PersistentSettings_ReadDword(0x8000,0x28);
  }
  MVar5 = 0;
  g_MovieDefaultAudioGainQ15 = MVar4;
  if (bVar6) {
    MVar5 = PersistentSettings_ReadDword(0x8000,0x4c);
  }
  g_MovieAlternateAudioGainQ15 = MVar5;
  return;
}


/* Address: 0x0054BFD0.
   Ownership: ui/frontend/settings.
   Purpose: Updates SOUND_OPTIONS_MUSIC_ENABLED, creates or stops the frontend looping-music voice, applies
   musicGainQ15, and updates related frontend controls. Queued UI action handler for FRONTEND_PAGE20[25] (0x2019).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], Resource_Load
   [assets/resource/runtime], Resource_Release [assets/resource/runtime], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  IDirectSoundBuffer *pIVar2;
  SoundSampleAsset *arg0;
  DirectSoundVoiceSet *arg2;
  dword dVar3;
  uint uVar4;
  uint value;
  bool bVar5;
  SoundCreateSampleVoiceSetEaxCf5 SVar6;
  SoundPlayVoiceEaxCf5 SVar7;
  ResourceLoadEaxEcxCf9 RVar8;
  
  uVar4 = 0;
  bVar5 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar5) {
    uVar4 = 2;
    (*g_GraphicsCursorSetFrame)(6);
    RVar8 = Resource_Load((word *)u_sound_music00_sam_00545c4e);
    arg0 = (SoundSampleAsset *)RVar8.eax;
    pIVar2 = g_FrontendMusicActiveBuffer;
    if (!RVar8.carry) {
      SVar6 = (*g_SoundCreateSampleVoiceSet)(arg0);
      arg2 = SVar6.eax;
      if (SVar6.carry) {
        Resource_Release(arg0);
        pIVar2 = g_FrontendMusicActiveBuffer;
      }
      else {
        g_FrontendMusicVoiceSet = arg2;
        Resource_Release(arg0);
        dVar3 = PersistentSettings_ReadDword(0x8000,0x2c);
        SVar7 = (*g_SoundPlayLooping)(dVar3,dVar3,arg2);
        pIVar2 = SVar7.eax;
        if (SVar7.carry) {
          (*g_SoundReleaseSampleVoiceSet)(arg2);
          g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
          pIVar2 = g_FrontendMusicActiveBuffer;
        }
      }
    }
    g_FrontendMusicActiveBuffer = pIVar2;
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    (*g_SoundStopVoice)(g_FrontendMusicActiveBuffer);
    (*g_SoundReleaseSampleVoiceSet)(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = (IDirectSoundBuffer *)0x0;
    g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
  }
  dVar3 = PersistentSettings_ReadDword(3,0x20);
  value = uVar4 | dVar3 & 0xfffffffd;
  PersistentSettings_WriteDword(value,0x20);
  pUVar1 = (control->base).parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar1 = (control->base).parent;
  }
  if ((value & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,&control->base);
    UiNodeList_SuppressActionId(0x204e,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,&control->base);
    UiNodeList_UnsuppressActionId(0x204e,&control->base);
  }
  if ((uVar4 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,&control->base);
  }
  if ((value & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,&control->base);
  }
  return;
}


/* Address: 0x0054C150.
   Ownership: ui/frontend/settings.
   Purpose: Updates SOUND_OPTIONS_REVERSE_STEREO and writes g_ReverseStereoMask as zero or 0xFFFFFFFF. Queued UI
   action handler for FRONTEND_PAGE20[26] (0x201A). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  dword currentAudioFlags;
  uint uVar1;
  sdword sVar2;
  bool bVar3;
  
  uVar1 = 0;
  sVar2 = 0;
  bVar3 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar3) {
    uVar1 = 4;
    sVar2 = -1;
  }
  currentAudioFlags = PersistentSettings_ReadDword(3,0x20);
  g_ReverseStereoMask = sVar2;
  PersistentSettings_WriteDword(uVar1 | currentAudioFlags & 0xfffffffb,0x20);
  return;
}


/* Address: 0x0054C1A0.
   Ownership: ui/frontend/settings.
   Purpose: Writes soundEffectsGainQ15 from control+0x58 and updates the positional/effect and UI-sound gain
   globals. Queued UI action handler for FRONTEND_PAGE20[27] (0x201B). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x24);
  g_SoundEffectsGainQ15 = value;
  g_UiSoundGainQ15 = value;
  return;
}


/* Address: 0x0054C1D0.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieDefaultAudioGainQ15 from control+0x58 and updates the movie default-gain global. Queued UI
   action handler for FRONTEND_PAGE20[28] (0x201C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x28);
  g_MovieDefaultAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C200.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieAlternateAudioGainQ15 from control+0x58 and updates the alternate movie-gain global used by
   timed movie playback events. Queued UI action handler for FRONTEND_PAGE20[78] (0x204E). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x4c);
  g_MovieAlternateAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C230.
   Ownership: ui/frontend/settings.
   Purpose: Writes musicGainQ15 from control+0x58 and immediately applies equal left/right gains to the active
   frontend music voice. Queued UI action handler for FRONTEND_PAGE20[29] (0x201D). Return datatype is preserved
   for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  dword musicGainQ15;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x2c);
  (*g_SoundSetVoiceGains)(value,value,g_FrontendMusicActiveBuffer);
  return;
}


/* Address: 0x0054D170.
   Ownership: ui/frontend/settings.
   Purpose: Persists control+0x58 as networkPlayerCount at settings offset 0x3C. Queued UI action handler for
   FRONTEND_PAGE20[7] (0x2007). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x3c);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,value,
             (word *)&g_FrontendNetworkPlayerCountTextUtf16);
  return;
}


/* Address: 0x0054D1F0.
   Ownership: ui/frontend/settings.
   Purpose: Writes the game/session name as 0x28 bytes at settings offset 0x88. Queued UI action handler for
   FRONTEND_PAGE20[8] (0x2008). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDwords [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendNetworkSettings_SetGameName(UiTextEditControl *control)

{
  UiTextEditControl *firstNode;
  UiNodeBase *parentCursor;
  
  parentCursor = (control->base).parent;
  firstNode = control;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    firstNode = (UiTextEditControl *)(firstNode->base).parent;
    parentCursor = (firstNode->base).parent;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(0x2004,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2004,&firstNode->base);
    PersistentSettings_WriteDwords(0x28,(dword *)control->textPrefix6C,0x88);
  }
  return;
}


/* Address: 0x0054D250.
   Ownership: ui/frontend/settings.
   Purpose: Updates action 0x2002 availability from the network-settings control state and player-name presence,
   then publishes the player descriptor when the control dirty bit requires it. Queued UI action handler for
   FRONTEND_PAGE20[9] (0x2009). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  UiNodeBase **ppUVar1;
  UiNodeBase *pUVar2;
  FrontendNetworkSettingsControlView250 *firstNode;
  
  pUVar2 = (networkSettings->commonState).commonPrefix.parent;
  firstNode = networkSettings;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    firstNode = (FrontendNetworkSettingsControlView250 *)
                (firstNode->commonState).commonPrefix.parent;
    pUVar2 = (firstNode->commonState).commonPrefix.parent;
  }
  if ((((networkSettings->textEditView).textEdit.base.left == 0) ||
      (*(int *)(*(int *)(networkSettings->textEditView).textEdit.base.bottom + 0x14) == 0)) ||
     ((short)g_FrontendLocalPlayerNameUtf16 == 0)) {
    UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&firstNode->commonState);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2002,(UiNodeBase *)&firstNode->commonState);
    if (((uint)(networkSettings->textEditView).textEdit.base.parent & 4) != 0) {
      ppUVar1 = &(networkSettings->textEditView).textEdit.base.parent;
      *ppUVar1 = (UiNodeBase *)((uint)*ppUVar1 & 0xfffffffb);
      FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
                ((FrontendNetworkSettingsControlView250 *)((int)firstNode + 0x4980));
    }
  }
  return;
}


/* Address: 0x00549620.
   Ownership: ui/frontend/settings.
   Purpose: Rebuilds faction and player control availability, selection state, labels, assignment buffers, and the
   localized task-description payload for the active frontend task-assignment page.
*/
void __fastcall
FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls
          (dword generationCursor,dword selectionIndex,UiRootNode *taskAssignmentRoot)

{
  sdword *psVar1;
  uint *puVar2;
  word wVar3;
  dword dVar4;
  uint uVar5;
  SessionNetworkRoleFlags SVar6;
  FrontendLoadedLevelRuntimeImage370 *pFVar7;
  FrontendFactionAssignmentIndex FVar8;
  SessionNetworkRoleFlags SVar9;
  int iVar10;
  FrontendPlayerRuntimeBlockCount FVar11;
  FrontendPlayerNameUtf16_28 *pFVar12;
  FrontendPlayerNameUtf16_28 *pFVar13;
  FrontendPlayerRuntimeRecord *pFVar14;
  FrontendPlayerRuntimeRecord *pFVar15;
  FrontendTaskAssignmentGeneratedFactionTextRow50 *pFVar16;
  word *pwVar17;
  word *pwVar18;
  uint local_24;
  uint local_20;
  
  local_20 = 0;
  local_24 = 0xfffffff7;
  FVar8 = (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex;
  pFVar14 = g_FrontendPlayerRuntimeBlocks;
  SVar9 = g_FrontendPlayerRuntimeBlockCount;
  SVar6 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (SVar6 != SESSION_NETWORK_ROLE_LOCAL) {
    if (g_LocalPlayerRuntimeId == pFVar14->playerRuntimeId) {
      FVar8 = (pFVar14->factionAssignment).factionAssignmentIndex;
      if ((pFVar14->factionAssignment).consensusValue != 0) {
        local_20 = 8;
        local_24 = 0xffffffff;
      }
      break;
    }
    pFVar14 = pFVar14 + 1;
    SVar9 = SVar9 - SESSION_NETWORK_ROLE_CLIENT;
    SVar6 = SVar9;
  }
  iVar10 = 7;
  taskAssignmentRoot[0x52].base.nextSibling =
       (UiNodeBase *)
       (FVar8 + 0x230010 + (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10);
  do {
    if ((*(uint *)((int)&taskAssignmentRoot->rootFlags +
                  g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar10]) & 2) != 0)
    goto FrontendTaskAssignment_ApplyEligibleFactionControlState;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar10] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL)
         && (dVar4 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[iVar10],
            (*(uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4) & 8) != 0)) {
        if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + dVar4) & 0x40) != 0)
        goto FrontendTaskAssignment_DisableUnavailableFactionControl;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
               SESSION_NETWORK_ROLE_LOCAL) {
        dVar4 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar10];
        puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
        *puVar2 = *puVar2 | 8;
        puVar2 = (uint *)((int)&taskAssignmentRoot->rootFlags + dVar4);
        *puVar2 = *puVar2 & 0xfffffbff;
        goto FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop;
      }
FrontendTaskAssignment_ApplyEligibleFactionControlState:
      dVar4 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar10];
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 | local_20;
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 & local_24;
      puVar2 = (uint *)((int)&taskAssignmentRoot->rootFlags + dVar4);
      *puVar2 = *puVar2 & 0xfffffbff;
    }
    else {
FrontendTaskAssignment_DisableUnavailableFactionControl:
      dVar4 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar10];
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 | 8;
      puVar2 = (uint *)((int)&taskAssignmentRoot->rootFlags + dVar4);
      *puVar2 = *puVar2 | 0x400;
    }
FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop:
    puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[iVar10]);
    *puVar2 = *puVar2 | 8;
    iVar10 = iVar10 + -1;
  } while (iVar10 != 0);
  iVar10 = 7;
  do {
    dVar4 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[iVar10];
    *(undefined4 *)((int)&taskAssignmentRoot->previousRoot + dVar4) = 0x2199;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar10] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
       (*(undefined4 *)((int)&taskAssignmentRoot->previousRoot + dVar4) = 0x219a,
       (*(uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                 g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[iVar10]) & 8) ==
       0)) {
      dVar4 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar10];
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 | local_20;
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 & local_24;
      puVar2 = (uint *)((int)&taskAssignmentRoot->rootFlags + dVar4);
      *puVar2 = *puVar2 & 0xfffffbff;
    }
    else {
      dVar4 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar10];
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
      *puVar2 = *puVar2 | 8;
      puVar2 = (uint *)((int)&taskAssignmentRoot->rootFlags + dVar4);
      *puVar2 = *puVar2 | 0x400;
    }
    pFVar7 = g_FrontendLoadedLevelAsset;
    iVar10 = iVar10 + -1;
    FVar11 = g_FrontendPlayerRuntimeBlockCount;
    pFVar14 = g_FrontendPlayerRuntimeBlocks;
  } while (iVar10 != 0);
  do {
    uVar5 = (pFVar14->factionAssignment).factionAssignmentIndex;
    *(undefined4 *)
     ((int)&taskAssignmentRoot->previousRoot +
     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar5]) = 0x2198;
    FVar11 = FVar11 - 1;
    pFVar14 = pFVar14 + 1;
  } while (FVar11 != 0);
  iVar10 = 7;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if ((*(uint *)((int)&taskAssignmentRoot->rootFlags +
                    g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[iVar10]) &
          0x40) == 0) {
        dVar4 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[iVar10];
        puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
        *puVar2 = *puVar2 | uVar5;
        puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
        *puVar2 = *puVar2 & (uint)pFVar7;
      }
      iVar10 = iVar10 + -1;
      FVar11 = g_FrontendPlayerRuntimeBlockCount;
      pFVar14 = g_FrontendPlayerRuntimeBlocks;
    } while (iVar10 != 0);
    do {
      puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets
                       [(pFVar14->factionAssignment).factionAssignmentIndex]);
      *puVar2 = *puVar2 | 8;
      FVar11 = FVar11 - 1;
      pFVar14 = pFVar14 + 1;
    } while (FVar11 != 0);
  }
  pFVar16 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows + 1;
  for (iVar10 = 0x8c; iVar10 != 0; iVar10 = iVar10 + -1) {
    pFVar16->textUtf16[0] = 0;
    pFVar16->textUtf16[1] = 0;
    pFVar16 = (FrontendTaskAssignmentGeneratedFactionTextRow50 *)(pFVar16->textUtf16 + 2);
  }
  psVar1 = &taskAssignmentRoot[0x4f].base.layoutHeight;
  *psVar1 = *psVar1 | 8;
  pFVar14 = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  psVar1 = &taskAssignmentRoot[0x4f].base.layoutHeight;
  *psVar1 = *psVar1 & 0xfffffff7;
  FVar11 = g_FrontendPlayerRuntimeBlockCount;
  pFVar12 = &pFVar14->playerName;
  do {
    pwVar17 = (word *)((int)&g_FrontendUiDisplayModeAndTaskAssignmentScratch +
                      *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(pFVar12 + 1) + 1) + 8) * 0x50);
    iVar10 = 0x28;
    if (*(int *)pwVar17 == 0) {
FrontendTaskAssignment_AppendPlayerNameToFactionRosterText:
      if (0x15 < iVar10) {
        iVar10 = 0x14;
      }
      if (*(FrontendConsensusValue *)
           ((int)((UiTransferEndpointDescriptor *)(pFVar12 + 1) + 1) + 0xc) == 0) {
        *pwVar17 = 0x8001;
        pFVar13 = pFVar12;
        for (; iVar10 != 0; iVar10 = iVar10 + -1) {
          pwVar17[1] = pFVar13->textUtf16[0];
          pFVar13 = (FrontendPlayerNameUtf16_28 *)(pFVar13->textUtf16 + 1);
          pwVar17 = pwVar17 + 1;
        }
        pwVar17[0] = 0x8000;
        pwVar17[1] = 0;
      }
      else {
        *pwVar17 = 0x8000;
        pFVar13 = pFVar12;
        for (; iVar10 != 0; iVar10 = iVar10 + -1) {
          pwVar17[1] = pFVar13->textUtf16[0];
          pFVar13 = (FrontendPlayerNameUtf16_28 *)(pFVar13->textUtf16 + 1);
          pwVar17 = pwVar17 + 1;
        }
        *pwVar17 = 0;
      }
    }
    else {
      do {
        pwVar18 = pwVar17;
        if (iVar10 == 0) break;
        iVar10 = iVar10 + -1;
        pwVar18 = pwVar17 + 1;
        wVar3 = *pwVar17;
        pwVar17 = pwVar18;
      } while (wVar3 != 0);
      pwVar17 = pwVar18 + 1;
      iVar10 = iVar10 + -1;
      if (iVar10 != 0) {
        pwVar18[-0xffffffff00000001] = 0x2c;
        pwVar18[0] = 0x20;
        goto FrontendTaskAssignment_AppendPlayerNameToFactionRosterText;
      }
    }
    FVar11 = FVar11 - 1;
    pFVar12 = pFVar12 + 0x7e;
    if (FVar11 == 0) {
      iVar10 = 7;
      if (((uint)taskAssignmentRoot[0x23].callbacks & 2) == 0) {
        do {
          dVar4 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar10];
          if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + dVar4) & 0x400) == 0) {
            puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
            *puVar2 = *puVar2 & 0xfffffff7;
          }
          iVar10 = iVar10 + -1;
          FVar11 = g_FrontendPlayerRuntimeBlockCount;
          pFVar14 = g_FrontendPlayerRuntimeBlocks;
        } while (iVar10 != 0);
      }
      else {
        do {
          dVar4 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar10];
          if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + dVar4) & 0x400) == 0) {
            puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + dVar4);
            *puVar2 = *puVar2 | 8;
          }
          iVar10 = iVar10 + -1;
          FVar11 = g_FrontendPlayerRuntimeBlockCount;
          pFVar14 = g_FrontendPlayerRuntimeBlocks;
        } while (iVar10 != 0);
      }
      do {
        if (g_LocalPlayerRuntimeId == pFVar14->playerRuntimeId) {
          iVar10 = (pFVar14->factionAssignment).factionAssignmentIndex;
          pFVar15 = g_FrontendPlayerRuntimeBlocks;
          while ((iVar10 != (pFVar15->factionAssignment).factionAssignmentIndex ||
                 ((int)(pFVar14->factionAssignment).readyOrWaitState <=
                  (int)(pFVar15->factionAssignment).readyOrWaitState))) {
            pFVar15 = pFVar15 + 1;
            FVar11 = FVar11 - 1;
            if (FVar11 == 0) {
              return;
            }
          }
          puVar2 = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar10]);
          *puVar2 = *puVar2 | 8;
          return;
        }
        FVar11 = FVar11 - 1;
        pFVar14 = pFVar14 + 1;
      } while (FVar11 != 0);
      return;
    }
  } while( true );
}


/* Address: 0x0054CD20.
   Ownership: ui/frontend/settings.
   Purpose: Copies the selected frontend player endpoint descriptor into the active transfer endpoint, publishes
   its sender context, resets the sequence token to -1, and invokes UiTransfer_SendPlayerDescriptorPacket20002Cf
   while preserving the backend CF result. Queued UI action handler for FRONTEND_PAGE20[2] (0x2002). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: UiTransfer_SendPlayerDescriptorPacket20002Cf [network/protocol/transfer].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  int iVar1;
  dword *selectedPlayerRecordDwordCursor;
  dword *selectedEndpointDwordCursor;
  bool bVar2;
  
  g_FrontendSessionToken = *(undefined4 *)(**(int **)(networkSettings->raw + 0x248) + 4);
  selectedPlayerRecordDwordCursor = (dword *)(**(int **)(networkSettings->raw + 0x248) + 0xa0);
  selectedEndpointDwordCursor = (dword *)&g_FrontendSelectedNetworkEndpoint;
  for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor = selectedPlayerRecordDwordCursor + 1;
    selectedEndpointDwordCursor = selectedEndpointDwordCursor + 1;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  bVar2 = UiTransfer_SendPlayerDescriptorPacket20002Cf();
  return bVar2;
}


/* Address: 0x0054B160.
   Ownership: ui/frontend/settings.
   Purpose: Walks to the display-settings root, checks the pending adapter, width, height, and bit-depth tuple
   against enumerated modes, updates actions 0x201E through 0x2030, selects the matching groups, and gates action
   0x2031 when settings are unchanged.
   Cross-module calls: DisplayModeTable_ContainsExactModeCf [graphics/backend/directdraw],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists],
   UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_ReadDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *displaySettingsRoot)

{
  undefined4 adapterIndex;
  undefined4 width_00;
  undefined4 height_00;
  undefined4 bitsPerPixel;
  dword dVar1;
  FrontendDisplayDimensionPixels width;
  FrontendDisplayDimensionPixels height;
  bool bVar2;
  UiNodeBase *pUVar3;
  
  bitsPerPixel = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.bitsPerPixel;
  height_00 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection
              .height;
  width_00 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             width;
  adapterIndex = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.adapterIndex;
  pUVar3 = displaySettingsRoot->parent;
  while (pUVar3 != (UiNodeBase *)0xffffffff) {
    displaySettingsRoot = displaySettingsRoot->parent;
    pUVar3 = displaySettingsRoot->parent;
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb4].topAnchorQ31,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex);
  pUVar3 = displaySettingsRoot;
  if (bVar2) {
    UiNodeList_SuppressActionId(0x201e,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201e,displaySettingsRoot);
  }
  if (bitsPerPixel == displaySettingsRoot[0xb4].topAnchorQ31) {
    pUVar3 = (UiNodeBase *)&displaySettingsRoot[0xb3].leftOffset;
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    ((FrontendColorDepthBits)displaySettingsRoot[0xb6].firstChild,height_00,width_00
                     ,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x201f,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201f,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb7].leftOffset,height_00,width_00,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2020,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2020,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb8].bottomAnchorQ31,height_00,width_00,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2021,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2021,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(4,pUVar3);
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa5].nodeFlags,
                     displaySettingsRoot[0xa5].layoutHeight,adapterIndex);
  pUVar3 = displaySettingsRoot;
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2022,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2022,displaySettingsRoot);
  }
  if ((width_00 == displaySettingsRoot[0xa5].layoutHeight) &&
     (height_00 == displaySettingsRoot[0xa5].nodeFlags)) {
    pUVar3 = (UiNodeBase *)&displaySettingsRoot[0xa4].leftAnchorQ31;
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa7].right,displaySettingsRoot[0xa7].top,
                     adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2023,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2023,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa8].topAnchorQ31,
                     displaySettingsRoot[0xa8].leftAnchorQ31,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2024,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2024,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].firstChild,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].nextSibling,
                     adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2025,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2025,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xab].leftOffset,
                     displaySettingsRoot[0xab].bottom,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2026,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2026,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xac].bottomAnchorQ31,
                     displaySettingsRoot[0xac].rightAnchorQ31,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2027,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2027,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,(FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].vtable,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].parent,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2028,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2028,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xaf].rightOffset,
                     displaySettingsRoot[0xaf].topOffset,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2029,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2029,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xb0].layoutHeight,
                     displaySettingsRoot[0xb0].layoutWidth,adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202a,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202a,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xb2].top,displaySettingsRoot[0xb2].left,
                     adapterIndex);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202b,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202b,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(10,pUVar3);
  bVar2 = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height_00,width_00,0);
  pUVar3 = displaySettingsRoot;
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202c,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202c,displaySettingsRoot);
  }
  if (adapterIndex == 0) {
    pUVar3 = (UiNodeBase *)&displaySettingsRoot[0x9c].topAnchorQ31;
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height_00,width_00,1);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202d,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202d,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height_00,width_00,2);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202e,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202e,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height_00,width_00,3);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x202f,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202f,displaySettingsRoot);
  }
  bVar2 = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height_00,width_00,4);
  if (bVar2) {
    UiNodeList_SuppressActionId(0x2030,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2030,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(5,pUVar3);
  dVar1 = PersistentSettings_ReadDword(1,0);
  if ((((dVar1 == adapterIndex) &&
       (dVar1 = PersistentSettings_ReadDword(0x280,4), dVar1 == width_00)) &&
      (dVar1 = PersistentSettings_ReadDword(0x1e0,8), dVar1 == height_00)) &&
     (dVar1 = PersistentSettings_ReadDword(0x10,0xc), dVar1 == bitsPerPixel)) {
    UiNodeList_SuppressActionId(0x2031,displaySettingsRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(0x2031,displaySettingsRoot);
  return;
}

