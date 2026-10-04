/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/editor_tool_selection.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/editor_tool_selection.h>
#include <thandor/thandor.h>

/* Module data. */

/* uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */
THANDOR_ALIGN(4) uint32_t g_UiCommandModeGColorVariantLimit = 16777215;

static uint32_t g_UiCommandSelectionPageBaseIndex = 0;

static int32_t g_UiMappedCommandControlOffsets[12] = {42892, 43076, 43260, 43444, 43628, 43812, 43996, 44180, 44364, 44548, 44732, 44916};

/* uint32_t render-state flag word copied into terrain packets (primitives.c); ui/ingame/commands.c sets/clears the masked G-colour variant bit */
uint32_t g_UiCommandModeGColorVariantFlags = 0x10000;

uint32_t g_UiCommandModeE = 0;

uint32_t g_UiCommandModeA = 0;

uint32_t g_UiCommandModeB = 0;

uint32_t g_UiCommandModeF = 0;

/* Implementation ownership: ui/ingame/editor_tool_selection. */

/* Editor mode tab G0, terrain height tool (action 0x1100: g_InGameUiActionHandlersPage11[0],
   g_UiCommandModeGHandlers[0]; also called by the editor hotkeys in ui/ingame/runtime.c). Selects the tab, shows
   the tool's pages and switches the world view to the height tool overlays: surface point, terrain point, grid
   vertex and secondary surface markers, with the unmasked terrain colour ramp.
*/
void InGameCommandModeG_Select0(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_HEIGHT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_ShowTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_SetSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}

/* Editor mode tab G1, terrain material tool (action 0x1101: g_InGameUiActionHandlersPage11[1],
   g_UiCommandModeGHandlers[1]; also called by the editor hotkeys in ui/ingame/runtime.c). Like G0, but without the
   secondary surface markers.
*/
void InGameCommandModeG_Select1(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_MATERIAL,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_ShowTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}

/* Editor mode tab G2, terrain smoothing tool (action 0x1102: g_InGameUiActionHandlersPage11[2],
   g_UiCommandModeGHandlers[2]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the surface point,
   grid vertex and secondary surface markers and is the only mode with the masked terrain colours
   (UiCommandModeG_ApplyMaskedColorVariant).
*/
void InGameCommandModeG_Select2(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_SMOOTHING,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_SetSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyMaskedColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}

/* Editor mode tab G3, unit placement tool (action 0x1105: g_InGameUiActionHandlersPage11[5],
   g_UiCommandModeGHandlers[3]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the army metrics
   and grid vertex markers and puts the army asset g_UiCommandModeGArmyAssetId into the selection detail panel;
   an unknown asset id is fatal.
*/
void InGameCommandModeG_Select3(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *armyRecord;
  uintptr_t checkedAssetLookup;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_UNIT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_HideSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  lookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&armyRecord);
  checkedAssetLookup = FatalError_ExitIfFailed(lookupError != 0 ? lookupError : (uintptr_t)armyRecord,
                                               lookupError != 0);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)checkedAssetLookup;
  InGameSelectionDetailPanel_Rebuild();
  return;
}

/* Editor mode tab G4, object placement tool (action 0x1106: g_InGameUiActionHandlersPage11[6],
   g_UiCommandModeGHandlers[4]; also called by the editor hotkeys in ui/ingame/runtime.c). Same overlays as G3
   (army metrics and grid vertex markers) without the detail panel update.
*/
void InGameCommandModeG_Select4(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_OBJECT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_HideSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}

/* Editor mode tab G5, region tool (action 0x1104: g_InGameUiActionHandlersPage11[4],
   g_UiCommandModeGHandlers[5]; also called by ui/ingame/runtime.c). Shows the surface point, army metrics, grid
   vertex and region markers; the region markers draw the variant chosen by mode F, so g_UiCommandModeF is copied
   into the world runtime (fieldRegion.regionToolMode) as well.
*/
void InGameCommandModeG_Select5(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_REGION,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ShowRegionMarkers(worldRuntime);
  (runtimeRoot->worldRuntime).fieldRegion.regionToolMode = g_UiCommandModeF;
  return;
}

