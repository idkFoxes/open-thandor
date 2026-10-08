/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/editor_keyboard.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/editor_keyboard.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_dispatch.h>

/* Module data. */

/* Actions of the map editor's keyboard table (InGameEditorKeyboard dispatch below). */
enum class InGameEditorKeyAction : uint32_t {
    LeaveEditorAndSession = 1,     /* Alt+Q */
    InfoTextNext = 2,              /* Ctrl+I */
    SaveMap = 3,                   /* F2 */
    Screenshot = 4,                /* Ctrl+P */
    ToggleSidePanel = 5,           /* Alt+I */
    LeaveEditor = 6,               /* Alt+E */
    CommitTerrainEdits = 7,        /* U, Alt+U */
    NextUnitOwnerFaction = 8,      /* Page Up */
    PreviousUnitOwnerFaction = 9,  /* Page Down */
    Left = 10,                     /* Left, Ctrl+Left */
    Right = 11,                    /* Right, Ctrl+Right */
    Up = 12,                       /* Up, Ctrl+Up */
    Down = 13,                     /* Down, Ctrl+Down */
    HeightTool0 = 14,              /* A */
    HeightTool1 = 15,              /* H */
    HeightTool2 = 16,              /* G */
    HeightOrMaterialTool3 = 17,    /* S */
    MaterialTool0 = 18,            /* P */
    MaterialTool1 = 19,            /* F */
    MaterialTool2 = 20,            /* T */
    PlacementOption1 = 21,         /* L */
    PlacementOption0 = 22,         /* N */
    PlacementOption2 = 23,         /* V */
    UnitPlacementTab = 24,         /* E */
    ObjectPlacementTab = 25,       /* B */
    SmoothingRelaxGated = 26,      /* C */
    SmoothingRelaxLand = 27,       /* D */
    SmoothingTool0 = 28,           /* W */
    SmoothingTool1 = 29,           /* Q */
    SmoothingTool2 = 30,           /* Y */
    RegionTab = 31,                /* R */
};
static_assert(sizeof(UiKeyCommandRecord<InGameEditorKeyAction>) == 0xC, "a key command record keeps the original 12 bytes");

