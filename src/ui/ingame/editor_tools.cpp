/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/editor_tools.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/editor_tools.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static int32_t g_UiCommandDragReferenceX = 0;

static int32_t g_UiCommandDragReferenceY = 0;


static int32_t g_UiCommandSelectionAnchorWorldXQ12 = 0;

static int32_t g_UiCommandSelectionAnchorWorldYQ12 = 0;

static int32_t g_UiCommandSelectionCurrentWorldXQ12 = 0;

static int32_t g_UiCommandSelectionCurrentWorldYQ12 = 0;

static uint32_t g_UiCommandDragAnchorWorldXQ12 = 0;

static uint32_t g_UiCommandDragAnchorWorldYQ12 = 0;

static uint32_t g_UiCommandDragStartScreenX = 0;

static uint32_t g_UiCommandDragStartScreenY = 0;

uint32_t g_LocalPlayerRuntimeId = 0;

int32_t g_InGameSelectionInsertTripletDwordCount = 0;

int32_t g_InGameSelectionRemoveTripletDwordCount = 0;

uint32_t g_UiCommandModeG = 0;

uint32_t g_UiCommandModeC = 0;

uint32_t g_UiCommandModeD = 0;

uint32_t g_UiCommandAbsoluteSelectionIndex = 0;

uint32_t g_UiCommandTerrainMaskToggleValue = 0;

FactionRuntimeIndex g_UiCommandModeGOwnerFactionIndex = 1;

uint32_t g_UiCommandCallerMaskHighBit = 0;

/* uint32_t[6]: active page of the mode preview page stack per command mode G; ui/ingame commands/runtime */
const uint32_t g_UiCommandModeGPrimaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

/* uint32_t[6]: active page of the mode detail page stack per command mode G; ui/ingame commands/runtime */
const uint32_t g_UiCommandModeGSecondaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

/* uint32_t[6]: active page of the mode command page stack per command mode G; ui/ingame commands/runtime */
const uint32_t g_UiCommandModeGTertiaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

/* Map editor pointer callback (installed as both selection.resolveContextAction*Callback of the world
   runtime by InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState): returns the cursor frame for the
   active editor tab and tool. For placing, a temporary army instance is created at the pointer and tested
   with ArmyRuntimeNode_DispatchTypedCallback to show whether it fits; with an army already picked up the
   picked army is tested instead.
*/
uint32_t InGameUiCommand_ResolveCursorCodeByMode
                (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12,
                uint32_t reservedArg3,WorldOwnerListNode *ownerNodeUnderPointer,
                WorldRuntimeContext *worldRuntime)

{
  SelectionPlayerRuntimeBlock *localSelectionBlock;
  uint32_t placementSubMode;
  uint32_t cursorCode;
  Bool8 callbackAccepted;
  ArmyRuntimeSlot *previewArmyRuntime;

  /* the world owner-list node under the pointer; only model nodes count */
  if ((ownerNodeUnderPointer != nullptr) &&
     (ownerNodeUnderPointer->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) {
    ownerNodeUnderPointer = nullptr;
  }
  /* The values are cursor frames of the editor tools (EDITOR_CURSOR_*, WORLD_CURSOR_*). */
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if (g_UiCommandModeC == 0) {
      return EDITOR_CURSOR_HEIGHT_RAISE;
    }
    if (g_UiCommandModeC == 1) {
      return EDITOR_CURSOR_HEIGHT_LOWER;
    }
    if (g_UiCommandModeC == 2) {
      return EDITOR_CURSOR_REBUILD_INFLUENCE;
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (g_UiCommandModeD != 3) {
      if (g_UiCommandModeD == 1) {
        return EDITOR_CURSOR_MATERIAL_MODE1;
      }
      if (g_UiCommandModeD == 2) {
        return EDITOR_CURSOR_MATERIAL_MODE2;
      }
      return EDITOR_CURSOR_PAINT;
    }
    break;
  case EDITOR_MODE_TERRAIN_SMOOTHING:
    if (g_UiCommandModeE == 0) {
      return EDITOR_CURSOR_SMOOTH;
    }
    if (g_UiCommandModeE == 1) {
      return EDITOR_CURSOR_RECEIVER_MASK;
    }
    return EDITOR_CURSOR_PAINT;
  case EDITOR_MODE_UNIT_PLACEMENT:
  case EDITOR_MODE_OBJECT_PLACEMENT:
    /* Mode 3 uses sub-mode A, mode 4 sub-mode B; the rest is shared. The preview instance below is
       created from the unit-placement army (g_UiCommandModeGArmyAssetId) in object placement too, as in
       the original. Sub-mode 0 places, 1 deletes, 2 moves. */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      placementSubMode = g_UiCommandModeA;
    }
    else {
      placementSubMode = g_UiCommandModeB;
    }
    if (placementSubMode == 1) {
      if (ownerNodeUnderPointer != nullptr) {
        return EDITOR_CURSOR_DELETE_TARGET;
      }
      return EDITOR_CURSOR_DELETE_NONE;
    }
    if (placementSubMode == 0) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return WORLD_CURSOR_NO_TARGET;
      }
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      if (localSelectionBlock->placedArmyToken == 0) {
        /* nothing picked up: test a temporary instance at the pointer */
        previewArmyRuntime = ArmyRuntime_CreateInstanceFromAsset
                          (1,0,pointerWorldXQ12,pointerWorldYQ12,g_UiCommandModeGOwnerFactionIndex,
                           g_UiCommandModeGArmyAssetId,worldRuntime,nullptr);
        if (previewArmyRuntime == nullptr) {
          return WORLD_CURSOR_MOVE;
        }
        cursorCode = WORLD_CURSOR_MOVE;
        /* the dispatch reads the army slot's first dword (its model runtime) through the holder view */
        callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback
                          (reinterpret_cast<Ptr32<ArmyRuntimeSlot> *>(previewArmyRuntime),worldRuntime);
        if (callbackAccepted) {
          cursorCode = WORLD_CURSOR_NO_TARGET;
        }
        ArmyRuntime_DestroyInstanceAndRefreshUi
                  (worldRuntime,reinterpret_cast<GameEntityRuntime *>(previewArmyRuntime)); /* entity view of the army */
        return cursorCode;
      }
      cursorCode = WORLD_CURSOR_MOVE;
    }
    else {
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      if (localSelectionBlock->placedArmyToken == 0) {
        if (ownerNodeUnderPointer != nullptr) {
          return WORLD_CURSOR_OWN_ARMY;
        }
        return WORLD_CURSOR_FOREIGN_ARMY;
      }
      cursorCode = WORLD_CURSOR_OWN_ARMY;
    }
    /* an army is picked up: test it instead (the next cursor frame when the test accepts) */
    callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback
                      (reinterpret_cast<Ptr32<ArmyRuntimeSlot> *>
                       (ArmyRuntime_FromToken(static_cast<int32_t>(localSelectionBlock->placedArmyToken))),
                       worldRuntime);
    if (callbackAccepted) {
      return cursorCode + 1;
    }
    return cursorCode;
  case EDITOR_MODE_REGION:
    return EDITOR_CURSOR_REGION;
  }
  return 0;
}

