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
void FrontendTaskAssignmentPage_Initialize(UiRootNode *frontendRoot)

{
  sdword *psVar1;
  UiAnchorFractionQ31 *pUVar2;
  uint *puVar3;
  UiNodeVtable *pUVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  word *pwVar8;
  uint uVar9;
  dword dVar10;
  int extraout_ECX;
  dword dVar11;
  void *replacementPayload;
  int iVar12;
  FrontendPlayerRuntimeRecord *pFVar13;
  FrontendPlayerRuntimeBlockCount FVar14;
  
  UiPageStack_SetActiveIndex(0xb,(UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  if ((int)g_FramebufferWidth < 0x281) {
    psVar1 = &frontendRoot[10].base.layoutHeight;
    *psVar1 = *psVar1 | 0x2000;
  }
  iVar5 = g_FrontendLoadedLevelAsset;
  iVar6 = *(int *)(g_FrontendLoadedLevelAsset + 0x314);
  iVar12 = *(int *)(g_FrontendLoadedLevelAsset + 0x310);
  uVar7 = 0;
  do {
    dVar11 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar7 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffff7;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffbff;
    *(undefined4 *)((int)&frontendRoot->previousRoot + dVar11) = 0x219a;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar7 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffbff;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar7 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffff7;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffbfd;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar7 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffff7;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xffffffbf;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar7 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffff7;
    uVar9 = uVar7 + 1;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar7 + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    iVar6 = iVar6 + -1;
    iVar12 = iVar12 + -1;
    uVar7 = uVar9;
  } while (iVar12 != 0);
  for (; iVar6 != 0; iVar6 = iVar6 + -1) {
    dVar11 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffff7;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffbff;
    *(undefined4 *)((int)&frontendRoot->previousRoot + dVar11) = 0x219a;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffbff;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffffd;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x400;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xffffffbf;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar9 + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    uVar9 = uVar9 + 1;
  }
  for (; uVar9 < 7; uVar9 = uVar9 + 1) {
    dVar11 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x400;
    *(undefined4 *)((int)&frontendRoot->previousRoot + dVar11) = 0x2199;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x400;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffffd;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x400;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x40;
    dVar11 = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[uVar9 + 1];
    puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags + dVar11);
    *puVar3 = *puVar3 | 8;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 | 0x40;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar9 + 1] = 0;
  }
  do {
    dVar11 = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[uVar9];
    *(uint *)((int)&frontendRoot->previousRoot + dVar11) =
         *(int *)((&g_InGameConditionRuntime)[uVar9][2].reserved5C_AB + iVar5 + 8) + 0x2173 + uVar9;
    puVar3 = (uint *)((int)&frontendRoot->rootFlags + dVar11);
    *puVar3 = *puVar3 & 0xfffffffd;
    iVar6 = g_LocalPlayerRuntimeId;
    uVar9 = uVar9 - 1;
  } while (uVar9 != 0);
  uVar7 = 1;
  dVar11 = 0;
  pFVar13 = g_FrontendPlayerRuntimeBlocks;
  FVar14 = g_FrontendPlayerRuntimeBlockCount;
  do {
    dVar10 = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[uVar7];
    (pFVar13->factionAssignment).factionAssignmentIndex = uVar7;
    dVar10 = (int)&(frontendRoot->base).nextSibling + dVar10;
    (pFVar13->factionAssignment).readyOrWaitState = 0;
    (pFVar13->factionAssignment).consensusValue = 0;
    *(undefined4 *)(dVar10 + 0x54) = 0x2198;
    if (iVar6 == pFVar13->playerRuntimeId) {
      dVar11 = uVar7 - 1;
    }
    uVar7 = uVar7 + 1;
    pFVar13 = pFVar13 + 1;
    if (*(uint *)(iVar5 + 0x310) < uVar7) {
      uVar7 = uVar7 - *(int *)(iVar5 + 0x310);
    }
    FVar14 = FVar14 - 1;
  } while (FVar14 != 0);
  puVar3 = (uint *)((int)&frontendRoot->rootFlags +
                   g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar11 + 1]);
  *puVar3 = *puVar3 | 2;
  puVar3 = (uint *)((int)&(frontendRoot->base).nodeFlags +
                   g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[dVar11 + 1]);
  *puVar3 = *puVar3 & 0xfffffff7;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(dVar10,dVar11,frontendRoot);
  pUVar4 = (frontendRoot->base).vtable;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    frontendRoot[0x24].base.bottomOffset = 0x60;
    frontendRoot[0x24].base.topAnchorQ31 = 0x60;
    frontendRoot[0x23].rootFlags = frontendRoot[0x23].rootFlags | 8;
    pUVar2 = &frontendRoot[0x21].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 & 0xfffffff7;
    pUVar2 = &frontendRoot[0x20].base.topAnchorQ31;
    *pUVar2 = *pUVar2 & 0xfffffff7;
    pUVar2 = &frontendRoot[0x20].base.rightAnchorQ31;
    *pUVar2 = *pUVar2 & 0xfffffbff;
  }
  else {
    frontendRoot[0x24].base.bottomOffset = 0;
    frontendRoot[0x24].base.topAnchorQ31 = 0;
    frontendRoot[0x23].rootFlags = frontendRoot[0x23].rootFlags & 0xfffffff7;
    frontendRoot[0x23].callbacks =
         (UiRootCallbacks *)((uint)frontendRoot[0x23].callbacks & 0xfffffffd);
    pUVar2 = &frontendRoot[0x21].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 | 8;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      psVar1 = &frontendRoot[0x21].base.layoutWidth;
      *psVar1 = *psVar1 | 0x400;
      pUVar2 = &frontendRoot[0x20].base.topAnchorQ31;
      *pUVar2 = *pUVar2 | 8;
      pUVar2 = &frontendRoot[0x20].base.rightAnchorQ31;
      *pUVar2 = *pUVar2 | 0x400;
    }
    else {
      psVar1 = &frontendRoot[0x21].base.layoutWidth;
      *psVar1 = *psVar1 & 0xfffffbff;
      pUVar2 = &frontendRoot[0x20].base.topAnchorQ31;
      *pUVar2 = *pUVar2 & 0xfffffff7;
      pUVar2 = &frontendRoot[0x20].base.rightAnchorQ31;
      *pUVar2 = *pUVar2 & 0xfffffbff;
    }
  }
  iVar6 = g_FrontendLoadedLevelAsset;
  (*pUVar4->layout)(&frontendRoot->base);
  pwVar8 = TextResource_Resolve(extraout_ECX + *(int *)(iVar6 + 0x170));
  *pwVar8 = 0x8000;
  pwVar8 = TextResource_Resolve(0x218c);
  RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,pwVar8);
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
undefined8 FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *displaySettingsRoot)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       displaySettingsRoot[1].top;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       displaySettingsRoot[1].right;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
  return CONCAT44(in_EDX,in_EAX);
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
undefined4 FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *displaySettingsRoot)

