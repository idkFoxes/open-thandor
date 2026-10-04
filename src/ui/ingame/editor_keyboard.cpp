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

THANDOR_ALIGN(16) UiCommandDispatchRecord g_InGameKeyboardDispatchRecords[37] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56F1C0},
    /*  1 */ {.commandCode = 0x30069, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E670},
    /*  2 */ {.commandCode = 0x20002, .continuationEntryAddress = 0x56E6A0},
    /*  3 */ {.commandCode = 0x30070, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56F160},
    /*  4 */ {.commandCode = 0x30069, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E5E0},
    /*  5 */ {.commandCode = 0x30065, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E6E0},
    /*  6 */ {.commandCode = 0x30075, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E720},
    /*  7 */ {.commandCode = 0x30075, .continuationEntryAddress = 0x56E720},
    /*  8 */ {.commandCode = 0x10012, .continuationEntryAddress = 0x56ED80},
    /*  9 */ {.commandCode = 0x1001A, .continuationEntryAddress = 0x56EDC0},
    /* 10 */ {.commandCode = 0x10014, .continuationEntryAddress = 0x56E7C0},
    /* 11 */ {.commandCode = 0x10016, .continuationEntryAddress = 0x56E930},
    /* 12 */ {.commandCode = 0x10011, .continuationEntryAddress = 0x56EAA0},
    /* 13 */ {.commandCode = 0x10019, .continuationEntryAddress = 0x56EC10},
    /* 14 */ {.commandCode = 0x10014, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E7C0},
    /* 15 */ {.commandCode = 0x10016, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E930},
    /* 16 */ {.commandCode = 0x10011, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56EAA0},
    /* 17 */ {.commandCode = 0x10019, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56EC10},
    /* 18 */ {.commandCode = 0x30061, .continuationEntryAddress = 0x56EE00},
    /* 19 */ {.commandCode = 0x30068, .continuationEntryAddress = 0x56EE20},
    /* 20 */ {.commandCode = 0x30067, .continuationEntryAddress = 0x56EE40},
    /* 21 */ {.commandCode = 0x30073, .continuationEntryAddress = 0x56EE60},
    /* 22 */ {.commandCode = 0x30070, .continuationEntryAddress = 0x56EEB0},
    /* 23 */ {.commandCode = 0x30066, .continuationEntryAddress = 0x56EED0},
    /* 24 */ {.commandCode = 0x30074, .continuationEntryAddress = 0x56EEF0},
    /* 25 */ {.commandCode = 0x3006C, .continuationEntryAddress = 0x56F020},
    /* 26 */ {.commandCode = 0x3006E, .continuationEntryAddress = 0x56EFB0},
    /* 27 */ {.commandCode = 0x30076, .continuationEntryAddress = 0x56F090},
    /* 28 */ {.commandCode = 0x30065, .continuationEntryAddress = 0x56F100},
    /* 29 */ {.commandCode = 0x30062, .continuationEntryAddress = 0x56F120},
    /* 30 */ {.commandCode = 0x30063, .continuationEntryAddress = 0x56EF70},
    /* 31 */ {.commandCode = 0x30064, .continuationEntryAddress = 0x56EF90},
    /* 32 */ {.commandCode = 0x30077, .continuationEntryAddress = 0x56EF10},
    /* 33 */ {.commandCode = 0x30071, .continuationEntryAddress = 0x56EF30},
    /* 34 */ {.commandCode = 0x30079, .continuationEntryAddress = 0x56EF50},
    /* 35 */ {.commandCode = 0x30072, .continuationEntryAddress = 0x56F140},
    /* 36: terminator (key code 0 ends the scan; the other two dwords are 0x90 fill) */
    {.commandCode = 0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}};

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
  if ((int)materialIndex < 0) {
    materialIndex = TERRAIN_MATERIAL_COUNT - 1;
  }
  while (g_TerrainMaterialTextureSets[materialIndex] == nullptr || --remainingSteps != 0) {
    materialIndex--;
    if ((int)materialIndex < 0) {
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
  hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uintptr_t)foundArmyAsset,
                                              armyLookupError != 0);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
  InGameSelectionDetailPanel_Rebuild();
}

/* Shows the preview of the newly chosen unit placement army and makes it the hovered record. */
static void InGameEditorKeyboard_ShowUnitPlacementArmy(UiRootNode *uiRoot)
{
  GraphicsTextureSourceAsset *previewTexture;

  previewTexture = (GraphicsTextureSourceAsset *)
           ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRoot,unitPlacementPreviewImage))->textureSource = previewTexture;
  InGameEditorKeyboard_HoverUnitPlacementArmy();
}

/* Shows the preview of the newly chosen object placement army. */
static void InGameEditorKeyboard_ShowObjectPlacementArmy(UiRootNode *uiRoot)
{
  GraphicsTextureSourceAsset *previewTexture;

  previewTexture = (GraphicsTextureSourceAsset *)
           ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRoot,objectPlacementPreviewImage))->textureSource = previewTexture;
}