/* Terrain material swatch click (action 0x1110, g_InGameUiActionHandlersPage11[16]): finds which of the
   twelve swatch controls (g_UiMappedCommandControlOffsets) was clicked and selects the material at that position
   of the current page. Clicks on other controls are ignored.
*/
void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source)

{
  UiNodeBase *root;
  int mappingsRemaining;
  int mappingIndex;

  root = source;
  while (root->parent != UI_NODE_NONE) {
    root = root->parent;
  }
  for (mappingIndex = 0, mappingsRemaining = MATERIAL_SWATCH_COUNT; mappingsRemaining != 0;
       mappingIndex++, mappingsRemaining--) {
    if ((int)((uintptr_t)source - (uintptr_t)root) == g_UiMappedCommandControlOffsets[mappingIndex]) {
      UiCommandMatrix_SelectIndex(mappingIndex + g_UiCommandSelectionPageBaseIndex,root);
      return;
    }
  }
  return;
}

/* Hides the grid vertex markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); called
   when the editor is switched off (InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState).
*/
void UiCommandModeG_HideGridVertexMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}

/* Height tool option 0 (action 0x1108, g_InGameUiActionHandlersPage11[8]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption0 among the four height tool buttons and sets g_UiCommandModeC = 0.
*/
void InGameCommandModeC_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption0));
  g_UiCommandModeC = 0;
  return;
}

/* Height tool option 1 (action 0x1109, g_InGameUiActionHandlersPage11[9]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption1 among the four height tool buttons and sets g_UiCommandModeC = 1.
*/
void InGameCommandModeC_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption0));
  g_UiCommandModeC = 1;
  return;
}

/* Height tool option 2 (action 0x110A, g_InGameUiActionHandlersPage11[10]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption2 among the four height tool buttons and sets g_UiCommandModeC = 2.
*/
void InGameCommandModeC_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption0));
  g_UiCommandModeC = 2;
  return;
}

/* Height tool option 3 (action 0x110B, g_InGameUiActionHandlersPage11[11]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption3 among the four height tool buttons and sets g_UiCommandModeC = 3.
*/
void InGameCommandModeC_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption0));
  g_UiCommandModeC = 3;
  return;
}

/* Material tool option 0 (action 0x110C, g_InGameUiActionHandlersPage11[12]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption0 among the four material tool buttons and sets
   g_UiCommandModeD = 0.
*/
void InGameCommandModeD_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption0));
  g_UiCommandModeD = 0;
  return;
}

/* Material tool option 1 (action 0x110D, g_InGameUiActionHandlersPage11[13]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption1 among the four material tool buttons and sets
   g_UiCommandModeD = 1.
*/
void InGameCommandModeD_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption0));
  g_UiCommandModeD = 1;
  return;
}

/* Material tool option 2 (action 0x110E, g_InGameUiActionHandlersPage11[14]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption2 among the four material tool buttons and sets
   g_UiCommandModeD = 2.
*/
void InGameCommandModeD_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption0));
  g_UiCommandModeD = 2;
  return;
}

/* Material tool option 3 (action 0x110F, g_InGameUiActionHandlersPage11[15]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption3 among the four material tool buttons and sets
   g_UiCommandModeD = 3.
*/
void InGameCommandModeD_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption0));
  g_UiCommandModeD = 3;
  return;
}

/* Unit placement option 0 (action 0x1111, g_InGameUiActionHandlersPage11[17]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption0 among the three unit placement buttons and sets
   g_UiCommandModeA = 0.
*/
void InGameCommandModeA_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption0));
  g_UiCommandModeA = 0;
  return;
}

/* Unit placement option 1 (action 0x1112, g_InGameUiActionHandlersPage11[18]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption1 among the three unit placement buttons and sets
   g_UiCommandModeA = 1.
*/
void InGameCommandModeA_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption0));
  g_UiCommandModeA = 1;
  return;
}

/* Unit placement option 2 (action 0x1113, g_InGameUiActionHandlersPage11[19]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption2 among the three unit placement buttons and sets
   g_UiCommandModeA = 2.
*/
void InGameCommandModeA_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption0));
  g_UiCommandModeA = 2;
  return;
}

