/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: ui/ingame/runtime. */

/* Address: 0x0056E3C0.
   Keyboard handler of the map editor (installed as g_UiRootCallbacks_0054FBC0.keyboardFallback by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState while the editor is active): looks the key up in
   g_InGameKeyboardDispatchRecords and runs the matching hotkey - tool and tab selection, cycling materials and
   placement armies, moving the field origin or the light direction, saving the map, screenshots and leaving
   the editor. Editor commands go through the command queue in network games.
*/
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
          (uint32_t keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot)

{
  UiNodeVtable **stack;
  int32_t *counterField;
  wchar_t screenshotTensDigit;
  wchar_t screenshotOnesDigit;
  GraphicsTextureSourceAsset *modeGPreviewTexture;
  GraphicsTextureSourceAsset *mode4PreviewTexture;
  UiCommandModeIndex materialIndex;
  int remainingSteps;
  uint32_t *dispatchRecord;
  uint32_t activePageIndex;
  uint32_t armyLookupError;
  ArmyAssetRecordPrefix *foundArmyAsset;
  uint32_t hoverRecordValue;
  GraphicsCapturedTextureSourceAsset *capturedFramebuffer;
  
  /* Records are {key code, required modifier mask, handler}; a zero mask matches only while neither Ctrl
     nor Alt is held. The table ends with a zero key code. The cases below are the original handler
     addresses stored in the records. */
  for (dispatchRecord = (uint32_t *)THANDOR_ADDR(g_InGameKeyboardDispatchRecords,0); ;
      dispatchRecord = dispatchRecord + 3) {
    if (*dispatchRecord == 0) {
      return;
    }
    if (*dispatchRecord != keyboardEventCode) continue;
    if (dispatchRecord[1] == 0) {
      if ((keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0) break;
    }
    else if ((keyboardStateMask & dispatchRecord[1]) != 0) break;
  }
  switch(dispatchRecord[2]) {
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
    counterField = (int32_t *)&((UiSingleLineTextControl *)INGAME_UI(uiRoot,worldViewCyclingInfoText))->text;
    *counterField = *counterField + 1;
    if (TEXT_ID_WORLD_VIEW_INFO_LAST <
        (uint32_t)((UiSingleLineTextControl *)INGAME_UI(uiRoot,worldViewCyclingInfoText))->text) {
      ((UiSingleLineTextControl *)INGAME_UI(uiRoot,worldViewCyclingInfoText))->text =
           (uint16_t *)TEXT_ID_WORLD_VIEW_INFO_FIRST;
    }
    break;
  case 0x56e6a0: /* F2: save the map */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommand_SaveFieldAndLevelAssetImages(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_SAVE_MAP,0,0,0);
    }
    break;
  case 0x56e6e0: /* Alt+E: leave the editor */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState(g_LocalPlayerRuntimeId,0,0,
                                                                  EDITOR_ACTIVE_STATE_LEAVE);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_ACTIVE_STATE,0,0,
                                                  EDITOR_ACTIVE_STATE_LEAVE);
    }
    break;
  case 0x56e720: /* U / Alt+U: commit the height or material edits */
    if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_HEIGHT) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS,0,0,0);
      }
    }
    else if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_CommitFlagsAndMaterialDeltas(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_COMMIT_MATERIALS,0,0,0);
      }
    }
    break;
  case 0x56e7c0: /* Left: previous material / army; Ctrl: turn the light, Shift: move the field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        /* previous material that has a texture set, wrapping around */
        materialIndex = g_UiCommandAbsoluteSelectionIndex - 1;
        if ((int)materialIndex < 0) {
          materialIndex = TERRAIN_MATERIAL_COUNT - 1;
        }
        while (g_TerrainMaterialTextureSets[materialIndex] == NULL) {
          materialIndex--;
          if ((int)materialIndex < 0) {
            materialIndex = TERRAIN_MATERIAL_COUNT - 1;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_FindPreviousPlaceableUnitWrapped
                           (g_UiCommandModeGArmyAssetId);
        modeGPreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,unitPlacementPreviewImage))->textureSource = modeGPreviewTexture;
        armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
        hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                    armyLookupError != 0);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_FindPreviousPlaceableObjectWrapped
                           (g_UiCommandMode4ArmyAssetId);
        mode4PreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,objectPlacementPreviewImage))->textureSource = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0,-EDITOR_ADJUST_STEP);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_LIGHT,0,0,-EDITOR_ADJUST_STEP);
        }
      }
      if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_TurnAuxiliaryAnglesClamped(g_LocalPlayerRuntimeId,0,0,-EDITOR_ADJUST_STEP);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES,0,0,-EDITOR_ADJUST_STEP);
        }
      }
    }
    break;
  case 0x56e930: /* Right: next material / army; Ctrl: turn the light, Shift: move the field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex + 1;
        if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
          materialIndex = 0;
        }
        while (g_TerrainMaterialTextureSets[materialIndex] == NULL) {
          materialIndex++;
          if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
            materialIndex = 0;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_FindNextPlaceableUnitWrapped(g_UiCommandModeGArmyAssetId);
        modeGPreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,unitPlacementPreviewImage))->textureSource = modeGPreviewTexture;
        armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
        hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                    armyLookupError != 0);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_FindNextPlaceableObjectWrapped(g_UiCommandMode4ArmyAssetId);
        mode4PreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,objectPlacementPreviewImage))->textureSource = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0,EDITOR_ADJUST_STEP);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_LIGHT,0,0,EDITOR_ADJUST_STEP);
        }
      }
      if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_TurnAuxiliaryAnglesClamped(g_LocalPlayerRuntimeId,0,0,EDITOR_ADJUST_STEP);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES,0,0,EDITOR_ADJUST_STEP);
        }
      }
    }
    break;
  case 0x56eaa0: /* Up: third material back / step the army list; Ctrl: light, Shift: field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex - 1;
        remainingSteps = MATERIAL_SWATCH_ROW_LENGTH;
        if ((int)materialIndex < 0) {
          materialIndex = TERRAIN_MATERIAL_COUNT - 1;
        }
        while ((g_TerrainMaterialTextureSets[materialIndex] == NULL ||
               --remainingSteps != 0)) {
          materialIndex--;
          if ((int)materialIndex < 0) {
            materialIndex = TERRAIN_MATERIAL_COUNT - 1;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_StepForwardPlaceableUnit(g_UiCommandModeGArmyAssetId);
        modeGPreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,unitPlacementPreviewImage))->textureSource = modeGPreviewTexture;
        armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
        hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                    armyLookupError != 0);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_StepForwardPlaceableObject(g_UiCommandMode4ArmyAssetId);
        mode4PreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,objectPlacementPreviewImage))->textureSource = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,-EDITOR_ADJUST_STEP,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_LIGHT,0,-EDITOR_ADJUST_STEP,0);
        }
      }
      if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_TurnAuxiliaryAnglesClamped(g_LocalPlayerRuntimeId,0,-EDITOR_ADJUST_STEP,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES,0,-EDITOR_ADJUST_STEP,0);
        }
      }
    }
    break;
  case 0x56ec10: /* Down: third material ahead / step the army list; Ctrl: light, Shift: field origin */
    if ((keyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      if (g_UiCommandModeG == EDITOR_MODE_TERRAIN_MATERIAL) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex + 1;
        remainingSteps = MATERIAL_SWATCH_ROW_LENGTH;
        if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
          materialIndex = 0;
        }
        while ((g_TerrainMaterialTextureSets[materialIndex] == NULL ||
               --remainingSteps != 0)) {
          materialIndex++;
          if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex) {
            materialIndex = 0;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
        g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_StepBackwardPlaceableUnit(g_UiCommandModeGArmyAssetId);
        modeGPreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,unitPlacementPreviewImage))->textureSource = modeGPreviewTexture;
        armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
        hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                    armyLookupError != 0);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == EDITOR_MODE_OBJECT_PLACEMENT) {
        g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_StepBackwardPlaceableObject(g_UiCommandMode4ArmyAssetId);
        mode4PreviewTexture = (GraphicsTextureSourceAsset *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
        ((UiImagePanelControl *)INGAME_UI(uiRoot,objectPlacementPreviewImage))->textureSource = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,EDITOR_ADJUST_STEP,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_LIGHT,0,EDITOR_ADJUST_STEP,0);
        }
      }
      if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_TurnAuxiliaryAnglesClamped(g_LocalPlayerRuntimeId,0,EDITOR_ADJUST_STEP,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES,0,EDITOR_ADJUST_STEP,0);
        }
      }
    }
    break;
  case 0x56ed80: /* Page Up: next owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex++;
      if (g_GameFactionRuntimeImage.tail.activeFactionCount <
          (uint32_t)g_UiCommandModeGOwnerFactionIndex) {
        g_UiCommandModeGOwnerFactionIndex = 1;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((uint32_t)uiRoot);
    }
    break;
  case 0x56edc0: /* Page Down: previous owner faction for unit placement */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      g_UiCommandModeGOwnerFactionIndex--;
      if (g_UiCommandModeGOwnerFactionIndex == 0) {
        g_UiCommandModeGOwnerFactionIndex = g_GameFactionRuntimeImage.tail.activeFactionCount;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((uint32_t)uiRoot);
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
      armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
      hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                  armyLookupError != 0);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
      InGameSelectionDetailPanel_Rebuild();
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
      armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
      hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                  armyLookupError != 0);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
      InGameSelectionDetailPanel_Rebuild();
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
      armyLookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&foundArmyAsset);
      hoverRecordValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                  armyLookupError != 0);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordValue;
      InGameSelectionDetailPanel_Rebuild();
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
    /* The original calls this without pushing arguments (stale stack, RET 0x10); capture the whole
       framebuffer like the end-game and end-movie screenshot commands. */
    capturedFramebuffer = g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
    if (capturedFramebuffer != NULL) {
      FileSystem_WriteBufferToPath
                ((capturedFramebuffer->common).allocationSizeBytes,capturedFramebuffer,
                 (uint16_t *)(u_Dscreen00_pcx_00572e3a + 1));
      screenshotOnesDigit = u_Dscreen00_pcx_00572e3a[8];
      screenshotTensDigit = u_Dscreen00_pcx_00572e3a[7];
      u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + 1;
      if (L'9' < (uint16_t)u_Dscreen00_pcx_00572e3a[8]) {
        u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + 1;
        u_Dscreen00_pcx_00572e3a[8] = screenshotOnesDigit - 9; /* '9' + 1 - 10: back to '0' */
        if (L'9' < (uint16_t)u_Dscreen00_pcx_00572e3a[7]) {
          u_Dscreen00_pcx_00572e3a[7] = screenshotTensDigit - 9;
        }
      }
    }
    break;
  case 0x56f1c0: /* Alt+Q: leave the editor and the session */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState(g_LocalPlayerRuntimeId,0,0,
                                                                  EDITOR_ACTIVE_STATE_LEAVE);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_ACTIVE_STATE,0,0,
                                                  EDITOR_ACTIVE_STATE_LEAVE);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommand_HandlePlayerDeparture(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_DEPARTURE,0,0,0);
    }
  }
  return;
}


/* Address: 0x0056BAD0.
   UI action 0x1005 (g_InGameUiActionHandlersPage10[5], InGameUiImage.messageSendAndCloseButton): sends the
   typed message to the chosen recipients (InGameSevenSlotCommand_SubmitTextAndSelectionMask) and closes the
   message window.
*/
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source)

{
  InGameSevenSlotCommand_SubmitTextAndSelectionMask(source);
  InGameSevenSlotCommand_ClosePage(source);
  return;
}

/* Address: 0x0056A610.
   UI action 0x1024 (g_InGameUiActionHandlersPage10[36]): Enter in the in-game chat line. In a local game the
   line is only compared with the cheat phrase g_DeveloperChatPhraseUtf16, which toggles the cheats and answers
   with a message. In a network game the text is sent to the recipients chosen in the message window (all, the
   ticked factions or the ticked session players) and the line is cleared. Either way the command page closes.
*/
void InGameChatInput_SendLineOrCheckCheatPhrase(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  int countOrTabOffset;
  uint32_t slotIndex;
  CommandPayload recipientMask;
  uint32_t slotBit;
  int *dwordCursor;
  uint16_t *textCursor;
  bool isMatch;
  UiNodeBase *recipientTab;

  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)commandTextEdit);
  if ((commandTextEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      /* REPE CMPSD over the 32 UTF-16 units of the cheat phrase */
      countOrTabOffset = 16; /* dwords */
      isMatch = true;
      dwordCursor = (int *)THANDOR_ADDR(g_DeveloperChatPhraseUtf16,0);
      textCursor = commandTextEdit->textBuffer;
      do {
        if (countOrTabOffset == 0) break;
        countOrTabOffset--;
        isMatch = *dwordCursor == *(int *)textCursor;
        dwordCursor++;
        textCursor += 2;
      } while (isMatch);
      if (isMatch) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED;
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_CHEAT_PHRASE_ENTERED;
        InGameRecentTextHistory_InsertAndRebuild8((uint16_t *)u_Hmmm__na_gut________0056321e);
      }
    }
    else {
      RichTextCommandStream_CopyToNarrow
                (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,
                 commandTextEdit->textBuffer);
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,NULL,3,
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientAllTab),
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientGroupsTab),
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientPlayersTab));
      /* Recipient mask: bit 9+n when box n of the faction tab is ticked (messageRecipientPlayersTab), bit 16+n
         for box n of the session player tab (messageRecipientGroupsTab), 0xFFFFFF00 for everyone. The seven
         check boxes sit at g_UiSevenSlotSelectionControlOffsets from the root. */
      countOrTabOffset = (int)recipientTab - (int)commandTextEdit;
      if (countOrTabOffset ==
          (int)offsetof(InGameUiImage,messageRecipientPlayersTab) - (int)offsetof(InGameUiImage,chatInputTextEdit)) {
        slotIndex = 0;
        recipientMask = 0;
        slotBit = INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE;
        do {
          dwordCursor = g_UiSevenSlotSelectionControlOffsets + slotIndex;
          slotBit = slotBit * 2;
          slotIndex++;
          isMatch = (bool)UiSelectableControl_IsSelected
                                  ((UiSelectableControl *)THANDOR_UI_AT(commandTextEdit,*dwordCursor - (int)offsetof(InGameUiImage,chatInputTextEdit)));
          if (isMatch) {
            recipientMask = recipientMask | slotBit;
          }
        } while (slotIndex < 7);
      }
      else if (countOrTabOffset ==
               (int)offsetof(InGameUiImage,messageRecipientGroupsTab) - (int)offsetof(InGameUiImage,chatInputTextEdit)) {
        slotIndex = 0;
        recipientMask = 0;
        slotBit = INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE;
        do {
          dwordCursor = g_UiSevenSlotSelectionControlOffsets + slotIndex;
          slotBit = slotBit * 2;
          slotIndex++;
          isMatch = (bool)UiSelectableControl_IsSelected
                                  ((UiSelectableControl *)THANDOR_UI_AT(commandTextEdit,*dwordCursor - (int)offsetof(InGameUiImage,chatInputTextEdit)));
          if (isMatch) {
            recipientMask = recipientMask | slotBit;
          }
        } while (slotIndex < 7);
      }
      else {
        recipientMask = INGAME_CHAT_RECIPIENT_EVERYONE;
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_SetPackedState(g_LocalPlayerRuntimeId,0,0,recipientMask);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_SET_RECIPIENTS,0,0,recipientMask);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[0].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[0].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[0].payload3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[0].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[0].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[0].payload3);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[1].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[1].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[1].payload3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[1].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[1].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[1].payload3);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[2].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[2].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[2].payload3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[2].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[2].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[2].payload3);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[3].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[3].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[3].payload3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[3].payload1,
                   g_UiSevenSlotCommandPayloadText.triples[3].payload2,
                   g_UiSevenSlotCommandPayloadText.triples[3].payload3);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_PublishConditionalRichText(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_PUBLISH,0,0,0);
      }
      commandTextEdit->cursorIndex = 0;
      commandTextEdit->selectionStart = 0;
      commandTextEdit->selectionEnd = 0;
      textCursor = commandTextEdit->textBuffer;
      for (countOrTabOffset = 24; countOrTabOffset != 0; countOrTabOffset--) { /* 48 code units */
        textCursor[0] = 0;
        textCursor[1] = 0;
        textCursor += 2;
      }
    }
  }
  UiPageStack_SetActiveIndex(0,&THANDOR_CONTAINER_OF(commandTextEdit, InGameCommandTextEntryPage2320, commandTextEdit)->commandPageStack);
  return;
}


/* Address: 0x0050ECE0.
   Writes a save game (called by InGameSaveGame_SaveSelectedOrTypedName with the world view): opens or creates
   the package at savePath (creating its directory if needed) and stores every runtime segment as a
   Huffman/RLE entry - army, modul, shot, effect, widget, light, field, level, daten, campagne (deleted without
   a campaign), stat and oldunit (deleted when empty). Pointer-holding images are converted to offsets for
   writing and rebased afterwards. Finally the 0x200-byte package header gets the save name, date and time
   and the level title and campaign index. Returns true (CF set) on failure; the busy count is raised meanwhile.
*/
bool InGameSaveGame_WritePackage(void *worldView,void *savePath)