/* 36 records and the terminator record [36] (key code 0 ends the dispatch scan) */
static UiKeyCommandRecord<InGameEditorKeyAction> g_InGameKeyboardDispatchRecords[37] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameEditorKeyAction::LeaveEditorAndSession},
    /*  1 */ {.commandCode = 0x30069, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::InfoTextNext},
    /*  2 */ {.commandCode = 0x20002, .action = InGameEditorKeyAction::SaveMap},
    /*  3 */ {.commandCode = 0x30070, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::Screenshot},
    /*  4 */ {.commandCode = 0x30069, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameEditorKeyAction::ToggleSidePanel},
    /*  5 */ {.commandCode = 0x30065, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameEditorKeyAction::LeaveEditor},
    /*  6 */ {.commandCode = 0x30075, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameEditorKeyAction::CommitTerrainEdits},
    /*  7 */ {.commandCode = 0x30075, .action = InGameEditorKeyAction::CommitTerrainEdits},
    /*  8 */ {.commandCode = 0x10012, .action = InGameEditorKeyAction::NextUnitOwnerFaction},
    /*  9 */ {.commandCode = 0x1001A, .action = InGameEditorKeyAction::PreviousUnitOwnerFaction},
    /* 10 */ {.commandCode = 0x10014, .action = InGameEditorKeyAction::Left},
    /* 11 */ {.commandCode = 0x10016, .action = InGameEditorKeyAction::Right},
    /* 12 */ {.commandCode = 0x10011, .action = InGameEditorKeyAction::Up},
    /* 13 */ {.commandCode = 0x10019, .action = InGameEditorKeyAction::Down},
    /* 14 */ {.commandCode = 0x10014, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::Left},
    /* 15 */ {.commandCode = 0x10016, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::Right},
    /* 16 */ {.commandCode = 0x10011, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::Up},
    /* 17 */ {.commandCode = 0x10019, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameEditorKeyAction::Down},
    /* 18 */ {.commandCode = 0x30061, .action = InGameEditorKeyAction::HeightTool0},
    /* 19 */ {.commandCode = 0x30068, .action = InGameEditorKeyAction::HeightTool1},
    /* 20 */ {.commandCode = 0x30067, .action = InGameEditorKeyAction::HeightTool2},
    /* 21 */ {.commandCode = 0x30073, .action = InGameEditorKeyAction::HeightOrMaterialTool3},
    /* 22 */ {.commandCode = 0x30070, .action = InGameEditorKeyAction::MaterialTool0},
    /* 23 */ {.commandCode = 0x30066, .action = InGameEditorKeyAction::MaterialTool1},
    /* 24 */ {.commandCode = 0x30074, .action = InGameEditorKeyAction::MaterialTool2},
    /* 25 */ {.commandCode = 0x3006C, .action = InGameEditorKeyAction::PlacementOption1},
    /* 26 */ {.commandCode = 0x3006E, .action = InGameEditorKeyAction::PlacementOption0},
    /* 27 */ {.commandCode = 0x30076, .action = InGameEditorKeyAction::PlacementOption2},
    /* 28 */ {.commandCode = 0x30065, .action = InGameEditorKeyAction::UnitPlacementTab},
    /* 29 */ {.commandCode = 0x30062, .action = InGameEditorKeyAction::ObjectPlacementTab},
    /* 30 */ {.commandCode = 0x30063, .action = InGameEditorKeyAction::SmoothingRelaxGated},
    /* 31 */ {.commandCode = 0x30064, .action = InGameEditorKeyAction::SmoothingRelaxLand},
    /* 32 */ {.commandCode = 0x30077, .action = InGameEditorKeyAction::SmoothingTool0},
    /* 33 */ {.commandCode = 0x30071, .action = InGameEditorKeyAction::SmoothingTool1},
    /* 34 */ {.commandCode = 0x30079, .action = InGameEditorKeyAction::SmoothingTool2},
    /* 35 */ {.commandCode = 0x30072, .action = InGameEditorKeyAction::RegionTab},
    /* 36: terminator (key code 0 ends the scan; the original's other two dwords were NOP fill, never read) */
    {.commandCode = 0}};

UiCommandRuntimeRecordPrefix *g_UiHoverSelectionRecord = nullptr;

uint32_t g_UiCommandModeGArmyAssetId = 0;

PckArmyAssetIdCatalog g_UiCommandMode4ArmyAssetId = ARM_0500_LBAUM_MDL0500;

/* Selects the stepCount-th material before the current one that has a texture set, wrapping around
   (1: the previous material, MATERIAL_SWATCH_ROW_LENGTH: one swatch row up). */
static void InGameEditorKeyboard_SelectMaterialBackward(int stepCount,UiRootNode *uiRoot)
{
  UiCommandModeIndex materialIndex;
  int remainingSteps;

  materialIndex = g_UiCommandAbsoluteSelectionIndex - 1;
  remainingSteps = stepCount;
  if (static_cast<int>(materialIndex) < 0) {
    materialIndex = TERRAIN_MATERIAL_COUNT - 1;
  }
  while (g_TerrainMaterialTextureSets[materialIndex] == nullptr || --remainingSteps != 0) {
    materialIndex--;
    if (static_cast<int>(materialIndex) < 0) {
      materialIndex = TERRAIN_MATERIAL_COUNT - 1;
    }
  }
  UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
}

/* Selects the stepCount-th material after the current one that has a texture set, wrapping around
   (1: the next material, MATERIAL_SWATCH_ROW_LENGTH: one swatch row down). */