{
  undefined4 in_EAX;
  
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = displaySettingsRoot[1].top;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
  return in_EAX;
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
void FrontendDisplaySettings_ApplyMode(void *control)

{
  int iVar1;
  undefined4 value;
  undefined4 value_00;
  PersistentSettingsDwordValue value_01;
  int extraout_ECX;
  PersistentSettingsDwordValue value_02;
  uint arg1;
  GraphicsTextureSourceAsset **ppGVar2;
  bool bVar3;
  dword arg2;
  dword arg3;
  dword arg0;
  
  value_00 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             bitsPerPixel;
  value = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
          adapterIndex;
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  bVar3 = false;
  arg1 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
         g_SoftwarePixelFormatConfig.blueBitCount + 0xf & 0xfffffff0;
  arg2 = g_FramebufferHeight;
  arg3 = g_FramebufferWidth;
  arg0 = g_ActiveGraphicsAdapterIndex;
  (*g_GraphicsDisplayModeHook)
            (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             adapterIndex,
             g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             bitsPerPixel,
             g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             height,g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                    persistentSelection.width);
  if (bVar3) {
    (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
    (*g_FatalErrorPrimaryDispatchCf)();
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    (*g_FatalErrorRuntimeDispatchCf)();
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
  PersistentSettings_WriteDword(value_01,4);
  PersistentSettings_WriteDword(value_02,8);
  PersistentSettings_WriteDword(value_00,0xc);
  UiRootStack_Relayout();
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_FrontendMenuTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowClassTextureSource);
  ppGVar2 = g_FontTextureSources;
  do {
    (*g_GraphicsTextureSourceConvertPaletteEntries)((GraphicsPaletteTextureSourceAsset *)*ppGVar2);
    ppGVar2 = ppGVar2 + 1;
  } while (extraout_ECX != 1);
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
void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control)

{
  UiTextEditControl *pUVar1;
  UiNodeBase *pUVar2;
  int extraout_EAX;
  UiNodeBase *firstNode;
  int iVar3;
  word *pwVar4;
  undefined4 *puVar5;
  
  pUVar2 = (control->base).parent;
  pUVar1 = control;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    pUVar1 = (UiTextEditControl *)(pUVar1->base).parent;
    pUVar2 = (pUVar1->base).parent;
  }
  pUVar2 = (UiNodeBase *)UiTextControl_UpdateNonEmptyValidity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(0x2001,pUVar2);
    UiNodeList_SuppressActionId(0x2002,firstNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2001,pUVar2);
    FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
              ((FrontendNetworkSettingsControlView250 *)(extraout_EAX + 0x4b68));
    PersistentSettings_WriteDwords(0x28,control->textPrefix6C,0x60);
    pwVar4 = control->textPrefix6C;
    puVar5 = &g_FrontendLocalPlayerNameUtf16;
    for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *(undefined4 *)pwVar4;
      pwVar4 = pwVar4 + 2;
      puVar5 = puVar5 + 1;
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
void FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control)

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
void FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 4;
  }
  else {
    value = extraout_ECX & 0xfffffffb;
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
void FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

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
void FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 1;
  }
  else {
    value = extraout_ECX & 0xfffffffe;
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
void FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 2;
  }
  else {
    value = extraout_ECX & 0xfffffffd;
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
void FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  PersistentSettingsDwordValue extraout_ECX;
  PersistentSettingsDwordValue extraout_ECX_00;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    UiNodeList_SuppressActionId(0x203f,(control->base).parent);
    value = extraout_ECX;
  }
  else {
    UiNodeList_UnsuppressActionId(0x203f,(control->base).parent);
    value = extraout_ECX_00;
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
void FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  PersistentSettingsDwordValue extraout_ECX;
  PersistentSettingsDwordValue extraout_ECX_00;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    UiNodeList_SuppressActionId(0x203e,(control->base).parent);
    value = extraout_ECX;
  }
  else {
    UiNodeList_UnsuppressActionId(0x203e,(control->base).parent);
    value = extraout_ECX_00;
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
void FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 4;
  }
  else {
    value = extraout_ECX & 0xfffffffb;
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
void FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  sdword *psVar1;
  dword dVar2;
  UiBooleanState32 extraout_EAX;
  UiBooleanState32 UVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  UiBooleanState32 extraout_ECX_02;
  UiSelectableControl *extraout_EDX;
  UiSelectableControl *extraout_EDX_00;
  UiSelectableControl *pUVar4;
  
  UiPageStack_SetActiveIndex(5,(UiPageStackControl *)&frontendRoot[0xe].base.rightAnchorQ31);
  if ((int)g_FramebufferWidth < 0x281) {
    psVar1 = &frontendRoot[10].base.layoutHeight;
    *psVar1 = *psVar1 | 0x2000;
  }
  dVar2 = PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_SetSelected
            (dVar2 & 1,(UiSelectableControl *)&frontendRoot[0x7b].base.bottomAnchorQ31);
  UiSelectableControl_SetSelected
            (extraout_ECX & 2,(UiSelectableControl *)&frontendRoot[0x7c].base.layoutHeight);
  UiSelectableControl_SetSelected
            (extraout_ECX_00 & 4,(UiSelectableControl *)&frontendRoot[0x75].base.bottom);
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  pUVar4 = (UiSelectableControl *)&frontendRoot[0x7e].base.nodeFlags;
  UVar3 = 0;
  if ((dVar2 & 1) != 0) {
    UiNodeList_SuppressActionId(0x203f,&frontendRoot->base);
    UVar3 = extraout_EAX;
    pUVar4 = extraout_EDX;
  }
  UiSelectableControl_SetSelected(UVar3,pUVar4);
  pUVar4 = (UiSelectableControl *)&frontendRoot[0x7f].callbacks;
  UVar3 = 0;
  if ((extraout_ECX_01 & 2) != 0) {
    UiNodeList_SuppressActionId(0x203e,&frontendRoot->base);
    UVar3 = extraout_ECX_02;
    pUVar4 = extraout_EDX_00;
  }
  UiSelectableControl_SetSelected(UVar3,pUVar4);
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
void FrontendGraphicsSettings_OpenAndSynchronize
               (FrontendGraphicsRuntimeSettingsPageState167C *source)

{
  FrontendGraphicsRuntimeSettingsPageState167C *firstNode;
  dword dVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  UiNodeBase *pUVar3;
  
  UiPageStack_SetActiveIndex(7,(UiPageStackControl *)(source[-2].reserved4C_1067 + 0xa20));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)(source[-2].reserved4C_1067 + 0x8cc) =
         *(uint *)(source[-2].reserved4C_1067 + 0x8cc) | 0x2000;
  }
  dVar1 = PersistentSettings_ReadDword(1,0x1c);
  UiSelectableControl_SetSelected(dVar1,&source->shadingEnabledControl);
  pUVar3 = (source->base).parent;
  firstNode = source;
  while (pUVar3 != (UiNodeBase *)0xffffffff) {
    firstNode = (FrontendGraphicsRuntimeSettingsPageState167C *)(firstNode->base).parent;
    pUVar3 = (firstNode->base).parent;
  }
  if (extraout_ECX == 0) {
    UiNodeList_SuppressActionId(0x2015,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&firstNode->base);
  }
  PersistentSettings_ReadDword(0x20,0x10);
  dVar1 = PersistentSettings_ReadDword(0x10,0x18);
  iVar2 = dVar1 * 4;
  if (extraout_ECX_00 == 0x20) {
    pUVar3 = (UiNodeBase *)&source->shadingResolutionRows;
    if (iVar2 == 0x40) {
      pUVar3 = (UiNodeBase *)((source->shadingResolutionRows).rows + 1);
    }
    else if (iVar2 == 0x80) {
      pUVar3 = (UiNodeBase *)((source->shadingResolutionRows).rows + 2);
    }
  }
  else if (extraout_ECX_00 == 0x40) {
    pUVar3 = (UiNodeBase *)((source->shadingResolutionRows).rows + 3);
    if (iVar2 == 0x80) {
      pUVar3 = (UiNodeBase *)((source->shadingResolutionRows).rows + 4);
    }
  }
  else {
    pUVar3 = (UiNodeBase *)((source->shadingResolutionRows).rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,pUVar3);
  dVar1 = PersistentSettings_ReadDword(1,0x30);
  if (dVar1 == 0) {
    pUVar3 = (UiNodeBase *)((source->textureResolutionRows).rows + 2);
  }
  else if (dVar1 == 1) {
    pUVar3 = (UiNodeBase *)((source->textureResolutionRows).rows + 1);
  }
  else {
    pUVar3 = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,pUVar3);
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
void FrontendAudioSettings_OpenAndSynchronize
               (FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeFlags *pUVar1;
  dword dVar2;
  UiNodeBase *firstNode;
  UiNodeBase *firstNode_00;
  UiNodeBase *extraout_EAX;
  UiNodeBase *firstNode_01;
  UiNodeBase *firstNode_02;
  UiNodeBase *extraout_EAX_00;
  UiNodeBase *extraout_EAX_01;
  UiNodeBase *extraout_EAX_02;
  UiNodeBase *pUVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint uVar4;
  
  UiPageStack_SetActiveIndex(8,&ADJ(settingsSourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    pUVar1 = &ADJ(settingsSourceNode).pageRoot.nodeFlags;
    *pUVar1 = *pUVar1 | 0x2000;
  }
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  UiSelectableControl_SetSelected(dVar2 & 1,&ADJ(settingsSourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(extraout_ECX & 2,&ADJ(settingsSourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected
            (extraout_ECX_00 & 4,&ADJ(settingsSourceNode)->reverseStereoControl);
  dVar2 = PersistentSettings_ReadDword(0x8000,0x24);
  (ADJ(settingsSourceNode)->soundEffectsGainControl).currentValue = dVar2;
  dVar2 = PersistentSettings_ReadDword(0x8000,0x28);
  (ADJ(settingsSourceNode)->movieDefaultAudioGainControl).currentValue = dVar2;
  dVar2 = PersistentSettings_ReadDword(0x8000,0x2c);
  (ADJ(settingsSourceNode)->musicGainControl).currentValue = dVar2;
  pUVar3 = settingsSourceNode->parent;
  while (pUVar3 != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    pUVar3 = settingsSourceNode->parent;
  }
  if ((extraout_ECX_01 & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x201c,firstNode_01);
    UiNodeList_SuppressActionId(0x204e,firstNode_02);
    pUVar3 = extraout_EAX_00;
    uVar4 = extraout_ECX_03;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x201c,firstNode);
    UiNodeList_UnsuppressActionId(0x204e,firstNode_00);
    pUVar3 = extraout_EAX;
    uVar4 = extraout_ECX_02;
  }
  if ((uVar4 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,pUVar3);
    pUVar3 = extraout_EAX_02;
    uVar4 = extraout_ECX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,pUVar3);
    pUVar3 = extraout_EAX_01;
    uVar4 = extraout_ECX_04;
  }
  if ((uVar4 & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,pUVar3);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,pUVar3);
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
void FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  PersistentSettingsDwordValue extraout_ECX;
  PersistentSettingsDwordValue extraout_ECX_00;
  PersistentSettingsDwordValue value;
  byte in_CF;
  UiNodeBase *parentCursor;
  
  UiSelectableControl_IsSelectedCf(control);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((in_CF & 1) == 0) {
    UiNodeList_SuppressActionId(0x2015,&control->base);
    value = extraout_ECX_00;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&control->base);
    value = extraout_ECX;
  }
  PersistentSettings_WriteDword(value,0x1c);
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
void FrontendShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  PersistentSettingsDwordValue value;
  int extraout_EAX;
  int iVar2;
  PersistentSettingsDwordValue value_00;
  int extraout_ECX;
  UiNodeBase *selectedControl;
  
  PersistentSettings_WriteDword((int)control[1].base.vtable * 2,0x14);
  PersistentSettings_WriteDword(value_00,0x10);
  PersistentSettings_WriteDword(value,0x18);
  iVar2 = extraout_EAX * 4;
  pUVar1 = (control->base).parent;
  if (extraout_ECX == 0x20) {
    selectedControl = (UiNodeBase *)&pUVar1[1].parent;
    if (iVar2 == 0x40) {
      selectedControl = (UiNodeBase *)&pUVar1[2].topOffset;
    }
    else if (iVar2 == 0x80) {
      selectedControl = (UiNodeBase *)&pUVar1[3].layoutWidth;
    }
  }
  else if (extraout_ECX == 0x40) {
    selectedControl = (UiNodeBase *)&pUVar1[5].left;
    if (iVar2 == 0x80) {
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
void FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  sdword extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x34);
  g_ModelLodDepthThresholdQ8 = extraout_EAX;
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
void FrontendTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentSettingsDwordValue value;
  uint extraout_EDX;
  UiNodeBase *unaff_EDI;
  UiNodeBase *graphicsSettingsRoot;
  
  graphicsSettingsRoot = (control->base).parent;
  if ((UiSelectableControl *)&graphicsSettingsRoot[1].parent == control) {
    unaff_EDI = (UiNodeBase *)&graphicsSettingsRoot[1].parent;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[2].bottom == control) {
    unaff_EDI = (UiNodeBase *)&graphicsSettingsRoot[2].bottom;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[3].leftAnchorQ31 == control) {
    unaff_EDI = (UiNodeBase *)&graphicsSettingsRoot[3].leftAnchorQ31;
  }
  UiSelectableGroup_SelectExclusive(3,unaff_EDI);
  PersistentSettings_WriteDword(value,0x30);
  g_TextureDownsampleShift = extraout_EDX >> 1;
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
void FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  dword dVar1;
  UiNodeBase *firstNode;
  UiNodeBase *firstNode_00;
  UiNodeBase *extraout_EAX;
  UiNodeBase *firstNode_01;
  UiNodeBase *firstNode_02;
  UiNodeBase *extraout_EAX_00;
  UiNodeBase *extraout_EAX_01;
  UiNodeBase *extraout_EAX_02;
  UiNodeBase *pUVar2;
  AudioMixerGainQ15 AVar3;
  MovieAudioGainQ15 MVar4;
  MovieAudioGainQ15 MVar5;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  uint extraout_ECX_08;
  uint uVar6;
  
  UiSelectableControl_IsSelectedCf(control);
  dVar1 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword(extraout_ECX | dVar1 & 0xfffffffe,0x20);
  pUVar2 = (control->base).parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar2 = (control->base).parent;
  }
  if ((extraout_ECX_00 & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,firstNode_01);
    UiNodeList_SuppressActionId(0x204e,firstNode_02);
    pUVar2 = extraout_EAX_00;
    uVar6 = extraout_ECX_02;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,firstNode);
    UiNodeList_UnsuppressActionId(0x204e,firstNode_00);
    pUVar2 = extraout_EAX;
    uVar6 = extraout_ECX_01;
  }
  if ((uVar6 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,pUVar2);
    pUVar2 = extraout_EAX_02;
    uVar6 = extraout_ECX_04;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,pUVar2);
    pUVar2 = extraout_EAX_01;
    uVar6 = extraout_ECX_03;
  }
  if ((uVar6 & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,pUVar2);
    uVar6 = extraout_ECX_06;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,pUVar2);
    uVar6 = extraout_ECX_05;
  }
  AVar3 = 0;
  if ((uVar6 & 1) != 0) {
    AVar3 = PersistentSettings_ReadDword(0x8000,0x24);
    uVar6 = extraout_ECX_07;
  }
  MVar4 = 0;
  g_UiSoundGainQ15 = AVar3;
  g_SoundEffectsGainQ15 = AVar3;
  if ((uVar6 & 1) != 0) {
    MVar4 = PersistentSettings_ReadDword(0x8000,0x28);
    uVar6 = extraout_ECX_08;
  }
  MVar5 = 0;
  g_MovieDefaultAudioGainQ15 = MVar4;
  if ((uVar6 & 1) != 0) {
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
void FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  IDirectSoundBuffer *pIVar1;
  dword dVar2;
  UiNodeBase *firstNode;
  UiNodeBase *firstNode_00;
  UiNodeBase *extraout_EAX;
  UiNodeBase *firstNode_01;
  UiNodeBase *firstNode_02;
  UiNodeBase *extraout_EAX_00;
  UiNodeBase *extraout_EAX_01;
  UiNodeBase *extraout_EAX_02;
  UiNodeBase *pUVar3;
  undefined4 extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint uVar4;
  undefined4 extraout_EDX;
  void *allocation;
  bool bVar5;
  undefined1 uVar6;
  DirectSoundVoiceSet *pDVar8;
  undefined8 uVar7;
  
  bVar5 = false;
  UiSelectableControl_IsSelectedCf(control);
  if (bVar5) {
    uVar6 = 0;
    (*g_GraphicsCursorSetFrame)(6);
    uVar7 = Resource_Load(extraout_ECX,extraout_EDX,(word *)u_sound_music00_sam_00545c4e);
    pIVar1 = g_FrontendMusicActiveBuffer;
    if (!(bool)uVar6) {
      _pDVar8 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar7);
      allocation = (void *)((ulonglong)_pDVar8 >> 0x20);
      pDVar8 = SUB84(_pDVar8,0);
      if ((bool)uVar6) {
        Resource_Release(allocation);
        pIVar1 = g_FrontendMusicActiveBuffer;
      }
      else {
        g_FrontendMusicVoiceSet = pDVar8;
        Resource_Release(allocation);
        dVar2 = PersistentSettings_ReadDword(0x8000,0x2c);
        pIVar1 = (*g_SoundPlayLooping)(dVar2,dVar2,pDVar8);
        if ((bool)uVar6) {
          (*g_SoundReleaseSampleVoiceSet)(pDVar8);
          g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
          pIVar1 = g_FrontendMusicActiveBuffer;
        }
      }
    }
    g_FrontendMusicActiveBuffer = pIVar1;
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    (*g_SoundStopVoice)(g_FrontendMusicActiveBuffer);
    (*g_SoundReleaseSampleVoiceSet)(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = (IDirectSoundBuffer *)0x0;
    g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
  }
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword(extraout_ECX_00 | dVar2 & 0xfffffffd,0x20);
  pUVar3 = (control->base).parent;
  while (pUVar3 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar3 = (control->base).parent;
  }
  if ((extraout_ECX_01 & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,firstNode_01);
    UiNodeList_SuppressActionId(0x204e,firstNode_02);
    pUVar3 = extraout_EAX_00;
    uVar4 = extraout_ECX_03;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,firstNode);
    UiNodeList_UnsuppressActionId(0x204e,firstNode_00);
    pUVar3 = extraout_EAX;
    uVar4 = extraout_ECX_02;
  }
  if ((uVar4 & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,pUVar3);
    pUVar3 = extraout_EAX_02;
    uVar4 = extraout_ECX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,pUVar3);
    pUVar3 = extraout_EAX_01;
    uVar4 = extraout_ECX_04;
  }
  if ((uVar4 & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,pUVar3);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,pUVar3);
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
void FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  dword currentAudioFlags;
  uint extraout_ECX;
  sdword extraout_EDX;
  
  UiSelectableControl_IsSelectedCf(control);
  currentAudioFlags = PersistentSettings_ReadDword(3,0x20);
  g_ReverseStereoMask = extraout_EDX;
  PersistentSettings_WriteDword(extraout_ECX | currentAudioFlags & 0xfffffffb,0x20);
  return;
}