/* World point under the pointer to grid point (Q12, not snapped), used throughout the editor callbacks:
   t = y * -0x20C8CC / 2^21, gx = x * 0x1C6E9C / 2^20 - t, gy = 2t (64-bit products). INGAME_SNAP_GRID_Q12
   (+ 0x3FF & ~0xFFF) snaps a value to the grid. */
static void InGameEditorPointer_GetGridPoint(Q12 pointerX,Q12 pointerY,FieldGridAsset *fieldGrid,
          uint32_t *gridXQ12,uint32_t *gridYQ12)

{
  FixedVectorQ12 nearestTerrainPoint;
  int64_t scaledGridX;
  int64_t scaledGridY;
  uint32_t halfGridY;

  FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,fieldGrid,&nearestTerrainPoint);
  scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  halfGridY = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
  *gridXQ12 = FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - halfGridY;
  *gridYQ12 = halfGridY * 2;
}

/* Flags of the field cell at the (rounded) grid point; false when the point lies outside the field. */
static Bool8 InGameEditorPointer_GetCellFlags(FieldGridAsset *fieldGrid,uint32_t gridXQ12,uint32_t gridYQ12,
          uint32_t *cellFlags)

{
  int cellX;
  int cellY;

  cellX = (int)(gridXQ12 + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
  if (cellX < 0) {
    return false;
  }
  cellY = (int)(gridYQ12 + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
  if ((cellY < 0) || ((int)fieldGrid->gridWidth <= cellX) || ((int)fieldGrid->gridHeight <= cellY)) {
    return false;
  }
  *cellFlags = (uint32_t)fieldGrid->cells[(int32_t)(cellY * fieldGrid->gridWidth + cellX)].flagsAndMaterial;
  return true;
}

/* Starts a height or smoothing drag: remembers the press position on screen and the snapped grid point under
   the pointer as the drag anchor. */
static void InGameEditorPointer_AnchorDrag(Q12 pointerX,Q12 pointerY,WorldRuntimeExtendedMapControlView *mapControl)

{
  uint32_t gridXQ12;
  uint32_t gridYQ12;

  g_UiCommandDragStartScreenX = mapControl->pointerPressX;
  g_UiCommandDragStartScreenY = mapControl->pointerPressY;
  InGameEditorPointer_GetGridPoint(pointerX,pointerY,mapControl->fieldGrid,&gridXQ12,&gridYQ12);
  g_UiCommandDragAnchorWorldXQ12 = INGAME_SNAP_GRID_Q12(gridXQ12);
  g_UiCommandDragAnchorWorldYQ12 = INGAME_SNAP_GRID_Q12(gridYQ12);
}

/* Height tools raise/lower (tool C 0 and 1): hides the surface marker and anchors the drag at the pressed grid
   point, clearing the player's scratch plane; a press beside the terrain marks the drag as not started. */
static void InGameEditorPointer_BeginHeightDrag
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,
          WorldRuntimeExtendedMapControlView *mapControl)

{
  mapControl->runtimeFlags = mapControl->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
    g_UiCommandDragStartScreenX = WORLD_POINTER_NO_HIT;
    return;
  }
  InGameEditorPointer_AnchorDrag(pointerX,pointerY,mapControl);
  InGameCommand_Issue<FieldGrid_ClearPlayerScratchPlane>(0,0,0);
}

/* Material tools other than 3 (tool D): 1 seeds a replacement of the matching region at the pressed grid
   point, 2 a replacement of the non-target region there, every other tool copies the cell material bytes. */
static void InGameEditorPointer_BeginMaterialEdit
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,
          WorldRuntimeExtendedMapControlView *mapControl)

{
  uint32_t gridXQ12;
  uint32_t gridYQ12;
  uint32_t snappedGridX;
  uint32_t snappedGridY;

  if ((g_UiCommandModeD != 1) && (g_UiCommandModeD != 2)) {
    InGameCommand_Issue<TerrainEditBuffer_CopyCellMaterialBytes>(0,0,0);
    return;
  }
  if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
    return;
  }
  InGameEditorPointer_GetGridPoint(pointerX,pointerY,mapControl->fieldGrid,&gridXQ12,&gridYQ12);
  snappedGridX = INGAME_SNAP_GRID_Q12(gridXQ12);
  snappedGridY = INGAME_SNAP_GRID_Q12(gridYQ12);
  if (g_UiCommandModeD == 1) {
    InGameCommand_Issue<TerrainMaterialEdit_SeedMatchingRegionReplacement>
              (g_UiCommandAbsoluteSelectionIndex,snappedGridY,snappedGridX);
    return;
  }
  InGameCommand_Issue<TerrainMaterialEdit_SeedNonTargetRegionReplacement>
            (g_UiCommandAbsoluteSelectionIndex,snappedGridY,snappedGridX);
}

/* Smoothing tab (tool E): 0 anchors a smoothing drag; 1 and the other tools sample the fluid receiver or
   source exclusion flag of the pressed cell. The drag sets the flag when the pressed cell lacks it and clears
   it otherwise; outside the field it sets it. */
static void InGameEditorPointer_BeginSmoothingTool
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,
          WorldRuntimeExtendedMapControlView *mapControl)

{
  uint32_t exclusionFlag;
  uint32_t gridXQ12;
  uint32_t gridYQ12;
  uint32_t cellFlags;

  if (g_UiCommandModeE == 0) {
    if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
      InGameEditorPointer_AnchorDrag(pointerX,pointerY,mapControl);
      return;
    }
    g_UiCommandDragStartScreenX = WORLD_POINTER_NO_HIT;
    return;
  }
  if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
    return;
  }
  if (g_UiCommandModeE == 1) {
    exclusionFlag = FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
  }
  else {
    exclusionFlag = FIELD_CELL_FLUID_SOURCE_EXCLUDED;
  }
  InGameEditorPointer_GetGridPoint(pointerX,pointerY,mapControl->fieldGrid,&gridXQ12,&gridYQ12);
  if (InGameEditorPointer_GetCellFlags(mapControl->fieldGrid,gridXQ12,gridYQ12,&cellFlags)) {
    g_UiCommandTerrainMaskToggleValue = cellFlags & exclusionFlag ^ exclusionFlag;
  }
  else {
    g_UiCommandTerrainMaskToggleValue = exclusionFlag;
  }
}