/* Object placement option 0 (action 0x1114, g_InGameUiActionHandlersPage11[20]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption0 among the three object placement buttons and sets
   g_UiCommandModeB = 0.
*/
void InGameCommandModeB_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption0));
  g_UiCommandModeB = 0;
  return;
}

/* Object placement option 1 (action 0x1115, g_InGameUiActionHandlersPage11[21]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption1 among the three object placement buttons and sets
   g_UiCommandModeB = 1.
*/
void InGameCommandModeB_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption0));
  g_UiCommandModeB = 1;
  return;
}

/* Object placement option 2 (action 0x1116, g_InGameUiActionHandlersPage11[22]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption2 among the three object placement buttons and sets
   g_UiCommandModeB = 2.
*/
void InGameCommandModeB_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption0));
  g_UiCommandModeB = 2;
  return;
}

/* Smoothing tool option 0 (action 0x1117, g_InGameUiActionHandlersPage11[23]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption0 and sets g_UiCommandModeE = 0. Unlike options 1 and 2 its
   exclusive group also contains smoothingRelaxLandButton.
*/
void InGameCommandModeE_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingRelaxLandButton),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption0));
  g_UiCommandModeE = 0;
  return;
}

/* Smoothing tool option 1 (action 0x1118, g_InGameUiActionHandlersPage11[24]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption1 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 1.
*/
void InGameCommandModeE_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption0));
  g_UiCommandModeE = 1;
  return;
}

/* Smoothing tool option 2 (action 0x1119, g_InGameUiActionHandlersPage11[25]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption2 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 2.
*/
void InGameCommandModeE_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption0));
  g_UiCommandModeE = 2;
  return;
}

/* Smoothing page button smoothingRelaxGatedButton (action 0x111A, g_InGameUiActionHandlersPage11[26]; also
   an editor hotkey in ui/ingame/runtime.c): runs 128 sign-gated terrain relaxation passes over the field, in a
   network game through command 0x3200 on every machine.
*/
void InGameCommandRange_DispatchState0(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_SIGN_GATED);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_SIGN_GATED);
  }
  return;
}

/* Smoothing page button smoothingRelaxLandButton (action 0x111B, g_InGameUiActionHandlersPage11[27]; also
   an editor hotkey in ui/ingame/runtime.c): like InGameCommandRange_DispatchState0 with the ungated land tool
   relaxation mode.
*/
void InGameCommandRange_DispatchState1(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  return;
}

/* Region tool option 0 (action 0x111C, g_InGameUiActionHandlersPage11[28]): selects regionToolOption0 of the
   two region tool buttons, sets g_UiCommandModeF = 0 and copies it into the world runtime (fieldRegion.regionToolMode), where the
   region markers of the world view read it.
*/
void InGameCommandModeF_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption0));
  g_UiCommandModeF = 0;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,worldView))
       ->fieldRegion.regionToolMode = 0;
  return;
}

/* Region tool option 1 (action 0x111D, g_InGameUiActionHandlersPage11[29]): selects regionToolOption1 and
   sets g_UiCommandModeF and its world runtime copy (fieldRegion.regionToolMode) to 1.
*/
void InGameCommandModeF_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption0));
  g_UiCommandModeF = 1;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,worldView))
       ->fieldRegion.regionToolMode = 1;
  return;
}

/* Terrain colours of the smoothing tool (InGameCommandModeG_Select2): rebuilds the terrain lighting colour ramp
   from the world runtime's lighting colours (lighting.baseColorArgb, rampStepColorArgb) with their alpha removed and the secondary
   colour (secondaryColorArgb) made opaque, relights the field grid with the light angles stored in the root, then sets
   UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED and the limit read by the terrain triangle and marker drawing.
*/
void UiCommandModeG_ApplyMaskedColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (((WorldRuntimeContext *)worldRuntime)->lighting.secondaryColorArgb | ARGB8888_ALPHA_MASK,
             ((WorldRuntimeContext *)worldRuntime)->lighting.baseColorArgb & 0xffffff,
             ((WorldRuntimeContext *)worldRuntime)->lighting.rampStepColorArgb & 0xffffff);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightElevationAngle,THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightAzimuthAngle,
             ((WorldRuntimeContext *)worldRuntime)->fieldGrid);
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags | UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED;
  g_UiCommandModeGColorVariantLimit = UI_COMMAND_MODE_G_COLOR_LIMIT_MASKED;
  return;
}