{
  uint16_t *timeTextDestination;
  uint8_t *destination;
  InGameLevelConditionStorage *sourceData;
  void *handle;
  uint32_t *modelSlotImage;
  uint32_t *oldUnitImage;
  uint32_t localeValue;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  int remainingCount;
  PckDecodedByteCount unpackedSize;
  uint32_t *headerDwords;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t *sourceCursor;
  uint32_t *destinationCursor;
  bool allZero;
  EngineFileHandle packageHandle;
  uint32_t oldUnitAllocationError;
  ResourceRegistrationImagePair domainImagePair;
  RuntimeHexSegmentImage segmentImage;
  bool upsertOk;

  g_InGameResourceRegistrationBusyCount++;
  /* first hand every player's pending army asset back to its faction */
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount; remainingPlayerBlocks != 0;
       remainingPlayerBlocks--) {
    GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
              (playerBlock->playerRuntimeId,0,0,(playerBlock->factionAssignment).factionAssignmentIndex);
    playerBlock++;
  }
  if (!InGameSaveGame_CreatePackage(savePath,&packageHandle)) {
    WidePath_SplitParentAndLeaf((uint16_t *)g_PackageScratchBuffer,(uint16_t *)THANDOR_ADDR(g_ResourceRegistrationDirectoryUtf16,0),savePath);
    if (g_FileSystemCreateDirectoryRecursive
            (FILESYSTEM_CREATE_DIRECTORY_RECURSIVE,(uint16_t *)THANDOR_ADDR(g_ResourceRegistrationDirectoryUtf16,0)) != 0)
      goto failed;
    if (!InGameSaveGame_CreatePackage(savePath,&packageHandle)) goto failed;
  }
  handle = (void *)(uintptr_t)packageHandle;
  ArmyRuntimePool_ConvertPointersToOffsetsForSave();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (PckDecodedByteCount)(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot)),
                              (uint32_t *)g_ArmyRuntimeSlots,(uint16_t *)u_army_hex_0050dfb4,
                              (EngineFileHandle)handle);
  ArmyRuntimePool_RebaseAfterLoad();
  if (!upsertOk) goto failed;
  ModelRuntimePool_UnrebaseBeforeSave();
  /* The unrebase returns the model runtime slot image in EAX and its size (0x400000) in EDX;
     the decompiler lost both. */
  modelSlotImage = (uint32_t *)g_ModelRuntimeSlots;
  unpackedSize = MODEL_RUNTIME_SLOT_COUNT * sizeof(ModelRuntimeSlot);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,unpackedSize,modelSlotImage,
                              (uint16_t *)u_modul_hex_0050dfee,(EngineFileHandle)handle);
  ModelRuntimePool_RebaseAfterLoad();
  if (!upsertOk) goto failed;
  domainImagePair = InGameSaveGame_PrepareShotSlots();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)u_shot_hex_0050dfdc,
                              (EngineFileHandle)handle);
  ShotRuntime_RebaseSlotsAfterLoad();
  if (!upsertOk) goto failed;
  domainImagePair = InGameSaveGame_PrepareEffectSlots();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)u_effect_hex_0050dfc6,
                              (EngineFileHandle)handle);
  EffectRuntime_RebaseSlotsAfterLoad();
  if (!upsertOk) goto failed;
  domainImagePair = InGameSaveGame_PrepareRegistrationRecords(worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)u_widget_hex_0050e02a,
                              (EngineFileHandle)handle);
  ResourceRegistrationRuntime_RebaseLoadedRecords(worldView);
  if (!upsertOk) goto failed;
  segmentImage = RuntimeHexSegment_GetLightImageAndToggleFlag();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)segmentImage.byteSize,
                              segmentImage.image,(uint16_t *)u_light_hex_0050e016,(EngineFileHandle)handle);
  RuntimeHexSegment_ToggleLightImageFlag();
  if (!upsertOk) goto failed;
  segmentImage = RuntimeHexSegment_GetFieldImage(worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)segmentImage.byteSize,
                              segmentImage.image,(uint16_t *)u_field_hex_0050e002,
                              (EngineFileHandle)handle);
  RuntimeHexSegment_AfterFieldImageNoOp(worldView);
  sourceData = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if (!upsertOk) goto failed;
  InGameSaveGame_StoreCameraAsPlayerStart(worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (sourceData->levelImage).header.resourceTables.
                              runtimePrefixByteSizeAndInitialArmyPlacementOffset,(uint32_t *)sourceData
                              ,(uint16_t *)u_level_hex_0050e040,(EngineFileHandle)handle);
  if (!upsertOk) goto failed;
  domainImagePair = InGameSaveGame_PrepareFactionImage();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)u_daten_hex_0050e054,
                              (EngineFileHandle)handle);
  GameFactionRuntime_RebaseLoadedArmyReferences();
  if (!upsertOk) goto failed;
  if (g_FrontendLoadedCampaignAsset == 0) {
    Package_DeleteEntry((uint16_t *)u_campagne_hex_0050e068,(EngineFileHandle)handle,NULL);
  }
  else {
    upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,((uint32_t *)(uintptr_t)g_FrontendLoadedCampaignAsset)[1],
                                g_FrontendLoadedCampaignAsset,(uint16_t *)u_campagne_hex_0050e068,
                                (EngineFileHandle)handle);
    if (!upsertOk) goto failed;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,GAME_STAT_TABLE_BYTES,g_GameStatTableImage,
                      (uint16_t *)u_stat_hex_0050e082,(EngineFileHandle)handle);
  /* The oldunit entry is written when there are old-unit records or any secondary-table dword is set. */
  allZero = false;
  if (g_OldUnitRecordCount == 0) {
    remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4;
    allZero = true;
    sourceCursor = g_OldUnitSecondaryTable;
    do {
      if (remainingCount == 0) break;
      remainingCount--;
      allZero = *sourceCursor == 0;
      sourceCursor++;
    } while (allZero);
  }
  if (allZero) {
    Package_DeleteEntry((uint16_t *)u_oldunit_hex_0050e094,(EngineFileHandle)handle,NULL);
  }
  else {
    oldUnitAllocationError =
         g_MemoryApi.alloc(4 + OLD_UNIT_PRIMARY_TABLE_BYTES + OLD_UNIT_SECONDARY_TABLE_BYTES,(void **)&oldUnitImage);
    sourceCursor = g_OldUnitPrimaryTable;
    if (oldUnitAllocationError != 0) goto failed;
    *oldUnitImage = g_OldUnitRecordCount;
    destinationCursor = oldUnitImage;
    for (remainingCount = OLD_UNIT_PRIMARY_TABLE_BYTES / 4; destinationCursor++, remainingCount != 0;
         remainingCount--) {
      *destinationCursor = *sourceCursor;
      sourceCursor++;
    }
    sourceCursor = g_OldUnitSecondaryTable;
    for (remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
      *destinationCursor = *sourceCursor;
      sourceCursor++;
      destinationCursor++;
    }
    Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(int)destinationCursor - (int)oldUnitImage,oldUnitImage,
                        (uint16_t *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
    g_MemoryApi.free(oldUnitImage);
  }
  destination = g_PackageScratchBuffer;
  headerDwords = (uint32_t *)destination; /* EDX: the 0x200-byte package header just read */
  if ((g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,handle) == 0) &&
     (g_FileSystemReadExact(sizeof(InGameSavePackageHeader),destination,handle) == 0)){
    /* InGameSavePackageHeader: save name (file name of savePath; the directory lands behind the header),
       level title text id, campaign index, "date, time" text, packed date and time. */
    WidePath_SplitParentAndLeaf
              (((InGameSavePackageHeader *)destination)->saveNameUtf16,(uint16_t *)(destination + sizeof(InGameSavePackageHeader)),savePath);
    localeValue = g_LocaleGetPackedCurrentDate();
    ((InGameSavePackageHeader *)destination)->packedDate = localeValue;
    localeValue = g_LocaleGetPackedCurrentTime();
    ((InGameSavePackageHeader *)destination)->packedTime = localeValue;
    localeValue = g_LocaleFormatCurrentDateUtf16(((InGameSavePackageHeader *)destination)->dateTimeTextUtf16);
    timeTextDestination =
         (uint16_t *)((uint8_t *)((InGameSavePackageHeader *)destination)->dateTimeTextUtf16 + localeValue + 4);
    timeTextDestination[-2] = L','; /* ", " between date and time */
    timeTextDestination[-1] = L' ';
    g_LocaleFormatCurrentTimeUtf16(timeTextDestination);
    localeValue = g_InGameLevelCampaignAssociationIndex;
    if (g_FrontendLoadedCampaignAsset == 0) {
      localeValue = INGAME_SAVE_NO_CAMPAIGN;
    }
    ((InGameSavePackageHeader *)headerDwords)->levelTitleTextId = g_InGameLevelTitleTextResourceIndex;
    ((InGameSavePackageHeader *)headerDwords)->campaignIndex = localeValue;
    if ((g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,handle) == 0) &&
       (g_FileSystemWriteExactOrFlush(sizeof(InGameSavePackageHeader),headerDwords,handle) == 0)){
      Package_Unmount((EngineFileHandle)handle);
      g_InGameResourceRegistrationBusyCount--;
      return false;
    }
  }
failed:
  g_InGameResourceRegistrationBusyCount--;
  return true;
}


/* Address: 0x0053D9F0.
   UI action 0x1000 (g_InGameUiActionHandlersPage10[0]): a click on the minimap (InGameUiImage.minimapView,
   root+0x9A1C). Latches the clicked grid
   cell (selectedSourceX/YQ12 = grid column/row into sourceOriginX/YQ12), converts it to world coordinates and
   moves both camera points of the
   world runtime (which lies 0x8FEC bytes before the map control) by the distance to the new centre, then clears
   the field grid dirty flag.
*/
void InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl)

{
  int64_t scaledProduct;
  int xDelta;
  int yComponent;
  
  yComponent = ((UiSelectionGeometryControl *)mapControl)->selectedSourceYQ12;
  ((UiSelectionGeometryControl *)mapControl)->sourceOriginXQ12 = ((UiSelectionGeometryControl *)mapControl)->selectedSourceXQ12;
  ((UiSelectionGeometryControl *)mapControl)->sourceOriginYQ12 = yComponent;
  /* isometric grid to world: x = (2*gx + gy) * FIELD_GRID_WORLD_COLUMN_STEP_X / 2^13,
     y = gy * FIELD_GRID_WORLD_ROW_STEP_Y / 2^12 (64-bit products) */
  scaledProduct = (int64_t)(yComponent + ((UiSelectionGeometryControl *)mapControl)->selectedSourceXQ12 * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
/* the world runtime is the worldView node of the same in-game UI copy */
#define MAP_WORLD ((WorldRuntimeContext *)THANDOR_UI_SIBLING(mapControl,InGameUiImage,minimapView,worldView))
  xDelta = (FIXED_PRODUCT_SHR(scaledProduct, Q12_SHIFT + 1)) -
          MAP_WORLD->motion.targetPositionXQ12;
  yComponent = FIXED_PRODUCT_SHR((int64_t)yComponent * FIELD_GRID_WORLD_ROW_STEP_Y, Q12_SHIFT) -
          MAP_WORLD->motion.targetPositionYQ12;
  MAP_WORLD->motion.targetPositionXQ12 = MAP_WORLD->motion.targetPositionXQ12 + xDelta;
  MAP_WORLD->motion.targetPositionYQ12 = MAP_WORLD->motion.targetPositionYQ12 + yComponent;
  MAP_WORLD->motion.positionXQ12 = MAP_WORLD->motion.positionXQ12 + xDelta;
  MAP_WORLD->motion.positionYQ12 = MAP_WORLD->motion.positionYQ12 + yComponent;
  WorldRuntime_ClearFieldGridDirtyFlag(MAP_WORLD);
#undef MAP_WORLD
  return;
}


/* Gives node the given edge offsets. */
static void InGameUiRuntime_SetEdgeOffsets
          (UiNodeBase *node,int32_t leftOffset,int32_t topOffset,int32_t rightOffset,int32_t bottomOffset)

{
  node->leftOffset = leftOffset;
  node->topOffset = topOffset;
  node->rightOffset = rightOffset;
  node->bottomOffset = bottomOffset;
}


/* The graphics variant digit in "gfx\panel\panel0.gfx" / "gfx\panel\diagram0.gfx" (0 below 800x600, 1 below
   1024x768, 2 otherwise) and the resource gauge geometry follow the display size. */
static void InGameUiRuntime_SelectDisplayModeLayout(UiRootNode *inGameRoot)

{
  uint16_t variantDigit;

  if ((g_FramebufferWidth < 800) || (g_FramebufferHeight < 600)) {
    variantDigit = L'0';
  }
  else if ((g_FramebufferWidth < 1024) || (g_FramebufferHeight < 768)) {
    variantDigit = L'1';
  }
  else {
    variantDigit = L'2';
  }
  u_gfx_panel_panel0_gfx_005630d0[INGAME_PANEL_GFX_PATH_VARIANT_DIGIT] = variantDigit;
  u_gfx_panel_diagram0_gfx_00563120[INGAME_DIAGRAM_GFX_PATH_VARIANT_DIGIT] = variantDigit;
  if (variantDigit == L'0') {
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteGauge),36,6,94,13);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,tritiumGauge),36,17,94,24);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,energyGauge),36,28,94,35);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteAmountText),4,5,31,13);
  }
  else {
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteGauge),44,9,110,16);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,tritiumGauge),44,23,110,30);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,energyGauge),44,37,110,44);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteAmountText),4,7,39,15);
  }
}


/* Caches the subresource sizes of panel0.gfx that the side panel layout needs. */
static void InGameUiRuntime_CachePanelSubresourceSizes(GraphicsTextureSourceAsset *panelTexture)

{
  GraphicsTextureLogicalSize logicalSize;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,panelTexture);
  g_InGamePanelTextureSubresource00Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(1,panelTexture);
  g_InGamePanelTextureSubresource01Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(2,panelTexture);
  g_InGamePanelTextureSubresource02Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(6,panelTexture);
  g_InGamePanelTextureSubresource06Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(7,panelTexture);
  g_InGamePanelTextureSubresource07Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(27,panelTexture);
  g_InGamePanelTextureSubresource27Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(28,panelTexture);
  g_InGamePanelTextureSubresource28Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(19,panelTexture);
  g_InGamePanelTextureSubresource19Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(20,panelTexture);
  g_InGamePanelTextureSubresource20Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(34,panelTexture);
  g_InGamePanelTextureSubresource34Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(32,panelTexture);
  g_InGamePanelTextureSubresource32Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(33,panelTexture);
  g_InGamePanelTextureSubresource33Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(2,panelTexture);
  g_InGamePanelTextureSubresource02Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(3,panelTexture);
  g_InGamePanelTextureSubresource03Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(4,panelTexture);
  g_InGamePanelTextureSubresource04Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(5,panelTexture);
  g_InGamePanelTextureSubresource05Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(36,panelTexture);
  g_InGamePanelTextureSubresource36Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(37,panelTexture);
  g_InGamePanelTextureSubresource37Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(6,panelTexture);
  g_InGamePanelTextureSubresource06Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,panelTexture);
  g_InGamePanelTextureSubresource00Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(7,panelTexture);
  g_InGamePanelTextureSubresource07Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(26,panelTexture);
  g_InGamePanelTextureSubresource26Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(31,panelTexture);
  g_InGamePanelTextureSubresource31Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(18,panelTexture);
  g_InGamePanelTextureSubresource18Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(23,panelTexture);
  g_InGamePanelTextureSubresource23Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(34,panelTexture);
  g_InGamePanelTextureSubresource34Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(32,panelTexture);
  g_InGamePanelTextureSubresource32Height = logicalSize.logicalHeightPixels;
}


/* Zeroes the edge offsets of the side panel frame parts and binds panel0.gfx to every panel control (the texture
   field sits at +0x50, +0x54 or +0x74 depending on the control class). */
static void InGameUiRuntime_BindPanelTexture(UiRootNode *inGameRoot,GraphicsTextureSourceAsset *panelTexture)

{
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameLeftEdge),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameLeftEdge))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameRightEdge),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameRightEdge))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameTopCap),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameTopCap))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameMenuBar),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameMenuBar))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameInfoSection),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameInfoSection))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameBottomCap),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameBottomCap))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,resourcePanel),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,resourcePanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,editorTabStripA))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,gamePanelsArea),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,gamePanelsArea))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,editorTabStripB))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8Popup))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9Popup))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,diplomacyFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,buildCatalogFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,specialBuildCatalogFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,armyStockFrame))->textureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainHeight))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainMaterial))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,resourcePanelIconButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainSmoothing))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,inGameMenuButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,missionObjectivesButton))->primaryTextureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,countdownDisplayPanel))->textureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,diplomacyPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabRegion))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,buildCatalogPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabUnitPlacement))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,specialBuildCatalogPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabObjectPlacement))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,armyStockPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton4))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton5))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton6))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton7))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxGatedButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxLandButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption1))->primaryTextureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,minimapView),0,0,0,0);
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,modePreviewPageStack),0,0,0,0);
  ((UiImageActionControl *)INGAME_UI(inGameRoot,notificationTargetButton))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,heightToolPreview))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,smoothingToolPreview))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,regionToolPreview))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,modeDetailPageStack),0,0,0,0);
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,singleSelectionUpgradeButton))->primaryTextureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,selectionDetailPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,heightToolPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,materialPalettePanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,smoothingToolPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,unitPlacementPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,objectPlacementPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,regionToolPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry24))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry25))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry26))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry27))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry28))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry29))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry30))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry31))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry32))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry33))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry34))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry35))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry36))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry37))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry38))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry39))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry40))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry41))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry42))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry43))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry44))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry45))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry46))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry47))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry24))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry25))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry26))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry27))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry28))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry29))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry30))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry31))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry32))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry33))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry34))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry35))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry36))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry37))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry38))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry39))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry40))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry41))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->primaryTextureSource = panelTexture;
}


/* Sizes the side panel parts from the panel subresources: the frame and the pages inside it move left by the
   widths of subresources 1 and 2 (the left edge also by subresource 0), and down by the heights of the frame
   pieces above them (subresources 2, 36, 3, 37, 4); the bottom cap and the mode detail page end above
   subresources 0 and 5. */
static void InGameUiRuntime_SizeSidePanelFrame(UiRootNode *inGameRoot)

{
  UiNodeBase *leftEdge;
  UiNodeBase *rightEdge;
  UiNodeBase *topCap;
  UiNodeBase *menuBar;
  UiNodeBase *infoSection;
  UiNodeBase *bottomCap;
  UiNodeBase *minimap;
  UiNodeBase *modePreview;
  UiNodeBase *modeDetail;
  int32_t size;

  leftEdge = INGAME_UI(inGameRoot,sidePanelFrameLeftEdge);
  rightEdge = INGAME_UI(inGameRoot,sidePanelFrameRightEdge);
  topCap = INGAME_UI(inGameRoot,sidePanelFrameTopCap);
  menuBar = INGAME_UI(inGameRoot,sidePanelFrameMenuBar);
  infoSection = INGAME_UI(inGameRoot,sidePanelFrameInfoSection);
  bottomCap = INGAME_UI(inGameRoot,sidePanelFrameBottomCap);
  minimap = INGAME_UI(inGameRoot,minimapView);
  modePreview = INGAME_UI(inGameRoot,modePreviewPageStack);
  modeDetail = INGAME_UI(inGameRoot,modeDetailPageStack);
  size = g_InGamePanelTextureSubresource01Width;
  leftEdge->leftOffset -= size;
  leftEdge->rightOffset -= size;
  rightEdge->leftOffset -= size;
  topCap->leftOffset -= size;
  topCap->rightOffset -= size;
  menuBar->leftOffset -= size;
  menuBar->rightOffset -= size;
  infoSection->leftOffset -= size;
  infoSection->rightOffset -= size;
  bottomCap->leftOffset -= size;
  bottomCap->rightOffset -= size;
  minimap->leftOffset -= size;
  minimap->rightOffset -= size;
  modePreview->leftOffset -= size;
  modePreview->rightOffset -= size;
  modeDetail->leftOffset -= size;
  modeDetail->rightOffset -= size;
  size = g_InGamePanelTextureSubresource02Width;
  leftEdge->leftOffset -= size;
  leftEdge->rightOffset -= size;
  topCap->leftOffset -= size;
  menuBar->leftOffset -= size;
  infoSection->leftOffset -= size;
  bottomCap->leftOffset -= size;
  minimap->leftOffset -= size;
  modePreview->leftOffset -= size;
  modeDetail->leftOffset -= size;
  leftEdge->leftOffset -= g_InGamePanelTextureSubresource00Width;
  INGAME_UI(inGameRoot,resourcePanel)->leftOffset -= g_InGamePanelTextureSubresource06Width;
  INGAME_UI(inGameRoot,gamePanelsArea)->leftOffset -= g_InGamePanelTextureSubresource07Width;
  size = g_InGamePanelTextureSubresource02Height;
  topCap->bottomOffset += size;
  menuBar->topOffset += size;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  minimap->topOffset += size;
  minimap->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource36Height;
  menuBar->topOffset += size;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  minimap->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource03Height;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource37Height;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource04Height;
  infoSection->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource00Height;
  bottomCap->topOffset += size;
  bottomCap->bottomOffset += size;
  modeDetail->bottomOffset += size;
  size = g_InGamePanelTextureSubresource05Height;
  bottomCap->topOffset -= size;
  modeDetail->bottomOffset -= size;
  INGAME_UI(inGameRoot,resourcePanel)->bottomOffset += g_InGamePanelTextureSubresource06Height;
  INGAME_UI(inGameRoot,gamePanelsArea)->topOffset -= g_InGamePanelTextureSubresource07Height;
}


/* The menu buttons and the countdown share the menu bar's rows, the selection group buttons the info section's;
   all of them span the side panel frame horizontally. The selection group buttons then form a 4x2 grid (left
   +5/+36/+66/+97, top +17/+40). The world view ends where the side panel stack begins. */
static void InGameUiRuntime_PlaceMenuAndSelectionGroupButtons(UiRootNode *inGameRoot)

{
  static const int32_t groupButtonColumnShifts[4] = { 5, 36, 66, 97 };
  static const int32_t groupButtonRowShifts[2] = { 17, 40 };
  UiNodeBase *groupButtons[8];
  UiNodeBase *menuBar;
  UiNodeBase *infoSection;
  int32_t frameLeft;
  int32_t frameRight;
  int buttonIndex;

  menuBar = INGAME_UI(inGameRoot,sidePanelFrameMenuBar);
  infoSection = INGAME_UI(inGameRoot,sidePanelFrameInfoSection);
  frameLeft = INGAME_UI(inGameRoot,sidePanelFrameLeftEdge)->leftOffset;
  frameRight = INGAME_UI(inGameRoot,sidePanelFrameRightEdge)->rightOffset;
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,inGameMenuButton),frameLeft,menuBar->topOffset,frameRight,menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,missionObjectivesButton),frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,countdownDisplayPanel),frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  groupButtons[0] = INGAME_UI(inGameRoot,selectionGroupButton0);
  groupButtons[1] = INGAME_UI(inGameRoot,selectionGroupButton1);
  groupButtons[2] = INGAME_UI(inGameRoot,selectionGroupButton2);
  groupButtons[3] = INGAME_UI(inGameRoot,selectionGroupButton3);
  groupButtons[4] = INGAME_UI(inGameRoot,selectionGroupButton4);
  groupButtons[5] = INGAME_UI(inGameRoot,selectionGroupButton5);
  groupButtons[6] = INGAME_UI(inGameRoot,selectionGroupButton6);
  groupButtons[7] = INGAME_UI(inGameRoot,selectionGroupButton7);
  for (buttonIndex = 0; buttonIndex < 8; buttonIndex++) {
    InGameUiRuntime_SetEdgeOffsets
              (groupButtons[buttonIndex],frameLeft + groupButtonColumnShifts[buttonIndex % 4],
               infoSection->topOffset + groupButtonRowShifts[buttonIndex / 4],frameRight,infoSection->bottomOffset);
  }
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,worldViewArea),0,0,frameLeft,0);
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelStack),frameLeft,0,0,0);
}


/* Sets the edges of one grid cell: edges[n] is the right/bottom edge of column/row n (counted from the bottom right
   cell) and edges[n + 1] its left/top edge. */
static void InGameUiRuntime_PlaceGridCell
          (UiNodeBase *cell,const int32_t *columnEdges,const int32_t *rowEdges,int column,int row)

{
  cell->leftOffset = columnEdges[column + 1];
  cell->topOffset = rowEdges[row + 1];
  cell->rightOffset = columnEdges[column];
  cell->bottomOffset = rowEdges[row];
}


/* Cells of the build catalog, special build catalog and army stock grids, laid out from the bottom right: each
   further column/row moves one subresource-34 cell to the left/up. */
static void InGameUiRuntime_PlaceCatalogGridCells(UiRootNode *inGameRoot)