/* Unit placement (sub-mode A, the mode-G army for its owner faction) and object placement (sub-mode B, the
   mode-4 army): sub-mode 0 places the army at the pointer, 1 deletes the army under the pointer, 2 picks it
   up for moving. With no army under the pointer, 1 and 2 start a drag selection instead. */
static void InGameEditorPointer_BeginPlacementTool
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeExtendedMapControlView *mapControl)

{
  CommandPayload placementFaction;
  CommandPayload armyToken;
  PckArmyAssetIdCatalog lookupToken;
  uint32_t placementSubMode;

  if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
    placementFaction = g_UiCommandModeGOwnerFactionIndex;
    lookupToken = g_UiCommandModeGArmyAssetId;
    placementSubMode = g_UiCommandModeA;
  }
  else {
    placementFaction = 0;
    lookupToken = g_UiCommandMode4ArmyAssetId;
    placementSubMode = g_UiCommandModeB;
  }
  if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
    return;
  }
  if (placementSubMode == 0) {
    InGameCommand_Issue<PlayerRuntime_SetPlacementFaction>(0,0,placementFaction);
    InGameCommand_Issue<PlayerRuntime_CreatePlacementArmy>(pointerX,pointerY,lookupToken);
    g_UiCommandDragReferenceX = pointerY;
    g_UiCommandDragReferenceY = pointerX;
    g_UiCommandDragStartScreenX = mapControl->pointerPressX;
    g_UiCommandDragStartScreenY = mapControl->pointerPressY;
    return;
  }
  if (ownerNodeUnderPointer != nullptr) {
    if (placementSubMode == 1) {
      armyToken = ArmyRuntime_Token
                  (WorldOwnerNode_ModelRuntime(ownerNodeUnderPointer)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
      InGameCommand_Issue<FrontendPlayerSelection_ApplyEntryOrAll>(0,0,armyToken);
      return;
    }
    g_UiCommandDragStartScreenX = mapControl->pointerPressX;
    g_UiCommandDragStartScreenY = mapControl->pointerPressY;
    armyToken = ArmyRuntime_Token
                (WorldOwnerNode_ModelRuntime(ownerNodeUnderPointer)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    InGameCommand_Issue<PlayerRuntime_SetPlacementArmy>(0,0,armyToken);
    g_UiCommandDragReferenceX = pointerY;
    g_UiCommandDragReferenceY = pointerX;
    return;
  }
  /* no army under the pointer: start a drag selection instead */
  mapControl->runtimeFlags = mapControl->runtimeFlags | WORLD_RUNTIME_FLAG_DRAG_SELECTING;
  InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
}

/* Region tab: region g_UiCommandModeF owns cell flag FIELD_CELL_XENITE_SUPPORT << region; when the pressed cell
   has it, bit 31 (INGAME_REGION_MASK_REMOVE) makes the drag remove it again, otherwise (also outside the
   field) the drag adds it. */
static void InGameEditorPointer_BeginRegionToggle
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,
          WorldRuntimeExtendedMapControlView *mapControl)

{
  uint32_t gridXQ12;
  uint32_t gridYQ12;
  uint32_t cellFlags;

  if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
    return;
  }
  InGameEditorPointer_GetGridPoint(pointerX,pointerY,mapControl->fieldGrid,&gridXQ12,&gridYQ12);
  if (InGameEditorPointer_GetCellFlags(mapControl->fieldGrid,gridXQ12,gridYQ12,&cellFlags) &&
      ((cellFlags & FIELD_CELL_XENITE_SUPPORT << ((uint8_t)g_UiCommandModeF & SHIFT_COUNT_MASK)) != 0)) {
    g_UiCommandCallerMaskHighBit = INGAME_REGION_MASK_REMOVE;
    return;
  }
  g_UiCommandCallerMaskHighBit = 0;
}

/* Map editor pointer press (selection.beginPointerCaptureCallback of the world runtime while the editor is
   active): starts the action of the active tab and tool - anchors a height or smoothing drag at the grid
   point under the pointer, seeds a material replacement, samples the fluid or region flag the drag will
   toggle, places, deletes or picks up an army, or (other tools) starts a rectangle selection. Editor
   commands go through the command queue in network games.
*/
void InGameUiCommand_BeginInteractionByMode
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,uint32_t reservedArg3,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeExtendedMapControlView *mapControl
          )

{
  uint32_t gridXQ12;
  uint32_t gridYQ12;

  if ((ownerNodeUnderPointer != nullptr) &&
     (ownerNodeUnderPointer->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) {
    ownerNodeUnderPointer = nullptr;
  }
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if ((g_UiCommandModeC == 0) || (g_UiCommandModeC == 1)) {
      InGameEditorPointer_BeginHeightDrag(pointerRegionCode,pointerX,pointerY,mapControl);
      return;
    }
    if (g_UiCommandModeC == 2) {
      InGameCommand_Issue<FieldGrid_ResetLocalInfluenceState>(0,0,0);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (g_UiCommandModeD != 3) {
      InGameEditorPointer_BeginMaterialEdit(pointerRegionCode,pointerX,pointerY,mapControl);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_SMOOTHING:
    InGameEditorPointer_BeginSmoothingTool(pointerRegionCode,pointerX,pointerY,mapControl);
    return;
  case EDITOR_MODE_UNIT_PLACEMENT:
  case EDITOR_MODE_OBJECT_PLACEMENT:
    InGameEditorPointer_BeginPlacementTool(pointerRegionCode,pointerX,pointerY,ownerNodeUnderPointer,mapControl);
    return;
  case EDITOR_MODE_REGION:
    InGameEditorPointer_BeginRegionToggle(pointerRegionCode,pointerX,pointerY,mapControl);
    return;
  }
  /* remaining tools: rectangle selection of grid cells; without Shift/Ctrl a new selection replaces the old */
  g_UiCommandSelectionAnchorWorldXQ12 = WORLD_POINTER_NO_HIT;
  if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      InGameCommand_Issue<SelectionPlayerRuntime_ClearTerrainEditSelectionState>(0,0,0);
    }
    InGameEditorPointer_GetGridPoint(pointerX,pointerY,mapControl->fieldGrid,&gridXQ12,&gridYQ12);
    g_UiCommandSelectionAnchorWorldXQ12 = gridXQ12;
    g_UiCommandSelectionAnchorWorldYQ12 = gridYQ12;
    g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    g_UiCommandSelectionCurrentWorldYQ12 = g_UiCommandSelectionAnchorWorldYQ12;
  }
}

/* Army drag selection step of InGameUiCommand_UpdateInteractionByMode: collects the own rendered armies
   inside the drag rectangle that are not selected yet (insert triplets) and those outside that are selected
   (remove triplets), skipping armies whose command is already queued, then sends both lists three entries
   per command, directly or through the command queue. */