/* Arrow key with Ctrl and/or Shift: Ctrl turns the light direction, Shift turns the auxiliary angles (moves
   the field origin), each by the given elevation and azimuth deltas, directly or through the command queue. */
static void InGameEditorKeyboard_TurnLightOrAuxiliaryAngles(uint32_t keyboardStateMask,int deltaElevation,
          int deltaAzimuth)
{
  if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    InGameCommand_Issue<TerrainLighting_AdjustDirectionAndRecomputeField>(0,deltaElevation,deltaAzimuth);
  }
  if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
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
          (uint32_t keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot)

{
  UiNodeVtable **stack;
  UiSingleLineTextControl *infoTextControl;
  const UiCommandDispatchRecord *dispatchRecord;
  uint32_t activePageIndex;

  /* First record with this key whose modifier class matches: a zero class matches only while neither Ctrl nor
     Alt is held, otherwise any modifier of the class must be held. The table ends with a zero key code. The
     cases below are the original handler addresses stored in the records. */
  dispatchRecord = UiCommandDispatch_Find(g_InGameKeyboardDispatchRecords,keyboardEventCode,keyboardStateMask,
                                          UiKeyModifierRule::AnyOfMask);
  if (dispatchRecord == nullptr) {
    return;
  }
  switch(dispatchRecord->continuationEntryAddress) {
  case 0x56e5e0: /* Alt+I: show or hide the side panel */
    stack = (struct UiNodeVtable * *)INGAME_UI(uiRoot,sidePanelStack);
    activePageIndex = UiPageStack_ActivePageIndex((UiPageStackControl *)stack);
    if (activePageIndex == 0) {
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)INGAME_UI(uiRoot,resourceBarModeStack));
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)INGAME_UI(uiRoot,gamePanelsModeStack));
      INGAME_UI(uiRoot,worldViewArea)->rightOffset = 0;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    else {
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(uiRoot,resourceBarModeStack));
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(uiRoot,gamePanelsModeStack));
      INGAME_UI(uiRoot,worldViewArea)->rightOffset = INGAME_UI(uiRoot,sidePanelFrameLeftEdge)->leftOffset;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    break;
  case 0x56e670: /* Ctrl+I: next of the world view info texts (the text field holds the text resource id) */
    infoTextControl = (UiSingleLineTextControl *)INGAME_UI(uiRoot,worldViewCyclingInfoText);
    infoTextControl->text = (uint16_t *)(intptr_t)((int32_t)infoTextControl->text + 1);
    if (TEXT_ID_WORLD_VIEW_INFO_LAST < (uint32_t)infoTextControl->text) {
      infoTextControl->text = (uint16_t *)TEXT_ID_WORLD_VIEW_INFO_FIRST;
    }
    break;
  case 0x56e6a0: /* F2: save the map */
    InGameCommand_Issue<InGameUiCommand_SaveFieldAndLevelAssetImages>(0,0,0);
    break;
  case 0x56e6e0: /* Alt+E: leave the editor */
    InGameCommand_Issue<InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState>(0,0,EDITOR_ACTIVE_STATE_LEAVE);
    break;
  case 0x56e720: /* U / Alt+U: commit the height or material edits */
    if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_HEIGHT) {
      InGameCommand_Issue<TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting>(0,0,0);
    }
    else if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
      InGameCommand_Issue<TerrainEditBuffer_CommitFlagsAndMaterialDeltas>(0,0,0);
    }
    break;
  case 0x56e7c0: /* Left: previous material / army; Ctrl: turn the light, Shift: move the field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
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
  case 0x56e930: /* Right: next material / army; Ctrl: turn the light, Shift: move the field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
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
  case 0x56eaa0: /* Up: third material back / step the army list; Ctrl: light, Shift: field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
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
  case 0x56ec10: /* Down: third material ahead / step the army list; Ctrl: light, Shift: field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
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
  case 0x56ed80: /* Page Up: next owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex++;
      if (g_GameFactionRuntimeImage.tail.activeFactionCount <
          (uint32_t)g_UiCommandModeGOwnerFactionIndex) {
        g_UiCommandModeGOwnerFactionIndex = 1;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((uintptr_t)uiRoot);
    }
    break;
  case 0x56edc0: /* Page Down: previous owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex--;
      if (g_UiCommandModeGOwnerFactionIndex == 0) {
        g_UiCommandModeGOwnerFactionIndex = g_GameFactionRuntimeImage.tail.activeFactionCount;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((uintptr_t)uiRoot);
    }
    break;
  /* Letter keys: editor tab and tool */
  case 0x56ee00: /* A */
    InGameCommandModeG_Select0((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainHeight));
    InGameCommandModeC_Select0((UiSpriteButtonControl *)INGAME_UI(uiRoot,heightToolOption0));
    break;
  case 0x56ee20: /* H */
    InGameCommandModeG_Select0((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainHeight));
    InGameCommandModeC_Select1((UiSpriteButtonControl *)INGAME_UI(uiRoot,heightToolOption1));
    break;
  case 0x56ee40: /* G */
    InGameCommandModeG_Select0((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainHeight));
    InGameCommandModeC_Select2((UiSpriteButtonControl *)INGAME_UI(uiRoot,heightToolOption2));
    break;
  case 0x56ee60: /* S */
    if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
      InGameCommandModeG_Select1((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainMaterial));
      InGameCommandModeD_Select3((UiSpriteButtonControl *)INGAME_UI(uiRoot,materialToolOption3));
    }
    else {
      InGameCommandModeG_Select0((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainHeight));
      InGameCommandModeC_Select3((UiSpriteButtonControl *)INGAME_UI(uiRoot,heightToolOption3));
    }
    break;
  case 0x56eeb0: /* P */
    InGameCommandModeG_Select1((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainMaterial));
    InGameCommandModeD_Select0((UiSpriteButtonControl *)INGAME_UI(uiRoot,materialToolOption0));
    break;
  case 0x56eed0: /* F */
    InGameCommandModeG_Select1((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainMaterial));
    InGameCommandModeD_Select1((UiSpriteButtonControl *)INGAME_UI(uiRoot,materialToolOption1));
    break;
  case 0x56eef0: /* T */
    InGameCommandModeG_Select1((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainMaterial));
    InGameCommandModeD_Select2((UiSpriteButtonControl *)INGAME_UI(uiRoot,materialToolOption2));
    break;
  case 0x56ef10: /* W */
    InGameCommandModeG_Select2((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainSmoothing));
    InGameCommandModeE_Select0((UiSpriteButtonControl *)INGAME_UI(uiRoot,smoothingToolOption0));
    break;
  case 0x56ef30: /* Q */
    InGameCommandModeG_Select2((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainSmoothing));
    InGameCommandModeE_Select1((UiSpriteButtonControl *)INGAME_UI(uiRoot,smoothingToolOption1));
    break;
  case 0x56ef50: /* Y */
    InGameCommandModeG_Select2((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainSmoothing));
    InGameCommandModeE_Select2((UiSpriteButtonControl *)INGAME_UI(uiRoot,smoothingToolOption2));
    break;
  case 0x56ef70: /* C */
    InGameCommandModeG_Select2((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainSmoothing));
    InGameCommandRange_DispatchState0((UiNodeBase *)INGAME_UI(uiRoot,smoothingRelaxGatedButton));
    break;
  case 0x56ef90: /* D */
    InGameCommandModeG_Select2((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabTerrainSmoothing));
    InGameCommandRange_DispatchState1(INGAME_UI(uiRoot,smoothingRelaxLandButton));
    break;
  case 0x56efb0: /* N */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabObjectPlacement));
      InGameCommandModeB_Select0((UiSpriteButtonControl *)INGAME_UI(uiRoot,objectPlacementOption0));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabUnitPlacement));
      InGameCommandModeA_Select0((UiSpriteButtonControl *)INGAME_UI(uiRoot,unitPlacementOption0));
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case 0x56f020: /* L */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabObjectPlacement));
      InGameCommandModeB_Select1((UiSpriteButtonControl *)INGAME_UI(uiRoot,objectPlacementOption1));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabUnitPlacement));
      InGameCommandModeA_Select1((UiSpriteButtonControl *)INGAME_UI(uiRoot,unitPlacementOption1));
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case 0x56f090: /* V */
    if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
      InGameCommandModeG_Select4((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabObjectPlacement));
      InGameCommandModeB_Select2((UiSpriteButtonControl *)INGAME_UI(uiRoot,objectPlacementOption2));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabUnitPlacement));
      InGameCommandModeA_Select2((UiSpriteButtonControl *)INGAME_UI(uiRoot,unitPlacementOption2));
      InGameEditorKeyboard_HoverUnitPlacementArmy();
    }
    break;
  case 0x56f100: /* E */
    InGameCommandModeG_Select3((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabUnitPlacement));
    break;
  case 0x56f120: /* B */
    InGameCommandModeG_Select4((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabObjectPlacement));
    break;
  case 0x56f140: /* R */
    InGameCommandModeG_Select5((UiSelectableControl *)INGAME_UI(uiRoot,editorModeTabRegion));
    break;
  case 0x56f160: /* Ctrl+P: screenshot to screenNN.pcx, counting the two digits up */
    /* The original calls the capture without passing its four arguments (it reads stale stack values) and
       writes the raw capture asset to the .pcx file without freeing it; saved here like the in-game Alt+P and
       end-movie screenshot commands (whole framebuffer, PCX-encoded, capture freed) because the raw dump is
       no PCX and leaks one capture per press. */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case 0x56f1c0: /* Alt+Q: leave the editor and the session */
    InGameCommand_Issue<InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState>(0,0,EDITOR_ACTIVE_STATE_LEAVE);
    InGameCommand_Issue<InGameCommand_HandlePlayerDeparture>(0,0,0);
  }
  return;
}
