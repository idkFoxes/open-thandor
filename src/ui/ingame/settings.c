/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/settings.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/settings.h>
#include <thandor/thandor.h>

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
  /* source is now the in-game UI root (template start) */
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,missionObjectivesButton));
  InGameSettingsPage_ToggleAndSynchronizeControls
            ((UiSelectableControl *)INGAME_UI(source,missionObjectivesButton));
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
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
  UiSelectableControl_SetSelected(1,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  return;
}


/* Address: 0x0055F520.
   Ownership: ui/ingame/settings.
   Purpose: Handles in game simulation speed adjust player and recompute minimum ticks.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          int stepDelta)

{
  FrontendPlayerRuntimeBlockCount remainingCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  InGameSimulationStepBatchTicks stepTicks;
  
  stepTicks = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks + stepDelta;
  if ((stepTicks != 0) && (stepTicks < 6)) {
    g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks = stepTicks;
    remainingCount = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->simulationStepTicks <
          stepTicks) {
        stepTicks = g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->simulationStepTicks;
      }
      playerRecord = playerRecord + 1;
      remainingCount = remainingCount - 1;
      g_InGameSimulationStepTicks = stepTicks;
    } while (remainingCount != 0);
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
void __thandor_void_preserve_eax_ecx_edx InGameMissionHelpPage_SelectTab0(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpBriefingTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpMouseTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpKeyboardTab),
      sourceNode);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpTabPageStack));
  return;
}


/* Address: 0x0056AAD0.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[34]@005624A0. Queued UI action handler
   for INGAME_PAGE10[34] (0x1022). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameMissionHelpPage_SelectTab1(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpKeyboardTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpMouseTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpBriefingTab),
      sourceNode);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpTabPageStack));
  return;
}


/* Address: 0x0056AB10.
   Ownership: ui/ingame/settings.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[35]@005624A0. Queued UI action handler
   for INGAME_PAGE10[35] (0x1023). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameMissionHelpPage_SelectTab2(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpMouseTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpBriefingTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpKeyboardTab),
      sourceNode);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpTabPageStack));
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
  UiPageStackControl *stack;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x40);
  /* control is rightButtonNoScrollCheckbox of the in-game UI template copy */
  stack = (UiPageStackControl *)THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,sidePanelStack);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 4;
    UiPageStack_SetActiveIndex(1,stack);
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,resourceBarModeStack));
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,gamePanelsModeStack));
    THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,worldViewArea)->rightOffset = 0;
  }
  else {
    value = optionFlags & 0xfffffffb;
    UiPageStack_SetActiveIndex(0,stack);
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,resourceBarModeStack));
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,gamePanelsModeStack));
    THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,worldViewArea)->rightOffset =
         THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,sidePanelFrameLeftEdge)->leftOffset;
  }
  UiContainer_LayoutChildren(THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,inGameRootPanel));
  PersistentSettings_Write(value,0x40);
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
  PersistentSettings_Write(control->boundValue,0x48);
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
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 1;
    /* control is autoZoomOffCheckbox; reset the minimap zoom */
    ((UiSelectionGeometryControl *)THANDOR_UI_SIBLING(control,InGameUiImage,autoZoomOffCheckbox,minimapView))->sampleScaleQ12 =
         0x800;
  }
  else {
    value = optionFlags & 0xfffffffe;
  }
  PersistentSettings_Write(value,0x40);
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
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 2;
    /* control is autoRotationOffCheckbox; reset the minimap rotation */
    ((UiSelectionGeometryControl *)THANDOR_UI_SIBLING(control,InGameUiImage,autoRotationOffCheckbox,minimapView))->
    rotationAngle = 0x2000;
  }
  else {
    value = optionFlags & 0xfffffffd;
  }
  PersistentSettings_Write(value,0x40);
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
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 1;
    /* control is linkRotationZoomCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationZoomCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | 0x40000000;
    UiNodeList_SuppressActionId(0x1215,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffe;
    /* control is linkRotationZoomCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationZoomCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & 0xbfffffff;
    UiNodeList_UnsuppressActionId(0x1215,(control->base).parent);
  }
  PersistentSettings_Write(value,0x5c);
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
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 2;
    /* control is linkRotationTiltCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationTiltCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | 0x80000000;
    UiNodeList_SuppressActionId(0x1214,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffd;
    /* control is linkRotationTiltCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationTiltCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & 0x7fffffff;
    UiNodeList_UnsuppressActionId(0x1214,(control->base).parent);
  }
  PersistentSettings_Write(value,0x5c);
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
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 4;
    /* control is hidePanelCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,hidePanelCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | 0x4000000;
  }
  else {
    value = optionFlags & 0xfffffffb;
    /* control is hidePanelCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,hidePanelCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & 0xfbffffff;
  }
  PersistentSettings_Write(value,0x5c);
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
InGameGraphicsSettings_OpenAndSynchronize(UiNodeBase *graphicsButton)

{
  UiNodeBase *firstNode;
  uint32_t settingValue;
  uint32_t storedSubresourceCount;
  int scaledSubresourceCount;
  UiNodeBase *parentOrSelectedRow;
  
/* graphicsButton is gameMenuGraphicsButton of the in-game UI template copy */
#define GRAPHICS_UI(node) THANDOR_UI_SIBLING(graphicsButton,InGameUiImage,gameMenuGraphicsButton,node)
  UiPageStack_SetActiveIndex(6,(UiPageStackControl *)GRAPHICS_UI(gameWindowPageStack));
  settingValue = PersistentSettings_Read(1,0x1c);
  UiSelectableControl_SetSelected(settingValue,(UiSelectableControl *)GRAPHICS_UI(shadingEnabledCheckbox));
  parentOrSelectedRow = graphicsButton->parent;
  firstNode = graphicsButton;
  while (parentOrSelectedRow != (UiNodeBase *)0xffffffff) {
    firstNode = firstNode->parent;
    parentOrSelectedRow = firstNode->parent;
  }
  if (settingValue == 0) {
    UiNodeList_SuppressActionId(0x1205,firstNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,firstNode);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(0x1207,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(0x1207,firstNode);
  }
  settingValue = PersistentSettings_Read(0x20,0x10);
  storedSubresourceCount = PersistentSettings_Read(0x10,0x18);
  scaledSubresourceCount = storedSubresourceCount * 4;
  if (settingValue == 0x20) {
    parentOrSelectedRow = GRAPHICS_UI(shadingLevel32x32Button);
    if (scaledSubresourceCount == 0x40) {
      parentOrSelectedRow = GRAPHICS_UI(shadingLevel32x64Button);
    }
    else if (scaledSubresourceCount == 0x80) {
      parentOrSelectedRow = GRAPHICS_UI(shadingLevel32x128Button);
    }
  }
  else if (settingValue == 0x40) {
    parentOrSelectedRow = GRAPHICS_UI(shadingLevel64x64Button);
    if (scaledSubresourceCount == 0x80) {
      parentOrSelectedRow = GRAPHICS_UI(shadingLevel64x128Button);
    }
  }
  else {
    parentOrSelectedRow = GRAPHICS_UI(shadingLevel128x128Button);
  }
  UiSelectableGroup_SelectExclusive(6,parentOrSelectedRow,
      GRAPHICS_UI(shadingLevel128x128Button),
      GRAPHICS_UI(shadingLevel64x128Button),
      GRAPHICS_UI(shadingLevel64x64Button),
      GRAPHICS_UI(shadingLevel32x128Button),
      GRAPHICS_UI(shadingLevel32x64Button),
      GRAPHICS_UI(shadingLevel32x32Button));
  settingValue = PersistentSettings_Read(1,0x30);
  if (settingValue == 0) {
    parentOrSelectedRow = GRAPHICS_UI(textureQualityHighButton);
  }
  else if (settingValue == 1) {
    parentOrSelectedRow = GRAPHICS_UI(textureQualityMediumButton);
  }
  else {
    parentOrSelectedRow = GRAPHICS_UI(textureQualityLowButton);
  }
  UiSelectableGroup_SelectExclusive(3,parentOrSelectedRow,
      GRAPHICS_UI(textureQualityHighButton),
      GRAPHICS_UI(textureQualityMediumButton),
      GRAPHICS_UI(textureQualityLowButton));
  settingValue = PersistentSettings_Read(0x10000,0x34);
  ((UiRangeSliderControl *)GRAPHICS_UI(modelDetailSlider))->value = settingValue;
#undef GRAPHICS_UI
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
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  uint32_t gainQ15;
  
  UiPageStack_SetActiveIndex(7,&THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->settingsPageStack);
  audioFlags = PersistentSettings_Read(3,0x20);
  UiSelectableControl_SetSelected(audioFlags & 1,&THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 2,&THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 4,&THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->reverseStereoControl);
  gainQ15 = PersistentSettings_Read(0x8000,0x24);
  (THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->soundEffectsGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(0x8000,0x28);
  (THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->movieDefaultAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(0x8000,0x4c);
  (THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->movieAlternateAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(0x8000,0x2c);
  (THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)->musicGainControl).currentValue = gainQ15;
  parentCursor = settingsSourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    parentCursor = settingsSourceNode->parent;
  }
  if ((audioFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x120c,settingsSourceNode);
    UiNodeList_SuppressActionId(0x121a,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x120c,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x121a,settingsSourceNode);
  }
  if ((audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,settingsSourceNode);
  }
  if ((audioFlags & 3) == 0) {
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
  uint8_t selectedState;
  UiNodeBase *parentCursor;
  
  selectedState = UiSelectableControl_IsSelectedCf(control);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((selectedState & 1) == 0) {
    UiNodeList_SuppressActionId(0x1205,&control->base);
    ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags =
         ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags & 0xfffdffff;
  }
  else {
    UiNodeList_UnsuppressActionId(0x1205,&control->base);
    ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags =
         ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags | 0x20000;
  }
  PersistentSettings_Write(selectedState & 1,0x1c);
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
  UiNodeBase *settingsRoot;
  uint32_t subresourceCount;
  int scaledSubresourceCount;
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t storedSubresourceCount;
  PersistentSettingsValue newGridHalfSize;
  PersistentSettingsValue newTextureDimension;
  UiNodeBase *selectedControl;
  StatusResult initStatus;
  FatalErrorCheckResult fatalResult;
  uint32_t value;
  
  subresourceCount = (uint32_t)((UiNumericPairTextButton *)control)->secondValue >> 2;
  value = subresourceCount;
  /* The option control stores the grid half size at +0x60 (ECX); the texture dimension is twice
     that (EDX). The decompile passed both as uninitialized locals. */
  newGridHalfSize = (PersistentSettingsValue)((UiNumericPairTextButton *)control)->firstValue;
  newTextureDimension = newGridHalfSize * 2;
  GraphicsShadingRuntime_Shutdown();
  initStatus = GraphicsShadingRuntime_InitializeGeneratedTextureCf
                    (subresourceCount,newGridHalfSize,newTextureDimension);
  fatalResult = FatalError_ReportIfFailed(initStatus.valueOrError,initStatus.failed);
  if (!fatalResult.failed) {
    PersistentSettings_Write(newTextureDimension,0x14);
    PersistentSettings_Write(newGridHalfSize,0x10);
    PersistentSettings_Write(value,0x18);
    scaledSubresourceCount = value << 2;
    /* control is one of the shading level buttons, its parent is shadingLevelGroup */
    settingsRoot = (control->base).parent;
    if (newGridHalfSize == 0x20) {
      selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel32x32Button);
      if (scaledSubresourceCount == 0x40) {
        selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel32x64Button);
      }
      else if (scaledSubresourceCount == 0x80) {
        selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel32x128Button);
      }
    }
    else if (newGridHalfSize == 0x40) {
      selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel64x64Button);
      if (scaledSubresourceCount == 0x80) {
        selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel64x128Button);
      }
    }
    else {
      selectedControl = THANDOR_UI_SIBLING(settingsRoot,InGameUiImage,shadingLevelGroup,shadingLevel128x128Button);
    }
    UiSelectableGroup_SelectExclusive(6,selectedControl,
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel128x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel64x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel64x64Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x64Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x32Button));
    return;
  }
  textureDimension = PersistentSettings_Read(0x40,0x14);
  gridHalfSize = PersistentSettings_Read(0x20,0x10);
  storedSubresourceCount = PersistentSettings_Read(0x10,0x18);
  GraphicsShadingRuntime_InitializeGeneratedTextureCf
            (storedSubresourceCount,gridHalfSize,textureDimension);
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
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x34);
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
  /* control is one of the texture quality buttons, its parent is textureQualityGroup */
  graphicsSettingsRoot = (control->base).parent;
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityLowButton) == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityLowButton);
  }
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityMediumButton) == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityMediumButton);
  }
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityHighButton) == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = THANDOR_UI_SIBLING(graphicsSettingsRoot,InGameUiImage,textureQualityGroup,textureQualityHighButton);
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityHighButton),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityMediumButton),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityLowButton));
  PersistentSettings_Write(qualityLevel,0x30);
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
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  AudioMixerGainQ15 effectsGainQ15;
  MovieAudioGainQ15 movieDefaultGainQ15;
  MovieAudioGainQ15 movieAlternateGainQ15;
  bool isEnabled;
  
  isEnabled = (bool)UiSelectableControl_IsSelectedCf(control);
  if (!isEnabled) {
    (*g_SoundStopVoice)(g_InGameActiveEffectVoice);
    g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
  }
  isEnabled = isEnabled;
  audioFlags = PersistentSettings_Read(3,0x20);
  PersistentSettings_Write((uint32_t)isEnabled | audioFlags & 0xfffffffe,0x20);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if (isEnabled) {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,&control->base);
    UiNodeList_UnsuppressActionId(0x121a,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,&control->base);
    UiNodeList_SuppressActionId(0x121a,&control->base);
  }
  if ((audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x120d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,&control->base);
  }
  if (isEnabled == 0 && (audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x120a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120a,&control->base);
  }
  effectsGainQ15 = 0;
  if (isEnabled) {
    effectsGainQ15 = PersistentSettings_Read(0x8000,0x24);
  }
  movieDefaultGainQ15 = 0;
  g_UiSoundGainQ15 = effectsGainQ15;
  g_SoundEffectsGainQ15 = effectsGainQ15;
  if (isEnabled) {
    movieDefaultGainQ15 = PersistentSettings_Read(0x8000,0x28);
  }
  movieAlternateGainQ15 = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGainQ15;
  if (isEnabled) {
    movieAlternateGainQ15 = PersistentSettings_Read(0x8000,0x4c);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGainQ15;
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
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  uint32_t musicEnabledBit;
  bool isEnabled;
  
  musicEnabledBit = 0;
  isEnabled = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isEnabled) {
    musicEnabledBit = 2;
  }
  else {
    (*g_SoundStopVoice)(g_InGameActiveMusicVoice);
    g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
    g_InGameMusicEnabled = 1;
  }
  audioFlags = PersistentSettings_Read(3,0x20);
  PersistentSettings_Write(musicEnabledBit | audioFlags & 0xfffffffd,0x20);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((audioFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x120b,&control->base);
    UiNodeList_SuppressActionId(0x120c,&control->base);
    UiNodeList_SuppressActionId(0x121a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120b,&control->base);
    UiNodeList_UnsuppressActionId(0x120c,&control->base);
    UiNodeList_UnsuppressActionId(0x121a,&control->base);
  }
  if (musicEnabledBit == 0) {
    UiNodeList_SuppressActionId(0x120d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x120d,&control->base);
  }
  if (musicEnabledBit == 0 && (audioFlags & 1) == 0) {
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
  uint32_t currentAudioFlags;
  uint32_t reverseStereoBit;
  int32_t reverseStereoMask;
  bool isSelected;
  
  reverseStereoBit = 0;
  reverseStereoMask = 0;
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    reverseStereoBit = 4;
    reverseStereoMask = -1;
  }
  currentAudioFlags = PersistentSettings_Read(3,0x20);
  g_ReverseStereoMask = reverseStereoMask;
  PersistentSettings_Write(reverseStereoBit | currentAudioFlags & 0xfffffffb,0x20);
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
  PersistentSettingsValue value;
  uint32_t effectsGainQ15;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x24);
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
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x28);
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
  PersistentSettingsValue value;
  uint32_t musicGainQ15;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x2c);
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
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x4c);
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
  UiNodeBase *parentCursor;
  UiNodeBase *uiRoot;
  uint32_t settingValue;
  bool isSelected;
  
  parentCursor = (settingsToggle->base).parent;
  uiRoot = &settingsToggle->base;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    uiRoot = uiRoot->parent;
    parentCursor = uiRoot->parent;
  }
  isSelected = (bool)UiSelectableControl_IsSelectedCf(settingsToggle);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(uiRoot,gameWindowPageStack));
    INGAME_UI(uiRoot,worldView)->nodeFlags = INGAME_UI(uiRoot,worldView)->nodeFlags & 0xfffffff7;
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(uiRoot,missionObjectivesButton));
  INGAME_UI(uiRoot,worldView)->nodeFlags = INGAME_UI(uiRoot,worldView)->nodeFlags | 8;
  UiKeyboardFocus_ReleaseNode(INGAME_UI(uiRoot,worldView));
  UiPageStack_SetActiveIndex(3,(UiPageStackControl *)INGAME_UI(uiRoot,gameWindowPageStack));
  settingValue = PersistentSettings_Read(0,0x40);
  UiSelectableControl_SetSelected(settingValue & 1,(UiSelectableControl *)INGAME_UI(uiRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected(settingValue & 2,(UiSelectableControl *)INGAME_UI(uiRoot,autoRotationOffCheckbox));
  UiSelectableControl_SetSelected
            (settingValue & 4,(UiSelectableControl *)INGAME_UI(uiRoot,rightButtonNoScrollCheckbox));
  settingValue = PersistentSettings_Read(0,0x5c);
  if ((settingValue & 1) != 0) {
    UiNodeList_SuppressActionId(0x1215,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & 1,(UiSelectableControl *)INGAME_UI(uiRoot,linkRotationZoomCheckbox));
  if ((settingValue & 2) != 0) {
    UiNodeList_SuppressActionId(0x1214,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & 2,(UiSelectableControl *)INGAME_UI(uiRoot,linkRotationTiltCheckbox));
  settingValue = PersistentSettings_Read(0x20,0x48);
  ((UiRangeSliderControl *)INGAME_UI(uiRoot,scrollSpeedSlider))->value = settingValue;
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
    UiNodeList_SuppressActionId(0x120e,uiRoot);
  }
  return;
}