static void InGameEditorPointer_UpdateArmyDragSelection(WorldRuntimeExtendedMapControlView *mapControl)
{
  int clearIndex;
  WorldOwnerListNode *runtimeNode;
  int ownerFactionIndex;
  GameEntityRuntime *entry;
  InGameCommandPayloadTripletValue32 payloadValue;
  Bool8 isEntryAbsent;
  uint32_t tripletDwordCount;
  CommandPayload *tripletEntry;

  /* clears both triplet buffers, then their counts, in memory order (the original clears the 26 dwords in
     one run) */
  for (clearIndex = 0; clearIndex < 12; clearIndex++) {
    g_InGameSelectionInsertTripletDwords[clearIndex] = 0;
  }
  for (clearIndex = 0; clearIndex < 12; clearIndex++) {
    g_InGameSelectionRemoveTripletDwords[clearIndex] = 0;
  }
  g_InGameSelectionInsertTripletDwordCount = 0;
  g_InGameSelectionRemoveTripletDwordCount = 0;
  ownerFactionIndex = mapControl->activeFactionRuntimeIndex;
  for (runtimeNode = mapControl->ownerListHead; runtimeNode != nullptr;
      runtimeNode = runtimeNode->nextNode) {
    if (!Any(runtimeNode->runtimeFlags & MODEL_NODE_FLAG_RENDERED)) continue;
    static_assert(offsetof(ModelRuntimeSlot,ownerArmyRuntimeOrSavedOffset) == 8,
                  "the owner army is the dword at payload + 8");
    entry = WorldOwnerNode_ModelRuntime(runtimeNode)->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    if (!Any(runtimeNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED) ||
        ownerFactionIndex != (entry->common).ownership.ownerIndex) continue;
    payloadValue = ArmyRuntime_Token(entry);
    if (WorldRuntimeNode_IsPositionInsideBounds(runtimeNode,mapControl)) {
      isEntryAbsent = SelectionInfo_IsEntryAbsent(entry);
      tripletDwordCount = g_InGameSelectionInsertTripletDwordCount;
      if (isEntryAbsent &&
          !InGameCommandQueue_ContainsTripletValue(payloadValue,
                                                   INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_INSERT)) {
        /* The value is stored before the count check: once the count has reached 11, further values keep
           overwriting the slot at that count. */
        g_InGameSelectionInsertTripletDwords[tripletDwordCount] = payloadValue;
        if (tripletDwordCount < 11) {
          g_InGameSelectionInsertTripletDwordCount++;
        }
      }
    }
    else {
      isEntryAbsent = SelectionInfo_IsEntryAbsent(entry);
      tripletDwordCount = g_InGameSelectionRemoveTripletDwordCount;
      if (!isEntryAbsent &&
          !InGameCommandQueue_ContainsTripletValue(payloadValue,
                                                   INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_REMOVE)) {
        g_InGameSelectionRemoveTripletDwords[tripletDwordCount] = payloadValue;
        if (tripletDwordCount < 11) {
          g_InGameSelectionRemoveTripletDwordCount++;
        }
      }
    }
  }
  if (g_InGameSelectionRemoveTripletDwordCount != 0) {
    tripletEntry = g_InGameSelectionRemoveTripletDwords;
    do {
      InGameCommand_Issue<FrontendPlayerSelection_RemoveThreeEntriesAndRefresh>
                (tripletEntry[2],tripletEntry[1],*tripletEntry);
      tripletDwordCount = g_InGameSelectionRemoveTripletDwordCount;
      tripletEntry += 3;
      g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount - 3;
    } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)tripletDwordCount);
  }
  if (g_InGameSelectionInsertTripletDwordCount != 0) {
    tripletEntry = g_InGameSelectionInsertTripletDwords;
    do {
      InGameCommand_Issue<FrontendPlayerSelection_InsertThreeEntriesAndRefresh>
                (tripletEntry[2],tripletEntry[1],*tripletEntry);
      tripletDwordCount = g_InGameSelectionInsertTripletDwordCount;
      tripletEntry += 3;
      g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount - 3;
    } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)tripletDwordCount);
  }
}

/* Terrain point under the pointer, as world X and Y snapped to the field grid (Q12). */
static void InGameEditorPointer_GetSnappedTerrainCell(GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,WorldRuntimeExtendedMapControlView *mapControl,
          uint32_t *outWorldXQ12,uint32_t *outWorldYQ12)
{
  FixedVectorQ12 nearestTerrainPoint;
  int64_t scaledGridX;
  int64_t scaledGridY;
  uint32_t halfWorldY;

  FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
  scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  halfWorldY = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
  *outWorldXQ12 = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - halfWorldY);
  *outWorldYQ12 = INGAME_SNAP_GRID_Q12(halfWorldY * 2);
}

/* Screen drag since the drag start packed for the height tools: the X distance (dropped while Shift or Ctrl
   is held) in the low bits, the Y distance scaled above it. */
static uint32_t InGameEditorPointer_PackedDragDelta(WorldRuntimeExtendedMapControlView *mapControl)
{
  uint32_t deltaXMask;

  deltaXMask = INGAME_DRAG_DELTA_X_MASK;
  if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) != 0) {
    deltaXMask = 0;
  }
  return mapControl->pointerX - g_UiCommandDragStartScreenX & deltaXMask |
         (mapControl->pointerY - g_UiCommandDragStartScreenY) * INGAME_DRAG_DELTA_Y_SCALE;
}

/* Cell rectangle selection step of InGameUiCommand_UpdateInteractionByMode: moves the current corner to the
   terrain point under the pointer, deselects the rectangle anchor..old corner row by row and selects anchor..new
   corner, directly or through the command queue. */