{
  int32_t columnEdges[9];
  int32_t rowEdges[8];
  int edgeIndex;

  columnEdges[0] = -g_InGamePanelTextureSubresource28Width;
  for (edgeIndex = 1; edgeIndex < 9; edgeIndex++) {
    columnEdges[edgeIndex] = columnEdges[edgeIndex - 1] - g_InGamePanelTextureSubresource34Width;
  }
  rowEdges[0] = -g_InGamePanelTextureSubresource31Height;
  for (edgeIndex = 1; edgeIndex < 8; edgeIndex++) {
    rowEdges[edgeIndex] = rowEdges[edgeIndex - 1] - g_InGamePanelTextureSubresource34Height;
  }
  /* build catalog: 4x6 cells, then 4 columns of 6 */
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry23),columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry24),columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry25),columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry26),columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry27),columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry28),columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry29),columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry30),columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry31),columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry32),columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry33),columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry34),columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry35),columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry36),columnEdges,rowEdges,6,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry37),columnEdges,rowEdges,6,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry38),columnEdges,rowEdges,6,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry39),columnEdges,rowEdges,6,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry40),columnEdges,rowEdges,6,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry41),columnEdges,rowEdges,6,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry42),columnEdges,rowEdges,7,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry43),columnEdges,rowEdges,7,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry44),columnEdges,rowEdges,7,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry45),columnEdges,rowEdges,7,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry46),columnEdges,rowEdges,7,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry47),columnEdges,rowEdges,7,5);
  /* special build catalog: 4x7 cells, then 2 columns of 7 */
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry23),columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry24),columnEdges,rowEdges,0,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry25),columnEdges,rowEdges,1,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry26),columnEdges,rowEdges,2,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry27),columnEdges,rowEdges,3,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry28),columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry29),columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry30),columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry31),columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry32),columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry33),columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry34),columnEdges,rowEdges,4,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry35),columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry36),columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry37),columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry38),columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry39),columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry40),columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry41),columnEdges,rowEdges,5,6);
  /* army stock: 4x6 cells */
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot23),columnEdges,rowEdges,3,5);
  /* the technology description scroll's left edge and frame's right edge move one catalog cell width */
  INGAME_UI(inGameRoot,technologyDescriptionScroll)->leftOffset += g_InGamePanelTextureSubresource34Width;
  INGAME_UI(inGameRoot,technologyDescriptionFrame)->rightOffset += g_InGamePanelTextureSubresource34Width;
}


/* Shifts the right edge of one diplomacy row's label columns and places its relation button. */
static void InGameUiRuntime_ShiftDiplomacyRowLabels
          (UiNodeBase *relationButton,UiNodeBase *playerNumberLabel,UiNodeBase *relationLabel,
           UiNodeBase *playerNameLabel,UiNodeBase *factionLabel,int32_t labelShift)

{
  relationButton->leftOffset = labelShift;
  playerNumberLabel->rightOffset += labelShift;
  relationLabel->rightOffset += labelShift;
  playerNameLabel->rightOffset += labelShift;
  factionLabel->rightOffset += labelShift;
}


/* The seven diplomacy rows, one subresource-32 height apart from the bottom, then their label columns. */
static void InGameUiRuntime_PlaceDiplomacyRows(UiRootNode *inGameRoot)

{
  UiNodeBase *rows[7];
  int32_t rowLeftOffset;
  int32_t rowRightOffset;
  int32_t rowBottomShift;
  int32_t rowTopShift;
  int32_t labelShift;
  int rowIndex;

  rows[0] = INGAME_UI(inGameRoot,diplomacyRow1);
  rows[1] = INGAME_UI(inGameRoot,diplomacyRow2);
  rows[2] = INGAME_UI(inGameRoot,diplomacyRow3);
  rows[3] = INGAME_UI(inGameRoot,diplomacyRow4);
  rows[4] = INGAME_UI(inGameRoot,diplomacyRow5);
  rows[5] = INGAME_UI(inGameRoot,diplomacyRow6);
  rows[6] = INGAME_UI(inGameRoot,diplomacyRow7);
  rowLeftOffset = g_InGamePanelTextureSubresource19Width;
  rowRightOffset = -g_InGamePanelTextureSubresource20Width;
  rowBottomShift = g_InGamePanelTextureSubresource23Height;
  for (rowIndex = 0; rowIndex < 7; rowIndex++) {
    rowTopShift = rowBottomShift + g_InGamePanelTextureSubresource32Height;
    rows[rowIndex]->bottomOffset -= rowBottomShift;
    rows[rowIndex]->leftOffset = rowLeftOffset;
    rows[rowIndex]->rightOffset = rowRightOffset;
    rows[rowIndex]->topOffset -= rowTopShift;
    rowBottomShift = rowTopShift;
  }
  labelShift = -g_InGamePanelTextureSubresource33Width;
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow1RelationButton),INGAME_UI(inGameRoot,diplomacyRow1PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow1RelationLabel),INGAME_UI(inGameRoot,diplomacyRow1PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow1FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow2RelationButton),INGAME_UI(inGameRoot,diplomacyRow2PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow2RelationLabel),INGAME_UI(inGameRoot,diplomacyRow2PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow2FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow3RelationButton),INGAME_UI(inGameRoot,diplomacyRow3PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow3RelationLabel),INGAME_UI(inGameRoot,diplomacyRow3PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow3FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow4RelationButton),INGAME_UI(inGameRoot,diplomacyRow4PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow4RelationLabel),INGAME_UI(inGameRoot,diplomacyRow4PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow4FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow5RelationButton),INGAME_UI(inGameRoot,diplomacyRow5PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow5RelationLabel),INGAME_UI(inGameRoot,diplomacyRow5PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow5FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow6RelationButton),INGAME_UI(inGameRoot,diplomacyRow6PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow6RelationLabel),INGAME_UI(inGameRoot,diplomacyRow6PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow6FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow7RelationButton),INGAME_UI(inGameRoot,diplomacyRow7PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow7RelationLabel),INGAME_UI(inGameRoot,diplomacyRow7PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow7FactionLabel),labelShift);
}


/* Gives target the edge offsets (leftOffset..bottomOffset) of source. */
static void InGameUiRuntime_CopyEdgeOffsets(UiNodeBase *target,const UiNodeBase *source)

{
  target->leftOffset = source->leftOffset;
  target->topOffset = source->topOffset;
  target->rightOffset = source->rightOffset;
  target->bottomOffset = source->bottomOffset;
}


/* The editor tab strips cover the resource panel and the game panel area; the editor tool option buttons reuse
   the selection group button positions. */
static void InGameUiRuntime_PlaceEditorToolOptions(UiRootNode *inGameRoot)

{
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,editorTabStripA),INGAME_UI(inGameRoot,resourcePanel));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,editorTabStripB),INGAME_UI(inGameRoot,gamePanelsArea));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,regionToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption2),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption2),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,regionToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption1),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption1),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingRelaxGatedButton),INGAME_UI(inGameRoot,selectionGroupButton6));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption3),INGAME_UI(inGameRoot,selectionGroupButton7));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption3),INGAME_UI(inGameRoot,selectionGroupButton7));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingRelaxLandButton),INGAME_UI(inGameRoot,selectionGroupButton7));
}


/* Selection detail page: icon/metrics box of one catalog cell plus a 2 pixel border with the text below it, the
   placeholders of the selection detail text templates, and the 12 metric cells of the multi-selection page. */
static void InGameUiRuntime_LayoutSelectionDetailPage(UiRootNode *inGameRoot)

{
  int32_t iconHeight;
  int32_t paddedIconWidth;
  int32_t paddedIconHeight;
  int32_t textWrapWidth;
  TextResourceId resourceId;
  uint16_t *templateText;
  int columnsRemaining;
  int cellSize;
  int cellLeft;
  int cellTop;
  uint32_t cellIndex;
  UiNodeBase *cell;

  iconHeight = g_InGamePanelTextureSubresource34Height;
  paddedIconWidth = g_InGamePanelTextureSubresource34Width + 2;
  paddedIconHeight = g_InGamePanelTextureSubresource34Height + 2;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->rightOffset = paddedIconWidth;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->bottomOffset = paddedIconHeight;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->leftOffset = 2;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->topOffset = 2;
  INGAME_UI(inGameRoot,hoverItemIcon)->rightOffset = paddedIconWidth;
  INGAME_UI(inGameRoot,hoverItemIcon)->bottomOffset = paddedIconHeight;
  INGAME_UI(inGameRoot,hoverItemIcon)->leftOffset = 2;
  INGAME_UI(inGameRoot,hoverItemIcon)->topOffset = 2;
  textWrapWidth = g_InGamePanelTextureSubresource02Width - 4;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->topOffset = iconHeight + 4;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,singleSelectionStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
  INGAME_UI(inGameRoot,hoverItemStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,hoverItemStatsText)->topOffset = iconHeight + 4;
  INGAME_UI(inGameRoot,hoverItemStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,hoverItemStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->topOffset = 2;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,unitPlacementStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
  /* point the placeholders 0..9 of the selection detail text templates at the shared value buffers */
  for (resourceId = TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE; resourceId < TEXT_ID_MODEL_NAME_BASE; resourceId++) {
    templateText = TextResource_Resolve(resourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,g_InGameSelectionDetailNameTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_InGameSelectionDetailArmourTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(2,g_InGameSelectionDetailWeaponName0TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(3,g_InGameSelectionDetailWeaponName1TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(4,g_InGameSelectionDetailWeaponName2TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(5,g_InGameSelectionDetailTextSlot05Utf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(6,g_InGameSelectionDetailBuildXeniteCostTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(7,g_InGameSelectionDetailBuildTimeTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(8,g_InGameSelectionDetailEnergyTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(9,g_InGameSelectionDetailTextSlot09Utf16,templateText);
  }
  /* multi-selection page: 12 metric cells in rows of three, each a third of the panel width square */
  columnsRemaining = 3;
  cellSize = (int)((uint64_t)(int64_t)g_InGamePanelTextureSubresource02Width / 3);
  cellLeft = 0;
  cellTop = 0;
  for (cellIndex = 0; cellIndex < 12; cellIndex++) {
    cell = THANDOR_UI_AT(inGameRoot,g_InGameSelectionDetailGridCellOffsets[cellIndex]);
    cell->leftOffset = cellLeft;
    cell->topOffset = cellTop;
    cell->rightOffset = cellLeft + cellSize;
    cell->bottomOffset = cellTop + cellSize;
    columnsRemaining--;
    if (columnsRemaining == 0) {
      columnsRemaining = 3;
      cellLeft = 0;
      cellTop = cellTop + cellSize;
    }
    else {
      cellLeft = cellLeft + cellSize;
    }
  }
}


/* Sizes the technology window around tech.gfx: the seven area tabs and the description scroll move up by one tab
   icon height, the window grows by seven icon widths and one icon height around its centre. */
static void InGameUiRuntime_SizeTechnologyWindow(UiRootNode *inGameRoot,GraphicsTextureSourceAsset *techTexture)

{
  GraphicsTextureLogicalSize logicalSize;
  uint32_t techTextureHeight;
  uint32_t halfWidthGrowth;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,techTexture);
  techTextureHeight = logicalSize.logicalHeightPixels;
  INGAME_UI(inGameRoot,technologyAreaTab1)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab2)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab3)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab4)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab5)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab6)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab7)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyDescriptionScroll)->bottomOffset -= techTextureHeight;
  halfWidthGrowth = logicalSize.logicalWidthPixels * 7 >> 1;
  INGAME_UI(inGameRoot,technologyWindow)->leftOffset -= halfWidthGrowth;
  INGAME_UI(inGameRoot,technologyWindow)->rightOffset += halfWidthGrowth;
  INGAME_UI(inGameRoot,technologyWindow)->topOffset -= techTextureHeight >> 1;
  INGAME_UI(inGameRoot,technologyWindow)->bottomOffset += techTextureHeight >> 1;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->wrapWidth =
       (INGAME_UI(inGameRoot,technologyWindow)->rightOffset - INGAME_UI(inGameRoot,technologyWindow)->leftOffset) + -24 +
       (INGAME_UI(inGameRoot,technologyDescriptionScroll)->rightOffset - INGAME_UI(inGameRoot,technologyDescriptionScroll)->leftOffset);
}


/* Click sounds: voice sets 0..6 of g_UiButtonSoundVoiceSets7, stored at the control class's sound field (+0x5C,
   +0x64, +0x68 or +0x70). */
static void InGameUiRuntime_AssignClickSounds(UiRootNode *inGameRoot)

{
  DirectSoundVoiceSet *buttonVoiceSet;

  buttonVoiceSet = g_UiButtonSoundVoiceSets7[0];
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8))->pointerActivationSound = g_UiButtonSoundVoiceSets7[0];
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9))->pointerActivationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,resourcePanelIconButton))->activationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,diplomacyPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,buildCatalogPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,specialBuildCatalogPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,armyStockPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainHeight))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainMaterial))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainSmoothing))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabRegion))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabUnitPlacement))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabObjectPlacement))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[1];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,inGameMenuButton))->activationSound = g_UiButtonSoundVoiceSets7[1];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,missionObjectivesButton))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[2];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton0))->activationSound = g_UiButtonSoundVoiceSets7[2];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton4))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton5))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton6))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton7))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxLandButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,singleSelectionUpgradeButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry23))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry24))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry25))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry26))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry27))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry28))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry29))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry30))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry31))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry32))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry33))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry34))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry35))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry36))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry37))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry38))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry39))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry40))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry41))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry42))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry43))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry44))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry45))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry46))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry47))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry23))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry24))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry25))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry26))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry27))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry28))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry29))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry30))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry31))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry32))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry33))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry34))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry35))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry36))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry37))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry38))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry39))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry40))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry41))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot23))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabMilitary))->activationSound = g_UiButtonSoundVoiceSets7[3];
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabEconomy))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabThird))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsContinueButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsSecondaryExitButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsChartModeButtonA))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsChartModeButtonB))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuSaveButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuGraphicsButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuQuitButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuAudioButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameSaveButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameDeleteButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuAbortMissionButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuSurrenderButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuRestartMissionButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,graphicsOptionsBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,soundOptionsBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageSendButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageSendAndCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageCancelButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpCloseButton))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,autoZoomOffCheckbox))->activationSound = g_UiButtonSoundVoiceSets7[4];
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,autoRotationOffCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,linkRotationZoomCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,linkRotationTiltCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,hidePanelCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,shadingEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityLowButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityMediumButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityHighButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,musicEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,effectsEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,reverseStereoCheckbox))->activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x32Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x64Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x128Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel64x64Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel64x128Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel128x128Button))->base.activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientPlayersTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientAllTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientGroupsTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox1))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox2))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox3))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox4))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox5))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox6))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox7))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab1))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab2))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab3))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab4))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab5))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab6))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab7))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpBriefingTab))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpKeyboardTab))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpMouseTab))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[5];
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,modelDetailSlider))->clickSound = g_UiButtonSoundVoiceSets7[5];
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,effectsVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,movieVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,musicVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,messageMovieVolumeSlider))->clickSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,saveNameEdit))->activationSound = g_UiButtonSoundVoiceSets7[6];
  ((UiListControl *)INGAME_UI(inGameRoot,saveGameList))->activationSound = buttonVoiceSet;
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,messageTextEdit))->activationSound = buttonVoiceSet;
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,chatInputTextEdit))->activationSound = buttonVoiceSet;
}


/* Loads a graphics package; on success it replaces the package in *slot (an atomic exchange in the original) and
   the previous package is released. Returns the loaded package, or NULL with the loader's error in *loadError. */
static GraphicsTextureSourceAsset *InGameUiRuntime_ReplaceTexturePackage
          (uint16_t *packagePath,GraphicsTextureSourceAsset **slot,uint32_t *loadError)

{
  GraphicsTextureSourceAsset *loadedPackage;
  GraphicsTextureSourceAsset *previousPackage;

  loadedPackage = g_GraphicsTextureSourceLoadPackageAsset(packagePath,loadError);
  previousPackage = *slot;
  if (loadedPackage != NULL) {
    *slot = loadedPackage;
    g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(previousPackage);
  }
  return loadedPackage;
}


/* Address: 0x0055C990.
   Lays out the freshly copied in-game UI template for the current display mode (called by both session
   initialisers): picks the panel/diagram graphics variant (gfx\panel\panel0/diagram0 with the digit 0, 1 or 2
   for below 800x600, below 1024x768, or larger) and the resource gauge geometry, loads panel0.gfx and caches the
   subresource sizes the layout needs, binds the texture to the side panel controls and positions them (frame
   edges, menu bar, selection group buttons, the build catalog / special catalog / army stock grids, diplomacy
   rows, editor tool options, selection detail page), patches the selection detail text templates
   0x18002C..0x18004E, then loads diagram0.gfx, window.gfx and tech.gfx (sizing the technology window) and assigns
   the UI click sounds. Returns true on success; false with the loader's error in *outError when a graphics
   package cannot be loaded.
   The diagram, window and technology texture slots are typed uint32_t in the image data, hence the slot casts.
*/
bool InGameUiRuntime_InitializeControlTreeResources(UiRootNode *inGameRoot,uint32_t *outError)

{
  uint32_t textureLoadError;
  GraphicsTextureSourceAsset *panelTexture;
  GraphicsTextureSourceAsset *diagramTexture;
  GraphicsTextureSourceAsset *windowTexture;
  GraphicsTextureSourceAsset *techTexture;

  InGameUiRuntime_SelectDisplayModeLayout(inGameRoot);
  panelTexture = InGameUiRuntime_ReplaceTexturePackage
                   ((uint16_t *)u_gfx_panel_panel0_gfx_005630d0,&g_InGamePanelTextureSource,&textureLoadError);
  if (panelTexture == NULL) {
    *outError = textureLoadError;
    return false;
  }
  InGameUiRuntime_CachePanelSubresourceSizes(panelTexture);
  InGameUiRuntime_BindPanelTexture(inGameRoot,panelTexture);
  InGameUiRuntime_SizeSidePanelFrame(inGameRoot);
  InGameUiRuntime_PlaceMenuAndSelectionGroupButtons(inGameRoot);
  InGameUiRuntime_PlaceCatalogGridCells(inGameRoot);
  InGameUiRuntime_PlaceDiplomacyRows(inGameRoot);
  InGameUiRuntime_PlaceEditorToolOptions(inGameRoot);
  InGameUiRuntime_LayoutSelectionDetailPage(inGameRoot);

  diagramTexture = InGameUiRuntime_ReplaceTexturePackage
                     ((uint16_t *)u_gfx_panel_diagram0_gfx_00563120,
                      (GraphicsTextureSourceAsset **)&g_InGameDiagramTextureSource,&textureLoadError);
  if (diagramTexture == NULL) {
    *outError = textureLoadError;
    return false;
  }
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,xeniteGauge))->textureSource = diagramTexture;
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,tritiumGauge))->textureSource = diagramTexture;
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,energyGauge))->textureSource = diagramTexture;

  windowTexture = InGameUiRuntime_ReplaceTexturePackage
                    ((uint16_t *)u_gfx_panel_window_gfx_0056318e,
                     (GraphicsTextureSourceAsset **)&g_InGameWindowTextureSource,&textureLoadError);
  if (windowTexture == NULL) {
    *outError = textureLoadError;
    return false;
  }
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,messageWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,gameMenuWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,quitGameWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,saveGameWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,graphicsSettingsWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,audioSettingsWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,missionHelpWindow))->textureSource = windowTexture;

  techTexture = InGameUiRuntime_ReplaceTexturePackage
                  ((uint16_t *)u_gfx_panel_tech_gfx_005630fa,
                   (GraphicsTextureSourceAsset **)&g_InGameTechnologyTextureSource,&textureLoadError);
  if (techTexture == NULL) {
    *outError = textureLoadError;
    return false;
  }
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab1Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab2Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab3Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab4Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab5Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab6Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab7Icon))->textureSource = techTexture;
  InGameUiRuntime_SizeTechnologyWindow(inGameRoot,techTexture);
  InGameUiRuntime_AssignClickSounds(inGameRoot);
  return true;
}


/* Address: 0x00563BD0.
   Per-tick HUD text update. Every 20 ticks (one second) it formats the render statistics into the debug overlay
   and reports the local player as slow (fewer than 13 frames in that second) or no longer slow; every tick it
   formats the camera pose, the selection point, free memory and the elapsed game time, and builds the faction
   status lines (name, player roster with pause/speed/slow marks, a counter) for the active factions 1..7.
*/
void InGameHud_UpdateStatusCountersAndSessionPrompts(void)

