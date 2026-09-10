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
undefined4 InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source)

{
  undefined4 in_EAX;
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe5].firstChild);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe5].firstChild);
  return in_EAX;
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
undefined4 InGameSettingsAction_CloseAndDepartPlayerMode0(UiNodeBase *source)

{
  undefined4 in_EAX;
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
  return in_EAX;
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
undefined4 InGameSettingsAction_CloseAndDepartPlayerMode1(UiNodeBase *source)

{
  undefined4 in_EAX;
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
  return in_EAX;
}

/* Address: 0x0056C5E0.
   Ownership: ui/ingame/settings.
   Purpose: Finds the UI root, clears the shared settings toggle at root+0x4388, and delegates to
   UiAction1003_ToggleInGameSettingsPage. This handler is referenced by action-table entry 0x1047. Queued UI action
   handler for INGAME_PAGE12[1] (0x1201). Return datatype is preserved for non-queue direct callers.
   Local calls: InGameSettingsPage_ToggleAndSynchronizeControls.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists].
*/
void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source)

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
void InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source)

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
undefined8
InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar1;
  undefined4 in_EDX;
  FrontendPlayerRuntimeRecord *pFVar2;
  InGameSimulationStepBatchTicks IVar3;
  
  IVar3 = g_SelectionPlayerRuntimeBlockPointers[param_1]->simulationStepTicks + param_4;
  if ((IVar3 != 0) && (IVar3 < 6)) {
    g_SelectionPlayerRuntimeBlockPointers[param_1]->simulationStepTicks = IVar3;
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0056AA90.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[33]@005624A0. Queued UI action handler
   for INGAME_PAGE10[33] (0x1021). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
undefined8 InGameSettingsPage_SelectTab0(UiNodeBase *sourceNode)

{
  undefined4 in_EAX;
  UiPageStackControl *stack;
  undefined4 in_EDX;
  
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(0,stack);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0056AAD0.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[34]@005624A0. Queued UI action handler
   for INGAME_PAGE10[34] (0x1022). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
undefined8 InGameSettingsPage_SelectTab1(UiNodeBase *sourceNode)

{
  undefined4 in_EAX;
  UiPageStackControl *stack;
  undefined4 in_EDX;
  
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(1,stack);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0056AB10.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[35]@005624A0. Queued UI action handler
   for INGAME_PAGE10[35] (0x1023). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
undefined8 InGameSettingsPage_SelectTab2(UiNodeBase *sourceNode)

{
  undefined4 in_EAX;
  UiPageStackControl *stack;
  undefined4 in_EDX;
  
  UiSelectableGroup_SelectExclusive(3,sourceNode);
  UiPageStack_SetActiveIndex(2,stack);
  return CONCAT44(in_EDX,in_EAX);
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
void InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  PersistentSettingsDwordValue value;
  UiPageStackControl *stack;
  UiPageStackControl *stack_00;
  UiPageStackControl *stack_01;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    UiPageStack_SetActiveIndex(1,stack);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5c].base.leftAnchorQ31);
    UiPageStack_SetActiveIndex(0,stack_00);
    control[-0x58].base.top = 0;
  }
  else {
    UiPageStack_SetActiveIndex(0,stack);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x5c].base.leftAnchorQ31);
    UiPageStack_SetActiveIndex(0,stack_01);
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
void InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

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
void InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 1;
    control[0x159].base.layoutWidth = 0x800;
  }
  else {
    value = extraout_ECX & 0xfffffffe;
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
void InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 2;
    control[0x158].base.rightAnchorQ31 = 0x2000;
  }
  else {
    value = extraout_ECX & 0xfffffffd;
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
void InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  UiAnchorFractionQ31 *pUVar1;
  PersistentSettingsDwordValue extraout_ECX;
  PersistentSettingsDwordValue extraout_ECX_00;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    pUVar1 = &control[-0x61].base.bottomAnchorQ31;
    *pUVar1 = *pUVar1 | 0x40000000;
    UiNodeList_SuppressActionId(0x1215,(control->base).parent);
    value = extraout_ECX;
  }
  else {
    pUVar1 = &control[-0x61].base.bottomAnchorQ31;
    *pUVar1 = *pUVar1 & 0xbfffffff;
    UiNodeList_UnsuppressActionId(0x1215,(control->base).parent);
    value = extraout_ECX_00;
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
void InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  UiAnchorFractionQ31 *pUVar1;
  PersistentSettingsDwordValue extraout_ECX;
  PersistentSettingsDwordValue extraout_ECX_00;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    pUVar1 = &control[-0x62].base.leftAnchorQ31;
    *pUVar1 = *pUVar1 | 0x80000000;
    UiNodeList_SuppressActionId(0x1214,(control->base).parent);
    value = extraout_ECX;
  }
  else {
    pUVar1 = &control[-0x62].base.leftAnchorQ31;
    *pUVar1 = *pUVar1 & 0x7fffffff;
    UiNodeList_UnsuppressActionId(0x1214,(control->base).parent);
    value = extraout_ECX_00;
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
void InGameGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  sdword *psVar1;
  uint extraout_ECX;
  PersistentSettingsDwordValue value;
  undefined1 in_CF;
  
  PersistentSettings_ReadDword(0,0x5c);
  UiSelectableControl_IsSelectedCf(control);
  if ((bool)in_CF) {
    value = extraout_ECX | 4;
    psVar1 = &control[-99].base.topOffset;
    *psVar1 = *psVar1 | 0x4000000;
  }
  else {
    value = extraout_ECX & 0xfffffffb;
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
void InGameGraphicsSettings_OpenAndSynchronize(InGameGraphicsRuntimeSettingsPageState12D0 *source)

{
  InGameGraphicsRuntimeSettingsPageState12D0 *firstNode;
  dword dVar1;
  UiNodeBase *extraout_EAX;
  UiNodeBase *extraout_EAX_00;
  UiNodeBase *pUVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  
  UiPageStack_SetActiveIndex(6,(UiPageStackControl *)(source[-2].reserved4C_CBB + 0xb14));
  dVar1 = PersistentSettings_ReadDword(1,0x1c);
  UiSelectableControl_SetSelected(dVar1,&source->shadingEnabledControl);
  pUVar2 = (source->base).parent;
  firstNode = source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    firstNode = (InGameGraphicsRuntimeSettingsPageState12D0 *)(firstNode->base).parent;
    pUVar2 = (firstNode->base).parent;
  }
  if (extraout_ECX == 0) {
    UiNodeList_SuppressActionId(0x1205,&firstNode->base);
    pUVar2 = extraout_EAX_00;
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,&firstNode->base);
    pUVar2 = extraout_EAX;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(0x1207,pUVar2);
  }
  else {
    UiNodeList_SuppressActionId(0x1207,pUVar2);
  }
  PersistentSettings_ReadDword(0x20,0x10);
  dVar1 = PersistentSettings_ReadDword(0x10,0x18);
  iVar3 = dVar1 * 4;
  if (extraout_ECX_00 == 0x20) {
    pUVar2 = (UiNodeBase *)&source->shadingResolutionRows;
    if (iVar3 == 0x40) {
      pUVar2 = (UiNodeBase *)((source->shadingResolutionRows).rows + 1);
    }
    else if (iVar3 == 0x80) {
      pUVar2 = (UiNodeBase *)((source->shadingResolutionRows).rows + 2);
    }
  }
  else if (extraout_ECX_00 == 0x40) {
    pUVar2 = (UiNodeBase *)((source->shadingResolutionRows).rows + 3);
    if (iVar3 == 0x80) {
      pUVar2 = (UiNodeBase *)((source->shadingResolutionRows).rows + 4);
    }
  }
  else {
    pUVar2 = (UiNodeBase *)((source->shadingResolutionRows).rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,pUVar2);
  dVar1 = PersistentSettings_ReadDword(1,0x30);
  if (dVar1 == 0) {
    pUVar2 = (UiNodeBase *)((source->textureResolutionRows).rows + 2);
  }
  else if (dVar1 == 1) {
    pUVar2 = (UiNodeBase *)((source->textureResolutionRows).rows + 1);
  }
  else {
    pUVar2 = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,pUVar2);
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
void InGameAudioSettings_OpenAndSynchronize
               (InGamePersistentSettingsPageSourceNodePtr settingsSourceNode)

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
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint uVar3;
  
  UiPageStack_SetActiveIndex(7,&ADJ(settingsSourceNode).settingsPageStack);
  dVar1 = PersistentSettings_ReadDword(3,0x20);
  UiSelectableControl_SetSelected(dVar1 & 1,&ADJ(settingsSourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(extraout_ECX & 2,&ADJ(settingsSourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected
            (extraout_ECX_00 & 4,&ADJ(settingsSourceNode)->reverseStereoControl);
  dVar1 = PersistentSettings_ReadDword(0x8000,0x24);
  (ADJ(settingsSourceNode)->soundEffectsGainControl).currentValue = dVar1;
  dVar1 = PersistentSettings_ReadDword(0x8000,0x28);
  (ADJ(settingsSourceNode)->movieDefaultAudioGainControl).currentValue = dVar1;
  dVar1 = PersistentSettings_ReadDword(0x8000,0x4c);
  (ADJ(settingsSourceNode)->movieAlternateAudioGainControl).currentValue = dVar1;
  dVar1 = PersistentSettings_ReadDword(0x8000,0x2c);
  (ADJ(settingsSourceNode)->musicGainControl).currentValue = dVar1;
  pUVar2 = settingsSourceNode->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    pUVar2 = settingsSourceNode->parent;
  }
  if ((extraout_ECX_01 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x120c,firstNode_01);
    UiNodeList_SuppressActionId(0x121a,firstNode_02);
    pUVar2 = extraout_EAX_00;
    uVar3 = extraout_ECX_03;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x120c,firstNode);
    UiNodeList_UnsuppressActionId(0x121a,firstNode_00);
    pUVar2 = extraout_EAX;
    uVar3 = extraout_ECX_02;
  }
  if ((uVar3 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_02;
    uVar3 = extraout_ECX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_01;
    uVar3 = extraout_ECX_04;
  }
  if ((uVar3 & 3) == 0) {
    UiNodeList_SuppressActionId(0x120a,pUVar2);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,pUVar2);
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
void InGameShadingSettings_SetEnabled(UiSelectableControl *control)

{
  int extraout_EAX;
  int extraout_EAX_00;
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
    UiNodeList_SuppressActionId(0x1205,&control->base);
    *(uint *)(extraout_EAX_00 + 0xa7c) = *(uint *)(extraout_EAX_00 + 0xa7c) & 0xfffdffff;
    value = extraout_ECX_00;
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,&control->base);
    *(uint *)(extraout_EAX + 0xa7c) = *(uint *)(extraout_EAX + 0xa7c) | 0x20000;
    value = extraout_ECX;
  }
  PersistentSettings_WriteDword(value,0x1c);
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
void InGameShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  uint uVar1;
  UiNodeBase *pUVar2;
  uint uVar3;
  GraphicsAssetSubresourceCount subresourceCount;
  PersistentSettingsDwordValue value;
  int extraout_EAX;
  int iVar4;
  dword textureDimension;
  dword gridHalfSize;
  dword subresourceCount_00;
  GraphicsPixelDimension gridHalfSize_00;
  PersistentSettingsDwordValue value_00;
  int extraout_ECX;
  GraphicsPixelDimension textureDimension_00;
  PersistentSettingsDwordValue value_01;
  UiNodeBase *selectedControl;
  undefined1 uVar5;
  
  uVar1 = control[1].base.left;
  uVar3 = uVar1 >> 2;
  uVar5 = (uVar1 >> 1 & 1) != 0;
  subresourceCount = GraphicsShadingRuntime_Shutdown();
  GraphicsShadingRuntime_InitializeGeneratedTextureCf
            (subresourceCount,gridHalfSize_00,textureDimension_00);
  (*g_FatalErrorRuntimeDispatchCf)(uVar3);
  if (!(bool)uVar5) {
    PersistentSettings_WriteDword(value_01,0x14);
    PersistentSettings_WriteDword(value_00,0x10);
    PersistentSettings_WriteDword(value,0x18);
    iVar4 = extraout_EAX * 4;
    pUVar2 = (control->base).parent;
    if (extraout_ECX == 0x20) {
      selectedControl = (UiNodeBase *)&pUVar2[1].parent;
      if (iVar4 == 0x40) {
        selectedControl = (UiNodeBase *)&pUVar2[2].topOffset;
      }
      else if (iVar4 == 0x80) {
        selectedControl = (UiNodeBase *)&pUVar2[3].layoutWidth;
      }
    }
    else if (extraout_ECX == 0x40) {
      selectedControl = (UiNodeBase *)&pUVar2[5].left;
      if (iVar4 == 0x80) {
        selectedControl = (UiNodeBase *)&pUVar2[6].bottomOffset;
      }
    }
    else {
      selectedControl = (UiNodeBase *)&pUVar2[7].nodeFlags;
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
void InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  sdword extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x34);
  g_ModelLodDepthThresholdQ8 = extraout_EAX;
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
void InGameTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentSettingsDwordValue value;
  dword extraout_EDX;
  UiNodeBase *unaff_EDI;
  UiNodeBase *graphicsSettingsRoot;
  
  (*g_GraphicsCursorSetFrame)(6);
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
  g_TextureDownsampleShift = extraout_EDX;
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
void InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

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
  bool bVar7;
  
  bVar7 = false;
  UiSelectableControl_IsSelectedCf(control);
  if (!bVar7) {
    (*g_SoundStopVoice)(g_InGameActiveEffectVoice);
    g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
  }
  dVar1 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword(extraout_ECX | dVar1 & 0xfffffffe,0x20);
  pUVar2 = (control->base).parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar2 = (control->base).parent;
  }
  if ((extraout_ECX_00 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,firstNode_01);
    UiNodeList_SuppressActionId(0x121a,firstNode_02);
    pUVar2 = extraout_EAX_00;
    uVar6 = extraout_ECX_02;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,firstNode);
    UiNodeList_UnsuppressActionId(0x121a,firstNode_00);
    pUVar2 = extraout_EAX;
    uVar6 = extraout_ECX_01;
  }
  if ((uVar6 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_02;
    uVar6 = extraout_ECX_04;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_01;
    uVar6 = extraout_ECX_03;
  }
  if ((uVar6 & 3) == 0) {
    UiNodeList_SuppressActionId(0x120a,pUVar2);
    uVar6 = extraout_ECX_06;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,pUVar2);
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

/* Address: 0x0056CD80.
   Ownership: ui/ingame/settings.
   Purpose: Updates SOUND_OPTIONS_MUSIC_ENABLED in the in-game settings screen, stops the active looping voice when
   disabling, and updates related controls. Queued UI action handler for INGAME_PAGE12[9] (0x1209). Return datatype
   is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control)

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
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint uVar3;
  bool bVar4;
  
  bVar4 = false;
  UiSelectableControl_IsSelectedCf(control);
  if (!bVar4) {
    (*g_SoundStopVoice)(g_InGameActiveMusicVoice);
    g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
    g_InGameMusicEnabled = 1;
  }
  dVar1 = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword(extraout_ECX | dVar1 & 0xfffffffd,0x20);
  pUVar2 = (control->base).parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    pUVar2 = (control->base).parent;
  }
  if ((extraout_ECX_00 & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,firstNode_01);
    UiNodeList_SuppressActionId(0x121a,firstNode_02);
    pUVar2 = extraout_EAX_00;
    uVar3 = extraout_ECX_02;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,firstNode);
    UiNodeList_UnsuppressActionId(0x121a,firstNode_00);
    pUVar2 = extraout_EAX;
    uVar3 = extraout_ECX_01;
  }
  if ((uVar3 & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_02;
    uVar3 = extraout_ECX_04;
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,pUVar2);
    pUVar2 = extraout_EAX_01;
    uVar3 = extraout_ECX_03;
  }
  if ((uVar3 & 3) == 0) {
    UiNodeList_SuppressActionId(0x120a,pUVar2);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,pUVar2);
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
void InGameAudioSettings_SetReverseStereo(UiSelectableControl *control)

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

/* Address: 0x0056CED0.
   Ownership: ui/ingame/settings.
   Purpose: Writes soundEffectsGainQ15, updates both effects gain globals, and immediately applies equal left/right
   gains to the active in-game effects test voice. Queued UI action handler for INGAME_PAGE12[11] (0x120B). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  dword effectsGainQ15;
  
  PersistentSettings_WriteDword(control->boundValue,0x24);
  g_UiSoundGainQ15 = effectsGainQ15;
  g_SoundEffectsGainQ15 = effectsGainQ15;
  (*g_SoundSetVoiceGains)(effectsGainQ15,effectsGainQ15,g_InGameActiveEffectVoice);
  return;
}

/* Address: 0x0056CF10.
   Ownership: ui/ingame/settings.
   Purpose: Writes movieDefaultAudioGainQ15 and updates the movie default-gain global. Queued UI action handler for
   INGAME_PAGE12[12] (0x120C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  MovieAudioGainQ15 extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x28);
  g_MovieDefaultAudioGainQ15 = extraout_EAX;
  return;
}

/* Address: 0x0056CF40.
   Ownership: ui/ingame/settings.
   Purpose: Writes musicGainQ15 and immediately applies equal left/right gains to the active in-game looping music
   voice. Queued UI action handler for INGAME_PAGE12[13] (0x120D). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  dword musicGainQ15;
  
  PersistentSettings_WriteDword(control->boundValue,0x2c);
  (*g_SoundSetVoiceGains)(musicGainQ15,musicGainQ15,g_InGameActiveMusicVoice);
  return;
}

/* Address: 0x0056CF70.
   Ownership: ui/ingame/settings.
   Purpose: Writes movieAlternateAudioGainQ15 and updates the alternate movie-gain global used by timed movie
   playback events. Queued UI action handler for INGAME_PAGE12[26] (0x121A). Return datatype is preserved for non-
   queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  MovieAudioGainQ15 extraout_EAX;
  
  PersistentSettings_WriteDword(control->boundValue,0x4c);
  g_MovieAlternateAudioGainQ15 = extraout_EAX;
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
void InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle)

{
  UiNodeBase *pUVar1;
  UiSelectableControl *pUVar2;
  UiPageStackControl *stack;
  UiPageStackControl *stack_00;
  dword dVar3;
  UiBooleanState32 extraout_EAX;
  UiBooleanState32 UVar4;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  UiBooleanState32 extraout_ECX_02;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int extraout_EDX_03;
  UiNodeBase *firstNode;
  UiNodeBase *firstNode_00;
  UiNodeBase *firstNode_01;
  int extraout_EDX_04;
  bool bVar5;
  
  pUVar1 = (settingsToggle->base).parent;
  pUVar2 = settingsToggle;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    pUVar2 = (UiSelectableControl *)(pUVar2->base).parent;
    pUVar1 = (pUVar2->base).parent;
  }
  bVar5 = (UiSelectableControl *)0xfffff42f < pUVar2;
  UiSelectableControl_IsSelectedCf(settingsToggle);
  if (!bVar5) {
    UiPageStack_SetActiveIndex(0,stack);
    *(uint *)(extraout_EDX_04 + 0xa78) = *(uint *)(extraout_EDX_04 + 0xa78) & 0xfffffff7;
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)(extraout_EDX + 0x4400));
  *(uint *)(extraout_EDX_00 + 0xa78) = *(uint *)(extraout_EDX_00 + 0xa78) | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)(extraout_EDX_00 + 0xa30));
  UiPageStack_SetActiveIndex(3,stack_00);
  dVar3 = PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_SetSelected(dVar3 & 1,(UiSelectableControl *)(extraout_EDX_01 + 0x2900));
  UiSelectableControl_SetSelected
            (extraout_ECX & 2,(UiSelectableControl *)(extraout_EDX_02 + 0x2960));
  UiSelectableControl_SetSelected
            (extraout_ECX_00 & 4,(UiSelectableControl *)(extraout_EDX_03 + 0x26d0));
  dVar3 = PersistentSettings_ReadDword(0,0x5c);
  UVar4 = 0;
  if ((dVar3 & 1) != 0) {
    UiNodeList_SuppressActionId(0x1215,firstNode);
    UVar4 = extraout_EAX;
  }
  UiSelectableControl_SetSelected(UVar4,(UiSelectableControl *)&firstNode[0x8d].rightAnchorQ31);
  UVar4 = 0;
  if ((extraout_ECX_01 & 2) != 0) {
    UiNodeList_SuppressActionId(0x1214,firstNode_00);
    UVar4 = extraout_ECX_02;
  }
  UiSelectableControl_SetSelected(UVar4,(UiSelectableControl *)(firstNode_00 + 0x8f));
  dVar3 = PersistentSettings_ReadDword(0x20,0x48);
  firstNode_01[0x88].bottomAnchorQ31 = dVar3;
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
    UiNodeList_SuppressActionId(0x120e,firstNode_01);
  }
  return;
}