static void InGameEditorPointer_ResizeCellRectangle(GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,WorldRuntimeExtendedMapControlView *mapControl)
{
  FixedVectorQ12 nearestTerrainPoint;
  int64_t scaledGridX;
  int64_t scaledGridY;
  uint32_t halfWorldY;
  int newCornerWorldX;
  int newCornerWorldY;
  int lowWorldX;
  int highWorldX;
  int lowWorldY;
  int highWorldY;
  uint32_t firstColumnQ12;
  uint32_t lastColumnQ12;
  CommandPayload rowQ12;

  FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
  scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  halfWorldY = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
  newCornerWorldX = (FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - halfWorldY;
  newCornerWorldY = halfWorldY * 2;
  /* deselect anchor..old corner */
  lowWorldX = g_UiCommandSelectionAnchorWorldXQ12;
  if ((int)g_UiCommandSelectionCurrentWorldXQ12 < (int)g_UiCommandSelectionAnchorWorldXQ12) {
    lowWorldX = g_UiCommandSelectionCurrentWorldXQ12;
    g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
  }
  highWorldY = g_UiCommandSelectionCurrentWorldYQ12;
  lowWorldY = g_UiCommandSelectionAnchorWorldYQ12;
  if ((int)g_UiCommandSelectionCurrentWorldYQ12 < (int)g_UiCommandSelectionAnchorWorldYQ12) {
    highWorldY = g_UiCommandSelectionAnchorWorldYQ12;
    lowWorldY = g_UiCommandSelectionCurrentWorldYQ12;
  }
  firstColumnQ12 = INGAME_SNAP_GRID_Q12(lowWorldX);
  rowQ12 = INGAME_SNAP_GRID_Q12(lowWorldY);
  lastColumnQ12 = INGAME_SNAP_GRID_Q12(g_UiCommandSelectionCurrentWorldXQ12);
  g_UiCommandSelectionCurrentWorldXQ12 = newCornerWorldX;
  g_UiCommandSelectionCurrentWorldYQ12 = newCornerWorldY;
  if ((int)firstColumnQ12 <= (int)lastColumnQ12) {
    for (; (int)rowQ12 <= (int)INGAME_SNAP_GRID_Q12(highWorldY); rowQ12 = rowQ12 + Q12_ONE) {
      InGameCommand_Issue<PlayerPairList_RemoveRange>(lastColumnQ12,rowQ12,firstColumnQ12);
    }
  }
  /* select anchor..new corner */
  lowWorldX = g_UiCommandSelectionAnchorWorldXQ12;
  highWorldX = g_UiCommandSelectionCurrentWorldXQ12;
  if ((int)g_UiCommandSelectionCurrentWorldXQ12 < (int)g_UiCommandSelectionAnchorWorldXQ12) {
    lowWorldX = g_UiCommandSelectionCurrentWorldXQ12;
    highWorldX = g_UiCommandSelectionAnchorWorldXQ12;
  }
  highWorldY = g_UiCommandSelectionCurrentWorldYQ12;
  lowWorldY = g_UiCommandSelectionAnchorWorldYQ12;
  if ((int)g_UiCommandSelectionCurrentWorldYQ12 < (int)g_UiCommandSelectionAnchorWorldYQ12) {
    highWorldY = g_UiCommandSelectionAnchorWorldYQ12;
    lowWorldY = g_UiCommandSelectionCurrentWorldYQ12;
  }
  firstColumnQ12 = INGAME_SNAP_GRID_Q12(lowWorldX);
  rowQ12 = INGAME_SNAP_GRID_Q12(lowWorldY);
  lastColumnQ12 = INGAME_SNAP_GRID_Q12(highWorldX);
  if ((int)firstColumnQ12 <= (int)lastColumnQ12) {
    for (; (int)rowQ12 <= (int)INGAME_SNAP_GRID_Q12(highWorldY); rowQ12 = rowQ12 + Q12_ONE) {
      InGameCommand_Issue<PlayerPairList_InsertRange>(lastColumnQ12,rowQ12,firstColumnQ12);
    }
  }
}

/* Map editor pointer drag (selection.updateDragSelectionCallback of the world runtime while the editor is
   active). During an army drag selection it adds the own armies inside the rectangle to the selection and
   removes those outside, three per command and skipping armies already queued. Otherwise it continues the
   action of the active tool: raises, lowers or smooths heights by the screen drag, paints material,
   rebuilds the influence, toggles fluid or region flags, moves or rotates a picked-up army, or grows the
   cell rectangle selection (deselecting the old rectangle, selecting the new one row by row).
*/
void InGameUiCommand_UpdateInteractionByMode(UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,uint32_t reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView *mapControl)

{
  uint32_t placementSubMode;
  uint32_t packedDragDelta;
  uint32_t deltaXMask;
  uint32_t dragDeltaX;
  int dragDeltaY;
  uint32_t cellWorldXQ12;
  uint32_t cellWorldYQ12;
  CommandPayload pointerYMoveDelta;
  CommandPayload pointerXMoveDelta;
  int rotateDragDistanceX;

  if ((mapControl->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    InGameEditorPointer_UpdateArmyDragSelection(mapControl);
    return;
  }
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if (g_UiCommandModeC == 0) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      packedDragDelta = InGameEditorPointer_PackedDragDelta(mapControl);
      InGameCommand_Issue<FieldGrid_ApplyPositiveCellDeltas>
                (g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,packedDragDelta);
      return;
    }
    if (g_UiCommandModeC == 1) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      packedDragDelta = InGameEditorPointer_PackedDragDelta(mapControl);
      InGameCommand_Issue<FieldGrid_ApplyNegativeCellDeltas>
                (g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,packedDragDelta);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      InGameEditorPointer_GetSnappedTerrainCell(pointerX,pointerY,mapControl,&cellWorldXQ12,&cellWorldYQ12);
      InGameCommand_Issue<FieldGrid_RebuildLocalInfluenceState>(0,cellWorldYQ12,cellWorldXQ12);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (g_UiCommandModeD != 3) {
      if (g_TerrainMaterialTextureSets[g_UiCommandAbsoluteSelectionIndex] ==
          nullptr) {
        return;
      }
      if (g_UiCommandModeD == 1) {
        return;
      }
      if (g_UiCommandModeD == 2) {
        return;
      }
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      InGameEditorPointer_GetSnappedTerrainCell(pointerX,pointerY,mapControl,&cellWorldXQ12,&cellWorldYQ12);
      InGameCommand_Issue<FieldGrid_ApplyLocalCellUpdate>
                (g_UiCommandAbsoluteSelectionIndex,cellWorldYQ12,cellWorldXQ12);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_SMOOTHING:
    if (g_UiCommandModeE == 0) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      deltaXMask = INGAME_DRAG_DELTA_X_MASK;
      if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) != 0) {
        deltaXMask = 0;
      }
      dragDeltaX = mapControl->pointerX - g_UiCommandDragStartScreenX;
      dragDeltaY = mapControl->pointerY - g_UiCommandDragStartScreenY;
      g_UiCommandDragStartScreenX = g_UiCommandDragStartScreenX + dragDeltaX;
      g_UiCommandDragStartScreenY = g_UiCommandDragStartScreenY + dragDeltaY;
      packedDragDelta = dragDeltaX & deltaXMask | dragDeltaY * INGAME_DRAG_DELTA_Y_SCALE;
      InGameCommand_Issue<FieldGrid_ApplyEncodedCellUpdate>
                (g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,packedDragDelta);
      return;
    }
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    InGameEditorPointer_GetSnappedTerrainCell(pointerX,pointerY,mapControl,&cellWorldXQ12,&cellWorldYQ12);
    if (g_UiCommandModeE != 1) {
      InGameCommand_Issue<FieldGrid_SetCellFluidSourceExcluded>
                (g_UiCommandTerrainMaskToggleValue,cellWorldYQ12,cellWorldXQ12);
      return;
    }
    InGameCommand_Issue<FieldGrid_SetCellFluidReceiverExcluded>
              (g_UiCommandTerrainMaskToggleValue,cellWorldYQ12,cellWorldXQ12);
    return;
  case EDITOR_MODE_UNIT_PLACEMENT:
  case EDITOR_MODE_OBJECT_PLACEMENT:
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      placementSubMode = g_UiCommandModeA;
    }
    else {
      placementSubMode = g_UiCommandModeB;
    }
    if (placementSubMode == 1) {
      return;
    }
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    if ((g_CursorButtonState & 4) != 0) {
      g_UiCommandDragStartScreenX = mapControl->pointerX;
      g_UiCommandDragStartScreenY = mapControl->pointerY;
      pointerYMoveDelta = pointerY - g_UiCommandDragReferenceX;
      pointerXMoveDelta = pointerX - g_UiCommandDragReferenceY;
      g_UiCommandDragReferenceX = g_UiCommandDragReferenceX + pointerYMoveDelta;
      g_UiCommandDragReferenceY = g_UiCommandDragReferenceY + pointerXMoveDelta;
      InGameCommand_Issue<SelectionPlayerRuntime_MovePrimarySelectionBy>(0,pointerXMoveDelta,pointerYMoveDelta);
      return;
    }
    /* The horizontal drag distance is taken before the pointer is put back to the drag start;
       g_PointerSetPosition returns nothing. */
    rotateDragDistanceX = mapControl->pointerX - g_UiCommandDragStartScreenX;
    g_PointerSetPosition(g_UiCommandDragStartScreenY,g_UiCommandDragStartScreenX);
    InGameCommand_Issue<SelectionPlayerRuntime_RotatePrimarySelectionBy>(0,0,rotateDragDistanceX << 6);
    return;
  case EDITOR_MODE_REGION:
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    InGameEditorPointer_GetSnappedTerrainCell(pointerX,pointerY,mapControl,&cellWorldXQ12,&cellWorldYQ12);
    InGameCommand_Issue<FieldGrid_SetCellResourceSupportFlag>
              (g_UiCommandModeF | g_UiCommandCallerMaskHighBit,cellWorldYQ12,cellWorldXQ12);
    return;
  }
  if ((pointerRegionCode != WORLD_POINTER_NO_HIT) && (g_UiCommandSelectionAnchorWorldXQ12 != WORLD_POINTER_NO_HIT)) {
    InGameEditorPointer_ResizeCellRectangle(pointerX,pointerY,mapControl);
  }
}