{
  SelectionPlayerRuntimeBlock *selectionBlock;
  uint32_t stepTicks;
  uint64_t elapsedMinutes;
  uint32_t value;
  uint16_t *stream;
  uint32_t frameOrFactionIndex;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  WorldRuntimeContext *world;
  FrontendPlayerRuntimeRecord *playerBlock;
  int factionRecordAddress;
  int rosterCount;
  uint16_t *rosterCursor;
  uint16_t *destination;
  uint32_t copiedByteCount;
  uint16_t *resolvedText;
  uint16_t *statusTemplate;
  WorldCameraPosition cameraPosition;
  WorldCameraOrientation cameraOrientation;
  InGameRuntimeRoot *runtimeRoot;
  
  frameOrFactionIndex = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown--;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 20;
    /* frames, then draw calls / texture binds / texture reloads per frame (2 decimals) */
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    if (frameOrFactionIndex == 0) {
      frameOrFactionIndex = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameOrFactionIndex,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameOrFactionIndex,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameOrFactionIndex,
               g_TextureDeviceReloadCount,g_FrontendDebugOverlayTextSlot03Utf16);
    /* bit 0 of g_InGameReadyStateToggleFlags: the slow state is currently reported */
    if ((g_InGameReadyStateToggleFlags & 1) == 0) {
      if (g_RenderedFrameCountSinceDebugRefresh < 13) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_SetSlowRenderingFlagById(g_LocalPlayerRuntimeId,0,0,PLAYER_SESSION_FLAG_SLOW_RENDERING);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SET_SLOW_RENDERING,0,0,
                                                      PLAYER_SESSION_FLAG_SLOW_RENDERING);
        }
        g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
      }
    }
    else if (12 < g_RenderedFrameCountSinceDebugRefresh) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_SetSlowRenderingFlagById(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SET_SLOW_RENDERING,0,0,0);
      }
      g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
    }
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
    g_TextureBindStateChangeCount = 0;
    g_TextureDeviceReloadCount = 0;
  }
  runtimeRoot = g_InGameRuntimeRoot;
  world = &g_InGameRuntimeRoot->worldRuntime;
  cameraPosition = WorldRuntime_GetCameraPosition(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  cameraOrientation = WorldRuntime_GetCameraOrientation(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  /* selection point, or "-" while pointerSurfaceHitDepth holds the 0x7FFFFFFF "none" marker */
  if ((runtimeRoot->worldRuntime).selection.pointerSurfaceHitDepth == INT32_MAX) {
    g_FrontendDebugOverlayTextSlot10Utf16[0] = L'-';
    g_FrontendDebugOverlayTextSlot10Utf16[1] = 0;
    g_FrontendDebugOverlayTextSlot11Utf16[0] = L'-';
    g_FrontendDebugOverlayTextSlot11Utf16[1] = 0;
  }
  else {
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,(runtimeRoot->worldRuntime).selection.pointerSurfaceHitWorldX,
               g_FrontendDebugOverlayTextSlot10Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,(runtimeRoot->worldRuntime).selection.pointerSurfaceHitWorldY,
               g_FrontendDebugOverlayTextSlot11Utf16);
  }
  value = g_MemoryApi.queryFreeBytes();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,value,
             g_FrontendDebugOverlayTextSlot12Utf16);
  /* 1200 simulation ticks = one minute at 20 ticks per second; rounded up, shown as hours and minutes */
  elapsedMinutes = (uint64_t)(g_GameFactionRuntimeImage.tail.simulationTick + 1199) / 1200;
  g_LocaleFormatTimeFieldsUtf16
            ((uint32_t)(elapsedMinutes / 60),(uint32_t)(elapsedMinutes % 60),g_FrontendDebugOverlayTextSlot13Utf16);
  /* faction 0 is skipped */
  frameOrFactionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord));
  destination = g_InGameFactionStatusTextScratchUtf16;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[frameOrFactionIndex] != 0) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frameOrFactionIndex] <
        FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 ((GameFactionRuntimeRecord *)factionRecordAddress)->economyProgressScore + ((GameFactionRuntimeRecord *)factionRecordAddress)->relationScore,(uint16_t *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0));
      rosterCount = 0;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        rosterCursor = g_InGamePlayerListTextScratchUtf16;
        do {
          if (frameOrFactionIndex == (playerBlock->factionAssignment).factionAssignmentIndex) {
            if (rosterCount != 0) {
              rosterCursor[0] = L',';
              rosterCursor[1] = L' ';
              rosterCursor += 2;
            }
            rosterCount++;
            if (RichTextCommandStream_CopyExpanded
                  (40,rosterCursor,(playerBlock->playerName).textUtf16,&copiedByteCount)) {
              rosterCursor = (uint16_t *)((int)rosterCursor + copiedByteCount);
              selectionBlock = g_SelectionPlayerRuntimeBlockPointers[playerBlock->playerRuntimeId];
              stepTicks = selectionBlock->simulationStepTicks;
              if ((selectionBlock->sessionFlags & PLAYER_SESSION_FLAG_PAUSE_REQUESTED) != 0) {
                /* "  P" */
                rosterCursor[0] = L' ';
                rosterCursor[1] = L' ';
                rosterCursor[2] = L'P';
                rosterCursor[3] = 0;
                rosterCursor += 3;
              }
              if (1 < stepTicks) {
                /* "  x<n>": the characters 'x' and '0' + stepTicks as one dword store */
                rosterCursor[0] = L' ';
                rosterCursor[1] = L' ';
                *(uint32_t *)(rosterCursor + 2) = stepTicks * 65536 + (L'0' << 16 | L'x');
                rosterCursor += 4;
              }
              if ((selectionBlock->sessionFlags & PLAYER_SESSION_FLAG_SLOW_RENDERING) != 0) {
                /* "  W" in rich-text save colour / palette colour 3 ... restore colour */
                rosterCursor[0] = L' ';
                rosterCursor[1] = L' ';
                rosterCursor[2] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_SAVE_COLOR;
                rosterCursor[3] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_3;
                rosterCursor[4] = L'W';
                rosterCursor[5] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_RESTORE_COLOR;
                rosterCursor += 6;
              }
            }
          }
          playerBlock++;
          remainingPlayers--;
        } while (remainingPlayers != 0);
        *rosterCursor = 0;
      }
      if (rosterCount == 0) {
        /* Local session or no player on this faction: status template without a roster. */
        resolvedText = TextResource_Resolve(TEXT_ID_FACTION_NO_ROSTER);
        rosterCursor = resolvedText;
      }
      else {
        resolvedText = TextResource_Resolve(TEXT_ID_FACTION_ROSTER_TEMPLATE);
        rosterCursor = resolvedText;
        RichTextCommandStream_PatchPayloadBySelector(0,g_InGamePlayerListTextScratchUtf16,rosterCursor);
      }
      /* faction name, roster, and economyProgressScore + relationScore */
      resolvedText = TextResource_Resolve(((GameFactionRuntimeRecord *)factionRecordAddress)->colorIndex +
                                          TEXT_ID_FACTION_NAME_BASE);
      statusTemplate = TextResource_Resolve(TEXT_ID_FACTION_STATUS_TEMPLATE);
      stream = statusTemplate;
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,rosterCursor,stream);
      RichTextCommandStream_PatchPayloadBySelector(2,(void *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0),stream);
      if (RichTextCommandStream_CopyExpanded(1024,destination,stream,&copiedByteCount)) {
        destination = (uint16_t *)((int)destination + copiedByteCount);
      }
    }
    frameOrFactionIndex++;
    factionRecordAddress = factionRecordAddress + sizeof(GameFactionRuntimeRecord);
  } while (frameOrFactionIndex <= 7);
}


/* Address: 0x005640E0.
   Network games only: resizes the in-game player status box to one text line per player and formats each
   line into g_InGamePlayerStatusTextSlots (text 0xFF05 or 0xFF06 depending on the player's ready/wait state,
   with the player's name patched in). Runs under the in-game tick spin lock because the network code updates
   the player records.
*/
void InGamePanel_RebuildPlayerStatusRows(void *inGameRoot)

{
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int panelHalfHeight;
  InGamePlayerStatusTextSlot *destination;
  FrontendPlayerNameUtf16 *playerName;
  RichTextExtent textExtent;
  uint16_t *resolvedText;
  GraphicsTextureLogicalSize windowTextureSize;
  UiConditionalActionControl *statusBox;

  g_SpinLockAcquire(&g_InGameStateTickSpinLock);
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    windowTextureSize = g_GraphicsTextureSourceGetLogicalSize(114,g_UiWindowTextureSource);
    /* the loop walks the player records by their name field (the pointer the original keeps in EDI) */
    playerName = &playerRecord->playerName;
    textExtent = RichTextCommandStream_MeasureLine
                      (g_UiTextStyleNormal,(uint16_t *)u_gfx_panel_panel0_gfx_005630d0);
    panelHalfHeight = (textExtent.heightPixels * remainingPlayers >> 1) + windowTextureSize.logicalHeightPixels;
    destination = g_InGamePlayerStatusTextSlots;
    statusBox = (UiConditionalActionControl *)INGAME_UI(inGameRoot,playerStatusBox);
    statusBox->lineCount = remainingPlayers;
    (statusBox->base).bottomOffset = panelHalfHeight;
    (statusBox->base).topOffset = -panelHalfHeight;
    UiContainer_LayoutChildren((statusBox->base).parent);
    do {
      playerRecord = (FrontendPlayerRuntimeRecord *)
           ((uint8_t *)playerName - offsetof(FrontendPlayerRuntimeRecord,playerName));
      if ((playerRecord->factionAssignment).readyOrWaitState == 0) {
        resourceId = TEXT_ID_PLAYER_STATUS_STATE_ZERO;
      }
      else {
        resourceId = TEXT_ID_PLAYER_STATUS_STATE_SET;
      }
      resolvedText = TextResource_Resolve(resourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,playerName,resolvedText);
      RichTextCommandStream_CopyExpanded(128,destination->text,resolvedText,NULL);
      destination++;
      playerName = (FrontendPlayerNameUtf16 *)((uint8_t *)playerName + sizeof(FrontendPlayerRuntimeRecord));
      remainingPlayers--;
    } while (remainingPlayers != 0);
  }
  g_SpinLockRelease(&g_InGameStateTickSpinLock);
  return;
}


/* Continuation addresses stored in g_InGameCommandDispatchRecords_00_Code00030073_Modifier33 (entry points
   inside the original function; the rewritten dispatcher below switches on them). */
enum InGameKeyCommandContinuation {
  INGAME_KEY_RECALL_GROUP = 0x567cc0,                /* 1..8 */
  INGAME_KEY_ADD_GROUP_TO_SELECTION = 0x567d10,      /* Shift+1..8 */
  INGAME_KEY_STORE_SELECTION_AS_GROUP = 0x567d60,    /* Ctrl+1..8, Alt+1..8 */
  INGAME_KEY_ADD_SELECTION_TO_GROUP = 0x567db0,      /* Ctrl+Shift+1..8, Alt+Shift+1..8 */
  INGAME_KEY_NOTIFICATION_ADVANCE = 0x567e00,        /* Space */
  INGAME_KEY_NOTIFICATION_CANCEL = 0x567e20,         /* Backspace */
  INGAME_KEY_CAMERA_TO_NOTIFICATION = 0x567e40,      /* Alt+Space */
  INGAME_KEY_CAMERA_TO_SELECTION = 0x567ea0,         /* Numpad 5 */
  INGAME_KEY_CAMERA_TO_CLASS11_MODEL = 0x567ed0,     /* B */
  INGAME_KEY_SELECTION_RESET_MOVEMENT = 0x567f60,    /* S, Shift+Alt+S */
  INGAME_KEY_SELECTION_STOP_MOVEMENT = 0x567fc0,     /* Shift+S */
  INGAME_KEY_SELECTION_CANCEL_TARGETS = 0x568020,    /* Alt+S */
  INGAME_KEY_UPGRADE_PAGE_TOGGLE = 0x568080,         /* F */
  INGAME_KEY_SELECT_OWNED_CLASS16 = 0x5680f0,        /* A */
  INGAME_KEY_SELECTION_SELF_DESTRUCT = 0x568130,     /* Alt+D */
  INGAME_KEY_FREE_CAMERA_TOGGLE = 0x568190,          /* Alt+C */
  INGAME_KEY_WRAPPED_STATUS_TEXT_TOGGLE = 0x5681a0,  /* O */
  INGAME_KEY_CHEAT_OCCUPANCY_TOGGLE = 0x5681b0       /* Ctrl+Alt+V */
};

/* Address: 0x005678C0.
   In-game key commands (the world view's dispatchCommandCallback): the first record of
   g_InGameCommandDispatchRecords_00_Code00030073_Modifier33 whose key code matches and whose modifier class
   (Shift / Ctrl / Alt, left or right) equals the held modifiers selects the command. Selection commands are
   ignored while the game is paused or the world input is disabled; network games queue them as player
   commands (code in brackets) instead of executing them.
     1..8                   recall selection group n (transfer mode 0)                 [0xBE0]
     Shift+1..8             add group n to the selection (mode 2)                      [0xBE0]
     Ctrl+1..8, Alt+1..8    store the selection as group n (mode 1)                    [0xBE0]
     Ctrl/Alt+Shift+1..8    add the selection to group n (mode 3)                      [0xBE0]
     S, Shift+Alt+S         stop: reset the selection's movement                       [0xE10]
     Shift+S                reset the selection's movement anchors                     [0xE30]
     Alt+S                  interrupt the selection's active targets                   [0xE50]
     Alt+D                  apply model hierarchy flags 0x418 to the selection         [0xE70]
     A                      InGameSelection_SelectAllOwnAircraftPads               [0x8F0]
     B                      camera to the first own model of runtime class 11
     Space / Backspace      advance/resolve or cancel the notification target (as the notification button)
     Alt+Space              camera to the last notification target position
     Numpad 5               camera to the centre of the selection
     F                      toggle the single-selection upgrade page (as its button)
     O                      show/hide the wrapped world-view status text
     Alt+C                  toggle the free camera (no pitch and distance clamps)
     Ctrl+Alt+V             cheat, single player only: toggle occupancy bit 0 on every field cell
   The original also returns CF set when no record matches; this void callback does not reproduce it.
*/
void InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world)

{
  /* Rewritten from the assembly (0x005678C0-0x00568204). The decompiled version jumped to the
     continuation labels inside the original machine code. EBX is the world view (worldRuntime). */
  UiCommandDispatchRecord *record = g_InGameCommandDispatchRecords_00_Code00030073_Modifier33;
  uint32_t target = 0;
  bool localSession =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL;
  bool commandsBlocked =
       (g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) != 0;

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & KEYBOARD_STATE_ANY_MODIFIER) != 0) continue;
    }
    else {
      if ((flags & KEYBOARD_STATE_SHIFT) != 0) {
        if ((modifierFlags & KEYBOARD_STATE_SHIFT) == 0) continue;
      }
      else if ((modifierFlags & KEYBOARD_STATE_SHIFT) != 0) {
        continue;
      }
      if ((flags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0) {
        if ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) continue;
      }
      else if ((flags & KEYBOARD_STATE_ALT) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
      }
      else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
      else {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case INGAME_KEY_RECALL_GROUP:
  case INGAME_KEY_ADD_GROUP_TO_SELECTION:
  case INGAME_KEY_STORE_SELECTION_AS_GROUP:
  case INGAME_KEY_ADD_SELECTION_TO_GROUP: {
    /* FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh mode bits: 1 = the selection is the
       source and the group the destination, 2 = merge instead of replace */
    uint32_t transferMode = (target == INGAME_KEY_RECALL_GROUP) ? 0 :
                            (target == INGAME_KEY_ADD_GROUP_TO_SELECTION) ? 2 :
                            (target == INGAME_KEY_STORE_SELECTION_AS_GROUP) ? 1 : 3;
    uint32_t groupIndex = commandCode - KEYBOARD_KEY_CODE_CHAR('1');
    if (commandsBlocked) {
      break;
    }
    if (localSession) {
      FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
                (g_LocalPlayerRuntimeId,world->activeFactionRuntimeIndex,transferMode,groupIndex);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_GROUP,world->activeFactionRuntimeIndex,transferMode,groupIndex);
    }
    break;
  }
  case INGAME_KEY_NOTIFICATION_ADVANCE:
    InGameTargetingContext_AdvanceOrResolveTarget
              ((InGameTargetingRootTraversalView *)
               THANDOR_UI_SIBLING(world,InGameUiImage,worldView,notificationTargetButton));
    break;
  case INGAME_KEY_NOTIFICATION_CANCEL:
    InGameTargetingContext_CancelAndRestoreState
              ((InGameTargetingRootTraversalView *)
               THANDOR_UI_SIBLING(world,InGameUiImage,worldView,notificationTargetButton));
    break;
  case INGAME_KEY_CAMERA_TO_NOTIFICATION: {
    /* the in-game root that holds this world view */
    InGameRuntimeRoot *root = (InGameRuntimeRoot *)
         ((uint8_t *)world - offsetof(InGameRuntimeRoot,worldRuntime));
    FixedVectorQ12 point;
    if ((root->targetingWorldXQ12 == 0) ||
        (root->targetingWorldYQ12 == 0)) {
      break;
    }
    FieldGrid_GetNearestTerrainPoint(root->targetingWorldYQ12,
                                     root->targetingWorldXQ12,world->fieldGrid,&point);
    WorldRuntime_PointCameraAtTarget
              ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).targetDistanceQ12,
               point.zQ12,root->targetingWorldYQ12,
               root->targetingWorldXQ12,world);
    break;
  }
  case INGAME_KEY_CAMERA_TO_SELECTION: {
    FixedVectorQ12 center;
    if (!SelectionInfoEntitySlots_ComputeAverageWorldPosition(&center)) {
      break;
    }
    WorldRuntime_PointCameraAtTarget
              ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
               center.zQ12,center.yQ12,center.xQ12,world);
    break;
  }
  case INGAME_KEY_CAMERA_TO_CLASS11_MODEL: {
    /* the first model on the owner list that belongs to the active faction and whose definition has
       runtime class 11 */
    WorldOwnerListNode *ownerNode = world->ownerListHead;
    uint32_t faction = world->activeFactionRuntimeIndex;
    for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      uint8_t *modelRuntime;
      if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      modelRuntime = (uint8_t *)ownerNode->runtimePayload;
      if ((faction == (uint32_t)((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
          (((ModelRuntimeSlot *)modelRuntime)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11)) {
        WorldRuntime_PointCameraAtTarget
                  ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
                   ownerNode->worldZQ12,ownerNode->worldYQ12,ownerNode->worldXQ12,world);
        break;
      }
    }
    break;
  }
  case INGAME_KEY_SELECTION_RESET_MOVEMENT:
  case INGAME_KEY_SELECTION_STOP_MOVEMENT:
  case INGAME_KEY_SELECTION_CANCEL_TARGETS:
  case INGAME_KEY_SELECTION_SELF_DESTRUCT: {
    static const uint32_t queuedCommandCodes[4] = {INGAME_COMMAND_SELECTION_RESET_MOVEMENT,INGAME_COMMAND_SELECTION_STOP_MOVEMENT,
                                                   INGAME_COMMAND_SELECTION_CANCEL_TARGETS,INGAME_COMMAND_SELECTION_SELF_DESTRUCT};
    int commandIndex = (target == INGAME_KEY_SELECTION_RESET_MOVEMENT) ? 0 :
                       (target == INGAME_KEY_SELECTION_STOP_MOVEMENT) ? 1 :
                       (target == INGAME_KEY_SELECTION_CANCEL_TARGETS) ? 2 : 3;
    if (commandsBlocked || SelectionInfo_AllEntriesEmptyOrMatchOwner(world->activeFactionRuntimeIndex)) {
      break;
    }
    if (!localSession) {
      InGameCommandQueue_AppendLocalPlayerCommand(queuedCommandCodes[commandIndex],0,0,0);
    }
    else if (commandIndex == 0) {
      PlayerSelection_ResetMovementPruneAndRecenterEntries(g_LocalPlayerRuntimeId,0,0,0);
    }
    else if (commandIndex == 1) {
      PlayerSelection_StopMovement(g_LocalPlayerRuntimeId,0,0,0);
    }
    else if (commandIndex == 2) {
      PlayerSelection_CancelTargets(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      PlayerSelection_SelfDestruct(g_LocalPlayerRuntimeId,0,0,0);
    }
    break;
  }
  case INGAME_KEY_UPGRADE_PAGE_TOGGLE: {
    UiSpriteButtonControl *upgradeButton = (UiSpriteButtonControl *)
         THANDOR_UI_SIBLING(world,InGameUiImage,worldView,singleSelectionUpgradeButton);
    if (commandsBlocked || (((upgradeButton->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
      break;
    }
    /* only while the single-selection page is shown */
    if (UiPageStack_ActivePageIndex
              ((UiPageStackControl *)THANDOR_UI_SIBLING(world,InGameUiImage,worldView,selectionDetailPageStack))
        != 1) {
      break;
    }
        if ((((upgradeButton->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) && (upgradeButton->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,upgradeButton->activationSound,NULL);
    }
    InGameTechnologyPanel_ToggleForSelection((UiNodeBase *)world);
    break;
  }
  case INGAME_KEY_SELECT_OWNED_CLASS16:
    if (commandsBlocked) {
      break;
    }
    if (localSession) {
      InGameSelection_SelectAllOwnAircraftPads(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_OWN_AIRCRAFT_PADS,0,0,0);
    }
    break;
  case INGAME_KEY_FREE_CAMERA_TOGGLE:
    /* no pitch and distance clamps in the world motion code */
    world->runtimeFlags = world->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
    break;
  case INGAME_KEY_WRAPPED_STATUS_TEXT_TOGGLE:
    THANDOR_UI_SIBLING(world,InGameUiImage,worldView,worldViewWrappedStatusText)->nodeFlags =
         THANDOR_UI_SIBLING(world,InGameUiImage,worldView,worldViewWrappedStatusText)->nodeFlags ^
         UI_NODE_SUPPRESSED;
    break;
  case INGAME_KEY_CHEAT_OCCUPANCY_TOGGLE:
    if (!localSession || ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) == 0)) {
      break;
    }
    /* the flag an ended local faction gets, whose simulation step also sets occupancy bit 0 on every cell */
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    else {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    FieldGrid_ClassifyCellFlagsToRuntimeByte(world->activeFactionRuntimeIndex,world->fieldGrid);
    break;
  default:
    Thandor_Log("InGameUi dispatch: unhandled continuation %08x",target);
    break;
  }
  return;
}


/* Address: 0x00569750.
   The world view's fieldRegion.clearTransientStateCallback: resets the notification target button's cursor
   frame to 0 when it still shows frame 0x1B.
*/
void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView)

{
  UiImageActionControl *notificationButton;

  notificationButton = (UiImageActionControl *)
       THANDOR_UI_SIBLING(worldView,InGameUiImage,worldView,notificationTargetButton);
  if (notificationButton->cursorFrame == INGAME_NOTIFICATION_CURSOR_CANCEL) {
    notificationButton->cursorFrame = 0;
  }
  return;
}

/* Address: 0x00569780.
   The world view's dispatchWorldContextActionCallback, unless the game is paused or the world input is
   blocked: a running camera move (runtimeFlags 0x10) is aborted and the saved camera restored; otherwise a
   pending unit placement is dropped (GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid, network command
   0x14F0), or else the selection is cleared (network command 0xBA0). Both outcomes of the
   SelectionInfo_TestNotOwnAircraftPadsWithAircraft test clear the selection, as in the original.
*/
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world)

{
  bool hasActiveOwnerType16;

  /* the original tests WORLD_INPUT_DISABLED twice */
  if ((((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) &&
      (((world->interaction).nodeFlags & 8) == 0)) &&
     ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) {
    if ((world->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
        hasActiveOwnerType16 = SelectionInfo_TestNotOwnAircraftPadsWithAircraft(world->activeFactionRuntimeIndex);
        if (hasActiveOwnerType16) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
          }
          return;
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
        }
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
               SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,world->activeFactionRuntimeIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CONSUME_PENDING_ARMY,0,0,world->activeFactionRuntimeIndex);
      }
    }
    else {
      world->runtimeFlags = world->runtimeFlags & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
      WorldRuntime_RestoreMotionStateFromSnapshot(world);
    }
  }
  return;
}