static void InGameEditorKeyboard_SelectMaterialForward(int stepCount,UiRootNode *uiRoot)
{
  UiCommandModeIndex materialIndex;
  int remainingSteps;

  materialIndex = g_UiCommandAbsoluteSelectionIndex + 1;
  remainingSteps = stepCount;
  if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
    materialIndex = 0;
  }
  while (g_TerrainMaterialTextureSets[materialIndex] == nullptr || --remainingSteps != 0) {
    materialIndex++;
    if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
      materialIndex = 0;
    }
  }
  UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
}

/* Makes the unit placement army (g_UiCommandModeGArmyAssetId) the hovered record and rebuilds the detail
   panel; a failed lookup is fatal. */
static void InGameEditorKeyboard_HoverUnitPlacementArmy()
{
  uint32_t armyLookupError;
  ArmyAssetRecordPrefix *foundArmyAsset;
  uintptr_t hoverRecordValue;

  armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
  hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError
                                                                  : reinterpret_cast<uintptr_t>(foundArmyAsset),
                                              armyLookupError != 0);
  g_UiHoverSelectionRecord = reinterpret_cast<UiCommandRuntimeRecordPrefix *>(hoverRecordValue);
  InGameSelectionDetailPanel_Rebuild();
}

/* Shows the preview of the newly chosen unit placement army and makes it the hovered record. */
static void InGameEditorKeyboard_ShowUnitPlacementArmy(UiRootNode *uiRoot)
{
  GraphicsTextureSourceAsset *previewTexture;

  previewTexture = reinterpret_cast<GraphicsTextureSourceAsset *>
           (ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId));
  InGameUi_Image(uiRoot)->unitPlacementPreviewImage.textureSource = previewTexture;
  InGameEditorKeyboard_HoverUnitPlacementArmy();
}

/* Shows the preview of the newly chosen object placement army. */
static void InGameEditorKeyboard_ShowObjectPlacementArmy(UiRootNode *uiRoot)
{
  GraphicsTextureSourceAsset *previewTexture;

  previewTexture = reinterpret_cast<GraphicsTextureSourceAsset *>
           (ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId));
  InGameUi_Image(uiRoot)->objectPlacementPreviewImage.textureSource = previewTexture;
}

/* Arrow key with Ctrl and/or Shift: Ctrl turns the light direction, Shift turns the auxiliary angles (moves
   the field origin), each by the given elevation and azimuth deltas, directly or through the command queue. */
static void InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(UiKeyboardStateMask keyboardStateMask,int deltaElevation,
          int deltaAzimuth)
{
  if (Any(keyboardStateMask & KEYBOARD_STATE_CTRL)) {
    InGameCommand_Issue<TerrainLighting_AdjustDirectionAndRecomputeField>(0,deltaElevation,deltaAzimuth);
  }
  if (Any(keyboardStateMask & KEYBOARD_STATE_SHIFT)) {
    InGameCommand_Issue<WorldRuntime_TurnAuxiliaryAnglesClamped>(0,deltaElevation,deltaAzimuth);
  }
}

/* Keyboard handler of the map editor (installed as g_InGameUiRootCallbacks.keyboardFallback by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState while the editor is active): looks the key up in
   g_InGameKeyboardDispatchRecords and runs the matching hotkey - tool and tab selection, cycling materials and
   placement armies, moving the field origin or the light direction, saving the map, screenshots and leaving
   the editor. Editor commands go through the command queue in network games.
*/
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot)