/* Map editor pointer release (selection.commitPointerActionCallback of the world runtime while the editor is
   active): ends the drag selection and finishes the tool's action - shows the surface point marker again
   after a height drag, converts the influence edit to height deltas, subtracts the painted materials, or drops the
   placed or moved army.
*/
void InGameUiCommand_EndInteractionByMode
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3,
          WorldOwnerListNode *worldNode,WorldRuntimeContext *worldRuntime)

{
  uint32_t activeMode;
  uint32_t placementSubMode;

  activeMode = g_UiCommandModeG;
  worldRuntime->runtimeFlags = worldRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAG_SELECTING;
  switch(activeMode) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if (g_UiCommandModeC == 0) {
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
    }
    else if (g_UiCommandModeC == 1) {
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
    }
    else if (g_UiCommandModeC == 2) {
      InGameCommand_Issue<TerrainEditBuffer_ConvertHeightsToDeltas>(0,0,0);
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (((g_UiCommandModeD != 3) && (g_UiCommandModeD != 1)) && (g_UiCommandModeD != 2)) {
      InGameCommand_Issue<TerrainEditBuffer_SubtractCurrentCellMaterialBytes>(0,0,0);
    }
    break;
  case EDITOR_MODE_UNIT_PLACEMENT:
  case EDITOR_MODE_OBJECT_PLACEMENT:
    if (activeMode == EDITOR_MODE_UNIT_PLACEMENT) {
      placementSubMode = g_UiCommandModeA;
    }
    else {
      placementSubMode = g_UiCommandModeB;
    }
    if (placementSubMode != 1) {
      InGameCommand_Issue<PlayerRuntime_ClearPlacementArmy>(0,0,0);
    }
  }
}

/* Map editor context action (selection.dispatchWorldContextActionCallback of the world runtime while the
   editor is active): clears the cell selection on the height and material tabs and the army selection on
   the unit placement tab.
*/
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime)

{
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    InGameCommand_Issue<SelectionPlayerRuntime_ClearTerrainEditSelectionState>(0,0,0);
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    InGameCommand_Issue<SelectionPlayerRuntime_ClearTerrainEditSelectionState>(0,0,0);
    break;
  case EDITOR_MODE_UNIT_PLACEMENT:
    InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
  }
}

/* Slot adapters for the callbacks installed below whose own signature differs from the slot's (calling through
   the slot type directly would be undefined behaviour). Each passes its arguments on unchanged: the uint32_t
   slot values become the handlers' int-typed codes and Q12/screen coordinates bit for bit, the world runtime
   is the same object under the handler's view type. */

static uint32_t EditorSlot_ResolveCursorCodeByMode
          (uint32_t pointerRegionCode,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,uint32_t reservedArg3,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeContext *worldRuntime)

{
  return InGameUiCommand_ResolveCursorCodeByMode
                   ((UiPointerRegionCode)pointerRegionCode,(Q12)pointerWorldXQ12,(Q12)pointerWorldYQ12,
                    reservedArg3,ownerNodeUnderPointer,worldRuntime);
}

static void EditorSlot_BeginInteractionByMode
          (uint32_t pointerRegionCode,uint32_t pointerX,uint32_t pointerY,uint32_t reservedArg3,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeContext *worldRuntime)

{
  /* the mode handlers take the world runtime through its extended map-control view (same object) */
  InGameUiCommand_BeginInteractionByMode
            ((UiPointerRegionCode)pointerRegionCode,(Q12)pointerX,(Q12)pointerY,reservedArg3,
             ownerNodeUnderPointer,reinterpret_cast<WorldRuntimeExtendedMapControlView *>(worldRuntime));
}

/* The handler's fifth parameter (optionalContext, unused) receives the node pointer's low 32 bits. */
static void EditorSlot_UpdateInteractionByMode
          (uint32_t pointerRegionCode,uint32_t pointerX,uint32_t pointerY,uint32_t reservedArg3,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeContext *worldRuntime)

{
  /* the mode handlers take the world runtime through its extended map-control view (same object) */
  InGameUiCommand_UpdateInteractionByMode
            ((UiPointerRegionCode)pointerRegionCode,(GraphicsScreenCoordinate)pointerX,
             (GraphicsScreenCoordinate)pointerY,reservedArg3,(int)(intptr_t)ownerNodeUnderPointer,
             reinterpret_cast<WorldRuntimeExtendedMapControlView *>(worldRuntime));
}