/* Address: 0x00569890.
   Queues an in-game notification (movie id, priority and position/orientation payload) in the four-slot
   queue g_InGameRuntimeRoot->notificationQueue, which is kept sorted by descending priority: every slot
   of lower priority is swapped (XCHG) with the carried record, so lower entries move down one slot and the
   lowest falls out. A notification with movie id 0 is ignored.
*/
void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved,
          uint32_t orientationValue,AngleTurn32 orientationAngle,
          Q12 secondaryWorldCoordinateQ12,Q12 primaryWorldCoordinateQ12,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId)

{
  InGameNotificationPayloadKind carriedPayloadKind;
  uint32_t carriedReserved10;
  uint32_t carriedOrientationValue;
  AngleTurn32 carriedOrientationAngle;
  Q12 carriedSecondaryCoordinate;
  Q12 carriedPrimaryCoordinate;
  InGameNotificationMovieId displacedMovieId;
  uint32_t displacedPriority;
  InGameNotificationMovieId remainingSlots;
  InGameNotificationQueueRecord *queueSlot;
  
  queueSlot = g_InGameRuntimeRoot->notificationQueue;
  remainingSlots = INGAME_NOTIFICATION_QUEUE_SLOTS;
  carriedPayloadKind = payloadKind;
  carriedReserved10 = payloadReserved;
  carriedOrientationValue = orientationValue;
  carriedOrientationAngle = orientationAngle;
  carriedSecondaryCoordinate = secondaryWorldCoordinateQ12;
  carriedPrimaryCoordinate = primaryWorldCoordinateQ12;
  displacedMovieId = notificationMovieId;
  /* displacedMovieId doubles as the loop condition: the movie id on entry, then the remaining slot count */
  while (displacedMovieId != 0) {
    displacedMovieId = notificationMovieId;
    displacedPriority = priority;
    payloadKind = carriedPayloadKind;
    payloadReserved = carriedReserved10;
    orientationValue = carriedOrientationValue;
    orientationAngle = carriedOrientationAngle;
    secondaryWorldCoordinateQ12 = carriedSecondaryCoordinate;
    primaryWorldCoordinateQ12 = carriedPrimaryCoordinate;
    if (queueSlot->priority < priority) {
      /* XCHG per dword: InGameRuntime_ProcessQueuedSessionNotificationTimer pops the queue on the timer thread */
      displacedMovieId = THANDOR_ATOMIC_EXCHANGE(&queueSlot->movieId,notificationMovieId);
      displacedPriority = THANDOR_ATOMIC_EXCHANGE(&queueSlot->priority,priority);
      primaryWorldCoordinateQ12 =
           (Q12)THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).worldXQ12,carriedPrimaryCoordinate);
      secondaryWorldCoordinateQ12 =
           (Q12)THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).worldYQ12,carriedSecondaryCoordinate);
      orientationAngle =
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).headingAngle,carriedOrientationAngle);
      orientationValue =
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).orientationOrPresentationValue,carriedOrientationValue);
      payloadReserved = THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).reserved10,carriedReserved10);
      payloadKind = (InGameNotificationPayloadKind)
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).payloadKind,carriedPayloadKind);
    }
    queueSlot++;
    remainingSlots--;
    carriedPayloadKind = payloadKind;
    carriedReserved10 = payloadReserved;
    carriedOrientationValue = orientationValue;
    carriedOrientationAngle = orientationAngle;
    carriedSecondaryCoordinate = secondaryWorldCoordinateQ12;
    carriedPrimaryCoordinate = primaryWorldCoordinateQ12;
    priority = displacedPriority;
    notificationMovieId = displacedMovieId;
    displacedMovieId = remainingSlots;
  }
  return;
}


/* Address: 0x00569B00.
   Rebuilds the diplomacy panel: one row (at most seven) per other active faction with its faction name,
   player number, relation state text and icon, and the name of the network player who controls it. The
   frame is sized for the row count (smaller offsets below 800 pixels width); the panel stays hidden with no
   other faction, while the world input is disabled, or when relationUiFlags bit 4 is set, and unused rows
   are switched to their empty page. g_UiAction1012TargetPlayerIndices keeps the faction of each row for the
   row buttons (action 0x1012).
*/
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node)

{
  uint32_t *controlFlags;
  SessionNetworkRoleFlags remainingNetworkPlayers;
  UiControlCount nextRemainingCount;
  int factionIndexOrRecord;
  uint32_t candidateFactionIndex;
  uint32_t remainingFactions;
  int controlOffset;
  uint32_t relationState;
  SessionNetworkRoleFlags remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t slotIndex;
  UiGridDimensions gridDimensions;
  UiControlCount otherActiveCount;

  /* node becomes the in-game UI root (parent -1) */
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  /* factionIndexOrRecord: faction index here, then the extra frame width, then the faction record address */
  factionIndexOrRecord = 1;
  otherActiveCount = 0;
  remainingFactions = g_GameFactionRuntimeImage.tail.activeFactionCount;
  do {
    if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexOrRecord] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
         (factionIndexOrRecord != ((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex)) &&
       ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) {
      otherActiveCount++;
    }
    factionIndexOrRecord++;
    remainingFactions--;
  } while (remainingFactions != 0);
  gridDimensions = UiGrid_OneColumnDimensionsPacked(otherActiveCount);
  factionIndexOrRecord = (int)gridDimensions.columnCount * g_InGamePanelTextureSubresource32Width +
        g_InGamePanelTextureSubresource19Width + g_InGamePanelTextureSubresource20Width;
  controlOffset = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource32Height +
        g_InGamePanelTextureSubresource18Height + g_InGamePanelTextureSubresource23Height;
  if ((int)g_FramebufferWidth < 800) {
    INGAME_UI(node,diplomacyFrame)->leftOffset = -31;
    INGAME_UI(node,diplomacyFrame)->rightOffset = -31;
    INGAME_UI(node,diplomacyFrame)->topOffset = -100;
    INGAME_UI(node,diplomacyFrame)->bottomOffset = -100;
  }
  else {
    INGAME_UI(node,diplomacyFrame)->leftOffset = -39;
    INGAME_UI(node,diplomacyFrame)->rightOffset = -39;
    INGAME_UI(node,diplomacyFrame)->topOffset = -126;
    INGAME_UI(node,diplomacyFrame)->bottomOffset = -126;
  }
  INGAME_UI(node,diplomacyFrame)->leftOffset = INGAME_UI(node,diplomacyFrame)->leftOffset - factionIndexOrRecord;
  INGAME_UI(node,diplomacyFrame)->topOffset = INGAME_UI(node,diplomacyFrame)->topOffset - controlOffset;
  INGAME_UI(node,diplomacyPanel)->nodeFlags = INGAME_UI(node,diplomacyPanel)->nodeFlags | UI_NODE_SUPPRESSED;
  if ((otherActiveCount != 0) && ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) {
    INGAME_UI(node,diplomacyPanel)->nodeFlags = INGAME_UI(node,diplomacyPanel)->nodeFlags & ~UI_NODE_SUPPRESSED;
  }
  (*INGAME_UI(node,diplomacyPanel)->vtable->layout)(INGAME_UI(node,diplomacyPanel));
  slotIndex = 0;
  if (otherActiveCount != 0) {
    candidateFactionIndex = 1;
    factionIndexOrRecord = THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord)); /* records[1] */
    do {
      nextRemainingCount = otherActiveCount + 1;
      if ((candidateFactionIndex != ((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex) &&
         (g_GameFactionRuntimeImage.tail.factionLifecycleStates[candidateFactionIndex] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE)) {
        g_UiAction1012TargetPlayerIndices[slotIndex] = candidateFactionIndex;
        UiPageStack_SetActiveIndex
                  (0,(UiPageStackControl *)
                     THANDOR_UI_AT(node,g_UiAction1012SlotPageOffsets[slotIndex]));
        /* the text fields hold text resource ids; the colour name of the faction's colorIndex */
        ((UiSingleLineTextControl *)((int)node + g_UiAction1012PlayerLabelTextOffsets[slotIndex]))->text =
             (uint16_t *)(((GameFactionRuntimeRecord *)factionIndexOrRecord)->colorIndex + TEXT_ID_FACTION_NAME_BASE);
        ((UiSingleLineTextControl *)((int)node + g_UiAction1012PlayerIndexTextOffsets[slotIndex]))->text =
             (uint16_t *)(candidateFactionIndex + TEXT_ID_PLAYER_NUMBER_BASE);
        relationState = g_GameFactionRuntimeImage.records
                        [((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex]
                        .packedRelationStates >> ((uint8_t)(candidateFactionIndex << 2) & SHIFT_COUNT_MASK) &
                        FACTION_RELATION_STATE_MASK;
        /* the SHL/SHR pair around the nibble shift, rendered as a mask: the index itself is unchanged */
        candidateFactionIndex = candidateFactionIndex & 0x3fffffff;
        ((UiSingleLineTextControl *)((int)node + g_UiAction1012StateTextOffsets[slotIndex]))->text =
             (uint16_t *)(relationState + TEXT_ID_DIPLOMATIC_RELATION_BASE);
        /* player name: empty, or in network games the name of the player assigned to this faction */
        controlOffset = g_UiAction1012IconImageOffsets[slotIndex];
        ((UiSingleLineTextControl *)((int)node + controlOffset))->text = (uint16_t *)&g_EmptyFrontendPlayerNameUtf16;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        /* Ghidra merged the network test and the player counter into remainingNetworkPlayers: nonzero role
           bits enter the loop, which then runs over the g_FrontendPlayerRuntimeBlockCount records */
        remainingNetworkPlayers = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (remainingNetworkPlayers != SESSION_NETWORK_ROLE_LOCAL) {
          if ((playerBlock->factionAssignment).factionAssignmentIndex == candidateFactionIndex) {
            ((UiSingleLineTextControl *)((int)node + controlOffset))->text = (uint16_t *)&playerBlock->playerName;
            break;
          }
          playerBlock++;
          remainingPlayerBlocks--;
          remainingNetworkPlayers = remainingPlayerBlocks;
        }
        /* the row's relation icon button: shown, with the sprite of the relation state; hidden again by the
           relationUiFlags rules */
        controlOffset = g_UiAction1012ControlOffsets[slotIndex];
        remainingFactions = g_UiAction1012SubresourceByState[relationState];
        slotIndex++;
        controlFlags = (uint32_t *)&THANDOR_UI_AT(node,controlOffset)->nodeFlags;
        *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
        ((UiCommandSpriteButtonControl *)((int)node + controlOffset))->sprite.normalSubresourceStartOrDescriptor =
             remainingFactions;
        nextRemainingCount = otherActiveCount;
        if (((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) != 0) &&
           ((7 < relationState ||
            (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) != 0 &&
             ((3 < relationState || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) != 0)))))))) {
          controlFlags = (uint32_t *)&THANDOR_UI_AT(node,controlOffset)->nodeFlags;
          *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
        }
      }
      otherActiveCount = nextRemainingCount;
      candidateFactionIndex++;
      factionIndexOrRecord = factionIndexOrRecord + sizeof(GameFactionRuntimeRecord);
      otherActiveCount--;
    } while (otherActiveCount != 0);
    if (6 < slotIndex) {
      return;
    }
  }
  do {
    UiPageStack_SetActiveIndex
              (1,(UiPageStackControl *)
                 THANDOR_UI_AT(node,g_UiAction1012SlotPageOffsets[slotIndex]));
    slotIndex++;
  } while (slotIndex < 7);
  return;
}


/* Address: 0x0056A460.
   Scores how well one of the level's music tracks (by sample number) fits the situation of the active faction:
   sums three army definition values over the faction's armies (+0x78 weighted 3 for armies with flag bit 0 at
   +0x2C) plus 50 per army with definition flag 0x10, and weights them by the track's number band (below 20, 50,
   70, or above). Track number 0 scores 0. The in-game music picks the best of the level's four tracks.
*/
uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime)

{
  int activeFactionIndex;
  uint32_t suitabilityScore;
  ArmyAssetRecord *armyDefinition;
  int modelRuntimeOrBonus;
  int registryWeight;
  ArmyAssetRecordPrefix *foundArmyAsset;
  int flag10BonusSum;
  int class74Sum;
  int weightedClass78Sum;
  int class70Sum;
  WorldOwnerListNode *ownerListNode;
  
  suitabilityScore = 0;
  class70Sum = 0;
  weightedClass78Sum = 0;
  class74Sum = 0;
  flag10BonusSum = 0;
  if (trackClassId != 0) {
    activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
    for (ownerListNode = worldRuntime->ownerListHead; ownerListNode != NULL;
        ownerListNode = ownerListNode->nextNode) {
      /* modelRuntimeOrBonus: the owner army runtime (ArmyRuntimeSlot), later the flag bonus */
      if ((ownerListNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelRuntimeOrBonus = (int)((ModelRuntimeSlot *)ownerListNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime, activeFactionIndex == ((ArmyRuntimeSlot *)modelRuntimeOrBonus)->factionIndex)) {
        if (ArmyAssetRegistry_FindById(((ArmyRuntimeSlot *)modelRuntimeOrBonus)->armyAssetId,&foundArmyAsset) == 0) {
          armyDefinition = (ArmyAssetRecord *)foundArmyAsset;
          registryWeight = 1;
          if ((((ArmyRuntimeSlot *)modelRuntimeOrBonus)->commandModeFlags & 1) != 0) {
            registryWeight = 3;
          }
          class70Sum = class70Sum + armyDefinition->definitionClassValue70;
          weightedClass78Sum = weightedClass78Sum + registryWeight * armyDefinition->definitionClassValue78;
          modelRuntimeOrBonus = 50;
          if ((armyDefinition->flags & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) == 0) {
            modelRuntimeOrBonus = 0;
          }
          class74Sum = class74Sum + armyDefinition->definitionClassValue74;
          flag10BonusSum = flag10BonusSum + modelRuntimeOrBonus;
        }
      }
    }
    /* weights are Q8 (256 = 1) */
    if (trackClassId < 20) {
      suitabilityScore = flag10BonusSum * 128 + class74Sum * 256 + weightedClass78Sum * 640 + class70Sum * 256;
    }
    else if (trackClassId < 50) {
      suitabilityScore = flag10BonusSum * -256 + class74Sum * 64 + 204800 + weightedClass78Sum * 16 + class70Sum * 128;
    }
    else if (trackClassId < 70) {
      suitabilityScore = flag10BonusSum * 128 + class74Sum * 256 + weightedClass78Sum * 32 + class70Sum * 768;
    }
    else {
      suitabilityScore = flag10BonusSum * 256 + class74Sum * 512 + weightedClass78Sum * 384 + class70Sum * 16;
    }
  }
  return suitabilityScore;
}


/* Address: 0x0056A8A0.
   UI action 0x101F (mission help toggle button): opening shows the mission help window (page 8) with the
   active faction's help text for this level, re-measures its three text panels and blocks the world input; a local
   game is paused meanwhile. Closing hides the window, re-enables the world input and resumes the game unless it
   was already paused before the window opened.
*/

void InGameMissionHelpPage_Toggle(UiNodeBase *source)

{
  WorldInteractionFlags *interactionFlagsField;
  bool isSelected;
  RichTextExtent wrappedExtent;
  uint16_t *resolvedText;
  InGameMissionHelpRootView *uiRoot;

  uiRoot = (InGameMissionHelpRootView *)source;
  while ((uiRoot->rootUi).base.parent != UI_NODE_NONE) {
    uiRoot = (InGameMissionHelpRootView *)(uiRoot->rootUi).base.parent;
  }
  isSelected = (bool)UiSelectableControl_IsSelected((UiSelectableControl *)source);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,&uiRoot->gameWindowPageStack);
    /* UI_NODE_SUPPRESSED on the world view: a window blocks the world input */
    interactionFlagsField = &(uiRoot->worldRuntime).interaction.nodeFlags;
    *interactionFlagsField = *interactionFlagsField & ~UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW) == 0) {
        g_UiCommandRuntimeFlags =
             g_UiCommandRuntimeFlags & ~(UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
      }
      else {
        g_UiCommandRuntimeFlags =
             g_UiCommandRuntimeFlags &
             ~(UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW);
      }
    }
    return;
  }
  UiSelectableControl_SetSelected(0,&uiRoot->inGameMenuButton);
  interactionFlagsField = &(uiRoot->worldRuntime).interaction.nodeFlags;
  *interactionFlagsField = *interactionFlagsField | UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&uiRoot->worldRuntime);
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_MISSION_HELP,&uiRoot->gameWindowPageStack);
  (uiRoot->missionBriefingPanel).textResourceId =
       (uiRoot->worldRuntime).activeFactionRuntimeIndex + TEXT_ID_MISSION_HELP_BASE +
       ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).header.
       titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE;
  resolvedText = TextResource_Resolve((uiRoot->missionBriefingPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->missionBriefingPanel).wrapWidth);
  (uiRoot->missionBriefingPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->missionBriefingPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->missionBriefingPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->missionBriefingPanel).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->keyboardHelpPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->keyboardHelpPanel).wrapWidth);
  (uiRoot->keyboardHelpPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->keyboardHelpPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->keyboardHelpPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->keyboardHelpPanel).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->mouseHelpPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->mouseHelpPanel).wrapWidth);
  (uiRoot->mouseHelpPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->mouseHelpPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->mouseHelpPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->mouseHelpPanel).scrollable);
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
       SESSION_NETWORK_ROLE_LOCAL) && ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE) == 0)) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW;
    }
    g_UiCommandRuntimeFlags =
         g_UiCommandRuntimeFlags | (UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
  }
  return;
}


/* Address: 0x0056AD00.
   UI action 0x101C (g_InGameUiActionHandlersPage10[28]): one of the three chart tabs of the results screen
   (resultsTabThird / Economy / Military) was clicked. Selects it exclusively and shows the chart page of the
   selected tab.
*/
void InGameResultsScreen_SelectChartTab(UiSelectableControl *selectableControl)

{
  uint32_t selectedTabIndex;
  int parentNodeAddress;
  void *rootNodeCursor;
  
  /* climb to the UI root (parent -1) */
  parentNodeAddress = (int)(selectableControl->base).parent;
  rootNodeCursor = selectableControl;
  while (parentNodeAddress != -1) {
    rootNodeCursor = (((UiSelectableControl *)rootNodeCursor)->base).parent;
    parentNodeAddress = *(int *)((int)rootNodeCursor + 8); /* ->parent */
  }
  UiSelectableGroup_SelectExclusive(3,&selectableControl->base,
      INGAME_UI(rootNodeCursor,resultsTabThird),
      INGAME_UI(rootNodeCursor,resultsTabEconomy),
      INGAME_UI(rootNodeCursor,resultsTabMilitary));
  /* Original quirk: the result is not tested; with no visible tab selected the index is 3 (no page) */
  UiSelectableGroup_FindVisibleSelected(NULL,&selectedTabIndex,3,
      INGAME_UI(rootNodeCursor,resultsTabThird),
      INGAME_UI(rootNodeCursor,resultsTabEconomy),
      INGAME_UI(rootNodeCursor,resultsTabMilitary));
  UiPageStack_SetActiveIndex
            (selectedTabIndex,
             (UiPageStackControl *)INGAME_UI(rootNodeCursor,resultsChartPageStack));
  return;
}


/* Address: 0x0056AD80.
   UI action 0x1012 (g_InGameUiActionHandlersPage10[18]): one of the seven relation buttons of the diplomacy
   rows. Finds the button's row through g_UiAction1012ControlOffsets and advances the relation of the local
   faction towards that row's faction (g_UiAction1012TargetPlayerIndices), or resets it when the activation
   carries UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK. Ignored while paused or with world input disabled.
*/
void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control)

{
  UiCommandSpriteButtonControl *rootControl;
  CommandPayload rowFactionIndex;
  GraphicsTextureSourceAsset *rootFactionValue;
  int slotIndex;

  if ((g_UiCommandRuntimeFlags &
       (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
    rootControl = control;
    while ((rootControl->sprite).selectable.base.parent != UI_NODE_NONE) {
      rootControl = (UiCommandSpriteButtonControl *)(rootControl->sprite).selectable.base.parent;
    }
    slotIndex = 6;
    while ((int)control - (int)rootControl != g_UiAction1012ControlOffsets[slotIndex]) {
      slotIndex--;
      if (slotIndex < 0) {
        return;
      }
    }
    rowFactionIndex = g_UiAction1012TargetPlayerIndices[slotIndex];
    /* rootControl[21].sprite.primaryTextureSource is root+0xA80, the activeFactionRuntimeIndex of the
       world runtime at root+0xA30: the local faction */
    if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0) {
      rootFactionValue = rootControl[21].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_AdvancePairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,rowFactionIndex,(FactionRuntimeIndex)rootFactionValue);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_ADVANCE_RELATION,0,rowFactionIndex,(CommandPayload)rootFactionValue);
      }
    }
    else {
      rootFactionValue = rootControl[21].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ResetPairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,rowFactionIndex,(FactionRuntimeIndex)rootFactionValue);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_RESET_RELATION,0,rowFactionIndex,(CommandPayload)rootFactionValue);
      }
    }
  }
  return;
}