/* Shows the region markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); only the region tool
   (InGameCommandModeG_Select5) uses it.
*/
void UiCommandModeG_ShowRegionMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}

/* Texture shown by a material swatch: the first texture of the material's set, NULL for an empty entry. */
static GraphicsTextureSourceAsset *TerrainMaterial_SwatchTexture(uint32_t materialIndex)
{
  if (g_TerrainMaterialTextureSets[materialIndex] == NULL) {
    return NULL;
  }
  return g_TerrainMaterialTextureSets[materialIndex]->entries[0].sourceAsset;
}

/* Selects terrain material absoluteIndex for the material tool (InGameCommandMatrix_SelectMappedControl and the
   editor hotkeys/initialisation in ui/ingame/runtime.c): shows its texture in materialToolSelectedSwatch,
   scrolls the twelve-swatch page in rows of three until the material is visible, fills the twelve swatches from
   g_TerrainMaterialTextureSets (empty entries show nothing) and selects the material's swatch.
*/
void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root)

{
  uint32_t pageEnd;
  uint32_t pageBase;
  
  g_UiCommandAbsoluteSelectionIndex = absoluteIndex;
  ((UiImagePanelControl *)INGAME_UI(root,materialToolSelectedSwatch))->textureSource =
       g_TerrainMaterialTextureSets[absoluteIndex]->entries[0].sourceAsset;
  pageEnd = g_UiCommandSelectionPageBaseIndex + MATERIAL_SWATCH_COUNT;
  pageBase = g_UiCommandSelectionPageBaseIndex;
  /* move the page by rows of three swatches until absoluteIndex lies in [pageBase, pageEnd) */
  while (absoluteIndex < pageBase) {
    pageBase = pageBase - MATERIAL_SWATCH_ROW_LENGTH;
    pageEnd = pageEnd - MATERIAL_SWATCH_ROW_LENGTH;
  }
  while (absoluteIndex >= pageEnd) {
    pageBase = pageBase + MATERIAL_SWATCH_ROW_LENGTH;
    pageEnd = pageEnd + MATERIAL_SWATCH_ROW_LENGTH;
  }
  g_UiCommandSelectionPageBaseIndex = pageBase;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch00))->textureSource = TerrainMaterial_SwatchTexture(pageBase);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch01))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 1);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch02))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 2);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch03))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 3);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch04))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 4);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch05))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 5);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch06))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 6);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch07))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 7);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch08))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 8);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch09))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 9);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch10))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 10);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch11))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 11);
  /* The original pushes all twelve command controls (offsets 11..0) as the variadic list. */
  UiSelectableGroup_SelectExclusive
            (MATERIAL_SWATCH_COUNT,THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[absoluteIndex - pageBase]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[0]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[1]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[2]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[3]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[4]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[5]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[6]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[7]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[8]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[9]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[10]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[11]));
  return;
}

/* Hides the surface point marker of the world view (clears WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G3/G4 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideSurfacePointMarker(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}

/* Shows the terrain point markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G0/G1 (height and material tools).
*/
void UiCommandModeG_ShowTerrainPointMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}

/* Sets WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY (view ray and markers use only the secondary field surface);
   editor mode tabs G0 and G2.
*/
void UiCommandModeG_SetSecondarySurfaceOnly(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}

/* Shows the army metrics overlay of the world view (sets WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS); editor mode tabs
   G3-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the editor is switched off.
*/
void UiCommandModeG_ShowArmyMetrics(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS;
  return;
}

/* Hides the army metrics overlay and ends a drag selection (clears WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS and
   WORLD_RUNTIME_FLAG_DRAG_SELECTING, which also draws the selection frame); editor mode tabs G0-G2.
*/
void UiCommandModeG_HideArmyMetricsAndEndDragSelect(WorldRuntimeContext *context)