static void EditorSlot_ClearTransientStateNoOp(WorldRuntimeContext *worldRuntime)

{
  (void)worldRuntime;
  UiCommandRuntime_CallbackNoOp();
}

/* The editor keyboard fallback returns nothing; UiKeyboard_DispatchPendingEvents ignores the slot's result. */
static Bool8 EditorSlot_KeyboardFallback(UiKeyboardStateMask keyboardStateMask,UiActionId keyCode,UiRootNode *uiRoot)

{
  InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
            (keyboardStateMask,(uint32_t)keyCode,uiRoot);
  return false;
}

static void EditorSlot_RebuildTerrainOccupancyAndVisualState(void *callbackContext,WorldOwnerListNode *node)

{
  ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback(static_cast<WorldRuntimeContext *>(callbackContext),node);
}

static void EditorSlot_ResetNotificationButtonCursor(WorldRuntimeContext *worldRuntime)

{
  InGameUiRuntime_ResetNotificationButtonCursor(worldRuntime);
}

static Bool8 EditorSlot_HotkeysKeyboardFallback
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,UiRootNode *uiRoot)

{
  return InGameHotkeys_DispatchCommandByFlags
           (modifierFlags,commandCode,THANDOR_CONTAINER_OF(uiRoot,InGameRuntimeRootFrameView,rootUi));
}

/* In-game command INGAME_COMMAND_EDITOR_ACTIVE_STATE: enters or (EDITOR_ACTIVE_STATE_LEAVE) leaves the map
   editor. Entering pauses the game, switches the side panel, resource bar and game panels to the editor
   pages, installs the editor callbacks (InGameUiCommand_*ByMode, camera keys, editor hotkeys) on the world
   runtime and the UI root, selects the current editor tab, clears the notification queue, stops the movie,
   restores the material and army previews and resets the terrain lighting and per-cell army/resource
   offsets. Leaving restores the game pages and callbacks, frees the army preview textures, rebuilds occupancy
   and terrain display flags and the lighting, and clears the hovered record. Each path runs only when the
   state actually changes (UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE).
*/
void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t activeStateFlags)