/* Address: 0x0056B520.
   UI action 0x1010 (also key F): toggles the in-game technology window (page 2 of the window page stack).
   When it opens with a selection, the technology panel is reset to the current area and the first selected
   entity's definition is assigned to the player (command INGAME_COMMAND_ASSIGN_ARMY_TOKEN). Ignored while the
   game is paused or the world input is disabled.
*/
void InGameTechnologyPanel_ToggleForSelection(UiNodeBase *source)

{
  UiPageStackControl *gameWindowStack;
  void *definitionRecord;
  UiPageIndex pageIndex;
  GameEntityRuntime *firstSelectedEntity;
  CommandPayload modelOffset;
  uint32_t activePageIndex;

  while ((((UiRootNode *)source)->base).parent != UI_NODE_NONE) {
    source = (((UiRootNode *)source)->base).parent;
  }
  if ((g_UiCommandRuntimeFlags &
       (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
    INGAME_UI(source,worldView)->nodeFlags = INGAME_UI(source,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
    gameWindowStack = (UiPageStackControl *)INGAME_UI(source,gameWindowPageStack);
    activePageIndex = UiPageStack_ActivePageIndex(gameWindowStack);
    if (activePageIndex == 2) {
      pageIndex = 0;
    }
    else {
      pageIndex = 2;
    }
    UiPageStack_SetActiveIndex(pageIndex,gameWindowStack);
    if ((pageIndex == 2) &&
       (firstSelectedEntity = SelectionInfo_GetFirstEntry(), firstSelectedEntity != NULL)) {
      definitionRecord = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
      InGameTechnologyPanel_ResetAndSelectCurrentArea((UiRootNode *)source);
      /* network-safe form of the pointer: offset from g_ModelRuntimeRebaseDelta */
      modelOffset = (int)definitionRecord - g_ModelRuntimeRebaseDelta;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch
                  (g_LocalPlayerRuntimeId,0,0,modelOffset);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_ASSIGN_ARMY_TOKEN,0,0,modelOffset);
      }
    }
  }
  return;
}


/* Address: 0x0056B5D0.
   UI action 0x1006 (g_InGameUiActionHandlersPage10[6]): the recipient tab InGameUiImage.messageRecipientPlayersTab
   of the message window. Selects the tab, shows the check box page and gives one check box to each faction
   still in the game (factions 1..7),
   labelled with the faction name (label texts 0x216D.. patched with name text 0x2173 + name index); the list
   is sized to the used rows and the unused check boxes are hidden.
*/
void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source)

{
  uint32_t *controlFlags;
  UiNodeBase *uiRootNode;
  int factionNameIndex;
  uint16_t *stream;
  TextResourceId resourceId;
  uint32_t factionIndexCursor;
  int factionRecordAddress;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(uiRootNode,messageRecipientAllTab),
      INGAME_UI(uiRootNode,messageRecipientGroupsTab),
      INGAME_UI(uiRootNode,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(uiRootNode,messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  factionIndexCursor = 1;
  /* a faction is named after its colour: colorIndex of the faction record selects the name text */
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord));
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexCursor] != 0) {
      resolvedText = TextResource_Resolve(resourceId);
      stream = resolvedText;
      factionNameIndex = ((GameFactionRuntimeRecord *)factionRecordAddress)->colorIndex;
      resourceId++;
      /* nodeFlags of the check box: g_UiSevenSlotSelectionControlOffsets is relative to the root */
      controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      filledSlotCount++;
      resolvedText = TextResource_Resolve(factionNameIndex + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText,stream);
    }
    factionIndexCursor++;
    factionRecordAddress = factionRecordAddress + sizeof(GameFactionRuntimeRecord);
  } while (factionIndexCursor < 8);
  INGAME_UI(uiRootNode,messageRecipientList)->bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  return;
}


/* Address: 0x0056B6E0.
   UI action 0x1007 (g_InGameUiActionHandlersPage10[7]): the recipient tab InGameUiImage.messageRecipientGroupsTab
   of the message window. Like action 0x1006, but gives one check box to each of the first seven session
   players (g_FrontendPlayerRuntimeBlocks), labelled with the player name from the player's selection block.
*/
void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  uint32_t *controlFlags;
  UiNodeBase *uiRootNode;
  SelectionPlayerRuntimeBlock *selectionBlock;
  TextResourceId resourceId;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(uiRootNode,messageRecipientAllTab),
      INGAME_UI(uiRootNode,messageRecipientGroupsTab),
      INGAME_UI(uiRootNode,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(uiRootNode,messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  do {
    resolvedText = TextResource_Resolve(resourceId);
    selectionBlock = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[filledSlotCount].playerRuntimeId];
    resourceId++;
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    filledSlotCount++;
    RichTextCommandStream_PatchPayloadBySelector(0,selectionBlock->playerNameUtf16,resolvedText);
    if (6 < filledSlotCount) break;
  } while (filledSlotCount < g_FrontendPlayerRuntimeBlockCount);
  INGAME_UI(uiRootNode,messageRecipientList)->bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  return;
}


/* Address: 0x0056B7E0.
   UI action 0x1008 (g_InGameUiActionHandlersPage10[8]): the recipient tab InGameUiImage.messageRecipientAllTab
   of the message window (send to everyone). Selects the tab and shows page 1 of the recipient page stack,
   which has no check boxes.
*/
void InGameSelectionPage_ShowSubpage1(UiNodeBase *source)

{
  UiNodeBase *rootNodeCursor;

  rootNodeCursor = source;
  while (rootNodeCursor->parent != UI_NODE_NONE) {
    rootNodeCursor = rootNodeCursor->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(rootNodeCursor,messageRecipientAllTab),
      INGAME_UI(rootNodeCursor,messageRecipientGroupsTab),
      INGAME_UI(rootNodeCursor,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rootNodeCursor,messageRecipientPageStack));
  return;
}


/* Address: 0x0056D500.
   UI action 0x100F (g_InGameUiActionHandlersPage10[15], InGameUiImage.messageHistoryPanel): a click on the
   message lines drops the oldest ones so that at most three remain - and always at least one, even when
   three or fewer are shown - then rebuilds the eight-line list.
*/
void InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView)

{
  uint32_t currentEntryCount;
  
  for (currentEntryCount = (historyView->recentTextPointerList).count; 4 < currentEntryCount;
      currentEntryCount = currentEntryCount - 1) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList(8,&historyView->recentTextPointerList);
  return;
}


/* Address: 0x0056F7F0.
   Map editor pointer callback (installed as both selection.resolveContextAction*Callback of the world
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
  uint32_t cursorCodeOrSubMode;
  bool callbackAccepted;
  ArmyRuntimeSlot *previewArmyRuntime;

  /* the world owner-list node under the pointer; only model nodes count */
  if ((ownerNodeUnderPointer != NULL) &&
     (ownerNodeUnderPointer->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) {
    ownerNodeUnderPointer = NULL;
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
      cursorCodeOrSubMode = g_UiCommandModeA;
    }
    else {
      cursorCodeOrSubMode = g_UiCommandModeB;
    }
    if (cursorCodeOrSubMode == 0) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return WORLD_CURSOR_NO_TARGET;
      }
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      cursorCodeOrSubMode = WORLD_CURSOR_MOVE;
      if (localSelectionBlock->placedArmyToken == 0) {
        previewArmyRuntime = ArmyRuntime_CreateInstanceFromAsset
                          (1,0,pointerWorldXQ12,pointerWorldYQ12,g_UiCommandModeGOwnerFactionIndex,
                           g_UiCommandModeGArmyAssetId,worldRuntime,NULL);
        if (previewArmyRuntime != NULL) {
          callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)previewArmyRuntime,worldRuntime);
          if (callbackAccepted) {
            cursorCodeOrSubMode = WORLD_CURSOR_NO_TARGET;
          }
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)previewArmyRuntime);
          return cursorCodeOrSubMode;
        }
        return WORLD_CURSOR_MOVE;
      }
    }
    else {
      if (cursorCodeOrSubMode == 1) {
        if (ownerNodeUnderPointer != NULL) {
          return EDITOR_CURSOR_DELETE_TARGET;
        }
        return EDITOR_CURSOR_DELETE_NONE;
      }
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      cursorCodeOrSubMode = WORLD_CURSOR_OWN_ARMY;
      if (localSelectionBlock->placedArmyToken == 0) {
        if (ownerNodeUnderPointer != NULL) {
          return WORLD_CURSOR_OWN_ARMY;
        }
        return WORLD_CURSOR_FOREIGN_ARMY;
      }
    }
    callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback
                      ((ArmyRuntimeSlot **)
                       (localSelectionBlock->placedArmyToken +
                       (int)g_ArmyRuntimeRebaseBaseMinusOne),worldRuntime);
    if (callbackAccepted) {
      return cursorCodeOrSubMode + 1;
    }
    return cursorCodeOrSubMode;
  case EDITOR_MODE_REGION:
    return EDITOR_CURSOR_REGION;
  }
  return 0;
}


/* Address: 0x0056FA70.
   Map editor pointer press (selection.beginPointerCaptureCallback of the world runtime while the editor is
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
  FieldGridAsset *mapFieldGrid;
  int64_t scaledGridX;
  int64_t scaledGridY;
  PckArmyAssetIdCatalog lookupToken;
  uint32_t placementSubMode;
  CommandPayload commandPayload;
  int cellX;
  uint32_t snappedWorldY;
  uint32_t worldCoordinateTerm;
  int cellY;
  FixedVectorQ12 nearestTerrainPoint;
  
  if ((ownerNodeUnderPointer != NULL) &&
     (ownerNodeUnderPointer->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) {
    ownerNodeUnderPointer = NULL;
  }
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if (g_UiCommandModeC == 0) {
      mapControl->runtimeFlags = mapControl->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        g_UiCommandDragStartScreenX = WORLD_POINTER_NO_HIT;
        return;
      }
      g_UiCommandDragStartScreenX = mapControl->pointerPressX;
      g_UiCommandDragStartScreenY = mapControl->pointerPressY;
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      /* World point to grid point (Q12), used throughout the editor callbacks: t = y * -0x20C8CC / 2^21,
         gx = x * 0x1C6E9C / 2^20 - t, gy = 2t (64-bit products); + 0x3FF & ~0xFFF snaps to the grid. */
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      g_UiCommandDragAnchorWorldXQ12 = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - worldCoordinateTerm);
      g_UiCommandDragAnchorWorldYQ12 = INGAME_SNAP_GRID_Q12(worldCoordinateTerm * 2);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_CLEAR_SCRATCH,0,0,0);
      return;
    }
    if (g_UiCommandModeC == 1) {
      mapControl->runtimeFlags = mapControl->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        g_UiCommandDragStartScreenX = WORLD_POINTER_NO_HIT;
        return;
      }
      g_UiCommandDragStartScreenX = mapControl->pointerPressX;
      g_UiCommandDragStartScreenY = mapControl->pointerPressY;
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      g_UiCommandDragAnchorWorldXQ12 = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - worldCoordinateTerm);
      g_UiCommandDragAnchorWorldYQ12 = INGAME_SNAP_GRID_Q12(worldCoordinateTerm * 2);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_CLEAR_SCRATCH,0,0,0);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ResetLocalInfluenceState(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_RESET_INFLUENCE,0,0,0);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (g_UiCommandModeD != 3) {
      if (g_UiCommandModeD == 1) {
        if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
          return;
        }
        FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
        scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
        scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        snappedWorldY = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
        worldCoordinateTerm = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - snappedWorldY);
        snappedWorldY = INGAME_SNAP_GRID_Q12(snappedWorldY * 2);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedMatchingRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_REPLACE_MATCHING,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
        return;
      }
      if (g_UiCommandModeD != 2) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainEditBuffer_CopyCellMaterialBytes(g_LocalPlayerRuntimeId,0,0,0);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_COPY_MATERIALS,0,0,0);
        return;
      }
      if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
        FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
        scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
        scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        snappedWorldY = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
        worldCoordinateTerm = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - snappedWorldY);
        snappedWorldY = INGAME_SNAP_GRID_Q12(snappedWorldY * 2);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedNonTargetRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
        return;
      }
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_SMOOTHING:
    if (g_UiCommandModeE == 0) {
      if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
        g_UiCommandDragStartScreenX = mapControl->pointerPressX;
        g_UiCommandDragStartScreenY = mapControl->pointerPressY;
        FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
        scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
        scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
        g_UiCommandDragAnchorWorldXQ12 = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - worldCoordinateTerm);
        g_UiCommandDragAnchorWorldYQ12 = INGAME_SNAP_GRID_Q12(worldCoordinateTerm * 2);
        return;
      }
      g_UiCommandDragStartScreenX = WORLD_POINTER_NO_HIT;
      return;
    }
    if (g_UiCommandModeE == 1) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      /* The drag sets the flag when the pressed cell lacks it and clears it otherwise; outside the
         field it sets it. */
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      mapFieldGrid = mapControl->fieldGrid;
      cellX = (int)(((FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - worldCoordinateTerm) + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
      if (cellX < 0) {
        g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
        return;
      }
      cellY = (int)(worldCoordinateTerm * 2 + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
      if (cellY < 0) {
        g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
        return;
      }
      if (cellX < (int)mapFieldGrid->gridWidth) {
        if (cellY < (int)mapFieldGrid->gridHeight) {
          g_UiCommandTerrainMaskToggleValue =
               mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
               FIELD_CELL_FLUID_RECEIVER_EXCLUDED ^ FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
          return;
        }
        g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
        return;
      }
      g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
      return;
    }
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    mapFieldGrid = mapControl->fieldGrid;
    cellX = (int)(((FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - worldCoordinateTerm) + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
    if (cellX < 0) {
      g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_SOURCE_EXCLUDED;
      return;
    }
    cellY = (int)(worldCoordinateTerm * 2 + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
    if (cellY < 0) {
      g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_SOURCE_EXCLUDED;
      return;
    }
    if (cellX < (int)mapFieldGrid->gridWidth) {
      if (cellY < (int)mapFieldGrid->gridHeight) {
        g_UiCommandTerrainMaskToggleValue =
             mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
             FIELD_CELL_FLUID_SOURCE_EXCLUDED ^ FIELD_CELL_FLUID_SOURCE_EXCLUDED;
        return;
      }
      g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_SOURCE_EXCLUDED;
      return;
    }
    g_UiCommandTerrainMaskToggleValue = FIELD_CELL_FLUID_SOURCE_EXCLUDED;
    return;
  case EDITOR_MODE_UNIT_PLACEMENT:
  case EDITOR_MODE_OBJECT_PLACEMENT:
    /* Mode 3 places the mode-G army for its owner faction (sub-mode A), mode 4 the mode-4 army (sub-mode B). */
    if (g_UiCommandModeG == EDITOR_MODE_UNIT_PLACEMENT) {
      commandPayload = g_UiCommandModeGOwnerFactionIndex;
      lookupToken = g_UiCommandModeGArmyAssetId;
      placementSubMode = g_UiCommandModeA;
    }
    else {
      commandPayload = 0;
      lookupToken = g_UiCommandMode4ArmyAssetId;
      placementSubMode = g_UiCommandModeB;
    }
    if (placementSubMode == 0) {
      if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetPlacementFaction(g_LocalPlayerRuntimeId,0,0,commandPayload);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_SET_FACTION,0,0,commandPayload);
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_CreatePlacementArmy
                    (g_LocalPlayerRuntimeId,pointerX,pointerY,lookupToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_CREATE_ARMY,pointerX,pointerY,lookupToken);
        }
        g_UiCommandDragReferenceX = pointerY;
        g_UiCommandDragReferenceY = pointerX;
        g_UiCommandDragStartScreenX = mapControl->pointerPressX;
        g_UiCommandDragStartScreenY = mapControl->pointerPressY;
        return;
      }
      return;
    }
    if (placementSubMode == 1) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      if (ownerNodeUnderPointer != NULL) {
        commandPayload = (int)((ModelRuntimeSlot *)ownerNodeUnderPointer->runtimePayload)->
                         ownerArmyRuntimeOrSavedOffset.armyRuntime -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ApplyEntryOrAll(g_LocalPlayerRuntimeId,0,0,commandPayload);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_DESTROY_ARMIES,0,0,commandPayload);
        return;
      }
    }
    else {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      if (ownerNodeUnderPointer != NULL) {
        g_UiCommandDragStartScreenX = mapControl->pointerPressX;
        g_UiCommandDragStartScreenY = mapControl->pointerPressY;
        commandPayload = (int)((ModelRuntimeSlot *)ownerNodeUnderPointer->runtimePayload)->
                         ownerArmyRuntimeOrSavedOffset.armyRuntime -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetPlacementArmy(g_LocalPlayerRuntimeId,0,0,commandPayload);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_SET_ARMY,0,0,commandPayload);
        }
        g_UiCommandDragReferenceX = pointerY;
        g_UiCommandDragReferenceY = pointerX;
        return;
      }
    }
    /* no army under the pointer: start a drag selection instead */
    mapControl->runtimeFlags = mapControl->runtimeFlags | WORLD_RUNTIME_FLAG_DRAG_SELECTING;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
    return;
  case EDITOR_MODE_REGION:
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    mapFieldGrid = mapControl->fieldGrid;
    cellX = (int)(((FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - worldCoordinateTerm) + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
    if (cellX < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    cellY = (int)(worldCoordinateTerm * 2 + INGAME_GRID_SNAP_BIAS_Q12) >> Q12_SHIFT;
    if (cellY < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    if (cellX < (int)mapFieldGrid->gridWidth) {
      if ((int)mapFieldGrid->gridHeight <= cellY) {
        g_UiCommandCallerMaskHighBit = 0;
        return;
      }
      /* region g_UiCommandModeF owns cell flag FIELD_CELL_XENITE_SUPPORT << region; bit 31 makes the drag remove it again */
      if ((mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
          FIELD_CELL_XENITE_SUPPORT << ((uint8_t)g_UiCommandModeF & SHIFT_COUNT_MASK)) != 0) {
        g_UiCommandCallerMaskHighBit = INGAME_REGION_MASK_REMOVE;
        return;
      }
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    g_UiCommandCallerMaskHighBit = 0;
    return;
  }
  /* remaining tools: rectangle selection of grid cells; without Shift/Ctrl a new selection replaces the old */
  g_UiCommandSelectionAnchorWorldXQ12 = WORLD_POINTER_NO_HIT;
  if (pointerRegionCode != WORLD_POINTER_NO_HIT) {
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) == 0) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_CLEAR_SELECTION,0,0,0);
      }
    }
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    worldCoordinateTerm = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    g_UiCommandSelectionAnchorWorldXQ12 =
         (FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - worldCoordinateTerm;
    g_UiCommandSelectionAnchorWorldYQ12 = worldCoordinateTerm * 2;
    g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    g_UiCommandSelectionCurrentWorldYQ12 = g_UiCommandSelectionAnchorWorldYQ12;
  }
  return;
}