{
  context->runtimeFlags =
       context->runtimeFlags & ~(WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS | WORLD_RUNTIME_FLAG_DRAG_SELECTING);
  return;
}

/* Shows the surface point marker of the world view (sets WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G0, G1, G2 and G5.
*/
void UiCommandModeG_ShowSurfacePointMarker(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}

/* Hides the terrain point markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G2-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideTerrainPointMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}

/* Clears WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY; editor mode tabs G1, G3-G5 and
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_ClearSecondarySurfaceOnly(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}

/* Terrain colours of every editor mode except smoothing (and of InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
   when the editor is switched off): rebuilds the terrain colour ramp from the world runtime's lighting colours
   (lighting.baseColorArgb, rampStepColorArgb, secondaryColorArgb) unchanged, relights the field grid with the light
   angles stored in the root, clears bit 0x1000 of g_UiCommandModeGColorVariantFlags and sets the limit to 0x00FFFFFF. The
   counterpart of UiCommandModeG_ApplyMaskedColorVariant.
*/
void UiCommandModeG_ApplyRawColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (((WorldRuntimeContext *)worldRuntime)->lighting.secondaryColorArgb,((WorldRuntimeContext *)worldRuntime)->lighting.baseColorArgb,
             ((WorldRuntimeContext *)worldRuntime)->lighting.rampStepColorArgb);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightElevationAngle,THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightAzimuthAngle,
             ((WorldRuntimeContext *)worldRuntime)->fieldGrid);
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags & ~UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED;
  g_UiCommandModeGColorVariantLimit = UI_COMMAND_MODE_G_COLOR_LIMIT_RAW;
  return;
}

/* Hides the region markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); every editor mode tab
   except G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideRegionMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}

/* Shows the grid vertex markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); every
   editor mode tab.
*/
void UiCommandModeG_ShowGridVertexMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}

/* Common part of the editor mode tabs InGameCommandModeG_Select0..5: selects the clicked tab among the six, shows
   the mode's pages in modePreviewPageStack, modeDetailPageStack and modeCommandPageStack (page tables
   g_UiCommandModeG*PageIndices) and stores the mode in g_UiCommandModeG. Returns the in-game root.
*/
InGameRuntimeRoot * UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source)

{
  InGameRuntimeRoot *root;

  root = (InGameRuntimeRoot *)source;
  while ((root->rootUi).base.parent != UI_NODE_NONE) {
    root = (InGameRuntimeRoot *)(root->rootUi).base.parent;
  }
  UiSelectableGroup_FindVisibleSelected(NULL,NULL,6,
      INGAME_UI(root,editorModeTabRegion),
      INGAME_UI(root,editorModeTabObjectPlacement),
      INGAME_UI(root,editorModeTabUnitPlacement),
      INGAME_UI(root,editorModeTabTerrainSmoothing),
      INGAME_UI(root,editorModeTabTerrainMaterial),
      INGAME_UI(root,editorModeTabTerrainHeight));
  UiSelectableGroup_SelectExclusive(6,&source->base,
      INGAME_UI(root,editorModeTabRegion),
      INGAME_UI(root,editorModeTabObjectPlacement),
      INGAME_UI(root,editorModeTabUnitPlacement),
      INGAME_UI(root,editorModeTabTerrainSmoothing),
      INGAME_UI(root,editorModeTabTerrainMaterial),
      INGAME_UI(root,editorModeTabTerrainHeight));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGPrimaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modePreviewPageStack));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGSecondaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modeDetailPageStack));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGTertiaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modeCommandPageStack));
  g_UiCommandModeG = modeIndex;
  return root;
}

void *g_UiCommandModeGHandlers[6] = {
    /* 0 */ THANDOR_FN(InGameCommandModeG_Select0),
    /* 1 */ THANDOR_FN(InGameCommandModeG_Select1),
    /* 2 */ THANDOR_FN(InGameCommandModeG_Select2),
    /* 3 */ THANDOR_FN(InGameCommandModeG_Select3),
    /* 4 */ THANDOR_FN(InGameCommandModeG_Select4),
    /* 5 */ THANDOR_FN(InGameCommandModeG_Select5)};