{
  WorldRuntimeFlags *runtimeFlagsField;
  WorldRuntimeContext *node;
  GraphicsTextureSet *materialTextureSet;
  ArmyAssetRecordPrefix *armyAsset;
  UiPageStackControl *modePreviewPageStack;
  InGameRuntimeRoot *root;
  InGameUiImage *image;
  uint32_t editorMode;
  uint32_t materialIndex;
  int remainingCount;
  int index;
  GraphicsTextureSourceAsset *panelTextureSource;
  GraphicsTextureSourceAsset *swatchTextureSource;
  uint32_t *queueDwords;
  FieldGridCell *fieldCellCursor;
  uint32_t activePageIndex;
  FieldGridAsset *worldFieldGrid;

  editorMode = g_UiCommandModeG;
  root = g_InGameRuntimeRoot;
  image = InGameUi_Image(root);
  if ((activeStateFlags & EDITOR_ACTIVE_STATE_LEAVE) == 0) {
    if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE)) {
      modePreviewPageStack =
           UiLayoutContainerControl_AsPageStack(&InGameUi_Image(g_InGameRuntimeRoot)->modePreviewPageStack);
      g_UiCommandRuntimeFlags =
           g_UiCommandRuntimeFlags |
           (UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
      runtimeFlagsField = &(g_InGameRuntimeRoot->worldRuntime).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | INGAME_WORLD_FLAG_EDITOR; /* cleared on leaving */
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGPrimaryPageIndices[editorMode],modePreviewPageStack);
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGSecondaryPageIndices[editorMode],
                 UiLayoutContainerControl_AsPageStack(&image->modeDetailPageStack));
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGTertiaryPageIndices[editorMode],
                 UiLayoutContainerControl_AsPageStack(&image->modeCommandPageStack));
      activePageIndex = UiPageStack_ActivePageIndex(&root->sidePanelPageStack);
      if (activePageIndex == 0) {
        UiPageStack_SetActiveIndex(1,&root->resourceBarModePageStack);
        UiPageStack_SetActiveIndex(1,&root->gamePanelsModePageStack);
      }
      UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&image->sidePanelMenuButtonStack));
      root->worldOverlayCallback = nullptr;
      (root->worldRuntime).selection.dispatchCommandCallback =
           InGameCameraCommand_DispatchByCodeAndModifierFlags;
      /* the mode handlers, the no-op and the keyboard fallback go in through the EditorSlot_ adapters above */
      (root->worldRuntime).selection.resolveContextActionPrimaryCallback = EditorSlot_ResolveCursorCodeByMode;
      (root->worldRuntime).selection.resolveContextActionSecondaryCallback = EditorSlot_ResolveCursorCodeByMode;
      (root->worldRuntime).selection.beginPointerCaptureCallback = EditorSlot_BeginInteractionByMode;
      (root->worldRuntime).selection.updateDragSelectionCallback = EditorSlot_UpdateInteractionByMode;
      (root->worldRuntime).selection.commitPointerActionCallback =
           InGameUiCommand_EndInteractionByMode;
      (root->worldRuntime).fieldRegion.clearTransientStateCallback = EditorSlot_ClearTransientStateNoOp;
      (root->worldRuntime).selection.dispatchWorldContextActionCallback =
           InGameUiCommand_ResetInteractionByMode;
      g_InGameUiRootCallbacks.keyboardFallback = EditorSlot_KeyboardFallback;
      /* InGameCommandModeG_Select0..5, applied to the mode's tab control. */
      g_UiCommandModeGHandlers[editorMode](InGameUi_EditorModeTab(image,editorMode));
      /* zeroes the first 32 dwords of the notification queue (dword by dword, not record by record) */
      queueDwords = reinterpret_cast<uint32_t *>(root->notificationQueue);
      for (index = 0; index < 32; index++) {
        queueDwords[index] = 0;
      }
      Movie_Close();
      materialIndex = g_UiCommandAbsoluteSelectionIndex;
      panelTextureSource = g_InGamePanelTextureSource;
      if (root->notificationButtonCursorFrame == PAYLOAD_ACTIVE) {
        root->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
      }
      materialTextureSet = g_TerrainMaterialTextureSets[materialIndex];
      root->notificationButtonTextureSource = (uintptr_t)panelTextureSource;
      /* preview texture of the selected material */
      swatchTextureSource = nullptr;
      if (materialTextureSet != nullptr) {
        swatchTextureSource = materialTextureSet->entries[0].sourceAsset;
      }
      root->notificationButtonSubresource = INGAME_PANEL_SUBRESOURCE_NOTIFICATION_IDLE;
      image->materialToolSelectedSwatch.textureSource = swatchTextureSource;
      UiCommandMatrix_SelectIndex(g_UiCommandAbsoluteSelectionIndex,&root->rootUi.base);
      g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_NormalizeIdToPlaceableUnit(g_UiCommandModeGArmyAssetId);
      image->unitPlacementPreviewImage.textureSource =
           reinterpret_cast<GraphicsTextureSourceAsset *>
           (ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId));
      g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_NormalizeIdToPlaceableObject(g_UiCommandMode4ArmyAssetId);
      image->objectPlacementPreviewImage.textureSource =
           reinterpret_cast<GraphicsTextureSourceAsset *>
           (ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId));
      FieldGrid_SetOccupancyMaskByteBit0AllCells
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
      WorldRuntime_ForEachOwnerListNode
                (&root->worldRuntime,
                 EditorSlot_RebuildTerrainOccupancyAndVisualState,
                 &root->worldRuntime);
      FieldGrid_ClassifyCellFlagsToRuntimeByte
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
      for (index = 0; index < 256; index++) {
        g_TerrainDirectionRecordTable256[index].angleAComponent0ScaledQ28 = 0;
        g_TerrainDirectionRecordTable256[index].angleAComponent1ScaledQ28 = 0;
        g_TerrainDirectionRecordTable256[index].angleBComponent0ScaledQ28 = 0;
      }
      /* Original quirk: a do-while, so an empty grid would run past the cells */
      worldFieldGrid = (root->worldRuntime).fieldGrid;
      remainingCount = worldFieldGrid->gridWidth * worldFieldGrid->gridHeight;
      fieldCellCursor = worldFieldGrid->cells;
      do {
        fieldCellCursor->armyRuntimeSavedOffset = 0;
        fieldCellCursor->resourceExtractionDescriptor = 0;
        fieldCellCursor++;
        remainingCount--;
      } while (remainingCount != 0);
    }
  }
  else if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE)) {
    g_UiCommandRuntimeFlags =
         g_UiCommandRuntimeFlags &
         ~(UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
    UiPageStack_SetActiveIndex
              (0,UiLayoutContainerControl_AsPageStack(&InGameUi_Image(g_InGameRuntimeRoot)->modePreviewPageStack));
    UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->modeDetailPageStack));
    UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->modeCommandPageStack));
    activePageIndex = UiPageStack_ActivePageIndex(&root->sidePanelPageStack);
    if (activePageIndex == 0) {
      UiPageStack_SetActiveIndex(0,&root->resourceBarModePageStack);
      UiPageStack_SetActiveIndex(0,&root->gamePanelsModePageStack);
    }
    UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->sidePanelMenuButtonStack));
    UiCommandModeG_HideSurfacePointMarker(&root->worldRuntime);
    root->worldOverlayCallback = InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
    (root->worldRuntime).selection.dispatchCommandCallback =
         InGameUiRuntime_DispatchCommandByCodeAndModifierFlags;
    (root->worldRuntime).selection.resolveContextActionPrimaryCallback =
         InGameWorldInput_ResolveContextActionAndCursor;
    (root->worldRuntime).selection.resolveContextActionSecondaryCallback =
         InGameWorldInput_ResolveContextActionAndCursor;
    (root->worldRuntime).selection.beginPointerCaptureCallback =
         InGameWorldInput_BeginPointerCapture;
    (root->worldRuntime).selection.updateDragSelectionCallback =
         InGameWorldInput_UpdateDragSelectionAndCamera;
    (root->worldRuntime).selection.commitPointerActionCallback =
         InGameWorldInput_CommitPointerAction;
    (root->worldRuntime).fieldRegion.clearTransientStateCallback = EditorSlot_ResetNotificationButtonCursor;
    (root->worldRuntime).selection.dispatchWorldContextActionCallback =
         InGameUiRuntime_DispatchWorldContextActionCallback;
    runtimeFlagsField = &(root->worldRuntime).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS;
    g_InGameUiRootCallbacks.keyboardFallback = EditorSlot_HotkeysKeyboardFallback;
    /* free the cached preview textures of all army asset records */
    for (index = 0; index < ARMY_ASSET_REGISTRY_SLOT_COUNT; index++) {
      armyAsset = g_ArmyAssetRecordRegistry[index];
      if (armyAsset != nullptr) {
        g_MemoryApi.free(Thandor_U32ToPointer<void>(armyAsset[2].byteSize)); /* 5f-format: ArmyAssetRecord.previewTexture (+0x20) */
        armyAsset[2].byteSize = 0;
      }
    }
    if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED)) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
    }
    WorldRuntime_ForEachOwnerListNode
              (&root->worldRuntime,
               EditorSlot_RebuildTerrainOccupancyAndVisualState,
               &root->worldRuntime);
    node = &root->worldRuntime;
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((root->worldRuntime).activeFactionRuntimeIndex,(root->worldRuntime).fieldGrid
              );
    UiCommandModeG_HideSurfacePointMarker(node);
    UiCommandModeG_HideTerrainPointMarkers(node);
    UiCommandModeG_ShowArmyMetrics(node);
    UiCommandModeG_ClearSecondarySurfaceOnly(node);
    UiCommandModeG_ApplyRawColorVariant(node);
    UiCommandModeG_HideGridVertexMarkers(node);
    UiCommandModeG_HideRegionMarkers(node);
    runtimeFlagsField = &(root->worldRuntime).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~INGAME_WORLD_FLAG_EDITOR; /* set on entering */
    TerrainDirectionTable_AdvanceAndRebuildVectors();
    g_UiHoverSelectionRecord = nullptr;
    InGameSelectionDetailPanel_Rebuild();
  }
}

/* In-game command INGAME_COMMAND_EDITOR_SAVE_MAP (editor hotkey F2): writes the edited map back - the field
   grid asset image and the level asset image - from the current world state. A failure of either is
   reported through FatalError_ReportIfFailed without stopping the game.
*/
void InGameUiCommand_SaveFieldAndLevelAssetImages
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3)

{
  InGameRuntimeRoot *runtimeRoot;
  uint32_t fieldSaveError;
  uint32_t levelSaveError;

  runtimeRoot = g_InGameRuntimeRoot;
  if (!FieldGrid_SaveAssetImageFromRuntimeState
                    (reinterpret_cast<uint32_t *>((g_InGameRuntimeRoot->worldRuntime).fieldGrid.get()),&fieldSaveError)) {
    FatalError_ReportIfFailed(fieldSaveError,true);
  }
  if (!InGameLevelRuntime_SaveLevelAssetImageFromWorldState
                    (THANDOR_CONTAINER_OF(&runtimeRoot->worldRuntime,InGameLevelSaveWorldView,worldRuntime),
                     &levelSaveError)) {
    FatalError_ReportIfFailed(levelSaveError,true);
  }
}