/* Address: 0x005703D0.
   Map editor pointer drag (selection.updateDragSelectionCallback of the world runtime while the editor is
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
  int64_t scaledGridX;
  int64_t scaledGridY;
  uint32_t placementSubMode;
  int boundWorldX;
  CommandPayload moveDeltaX;
  int boundWorldY;
  int workValue;
  uint32_t encodedValue;
  int upperWorldY;
  CommandPayload rowOrDeltaValue;
  uint32_t columnValue;
  int lowerWorldY;
  InGameCommandPayloadTripletValue32 payloadValue;
  uint32_t *tripletClearCursor;
  WorldOwnerListNode *runtimeNode;
  CommandPayload *tripletEntry;
  bool conditionResult;
  FixedVectorQ12 nearestTerrainPoint;
  GameEntityRuntime *entry;
  
  if ((mapControl->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    /* clears both triplet buffers with their counts (26 dwords) */
    tripletClearCursor = (uint32_t *)&g_InGameSelectionInsertTripletDwords;
    for (workValue = 26; workValue != 0; workValue--) {
      *tripletClearCursor = 0;
      tripletClearCursor++;
    }
    runtimeNode = (WorldOwnerListNode *)mapControl->ownerListHead;
    workValue = mapControl->activeFactionRuntimeIndex;
    if (runtimeNode == NULL) {
      return;
    }
    do {
      if ((((runtimeNode->runtimeFlags & MODEL_NODE_FLAG_RENDERED) != 0) &&
          (entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
          (runtimeNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED) != 0)) &&
         (workValue == (entry->common).ownership.ownerIndex)) {
        payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
        conditionResult = WorldRuntimeNode_IsPositionInsideBounds(runtimeNode,mapControl);
        if (conditionResult) {
          conditionResult = SelectionInfo_IsEntryAbsent(entry);
          encodedValue = g_InGameSelectionInsertTripletDwordCount;
          if (((conditionResult) &&
              (conditionResult = InGameCommandQueue_ContainsTripletValue(payloadValue,
                                   INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_INSERT), !conditionResult))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionInsertTripletDwords + encodedValue * 4) = payloadValue, encodedValue < 11))
          {
            g_InGameSelectionInsertTripletDwordCount++;
          }
        }
        else {
          conditionResult = SelectionInfo_IsEntryAbsent(entry);
          encodedValue = g_InGameSelectionRemoveTripletDwordCount;
          if (((!conditionResult) &&
              (conditionResult = InGameCommandQueue_ContainsTripletValue(payloadValue,
                                   INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_REMOVE), !conditionResult))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionRemoveTripletDwords + encodedValue * 4) = payloadValue, encodedValue < 11))
          {
            g_InGameSelectionRemoveTripletDwordCount++;
          }
        }
      }
      runtimeNode = runtimeNode->nextNode;
    } while (runtimeNode != NULL);
    if (g_InGameSelectionRemoveTripletDwordCount != 0) {
      tripletEntry = (CommandPayload *)&g_InGameSelectionRemoveTripletDwords;
      do {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                    (g_LocalPlayerRuntimeId,tripletEntry[2],tripletEntry[1],*tripletEntry);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_REMOVE,tripletEntry[2],tripletEntry[1],*tripletEntry);
        }
        encodedValue = g_InGameSelectionRemoveTripletDwordCount;
        tripletEntry += 3;
        g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount - 3;
      } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)encodedValue);
    }
    if (g_InGameSelectionInsertTripletDwordCount == 0) {
      return;
    }
    tripletEntry = (CommandPayload *)&g_InGameSelectionInsertTripletDwords;
    do {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,tripletEntry[2],tripletEntry[1],*tripletEntry);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_INSERT,tripletEntry[2],tripletEntry[1],*tripletEntry);
      }
      encodedValue = g_InGameSelectionInsertTripletDwordCount;
      tripletEntry += 3;
      g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount - 3;
    } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)encodedValue);
    return;
  }
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if (g_UiCommandModeC == 0) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      encodedValue = INGAME_DRAG_DELTA_X_MASK;
      if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) != 0) {
        encodedValue = 0;
      }
      encodedValue = mapControl->pointerX - g_UiCommandDragStartScreenX & encodedValue |
              (mapControl->pointerY - g_UiCommandDragStartScreenY) * INGAME_DRAG_DELTA_Y_SCALE;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_RAISE_HEIGHTS,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyPositiveCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeC == 1) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      encodedValue = INGAME_DRAG_DELTA_X_MASK;
      if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) != 0) {
        encodedValue = 0;
      }
      encodedValue = mapControl->pointerX - g_UiCommandDragStartScreenX & encodedValue |
              (mapControl->pointerY - g_UiCommandDragStartScreenY) * INGAME_DRAG_DELTA_Y_SCALE;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_LOWER_HEIGHTS,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyNegativeCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      columnValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      encodedValue = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - columnValue);
      columnValue = INGAME_SNAP_GRID_Q12(columnValue * 2);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE,0,columnValue,encodedValue);
        return;
      }
      FieldGrid_RebuildLocalInfluenceState(g_LocalPlayerRuntimeId,0,columnValue,encodedValue);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (g_UiCommandModeD != 3) {
      if (g_TerrainMaterialTextureSets[g_UiCommandAbsoluteSelectionIndex] ==
          NULL) {
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
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      columnValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      encodedValue = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - columnValue);
      columnValue = INGAME_SNAP_GRID_Q12(columnValue * 2);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_PAINT_MATERIAL,g_UiCommandAbsoluteSelectionIndex,columnValue,encodedValue);
        return;
      }
      FieldGrid_ApplyLocalCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,columnValue,encodedValue);
      return;
    }
    break;
  case EDITOR_MODE_TERRAIN_SMOOTHING:
    if (g_UiCommandModeE == 0) {
      if (g_UiCommandDragStartScreenX == WORLD_POINTER_NO_HIT) {
        return;
      }
      encodedValue = INGAME_DRAG_DELTA_X_MASK;
      if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL)) != 0) {
        encodedValue = 0;
      }
      columnValue = mapControl->pointerX - g_UiCommandDragStartScreenX;
      workValue = mapControl->pointerY - g_UiCommandDragStartScreenY;
      g_UiCommandDragStartScreenX = g_UiCommandDragStartScreenX + columnValue;
      g_UiCommandDragStartScreenY = g_UiCommandDragStartScreenY + workValue;
      encodedValue = columnValue & encodedValue | workValue * INGAME_DRAG_DELTA_Y_SCALE;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_SMOOTH,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyEncodedCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeE != 1) {
      if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
        return;
      }
      FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
      scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      columnValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
      encodedValue = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - columnValue);
      columnValue = INGAME_SNAP_GRID_Q12(columnValue * 2);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
        return;
      }
      FieldGrid_SetCellFluidSourceExcluded
                (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
      return;
    }
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    columnValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    encodedValue = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - columnValue);
    columnValue = INGAME_SNAP_GRID_Q12(columnValue * 2);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
      return;
    }
    FieldGrid_SetCellFluidReceiverExcluded
              (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
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
      moveDeltaX = pointerY - g_UiCommandDragReferenceX;
      rowOrDeltaValue = pointerX - g_UiCommandDragReferenceY;
      g_UiCommandDragReferenceX = g_UiCommandDragReferenceX + moveDeltaX;
      g_UiCommandDragReferenceY = g_UiCommandDragReferenceY + rowOrDeltaValue;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_MOVE,0,rowOrDeltaValue,moveDeltaX);
        return;
      }
      SelectionPlayerRuntime_MovePrimarySelectionBy
                (g_LocalPlayerRuntimeId,0,rowOrDeltaValue,moveDeltaX);
      return;
    }
    /* EAX is the horizontal drag distance computed before the call (MOV EAX,[EBX+0x168]; SUB EAX,
       [DragStartScreenX] at 00570ac0); g_PointerSetPosition preserves EAX, it does not return a value. */
    workValue = mapControl->pointerX - g_UiCommandDragStartScreenX;
    g_PointerSetPosition(g_UiCommandDragStartScreenY,g_UiCommandDragStartScreenX);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_ROTATE,0,0,workValue << 6);
      return;
    }
    SelectionPlayerRuntime_RotatePrimarySelectionBy(g_LocalPlayerRuntimeId,0,0,workValue << 6);
    return;
  case EDITOR_MODE_REGION:
    if (pointerRegionCode == WORLD_POINTER_NO_HIT) {
      return;
    }
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    columnValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    encodedValue = INGAME_SNAP_GRID_Q12(FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT) - columnValue);
    columnValue = INGAME_SNAP_GRID_Q12(columnValue * 2);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_EDITOR_APPLY_REGION_MASK,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,columnValue,encodedValue);
      return;
    }
    FieldGrid_SetCellResourceSupportFlag
              (g_LocalPlayerRuntimeId,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,columnValue,encodedValue);
    return;
  }
  if ((pointerRegionCode != WORLD_POINTER_NO_HIT) && (g_UiCommandSelectionAnchorWorldXQ12 != WORLD_POINTER_NO_HIT)) {
    FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid,&nearestTerrainPoint);
    scaledGridX = (int64_t)nearestTerrainPoint.xQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    scaledGridY = (int64_t)nearestTerrainPoint.yQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
    encodedValue = FIXED_PRODUCT_SHR(scaledGridY, Q20_SHIFT + 1);
    boundWorldY = (FIXED_PRODUCT_SHR(scaledGridX, Q20_SHIFT)) - encodedValue;
    workValue = encodedValue * 2;
    /* boundWorldY/workValue hold the new corner here. The original exchanges it (XCHG) with
       g_UiCommandSelectionCurrentWorld*; the rectangle anchor..old corner is deselected row by row, then
       anchor..new corner is selected. */
    LOCK();
    UNLOCK();
    LOCK();
    UNLOCK();
    boundWorldX = g_UiCommandSelectionAnchorWorldXQ12;
    if ((int)g_UiCommandSelectionCurrentWorldXQ12 < (int)g_UiCommandSelectionAnchorWorldXQ12) {
      boundWorldX = g_UiCommandSelectionCurrentWorldXQ12;
      g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    }
    upperWorldY = g_UiCommandSelectionCurrentWorldYQ12;
    lowerWorldY = g_UiCommandSelectionAnchorWorldYQ12;
    if ((int)g_UiCommandSelectionCurrentWorldYQ12 < (int)g_UiCommandSelectionAnchorWorldYQ12) {
      upperWorldY = g_UiCommandSelectionAnchorWorldYQ12;
      lowerWorldY = g_UiCommandSelectionCurrentWorldYQ12;
    }
    encodedValue = INGAME_SNAP_GRID_Q12(boundWorldX);
    rowOrDeltaValue = INGAME_SNAP_GRID_Q12(lowerWorldY);
    columnValue = INGAME_SNAP_GRID_Q12(g_UiCommandSelectionCurrentWorldXQ12);
    g_UiCommandSelectionCurrentWorldXQ12 = boundWorldY;
    g_UiCommandSelectionCurrentWorldYQ12 = workValue;
    if ((int)encodedValue <= (int)columnValue) {
      for (; (int)rowOrDeltaValue <= (int)INGAME_SNAP_GRID_Q12(upperWorldY); rowOrDeltaValue = rowOrDeltaValue + Q12_ONE) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_RemoveRange(g_LocalPlayerRuntimeId,columnValue,rowOrDeltaValue,encodedValue);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_DESELECT_RANGE,columnValue,rowOrDeltaValue,encodedValue);
        }
      }
    }
    workValue = g_UiCommandSelectionAnchorWorldXQ12;
    boundWorldX = g_UiCommandSelectionCurrentWorldXQ12;
    if ((int)g_UiCommandSelectionCurrentWorldXQ12 < (int)g_UiCommandSelectionAnchorWorldXQ12) {
      workValue = g_UiCommandSelectionCurrentWorldXQ12;
      boundWorldX = g_UiCommandSelectionAnchorWorldXQ12;
    }
    boundWorldY = g_UiCommandSelectionCurrentWorldYQ12;
    upperWorldY = g_UiCommandSelectionAnchorWorldYQ12;
    if ((int)g_UiCommandSelectionCurrentWorldYQ12 < (int)g_UiCommandSelectionAnchorWorldYQ12) {
      boundWorldY = g_UiCommandSelectionAnchorWorldYQ12;
      upperWorldY = g_UiCommandSelectionCurrentWorldYQ12;
    }
    encodedValue = INGAME_SNAP_GRID_Q12(workValue);
    rowOrDeltaValue = INGAME_SNAP_GRID_Q12(upperWorldY);
    columnValue = INGAME_SNAP_GRID_Q12(boundWorldX);
    if ((int)encodedValue <= (int)columnValue) {
      for (; (int)rowOrDeltaValue <= (int)INGAME_SNAP_GRID_Q12(boundWorldY); rowOrDeltaValue = rowOrDeltaValue + Q12_ONE) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_InsertRange(g_LocalPlayerRuntimeId,columnValue,rowOrDeltaValue,encodedValue);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_SELECT_RANGE,columnValue,rowOrDeltaValue,encodedValue);
        }
      }
    }
  }
  return;
}


/* Address: 0x00570D60.
   Map editor pointer release (selection.commitPointerActionCallback of the world runtime while the editor is
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
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_ConvertHeightsToDeltas(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS,0,0,0);
      }
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if (((g_UiCommandModeD != 3) && (g_UiCommandModeD != 1)) && (g_UiCommandModeD != 2)) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_SubtractCurrentCellMaterialBytes(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS,0,0,0);
      }
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
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        PlayerRuntime_ClearPlacementArmy(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLACEMENT_CLEAR_ARMY,0,0,0);
      }
    }
  }
  return;
}


/* Address: 0x00570F30.
   Map editor context action (selection.dispatchWorldContextActionCallback of the world runtime while the
   editor is active): clears the cell selection on the height and material tabs and the army selection on
   the unit placement tab.
*/
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime)

{
  switch(g_UiCommandModeG) {
  case EDITOR_MODE_TERRAIN_HEIGHT:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_CLEAR_SELECTION,0,0,0);
    }
    break;
  case EDITOR_MODE_TERRAIN_MATERIAL:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_EDITOR_CLEAR_SELECTION,0,0,0);
    }
    break;
  case EDITOR_MODE_UNIT_PLACEMENT:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
    }
  }
  return;
}


/* Address: 0x005609F0.
   In-game command INGAME_COMMAND_EDITOR_ACTIVE_STATE: enters or (EDITOR_ACTIVE_STATE_LEAVE) leaves the map
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
  uint8_t *pageStackBlock;
  InGameRuntimeRoot *root;
  uint32_t modeOrValue;
  int remainingCount;
  GraphicsTextureSourceAsset *textureSourceValue;
  InGameNotificationQueueRecord *queueSlotCursor;
  TerrainDirectionRecord *directionRecord;
  FieldGridCell *fieldCellCursor;
  ArmyAssetRecordPrefix **registrySlot;
  uint32_t activePageIndex;
  FieldGridAsset *worldFieldGrid;
  
  modeOrValue = g_UiCommandModeG;
  root = g_InGameRuntimeRoot;
  if ((activeStateFlags & EDITOR_ACTIVE_STATE_LEAVE) == 0) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
      pageStackBlock = (uint8_t *)INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack);
      g_UiCommandRuntimeFlags =
           g_UiCommandRuntimeFlags |
           (UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
      runtimeFlagsField = &(g_InGameRuntimeRoot->worldRuntime).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | INGAME_WORLD_FLAG_EDITOR; /* cleared on leaving */
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGPrimaryPageIndices[modeOrValue],(UiPageStackControl *)pageStackBlock);
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGSecondaryPageIndices[modeOrValue],
                 (UiPageStackControl *)INGAME_UI(root,modeDetailPageStack));
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGTertiaryPageIndices[modeOrValue],
                 (UiPageStackControl *)INGAME_UI(root,modeCommandPageStack));
      activePageIndex = UiPageStack_ActivePageIndex(&root->sidePanelPageStack);
      if (activePageIndex == 0) {
        UiPageStack_SetActiveIndex(1,&root->resourceBarModePageStack);
        UiPageStack_SetActiveIndex(1,&root->gamePanelsModePageStack);
      }
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(root,sidePanelMenuButtonStack));
      root->worldOverlayCallback = NULL;
      (root->worldRuntime).selection.dispatchCommandCallback =
           InGameCameraCommand_DispatchByCodeAndModifierFlags;
      (root->worldRuntime).selection.resolveContextActionPrimaryCallback =
           InGameUiCommand_ResolveCursorCodeByMode;
      (root->worldRuntime).selection.resolveContextActionSecondaryCallback =
           InGameUiCommand_ResolveCursorCodeByMode;
      (root->worldRuntime).selection.beginPointerCaptureCallback =
           InGameUiCommand_BeginInteractionByMode;
      (root->worldRuntime).selection.updateDragSelectionCallback =
           InGameUiCommand_UpdateInteractionByMode;
      (root->worldRuntime).selection.commitPointerActionCallback =
           InGameUiCommand_EndInteractionByMode;
      (root->worldRuntime).fieldRegion.clearTransientStateCallback =
           UiCommandRuntime_CallbackNoOp;
      (root->worldRuntime).selection.dispatchWorldContextActionCallback =
           InGameUiCommand_ResetInteractionByMode;
      g_UiRootCallbacks_0054FBC0.keyboardFallback =
           InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags;
      /* InGameCommandModeG_Select0..5, applied to the mode's tab control. */
      (*(void (*)(UiSelectableControl *))g_UiCommandModeGHandlers[modeOrValue])
                ((UiSelectableControl *)
                 THANDOR_UI_AT(root,g_UiCommandModeGControlOffsets[modeOrValue]));
      /* zeroes the first 32 dwords of the notification queue (the cursor steps 4 bytes, not a record) */
      queueSlotCursor = root->notificationQueue;
      for (remainingCount = 32; remainingCount != 0; remainingCount--) {
        queueSlotCursor->movieId = 0;
        queueSlotCursor = (InGameNotificationQueueRecord *)&queueSlotCursor->priority;
      }
      Movie_Close();
      modeOrValue = g_UiCommandAbsoluteSelectionIndex;
      textureSourceValue = g_InGamePanelTextureSource;
      if (root->notificationButtonCursorFrame == PAYLOAD_ACTIVE) {
        root->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
      }
      materialTextureSet = g_TerrainMaterialTextureSets[modeOrValue];
      root->notificationButtonTextureSource = (uint32_t)textureSourceValue;
      /* preview texture of the selected material */
      textureSourceValue = NULL;
      if (materialTextureSet != NULL) {
        textureSourceValue = materialTextureSet->entries[0].sourceAsset;
      }
      root->notificationButtonSubresource = INGAME_PANEL_SUBRESOURCE_NOTIFICATION_IDLE;
      ((UiImagePanelControl *)INGAME_UI(root,materialToolSelectedSwatch))->textureSource = textureSourceValue;
      UiCommandMatrix_SelectIndex(g_UiCommandAbsoluteSelectionIndex,(UiNodeBase *)root);
      g_UiCommandModeGArmyAssetId = ArmyAssetRegistry_NormalizeIdToPlaceableUnit(g_UiCommandModeGArmyAssetId);
      modeOrValue = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
      ((UiImagePanelControl *)INGAME_UI(root,unitPlacementPreviewImage))->textureSource = (GraphicsTextureSourceAsset *)modeOrValue;
      g_UiCommandMode4ArmyAssetId = ArmyAssetRegistry_NormalizeIdToPlaceableObject(g_UiCommandMode4ArmyAssetId);
      modeOrValue = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
      ((UiImagePanelControl *)INGAME_UI(root,objectPlacementPreviewImage))->textureSource = (GraphicsTextureSourceAsset *)modeOrValue;
      FieldGrid_SetOccupancyMaskByteBit0AllCells
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
      WorldRuntime_ForEachOwnerListNode
                (&root->worldRuntime,
                 ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
                 &root->worldRuntime);
      FieldGrid_ClassifyCellFlagsToRuntimeByte
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
      remainingCount = 256;
      directionRecord = g_TerrainDirectionRecordTable256;
      do {
        directionRecord->angleAComponent0ScaledQ28 = 0;
        directionRecord->angleAComponent1ScaledQ28 = 0;
        directionRecord->angleBComponent0ScaledQ28 = 0;
        directionRecord++;
        remainingCount--;
      } while (remainingCount != 0);
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
  else if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
    g_UiCommandRuntimeFlags =
         g_UiCommandRuntimeFlags &
         ~(UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack));
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(root,modeDetailPageStack));
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(root,modeCommandPageStack));
    activePageIndex = UiPageStack_ActivePageIndex(&root->sidePanelPageStack);
    if (activePageIndex == 0) {
      UiPageStack_SetActiveIndex(0,&root->resourceBarModePageStack);
      UiPageStack_SetActiveIndex(0,&root->gamePanelsModePageStack);
    }
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(root,sidePanelMenuButtonStack));
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
    (root->worldRuntime).fieldRegion.clearTransientStateCallback =
         InGameUiRuntime_ResetNotificationButtonCursor;
    (root->worldRuntime).selection.dispatchWorldContextActionCallback =
         InGameUiRuntime_DispatchWorldContextActionCallback;
    runtimeFlagsField = &(root->worldRuntime).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS;
    g_UiRootCallbacks_0054FBC0.keyboardFallback = InGameHotkeys_DispatchCommandByFlags;
    /* free the cached preview textures of all army asset records */
    registrySlot = g_ArmyAssetRecordRegistry;
    remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT;
    do {
      armyAsset = *registrySlot;
      if (armyAsset != NULL) {
        g_MemoryApi.free((void *)armyAsset[2].byteSize);
        armyAsset[2].byteSize = 0;
      }
      registrySlot++;
      remainingCount--;
    } while (remainingCount != 0);
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells
                ((root->worldRuntime).activeFactionRuntimeIndex,
                 (root->worldRuntime).fieldGrid);
    }
    WorldRuntime_ForEachOwnerListNode
              (&root->worldRuntime,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
               ,&root->worldRuntime);
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
    g_UiHoverSelectionRecord = NULL;
    InGameSelectionDetailPanel_Rebuild();
  }
  return;
}


/* Address: 0x005622F0.
   In-game command INGAME_COMMAND_EDITOR_SAVE_MAP (editor hotkey F2): writes the edited map back - the field
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
                    ((uint32_t *)(g_InGameRuntimeRoot->worldRuntime).fieldGrid,&fieldSaveError)) {
    FatalError_ReportIfFailed(fieldSaveError,true);
  }
  if (!InGameLevelRuntime_SaveLevelAssetImageFromWorldState
                    ((InGameLevelSaveWorldView *)&runtimeRoot->worldRuntime,&levelSaveError)) {
    FatalError_ReportIfFailed(levelSaveError,true);
  }
  return;
}


/* Address: 0x00567040.
   Shows an in-game message line (chat, player departure, network notices): adds the UTF-16 text to the
   recent-text history and rebuilds the eight-line pointer list the in-game UI displays
   (g_InGameRuntimeRoot->recentTextHistory).
*/
void InGameRecentTextHistory_InsertAndRebuild8(uint16_t *text)

{
  RecentTextHistoryPointerList *messageList;

  messageList = &g_InGameRuntimeRoot->recentTextHistory;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(8,messageList);
  return;
}


/* Address: 0x0056B850.
   UI action 0x1002 (g_InGameUiActionHandlersPage10[2], InGameUiImage.messageCancelButton): closes the message
   window - shows the world view again and switches the game window page stack back to page 0.
*/
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  INGAME_UI(source,worldView)->nodeFlags = INGAME_UI(source,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,(UiPageStackControl *)INGAME_UI(source,gameWindowPageStack));
  return;
}


