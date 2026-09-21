/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/settings.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/settings.h>

/* Implementation ownership: ui/ingame/settings. */

/* Address: 0x0056AB50.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[32]@005624A0. Queued UI action handler
   for INGAME_PAGE10[32] (0x1020). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists].
*/
void __thandor_preserve_eax InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source)

{
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe5].firstChild);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe5].firstChild);
  return;
}


/* Address: 0x0056AB90.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[29]@005624A0. Queued UI action handler
   for INGAME_PAGE10[29] (0x101D). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists],
   InGameCommand150_HandlePlayerDepartureAndOwnership [ui/ingame/commands],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax InGameSettingsAction_CloseAndDepartPlayerMode0(UiNodeBase *source)

{
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe3].topOffset);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe3].topOffset);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand150_HandlePlayerDepartureAndOwnership(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x150,0,0,0);
  }
  return;
}


/* Address: 0x0056ABF0.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[30]@005624A0. Queued UI action handler
   for INGAME_PAGE10[30] (0x101E). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists],
   InGameCommand150_HandlePlayerDepartureAndOwnership [ui/ingame/commands],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax InGameSettingsAction_CloseAndDepartPlayerMode1(UiNodeBase *source)

{
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe3].topOffset);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe3].topOffset);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand150_HandlePlayerDepartureAndOwnership(g_LocalPlayerRuntimeId,0,0,1);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x150,0,0,1);
  }
  return;
}


/* Address: 0x0056C5E0.
   Ownership: ui/ingame/settings.
   Purpose: Finds the UI root, clears the shared settings toggle at root+0x4388, and delegates to
   UiAction1003_ToggleInGameSettingsPage. This handler is referenced by action-table entry 0x1047. Queued UI action
   handler for INGAME_PAGE12[1] (0x1201). Return datatype is preserved for non-queue direct callers.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists].
*/
void __thandor_preserve_eax InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source)

{
  UiNodeBase *parentCursor;
  
  parentCursor = source->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentCursor = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe3].topOffset);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe3].topOffset);
  return;
}


/* Address: 0x0056C620.
   Ownership: ui/ingame/settings.
   Purpose: Finds the UI root, sets the shared settings toggle at root+0x4388, and delegates to
   UiAction1003_ToggleInGameSettingsPage. This handler is referenced by action-table entry 0x105E. Queued UI action
   handler for INGAME_PAGE12[24] (0x1218). Return datatype is preserved for non-queue direct callers.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists].
*/
void __thandor_preserve_eax InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source)

{
  UiNodeBase *parentCursor;
  
  parentCursor = source->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentCursor = source->parent;
  }
  UiSelectableControl_SetSelected(1,(UiSelectableControl *)&source[0xe3].topOffset);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe3].topOffset);
  return;
}