/* Address: 0x0054C1A0.
   Ownership: ui/frontend/settings.
   Purpose: Writes soundEffectsGainQ15 from control+0x58 and updates the positional/effect and UI-sound gain
   globals. Queued UI action handler for FRONTEND_PAGE20[27] (0x201B). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  AudioMixerGainQ15 extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x24);
  g_SoundEffectsGainQ15 = extraout_EAX;
  g_UiSoundGainQ15 = extraout_EAX;
  return;
}

/* Address: 0x0054C1D0.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieDefaultAudioGainQ15 from control+0x58 and updates the movie default-gain global. Queued UI
   action handler for FRONTEND_PAGE20[28] (0x201C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  MovieAudioGainQ15 extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x28);
  g_MovieDefaultAudioGainQ15 = extraout_EAX;
  return;
}

/* Address: 0x0054C200.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieAlternateAudioGainQ15 from control+0x58 and updates the alternate movie-gain global used by
   timed movie playback events. Queued UI action handler for FRONTEND_PAGE20[78] (0x204E). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  MovieAudioGainQ15 extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x4c);
  g_MovieAlternateAudioGainQ15 = extraout_EAX;
  return;
}

/* Address: 0x0054C230.
   Ownership: ui/frontend/settings.
   Purpose: Writes musicGainQ15 from control+0x58 and immediately applies equal left/right gains to the active
   frontend music voice. Queued UI action handler for FRONTEND_PAGE20[29] (0x201D). Return datatype is preserved
   for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  dword musicGainQ15;
  
  PersistentSettings_WriteDword(control->boundValue,0x2c);
  (*g_SoundSetVoiceGains)(musicGainQ15,musicGainQ15,g_FrontendMusicActiveBuffer);
  return;
}

/* Address: 0x0054D170.
   Ownership: ui/frontend/settings.
   Purpose: Persists control+0x58 as networkPlayerCount at settings offset 0x3C. Queued UI action handler for
   FRONTEND_PAGE20[7] (0x2007). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control)

{
  sdword arg4;
  
  PersistentSettings_WriteDword(control->boundValue,0x3c);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,arg4,(word *)&g_FrontendNetworkPlayerCountTextUtf16
            );
  return;
}

/* Address: 0x0054D1F0.
   Ownership: ui/frontend/settings.
   Purpose: Writes the game/session name as 0x28 bytes at settings offset 0x88. Queued UI action handler for
   FRONTEND_PAGE20[8] (0x2008). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDwords [core/settings/persistent].
*/
void FrontendNetworkSettings_SetGameName(UiTextEditControl *control)

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
    PersistentSettings_WriteDwords(0x28,control->textPrefix6C,0x88);
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
undefined8
FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  UiNodeBase **ppUVar1;
  UiNodeBase *pUVar2;
  FrontendNetworkSettingsControlView250 *firstNode;
  undefined4 in_EAX;
  int extraout_EAX;
  undefined4 in_EDX;
  
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
                ((FrontendNetworkSettingsControlView250 *)(extraout_EAX + 0x4980));
    }
  }
  return CONCAT44(in_EDX,in_EAX);
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
  uint uVar7;
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
       (UiNodeBase *)(FVar8 + 0x230010 + *(int *)(g_FrontendLoadedLevelAsset + 0x170) * 0x10);
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
    uVar7 = g_FrontendLoadedLevelAsset;
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
        *puVar2 = *puVar2 & uVar7;
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
undefined1
FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  undefined1 in_AL;
  int iVar1;
  dword *selectedPlayerRecordDwordCursor;
  dword *selectedEndpointDwordCursor;
  
  g_FrontendSessionToken = *(undefined4 *)(**(int **)(networkSettings->raw + 0x248) + 4);
  selectedPlayerRecordDwordCursor = (dword *)(**(int **)(networkSettings->raw + 0x248) + 0xa0);
  selectedEndpointDwordCursor = (dword *)&g_FrontendSelectedNetworkEndpoint;
  for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor = selectedPlayerRecordDwordCursor + 1;
    selectedEndpointDwordCursor = selectedEndpointDwordCursor + 1;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  UiTransfer_SendPlayerDescriptorPacket20002Cf();
  return in_AL;
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
void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *displaySettingsRoot)