/* Address: 0x0056B890.
   UI action 0x1004 (g_InGameUiActionHandlersPage10[4], InGameUiImage.messageSendButton; also called by
   InGameSevenSlotCommand_SubmitAndClosePage): sends the text of the message window. The text is narrowed to
   48 bytes, the recipient mask is built from the selected tab and its check boxes (as in
   InGameChatInput_SendLineOrCheckCheatPhrase), then the mask, the text as four 12-byte chat commands and the publish command
   are issued, and the text field is cleared.
*/
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source)

{
  int32_t *formBase;
  int *controlOffsetEntry;
  int offsetOrCount;
  uint32_t slotIndex;
  CommandPayload recipientMask;
  uint32_t slotBit;
  UiAnchorFractionQ31 *textCursor;
  bool isSelected;
  UiNodeBase *recipientTab;

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  /* offsets below are relative to the text field (root+0x1C98): +0x1EC the faction tab
     (messageRecipientPlayersTab), +0x24C the session player tab (messageRecipientGroupsTab) */
  formBase = (int32_t *)INGAME_UI(source,messageTextEdit);
  RichTextCommandStream_CopyToNarrow
            (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,((UiTextEditControl *)INGAME_UI(source,messageTextEdit))->textBuffer);
  /* Original quirk: the result is not tested; with no tab selected this is the last tab */
  UiSelectableGroup_FindVisibleSelected(&recipientTab,NULL,3,
      INGAME_UI(source,messageRecipientAllTab),
      INGAME_UI(source,messageRecipientGroupsTab),
      INGAME_UI(source,messageRecipientPlayersTab));
  offsetOrCount = (int)recipientTab - (int)formBase;
  if (offsetOrCount ==
      (int)offsetof(InGameUiImage,messageRecipientPlayersTab) - (int)offsetof(InGameUiImage,messageTextEdit)) {
    slotIndex = 0;
    recipientMask = 0;
    slotBit = INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE;
    do {
      controlOffsetEntry = g_UiSevenSlotSelectionControlOffsets + slotIndex;
      slotBit = slotBit * 2;
      slotIndex++;
      isSelected = (bool)UiSelectableControl_IsSelected
                              ((UiSelectableControl *)(*controlOffsetEntry + -(int)offsetof(InGameUiImage,messageTextEdit) + (int)formBase));
      if (isSelected) {
        recipientMask = recipientMask | slotBit;
      }
    } while (slotIndex < 7);
  }
  else if (offsetOrCount ==
           (int)offsetof(InGameUiImage,messageRecipientGroupsTab) - (int)offsetof(InGameUiImage,messageTextEdit)) {
    slotIndex = 0;
    recipientMask = 0;
    slotBit = INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE;
    do {
      controlOffsetEntry = g_UiSevenSlotSelectionControlOffsets + slotIndex;
      slotBit = slotBit * 2;
      slotIndex++;
      isSelected = (bool)UiSelectableControl_IsSelected
                              ((UiSelectableControl *)(*controlOffsetEntry + -(int)offsetof(InGameUiImage,messageTextEdit) + (int)formBase));
      if (isSelected) {
        recipientMask = recipientMask | slotBit;
      }
    } while (slotIndex < 7);
  }
  else {
    recipientMask = INGAME_CHAT_RECIPIENT_EVERYONE;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_SetPackedState(g_LocalPlayerRuntimeId,0,0,recipientMask);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_SET_RECIPIENTS,0,0,recipientMask);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[0].payload1,
               g_UiSevenSlotCommandPayloadText.triples[0].payload2,
               g_UiSevenSlotCommandPayloadText.triples[0].payload3);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[0].payload1,
               g_UiSevenSlotCommandPayloadText.triples[0].payload2,
               g_UiSevenSlotCommandPayloadText.triples[0].payload3);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[1].payload1,
               g_UiSevenSlotCommandPayloadText.triples[1].payload2,
               g_UiSevenSlotCommandPayloadText.triples[1].payload3);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[1].payload1,
               g_UiSevenSlotCommandPayloadText.triples[1].payload2,
               g_UiSevenSlotCommandPayloadText.triples[1].payload3);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[2].payload1,
               g_UiSevenSlotCommandPayloadText.triples[2].payload2,
               g_UiSevenSlotCommandPayloadText.triples[2].payload3);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[2].payload1,
               g_UiSevenSlotCommandPayloadText.triples[2].payload2,
               g_UiSevenSlotCommandPayloadText.triples[2].payload3);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[3].payload1,
               g_UiSevenSlotCommandPayloadText.triples[3].payload2,
               g_UiSevenSlotCommandPayloadText.triples[3].payload3);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[3].payload1,
               g_UiSevenSlotCommandPayloadText.triples[3].payload2,
               g_UiSevenSlotCommandPayloadText.triples[3].payload3);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_PublishConditionalRichText(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_PUBLISH,0,0,0);
  }
  ((UiTextEditControl *)INGAME_UI(source,messageTextEdit))->cursorIndex = 0;
  ((UiTextEditControl *)INGAME_UI(source,messageTextEdit))->selectionStart = 0;
  ((UiTextEditControl *)INGAME_UI(source,messageTextEdit))->selectionEnd = 0;
  textCursor = (UiAnchorFractionQ31 *)((UiTextEditControl *)INGAME_UI(source,messageTextEdit))->textBuffer;
  for (offsetOrCount = 24; offsetOrCount != 0; offsetOrCount--) {
    *textCursor = 0;
    textCursor++;
  }
  return;
}


/* Address: 0x005669B0.
   Rebuilds the selection detail panel (page stack: 0 empty, 1 one own entity, 2 grid of up to 12 own entities,
   3 the hovered stock/build record). Page 1 shows armour, energy, name and up to three weapon names of the
   entity plus the name of its linked army asset (definition classes 0x0B/0x0D/0x16), and enables the technology button only when a technology is
   available; page 3 shows the hovered record's armour, costs, build time, energy, name and weapons.
*/
void InGameSelectionDetailPanel_Rebuild(void)

{
  UiPageStackControl *stack;
  uint8_t *clearedControlBytes;
  GraphicsTextureSourceAsset *hoverTextureSource;
  uint32_t buildDuration;
  ModelRuntimeSlot *attachedModelRuntime;
  UiCommandRuntimeRecordPrefix *definitionNode;
  int selectedCountOrCounter;
  uint32_t detailValue;
  EnergyDemandQ4 displayedEnergy;
  uint16_t *sourceText;
  uint32_t metricValue;
  ArmyAssetRecordPrefix *linkedArmyAsset;
  int slotCounterOrOffset;
  int workValue;
  ModelDefinitionHierarchyNodeAddress32 hierarchyNodeAddress;
  ArmyModelTreeNodeAddressView *linkedDefinitionListView;
  InGameRuntimeRoot *rootCursor;
  GameEntityRuntime *lastSelectedEntity;
  int *recordCursor;
  GameEntityRuntime **entitySlot;
  uint16_t *destinationText;
  bool conditionResult;
  uint32_t armyLookupError;
  ArmyAssetRecordPrefix *foundArmyAsset;
  uint32_t armyAssetValue;
  uint16_t *resolvedText;
  ModelDefinitionRecordPrefix *unlockedDefinition;
  ModelRuntimeSlot *selectedModelRuntime;
  ModelRuntimeSlot *selectedModelRuntimeTail;
  ModelRuntimeSlot *selectedModelRuntimeTail2;
  
  definitionNode = g_UiHoverSelectionRecord;
  rootCursor = g_InGameRuntimeRoot;
  if (g_InGameRuntimeRoot == NULL) {
    return;
  }
  slotCounterOrOffset = SELECTION_ENTRY_CAPACITY;
  workValue = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  selectedCountOrCounter = 0;
  lastSelectedEntity = NULL;
  entitySlot = g_SelectionInfoEntitySlots->entries;
  do {
    if (*entitySlot != NULL) {
      selectedCountOrCounter++;
      lastSelectedEntity = *entitySlot;
    }
    entitySlot++;
    slotCounterOrOffset--;
  } while (slotCounterOrOffset != 0);
  stack = &g_InGameRuntimeRoot->selectionDetailPageStack;
  if (g_UiHoverSelectionRecord == NULL) {
    if (selectedCountOrCounter == 1) {
      if (workValue == (lastSelectedEntity->common).ownership.ownerIndex) {
        recordCursor = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        workValue = *recordCursor;
        conditionResult = FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
                           ((RuntimeToken)recordCursor,
                            (g_InGameRuntimeRoot->worldRuntime).selection.activePlayerRuntimeId)
        ;
        if (!conditionResult) {
          /* The technology button stays available when any of the 28 technology slots is available. */
          UiNodeList_UnsuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,(UiNodeBase *)rootCursor);
          selectedCountOrCounter = 28;
          do {
            conditionResult = Technology_IsAvailableForFaction
                               ((PckTechnologyIdCatalog)((ModelDefinition *)workValue)->researchTechnologyIds[selectedCountOrCounter],
                                (lastSelectedEntity->common).ownership.ownerIndex);
            if (conditionResult) break;
            selectedCountOrCounter--;
          } while (selectedCountOrCounter != 0);
          if (!conditionResult) {
            UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,(UiNodeBase *)rootCursor);
          }
        }
        else {
          UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,(UiNodeBase *)rootCursor);
        }
        armyLookupError = ArmyAssetRegistry_FindById((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId,
                                                     &foundArmyAsset);
        armyAssetValue = FatalError_ExitIfFailed(armyLookupError != 0 ? armyLookupError : (uint32_t)foundArmyAsset,
                                                  armyLookupError != 0);
        UiPageStack_SetActiveIndex(1,stack);
        detailValue = ((ArmyAssetRecord *)armyAssetValue)->selectionDetailValue;
        workValue = ModelRuntimeHierarchy_SumArmour((int *)lastSelectedEntity);
        rootCursor->selectionDetailArmyAssetValue = detailValue;
        rootCursor->selectionDetailEntity = lastSelectedEntity;
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,workValue,g_InGameSelectionDetailArmourTextUtf16
                  );
        metricValue = ModelRuntime_QueryActiveHierarchyMetric((ArmyRuntimeSlot *)lastSelectedEntity);
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,metricValue >> 4,
                   g_InGameSelectionDetailEnergyTextUtf16);
        ((UiWrappedTextControl *)INGAME_UI(rootCursor,singleSelectionStatsText))->text = (uint16_t *)(((ArmyAssetRecord *)armyAssetValue)->selectionDetailTemplateVariantIndex +
                                  TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE);
        resolvedText = TextResource_Resolve
                           (((ModelDefinition *)*(int *)(lastSelectedEntity->common).ownership.
                                             definitionOrClassRecord)->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
        sourceText = resolvedText;
        destinationText = g_InGameSelectionDetailNameTextUtf16;
        for (workValue = 64; workValue != 0; workValue--) {
          *destinationText = *sourceText;
          sourceText++;
          destinationText++;
        }
        /* text 0x18004E fills unused weapon slots; name texts are 0x18004F + the definition's name index */
        resolvedText = TextResource_Resolve(TEXT_ID_SELECTION_DETAIL_NO_WEAPON);
        sourceText = resolvedText;
        RichTextCommandStream_CopyExpanded
                  (128,g_InGameSelectionDetailWeaponName0TextUtf16,sourceText,NULL);
        RichTextCommandStream_CopyExpanded
                  (128,g_InGameSelectionDetailWeaponName1TextUtf16,sourceText,NULL);
        RichTextCommandStream_CopyExpanded
                  (128,g_InGameSelectionDetailWeaponName2TextUtf16,sourceText,NULL);
        g_InGameSelectionDetailTextSlot05Utf16[0] = L'-';
        g_InGameSelectionDetailTextSlot05Utf16[1] = 0;
        g_InGameSelectionDetailTextSlot09Utf16[0] = L'-';
        g_InGameSelectionDetailTextSlot09Utf16[1] = 0;
        selectedModelRuntime = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        if (((selectedModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) {
          /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
             foundArmyAsset) */
          ArmyAssetRegistry_FindById((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId,&foundArmyAsset);
          workValue = selectedModelRuntime->researchTechnologyId;
          ((UiWrappedTextControl *)INGAME_UI(rootCursor,singleSelectionStatsText))->text =
               (uint16_t *)(foundArmyAsset->selectionDetailTemplateVariantIndex +
                              TEXT_ID_SELECTION_DETAIL_RESEARCH_TEMPLATE_BASE);
          resolvedText = TextResource_Resolve(workValue * 2 + TECHNOLOGY_TEXT_ID_BASE);
          RichTextCommandStream_CopyExpanded
                    (128,g_InGameSelectionDetailTextSlot09Utf16,resolvedText,NULL);
        }
        if (selectedModelRuntime->attachmentCount != 0) {
          attachedModelRuntime = selectedModelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
          if (attachedModelRuntime != NULL) {
            resolvedText = TextResource_Resolve
                               (((attachedModelRuntime->definitionOrSavedId).definition)->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
            sourceText = resolvedText;
            destinationText = g_InGameSelectionDetailWeaponName0TextUtf16;
            for (workValue = 64; workValue != 0; workValue--) {
              *destinationText = *sourceText;
              sourceText++;
              destinationText++;
            }
          }
          selectedModelRuntimeTail = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
          if (1 < selectedModelRuntimeTail->attachmentCount) {
            attachedModelRuntime = selectedModelRuntimeTail->attachments[1].childModelRuntimeOrSavedOffset;
            if (attachedModelRuntime != NULL) {
              resolvedText = TextResource_Resolve
                                 (((attachedModelRuntime->definitionOrSavedId).definition)->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
              sourceText = resolvedText;
              destinationText = g_InGameSelectionDetailWeaponName1TextUtf16;
              for (workValue = 64; workValue != 0; workValue--) {
                *destinationText = *sourceText;
                sourceText++;
                destinationText++;
              }
            }
            selectedModelRuntimeTail2 = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
            if ((2 < selectedModelRuntimeTail2->attachmentCount) &&
               (attachedModelRuntime = selectedModelRuntimeTail2->attachments[2].
                         childModelRuntimeOrSavedOffset, attachedModelRuntime != NULL)) {
              resolvedText = TextResource_Resolve
                                 (((attachedModelRuntime->definitionOrSavedId).definition)->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
              sourceText = resolvedText;
              destinationText = g_InGameSelectionDetailWeaponName2TextUtf16;
              for (workValue = 64; workValue != 0; workValue--) {
                *destinationText = *sourceText;
                sourceText++;
                destinationText++;
              }
            }
          }
        }
        recordCursor = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        workValue = ((ModelDefinition *)*recordCursor)->runtimeClassId;
        if (workValue == MODEL_RUNTIME_CLASS_22) {
          if (recordCursor[43] != 1) {
            return;
          }
          ArmyAssetRegistry_FindById(recordCursor[24],&linkedArmyAsset);
        }
        else if (workValue == MODEL_RUNTIME_CLASS_11) {
          if (recordCursor[46] != 1) {
            return;
          }
          ArmyAssetRegistry_FindById(recordCursor[24],&linkedArmyAsset);
        }
        else {
          if (workValue != MODEL_RUNTIME_CLASS_13) {
            if (workValue != MODEL_RUNTIME_CLASS_14) {
              return;
            }
            g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,recordCursor[24],
                       g_InGameSelectionDetailWeaponName0TextUtf16);
            return;
          }
          if (recordCursor[46] != 1) {
            return;
          }
          ArmyAssetRegistry_FindById(recordCursor[24],&linkedArmyAsset);
        }
        /* Original quirk: none of the three lookups above checks its status (an unknown id leaves the error
           code in linkedArmyAsset) */
        linkedDefinitionListView =
             (ArmyModelTreeNodeAddressView *)linkedArmyAsset->rootNodeOffsetOrPointer;
        if (linkedArmyAsset->selectionDetailTemplateVariantIndex < 8) {
          ((UiWrappedTextControl *)INGAME_UI(rootCursor,singleSelectionStatsText))->text =
               (uint16_t *)((int)((UiWrappedTextControl *)INGAME_UI(rootCursor,singleSelectionStatsText))->text +
               linkedArmyAsset->selectionDetailTemplateVariantIndex);
        }
        unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                           ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                            (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
        resolvedText = TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
        sourceText = resolvedText;
        destinationText = g_InGameSelectionDetailTextSlot05Utf16;
        goto copyDefinitionNames;
      }
    }
    else if ((selectedCountOrCounter != 0) && (workValue == (lastSelectedEntity->common).ownership.ownerIndex)) {
      UiPageStack_SetActiveIndex(2,stack);
      /* fill up to 12 UiArmyMetricsPanel grid cells (entity, and the army asset's dword into textureSource), then clear the rest */
      selectedCountOrCounter = SELECTION_ENTRY_CAPACITY;
      recordCursor = g_InGameSelectionDetailGridCellOffsets;
      workValue = 12;
      entitySlot = g_SelectionInfoEntitySlots->entries;
      do {
        lastSelectedEntity = *entitySlot;
        if ((lastSelectedEntity != NULL) && (workValue != 0)) {
          slotCounterOrOffset = *recordCursor;
          ((UiArmyMetricsPanel *)THANDOR_UI_AT(rootCursor,slotCounterOrOffset))->entity = (RuntimeModelFactionPrefix *)lastSelectedEntity;
          /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
             foundArmyAsset) */
          ArmyAssetRegistry_FindById((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId,&foundArmyAsset);
          ((UiArmyMetricsPanel *)THANDOR_UI_AT(rootCursor,slotCounterOrOffset))->base.textureSource =
               (GraphicsTextureSourceAsset *)foundArmyAsset[1].registryId;
          /* adds 0: slotCounterOrOffset was just loaded from *recordCursor */
          rootCursor = (InGameRuntimeRoot *)((int)rootCursor + (slotCounterOrOffset - *recordCursor));
          workValue--;
          recordCursor++;
        }
        entitySlot++;
        selectedCountOrCounter--;
      } while (selectedCountOrCounter != 0);
      for (; workValue != 0; workValue--) {
        clearedControlBytes = (uint8_t *)&((UiArmyMetricsPanel *)THANDOR_UI_AT(rootCursor,*recordCursor))->base.textureSource;
        clearedControlBytes[0] = 0;
        clearedControlBytes[1] = 0;
        clearedControlBytes[2] = 0;
        clearedControlBytes[3] = 0;
        recordCursor++;
      }
      /* (The decompile rewrote text slots 05/09 with their own contents here; the asm does not touch them.) */
      return;
    }
    UiPageStack_SetActiveIndex(0,stack);
  }
  else {
    UiPageStack_SetActiveIndex(3,stack);
    hoverTextureSource = definitionNode->textureSource;
    detailValue = ArmyAssetHierarchy_SumFactionUnlockedArmour
                      ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    ((UiImagePanelControl *)INGAME_UI(rootCursor,hoverItemIcon))->textureSource = hoverTextureSource;
    metricValue = definitionNode->buildXeniteCostQ4;
    buildDuration = definitionNode->buildDurationQ5;
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,detailValue,g_InGameSelectionDetailArmourTextUtf16);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,metricValue >> 4,
               g_InGameSelectionDetailBuildXeniteCostTextUtf16);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,buildDuration >> 5,
               g_InGameSelectionDetailBuildTimeTextUtf16);
    displayedEnergy = ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
                      ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,displayedEnergy >> 4,g_InGameSelectionDetailEnergyTextUtf16
              );
    workValue = definitionNode->selectionDetailTemplateVariantIndex + TEXT_ID_SELECTION_DETAIL_HOVER_TEMPLATE_BASE;
    ((UiWrappedTextControl *)INGAME_UI(rootCursor,hoverItemStatsText))->text = (uint16_t *)workValue;
    ((UiWrappedTextControl *)INGAME_UI(rootCursor,unitPlacementStatsText))->text = (uint16_t *)workValue;
    linkedDefinitionListView = (ArmyModelTreeNodeAddressView *)definitionNode->rootNodeOffsetOrPointer;
    unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                        (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
    resolvedText = TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
    sourceText = resolvedText;
    destinationText = g_InGameSelectionDetailNameTextUtf16;
copyDefinitionNames:
    for (workValue = 64; workValue != 0; workValue--) {
      *destinationText = *sourceText;
      sourceText++;
      destinationText++;
    }
    resolvedText = TextResource_Resolve(TEXT_ID_SELECTION_DETAIL_NO_WEAPON);
    sourceText = resolvedText;
    RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName0TextUtf16,sourceText,NULL);
    RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName1TextUtf16,sourceText,NULL);
    RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName2TextUtf16,sourceText,NULL);
    if (linkedDefinitionListView->childListCount != 0) {
      unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                         ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                          linkedDefinitionListView->childList0Address);
      resolvedText = TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
      sourceText = resolvedText;
      destinationText = g_InGameSelectionDetailWeaponName0TextUtf16;
      for (workValue = 64; workValue != 0; workValue--) {
        *destinationText = *sourceText;
        sourceText++;
        destinationText++;
      }
      if (1 < linkedDefinitionListView->childListCount) {
        unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                           ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                            linkedDefinitionListView->childList1Address);
        resolvedText = TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
        sourceText = resolvedText;
        destinationText = g_InGameSelectionDetailWeaponName1TextUtf16;
        for (workValue = 64; workValue != 0; workValue--) {
          *destinationText = *sourceText;
          sourceText++;
          destinationText++;
        }
        if (2 < linkedDefinitionListView->childListCount) {
          unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                             ((rootCursor->worldRuntime).activeFactionRuntimeIndex,
                              linkedDefinitionListView->childList2Address);
          resolvedText = TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
          sourceText = resolvedText;
          destinationText = g_InGameSelectionDetailWeaponName2TextUtf16;
          for (workValue = 64; workValue != 0; workValue--) {
            *destinationText = *sourceText;
            sourceText++;
            destinationText++;
          }
        }
      }
    }
  }
  return;
}