/* Address: 0x0055F520.
   Ownership: ui/ingame/settings.
   Purpose: Handles in game simulation speed adjust player and recompute minimum ticks.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          int stepDelta)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  InGameSimulationStepBatchTicks IVar3;
  
  IVar3 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks + stepDelta;
  if ((IVar3 != 0) && (IVar3 < 6)) {
    g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks = IVar3;
    FVar1 = g_FrontendPlayerRuntimeBlockCount;
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    do {
      if (g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->simulationStepTicks <
          IVar3) {
        IVar3 = g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->simulationStepTicks;
      }
      pFVar2 = pFVar2 + 1;
      FVar1 = FVar1 - 1;
      g_InGameSimulationStepTicks = IVar3;
    } while (FVar1 != 0);
  }
  return;
}


/* Address: 0x0056AA90.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[33]@005624A0. Queued UI action handler
   for INGAME_PAGE10[33] (0x1021). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab0(UiNodeBase *sourceNode)

{
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&sourceNode[3].bottomAnchorQ31);
  return;
}


/* Address: 0x0056AAD0.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[34]@005624A0. Queued UI action handler
   for INGAME_PAGE10[34] (0x1022). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab1(UiNodeBase *sourceNode)

{
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&sourceNode[2].rightOffset);
  return;
}


/* Address: 0x0056AB10.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[35]@005624A0. Queued UI action handler
   for INGAME_PAGE10[35] (0x1023). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab2(UiNodeBase *sourceNode)

{
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&sourceNode[1].top);
  return;
}


/* Address: 0x0056BAF0.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x105C, 0x1134, and 0x1216. Updates bit 2 of optionFlags40 and the
   corresponding in-game runtime state. The original label remains unresolved. Queued UI action handler for
   INGAME_PAGE12[22] (0x1216). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiPageStack_SetActiveIndex [ui/controls/layout], UiContainer_LayoutChildren
   [ui/controls/layout], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  sdword *stack;
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  stack = &control[0x4e].base.layoutHeight;
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 4;
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)stack);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5c].base.leftAnchorQ31);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5f].base.nodeFlags);
    control[-0x58].base.top = 0;
  }
  else {
    value = dVar1 & 0xfffffffb;
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)stack);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5c].base.leftAnchorQ31);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5f].base.nodeFlags);
    control[-0x58].base.top = control[0x50].base.top;
  }
  UiContainer_LayoutChildren((UiNodeBase *)&control[-0x77].base.bottomAnchorQ31);
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0056BBB0.
   Ownership: ui/ingame/settings.
   Purpose: Persists control+0x58 as cameraScrollStep at settings offset 0x48. Queued UI action handler for
   INGAME_PAGE12[23] (0x1217). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentSettings_WriteDword(control->boundValue,0x48);
  return;
}


/* Address: 0x0056BBD0.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x1058, 0x1130, and 0x1212. Updates bit 0 of optionFlags40 and its
   corresponding in-game runtime value. The original label remains unresolved. Queued UI action handler for
   INGAME_PAGE12[18] (0x1212). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 1;
    control[0x159].base.layoutWidth = 0x800;
  }
  else {
    value = dVar1 & 0xfffffffe;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0056BC20.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x1059, 0x1131, and 0x1213. Updates bit 1 of optionFlags40 and its
   corresponding in-game runtime value. The original label remains unresolved. Queued UI action handler for
   INGAME_PAGE12[19] (0x1213). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  dword dVar1;
  PersistentSettingsDwordValue value;
  bool bVar2;
  
  dVar1 = PersistentSettings_ReadDword(0,0x40);
  bVar2 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar2) {
    value = dVar1 | 2;
    control[0x158].base.rightAnchorQ31 = 0x2000;
  }
  else {
    value = dVar1 & 0xfffffffd;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0056BC70.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x105A, 0x1132, and 0x1214. Updates bit 0 of optionFlags5C and runtime bit
   0x40000000. Selecting it suppresses paired action 0x1215. Queued UI action handler for INGAME_PAGE12[20]
   (0x1214). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  UiAnchorFractionQ31 *pUVar1;
  dword dVar2;
  PersistentSettingsDwordValue value;
  bool bVar3;
  
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  bVar3 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar3) {
    value = dVar2 | 1;
    pUVar1 = &control[-0x61].base.bottomAnchorQ31;
    *pUVar1 = *pUVar1 | 0x40000000;
    UiNodeList_SuppressActionId(0x1215,(control->base).parent);
  }
  else {
    value = dVar2 & 0xfffffffe;
    pUVar1 = &control[-0x61].base.bottomAnchorQ31;
    *pUVar1 = *pUVar1 & 0xbfffffff;
    UiNodeList_UnsuppressActionId(0x1215,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0056BCF0.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x105B, 0x1133, and 0x1215. Updates bit 1 of optionFlags5C and runtime bit
   0x80000000. Selecting it suppresses paired action 0x1214. Queued UI action handler for INGAME_PAGE12[21]
   (0x1215). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  UiAnchorFractionQ31 *pUVar1;
  dword dVar2;
  PersistentSettingsDwordValue value;
  bool bVar3;
  
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  bVar3 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar3) {
    value = dVar2 | 2;
    pUVar1 = &control[-0x62].base.leftAnchorQ31;
    *pUVar1 = *pUVar1 | 0x80000000;
    UiNodeList_SuppressActionId(0x1214,(control->base).parent);
  }
  else {
    value = dVar2 & 0xfffffffd;
    pUVar1 = &control[-0x62].base.leftAnchorQ31;
    *pUVar1 = *pUVar1 & 0x7fffffff;
    UiNodeList_UnsuppressActionId(0x1214,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0056BD70.
   Ownership: ui/ingame/settings.
   Purpose: Registered under UI actions 0x1061, 0x1139, and 0x121B. Updates bit 2 of optionFlags5C and runtime bit
   0x04000000. The original label remains unresolved. Queued UI action handler for INGAME_PAGE12[27] (0x121B).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  sdword *psVar1;
  dword dVar2;
  PersistentSettingsDwordValue value;
  bool bVar3;
  
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  bVar3 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar3) {
    value = dVar2 | 4;
    psVar1 = &control[-99].base.topOffset;
    *psVar1 = *psVar1 | 0x4000000;
  }
  else {
    value = dVar2 & 0xfffffffb;
    psVar1 = &control[-99].base.topOffset;
    *psVar1 = *psVar1 & 0xfbffffff;
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0056C6D0.
   Ownership: ui/ingame/settings.
   Purpose: Recovered action-table target INGAME_PAGE12[2] (0x1202).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameGraphicsSettings_OpenAndSynchronize(InGameGraphicsRuntimeSettingsPageState12D0 *source)

{
  InGameGraphicsRuntimeSettingsPageState12D0 *firstNode;
  dword dVar1;
  dword dVar2;
  int iVar3;
  UiNodeBase *pUVar4;
  
  UiPageStack_SetActiveIndex(6,(UiPageStackControl *)(source[-2].reserved4C_CBB + 0xb14));
  dVar1 = PersistentSettings_ReadDword(1,0x1c);
  UiSelectableControl_SetSelected(dVar1,&source->shadingEnabledControl);
  pUVar4 = (source->base).parent;
  firstNode = source;
  while (pUVar4 != (UiNodeBase *)0xffffffff) {
    firstNode = (InGameGraphicsRuntimeSettingsPageState12D0 *)(firstNode->base).parent;
    pUVar4 = (firstNode->base).parent;
  }
  if (dVar1 == 0) {
    UiNodeList_SuppressActionId(0x1205,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,&firstNode->base);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(0x1207,&firstNode->base);
  }
  else {
    UiNodeList_SuppressActionId(0x1207,&firstNode->base);
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


/* Address: 0x0056C860.
   Ownership: ui/ingame/settings.
   Purpose: Recovered action-table target INGAME_PAGE12[3] (0x1203).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameAudioSettings_OpenAndSynchronize(InGamePersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeBase *pUVar1;
  dword dVar2;
  dword dVar3;
  
  UiPageStack_SetActiveIndex(7,&ADJ(settingsSourceNode).settingsPageStack);
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  UiSelectableControl_SetSelected(dVar2 & 1,&ADJ(settingsSourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(dVar2 & 2,&ADJ(settingsSourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(dVar2 & 4,&ADJ(settingsSourceNode)->reverseStereoControl);
  dVar3 = PersistentSettings_ReadDword(0x8000,0x24);
  (ADJ(settingsSourceNode)->soundEffectsGainControl).currentValue = dVar3;
  dVar3 = PersistentSettings_ReadDword(0x8000,0x28);
  (ADJ(settingsSourceNode)->movieDefaultAudioGainControl).currentValue = dVar3;
  dVar3 = PersistentSettings_ReadDword(0x8000,0x4c);
  (ADJ(settingsSourceNode)->movieAlternateAudioGainControl).currentValue = dVar3;
  dVar3 = PersistentSettings_ReadDword(0x8000,0x2c);
  (ADJ(settingsSourceNode)->musicGainControl).currentValue = dVar3;
  pUVar1 = settingsSourceNode->parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    pUVar1 = settingsSourceNode->parent;
  }
  if ((dVar2 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x120c,settingsSourceNode);
    UiNodeList_SuppressActionId(0x121a,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x120c,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x121a,settingsSourceNode);
  }
  if ((dVar2 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,settingsSourceNode);
  }
  if ((dVar2 & 3) == 0) {
    UiNodeList_SuppressActionId(0x120a,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,settingsSourceNode);
  }
  return;
}


/* Address: 0x0056C9C0.
   Ownership: ui/ingame/settings.
   Purpose: Updates the in-game Shading toggle, persists shadingEnabled at settings offset 0x1C, and mirrors it to
   runtime bit 0x00020000. Queued UI action handler for INGAME_PAGE12[4] (0x1204). Return datatype is preserved for
   non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx InGameShadingSettings_SetEnabled(UiSelectableControl *control)

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
    UiNodeList_SuppressActionId(0x1205,&control->base);
    control[0x1f].actionId = control[0x1f].actionId & 0xfffdffff;
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,&control->base);
    control[0x1f].actionId = control[0x1f].actionId | 0x20000;
  }
  PersistentSettings_WriteDword(bVar1 & 1,0x1c);
  return;
}


/* Address: 0x0056CA30.
   Ownership: ui/ingame/settings.
   Purpose: Rebuilds generated shading resources for the selected Shading Level and persists the accepted 0x10,
   0x14, and 0x18 tuple. Queued UI action handler for INGAME_PAGE12[5] (0x1205). Return datatype is preserved for
   non-queue direct callers.
   Cross-module calls: GraphicsShadingRuntime_Shutdown [graphics/render/shading],
   GraphicsShadingRuntime_InitializeGeneratedTextureCf [graphics/render/shading], PersistentSettings_WriteDword
   [core/settings/persistent], UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  uint subresourceCount;
  int iVar2;
  dword textureDimension;
  dword gridHalfSize;
  dword subresourceCount_00;
  PersistentSettingsDwordValue gridHalfSize_00;
  PersistentSettingsDwordValue textureDimension_00;
  UiNodeBase *selectedControl;
  StatusValueEaxCf5 SVar3;
  FatalErrorEaxCf5 FVar4;
  uint value;
  
  subresourceCount = (uint)control[1].base.left >> 2;
  value = subresourceCount;
  GraphicsShadingRuntime_Shutdown();
  SVar3 = GraphicsShadingRuntime_InitializeGeneratedTextureCf
                    (subresourceCount,gridHalfSize_00,textureDimension_00);
  FVar4 = (*g_FatalErrorRuntimeDispatchCf)(SVar3.valueOrError,SVar3.carry);
  if (!FVar4.carry) {
    PersistentSettings_WriteDword(textureDimension_00,0x14);
    PersistentSettings_WriteDword(gridHalfSize_00,0x10);
    PersistentSettings_WriteDword(value,0x18);
    iVar2 = value << 2;
    pUVar1 = (control->base).parent;
    if (gridHalfSize_00 == 0x20) {
      selectedControl = (UiNodeBase *)&pUVar1[1].parent;
      if (iVar2 == 0x40) {
        selectedControl = (UiNodeBase *)&pUVar1[2].topOffset;
      }
      else if (iVar2 == 0x80) {
        selectedControl = (UiNodeBase *)&pUVar1[3].layoutWidth;
      }
    }
    else if (gridHalfSize_00 == 0x40) {
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
  textureDimension = PersistentSettings_ReadDword(0x40,0x14);
  gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
  subresourceCount_00 = PersistentSettings_ReadDword(0x10,0x18);
  GraphicsShadingRuntime_InitializeGeneratedTextureCf
            (subresourceCount_00,gridHalfSize,textureDimension);
  return;
}


/* Address: 0x0056CB60.
   Ownership: ui/ingame/settings.
   Purpose: Persists the custom-slider Q8 model-LOD depth threshold at settings offset 0x34 and mirrors it to
   g_ModelLodDepthThresholdQ8. Queued UI action handler for INGAME_PAGE12[6] (0x1206). Return datatype is preserved
   for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x34);
  g_ModelLodDepthThresholdQ8 = value;
  return;
}


/* Address: 0x0056CB90.
   Ownership: ui/ingame/settings.
   Purpose: Maps the Texture Quality choices High, Medium, and Low to persisted values 0, 1, and 2 at settings
   offset 0x30, then rebuilds texture staging resources. Queued UI action handler for INGAME_PAGE12[7] (0x1207).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl;
  UiNodeBase *graphicsSettingsRoot;
  
  (*g_GraphicsCursorSetFrame)(6);
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
  g_TextureDownsampleShift = qualityLevel;
  (*g_GraphicsRebuildAllStagingTextures)();
  (*g_GraphicsCursorSetFrame)(0);
  return;
}


/* Address: 0x0056CC20.
   Ownership: ui/ingame/settings.
   Purpose: Updates SOUND_OPTIONS_EFFECTS_ENABLED in the in-game settings screen, stops the active effects test
   voice when disabling, updates related controls, and loads or clears the three effects/movie gain globals. Queued
   UI action handler for INGAME_PAGE12[8] (0x1208). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx
InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  dword dVar2;
  AudioMixerGainQ15 AVar3;
  MovieAudioGainQ15 MVar4;
  MovieAudioGainQ15 MVar5;
  bool bVar6;
  
  bVar6 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (!bVar6) {
    (*g_SoundStopVoice)(g_InGameActiveEffectVoice);
    g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
  }
  bVar6 = bVar6;
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword((uint)bVar6 | dVar2 & 0xfffffffe,0x20);
  pUVar1 = (control->base).parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar1 = (control->base).parent;
  }
  if (bVar6) {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,&control->base);
    UiNodeList_UnsuppressActionId(0x121a,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,&control->base);
    UiNodeList_SuppressActionId(0x121a,&control->base);
  }
  if ((dVar2 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,&control->base);
  }
  if (bVar6 == 0 && (dVar2 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,&control->base);
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


/* Address: 0x0056CD80.
   Ownership: ui/ingame/settings.
   Purpose: Updates SOUND_OPTIONS_MUSIC_ENABLED in the in-game settings screen, stops the active looping voice when
   disabling, and updates related controls. Queued UI action handler for INGAME_PAGE12[9] (0x1209). Return datatype
   is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx
InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  UiNodeBase *pUVar1;
  dword dVar2;
  uint uVar3;
  bool bVar4;
  
  uVar3 = 0;
  bVar4 = (bool)UiSelectableControl_IsSelectedCf(control);
  if (bVar4) {
    uVar3 = 2;
  }
  else {
    (*g_SoundStopVoice)(g_InGameActiveMusicVoice);
    g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
    g_InGameMusicEnabled = 1;
  }
  dVar2 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword(uVar3 | dVar2 & 0xfffffffd,0x20);
  pUVar1 = (control->base).parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar1 = (control->base).parent;
  }
  if ((dVar2 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,&control->base);
    UiNodeList_SuppressActionId(0x121a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,&control->base);
    UiNodeList_UnsuppressActionId(0x121a,&control->base);
  }
  if (uVar3 == 0) {
    UiNodeList_SuppressActionId(0x120d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,&control->base);
  }
  if (uVar3 == 0 && (dVar2 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,&control->base);
  }
  return;
}


/* Address: 0x0056CE80.
   Ownership: ui/ingame/settings.
   Purpose: Updates SOUND_OPTIONS_REVERSE_STEREO and writes g_ReverseStereoMask as zero or 0xFFFFFFFF. Queued UI
   action handler for INGAME_PAGE12[10] (0x120A). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameAudioSettings_SetReverseStereo(UiSelectableControl *control)

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


/* Address: 0x0056CED0.
   Ownership: ui/ingame/settings.
   Purpose: Writes soundEffectsGainQ15, updates both effects gain globals, and immediately applies equal left/right
   gains to the active in-game effects test voice. Queued UI action handler for INGAME_PAGE12[11] (0x120B). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  dword effectsGainQ15;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x24);
  g_UiSoundGainQ15 = value;
  g_SoundEffectsGainQ15 = value;
  (*g_SoundSetVoiceGains)(value,value,g_InGameActiveEffectVoice);
  return;
}


/* Address: 0x0056CF10.
   Ownership: ui/ingame/settings.
   Purpose: Writes movieDefaultAudioGainQ15 and updates the movie default-gain global. Queued UI action handler for
   INGAME_PAGE12[12] (0x120C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x28);
  g_MovieDefaultAudioGainQ15 = value;
  return;
}


/* Address: 0x0056CF40.
   Ownership: ui/ingame/settings.
   Purpose: Writes musicGainQ15 and immediately applies equal left/right gains to the active in-game looping music
   voice. Queued UI action handler for INGAME_PAGE12[13] (0x120D). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  dword musicGainQ15;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x2c);
  (*g_SoundSetVoiceGains)(value,value,g_InGameActiveMusicVoice);
  return;
}


/* Address: 0x0056CF70.
   Ownership: ui/ingame/settings.
   Purpose: Writes movieAlternateAudioGainQ15 and updates the alternate movie-gain global used by timed movie
   playback events. Queued UI action handler for INGAME_PAGE12[26] (0x121A). Return datatype is preserved for non-
   queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x4c);
  g_MovieAlternateAudioGainQ15 = value;
  return;
}


/* Address: 0x0056C440.
   Ownership: ui/ingame/settings.
   Purpose: Toggles the in-game settings page from a selectable control. When selected, activates root page-stack
   index 3, marks the root settings state active, synchronizes persistent optionFlags40, optionFlags5C, and
   cameraScrollStep into verified controls, applies mutual-exclusion suppression, and updates
   g_UiCommandRuntimeFlags. When cleared, restores page index 0 and clears the corresponding root/runtime state.
   Queued UI action handler for INGAME_PAGE10[3] (0x1003). Return datatype is preserved for non-queue direct
   callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiSelectableControl_SetSelected [ui/controls/lists], UiKeyboardFocus_ReleaseNode
   [ui/controls/input], PersistentSettings_ReadDword [core/settings/persistent], UiNodeList_SuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle)

{
  UiNodeBase *pUVar1;
  UiSelectableControl *firstNode;
  dword dVar2;
  bool bVar3;
  
  pUVar1 = (settingsToggle->base).parent;
  firstNode = settingsToggle;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    firstNode = (UiSelectableControl *)(firstNode->base).parent;
    pUVar1 = (firstNode->base).parent;
  }
  bVar3 = (bool)UiSelectableControl_IsSelectedCf(settingsToggle);
  if (!bVar3) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(firstNode + 0x24));
    firstNode[0x1f].stateFlags = firstNode[0x1f].stateFlags & 0xfffffff7;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_UiCommandRuntimeFlags & 0x400) == 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffbffe;
      }
      else {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffbbff;
      }
    }
    return;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&firstNode[0xcf].base.top);
  firstNode[0x1f].stateFlags = firstNode[0x1f].stateFlags | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&firstNode[0x1f].base.firstChild);
  UiPageStack_SetActiveIndex(3,(UiPageStackControl *)(firstNode + 0x24));
  dVar2 = PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_SetSelected(dVar2 & 1,(UiSelectableControl *)&firstNode[0x7c].actionId);
  UiSelectableControl_SetSelected(dVar2 & 2,(UiSelectableControl *)&firstNode[0x7e].base.parent);
  UiSelectableControl_SetSelected(dVar2 & 4,(UiSelectableControl *)&firstNode[0x76].base.right);
  dVar2 = PersistentSettings_ReadDword(0,0x5c);
  if ((dVar2 & 1) != 0) {
    UiNodeList_SuppressActionId(0x1215,&firstNode->base);
  }
  UiSelectableControl_SetSelected(dVar2 & 1,(UiSelectableControl *)&firstNode[0x80].base.top);
  if ((dVar2 & 2) != 0) {
    UiNodeList_SuppressActionId(0x1214,&firstNode->base);
  }
  UiSelectableControl_SetSelected(dVar2 & 2,(UiSelectableControl *)&firstNode[0x81].base.leftOffset)
  ;
  dVar2 = PersistentSettings_ReadDword(0x20,0x48);
  firstNode[0x7b].base.layoutWidth = dVar2;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_UiCommandRuntimeFlags & 0x4000) == 0) {
      if ((g_UiCommandRuntimeFlags & 1) != 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x400;
      }
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x4001;
    }
  }
  else {
    UiNodeList_SuppressActionId(0x120e,&firstNode->base);
  }
  return;
}