{
  undefined4 adapterIndex;
  undefined4 bitsPerPixel;
  dword dVar1;
  FrontendDisplayDimensionPixels extraout_ECX;
  FrontendDisplayDimensionPixels extraout_ECX_00;
  FrontendDisplayDimensionPixels extraout_ECX_01;
  FrontendDisplayDimensionPixels extraout_ECX_02;
  FrontendDisplayDimensionPixels extraout_ECX_03;
  FrontendDisplayDimensionPixels extraout_ECX_04;
  uint extraout_ECX_05;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  uint extraout_ECX_08;
  uint extraout_ECX_09;
  uint extraout_ECX_10;
  UiNodeBase *extraout_ECX_11;
  UiNodeBase *extraout_ECX_12;
  uint extraout_ECX_13;
  uint extraout_ECX_14;
  uint extraout_ECX_15;
  uint extraout_ECX_16;
  UiNodeBase *extraout_ECX_17;
  UiNodeBase *extraout_ECX_18;
  UiNodeBase *pUVar2;
  uint extraout_ECX_19;
  uint extraout_ECX_20;
  uint extraout_ECX_21;
  uint extraout_ECX_22;
  uint uVar3;
  FrontendDisplayDimensionPixels width;
  FrontendDisplayDimensionPixels extraout_ECX_23;
  FrontendDisplayDimensionPixels extraout_ECX_24;
  FrontendDisplayDimensionPixels extraout_ECX_25;
  FrontendDisplayDimensionPixels extraout_ECX_26;
  FrontendDisplayDimensionPixels extraout_ECX_27;
  FrontendDisplayDimensionPixels extraout_ECX_28;
  FrontendDisplayDimensionPixels extraout_ECX_29;
  FrontendDisplayDimensionPixels extraout_ECX_30;
  FrontendDisplayDimensionPixels FVar4;
  dword extraout_ECX_31;
  FrontendDisplayDimensionPixels extraout_EDX;
  FrontendDisplayDimensionPixels extraout_EDX_00;
  FrontendDisplayDimensionPixels extraout_EDX_01;
  FrontendDisplayDimensionPixels extraout_EDX_02;
  FrontendDisplayDimensionPixels extraout_EDX_03;
  FrontendDisplayDimensionPixels extraout_EDX_04;
  UiNodeFlags extraout_EDX_05;
  UiNodeFlags extraout_EDX_06;
  UiNodeFlags UVar5;
  uint extraout_EDX_07;
  uint extraout_EDX_08;
  uint extraout_EDX_09;
  uint extraout_EDX_10;
  UiNodeBase *extraout_EDX_11;
  UiNodeBase *extraout_EDX_12;
  UiNodeBase *pUVar6;
  uint extraout_EDX_13;
  uint extraout_EDX_14;
  uint extraout_EDX_15;
  uint extraout_EDX_16;
  UiNodeVtable *extraout_EDX_17;
  UiNodeVtable *extraout_EDX_18;
  UiNodeVtable *pUVar7;
  uint extraout_EDX_19;
  uint extraout_EDX_20;
  uint extraout_EDX_21;
  uint extraout_EDX_22;
  uint uVar8;
  FrontendDisplayDimensionPixels height;
  FrontendDisplayDimensionPixels extraout_EDX_23;
  FrontendDisplayDimensionPixels extraout_EDX_24;
  FrontendDisplayDimensionPixels extraout_EDX_25;
  FrontendDisplayDimensionPixels extraout_EDX_26;
  FrontendDisplayDimensionPixels extraout_EDX_27;
  FrontendDisplayDimensionPixels extraout_EDX_28;
  FrontendDisplayDimensionPixels extraout_EDX_29;
  FrontendDisplayDimensionPixels extraout_EDX_30;
  FrontendDisplayDimensionPixels FVar9;
  dword extraout_EDX_31;
  bool bVar10;
  UiNodeBase *pUVar11;
  UiNodeFlags *pUStack_58;
  sdword *psStack_54;
  sdword *psStack_50;
  sdword *psStack_4c;
  sdword *psStack_48;
  
  bitsPerPixel = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.bitsPerPixel;
  adapterIndex = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.adapterIndex;
  pUVar11 = displaySettingsRoot->parent;
  while (pUVar11 != (UiNodeBase *)0xffffffff) {
    displaySettingsRoot = displaySettingsRoot->parent;
    pUVar11 = displaySettingsRoot->parent;
  }
  bVar10 = false;
  psStack_48 = &displaySettingsRoot[0xab].topOffset;
  psStack_4c = &displaySettingsRoot[0xac].layoutWidth;
  psStack_50 = &displaySettingsRoot[0xae].left;
  psStack_54 = &displaySettingsRoot[0xaf].bottomOffset;
  pUStack_58 = &displaySettingsRoot[0xb0].nodeFlags;
  DisplayModeTable_ContainsExactModeCf
            (displaySettingsRoot[0xb4].topAnchorQ31,
             g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             height,g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                    persistentSelection.width,
             g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             adapterIndex);
  pUVar11 = displaySettingsRoot;
  if (bVar10) {
    UiNodeList_SuppressActionId(0x201e,displaySettingsRoot);
    FVar4 = extraout_ECX;
    FVar9 = extraout_EDX;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201e,displaySettingsRoot);
    FVar4 = extraout_ECX_00;
    FVar9 = extraout_EDX_00;
  }
  bVar10 = (uint)bitsPerPixel < displaySettingsRoot[0xb4].topAnchorQ31;
  if (bitsPerPixel == displaySettingsRoot[0xb4].topAnchorQ31) {
    pUVar11 = (UiNodeBase *)&displaySettingsRoot[0xb3].leftOffset;
  }
  DisplayModeTable_ContainsExactModeCf
            ((FrontendColorDepthBits)displaySettingsRoot[0xb6].firstChild,FVar9,FVar4,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x201f,displaySettingsRoot);
    FVar4 = extraout_ECX_01;
    FVar9 = extraout_EDX_01;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201f,displaySettingsRoot);
    FVar4 = extraout_ECX_02;
    FVar9 = extraout_EDX_02;
  }
  bVar10 = (uint)bitsPerPixel < displaySettingsRoot[0xb6].firstChild;
  DisplayModeTable_ContainsExactModeCf
            (displaySettingsRoot[0xb7].leftOffset,FVar9,FVar4,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2020,displaySettingsRoot);
    FVar4 = extraout_ECX_03;
    FVar9 = extraout_EDX_03;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2020,displaySettingsRoot);
    FVar4 = extraout_ECX_04;
    FVar9 = extraout_EDX_04;
  }
  bVar10 = (uint)bitsPerPixel < (uint)displaySettingsRoot[0xb7].leftOffset;
  DisplayModeTable_ContainsExactModeCf
            (displaySettingsRoot[0xb8].bottomAnchorQ31,FVar9,FVar4,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2021,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2021,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(4,pUVar11);
  bVar10 = (undefined1 *)0xffffffef < &stack0xffffff98;
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xa5].nodeFlags,displaySettingsRoot[0xa5].layoutHeight
             ,adapterIndex);
  pUVar11 = displaySettingsRoot;
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2022,displaySettingsRoot);
    uVar3 = extraout_ECX_05;
    UVar5 = extraout_EDX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2022,displaySettingsRoot);
    uVar3 = extraout_ECX_06;
    UVar5 = extraout_EDX_06;
  }
  bVar10 = uVar3 < (uint)displaySettingsRoot[0xa5].layoutHeight;
  if ((uVar3 == displaySettingsRoot[0xa5].layoutHeight) &&
     (bVar10 = UVar5 < displaySettingsRoot[0xa5].nodeFlags,
     UVar5 == displaySettingsRoot[0xa5].nodeFlags)) {
    pUVar11 = (UiNodeBase *)&displaySettingsRoot[0xa4].leftAnchorQ31;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xa7].right,displaySettingsRoot[0xa7].top,adapterIndex
            );
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2023,displaySettingsRoot);
    uVar3 = extraout_ECX_07;
    uVar8 = extraout_EDX_07;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2023,displaySettingsRoot);
    uVar3 = extraout_ECX_08;
    uVar8 = extraout_EDX_08;
  }
  bVar10 = uVar3 < (uint)displaySettingsRoot[0xa7].top;
  if (uVar3 == displaySettingsRoot[0xa7].top) {
    bVar10 = uVar8 < (uint)displaySettingsRoot[0xa7].right;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xa8].topAnchorQ31,
             displaySettingsRoot[0xa8].leftAnchorQ31,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2024,displaySettingsRoot);
    uVar3 = extraout_ECX_09;
    uVar8 = extraout_EDX_09;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2024,displaySettingsRoot);
    uVar3 = extraout_ECX_10;
    uVar8 = extraout_EDX_10;
  }
  bVar10 = uVar3 < displaySettingsRoot[0xa8].leftAnchorQ31;
  if (uVar3 == displaySettingsRoot[0xa8].leftAnchorQ31) {
    bVar10 = uVar8 < displaySettingsRoot[0xa8].topAnchorQ31;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,(FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].firstChild,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].nextSibling,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2025,displaySettingsRoot);
    pUVar2 = extraout_ECX_11;
    pUVar6 = extraout_EDX_11;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2025,displaySettingsRoot);
    pUVar2 = extraout_ECX_12;
    pUVar6 = extraout_EDX_12;
  }
  bVar10 = pUVar2 < displaySettingsRoot[0xaa].nextSibling;
  if (pUVar2 == displaySettingsRoot[0xaa].nextSibling) {
    bVar10 = pUVar6 < displaySettingsRoot[0xaa].firstChild;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xab].leftOffset,displaySettingsRoot[0xab].bottom,
             adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2026,displaySettingsRoot);
    uVar3 = extraout_ECX_13;
    uVar8 = extraout_EDX_13;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2026,displaySettingsRoot);
    uVar3 = extraout_ECX_14;
    uVar8 = extraout_EDX_14;
  }
  bVar10 = uVar3 < (uint)displaySettingsRoot[0xab].bottom;
  if (uVar3 == displaySettingsRoot[0xab].bottom) {
    bVar10 = uVar8 < (uint)displaySettingsRoot[0xab].leftOffset;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xac].bottomAnchorQ31,
             displaySettingsRoot[0xac].rightAnchorQ31,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2027,displaySettingsRoot);
    uVar3 = extraout_ECX_15;
    uVar8 = extraout_EDX_15;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2027,displaySettingsRoot);
    uVar3 = extraout_ECX_16;
    uVar8 = extraout_EDX_16;
  }
  bVar10 = uVar3 < displaySettingsRoot[0xac].rightAnchorQ31;
  if (uVar3 == displaySettingsRoot[0xac].rightAnchorQ31) {
    bVar10 = uVar8 < displaySettingsRoot[0xac].bottomAnchorQ31;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,(FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].vtable,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].parent,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2028,displaySettingsRoot);
    pUVar2 = extraout_ECX_17;
    pUVar7 = extraout_EDX_17;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2028,displaySettingsRoot);
    pUVar2 = extraout_ECX_18;
    pUVar7 = extraout_EDX_18;
  }
  bVar10 = pUVar2 < displaySettingsRoot[0xae].parent;
  if (pUVar2 == displaySettingsRoot[0xae].parent) {
    bVar10 = pUVar7 < displaySettingsRoot[0xae].vtable;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xaf].rightOffset,displaySettingsRoot[0xaf].topOffset,
             adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2029,displaySettingsRoot);
    uVar3 = extraout_ECX_19;
    uVar8 = extraout_EDX_19;
  }
  else {
    UiNodeList_UnsuppressActionId(0x2029,displaySettingsRoot);
    uVar3 = extraout_ECX_20;
    uVar8 = extraout_EDX_20;
  }
  bVar10 = uVar3 < (uint)displaySettingsRoot[0xaf].topOffset;
  if (uVar3 == displaySettingsRoot[0xaf].topOffset) {
    bVar10 = uVar8 < (uint)displaySettingsRoot[0xaf].rightOffset;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xb0].layoutHeight,
             displaySettingsRoot[0xb0].layoutWidth,adapterIndex);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202a,displaySettingsRoot);
    uVar3 = extraout_ECX_21;
    uVar8 = extraout_EDX_21;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202a,displaySettingsRoot);
    uVar3 = extraout_ECX_22;
    uVar8 = extraout_EDX_22;
  }
  bVar10 = uVar3 < (uint)displaySettingsRoot[0xb0].layoutWidth;
  if (uVar3 == displaySettingsRoot[0xb0].layoutWidth) {
    bVar10 = uVar8 < (uint)displaySettingsRoot[0xb0].layoutHeight;
  }
  DisplayModeTable_ContainsExactModeCf
            (bitsPerPixel,displaySettingsRoot[0xb2].top,displaySettingsRoot[0xb2].left,adapterIndex)
  ;
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202b,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202b,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(10,pUVar11);
  bVar10 = (undefined1 *)0xffffffd7 < &pUStack_58;
  DisplayModeTable_ContainsExactModeCf(bitsPerPixel,height,width,0);
  pUVar11 = displaySettingsRoot;
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202c,displaySettingsRoot);
    FVar4 = extraout_ECX_23;
    FVar9 = extraout_EDX_23;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202c,displaySettingsRoot);
    FVar4 = extraout_ECX_24;
    FVar9 = extraout_EDX_24;
  }
  bVar10 = false;
  if (adapterIndex == 0) {
    pUVar11 = (UiNodeBase *)&displaySettingsRoot[0x9c].topAnchorQ31;
  }
  psStack_48 = (sdword *)0x54b5f9;
  DisplayModeTable_ContainsExactModeCf(bitsPerPixel,FVar9,FVar4,1);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202d,displaySettingsRoot);
    FVar4 = extraout_ECX_25;
    FVar9 = extraout_EDX_25;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202d,displaySettingsRoot);
    FVar4 = extraout_ECX_26;
    FVar9 = extraout_EDX_26;
  }
  bVar10 = adapterIndex == 0;
  psStack_48 = (sdword *)0x54b629;
  DisplayModeTable_ContainsExactModeCf(bitsPerPixel,FVar9,FVar4,2);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202e,displaySettingsRoot);
    FVar4 = extraout_ECX_27;
    FVar9 = extraout_EDX_27;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202e,displaySettingsRoot);
    FVar4 = extraout_ECX_28;
    FVar9 = extraout_EDX_28;
  }
  bVar10 = (uint)adapterIndex < 2;
  psStack_48 = (sdword *)0x54b659;
  DisplayModeTable_ContainsExactModeCf(bitsPerPixel,FVar9,FVar4,3);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x202f,displaySettingsRoot);
    FVar4 = extraout_ECX_29;
    FVar9 = extraout_EDX_29;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202f,displaySettingsRoot);
    FVar4 = extraout_ECX_30;
    FVar9 = extraout_EDX_30;
  }
  bVar10 = (uint)adapterIndex < 3;
  psStack_48 = (sdword *)0x54b689;
  DisplayModeTable_ContainsExactModeCf(bitsPerPixel,FVar9,FVar4,4);
  if (bVar10) {
    UiNodeList_SuppressActionId(0x2030,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2030,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(5,pUVar11);
  dVar1 = PersistentSettings_ReadDword(1,0);
  if ((((dVar1 == adapterIndex) &&
       (dVar1 = PersistentSettings_ReadDword(0x280,4), dVar1 == extraout_ECX_31)) &&
      (dVar1 = PersistentSettings_ReadDword(0x1e0,8), dVar1 == extraout_EDX_31)) &&
     (dVar1 = PersistentSettings_ReadDword(0x10,0xc), dVar1 == bitsPerPixel)) {
    UiNodeList_SuppressActionId(0x2031,displaySettingsRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(0x2031,displaySettingsRoot);
  return;
}