{
  InGameUiImage *image;
  UiPageStackControl *sidePanelStack;
  const UiKeyCommandRecord<InGameEditorKeyAction> *dispatchRecord;
  uint32_t activePageIndex;

  /* First record with this key whose modifier class matches: a zero class matches only while neither Ctrl nor
     Alt is held, otherwise any modifier of the class must be held. The table ends with a zero key code. Each
     record names its action, one case below. */
  dispatchRecord = UiCommandDispatch_Find(g_InGameKeyboardDispatchRecords,keyboardEventCode,keyboardStateMask,
                                          UiKeyModifierRule::AnyOfMask);
  if (dispatchRecord == nullptr) {
    return;
  }
  image = InGameUi_Image(uiRoot);
  switch(dispatchRecord->action) {
  case InGameEditorKeyAction::ToggleSidePanel: /* Alt+I: show or hide the side panel */
    sidePanelStack = UiLayoutContainerControl_AsPageStack(&image->sidePanelStack);
    activePageIndex = UiPageStack_ActivePageIndex(sidePanelStack);
    if (activePageIndex == 0) {
      UiPageStack_SetActiveIndex(1,sidePanelStack);
      UiPageStack_SetActiveIndex(2,UiLayoutContainerControl_AsPageStack(&image->resourceBarModeStack));
      UiPageStack_SetActiveIndex(2,UiLayoutContainerControl_AsPageStack(&image->gamePanelsModeStack));
      image->worldViewArea.base.rightOffset = 0;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    else {
      UiPageStack_SetActiveIndex(0,sidePanelStack);
      UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&image->resourceBarModeStack));
      UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&image->gamePanelsModeStack));
      image->worldViewArea.base.rightOffset = image->sidePanelFrameLeftEdge.base.leftOffset;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    break;
  case InGameEditorKeyAction::InfoTextNext: /* Ctrl+I: next of the world view info texts (the text field holds the text resource id) */
    InGameWorldView_ShowNextInfoText(&image->worldViewCyclingInfoText);
    break;
  case InGameEditorKeyAction::SaveMap: /* F2: save the map */
    InGameCommand_Issue<InGameUiCommand_SaveFieldAndLevelAssetImages>(0,0,0);
    break;
  case InGameEditorKeyAction::LeaveEditor: /* Alt+E: leave the editor */
    InGameCommand_Issue<InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState>(0,0,EDITOR_ACTIVE_STATE_LEAVE);
    break;
  case InGameEditorKeyAction::CommitTerrainEdits: /* U / Alt+U: commit the height or material edits */
    if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_HEIGHT) {
      InGameCommand_Issue<TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting>(0,0,0);
    }
    else if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
      InGameCommand_Issue<TerrainEditBuffer_CommitFlagsAndMaterialDeltas>(0,0,0);
    }
    break;
  case InGameEditorKeyAction::Left: /* Left: previous material / army; Ctrl: turn the light, Shift: move the field origin */
    if (!Any(keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL))) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        InGameEditorKeyboard_SelectMaterialBackward(1,uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_FindPreviousPlaceableUnitWrapped
                           (g_UiCommandModeGArmyAssetId);
        InGameEditorKeyboard_ShowUnitPlacementArmy(uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_FindPreviousPlaceableObjectWrapped
                           (g_UiCommandMode4ArmyAssetId);
        InGameEditorKeyboard_ShowObjectPlacementArmy(uiRoot);
      }
    }
    else {
      InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(keyboardStateMask,0,-EDITOR_ADJUST_STEP);
    }
    break;
  case InGameEditorKeyAction::Right: /* Right: next material / army; Ctrl: turn the light, Shift: move the field origin */
    if (!Any(keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL))) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        InGameEditorKeyboard_SelectMaterialForward(1,uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_FindNextPlaceableUnitWrapped(g_UiCommandModeGArmyAssetId);
        InGameEditorKeyboard_ShowUnitPlacementArmy(uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_FindNextPlaceableObjectWrapped(g_UiCommandMode4ArmyAssetId);
        InGameEditorKeyboard_ShowObjectPlacementArmy(uiRoot);
      }
    }
    else {
      InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(keyboardStateMask,0,EDITOR_ADJUST_STEP);
    }
    break;
  case InGameEditorKeyAction::Up: /* Up: third material back / step the army list; Ctrl: light, Shift: field origin */
    if (!Any(keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL))) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        InGameEditorKeyboard_SelectMaterialBackward(MATERIAL_SWATCH_ROW_LENGTH,uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_StepForwardPlaceableUnit(g_UiCommandModeGArmyAssetId);
        InGameEditorKeyboard_ShowUnitPlacementArmy(uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_StepForwardPlaceableObject(g_UiCommandMode4ArmyAssetId);
        InGameEditorKeyboard_ShowObjectPlacementArmy(uiRoot);
      }
    }
    else {
      InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(keyboardStateMask,-EDITOR_ADJUST_STEP,0);
    }
    break;
  case InGameEditorKeyAction::Down: /* Down: third material ahead / step the army list; Ctrl: light, Shift: field origin */
    if (!Any(keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL))) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        InGameEditorKeyboard_SelectMaterialForward(MATERIAL_SWATCH_ROW_LENGTH,uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_StepBackwardPlaceableUnit(g_UiCommandModeGArmyAssetId);
        InGameEditorKeyboard_ShowUnitPlacementArmy(uiRoot);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_StepBackwardPlaceableObject(g_UiCommandMode4ArmyAssetId);
        InGameEditorKeyboard_ShowObjectPlacementArmy(uiRoot);
      }
    }
    else {
      InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(keyboardStateMask,EDITOR_ADJUST_STEP,0);
    }
    break;
  case InGameEditorKeyAction::NextUnitOwnerFaction: /* Page Up: next owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex++;
      if (g_GameFactionRuntimeImage.tail.activeFactionCount <
          static_cast<uint32_t>(g_UiCommandModeGOwnerFactionIndex)) {
        g_UiCommandModeGOwnerFactionIndex = 1;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected
                (reinterpret_cast<uintptr_t>(uiRoot));
    }
    break;
  case InGameEditorKeyAction::PreviousUnitOwnerFaction: /* Page Down: previous owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex--;
      if (g_UiCommandModeGOwnerFactionIndex == 0) {
        g_UiCommandModeGOwnerFactionIndex = g_GameFactionRuntimeImage.tail.activeFactionCount;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected
                (reinterpret_cast<uintptr_t>(uiRoot));
    }
    break;
  /* Letter keys: editor tab and tool */
  case InGameEditorKeyAction::HeightTool0: /* A */
    InGameCommandModeG_Select0(&image->editorModeTabTerrainHeight.selectable);
    InGameCommandModeC_Select0(&image->heightToolOption0);
    break;
  case InGameEditorKeyAction::HeightTool1: /* H */
    InGameCommandModeG_Select0(&image->editorModeTabTerrainHeight.selectable);
    InGameCommandModeC_Select1(&image->heightToolOption1);
    break;
  case InGameEditorKeyAction::HeightTool2: /* G */
    InGameCommandModeG_Select0(&image->editorModeTabTerrainHeight.selectable);
    InGameCommandModeC_Select2(&image->heightToolOption2);
    break;
  case InGameEditorKeyAction::HeightOrMaterialTool3: /* S */
    if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
      InGameCommandModeG_Select1(&image->editorModeTabTerrainMaterial.selectable);
      InGameCommandModeD_Select3(&image->materialToolOption3);
    }
    else {
      InGameCommandModeG_Select0(&image->editorModeTabTerrainHeight.selectable);
      InGameCommandModeC_Select3(&image->heightToolOption3);
    }
    break;
  case InGameEditorKeyAction::MaterialTool0: /* P */
    InGameCommandModeG_Select1(&image->editorModeTabTerrainMaterial.selectable);
    InGameCommandModeD_Select0(&image->materialToolOption0);
    break;
  case InGameEditorKeyAction::MaterialTool1: /* F */
    InGameCommandModeG_Select1(&image->editorModeTabTerrainMaterial.selectable);
    InGameCommandModeD_Select1(&image->materialToolOption1);
    break;
  case InGameEditorKeyAction::MaterialTool2: /* T */
    InGameCommandModeG_Select1(&image->editorModeTabTerrainMaterial.selectable);
    InGameCommandModeD_Select2(&image->materialToolOption2);
    break;
  case InGameEditorKeyAction::SmoothingTool0: /* W */
    InGameCommandModeG_Select2(&image->editorModeTabTerrainSmoothing.selectable);
    InGameCommandModeE_Select0(&image->smoothingToolOption0);
    break;
  case InGameEditorKeyAction::SmoothingTool1: /* Q */
    InGameCommandModeG_Select2(&image->editorModeTabTerrainSmoothing.selectable);
    InGameCommandModeE_Select1(&image->smoothingToolOption1);
    break;
  case InGameEditorKeyAction::SmoothingTool2: /* Y */
    InGameCommandModeG_Select2(&image->editorModeTabTerrainSmoothing.selectable);
    InGameCommandModeE_Select2(&image->smoothingToolOption2);
    break;
  case InGameEditorKeyAction::SmoothingRelaxGated: /* C */
    InGameCommandModeG_Select2(&image->editorModeTabTerrainSmoothing.selectable);
    InGameCommandRange_DispatchState0(&image->smoothingRelaxGatedButton.selectable.base);
    break;
  case InGameEditorKeyAction::SmoothingRelaxLand: /* D */
    InGameCommandModeG_Select2(&image->editorModeTabTerrainSmoothing.selectable);
    InGameCommandRange_DispatchState1(&image->smoothingRelaxLandButton.selectable.base);
    break;
  case InGameEditorKeyAction::PlacementOption0: /* N */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4(InGameUi_ObjectPlacementTab(image));
      InGameCommandModeB_Select0(&image->objectPlacementOption0);
    }
    else {
      InGameCommandModeG_Select3(&image->editorModeTabUnitPlacement.selectable);
      InGameCommandModeA_Select0(&image->unitPlacementOption0);
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case InGameEditorKeyAction::PlacementOption1: /* L */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4(InGameUi_ObjectPlacementTab(image));
      InGameCommandModeB_Select1(&image->objectPlacementOption1);
    }
    else {
      InGameCommandModeG_Select3(&image->editorModeTabUnitPlacement.selectable);
      InGameCommandModeA_Select1(&image->unitPlacementOption1);
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case InGameEditorKeyAction::PlacementOption2: /* V */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4(InGameUi_ObjectPlacementTab(image));
      InGameCommandModeB_Select2(&image->objectPlacementOption2);
    }
    else {
      InGameCommandModeG_Select3(&image->editorModeTabUnitPlacement.selectable);
      InGameCommandModeA_Select2(&image->unitPlacementOption2);
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case InGameEditorKeyAction::UnitPlacementTab: /* E */
    InGameCommandModeG_Select3(&image->editorModeTabUnitPlacement.selectable);
    break;
  case InGameEditorKeyAction::ObjectPlacementTab: /* B */
    InGameCommandModeG_Select4(InGameUi_ObjectPlacementTab(image));
    break;
  case InGameEditorKeyAction::RegionTab: /* R */
    InGameCommandModeG_Select5(&image->editorModeTabRegion.selectable);
    break;
  case InGameEditorKeyAction::Screenshot: /* Ctrl+P: screenshot to screenNN.pcx, counting the two digits up */
    /* The original calls the capture without passing its four arguments (it reads stale stack values) and
       writes the raw capture asset to the .pcx file without freeing it; saved here like the in-game Alt+P and
       end-movie screenshot commands (whole framebuffer, PCX-encoded, capture freed) because the raw dump is
       no PCX and leaks one capture per press. */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case InGameEditorKeyAction::LeaveEditorAndSession: /* Alt+Q: leave the editor and the session */
    InGameCommand_Issue<InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState>(0,0,EDITOR_ACTIVE_STATE_LEAVE);
    InGameCommand_Issue<InGameCommand_HandlePlayerDeparture>(0,0,0);
  }
}
