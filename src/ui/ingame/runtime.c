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
   Ownership: ui/ingame/runtime.
   Purpose: Handles in game ui root keyboard fallback dispatch command by code and modifier flags carry-flag
   result.
   Local calls: InGameUiCommand_SaveFieldAndLevelAssetImages,
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState, InGameSelectionDetailPanel_Rebuild.
   Cross-module calls: UiPageStack_ActivePageNotInListCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiContainer_LayoutChildren [ui/controls/layout],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting [world/terrain/editing],
   TerrainEditBuffer_CommitFlagsAndMaterialDeltas [world/terrain/editing].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf
          (dword keyboardStateMask,dword keyboardEventCode,UiRootNode *uiRoot)

{
  UiNodeVtable **stack;
  sdword *counterField;
  wchar_t screenshotTensDigit;
  wchar_t screenshotOnesDigit;
  UiNodeVtable *modeGPreviewTexture;
  dword mode4PreviewTexture;
  UiCommandModeIndex materialIndex;
  int remainingSteps;
  dword *dispatchRecord;
  dword *nextDispatchRecord;
  StatusValueEaxCf5 pageNotInListResult;
  ArmyRegistryIdEaxCf5_571b00 previousModeGArmy;
  ArmyRegistryIdEaxCf5_571d40 previousMode4Army;
  ArmyRegistryIdEaxCf5_571ab0 nextModeGArmy;
  ArmyRegistryIdEaxCf5_571cf0 nextMode4Army;
  ArmyRegistryIdEaxCf5_571a10 steppedForwardModeGArmy;
  ArmyRegistryIdEaxCf5_571c50 steppedForwardMode4Army;
  ArmyRegistryIdEaxCf5_571a60 steppedBackwardModeGArmy;
  ArmyRegistryIdEaxCf5_571ca0 steppedBackwardMode4Army;
  ArmyRegistryEaxCf5_51b6d0 foundArmyAsset;
  FatalErrorEaxCf5 hoverRecordResult;
  GraphicsFramebufferCaptureEaxCf5 capturedFramebuffer;
  
  nextDispatchRecord = (dword *)THANDOR_ADDR(g_InGameKeyboardDispatchRecords,0);
  do {
    while( true ) {
      do {
        dispatchRecord = nextDispatchRecord;
        if (*dispatchRecord == 0) {
          return;
        }
        nextDispatchRecord = dispatchRecord + 3;
      } while (*dispatchRecord != keyboardEventCode);
      if (dispatchRecord[1] == 0) break;
      if ((keyboardStateMask & dispatchRecord[1]) != 0) goto override_jmp_0056e406_switch;
    }
  } while ((keyboardStateMask & 0x3c) != 0);
override_jmp_0056e406_switch:
                    // WARNING: Switch is manually overridden
  switch(dispatchRecord[2]) {
  case 0x56e5e0:
    stack = (struct UiNodeVtable * *)THANDOR_UI_AT(uiRoot,0x40AC);
    pageNotInListResult = UiPageStack_ActivePageNotInListCf((UiPageStackControl *)stack);
    if (pageNotInListResult.valueOrError == 0) {
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)THANDOR_UI_AT(uiRoot,0x4530));
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)THANDOR_UI_AT(uiRoot,0x4644));
      THANDOR_UI_FIELD(uiRoot,0x9DC + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)0x0;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    else {
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)THANDOR_UI_AT(uiRoot,0x4530));
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)THANDOR_UI_AT(uiRoot,0x4644));
      THANDOR_UI_FIELD(uiRoot,0x9DC + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)THANDOR_UI_AT(uiRoot,0x4104)->leftOffset;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    break;
  case 0x56e670:
    counterField = &THANDOR_UI_FIELD(uiRoot,0x2384 + 0x54,sdword);
    *counterField = *counterField + 1;
    if (0x117 < (uint)THANDOR_UI_FIELD(uiRoot,0x2384 + 0x54,sdword)) {
      THANDOR_UI_FIELD(uiRoot,0x2384 + 0x54,sdword) = 0x112;
    }
    break;
  case 0x56e6a0:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommand_SaveFieldAndLevelAssetImages(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x31c0,0,0,0);
    }
    break;
  case 0x56e6e0:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState(g_LocalPlayerRuntimeId,0,0,4);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x18c0,0,0,4);
    }
    break;
  case 0x56e720:
    if (g_UiCommandModeG == 0) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x25a0,0,0,0);
      }
    }
    else if (g_UiCommandModeG == 1) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_CommitFlagsAndMaterialDeltas(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2870,0,0,0);
      }
    }
    break;
  case 0x56e7c0:
    if ((keyboardStateMask & 0xf) == 0) {
      if (g_UiCommandModeG == 1) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex - 1;
        if ((int)materialIndex < 0) {
          materialIndex = 0x19;
        }
        while (g_TerrainMaterialTextureSets[materialIndex] == (GraphicsTextureSet *)0x0) {
          materialIndex = materialIndex - 1;
          if ((int)materialIndex < 0) {
            materialIndex = 0x19;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        previousModeGArmy = ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf
                           (g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = previousModeGArmy.eax;
        modeGPreviewTexture = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9CD0 + 0x54,struct UiNodeVtable *) = modeGPreviewTexture;
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        previousMode4Army = ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf
                           (g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = previousMode4Army.eax;
        mode4PreviewTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9D88 + 0x54,sdword) = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & 0xc) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0,0xfffffc00);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d70,0,0,0xfffffc00);
        }
      }
      if ((keyboardStateMask & 3) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_AdjustFieldOriginWrappedClamped(g_LocalPlayerRuntimeId,0,0,-0x400);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d00,0,0,0xfffffc00);
        }
      }
    }
    break;
  case 0x56e930:
    if ((keyboardStateMask & 0xf) == 0) {
      if (g_UiCommandModeG == 1) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex + 1;
        if (0x19 < materialIndex) {
          materialIndex = 0;
        }
        while (g_TerrainMaterialTextureSets[materialIndex] == (GraphicsTextureSet *)0x0) {
          materialIndex = materialIndex + 1;
          if (0x19 < materialIndex) {
            materialIndex = 0;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        nextModeGArmy = ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(g_UiCommandModeGArmyAssetId)
        ;
        g_UiCommandModeGArmyAssetId = nextModeGArmy.eax;
        modeGPreviewTexture = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9CD0 + 0x54,struct UiNodeVtable *) = modeGPreviewTexture;
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        nextMode4Army = ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = nextMode4Army.eax;
        mode4PreviewTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9D88 + 0x54,sdword) = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & 0xc) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0,0x400);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d70,0,0,0x400);
        }
      }
      if ((keyboardStateMask & 3) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_AdjustFieldOriginWrappedClamped(g_LocalPlayerRuntimeId,0,0,0x400);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d00,0,0,0x400);
        }
      }
    }
    break;
  case 0x56eaa0:
    if ((keyboardStateMask & 0xf) == 0) {
      if (g_UiCommandModeG == 1) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex - 1;
        remainingSteps = 3;
        if ((int)materialIndex < 0) {
          materialIndex = 0x19;
        }
        while ((g_TerrainMaterialTextureSets[materialIndex] == (GraphicsTextureSet *)0x0 ||
               (remainingSteps = remainingSteps + -1, remainingSteps != 0))) {
          materialIndex = materialIndex - 1;
          if ((int)materialIndex < 0) {
            materialIndex = 0x19;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        steppedForwardModeGArmy = ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = steppedForwardModeGArmy.eax;
        modeGPreviewTexture = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9CD0 + 0x54,struct UiNodeVtable *) = modeGPreviewTexture;
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        steppedForwardMode4Army = ArmyAssetRegistry_StepForwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = steppedForwardMode4Army.eax;
        mode4PreviewTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9D88 + 0x54,sdword) = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & 0xc) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0xfffffc00,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d70,0,0xfffffc00,0);
        }
      }
      if ((keyboardStateMask & 3) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_AdjustFieldOriginWrappedClamped(g_LocalPlayerRuntimeId,0,-0x400,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d00,0,0xfffffc00,0);
        }
      }
    }
    break;
  case 0x56ec10:
    if ((keyboardStateMask & 0xf) == 0) {
      if (g_UiCommandModeG == 1) {
        materialIndex = g_UiCommandAbsoluteSelectionIndex + 1;
        remainingSteps = 3;
        if (0x19 < materialIndex) {
          materialIndex = 0;
        }
        while ((g_TerrainMaterialTextureSets[materialIndex] == (GraphicsTextureSet *)0x0 ||
               (remainingSteps = remainingSteps + -1, remainingSteps != 0))) {
          materialIndex = materialIndex + 1;
          if (0x19 < materialIndex) {
            materialIndex = 0;
          }
        }
        UiCommandMatrix_SelectIndex(materialIndex,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        steppedBackwardModeGArmy = ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = steppedBackwardModeGArmy.eax;
        modeGPreviewTexture = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9CD0 + 0x54,struct UiNodeVtable *) = modeGPreviewTexture;
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        steppedBackwardMode4Army = ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = steppedBackwardMode4Army.eax;
        mode4PreviewTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        THANDOR_UI_FIELD(uiRoot,0x9D88 + 0x54,sdword) = mode4PreviewTexture;
      }
    }
    else {
      if ((keyboardStateMask & 0xc) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainLighting_AdjustDirectionAndRecomputeField(g_LocalPlayerRuntimeId,0,0x400,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d70,0,0x400,0);
        }
      }
      if ((keyboardStateMask & 3) != 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          WorldRuntime_AdjustFieldOriginWrappedClamped(g_LocalPlayerRuntimeId,0,0x400,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2d00,0,0x400,0);
        }
      }
    }
    break;
  case 0x56ed80:
    if (g_UiCommandModeG == 3) {
      g_UiCommandModeGOwnerFactionIndex = g_UiCommandModeGOwnerFactionIndex + 1;
      if (g_GameFactionRuntimeImage.tail.activeFactionCount <
          (uint)g_UiCommandModeGOwnerFactionIndex) {
        g_UiCommandModeGOwnerFactionIndex = 1;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((dword)uiRoot);
    }
    break;
  case 0x56edc0:
    if (g_UiCommandModeG == 3) {
      g_UiCommandModeGOwnerFactionIndex = g_UiCommandModeGOwnerFactionIndex + -1;
      if (g_UiCommandModeGOwnerFactionIndex == 0) {
        g_UiCommandModeGOwnerFactionIndex = g_GameFactionRuntimeImage.tail.activeFactionCount;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((dword)uiRoot);
    }
    break;
  case 0x56ee00:
    InGameCommandModeG_Select0((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4BA4));
    InGameCommandModeC_Select0((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB610));
    break;
  case 0x56ee20:
    InGameCommandModeG_Select0((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4BA4));
    InGameCommandModeC_Select1((UiSpriteButtonControl *)(uiRoot + 0x213));
    break;
  case 0x56ee40:
    InGameCommandModeG_Select0((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4BA4));
    InGameCommandModeC_Select2((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB700));
    break;
  case 0x56ee60:
    if (g_UiCommandModeG == 1) {
      InGameCommandModeG_Select1((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C1C));
      InGameCommandModeD_Select3((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB958));
    }
    else {
      InGameCommandModeG_Select0((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4BA4));
      InGameCommandModeC_Select3((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB778));
    }
    break;
  case 0x56eeb0:
    InGameCommandModeG_Select1((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C1C));
    InGameCommandModeD_Select0((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB7F0));
    break;
  case 0x56eed0:
    InGameCommandModeG_Select1((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C1C));
    InGameCommandModeD_Select1((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB868));
    break;
  case 0x56eef0:
    InGameCommandModeG_Select1((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C1C));
    InGameCommandModeD_Select2((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB8E0));
    break;
  case 0x56ef10:
    InGameCommandModeG_Select2((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C94));
    InGameCommandModeE_Select0((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xB9D0));
    break;
  case 0x56ef30:
    InGameCommandModeG_Select2((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C94));
    InGameCommandModeE_Select1((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBA48));
    break;
  case 0x56ef50:
    InGameCommandModeG_Select2((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C94));
    InGameCommandModeE_Select2((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBAC0));
    break;
  case 0x56ef70:
    InGameCommandModeG_Select2((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C94));
    InGameCommandRange_DispatchState0((UiNodeBase *)THANDOR_UI_AT(uiRoot,0xBB38));
    break;
  case 0x56ef90:
    InGameCommandModeG_Select2((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x4C94));
    InGameCommandRange_DispatchState1(&uiRoot[0x222].base);
    break;
  case 0x56efb0:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select0((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBD90));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x9930));
      InGameCommandModeA_Select0((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBC28));
      foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f020:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select1((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBE80));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x9930));
      InGameCommandModeA_Select1((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBD18));
      foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f090:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select2((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBE08));
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x9930));
      InGameCommandModeA_Select2((UiSpriteButtonControl *)THANDOR_UI_AT(uiRoot,0xBCA0));
      foundArmyAsset = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      hoverRecordResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)hoverRecordResult.eax;
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f100:
    InGameCommandModeG_Select3((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x9930));
    break;
  case 0x56f120:
    InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
    break;
  case 0x56f140:
    InGameCommandModeG_Select5((UiSelectableControl *)THANDOR_UI_AT(uiRoot,0x98B8));
    break;
  case 0x56f160:
    /* The original calls this without pushing arguments (stale stack, RET 0x10); capture the whole
       framebuffer like the end-game and end-movie screenshot commands. */
    capturedFramebuffer = (*g_GraphicsFramebufferCaptureRegion)(g_FramebufferHeight,g_FramebufferWidth,0,0);
    if (!capturedFramebuffer.carry) {
      FileSystem_WriteBufferToPathCf
                (((capturedFramebuffer.eax)->common).allocationSizeBytes,capturedFramebuffer.eax,
                 (word *)(u_Dscreen00_pcx_00572e3a + 1));
      screenshotOnesDigit = u_Dscreen00_pcx_00572e3a[8];
      screenshotTensDigit = u_Dscreen00_pcx_00572e3a[7];
      u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\x01';
      if (0x39 < (ushort)u_Dscreen00_pcx_00572e3a[8]) {
        u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + L'\x01';
        u_Dscreen00_pcx_00572e3a[8] = screenshotOnesDigit + L'\xfff7';
        if (0x39 < (ushort)u_Dscreen00_pcx_00572e3a[7]) {
          u_Dscreen00_pcx_00572e3a[7] = screenshotTensDigit + L'\xfff7';
        }
      }
    }
    break;
  case 0x56f1c0:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState(g_LocalPlayerRuntimeId,0,0,4);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x18c0,0,0,4);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommand150_HandlePlayerDepartureAndOwnership(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x150,0,0,0);
    }
  }
  return;
}


/* Address: 0x0056BAD0.
   Ownership: ui/ingame/runtime.
   Purpose: Calls UiAction1004_SubmitSevenSlotCommand and then UiAction1002_CloseCommandPage for the same source
   node. Queued UI action handler for INGAME_PAGE10[5] (0x1005). Return datatype is preserved for non-queue direct
   callers.
   Local calls: InGameSevenSlotCommand_SubmitTextAndSelectionMask, InGameSevenSlotCommand_ClosePage.
*/
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source)

{
  InGameSevenSlotCommand_SubmitTextAndSelectionMask(source);
  InGameSevenSlotCommand_ClosePage(source);
  return;
}

/* Address: 0x0056A610.
   Ownership: ui/ingame/runtime.
   Purpose: Recovered action-table target INGAME_PAGE10[36] (0x1024).
   Local calls: InGameRecentTextHistory_InsertAndRebuild8.
   Cross-module calls: UiTextControl_UpdateNonEmptyValidity [ui/controls/text],
   RichTextCommandStream_CopyToNarrowCf [assets/text/richtext], UiSelectableGroup_NoneVisibleSelectedCf
   [ui/controls/lists], UiSelectableControl_IsSelectedCf [ui/controls/lists],
   FrontendPlayerTextCommand_SetPackedState [ui/frontend/player], InGameCommandQueue_AppendLocalPlayerCommand
   [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  int remainingDwords;
  uint slotIndex;
  CommandPayloadDword04 packedState;
  uint slotBit;
  int *phraseCursor;
  word *textCursor;
  bool isMatch;
  UiSelectableNodeEaxEcxCf9 visibleSelection;
  
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)commandTextEdit);
  if ((commandTextEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      remainingDwords = 0x10;
      isMatch = true;
      phraseCursor = (int *)THANDOR_ADDR(g_DeveloperChatPhraseUtf16,0);
      textCursor = commandTextEdit->textBuffer;
      do {
        if (remainingDwords == 0) break;
        remainingDwords = remainingDwords + -1;
        isMatch = *phraseCursor == *(int *)textCursor;
        phraseCursor = phraseCursor + 1;
        textCursor = textCursor + 2;
      } while (isMatch);
      if (isMatch) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 0x40000;
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x80000;
        InGameRecentTextHistory_InsertAndRebuild8((word *)u_Hmmm__na_gut________0056321e);
      }
    }
    else {
      RichTextCommandStream_CopyToNarrowCf
                (0x30,g_UiSevenSlotCommandPayloadText.textBytes,commandTextEdit->textBuffer);
      visibleSelection = UiSelectableGroup_NoneVisibleSelectedCf(3,
      THANDOR_UI_AT(commandTextEdit,0x1e94),
      THANDOR_UI_AT(commandTextEdit,0x1e34),
      THANDOR_UI_AT(commandTextEdit,0x1dd4));
      remainingDwords = (int)visibleSelection.node - (int)commandTextEdit;
      if (remainingDwords == 0x1dd4) {
        slotIndex = 0;
        packedState = 0;
        slotBit = 0x100;
        do {
          phraseCursor = g_UiSevenSlotSelectionControlOffsets + slotIndex;
          slotBit = slotBit * 2;
          slotIndex = slotIndex + 1;
          isMatch = (bool)UiSelectableControl_IsSelectedCf
                                  ((UiSelectableControl *)(*phraseCursor + -0xb0 + (int)commandTextEdit));
          if (isMatch) {
            packedState = packedState | slotBit;
          }
        } while (slotIndex < 7);
      }
      else if (remainingDwords == 0x1e34) {
        slotIndex = 0;
        packedState = 0;
        slotBit = 0x8000;
        do {
          phraseCursor = g_UiSevenSlotSelectionControlOffsets + slotIndex;
          slotBit = slotBit * 2;
          slotIndex = slotIndex + 1;
          isMatch = (bool)UiSelectableControl_IsSelectedCf
                                  ((UiSelectableControl *)(*phraseCursor + -0xb0 + (int)commandTextEdit));
          if (isMatch) {
            packedState = packedState | slotBit;
          }
        } while (slotIndex < 7);
      }
      else {
        packedState = 0xffffff00;
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_SetPackedState(g_LocalPlayerRuntimeId,0,0,packedState);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(6000,0,0,packedState);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x17a0,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x17a0,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x17a0,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_AppendTripleClamped
                  (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x17a0,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
                   g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
                   g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
      }
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerTextCommand_PublishConditionalRichText(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1810,0,0,0);
      }
      commandTextEdit->cursorIndex = 0;
      commandTextEdit->selectionStart = 0;
      commandTextEdit->selectionEnd = 0;
      textCursor = commandTextEdit->textBuffer;
      for (remainingDwords = 0x18; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
        textCursor[0] = 0;
        textCursor[1] = 0;
        textCursor = textCursor + 2;
      }
    }
  }
  UiPageStack_SetActiveIndex(0,&THANDOR_CONTAINER_OF(commandTextEdit, InGameCommandTextEntryPage2320, commandTextEdit)->commandPageStack);
  return;
}


/* Address: 0x0050ECE0.
   Ownership: ui/ingame/runtime.
   Purpose: Carry-flag success/failure semantics are preserved in the comment rather than fabricated as an ordinary
   return.
   Cross-module calls: GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid [gameplay/faction/runtime],
   ResourceRegistration_OpenSourceCf [assets/resource/runtime], WidePath_SplitParentAndLeaf [core/text/path],
   ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs [gameplay/army/runtime], Package_UpsertEntry
   [assets/package/runtime], ArmyRuntimePool_RebaseAfterLoad [gameplay/army/runtime].
*/

bool __thandor_cf_preserve_eax_ecx_edx
InGameUiAction1210_ResourceRegistrationHelper(void *runtimeBase,void *resourcePath)

{
  word *timeTextDestination;
  byte *destination;
  InGameLevelConditionStorageView800 *sourceData;
  void *handle;
  dword *modelSlotImage;
  dword *oldUnitImage;
  dword localeValue;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  int remainingCount;
  PckDecodedByteCount unpackedSize;
  dword *headerDwords;
  FrontendPlayerRuntimeRecord *playerBlock;
  dword *sourceCursor;
  dword *destinationCursor;
  bool allZero;
  RuntimeImagePointerByteSizeEdxEax8 pointerImage;
  StatusValueEaxCf5 upsertStatus;
  ArenaAllocEaxCf5 oldUnitAllocation;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  FileSystemWriteEaxCf5 writeResult;
  ResourceRegistrationImagePair domainImagePair;
  uint upsertFailed;
  
  g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + '\x01';
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount; remainingPlayerBlocks != 0; remainingPlayerBlocks = remainingPlayerBlocks - 1) {
    GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
              (playerBlock->playerRuntimeId,0,0,(playerBlock->factionAssignment).factionAssignmentIndex);
    playerBlock = playerBlock + 1;
  }
  upsertStatus = ResourceRegistration_OpenSourceCf(resourcePath);
  handle = (void *)upsertStatus.valueOrError;
  if (upsertStatus.carry) {
    WidePath_SplitParentAndLeaf((word *)g_PackageScratchBuffer,(word *)THANDOR_ADDR(g_ResourceRegistrationDirectoryUtf16,0),resourcePath);
    upsertStatus = (*g_FileSystemCreateDirectoryRecursiveCf)
                      (FILESYSTEM_CREATE_DIRECTORY_RECURSIVE,(word *)THANDOR_ADDR(g_ResourceRegistrationDirectoryUtf16,0));
    if (upsertStatus.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
    upsertStatus = ResourceRegistration_OpenSourceCf(resourcePath);
    handle = (void *)upsertStatus.valueOrError;
    if (upsertStatus.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  pointerImage = ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs();
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(pointerImage >> 0x20),
                              (dword *)pointerImage,(word *)u_army_hex_0050dfb4,(EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  ArmyRuntimePool_RebaseAfterLoad();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ModelRuntimePool_UnrebaseBeforeSave();
  /* The unrebase returns the model runtime slot image in EAX and its size (0x400000) in EDX;
     the decompiler lost both. */
  modelSlotImage = (dword *)g_ModelRuntimeSlots;
  unpackedSize = 0x400000;
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,unpackedSize,modelSlotImage,
                              (word *)u_modul_hex_0050dfee,(EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  ModelRuntimePool_RebaseAfterLoad();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  domainImagePair = ResourceRegistration_QueryDomain2Pair();
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (dword *)(domainImagePair >> 0x20),(word *)u_shot_hex_0050dfdc,
                              (EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  ShotRuntime_RebaseSlotsAfterLoad();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  domainImagePair = ResourceRegistration_QueryDomain1Pair();
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (dword *)(domainImagePair >> 0x20),(word *)u_effect_hex_0050dfc6,
                              (EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  EffectRuntime_RebaseSlotsAfterLoad();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  domainImagePair = ResourceRegistration_SelectDomainPair(runtimeBase);
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (dword *)(domainImagePair >> 0x20),(word *)u_widget_hex_0050e02a,
                              (EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  ResourceRegistrationRuntime_RebaseLoadedRecords(runtimeBase);
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  pointerImage = RuntimeHexSegment_GetLightImageAndToggleFlagRegs();
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(pointerImage >> 0x20),
                              (dword *)pointerImage,(word *)u_light_hex_0050e016,(EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  RuntimeHexSegment_ToggleLightImageFlag();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  domainImagePair = RuntimeHexSegment_GetFieldImageRegs(runtimeBase);
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (dword *)(domainImagePair >> 0x20),(word *)u_field_hex_0050e002,
                              (EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  RuntimeHexSegment_AfterFieldImageNoOp(runtimeBase);
  sourceData = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ResourceRegistration_ResolveRuntimeRecord(runtimeBase);
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (sourceData->levelImage).header.resourceTables.
                              runtimePrefixByteSizeAndInitialArmyPlacementOffset,(dword *)sourceData
                              ,(word *)u_level_hex_0050e040,(EngineFileHandle)handle);
  if (upsertStatus.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  domainImagePair = ResourceRegistration_QueryDomain0Pair();
  upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (dword *)(domainImagePair >> 0x20),(word *)u_daten_hex_0050e054,
                              (EngineFileHandle)handle);
  upsertFailed = (uint)(upsertStatus.carry & 1);
  GameFactionRuntime_RebaseLoadedArmyReferences();
  if ((upsertFailed & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  if (g_FrontendLoadedCampaignAsset == (dword *)0x0) {
    Package_DeleteEntry((word *)u_campagne_hex_0050e068,(EngineFileHandle)handle);
  }
  else {
    upsertStatus = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,((dword *)(uintptr_t)g_FrontendLoadedCampaignAsset)[1],
                                g_FrontendLoadedCampaignAsset,(word *)u_campagne_hex_0050e068,
                                (EngineFileHandle)handle);
    if (upsertStatus.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,0x38000,g_GameStatTableImage,
                      (word *)u_stat_hex_0050e082,(EngineFileHandle)handle);
  if (g_OldUnitRecordCount == 0) {
    remainingCount = 0x40;
    allZero = true;
    sourceCursor = g_OldUnitSecondaryTable;
    do {
      if (remainingCount == 0) break;
      remainingCount = remainingCount + -1;
      allZero = *sourceCursor == 0;
      sourceCursor = sourceCursor + 1;
    } while (allZero);
    if (!allZero) goto InGameResourceRegistration_SerializeOldUnitTables;
    Package_DeleteEntry((word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
  }
  else {
InGameResourceRegistration_SerializeOldUnitTables:
    oldUnitAllocation = (*g_MemoryApi.alloc)(0x4104);
    sourceCursor = g_OldUnitPrimaryTable;
    oldUnitImage = (dword *)oldUnitAllocation.eax;
    if (oldUnitAllocation.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
    *oldUnitImage = g_OldUnitRecordCount;
    destinationCursor = oldUnitImage;
    for (remainingCount = 0x1000; destinationCursor = destinationCursor + 1, remainingCount != 0; remainingCount = remainingCount + -1) {
      *destinationCursor = *sourceCursor;
      sourceCursor = sourceCursor + 1;
    }
    sourceCursor = g_OldUnitSecondaryTable;
    for (remainingCount = 0x40; remainingCount != 0; remainingCount = remainingCount + -1) {
      *destinationCursor = *sourceCursor;
      sourceCursor = sourceCursor + 1;
      destinationCursor = destinationCursor + 1;
    }
    Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(int)destinationCursor - (int)oldUnitImage,oldUnitImage,
                        (word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
    (*g_MemoryApi.free)(oldUnitImage);
  }
  destination = g_PackageScratchBuffer;
  headerDwords = (dword *)destination; /* EDX: the 0x200-byte package header just read */
  seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
  if ((!seekResult.carry) &&
     (readResult = (*g_FileSystemReadExactCf)(0x200,destination,handle), !readResult.carry)){
    WidePath_SplitParentAndLeaf
              ((word *)(destination + 0x100),(word *)(destination + 0x200),resourcePath);
    localeValue = (*g_LocaleGetPackedCurrentDate)();
    *(dword *)(destination + 0x1f0) = localeValue;
    localeValue = (*g_LocaleGetPackedCurrentTime)();
    *(dword *)(destination + 500) = localeValue;
    localeValue = (*g_LocaleFormatCurrentDateUtf16)((word *)(destination + 0x1c0));
    timeTextDestination = (word *)(localeValue + 4 + (int)(destination + 0x1c0));
    timeTextDestination[-2] = 0x2c; /* ", " between date and time */
    timeTextDestination[-1] = 0x20;
    (*g_LocaleFormatCurrentTimeUtf16)(timeTextDestination);
    localeValue = g_InGameLevelCampaignAssociationIndex;
    if (g_FrontendLoadedCampaignAsset == (dword *)0x0) {
      localeValue = 0xffffffff;
    }
    headerDwords[0x5c] = g_InGameLevelTitleTextResourceIndex;
    headerDwords[100] = localeValue;
    seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
    if ((!seekResult.carry) &&
       (writeResult = (*g_FileSystemWriteExactOrFlushCf)(0x200,headerDwords,handle), !writeResult.carry)){
      Package_Unmount((EngineFileHandle)handle);
      g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + -1;
      return false;
    }
  }
InGameResourceRegistration_DecrementBusyCountAndReturn:
  g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + -1;
  return true;
}


/* Address: 0x0053D9F0.
   Ownership: ui/ingame/runtime.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[0]@005624A0. Queued UI action handler
   for INGAME_PAGE10[0] (0x1000). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   mapControl→InGameMapViewControlAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl)

{
  longlong scaledProduct;
  int xDelta;
  int yComponent;
  
  yComponent = *(int *)(mapControl + 0x6c);
  *(int *)(mapControl + 0x50) = *(int *)(mapControl + 0x68);
  *(int *)(mapControl + 0x54) = yComponent;
  scaledProduct = (longlong)(yComponent + *(int *)(mapControl + 0x68) * 2) * 0x901;
  xDelta = ((int)((ulonglong)scaledProduct >> 0x20) << 0x13 | (uint)scaledProduct >> 0xd) -
          *(int *)(mapControl + -0x8f6c);
  yComponent = ((int)((ulonglong)((longlong)yComponent * -1999) >> 0x20) << 0x14 |
          (uint)((longlong)yComponent * -1999) >> 0xc) - *(int *)(mapControl + -0x8f68);
  *(int *)(mapControl + -0x8f6c) = *(int *)(mapControl + -0x8f6c) + xDelta;
  *(int *)(mapControl + -0x8f68) = *(int *)(mapControl + -0x8f68) + yComponent;
  *(int *)(mapControl + -0x8f8c) = *(int *)(mapControl + -0x8f8c) + xDelta;
  *(int *)(mapControl + -0x8f88) = *(int *)(mapControl + -0x8f88) + yComponent;
  WorldRuntime_ClearFieldGridDirtyFlag((WorldRuntimeContext *)(mapControl + -0x8fec));
  return;
}


/* Address: 0x0055C990.
   Ownership: ui/ingame/runtime.
   Purpose: Initializes the copied in-game UI control-tree runtime for the active display size. It selects
   resolution-specific panel and diagram variants, loads and binds the UI graphics and text resources, and
   populates the large root allocation used by both new-session and loaded-session startup. The archived
   implementation receives one stack argument and reports resource failure through carry.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameUiRuntime_InitializeControlTreeResourcesCf(UiRootNode *inGameRoot)

{
  sdword *sdwordField;
  UiAnchorFractionQ31 *anchorField;
  UiNodeFlags *nodeFlagsField;
  UiNodeBase **nodePointerField;
  UiNodeVtable **vtablePointerField;
  UiRootFlags rootFlagsValue;
  UiNodeFlags nodeFlagsValue;
  UiRootCallbacks *callbacksValue;
  sdword subresourceWidth;
  UiAnchorFractionQ31 firstAnchorValue;
  UiAnchorFractionQ31 secondAnchorValue;
  int detailControlOffset;
  GraphicsTextureSourceAsset *textureSourceValue;
  sdword subresource23Height;
  DirectSoundVoiceSet *buttonVoiceSet;
  UiRootNode *columnOffset;
  UiNodeVtable *sharedLayoutValue;
  word *stream;
  int cellLeft;
  TextResourceId resourceId;
  int columnsRemaining;
  UiRootNode *rowOffset;
  UiNodeBase *offsetValue;
  int stepOffset;
  UiNodeBase *paddedIconHeight;
  int cellTop;
  uint techTextureHeight;
  uint detailIndex;
  GraphicsTextureSourceLoadEaxCf5 loadedTexture;
  TextResourceResolveEaxCf5 resolvedText;
  StatusValueEaxCf5 initStatus;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  GraphicsTextureSourceAsset *loadedTextureSource;
  
  if ((g_FramebufferWidth < 800) || (g_FramebufferHeight < 600)) {
    u_gfx_panel_panel0_gfx_005630d0[0xf] = L'0';
    u_gfx_panel_diagram0_gfx_00563120[0x11] = L'0';
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x20,enum UiRootFlags) = 0x24;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)0x6;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x28,struct UiRootNode *) = (UiRootNode *)0x5e;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)0xd;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x20,dword) = 0x24;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x24,dword) = 0x11;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x28,dword) = 0x5e;
    THANDOR_UI_AT(inGameRoot,0x49FC)->bottomOffset = 0x18;
    THANDOR_UI_AT(inGameRoot,0x4A94)->leftOffset = 0x24;
    THANDOR_UI_AT(inGameRoot,0x4A94)->topOffset = 0x1c;
    THANDOR_UI_AT(inGameRoot,0x4A94)->rightOffset = 0x5e;
    THANDOR_UI_AT(inGameRoot,0x4A94)->bottomOffset = 0x23;
    THANDOR_UI_AT(inGameRoot,0x4B44)->leftOffset = 4;
    THANDOR_UI_AT(inGameRoot,0x4B44)->topOffset = 5;
    THANDOR_UI_AT(inGameRoot,0x4B44)->rightOffset = 0x1f;
    THANDOR_UI_AT(inGameRoot,0x4B44)->bottomOffset = 0xd;
  }
  else if ((g_FramebufferWidth < 0x400) || (g_FramebufferHeight < 0x300)) {
    u_gfx_panel_panel0_gfx_005630d0[0xf] = L'1';
    u_gfx_panel_diagram0_gfx_00563120[0x11] = L'1';
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x20,enum UiRootFlags) = 0x2c;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)0x9;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x28,struct UiRootNode *) = (UiRootNode *)0x6e;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)0x10;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x20,dword) = 0x2c;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x24,dword) = 0x17;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x28,dword) = 0x6e;
    THANDOR_UI_AT(inGameRoot,0x49FC)->bottomOffset = 0x1e;
    THANDOR_UI_AT(inGameRoot,0x4A94)->leftOffset = 0x2c;
    THANDOR_UI_AT(inGameRoot,0x4A94)->topOffset = 0x25;
    THANDOR_UI_AT(inGameRoot,0x4A94)->rightOffset = 0x6e;
    THANDOR_UI_AT(inGameRoot,0x4A94)->bottomOffset = 0x2c;
    THANDOR_UI_AT(inGameRoot,0x4B44)->leftOffset = 4;
    THANDOR_UI_AT(inGameRoot,0x4B44)->topOffset = 7;
    THANDOR_UI_AT(inGameRoot,0x4B44)->rightOffset = 0x27;
    THANDOR_UI_AT(inGameRoot,0x4B44)->bottomOffset = 0xf;
  }
  else {
    u_gfx_panel_panel0_gfx_005630d0[0xf] = L'2';
    u_gfx_panel_diagram0_gfx_00563120[0x11] = L'2';
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x20,enum UiRootFlags) = 0x2c;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)0x9;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x28,struct UiRootNode *) = (UiRootNode *)0x6e;
    THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)0x10;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x20,dword) = 0x2c;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x24,dword) = 0x17;
    THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x28,dword) = 0x6e;
    THANDOR_UI_AT(inGameRoot,0x49FC)->bottomOffset = 0x1e;
    THANDOR_UI_AT(inGameRoot,0x4A94)->leftOffset = 0x2c;
    THANDOR_UI_AT(inGameRoot,0x4A94)->topOffset = 0x25;
    THANDOR_UI_AT(inGameRoot,0x4A94)->rightOffset = 0x6e;
    THANDOR_UI_AT(inGameRoot,0x4A94)->bottomOffset = 0x2c;
    THANDOR_UI_AT(inGameRoot,0x4B44)->leftOffset = 4;
    THANDOR_UI_AT(inGameRoot,0x4B44)->topOffset = 7;
    THANDOR_UI_AT(inGameRoot,0x4B44)->rightOffset = 0x27;
    THANDOR_UI_AT(inGameRoot,0x4B44)->bottomOffset = 0xf;
  }
  loadedTexture = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_panel0_gfx_005630d0);
  textureSourceValue = g_InGamePanelTextureSource;
  loadedTextureSource = loadedTexture.eax;
  if (!loadedTexture.carry) {
    LOCK();
    UNLOCK();
    g_InGamePanelTextureSource = loadedTextureSource;
    (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(textureSourceValue);
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,loadedTextureSource);
    g_InGamePanelTextureSubresource00Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(1,loadedTextureSource);
    g_InGamePanelTextureSubresource01Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(2,loadedTextureSource);
    g_InGamePanelTextureSubresource02Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(6,loadedTextureSource);
    g_InGamePanelTextureSubresource06Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(7,loadedTextureSource);
    g_InGamePanelTextureSubresource07Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x1b,loadedTextureSource);
    g_InGamePanelTextureSubresource27Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x1c,loadedTextureSource);
    g_InGamePanelTextureSubresource28Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,loadedTextureSource);
    g_InGamePanelTextureSubresource19Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x14,loadedTextureSource);
    g_InGamePanelTextureSubresource20Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,loadedTextureSource);
    g_InGamePanelTextureSubresource34Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,loadedTextureSource);
    g_InGamePanelTextureSubresource32Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x21,loadedTextureSource);
    g_InGamePanelTextureSubresource33Width = logicalSize.logicalWidthPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(2,loadedTextureSource);
    g_InGamePanelTextureSubresource02Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(3,loadedTextureSource);
    g_InGamePanelTextureSubresource03Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(4,loadedTextureSource);
    g_InGamePanelTextureSubresource04Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(5,loadedTextureSource);
    g_InGamePanelTextureSubresource05Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x24,loadedTextureSource);
    g_InGamePanelTextureSubresource36Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x25,loadedTextureSource);
    g_InGamePanelTextureSubresource37Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(6,loadedTextureSource);
    g_InGamePanelTextureSubresource06Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,loadedTextureSource);
    g_InGamePanelTextureSubresource00Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(7,loadedTextureSource);
    g_InGamePanelTextureSubresource07Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x1a,loadedTextureSource);
    g_InGamePanelTextureSubresource26Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x1f,loadedTextureSource);
    g_InGamePanelTextureSubresource31Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x12,loadedTextureSource);
    g_InGamePanelTextureSubresource18Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x17,loadedTextureSource);
    g_InGamePanelTextureSubresource23Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,loadedTextureSource);
    g_InGamePanelTextureSubresource34Height = logicalSize.logicalHeightPixels;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,loadedTextureSource);
    textureSourceValue = g_InGamePanelTextureSource;
    g_InGamePanelTextureSubresource32Height = logicalSize.logicalHeightPixels;
    THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x24,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x28,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x2C,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x20,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x24,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x28,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x2C,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x20,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x24,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x28,dword) = 0;
    THANDOR_UI_AT(inGameRoot,0x41BC)->bottomOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x20,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x24,dword) = 0;
    THANDOR_UI_AT(inGameRoot,0x4218)->rightOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x4218)->bottomOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x20,dword) = 0;
    THANDOR_UI_AT(inGameRoot,0x4274)->topOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x4274)->rightOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_AT(inGameRoot,0x42D0)->leftOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x42D0)->topOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x28,enum UiNodeFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x2C,enum UiRootFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x458C + 0x20,dword) = 0;
    THANDOR_UI_AT(inGameRoot,0x458C)->topOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x458C)->rightOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x458C + 0x2C,enum UiNodeFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x458C + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x45E8 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x20,enum UiNodeFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x24,enum UiRootFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x2C,struct UiRootNode *) = (UiRootNode *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x46FC + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x47C4 + 0x50,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x488C + 0x50,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4D78 + 0x50,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5E20 + 0x50,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x76EC + 0x50,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8CB8 + 0x50,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4758 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4BA4 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4820 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4C1C + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x48EC + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4C94 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4400 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4478 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x4D0C + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x98B8 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5DB4 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9930 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7680 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x99A8 + 0x54,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8C4C + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB210 + 0x54,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB310 + 0x54,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB390 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB410 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB688 + 0x54,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB700 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB778 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB7F0 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB8E0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBA48 + 0x54,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBAC0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBB38 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBBB0 + 0x54,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBC28 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBCA0 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD18 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE08 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xBF70 + 0x54,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x20,dword) = 0;
    THANDOR_UI_AT(inGameRoot,0x9A1C)->topOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x9A1C)->rightOffset = 0;
    THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x2C,enum UiNodeFlags) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x20,struct UiRootNode *) = (UiRootNode *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *) = (UiNodeBase *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x28,struct UiNodeBase *) = (UiNodeBase *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x2C,struct UiNodeBase *) = (UiNodeBase *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9AFC + 0x54,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9B60 + 0x54,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9C18 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9DE4 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x20,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x28,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x2C,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0xA0CC + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9F50 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xA678 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xA6D4 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xAFD0 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB02C + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB0E4 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0xB140 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5E7C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5EFC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5F7C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5FFC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x607C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x60FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x617C + 0x74,struct UiRootCallbacks *) = (UiRootCallbacks *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x61FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x627C + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x62FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x637C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x63FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x647C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x64FC + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x657C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x65FC + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x667C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x74,struct UiRootCallbacks *) = (UiRootCallbacks *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x677C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x687C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x68FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x697C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x69FC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6A7C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6AFC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6B7C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6BFC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x74,struct UiRootCallbacks *) = (UiRootCallbacks *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6CFC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6DFC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6E7C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6EFC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6F7C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x6FFC + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x707C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x70FC + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x717C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x74,struct UiRootCallbacks *) = (UiRootCallbacks *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x727C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x737C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x73FC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x747C + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x74FC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x757C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x75FC + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7748 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7848 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x78C8 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7948 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x79C8 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7A48 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x74,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7BC8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x74,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7CC8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7DC8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7E48 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7EC8 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7F48 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x7FC8 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x74,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8148 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x74,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8248 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8348 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x83C8 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8448 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x84C8 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8548 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x74,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x86C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x74,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x87C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x88C8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8948 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x89C8 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8A48 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8AC8 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x74,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8D14 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8D90 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8E0C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x74,struct UiRootNode *) = (UiRootNode *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8F04 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x8FFC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9078 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x90F4 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9170 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9268 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x92E4 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9360 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x93DC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x74,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x94D4 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9550 + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x95CC + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x74,struct UiRootCallbacks *) = (UiRootCallbacks *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9740 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x97BC + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x9838 + 0x74,struct UiNodeVtable *) = (UiNodeVtable *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5A4C + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5A4C + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5AC8 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5AC8 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5B44 + 0x74,enum UiNodeFlags) = (UiNodeFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5B44 + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5BC0 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5BC0 + 0x54,enum UiRootFlags) = (UiRootFlags)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5C3C + 0x74,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5C3C + 0x54,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5CB8 + 0x74,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5CB8 + 0x54,dword) = (UiAnchorFractionQ31)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5D34 + 0x74,sdword) = (sdword)textureSourceValue;
    THANDOR_UI_FIELD(inGameRoot,0x5D34 + 0x54,struct UiNodeBase *) = (UiNodeBase *)textureSourceValue;
    subresourceWidth = g_InGamePanelTextureSubresource01Width;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    *sdwordField = *sdwordField - g_InGamePanelTextureSubresource01Width;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x28,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x28,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4218)->rightOffset;
    *sdwordField = *sdwordField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4274)->rightOffset;
    *sdwordField = *sdwordField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x42D0)->leftOffset;
    *sdwordField = *sdwordField - subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x28,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x9A1C)->rightOffset;
    *sdwordField = *sdwordField - subresourceWidth;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x20,struct UiRootNode *) = (UiRootNode *)((int)THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x20,struct UiRootNode *) - subresourceWidth);
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x28,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)*nodePointerField - subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x28,dword);
    *anchorField = *anchorField - subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource02Width;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    *sdwordField = *sdwordField - g_InGamePanelTextureSubresource02Width;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4104 + 0x28,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x41BC + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x42D0)->leftOffset;
    *sdwordField = *sdwordField - subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x20,struct UiRootNode *) = (UiRootNode *)((int)THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x20,struct UiRootNode *) - subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x20,dword);
    *anchorField = *anchorField - subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    *sdwordField = *sdwordField - g_InGamePanelTextureSubresource00Width;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x458C + 0x20,dword);
    *anchorField = *anchorField - g_InGamePanelTextureSubresource06Width;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x20,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - g_InGamePanelTextureSubresource07Width;
    subresourceWidth = g_InGamePanelTextureSubresource02Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x41BC)->bottomOffset;
    *sdwordField = *sdwordField + g_InGamePanelTextureSubresource02Height;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4218)->bottomOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4274)->topOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x9A1C)->topOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *) =
         (UiNodeBase *)((int)&(THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *))->nextSibling + subresourceWidth);
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x2C,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)&(*nodePointerField)->nextSibling + subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource36Height;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x24,dword);
    *anchorField = *anchorField + g_InGamePanelTextureSubresource36Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4218)->bottomOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4274)->topOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x9A1C + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *) =
         (UiNodeBase *)((int)&(THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *))->nextSibling + subresourceWidth);
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x2C,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)&(*nodePointerField)->nextSibling + subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource03Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4218)->bottomOffset;
    *sdwordField = *sdwordField + g_InGamePanelTextureSubresource03Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4274)->topOffset;
    *sdwordField = *sdwordField + subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *) =
         (UiNodeBase *)((int)&(THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x24,struct UiNodeBase *))->nextSibling + subresourceWidth);
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x2C,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)&(*nodePointerField)->nextSibling + subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource37Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x4274)->topOffset;
    *sdwordField = *sdwordField + g_InGamePanelTextureSubresource37Height;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x9A8C + 0x2C,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)&(*nodePointerField)->nextSibling + subresourceWidth);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource04Height;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + g_InGamePanelTextureSubresource04Height;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x24,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource00Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x42D0)->topOffset;
    *sdwordField = *sdwordField + g_InGamePanelTextureSubresource00Height;
    THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x2C,enum UiRootFlags) = THANDOR_UI_FIELD(inGameRoot,0x42D0 + 0x2C,enum UiRootFlags) + subresourceWidth;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x2C,dword);
    *anchorField = *anchorField + subresourceWidth;
    subresourceWidth = g_InGamePanelTextureSubresource05Height;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x42D0)->topOffset;
    *sdwordField = *sdwordField - g_InGamePanelTextureSubresource05Height;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x9EE0 + 0x2C,dword);
    *anchorField = *anchorField - subresourceWidth;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x458C + 0x2C,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + g_InGamePanelTextureSubresource06Height;
    THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x24,enum UiRootFlags) =
         THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x24,enum UiRootFlags) - g_InGamePanelTextureSubresource07Height;
    rootFlagsValue = THANDOR_UI_FIELD(inGameRoot,0x4218 + 0x24,dword);
    columnOffset = (UiRootNode *)THANDOR_UI_AT(inGameRoot,0x4218)->bottomOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x24,enum UiRootFlags) = rootFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x2C,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x4400)->topOffset = rootFlagsValue;
    THANDOR_UI_AT(inGameRoot,0x4400)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4478 + 0x24,dword) = rootFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0x4478 + 0x2C,dword) = (UiAnchorFractionQ31)columnOffset;
    nodeFlagsValue = THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    callbacksValue = (UiRootCallbacks *)THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x28,dword);
    THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x20,enum UiNodeFlags) = nodeFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x28,struct UiRootCallbacks *) = callbacksValue;
    THANDOR_UI_AT(inGameRoot,0x4400)->leftOffset = nodeFlagsValue;
    THANDOR_UI_AT(inGameRoot,0x4400)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0x4478 + 0x20,dword) = nodeFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0x4478 + 0x28,dword) = (UiAnchorFractionQ31)callbacksValue;
    sharedLayoutValue = (UiNodeVtable *)THANDOR_UI_AT(inGameRoot,0x4274)->topOffset;
    columnOffset = (UiRootNode *)THANDOR_UI_FIELD(inGameRoot,0x4274 + 0x2C,enum UiNodeFlags);
    THANDOR_UI_AT(inGameRoot,0xB210)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xB210)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x24,enum UiRootFlags) = (UiRootFlags)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x2C,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0xB310)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xB310)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xB390)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xB390 + 0x2C,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xB410)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xB410)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x24,dword) = (UiAnchorFractionQ31)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xB490)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x24,struct UiNodeVtable *) = sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xB510)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x24,dword) = (UiAnchorFractionQ31)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x2C,dword) = (UiAnchorFractionQ31)columnOffset;
    offsetValue = (UiNodeBase *)THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    callbacksValue = (UiRootCallbacks *)THANDOR_UI_FIELD(inGameRoot,0x4160 + 0x28,dword);
    THANDOR_UI_AT(inGameRoot,0xB210)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB210)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x20,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x28,struct UiRootCallbacks *) = callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xB310)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB310)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xB390)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB390 + 0x28,enum UiNodeFlags) = (UiNodeFlags)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xB410)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB410)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB490)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB510)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x28,dword) = (UiAnchorFractionQ31)callbacksValue;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB210)->leftOffset;
    *sdwordField = *sdwordField + 5;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x20,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_HAS_KEYBOARD_FOCUS);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB310)->leftOffset;
    *sdwordField = *sdwordField + 0x42;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB390)->leftOffset;
    *sdwordField = *sdwordField + 0x61;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB410)->leftOffset;
    *sdwordField = *sdwordField + 5;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x20,dword);
    *anchorField = *anchorField + 0x24;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x20,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)&(*nodePointerField)->layoutWidth + 2);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x20,dword);
    *anchorField = *anchorField + 0x61;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB210)->topOffset;
    *sdwordField = *sdwordField + 0x11;
    THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x24,enum UiRootFlags) = THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x24,enum UiRootFlags) + 0x11;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB310)->topOffset;
    *sdwordField = *sdwordField + 0x11;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB390)->topOffset;
    *sdwordField = *sdwordField + 0x11;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0xB410)->topOffset;
    *sdwordField = *sdwordField + 0x28;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x24,dword);
    *anchorField = *anchorField + 0x28;
    vtablePointerField = &THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x24,struct UiNodeVtable *);
    *vtablePointerField = (UiNodeVtable *)&(*vtablePointerField)->pointerMove;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x24,dword);
    *anchorField = *anchorField + 0x28;
    sharedLayoutValue = (UiNodeVtable *)THANDOR_UI_AT(inGameRoot,0x4104)->leftOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9DC + 0x20,struct UiNodeBase *) = (UiNodeBase *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9DC + 0x24,struct UiNodeBase *) = (UiNodeBase *)0x0;
    THANDOR_UI_FIELD(inGameRoot,0x9DC + 0x28,struct UiNodeVtable *) = sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0x9DC)->bottomOffset = 0;
    THANDOR_UI_AT(inGameRoot,0x40AC)->leftOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0x40AC + 0x24,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x40AC + 0x28,dword) = 0;
    THANDOR_UI_FIELD(inGameRoot,0x40AC + 0x2C,dword) = 0;
    columnOffset = (UiRootNode *)-g_InGamePanelTextureSubresource28Width;
    rowOffset = (UiRootNode *)-g_InGamePanelTextureSubresource31Height;
    THANDOR_UI_AT(inGameRoot,0x5E7C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x5E7C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x5EFC + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x5F7C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x5FFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x607C + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x627C + 0x28,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x647C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x667C + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x687C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6A7C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x707C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x737C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7748)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7748)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x2C,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7848)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x78C8 + 0x2C,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7948)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7F48)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8148 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8348)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8548)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x88C8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8D14 + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8D14 + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8D90)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8E0C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8F04)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x90F4 + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x92E4)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x94D4)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    columnOffset = (UiRootNode *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    rowOffset = (UiRootNode *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_AT(inGameRoot,0x5E7C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x5E7C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x5EFC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x5EFC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x5F7C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x5FFC + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x607C + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x607C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x60FC + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x60FC + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x617C + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x61FC + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x627C + 0x20,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x62FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x647C + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x64FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x667C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x687C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x68FC + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6A7C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6AFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6DFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x707C + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x70FC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x737C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x73FC + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7748)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7748)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x24,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7848)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x78C8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7948)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7948)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x79C8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x79C8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7A48)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7BC8 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x20,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7DC8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7F48 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7FC8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8148)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8348)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x83C8 + 0x28,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8548 + 0x24,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x88C8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8948 + 0x2C,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8D14)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8D14 + 0x24,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8D90)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8D90)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E0C + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8F04)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8F04 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x2C,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8FFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9078)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x90F4 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9170 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x92E4 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9360)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x94D4)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9550 + 0x28,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x20,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9740)->rightOffset = (sdword)columnOffset;
    columnOffset = (UiRootNode *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    offsetValue = (UiNodeBase *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_FIELD(inGameRoot,0x5EFC + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x5F7C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x607C + 0x24,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0x60FC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x60FC + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x617C + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x617C + 0x24,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0x61FC)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x627C + 0x2C,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0x62FC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x62FC)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x637C + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x637C + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x63FC)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x64FC + 0x20,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x657C + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x20,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x677C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x68FC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x697C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6AFC + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x6B7C)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x6DFC)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6E7C + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x70FC + 0x24,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x717C + 0x2C,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x73FC + 0x24,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x747C)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x20,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7848)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7948)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x79C8 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x79C8 + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7A48)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7A48 + 0x24,struct UiNodeVtable *) = (UiNodeVtable *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7BC8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7BC8 + 0x2C,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x2C,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7CC8)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7DC8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7E48 + 0x28,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7FC8 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8248)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x83C8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8448)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x8948)->topOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x89C8)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x8D90)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E0C + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8F04)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x20,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x24,enum UiRootFlags) = (UiRootFlags)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x8FFC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8FFC)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x9078 + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x90F4)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x9170)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9170 + 0x2C,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x28,struct UiRootNode *) = columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x2C,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_AT(inGameRoot,0x9268)->bottomOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x9360 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x93DC + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9550)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x95CC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9740)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x97BC + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    columnOffset = (UiRootNode *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    rowOffset = (UiRootNode *)((int)offsetValue - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_FIELD(inGameRoot,0x5F7C + 0x20,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x5FFC + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x617C + 0x20,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x61FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x627C + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x62FC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x637C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x637C + 0x24,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x63FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x63FC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x647C + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x64FC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x657C + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x657C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x65FC + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x65FC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x677C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x28,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x697C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x69FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6B7C + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6BFC + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6E7C + 0x24,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6EFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x717C + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x747C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x74FC + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7848)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x78C8 + 0x28,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7A48 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7BC8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x24,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7CC8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7CC8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x2C,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7DC8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7E48)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7E48 + 0x2C,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7EC8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x7EC8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8248)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8448)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x84C8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x86C8 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x89C8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8A48)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E0C + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8FFC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9078)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x90F4 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9170)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x20,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9268)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9268)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x92E4 + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9360)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x93DC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x93DC + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x95CC + 0x20,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x97BC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x9838)->rightOffset = (sdword)columnOffset;
    offsetValue = (UiNodeBase *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    rowOffset = (UiRootNode *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_FIELD(inGameRoot,0x5FFC + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x61FC)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x63FC)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x647C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x64FC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x657C + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x65FC + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x65FC + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x667C + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x677C + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x20,enum UiRootFlags) = (UiRootFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x69FC + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x6A7C)->rightOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6AFC + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6B7C + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6BFC + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6BFC + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6CFC)->rightOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x6EFC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6F7C + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x727C + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x74FC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x757C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x78C8)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7CC8)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x24,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7DC8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7E48)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7EC8)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x7EC8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7F48)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x7FC8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x20,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x84C8 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x8548)->rightOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x86C8 + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x86C8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x87C8)->rightOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8A48 + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8AC8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x9078 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x9268)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x92E4)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9360 + 0x24,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x93DC + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x24,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x94D4)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9550 + 0x2C,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x95CC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9838)->leftOffset = (sdword)offsetValue;
    columnOffset = (UiRootNode *)((int)offsetValue - g_InGamePanelTextureSubresource34Width);
    rowOffset = (UiRootNode *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_FIELD(inGameRoot,0x667C + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x677C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x687C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x68FC + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x697C)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x69FC + 0x2C,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6A7C + 0x20,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6AFC + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6B7C + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6BFC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x20,struct UiRootNode *) = columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6CFC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6CFC + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x28,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6DFC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6E7C + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6EFC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6F7C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6F7C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6FFC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6FFC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x727C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x757C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x75FC)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7F48 + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x7FC8 + 0x24,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8148 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8248)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x2C,struct UiRootNode *) = rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8548 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x86C8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x24,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x87C8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x87C8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x20,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x88C8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8948 + 0x28,enum UiNodeFlags) = (UiNodeFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x89C8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8A48)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8AC8)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8AC8 + 0x24,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x2C,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x94D4)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9550)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x95CC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9740)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x97BC + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9838)->bottomOffset = (sdword)rowOffset;
    columnOffset = (UiRootNode *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    rowOffset = (UiRootNode *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_AT(inGameRoot,0x687C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x68FC + 0x24,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x697C)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x69FC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x6CFC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x20,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6DFC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6E7C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6EFC)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6F7C + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x6FFC + 0x20,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x6FFC)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x707C + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x70FC + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x717C + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x28,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x727C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x28,struct UiRootNode *) = columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x75FC + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8148)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x24,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8248)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x24,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8348)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x83C8 + 0x2C,enum UiRootFlags) = (UiRootFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x8448)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x84C8)->bottomOffset = (sdword)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x87C8)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x2C,struct UiRootNode *) = rowOffset;
    THANDOR_UI_AT(inGameRoot,0x88C8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8948)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x89C8)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8A48 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8AC8 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x24,dword) = (UiAnchorFractionQ31)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x24,struct UiNodeBase *) = (UiNodeBase *)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9740)->topOffset = (sdword)rowOffset;
    THANDOR_UI_FIELD(inGameRoot,0x97BC + 0x24,enum UiNodeFlags) = (UiNodeFlags)rowOffset;
    THANDOR_UI_AT(inGameRoot,0x9838)->topOffset = (sdword)rowOffset;
    columnOffset = (UiRootNode *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    offsetValue = (UiNodeBase *)((int)rowOffset - g_InGamePanelTextureSubresource34Height);
    THANDOR_UI_FIELD(inGameRoot,0x707C + 0x20,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x70FC + 0x20,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x717C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x20,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0x727C)->leftOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x20,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x737C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x73FC + 0x28,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x747C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x74FC)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x757C)->rightOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0x75FC + 0x28,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_AT(inGameRoot,0x8348)->topOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x83C8)->topOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0x8448)->topOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x84C8 + 0x24,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x24,enum UiRootFlags) = (UiRootFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x24,struct UiNodeBase *) = offsetValue;
    sharedLayoutValue = (UiNodeVtable *)((int)columnOffset - g_InGamePanelTextureSubresource34Width);
    THANDOR_UI_AT(inGameRoot,0x737C)->leftOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0x73FC)->leftOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0x747C)->leftOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0x74FC + 0x20,dword) = (UiAnchorFractionQ31)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0x757C + 0x20,struct UiNodeVtable *) = sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0x75FC + 0x20,dword) = (UiAnchorFractionQ31)sharedLayoutValue;
    subresourceWidth = g_InGamePanelTextureSubresource34Width;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x1AF4)->leftOffset;
    *sdwordField = *sdwordField + g_InGamePanelTextureSubresource34Width;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x1A98 + 0x28,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField + subresourceWidth;
    subresource23Height = g_InGamePanelTextureSubresource23Height;
    subresourceWidth = g_InGamePanelTextureSubresource19Width;
    rootFlagsValue = -g_InGamePanelTextureSubresource20Width;
    THANDOR_UI_FIELD(inGameRoot,0x4DD4 + 0x2C,struct UiRootCallbacks *) =
         (UiRootCallbacks *)
         ((int)THANDOR_UI_FIELD(inGameRoot,0x4DD4 + 0x2C,struct UiRootCallbacks *) - g_InGamePanelTextureSubresource23Height);
    THANDOR_UI_AT(inGameRoot,0x4DD4)->leftOffset = subresourceWidth;
    stepOffset = subresource23Height + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4DD4 + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4DD4 + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4E2C + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4E2C + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4E2C)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4E2C + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4E2C + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4E84 + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4E84 + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4E84)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4E84 + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4E84 + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4EDC + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4EDC + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4EDC)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4EDC + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4EDC + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4F34 + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4F34 + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4F34)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4F34 + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4F34 + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4F8C + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4F8C + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4F8C)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4F8C + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4F8C + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    THANDOR_UI_FIELD(inGameRoot,0x4FE4 + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x4FE4 + 0x2C,struct UiRootCallbacks *) - stepOffset);
    THANDOR_UI_AT(inGameRoot,0x4FE4)->leftOffset = subresourceWidth;
    stepOffset = stepOffset + g_InGamePanelTextureSubresource32Height;
    THANDOR_UI_FIELD(inGameRoot,0x4FE4 + 0x28,enum UiRootFlags) = rootFlagsValue;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x4FE4 + 0x24,enum UiNodeFlags);
    *nodeFlagsField = *nodeFlagsField - stepOffset;
    offsetValue = (UiNodeBase *)-g_InGamePanelTextureSubresource33Width;
    THANDOR_UI_FIELD(inGameRoot,0x5A4C + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x503C + 0x28,enum UiRootFlags) = (int)&offsetValue->nextSibling + THANDOR_UI_FIELD(inGameRoot,0x503C + 0x28,enum UiRootFlags);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x5544)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x57C8 + 0x28,enum UiNodeFlags);
    *nodeFlagsField = (int)&offsetValue->nextSibling + *nodeFlagsField;
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x52C0)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_AT(inGameRoot,0x5AC8)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x5098 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x5098 + 0x28,struct UiRootCallbacks *) + (int)offsetValue)
    ;
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x55A0 + 0x28,dword);
    *anchorField = (int)&offsetValue->nextSibling + *anchorField;
    THANDOR_UI_FIELD(inGameRoot,0x5824 + 0x28,enum UiRootFlags) = (int)&offsetValue->nextSibling + THANDOR_UI_FIELD(inGameRoot,0x5824 + 0x28,enum UiRootFlags);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x531C)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_FIELD(inGameRoot,0x5B44 + 0x20,enum UiRootFlags) = (UiRootFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x50F4 + 0x28,struct UiRootNode *) =
         (UiRootNode *)((int)THANDOR_UI_FIELD(inGameRoot,0x50F4 + 0x28,struct UiRootNode *) + (int)offsetValue);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x55FC + 0x28,dword);
    *anchorField = (int)&offsetValue->nextSibling + *anchorField;
    THANDOR_UI_FIELD(inGameRoot,0x5880 + 0x28,struct UiRootCallbacks *) =
         (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x5880 + 0x28,struct UiRootCallbacks *) + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x5378)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_AT(inGameRoot,0x5BC0)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0x5150 + 0x28,struct UiNodeBase *) =
         (UiNodeBase *)((int)&offsetValue->nextSibling + (int)THANDOR_UI_FIELD(inGameRoot,0x5150 + 0x28,struct UiNodeBase *));
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x5658 + 0x28,dword);
    *anchorField = (int)&offsetValue->nextSibling + *anchorField;
    THANDOR_UI_FIELD(inGameRoot,0x58DC + 0x28,struct UiRootNode *) =
         (UiRootNode *)((int)THANDOR_UI_FIELD(inGameRoot,0x58DC + 0x28,struct UiRootNode *) + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x53D4)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_FIELD(inGameRoot,0x5C3C + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x51AC + 0x28,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)*nodePointerField + (int)offsetValue);
    anchorField = &THANDOR_UI_FIELD(inGameRoot,0x56B4 + 0x28,dword);
    *anchorField = (int)&offsetValue->nextSibling + *anchorField;
    THANDOR_UI_FIELD(inGameRoot,0x5938 + 0x28,struct UiNodeBase *) =
         (UiNodeBase *)((int)&offsetValue->nextSibling + (int)THANDOR_UI_FIELD(inGameRoot,0x5938 + 0x28,struct UiNodeBase *));
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x5430)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_FIELD(inGameRoot,0x5CB8 + 0x20,struct UiNodeBase *) = offsetValue;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x5208 + 0x28,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)*nodePointerField + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x5710)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x5994 + 0x28,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)*nodePointerField + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x548C)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    THANDOR_UI_AT(inGameRoot,0x5D34)->leftOffset = (sdword)offsetValue;
    vtablePointerField = &THANDOR_UI_FIELD(inGameRoot,0x5264 + 0x28,struct UiNodeVtable *);
    *vtablePointerField = (UiNodeVtable *)((int)*vtablePointerField + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x576C)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    nodePointerField = &THANDOR_UI_FIELD(inGameRoot,0x59F0 + 0x28,struct UiNodeBase *);
    *nodePointerField = (UiNodeBase *)((int)*nodePointerField + (int)offsetValue);
    sdwordField = &THANDOR_UI_AT(inGameRoot,0x54E8)->rightOffset;
    *sdwordField = (int)&offsetValue->nextSibling + *sdwordField;
    subresourceWidth = THANDOR_UI_AT(inGameRoot,0x458C)->topOffset;
    THANDOR_UI_AT(inGameRoot,0x45E8)->leftOffset = THANDOR_UI_FIELD(inGameRoot,0x458C + 0x20,dword);
    THANDOR_UI_AT(inGameRoot,0x45E8)->topOffset = subresourceWidth;
    rootFlagsValue = THANDOR_UI_FIELD(inGameRoot,0x458C + 0x2C,enum UiNodeFlags);
    THANDOR_UI_FIELD(inGameRoot,0x45E8 + 0x28,enum UiNodeFlags) = THANDOR_UI_AT(inGameRoot,0x458C)->rightOffset;
    THANDOR_UI_FIELD(inGameRoot,0x45E8 + 0x2C,enum UiRootFlags) = rootFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0x46FC + 0x20,enum UiRootFlags) = THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x20,enum UiNodeFlags);
    THANDOR_UI_FIELD(inGameRoot,0x46FC + 0x24,struct UiRootCallbacks *) = (UiRootCallbacks *)THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x24,enum UiRootFlags);
    THANDOR_UI_FIELD(inGameRoot,0x46FC + 0x28,struct UiRootNode *) = (UiRootNode *)THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x28,struct UiRootCallbacks *);
    THANDOR_UI_FIELD(inGameRoot,0x46FC + 0x2C,struct UiNodeBase *) = &(THANDOR_UI_FIELD(inGameRoot,0x46A0 + 0x2C,struct UiRootNode *))->base;
    offsetValue = (UiNodeBase *)THANDOR_UI_AT(inGameRoot,0xB210)->leftOffset;
    columnOffset = (UiRootNode *)THANDOR_UI_AT(inGameRoot,0xB210)->topOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x24,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xB7F0)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB7F0)->topOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x24,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0xBC28)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xBC28)->topOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x20,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x24,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x24,struct UiRootNode *) = columnOffset;
    offsetValue = (UiNodeBase *)THANDOR_UI_AT(inGameRoot,0xB210)->rightOffset;
    columnOffset = (UiRootNode *)THANDOR_UI_AT(inGameRoot,0xB210)->bottomOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x2C,struct UiNodeVtable *) = (UiNodeVtable *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB7F0 + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB7F0 + 0x2C,dword) = (UiAnchorFractionQ31)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBC28 + 0x28,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBC28 + 0x2C,enum UiRootFlags) = (UiRootFlags)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x28,struct UiRootCallbacks *) = (UiRootCallbacks *)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x2C,struct UiRootNode *) = columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x2C,struct UiNodeBase *) = (UiNodeBase *)columnOffset;
    offsetValue = (UiNodeBase *)THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x20,enum UiNodeFlags);
    sharedLayoutValue = (UiNodeVtable *)THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x24,enum UiRootFlags);
    THANDOR_UI_AT(inGameRoot,0xB688)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xB688)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x20,enum UiNodeFlags) = (UiNodeFlags)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x24,enum UiRootFlags) = (UiRootFlags)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xBA48)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xBA48)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xBCA0 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xBCA0 + 0x24,struct UiNodeVtable *) = sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xBE08)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xBE08)->topOffset = (sdword)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xBF70)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xBF70)->topOffset = (sdword)sharedLayoutValue;
    callbacksValue = THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x28,struct UiRootCallbacks *);
    columnOffset = THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x2C,struct UiRootNode *);
    THANDOR_UI_AT(inGameRoot,0xB688)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xB688)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x28,struct UiRootCallbacks *) = callbacksValue;
    THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x2C,struct UiRootNode *) = columnOffset;
    THANDOR_UI_AT(inGameRoot,0xBA48)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xBA48)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xBCA0)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xBCA0)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xBE08)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xBE08)->bottomOffset = (sdword)columnOffset;
    THANDOR_UI_AT(inGameRoot,0xBF70)->rightOffset = (sdword)callbacksValue;
    THANDOR_UI_AT(inGameRoot,0xBF70)->bottomOffset = (sdword)columnOffset;
    firstAnchorValue = THANDOR_UI_AT(inGameRoot,0xB310)->leftOffset;
    secondAnchorValue = THANDOR_UI_AT(inGameRoot,0xB310)->topOffset;
    THANDOR_UI_AT(inGameRoot,0xB700)->leftOffset = firstAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xB700)->topOffset = secondAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xB8E0)->leftOffset = firstAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xB8E0)->topOffset = secondAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xBAC0 + 0x20,dword) = firstAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xBAC0 + 0x24,dword) = secondAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xBD18)->leftOffset = firstAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xBD18)->topOffset = secondAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x20,dword) = firstAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x24,dword) = secondAnchorValue;
    nodeFlagsValue = THANDOR_UI_AT(inGameRoot,0xB310)->rightOffset;
    rootFlagsValue = THANDOR_UI_AT(inGameRoot,0xB310)->bottomOffset;
    THANDOR_UI_FIELD(inGameRoot,0xB700 + 0x28,enum UiNodeFlags) = nodeFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0xB700 + 0x2C,enum UiRootFlags) = rootFlagsValue;
    THANDOR_UI_AT(inGameRoot,0xB8E0)->rightOffset = nodeFlagsValue;
    THANDOR_UI_AT(inGameRoot,0xB8E0)->bottomOffset = rootFlagsValue;
    THANDOR_UI_AT(inGameRoot,0xBAC0)->rightOffset = nodeFlagsValue;
    THANDOR_UI_AT(inGameRoot,0xBAC0)->bottomOffset = rootFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD18 + 0x28,dword) = nodeFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0xBD18 + 0x2C,dword) = rootFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x28,dword) = nodeFlagsValue;
    THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x2C,dword) = rootFlagsValue;
    offsetValue = (UiNodeBase *)THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x24,struct UiNodeVtable *);
    THANDOR_UI_FIELD(inGameRoot,0xBB38 + 0x20,struct UiNodeBase *) = THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x20,struct UiNodeBase *);
    THANDOR_UI_FIELD(inGameRoot,0xBB38 + 0x24,struct UiNodeBase *) = offsetValue;
    sharedLayoutValue = (UiNodeVtable *)THANDOR_UI_AT(inGameRoot,0xB510)->bottomOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBB38 + 0x28,struct UiNodeBase *) = (UiNodeBase *)THANDOR_UI_AT(inGameRoot,0xB510)->rightOffset;
    THANDOR_UI_FIELD(inGameRoot,0xBB38 + 0x2C,struct UiNodeVtable *) = sharedLayoutValue;
    offsetValue = (UiNodeBase *)THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x20,dword);
    sharedLayoutValue = (UiNodeVtable *)THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x24,dword);
    THANDOR_UI_FIELD(inGameRoot,0xB778 + 0x20,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB778 + 0x24,struct UiNodeVtable *) = sharedLayoutValue;
    THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x20,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x24,dword) = (UiAnchorFractionQ31)sharedLayoutValue;
    THANDOR_UI_AT(inGameRoot,0xBBB0)->leftOffset = (sdword)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xBBB0)->topOffset = (sdword)sharedLayoutValue;
    firstAnchorValue = THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x28,dword);
    secondAnchorValue = THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x2C,dword);
    THANDOR_UI_AT(inGameRoot,0xB778)->rightOffset = firstAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xB778)->bottomOffset = secondAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x28,dword) = firstAnchorValue;
    THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x2C,dword) = secondAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xBBB0)->rightOffset = firstAnchorValue;
    THANDOR_UI_AT(inGameRoot,0xBBB0)->bottomOffset = secondAnchorValue;
    subresourceWidth = g_InGamePanelTextureSubresource34Height;
    offsetValue = (UiNodeBase *)(g_InGamePanelTextureSubresource34Width + 2);
    paddedIconHeight = (UiNodeBase *)(g_InGamePanelTextureSubresource34Height + 2);
    THANDOR_UI_FIELD(inGameRoot,0xA00C + 0x28,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xA00C + 0x2C,struct UiNodeBase *) = paddedIconHeight;
    THANDOR_UI_FIELD(inGameRoot,0xA00C + 0x20,struct UiRootNode *) = (UiRootNode *)0x2;
    THANDOR_UI_FIELD(inGameRoot,0xA00C + 0x24,struct UiNodeBase *) = (UiNodeBase *)0x2;
    THANDOR_UI_FIELD(inGameRoot,0xA140 + 0x28,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xA140 + 0x2C,dword) = (UiAnchorFractionQ31)paddedIconHeight;
    THANDOR_UI_AT(inGameRoot,0xA140)->leftOffset = 2;
    THANDOR_UI_AT(inGameRoot,0xA140)->topOffset = 2;
    offsetValue = (UiNodeBase *)(g_InGamePanelTextureSubresource02Width - 4);
    THANDOR_UI_FIELD(inGameRoot,0xA06C + 0x20,struct UiNodeBase *) = (UiNodeBase *)0x2;
    THANDOR_UI_FIELD(inGameRoot,0xA06C + 0x24,struct UiNodeBase *) = (UiNodeBase *)(subresourceWidth + 4);
    THANDOR_UI_FIELD(inGameRoot,0xA06C + 0x28,struct UiNodeVtable *) = (UiNodeVtable *)0xfffffffe;
    THANDOR_UI_FIELD(inGameRoot,0xA06C + 0x50,dword) = (UiAnchorFractionQ31)offsetValue;
    THANDOR_UI_AT(inGameRoot,0xA19C)->leftOffset = 2;
    THANDOR_UI_FIELD(inGameRoot,0xA19C + 0x24,dword) = (UiAnchorFractionQ31)(subresourceWidth + 4);
    THANDOR_UI_FIELD(inGameRoot,0xA19C + 0x28,dword) = 0xfffffffe;
    THANDOR_UI_FIELD(inGameRoot,0xA19C + 0x50,struct UiNodeBase *) = offsetValue;
    THANDOR_UI_FIELD(inGameRoot,0xB088 + 0x20,struct UiRootCallbacks *) = (UiRootCallbacks *)0x2;
    THANDOR_UI_FIELD(inGameRoot,0xB088 + 0x24,struct UiRootNode *) = (UiRootNode *)0x2;
    THANDOR_UI_FIELD(inGameRoot,0xB088 + 0x28,struct UiNodeBase *) = (UiNodeBase *)0xfffffffe;
    THANDOR_UI_FIELD(inGameRoot,0xB088 + 0x50,sdword) = (sdword)offsetValue;
    resourceId = 0x18002c;
    do {
      resolvedText = TextResource_Resolve(resourceId);
      stream = resolvedText.eax;
      resourceId = resourceId + 1;
      RichTextCommandStream_PatchPayloadBySelector(0,g_InGameSelectionDetailNameTextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,g_InGameSelectionDetailArmourTextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (2,g_InGameSelectionDetailWeaponName0TextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (3,g_InGameSelectionDetailWeaponName1TextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (4,g_InGameSelectionDetailWeaponName2TextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(5,g_InGameSelectionDetailTextSlot05Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (6,g_InGameSelectionDetailBuildXeniteCostTextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (7,g_InGameSelectionDetailBuildTimeTextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(8,g_InGameSelectionDetailEnergyTextUtf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(9,g_InGameSelectionDetailTextSlot09Utf16,stream);
    } while (resourceId < 0x18004f);
    columnsRemaining = 3;
    stepOffset = (int)((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
    detailIndex = 0;
    cellLeft = 0;
    cellTop = 0;
    do {
      detailControlOffset = g_InGameSelectionDetailGridCellOffsets[detailIndex];
      *(int *)((int)&(inGameRoot->base).leftOffset + detailControlOffset) = cellLeft;
      *(int *)((int)&(inGameRoot->base).topOffset + detailControlOffset) = cellTop;
      cellLeft = cellLeft + stepOffset;
      cellTop = cellTop + stepOffset;
      *(int *)((int)&(inGameRoot->base).rightOffset + detailControlOffset) = cellLeft;
      *(int *)((int)&(inGameRoot->base).bottomOffset + detailControlOffset) = cellTop;
      detailIndex = detailIndex + 1;
      columnsRemaining = columnsRemaining + -1;
      if (columnsRemaining == 0) {
        columnsRemaining = 3;
        cellLeft = 0;
      }
      else {
        cellTop = cellTop - stepOffset;
      }
    } while (detailIndex < 0xc);
    loadedTexture = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_diagram0_gfx_00563120);
    textureSourceValue = g_InGameDiagramTextureSource;
    loadedTextureSource = loadedTexture.eax;
    if (!loadedTexture.carry) {
      LOCK();
      UNLOCK();
      g_InGameDiagramTextureSource = loadedTextureSource;
      (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(textureSourceValue);
      THANDOR_UI_FIELD(inGameRoot,0x4964 + 0x58,sdword) = (sdword)loadedTextureSource;
      THANDOR_UI_FIELD(inGameRoot,0x49FC + 0x58,sdword) = (sdword)loadedTextureSource;
      THANDOR_UI_FIELD(inGameRoot,0x4A94 + 0x58,struct UiRootNode *) = (UiRootNode *)loadedTextureSource;
      loadedTexture = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_window_gfx_0056318e);
      textureSourceValue = g_InGameWindowTextureSource;
      loadedTextureSource = loadedTexture.eax;
      if (!loadedTexture.carry) {
        LOCK();
        UNLOCK();
        g_InGameWindowTextureSource = loadedTextureSource;
        (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(textureSourceValue);
        THANDOR_UI_FIELD(inGameRoot,0xE6C + 0x54,struct UiRootCallbacks *) = (UiRootCallbacks *)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0x1BE0 + 0x54,struct UiNodeBase *) = (UiNodeBase *)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xC44 + 0x54,dword) = (UiAnchorFractionQ31)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xCA0 + 0x54,dword) = (UiAnchorFractionQ31)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xCFC + 0x54,sdword) = (sdword)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xD58 + 0x54,sdword) = (sdword)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xDB4 + 0x54,enum UiNodeFlags) = (UiNodeFlags)loadedTextureSource;
        THANDOR_UI_FIELD(inGameRoot,0xE10 + 0x54,enum UiRootFlags) = (UiRootFlags)loadedTextureSource;
        loadedTexture = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_tech_gfx_005630fa);
        textureSourceValue = g_InGameTechnologyTextureSource;
        loadedTextureSource = loadedTexture.eax;
        if (!loadedTexture.carry) {
          LOCK();
          UNLOCK();
          g_InGameTechnologyTextureSource = loadedTextureSource;
          (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(textureSourceValue);
          THANDOR_UI_FIELD(inGameRoot,0x1814 + 0x54,struct UiNodeBase *) = (UiNodeBase *)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x1870 + 0x54,struct UiNodeBase *) = (UiNodeBase *)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x18CC + 0x54,struct UiNodeBase *) = (UiNodeBase *)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x1928 + 0x54,struct UiNodeVtable *) = (UiNodeVtable *)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x1984 + 0x54,sdword) = (sdword)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x19E0 + 0x54,sdword) = (sdword)loadedTextureSource;
          THANDOR_UI_FIELD(inGameRoot,0x1A3C + 0x54,sdword) = (sdword)loadedTextureSource;
          logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,loadedTextureSource);
          techTextureHeight = logicalSize.logicalHeightPixels;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0x1544)->topOffset;
          *sdwordField = *sdwordField - techTextureHeight;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0x15AC)->topOffset;
          *sdwordField = *sdwordField - techTextureHeight;
          anchorField = &THANDOR_UI_FIELD(inGameRoot,0x1614 + 0x24,dword);
          *anchorField = *anchorField - techTextureHeight;
          nodeFlagsField = &THANDOR_UI_FIELD(inGameRoot,0x167C + 0x24,enum UiNodeFlags);
          *nodeFlagsField = *nodeFlagsField - techTextureHeight;
          THANDOR_UI_FIELD(inGameRoot,0x16E4 + 0x24,struct UiNodeBase *) =
               (UiNodeBase *)((int)THANDOR_UI_FIELD(inGameRoot,0x16E4 + 0x24,struct UiNodeBase *) - techTextureHeight);
          sdwordField = &THANDOR_UI_AT(inGameRoot,0x174C)->topOffset;
          *sdwordField = *sdwordField - techTextureHeight;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0x17B4)->topOffset;
          *sdwordField = *sdwordField - techTextureHeight;
          THANDOR_UI_FIELD(inGameRoot,0x1AF4 + 0x2C,struct UiRootCallbacks *) = (UiRootCallbacks *)((int)THANDOR_UI_FIELD(inGameRoot,0x1AF4 + 0x2C,struct UiRootCallbacks *) - techTextureHeight)
          ;
          detailIndex = logicalSize.logicalWidthPixels * 7 >> 1;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0xE6C)->leftOffset;
          *sdwordField = *sdwordField - detailIndex;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0xE6C)->rightOffset;
          *sdwordField = *sdwordField + detailIndex;
          sdwordField = &THANDOR_UI_AT(inGameRoot,0xE6C)->topOffset;
          *sdwordField = *sdwordField - (techTextureHeight >> 1);
          sdwordField = &THANDOR_UI_AT(inGameRoot,0xE6C)->bottomOffset;
          *sdwordField = *sdwordField + (techTextureHeight >> 1);
          THANDOR_UI_FIELD(inGameRoot,0x1B84 + 0x50,struct UiRootNode *) =
               (UiRootNode *)
               ((THANDOR_UI_AT(inGameRoot,0xE6C)->rightOffset - THANDOR_UI_AT(inGameRoot,0xE6C)->leftOffset) + -0x18 +
               (THANDOR_UI_FIELD(inGameRoot,0x1AF4 + 0x28,enum UiRootFlags) - THANDOR_UI_AT(inGameRoot,0x1AF4)->leftOffset));
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[0];
          THANDOR_UI_FIELD(inGameRoot,0x4758 + 0x68,sdword) = (sdword)g_UiButtonSoundVoiceSets7[0];
          THANDOR_UI_FIELD(inGameRoot,0x4820 + 0x68,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x48EC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x4D0C + 0x68,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5DB4 + 0x68,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7680 + 0x68,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8C4C + 0x68,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x4BA4 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x4C1C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x4C94 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x98B8 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9930 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x99A8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[1];
          THANDOR_UI_FIELD(inGameRoot,0x4388 + 0x70,sdword) = (sdword)g_UiButtonSoundVoiceSets7[1];
          THANDOR_UI_FIELD(inGameRoot,0x4400 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[2];
          THANDOR_UI_FIELD(inGameRoot,0xB210 + 0x70,sdword) = (sdword)g_UiButtonSoundVoiceSets7[2];
          THANDOR_UI_FIELD(inGameRoot,0xB290 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB310 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB390 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB410 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB490 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB510 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB590 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB610 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB688 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB700 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB778 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB7F0 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB868 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB8E0 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB958 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB9D0 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBA48 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBAC0 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBBB0 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBC28 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBCA0 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBD18 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBD90 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBE08 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBE80 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBEF8 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xBF70 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xA0CC + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5A4C + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5AC8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5B44 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5BC0 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5C3C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5CB8 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5D34 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5E7C + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5EFC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5F7C + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x5FFC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x607C + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x60FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x617C + 0x70,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x61FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x627C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x62FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x637C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x63FC + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x647C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x64FC + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x657C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x65FC + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x667C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x66FC + 0x70,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x677C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x67FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x687C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x68FC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x697C + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x69FC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6A7C + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6AFC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6B7C + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6BFC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6C7C + 0x70,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6CFC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6D7C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6DFC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6E7C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6EFC + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6F7C + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6FFC + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x707C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x70FC + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x717C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x71FC + 0x70,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x727C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x72FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x737C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x73FC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x747C + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x74FC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x757C + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x75FC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7748 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x77C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7848 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x78C8 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7948 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x79C8 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7A48 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7AC8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7B48 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7BC8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7C48 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7CC8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7D48 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7DC8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7E48 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7EC8 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7F48 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7FC8 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8048 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x80C8 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8148 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x81C8 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8248 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x82C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8348 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x83C8 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8448 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x84C8 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8548 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x85C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8648 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x86C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8748 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x87C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8848 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x88C8 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8948 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x89C8 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8A48 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8AC8 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8B48 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8BC8 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8D14 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8D90 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8E0C + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8E88 + 0x70,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8F04 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8F80 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x8FFC + 0x70,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9078 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x90F4 + 0x70,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9170 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x91EC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9268 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x92E4 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9360 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x93DC + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9458 + 0x70,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x94D4 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9550 + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x95CC + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9648 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x96C4 + 0x70,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9740 + 0x70,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x97BC + 0x70,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x9838 + 0x70,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
          THANDOR_UI_FIELD(inGameRoot,0x584 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)g_UiButtonSoundVoiceSets7[3];
          THANDOR_UI_FIELD(inGameRoot,0x5E4 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x644 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x6A4 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x704 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x764 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x7C4 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2550 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2610 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x25B0 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2670 + 0x5C,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2B34 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2B94 + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2C50 + 0x5C,struct UiRootNode *) = (UiRootNode *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2CB0 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3034 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x30F0 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3150 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x31B0 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3210 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x38E0 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1E24 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1DC4 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1D64 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x14DC + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x147C + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xF24 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
          THANDOR_UI_FIELD(inGameRoot,0x2900 + 0x5C,sdword) = (sdword)g_UiButtonSoundVoiceSets7[4];
          THANDOR_UI_FIELD(inGameRoot,0x2960 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2A14 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2A74 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2AD4 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x32CC + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x37C0 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3820 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3880 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x399C + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x39FC + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3A5C + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3380 + 0x5C,enum UiRootFlags) = (UiRootFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x33E8 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3450 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x34B8 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3520 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3588 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1E84 + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1F44 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1EE4 + 0x5C,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x20E4 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2144 + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x21A4 + 0x5C,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2204 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2264 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x22C4 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x2324 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1544 + 0x5C,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x15AC + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1614 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x167C + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x16E4 + 0x5C,dword) = (UiAnchorFractionQ31)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x174C + 0x5C,enum UiNodeFlags) = (UiNodeFlags)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x17B4 + 0x5C,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xF84 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xFE4 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1044 + 0x5C,sdword) = (sdword)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[5];
          THANDOR_UI_FIELD(inGameRoot,0x3704 + 0x64,sdword) = (sdword)g_UiButtonSoundVoiceSets7[5];
          THANDOR_UI_FIELD(inGameRoot,0x3BD0 + 0x64,struct UiNodeVtable *) = (UiNodeVtable *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3D4C + 0x64,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x3EC8 + 0x64,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x4044 + 0x64,struct UiNodeBase *) = (UiNodeBase *)buttonVoiceSet;
          buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
          THANDOR_UI_FIELD(inGameRoot,0x2F2C + 0x68,sdword) = (sdword)g_UiButtonSoundVoiceSets7[6];
          THANDOR_UI_FIELD(inGameRoot,0x2DA0 + 0x68,struct UiRootCallbacks *) = (UiRootCallbacks *)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0x1C98 + 0x68,sdword) = (sdword)buttonVoiceSet;
          THANDOR_UI_FIELD(inGameRoot,0xB0 + 0x68,sdword) = (sdword)buttonVoiceSet;
          loadedTexture.carry = false;
          loadedTexture.eax = (GraphicsTextureSourceAsset *)buttonVoiceSet;
        }
      }
    }
  }
  initStatus.valueOrError = (dword)loadedTexture.eax;
  initStatus.carry = loadedTexture.carry;
  return initStatus;
}


/* Address: 0x00563BD0.
   Ownership: ui/ingame/runtime.
   Purpose: Updates the in-game HUD status counters, localized session text, timers, ready-state prompts, and
   related transient text state.
   Cross-module calls: WideNumber_FormatUtf16 [core/text/string], FrontendPlayerRuntime_SetReadyFlagById
   [ui/frontend/player], InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   WorldRuntime_GetVector0Regs [world/runtime/core], WorldRuntime_GetVector1Regs [world/runtime/core],
   TextResource_Resolve [assets/text/resources].
*/
void __thandor_void_preserve_eax_ecx_edx InGameHud_UpdateStatusCountersAndSessionPrompts(void)

{
  SelectionPlayerRuntimeBlock *selectionBlock;
  uint stepTicks;
  ulonglong elapsedSeconds;
  dword value;
  word *stream;
  uint frameOrFactionIndex;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  WorldRuntimeContext *world;
  FrontendPlayerRuntimeRecord *playerBlock;
  int factionRecordAddress;
  int rosterCount;
  word *rosterCursor;
  word *destination;
  RichTextCopyExpandedEaxCf5 copiedText;
  TextResourceResolveEaxCf5 resolvedText;
  TextResourceResolveEaxCf5 statusTemplate;
  WorldVector0EaxEcxEdx12 cameraPosition;
  WorldVector1EaxEcxEdx12 cameraOrientation;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  
  frameOrFactionIndex = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown = g_DebugOverlayCounterRefreshCountdown - 1;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 0x14;
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
    if ((g_InGameReadyStateToggleFlags & 1) == 0) {
      if (g_RenderedFrameCountSinceDebugRefresh < 0xd) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_SetReadyFlagById(g_LocalPlayerRuntimeId,0,0,2);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x340,0,0,2);
        }
        g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
      }
    }
    else if (0xc < g_RenderedFrameCountSinceDebugRefresh) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_SetReadyFlagById(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x340,0,0,0);
      }
      g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
    }
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
    g_TextureBindStateChangeCount = 0;
    g_TextureDeviceReloadCount = 0;
  }
  runtimeRoot = g_InGameRuntimeRoot;
  world = &g_InGameRuntimeRoot->worldRuntime0A30;
  cameraPosition = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  cameraOrientation = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  if (*(int *)((runtimeRoot->worldRuntime0A30).selection.reserved04_1F + 0x10) == 0x7fffffff) {
    g_FrontendDebugOverlayTextSlot10Utf16[0] = 0x2d;
    g_FrontendDebugOverlayTextSlot10Utf16[1] = 0;
    g_FrontendDebugOverlayTextSlot11Utf16[0] = 0x2d;
    g_FrontendDebugOverlayTextSlot11Utf16[1] = 0;
  }
  else {
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,*(WideNumberSignedValue32 *)
                     ((runtimeRoot->worldRuntime0A30).selection.reserved04_1F + 8),
               g_FrontendDebugOverlayTextSlot10Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,*(WideNumberSignedValue32 *)
                     ((runtimeRoot->worldRuntime0A30).selection.reserved04_1F + 0xc),
               g_FrontendDebugOverlayTextSlot11Utf16);
  }
  value = (*g_MemoryApi.queryFreeBytes)();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,value,
             g_FrontendDebugOverlayTextSlot12Utf16);
  elapsedSeconds = (ulonglong)(g_GameFactionRuntimeImage.tail.simulationTick + 0x4af) / 0x4b0;
  (*g_LocaleFormatTimeFieldsUtf16)
            ((dword)(elapsedSeconds / 0x3c),(dword)(elapsedSeconds % 0x3c),g_FrontendDebugOverlayTextSlot13Utf16);
  frameOrFactionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  destination = g_InGameFactionStatusTextScratchUtf16;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[frameOrFactionIndex] != 0) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frameOrFactionIndex] <
        FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(factionRecordAddress + 0x90) + *(int *)(factionRecordAddress + 0x94),(word *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0));
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster:
        resolvedText = TextResource_Resolve(0x21d4);
        rosterCursor = resolvedText.eax;
      }
      else {
        rosterCount = 0;
        remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        rosterCursor = g_InGamePlayerListTextScratchUtf16;
        do {
          if (frameOrFactionIndex == (playerBlock->factionAssignment).factionAssignmentIndex) {
            if (rosterCount != 0) {
              rosterCursor[0] = 0x2c;
              rosterCursor[1] = 0x20;
              rosterCursor = rosterCursor + 2;
            }
            rosterCount = rosterCount + 1;
            copiedText = RichTextCommandStream_CopyExpandedCf
                               (0x28,rosterCursor,(playerBlock->playerName).textUtf16);
            if (!copiedText.carry) {
              rosterCursor = (word *)((int)rosterCursor + copiedText.eax);
              selectionBlock = g_SelectionPlayerRuntimeBlockPointers[playerBlock->playerRuntimeId];
              stepTicks = selectionBlock->simulationStepTicks;
              if ((selectionBlock->sessionFlags & 1) != 0) {
                rosterCursor[0] = 0x20;
                rosterCursor[1] = 0x20;
                rosterCursor[2] = 0x50;
                rosterCursor[3] = 0;
                rosterCursor = rosterCursor + 3;
              }
              if (1 < stepTicks) {
                rosterCursor[0] = 0x20;
                rosterCursor[1] = 0x20;
                *(uint *)(rosterCursor + 2) = stepTicks * 0x10000 + 0x300078;
                rosterCursor = rosterCursor + 4;
              }
              if ((selectionBlock->sessionFlags & 2) != 0) {
                rosterCursor[0] = 0x20;
                rosterCursor[1] = 0x20;
                rosterCursor[2] = 0x8004;
                rosterCursor[3] = 0x8003;
                rosterCursor[4] = 0x57;
                rosterCursor[5] = 0x8005;
                rosterCursor = rosterCursor + 6;
              }
            }
          }
          playerBlock = playerBlock + 1;
          remainingPlayers = remainingPlayers - 1;
        } while (remainingPlayers != 0);
        *rosterCursor = 0;
        if (rosterCount == 0)
        goto 
        InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster
        ;
        resolvedText = TextResource_Resolve(0x21d3);
        rosterCursor = resolvedText.eax;
        RichTextCommandStream_PatchPayloadBySelector(0,g_InGamePlayerListTextScratchUtf16,rosterCursor);
      }
      resolvedText = TextResource_Resolve(*(int *)(factionRecordAddress + 0x38) + 0x2173);
      statusTemplate = TextResource_Resolve(0x21d2);
      stream = statusTemplate.eax;
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText.eax,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,rosterCursor,stream);
      RichTextCommandStream_PatchPayloadBySelector(2,(void *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0),stream);
      copiedText = RichTextCommandStream_CopyExpandedCf(0x400,destination,stream);
      if (!copiedText.carry) {
        destination = (word *)((int)destination + copiedText.eax);
      }
    }
    frameOrFactionIndex = frameOrFactionIndex + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
    if (7 < frameOrFactionIndex) {
      return;
    }
  } while( true );
}


/* Address: 0x005640E0.
   Ownership: ui/ingame/runtime.
   Purpose: Runs under the shared gameplay spin lock when the multiplayer/network mode bits are nonzero. It queries
   window texture subresource 0x72, uses the panel0 UTF-16 template for row sizing, updates the verified panel
   control at uiState+0x8E4, and rebuilds one 0x80-byte formatted text slot per player. Text resource 0xFF05 is
   selected when playerRecord+0x3C is zero and 0xFF06 otherwise; placeholder zero is patched with the player-record
   pointer before copying. The fixed backing array holds eight rows.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiContainer_LayoutChildren
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], RichTextCommandStream_CopyExpandedCf [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx InGamePanel_RebuildPlayerStatusRows(void *uiState)

{
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int panelHalfHeight;
  InGamePlayerStatusTextSlot *destination;
  FrontendPlayerNameUtf16_28 *replacementPayload;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 windowTextureSize;
  
  (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    windowTextureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    replacementPayload = &firstPlayerRecord->playerName;
    textExtent = RichTextCommandStream_MeasureRegs
                      (g_UiTextStyleNormal,(word *)u_gfx_panel_panel0_gfx_005630d0);
    panelHalfHeight = (textExtent.heightPixels * remainingPlayers >> 1) + windowTextureSize.logicalHeightPixels;
    destination = g_InGamePlayerStatusTextSlots;
    *(FrontendPlayerRuntimeBlockCount *)((int)uiState + 0x93c) = remainingPlayers;
    *(int *)((int)uiState + 0x910) = panelHalfHeight;
    *(int *)((int)uiState + 0x908) = -panelHalfHeight;
    UiContainer_LayoutChildren(*(UiNodeBase **)((int)uiState + 0x8ec));
    do {
      if (((FrontendPlayerFactionAssignmentState10 *)
          ((int)((UiTransferEndpointDescriptor *)(replacementPayload + 1) + 1) + 4))->
          readyOrWaitState == 0) {
        resourceId = 0xff05;
      }
      else {
        resourceId = 0xff06;
      }
      resolvedText = TextResource_Resolve(resourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,resolvedText.eax);
      RichTextCommandStream_CopyExpandedCf(0x80,destination->text,resolvedText.eax);
      destination = destination + 1;
      replacementPayload = replacementPayload + 0x7e;
      remainingPlayers = remainingPlayers - 1;
    } while (remainingPlayers != 0);
  }
  (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
  return;
}


/* Address: 0x005678C0.
   Ownership: ui/ingame/runtime.
   Purpose: In-game UI command dispatcher selected by command code and modifier flags. Typed parameters: p0
   modifierFlags→UiKeyboardStateMask_V297, p1 commandCode→UiActionId_V338. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *inGameRuntime)

{
  /* Rewritten from the assembly (0x005678C0-0x00568204). The decompiled version jumped to the
     continuation labels inside the original machine code. EBX is the in-game runtime root. */
  byte *rt = (byte *)inGameRuntime;
  UiCommandDispatchRecord *record = g_InGameCommandDispatchRecords_00_Code00030073_Modifier33;
  dword target = 0;
  bool localSession = (g_SessionNetworkRoleFlags & 3) == 0;
  bool commandsBlocked = (g_UiCommandRuntimeFlags & 0x101) != 0;

#define RT_DWORD(offset) (*(dword *)(rt + (offset)))
  for (;; record++) {
    uint flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & 0x3f) != 0) continue;
    }
    else {
      if ((flags & 3) != 0) {
        if ((modifierFlags & 3) == 0) continue;
      }
      else if ((modifierFlags & 3) != 0) {
        continue;
      }
      if ((flags & 0x3c) == 0) {
        if ((modifierFlags & 0x3c) != 0) continue;
      }
      else if ((flags & 0x30) == 0) {
        if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) != 0)) continue;
      }
      else if ((flags & 0xc) == 0) {
        if (((modifierFlags & 0xc) != 0) || ((modifierFlags & 0x30) == 0)) continue;
      }
      else {
        if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0)) continue;
      }
    }
    target = (dword)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x567cc0: /* number key: select group */
  case 0x567d10: /* with shift-class modifier: mode 2 */
  case 0x567d60: /* mode 1 */
  case 0x567db0: { /* mode 3 */
    dword mode = (target == 0x567cc0) ? 0 : (target == 0x567d10) ? 2 : (target == 0x567d60) ? 1 : 3;
    dword group = commandCode - 0x30031;
    if (commandsBlocked) {
      break;
    }
    if (localSession) {
      FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
                (g_LocalPlayerRuntimeId,RT_DWORD(0x50),mode,group);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0xbe0,RT_DWORD(0x50),mode,group);
    }
    break;
  }
  case 0x567e00:
    InGameTargetingContext_AdvanceOrResolveTarget((InGameTargetingRootTraversalView9E60 *)(rt + 0x90cc));
    break;
  case 0x567e20:
    InGameTargetingContext_CancelAndRestoreState((InGameTargetingRootTraversalView9E60 *)(rt + 0x90cc));
    break;
  case 0x567e40: { /* camera to the last event position */
    FieldGridNearestPointRegsCf13 point;
    if ((RT_DWORD(0x9428) == 0) || (RT_DWORD(0x942c) == 0)) {
      break;
    }
    point = FieldGrid_GetNearestTerrainPoint(RT_DWORD(0x942c),RT_DWORD(0x9428),
                                             *(FieldGridAsset **)(rt + 0x54));
    WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
              (RT_DWORD(0x74),RT_DWORD(0x70),RT_DWORD(0x8c),point.edx,RT_DWORD(0x942c),
               RT_DWORD(0x9428),inGameRuntime);
    break;
  }
  case 0x567ea0: { /* camera to the selection */
    WorldPositionEaxEcxEdxCf13 center = SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf();
    if (center.carry) {
      break;
    }
    WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
              (RT_DWORD(0x74),RT_DWORD(0x70),RT_DWORD(0x7c),center.worldZQ12,center.worldYQ12,
               center.worldXQ12,inGameRuntime);
    break;
  }
  case 0x567ed0: { /* camera to the faction's headquarters (class 0x0B) */
    byte *node = *(byte **)(rt + 0xd8);
    dword faction = RT_DWORD(0x50);
    for (; node != (byte *)0; node = *(byte **)(node + 4)) {
      byte *payload;
      if (*(dword *)(node + 0xa4) != 0) {
        continue;
      }
      payload = *(byte **)(node + 0x48);
      if ((faction == *(dword *)(*(byte **)(payload + 8) + 0xc)) &&
          (*(dword *)(*(byte **)payload + 0x4c) == 0xb)) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (RT_DWORD(0x74),RT_DWORD(0x70),RT_DWORD(0x7c),*(dword *)(node + 0x9c),
                   *(dword *)(node + 0x98),*(dword *)(node + 0x94),inGameRuntime);
        break;
      }
    }
    break;
  }
  case 0x567f60:
  case 0x567fc0:
  case 0x568020:
  case 0x568130: {
    static const dword queued[4] = {0xe10,0xe30,0xe50,0xe70};
    int which = (target == 0x567f60) ? 0 : (target == 0x567fc0) ? 1 : (target == 0x568020) ? 2 : 3;
    if (commandsBlocked || SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(RT_DWORD(0x50))) {
      break;
    }
    if (!localSession) {
      InGameCommandQueue_AppendLocalPlayerCommand(queued[which],0,0,0);
    }
    else if (which == 0) {
      PlayerSelection_ResetMovementPruneAndRecenterEntries(g_LocalPlayerRuntimeId,0,0,0);
    }
    else if (which == 1) {
      PlayerSelection_ResetMovementAnchorsAndClearFlag200ForEligibleEntries(g_LocalPlayerRuntimeId,0,0,0);
    }
    else if (which == 2) {
      PlayerSelection_InterruptTargetsAndClearFlag10ForEligibleEntries(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      PlayerSelection_ApplyFlags418UnlessBit8ToEligibleEntries(g_LocalPlayerRuntimeId,0,0,0);
    }
    break;
  }
  case 0x568080: { /* selection page toggle */
    byte *button = rt + 0x969c;
    if (commandsBlocked || ((RT_DWORD(0x96e4) & 8) != 0)) {
      break;
    }
    if (UiPageStack_ActivePageNotInListCf((UiPageStackControl *)(rt + 0x957c)).valueOrError != 1) {
      break;
    }
    if (((*(dword *)(button + 0x4c) & 0x200) != 0) && (*(dword *)(button + 0x70) != 0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,*(DirectSoundVoiceSet **)(button + 0x70));
    }
    InGameSelectionPage_ToggleAndRefreshPage2((UiNodeBase *)rt);
    break;
  }
  case 0x5680f0:
    if (commandsBlocked) {
      break;
    }
    if (localSession) {
      InGameSelection_RebuildOwnedClass16Selection(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x8f0,0,0,0);
    }
    break;
  case 0x568190:
    RT_DWORD(0x4c) = RT_DWORD(0x4c) ^ 0x40000;
    break;
  case 0x5681a0:
    RT_DWORD(0x1ab0) = RT_DWORD(0x1ab0) ^ 8;
    break;
  case 0x5681b0: /* developer toggle: occupancy overlay */
    if (!localSession || ((g_UiCommandRuntimeFlags & 0x40000) == 0)) {
      break;
    }
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 8;
    if ((g_UiCommandRuntimeFlags & 8) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells(RT_DWORD(0x50),*(FieldGridAsset **)(rt + 0x54));
    }
    else {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(RT_DWORD(0x50),*(FieldGridAsset **)(rt + 0x54));
    }
    FieldGrid_ClassifyCellFlagsToRuntimeByte(RT_DWORD(0x50),*(FieldGridAsset **)(rt + 0x54));
    break;
  default:
    Thandor_Log("InGameUi dispatch: unhandled continuation %08x",target);
    break;
  }
#undef RT_DWORD
  return;
}


/* Address: 0x00569750.
   Ownership: ui/ingame/runtime.
   Purpose: One-argument in-game UI callback that clears transient state value 0x1B at context offset +0x911C.
*/
void InGameUiRuntime_ClearTransientState1BCallback(void *context)

{
  if (*(int *)((int)context + 0x911c) == 0x1b) {
    *(undefined4 *)((int)context + 0x911c) = 0;
  }
  return;
}

/* Address: 0x00569780.
   Ownership: ui/ingame/runtime.
   Purpose: One-argument in-game UI callback that dispatches the current world/selection action through the local
   frontend or command-queue path, subject to verified runtime suppression flags.
   Cross-module calls: SelectionInfo_ValidateOwnerType16AndAnyActiveCf [gameplay/selection/runtime],
   FrontendPlayerSelection_ClearAndRefreshLocalPanels [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid [gameplay/faction/runtime],
   WorldRuntime_RestoreMotionStateFromSnapshot [world/runtime/core].
*/
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *context)

{
  bool hasActiveOwnerType16;
  
  if ((((g_UiCommandRuntimeFlags & 0x101) == 0) &&
      (((context->interaction).interactionFlags48 & 8) == 0)) &&
     ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
    if ((context->runtimeFlags & 0x10) == 0) {
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        hasActiveOwnerType16 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(context->activeFactionRuntimeIndex);
        if (hasActiveOwnerType16) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
          }
          return;
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
        }
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
               SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,context->activeFactionRuntimeIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x14f0,0,0,context->activeFactionRuntimeIndex);
      }
    }
    else {
      context->runtimeFlags = context->runtimeFlags & 0xffffffef;
      WorldRuntime_RestoreMotionStateFromSnapshot(context);
    }
  }
  return;
}


/* Address: 0x00569890.
   Ownership: ui/ingame/runtime.
   Purpose: Inserts one eight-dword notification record into the four-slot in-game queue by descending priority,
   shifting displaced records through later slots and ignoring records with a zero identifier.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameNotificationQueue_InsertPriorityRecord
          (InGameNotificationPayloadKind payloadKind,dword payloadReserved10,
          dword orientationOrPresentationValue0C,AngleTurn32 primaryOrientationAngle08,
          Q12 secondaryWorldCoordinateQ12_04,Q12 primaryWorldCoordinateQ12_00,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId)

{
  Q12 *secondaryCoordinateSlot;
  AngleTurn32 *orientationAngleSlot;
  dword *payloadDwordSlot;
  InGameNotificationPayloadKind *payloadKindSlot;
  InGameNotificationPayloadKind carriedPayloadKind;
  dword carriedReserved10;
  dword carriedOrientationValue;
  AngleTurn32 carriedOrientationAngle;
  Q12 carriedSecondaryCoordinate;
  Q12 carriedPrimaryCoordinate;
  InGameNotificationMovieId displacedMovieId;
  uint displacedPriority;
  InGameNotificationMovieId remainingSlots;
  InGameNotificationQueueRecord20 *queueSlot;
  
  queueSlot = g_InGameRuntimeRoot->notificationQueue9E60;
  remainingSlots = 4;
  carriedPayloadKind = payloadKind;
  carriedReserved10 = payloadReserved10;
  carriedOrientationValue = orientationOrPresentationValue0C;
  carriedOrientationAngle = primaryOrientationAngle08;
  carriedSecondaryCoordinate = secondaryWorldCoordinateQ12_04;
  carriedPrimaryCoordinate = primaryWorldCoordinateQ12_00;
  displacedMovieId = notificationMovieId;
  while (displacedMovieId != 0) {
    displacedMovieId = notificationMovieId;
    displacedPriority = priority;
    payloadKind = carriedPayloadKind;
    payloadReserved10 = carriedReserved10;
    orientationOrPresentationValue0C = carriedOrientationValue;
    primaryOrientationAngle08 = carriedOrientationAngle;
    secondaryWorldCoordinateQ12_04 = carriedSecondaryCoordinate;
    primaryWorldCoordinateQ12_00 = carriedPrimaryCoordinate;
    if (queueSlot->priority04 < priority) {
      LOCK();
      displacedMovieId = queueSlot->notificationMovieId00;
      queueSlot->notificationMovieId00 = notificationMovieId;
      UNLOCK();
      LOCK();
      displacedPriority = queueSlot->priority04;
      queueSlot->priority04 = priority;
      UNLOCK();
      LOCK();
      primaryWorldCoordinateQ12_00 = (queueSlot->payload08).primaryWorldCoordinateQ12_00;
      (queueSlot->payload08).primaryWorldCoordinateQ12_00 = carriedPrimaryCoordinate;
      UNLOCK();
      LOCK();
      secondaryCoordinateSlot = &(queueSlot->payload08).secondaryWorldCoordinateQ12_04;
      secondaryWorldCoordinateQ12_04 = *secondaryCoordinateSlot;
      *secondaryCoordinateSlot = carriedSecondaryCoordinate;
      UNLOCK();
      LOCK();
      orientationAngleSlot = &(queueSlot->payload08).primaryOrientationAngle08;
      primaryOrientationAngle08 = *orientationAngleSlot;
      *orientationAngleSlot = carriedOrientationAngle;
      UNLOCK();
      LOCK();
      payloadDwordSlot = &(queueSlot->payload08).orientationOrPresentationValue0C;
      orientationOrPresentationValue0C = *payloadDwordSlot;
      *payloadDwordSlot = carriedOrientationValue;
      UNLOCK();
      LOCK();
      payloadDwordSlot = &(queueSlot->payload08).reserved10;
      payloadReserved10 = *payloadDwordSlot;
      *payloadDwordSlot = carriedReserved10;
      UNLOCK();
      LOCK();
      payloadKindSlot = &(queueSlot->payload08).payloadKind14;
      payloadKind = *payloadKindSlot;
      *payloadKindSlot = carriedPayloadKind;
      UNLOCK();
    }
    queueSlot = queueSlot + 1;
    remainingSlots = remainingSlots - 1;
    carriedPayloadKind = payloadKind;
    carriedReserved10 = payloadReserved10;
    carriedOrientationValue = orientationOrPresentationValue0C;
    carriedOrientationAngle = primaryOrientationAngle08;
    carriedSecondaryCoordinate = secondaryWorldCoordinateQ12_04;
    carriedPrimaryCoordinate = primaryWorldCoordinateQ12_00;
    priority = displacedPriority;
    notificationMovieId = displacedMovieId;
    displacedMovieId = remainingSlots;
  }
  return;
}


/* Address: 0x00569B00.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, enumerates active player slots excluding the root's current player index, fills at
   most seven action-0x1012 target controls, updates their text, icon, state sprite, visibility, and page
   selection, then hides unused slots.
   Cross-module calls: UiGrid_OneColumnDimensionsPacked [ui/controls/layout], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node)

{
  uint *controlFlags;
  UiNodeBase *ancestorParent;
  SessionNetworkRoleFlags remainingNetworkPlayers;
  UiControlCount nextRemainingCount;
  int factionIndexOrRecord;
  uint candidateFactionIndex;
  dword remainingFactions;
  int controlOffset;
  uint relationState;
  SessionNetworkRoleFlags remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint slotIndex;
  UiGridDimensionsEdxEax8 gridDimensions;
  UiControlCount otherActiveCount;
  
  ancestorParent = node->parent;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    ancestorParent = node->parent;
  }
  factionIndexOrRecord = 1;
  otherActiveCount = 0;
  remainingFactions = g_GameFactionRuntimeImage.tail.activeFactionCount;
  do {
    if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexOrRecord] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (factionIndexOrRecord != node[0x23].bottom)) &&
       ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
      otherActiveCount = otherActiveCount + 1;
    }
    factionIndexOrRecord = factionIndexOrRecord + 1;
    remainingFactions = remainingFactions - 1;
  } while (remainingFactions != 0);
  gridDimensions = UiGrid_OneColumnDimensionsPacked(otherActiveCount);
  factionIndexOrRecord = (int)gridDimensions * g_InGamePanelTextureSubresource32Width +
          g_InGamePanelTextureSubresource19Width + g_InGamePanelTextureSubresource20Width;
  controlOffset = (int)(gridDimensions >> 0x20) * g_InGamePanelTextureSubresource32Height +
          g_InGamePanelTextureSubresource18Height + g_InGamePanelTextureSubresource23Height;
  if ((int)g_FramebufferWidth < 800) {
    node[0x105].bottom = -0x1f;
    node[0x105].topOffset = -0x1f;
    node[0x105].leftOffset = -100;
    node[0x105].rightOffset = -100;
  }
  else {
    node[0x105].bottom = -0x27;
    node[0x105].topOffset = -0x27;
    node[0x105].leftOffset = -0x7e;
    node[0x105].rightOffset = -0x7e;
  }
  node[0x105].bottom = node[0x105].bottom - factionIndexOrRecord;
  node[0x105].leftOffset = node[0x105].leftOffset - controlOffset;
  node[0x104].topOffset = node[0x104].topOffset | 8;
  if ((otherActiveCount != 0) && ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) {
    node[0x104].topOffset = node[0x104].topOffset & 0xfffffff7;
  }
  (**(code **)(node[0x103].topAnchorQ31 + 0xc))((UiNodeBase *)&node[0x103].rightOffset);
  slotIndex = 0;
  if (otherActiveCount != 0) {
    candidateFactionIndex = 1;
    factionIndexOrRecord = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
    do {
      nextRemainingCount = otherActiveCount + 1;
      if ((candidateFactionIndex != node[0x23].bottom) &&
         (g_GameFactionRuntimeImage.tail.factionLifecycleStates[candidateFactionIndex] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE)) {
        g_UiAction1012TargetPlayerIndices[slotIndex] = candidateFactionIndex;
        UiPageStack_SetActiveIndex
                  (0,(UiPageStackControl *)
                     ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[slotIndex]));
        *(int *)((int)&node[1].parent + g_UiAction1012PlayerLabelTextOffsets[slotIndex]) =
             *(int *)(factionIndexOrRecord + 0x38) + 0x2173;
        *(uint *)((int)&node[1].parent + g_UiAction1012PlayerIndexTextOffsets[slotIndex]) =
             candidateFactionIndex + 0x2190;
        relationState = g_GameFactionRuntimeImage.records[node[0x23].bottom].packedRelationStates >>
                ((byte)(candidateFactionIndex << 2) & 0x1f) & 0xf;
        candidateFactionIndex = candidateFactionIndex & 0x3fffffff;
        *(uint *)((int)&node[1].parent + g_UiAction1012StateTextOffsets[slotIndex]) = relationState + 0x21a3;
        controlOffset = g_UiAction1012IconImageOffsets[slotIndex];
        *(undefined **)((int)&node[1].parent + controlOffset) = &g_EmptyFrontendPlayerNameUtf16;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        remainingNetworkPlayers = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (remainingNetworkPlayers != SESSION_NETWORK_ROLE_LOCAL) {
          if ((playerBlock->factionAssignment).factionAssignmentIndex == candidateFactionIndex) {
            *(FrontendPlayerNameUtf16_28 **)((int)&node[1].parent + controlOffset) = &playerBlock->playerName;
            break;
          }
          playerBlock = playerBlock + 1;
          remainingPlayerBlocks = remainingPlayerBlocks - SESSION_NETWORK_ROLE_CLIENT;
          remainingNetworkPlayers = remainingPlayerBlocks;
        }
        controlOffset = g_UiAction1012ControlOffsets[slotIndex];
        remainingFactions = g_UiAction1012SubresourceByState[relationState];
        slotIndex = slotIndex + 1;
        controlFlags = (uint *)((int)&node->nodeFlags + controlOffset);
        *controlFlags = *controlFlags & 0xfffffff7;
        *(dword *)((int)&node[1].vtable + controlOffset) = remainingFactions;
        nextRemainingCount = otherActiveCount;
        if (((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) != 0) &&
           ((7 < relationState ||
            (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) != 0 &&
             ((3 < relationState || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) != 0)))))))) {
          controlFlags = (uint *)((int)&node->nodeFlags + controlOffset);
          *controlFlags = *controlFlags | 8;
        }
      }
      otherActiveCount = nextRemainingCount;
      candidateFactionIndex = candidateFactionIndex + 1;
      factionIndexOrRecord = factionIndexOrRecord + 0x740;
      otherActiveCount = otherActiveCount - 1;
    } while (otherActiveCount != 0);
    if (6 < slotIndex) {
      return;
    }
  }
  do {
    UiPageStack_SetActiveIndex
              (1,(UiPageStackControl *)
                 ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[slotIndex]));
    slotIndex = slotIndex + 1;
  } while (slotIndex < 7);
  return;
}


/* Address: 0x0056A460.
   Ownership: ui/ingame/runtime.
   Purpose: Walks world entries and computes the weighted suitability score for a music track class. Typed
   parameters: p0 trackClassId→MusicTrackClassId_V343. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
dword __thandor_eax_preserve_ecx_edx
InGameMusic_ComputeTrackSuitabilityScore
          (MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime)

{
  int activeFactionIndex;
  dword suitabilityScore;
  ArmyAssetRuntimeSemanticView80 *armyDefinition;
  int modelRuntimeOrBonus;
  int registryWeight;
  ArmyRegistryEaxCf5_51b6d0 foundArmyAsset;
  int flag10BonusSum;
  int class74Sum;
  int weightedClass78Sum;
  int class70Sum;
  WorldOwnerListNode100 *ownerListNode;
  
  suitabilityScore = 0;
  class70Sum = 0;
  weightedClass78Sum = 0;
  class74Sum = 0;
  flag10BonusSum = 0;
  if (trackClassId != 0) {
    activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
    for (ownerListNode = worldRuntime->ownerListHead; ownerListNode != (WorldOwnerListNode100 *)0x0;
        ownerListNode = ownerListNode->nextNode) {
      if ((ownerListNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelRuntimeOrBonus = *(int *)((int)ownerListNode->runtimePayload + 8), activeFactionIndex == *(int *)(modelRuntimeOrBonus + 0xc))) {
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf(*(PckArmyAssetIdCatalog *)(modelRuntimeOrBonus + 0xa0));
        armyDefinition = (ArmyAssetRuntimeSemanticView80 *)foundArmyAsset.eax;
        if (!foundArmyAsset.carry) {
          registryWeight = 1;
          if ((*(uint *)(modelRuntimeOrBonus + 0x2c) & 1) != 0) {
            registryWeight = 3;
          }
          class70Sum = class70Sum + armyDefinition->definitionClassValue70;
          weightedClass78Sum = weightedClass78Sum + registryWeight * armyDefinition->definitionClassValue78;
          modelRuntimeOrBonus = 0x32;
          if ((armyDefinition->flags14 & 0x10) == 0) {
            modelRuntimeOrBonus = 0;
          }
          class74Sum = class74Sum + armyDefinition->definitionClassValue74;
          flag10BonusSum = flag10BonusSum + modelRuntimeOrBonus;
        }
      }
    }
    if (trackClassId < 0x14) {
      suitabilityScore = flag10BonusSum * 0x80 + class74Sum * 0x100 + weightedClass78Sum * 0x280 + class70Sum * 0x100;
    }
    else if (trackClassId < 0x32) {
      suitabilityScore = flag10BonusSum * -0x100 + class74Sum * 0x40 + 0x32000 + weightedClass78Sum * 0x10 + class70Sum * 0x80;
    }
    else if (trackClassId < 0x46) {
      suitabilityScore = flag10BonusSum * 0x80 + class74Sum * 0x100 + weightedClass78Sum * 0x20 + class70Sum * 0x300;
    }
    else {
      suitabilityScore = flag10BonusSum * 0x100 + class74Sum * 0x200 + weightedClass78Sum * 0x180 + class70Sum * 0x10;
    }
  }
  return suitabilityScore;
}


/* Address: 0x0056A8A0.
   Ownership: ui/ingame/runtime.
   Purpose: Recovered action-table target INGAME_PAGE10[31] (0x101F).
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiSelectableControl_SetSelected [ui/controls/lists], UiKeyboardFocus_ReleaseNode
   [ui/controls/input], TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureWrappedBlockRegs
   [assets/text/richtext].
*/

void __thandor_void_preserve_eax_ecx_edx InGameUiAction101F_Handler(UiNodeBase *source)

{
  WorldInteractionFlags *interactionFlagsField;
  UiNodeBase *ancestorParent;
  bool isSelected;
  RichTextExtentRegs wrappedExtent;
  TextResourceResolveEaxCf5 resolvedText;
  InGameAction101FRootView43DC *uiRoot;
  
  ancestorParent = source->parent;
  uiRoot = (InGameAction101FRootView43DC *)source;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    uiRoot = (InGameAction101FRootView43DC *)(uiRoot->rootUi0000).base.parent;
    ancestorParent = (uiRoot->rootUi0000).base.parent;
  }
  isSelected = (bool)UiSelectableControl_IsSelectedCf((UiSelectableControl *)source);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(0,&uiRoot->technologyPageStack0BD0);
    interactionFlagsField = &(uiRoot->worldRuntime0A30).interaction.interactionFlags48;
    *interactionFlagsField = *interactionFlagsField & 0xfffffff7;
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
  UiSelectableControl_SetSelected(0,&uiRoot->sharedSettingsToggle4388);
  interactionFlagsField = &(uiRoot->worldRuntime0A30).interaction.interactionFlags48;
  *interactionFlagsField = *interactionFlagsField | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&uiRoot->worldRuntime0A30);
  UiPageStack_SetActiveIndex(8,&uiRoot->technologyPageStack0BD0);
  (uiRoot->textPanel0_1100).textResourceIdE4 =
       (uiRoot->worldRuntime0A30).activeFactionRuntimeIndex + 0x230017 +
       ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).header.
       titleTextResourceIndex * 0x10;
  resolvedText = TextResource_Resolve((uiRoot->textPanel0_1100).textResourceIdE4);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,resolvedText.eax,(uiRoot->textPanel0_1100).wrapWidthE0);
  (uiRoot->textPanel0_1100).measuredWidthB8 = wrappedExtent.widthPixels + 6;
  (uiRoot->textPanel0_1100).measuredHeightBC = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->textPanel0_1100).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->textPanel0_1100).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->textPanel1_11EC).textResourceIdE4);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,resolvedText.eax,(uiRoot->textPanel1_11EC).wrapWidthE0);
  (uiRoot->textPanel1_11EC).measuredWidthB8 = wrappedExtent.widthPixels + 6;
  (uiRoot->textPanel1_11EC).measuredHeightBC = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->textPanel1_11EC).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->textPanel1_11EC).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->textPanel2_1334).textResourceIdE4);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,resolvedText.eax,(uiRoot->textPanel2_1334).wrapWidthE0);
  (uiRoot->textPanel2_1334).measuredWidthB8 = wrappedExtent.widthPixels + 6;
  (uiRoot->textPanel2_1334).measuredHeightBC = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->textPanel2_1334).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->textPanel2_1334).scrollable);
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
       SESSION_NETWORK_ROLE_LOCAL) && ((g_UiCommandRuntimeFlags & 0x4000) == 0)) {
    if ((g_UiCommandRuntimeFlags & 1) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x400;
    }
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x4001;
  }
  return;
}


/* Address: 0x0056AD00.
   Ownership: ui/ingame/runtime.
   Purpose: Recovered action-table target INGAME_PAGE10[28] (0x101C).
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists],
   UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists], UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction101C_Handler(UiSelectableControl *selectableControl)

{
  UiSelectableNodeEaxEcxCf9 visibleSelection;
  int parentNodeAddress;
  void *rootNodeCursor;
  
  parentNodeAddress = (int)(selectableControl->base).parent;
  rootNodeCursor = selectableControl;
  while (parentNodeAddress != -1) {
    rootNodeCursor = (((UiSelectableControl *)rootNodeCursor)->base).parent;
    parentNodeAddress = *(int *)((int)rootNodeCursor + 8);
  }
  UiSelectableGroup_SelectExclusive(3,&selectableControl->base,
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x644),
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x5e4),
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x584));
  visibleSelection = UiSelectableGroup_NoneVisibleSelectedCf(3,
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x644),
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x5e4),
      THANDOR_UI_AT(Thandor_UiRoot(selectableControl),0x584));
  UiPageStack_SetActiveIndex
            (visibleSelection.controlIndexOrCount,
             (UiPageStackControl *)
             &(((UiSelectableControl *)((int)rootNodeCursor + 0x39c))->base).left);
  return;
}


/* Address: 0x0056AD80.
   Ownership: ui/ingame/runtime.
   Purpose: Handles numeric action 0x1012 for one of seven fixed variant-A controls. It resolves the control
   through g_UiAction1012ControlOffsets, loads the corresponding runtime payload, and chooses a backend operation
   from activationInputState bits. Queued UI action handler for INGAME_PAGE10[18] (0x1012). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: GameFactionRuntime_AdvancePairwiseRelationState [gameplay/faction/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   GameFactionRuntime_ResetPairwiseRelationState [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control)

{
  UiNodeBase *ancestorParent;
  UiCommandSpriteButtonControl *rootControl;
  CommandPayloadDword08 payloadDword08;
  GraphicsTextureSourceAsset *rootFactionValue;
  int slotIndex;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    ancestorParent = (control->sprite).selectable.base.parent;
    rootControl = control;
    while (ancestorParent != (UiNodeBase *)0xffffffff) {
      rootControl = (UiCommandSpriteButtonControl *)(rootControl->sprite).selectable.base.parent;
      ancestorParent = (rootControl->sprite).selectable.base.parent;
    }
    slotIndex = 6;
    while ((int)control - (int)rootControl != g_UiAction1012ControlOffsets[slotIndex]) {
      slotIndex = slotIndex + -1;
      if (slotIndex < 0) {
        return;
      }
    }
    payloadDword08 = g_UiAction1012TargetPlayerIndices[slotIndex];
    if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0) {
      rootFactionValue = rootControl[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_AdvancePairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,payloadDword08,(FactionRuntimeIndex)rootFactionValue);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x660,0,payloadDword08,(CommandPayloadDword04)rootFactionValue);
      }
    }
    else {
      rootFactionValue = rootControl[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ResetPairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,payloadDword08,(FactionRuntimeIndex)rootFactionValue);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x7e0,0,payloadDword08,(CommandPayloadDword04)rootFactionValue);
      }
    }
  }
  return;
}


/* Address: 0x0056B520.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, clears suppression on the command-page container, toggles the page stack at
   root+0xBD0 between pages 0 and 2, refreshes the seven-slot selection state when page 2 becomes active, and
   dispatches the verified backend payload. Queued UI action handler for INGAME_PAGE10[16] (0x1010). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: UiPageStack_ActivePageNotInListCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], SelectionInfo_GetFirstEntry [gameplay/selection/runtime],
   InGameTechnologyPanel_ResetAndSelectCurrentArea [ui/ingame/technology],
   FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80 [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_ToggleAndRefreshPage2(UiNodeBase *source)

{
  sdword *controlField;
  UiNodeBase *ancestorParent;
  void *definitionRecord;
  UiPageIndex pageIndex;
  GameEntityRuntime *firstSelectedEntity;
  CommandPayloadDword04 modelOffset;
  StatusValueEaxCf5 pageNotInListResult;
  
  ancestorParent = source->parent;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    source = (((UiRootNode *)source)->base).parent;
    ancestorParent = (((UiRootNode *)source)->base).parent;
  }
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    controlField = &(((UiRootNode *)((int)source + 0xa50))->base).rightOffset;
    *controlField = *controlField & 0xfffffff7;
    controlField = &(((UiRootNode *)((int)source + 0xbb0))->base).leftOffset;
    pageNotInListResult = UiPageStack_ActivePageNotInListCf((UiPageStackControl *)controlField);
    if (pageNotInListResult.valueOrError == 2) {
      pageIndex = 0;
    }
    else {
      pageIndex = 2;
    }
    UiPageStack_SetActiveIndex(pageIndex,(UiPageStackControl *)controlField);
    if ((pageIndex == 2) &&
       (firstSelectedEntity = SelectionInfo_GetFirstEntry(), firstSelectedEntity != (GameEntityRuntime *)0x0)) {
      definitionRecord = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
      InGameTechnologyPanel_ResetAndSelectCurrentArea((UiRootNode *)source);
      modelOffset = (int)definitionRecord - g_ModelRuntimeRebaseDelta;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
                  (g_LocalPlayerRuntimeId,0,0,modelOffset);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x16b0,0,0,modelOffset);
      }
    }
  }
  return;
}


/* Address: 0x0056B5D0.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, initializes the three selection-mode controls, selects subpage 0, populates up to
   seven fixed controls from active player slots, updates labels and visibility, recomputes the scroll extent, and
   hides unused controls. Queued UI action handler for INGAME_PAGE10[6] (0x1006). Return datatype is preserved for
   non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiScrollableControl_RebuildViewportAndScrollbars [ui/controls/lists],
   UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source)

{
  uint *controlFlags;
  UiNodeBase *ancestorParent;
  UiNodeBase *uiRootNode;
  int factionNameIndex;
  word *stream;
  TextResourceId resourceId;
  uint factionIndexCursor;
  int factionRecordAddress;
  uint filledSlotCount;
  TextResourceResolveEaxCf5 resolvedText;
  
  ancestorParent = source->parent;
  uiRootNode = source;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    uiRootNode = uiRootNode->parent;
    ancestorParent = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1f44),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1ee4),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1e84));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&uiRootNode[0x6a].bottomOffset);
  resourceId = 0x216d;
  filledSlotCount = 0;
  factionIndexCursor = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexCursor] != 0) {
      resolvedText = TextResource_Resolve(resourceId);
      stream = resolvedText.eax;
      factionNameIndex = *(int *)(factionRecordAddress + 0x38);
      resourceId = resourceId + 1;
      controlFlags = (uint *)((int)&uiRootNode->nodeFlags + g_UiSevenSlotSelectionControlOffsets[filledSlotCount]);
      *controlFlags = *controlFlags & 0xfffffff7;
      filledSlotCount = filledSlotCount + 1;
      resolvedText = TextResource_Resolve(factionNameIndex + 0x2173);
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText.eax,stream);
    }
    factionIndexCursor = factionIndexCursor + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
  } while (factionIndexCursor < 8);
  uiRootNode[0x6e].left = filledSlotCount * 0x18;
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)&uiRootNode[0x6b].rightAnchorQ31);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)&uiRootNode[0x6b].rightAnchorQ31);
  for (; filledSlotCount < 7; filledSlotCount = filledSlotCount + 1) {
    controlFlags = (uint *)((int)&uiRootNode->nodeFlags + g_UiSevenSlotSelectionControlOffsets[filledSlotCount]);
    *controlFlags = *controlFlags | 8;
  }
  return;
}


/* Address: 0x0056B6E0.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, initializes the same selection controls, selects subpage 0, populates up to seven
   fixed controls from the runtime record catalog, updates localized labels and visibility, recomputes the scroll
   extent, and hides unused controls. Queued UI action handler for INGAME_PAGE10[7] (0x1007). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiScrollableControl_RebuildViewportAndScrollbars [ui/controls/lists],
   UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  uint *controlFlags;
  UiNodeBase *ancestorParent;
  UiNodeBase *uiRootNode;
  SelectionPlayerRuntimeBlock *selectionBlock;
  TextResourceId resourceId;
  uint filledSlotCount;
  TextResourceResolveEaxCf5 resolvedText;
  
  ancestorParent = source->parent;
  uiRootNode = source;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    uiRootNode = uiRootNode->parent;
    ancestorParent = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1f44),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1ee4),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1e84));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&uiRootNode[0x6a].bottomOffset);
  resourceId = 0x216d;
  filledSlotCount = 0;
  do {
    resolvedText = TextResource_Resolve(resourceId);
    selectionBlock = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[filledSlotCount].playerRuntimeId];
    resourceId = resourceId + 1;
    controlFlags = (uint *)((int)&uiRootNode->nodeFlags + g_UiSevenSlotSelectionControlOffsets[filledSlotCount]);
    *controlFlags = *controlFlags & 0xfffffff7;
    filledSlotCount = filledSlotCount + 1;
    RichTextCommandStream_PatchPayloadBySelector(0,selectionBlock->reserved80B0_8117 + 0x40,resolvedText.eax);
    if (6 < filledSlotCount) break;
  } while (filledSlotCount < g_FrontendPlayerRuntimeBlockCount);
  uiRootNode[0x6e].left = filledSlotCount * 0x18;
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)&uiRootNode[0x6b].rightAnchorQ31);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)&uiRootNode[0x6b].rightAnchorQ31);
  for (; filledSlotCount < 7; filledSlotCount = filledSlotCount + 1) {
    controlFlags = (uint *)((int)&uiRootNode->nodeFlags + g_UiSevenSlotSelectionControlOffsets[filledSlotCount]);
    *controlFlags = *controlFlags | 8;
  }
  return;
}


/* Address: 0x0056B7E0.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, initializes the three selection-mode controls, and selects subpage 1 in the page
   stack at root+0x1FA4. Queued UI action handler for INGAME_PAGE10[8] (0x1008). Return datatype is preserved for
   non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSelectionPage_ShowSubpage1(UiNodeBase *source)

{
  int selectionGroupAddress;
  UiNodeBase *parentNode;
  UiNodeBase *rootNodeCursor;
  
  parentNode = source->parent;
  rootNodeCursor = source;
  while (parentNode != (UiNodeBase *)0xffffffff) {
    rootNodeCursor = rootNodeCursor->parent;
    parentNode = rootNodeCursor->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1f44),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1ee4),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1e84));
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&rootNodeCursor[0x6a].bottomOffset);
  return;
}


/* Address: 0x0056D500.
   Ownership: ui/ingame/runtime.
   Purpose: Removes the oldest serial-tagged recent-text entries until the displayed count is reduced to three,
   then sorts the shared eight slots and rebuilds the caller's index list with capacity eight. Queued UI action
   handler for INGAME_PAGE10[15] (0x100F). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: RecentTextHistory_RemoveOldest [ui/support/runtime],
   RecentTextHistory_SortAndBuildPointerList [ui/support/runtime].
*/
void __thandor_void_preserve_eax_ecx
InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView)

{
  uint currentEntryCount;
  
  for (currentEntryCount = (historyView->recentTextPointerList).count; 4 < currentEntryCount;
      currentEntryCount = currentEntryCount - 1) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList(8,&historyView->recentTextPointerList);
  return;
}


/* Address: 0x0056F7F0.
   Ownership: ui/ingame/runtime.
   Purpose: Typed parameters: p0 pointerRegionCode→UiPointerRegionCode_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p4 armyRuntimeUnderPointer→ArmyRuntimeSlot *. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime],
   ArmyRuntimeNode_DispatchTypedCallback [gameplay/army/runtime], ArmyRuntime_DestroyInstanceAndRefreshUi
   [gameplay/army/runtime].
*/
dword InGameUiCommand_ResolveCursorCodeByMode
                (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12,
                dword reservedArg3,ArmyRuntimeSlot *armyRuntimeUnderPointer,
                WorldRuntimeContext *worldRuntime)

{
  SelectionPlayerRuntimeBlock *localSelectionBlock;
  dword cursorCodeOrSubMode;
  bool callbackAccepted;
  ArmyRuntimeCreateEaxCf5 previewArmyRuntime;
  
  if ((armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) &&
     (armyRuntimeUnderPointer->runtimeStateA4 != 0)) {
    armyRuntimeUnderPointer = (ArmyRuntimeSlot *)0x0;
  }
                    // WARNING: Switch is manually overridden
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      return 0x1c;
    }
    if (g_UiCommandModeC == 1) {
      return 0x1e;
    }
    if (g_UiCommandModeC == 2) {
      return 0x20;
    }
    break;
  case 1:
    if (g_UiCommandModeD != 3) {
      if (g_UiCommandModeD == 1) {
        return 0x24;
      }
      if (g_UiCommandModeD == 2) {
        return 0x23;
      }
      return 0x22;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      return 0x1d;
    }
    if (g_UiCommandModeE == 1) {
      return 0x1f;
    }
    return 0x22;
  case 3:
    cursorCodeOrSubMode = g_UiCommandModeA;
    goto joined_r0x0056fa27;
  case 4:
    cursorCodeOrSubMode = g_UiCommandModeB;
joined_r0x0056fa27:
    if (cursorCodeOrSubMode == 0) {
      if (pointerRegionCode == 0x7fffffff) {
        return 0x18;
      }
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      cursorCodeOrSubMode = 0x17;
      if (localSelectionBlock->primarySelectionEntityOffset8094 == 0) {
        previewArmyRuntime = ArmyRuntime_CreateInstanceFromAssetCf
                          (1,0,pointerWorldXQ12,pointerWorldYQ12,g_UiCommandModeGOwnerFactionIndex,
                           g_UiCommandModeGArmyAssetId,worldRuntime);
        if (!previewArmyRuntime.carry) {
          callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)previewArmyRuntime.eax,worldRuntime);
          if (callbackAccepted) {
            cursorCodeOrSubMode = 0x18;
          }
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)previewArmyRuntime.eax);
          return cursorCodeOrSubMode;
        }
        return 0x17;
      }
    }
    else {
      if (cursorCodeOrSubMode == 1) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x19;
        }
        return 0x1a;
      }
      localSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      cursorCodeOrSubMode = 0x15;
      if (localSelectionBlock->primarySelectionEntityOffset8094 == 0) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x15;
        }
        return 0x16;
      }
    }
    callbackAccepted = ArmyRuntimeNode_DispatchTypedCallback
                      ((ArmyRuntimeSlot **)
                       (localSelectionBlock->primarySelectionEntityOffset8094 +
                       (int)g_ArmyRuntimeRebaseBaseMinusOne),worldRuntime);
    if (callbackAccepted) {
      return cursorCodeOrSubMode + 1;
    }
    return cursorCodeOrSubMode;
  case 5:
    return 0x21;
  }
  return 0;
}


/* Address: 0x0056FA70.
   Ownership: ui/ingame/runtime.
   Purpose: Typed parameters: p0 pointerRegionCode→UiPointerRegionCode_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p4 armyRuntimeUnderPointer→ArmyRuntimeSlot *. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p5 mapControl→WorldRuntimeExtendedMapControlAddress32_V345.
   Cross-module calls: FieldGrid_GetNearestTerrainPoint [world/terrain/grid], FieldGrid_ClearPlayerScratchPlane
   [world/terrain/grid], InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FieldGrid_ResetLocalInfluenceState [world/terrain/grid], TerrainMaterialEdit_SeedMatchingRegionReplacement
   [world/terrain/editing], TerrainMaterialEdit_SeedNonTargetRegionReplacement [world/terrain/editing].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_BeginInteractionByMode
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,dword reservedArg3,
          ArmyRuntimeSlot *armyRuntimeUnderPointer,WorldRuntimeExtendedMapControlView170 *mapControl
          )

{
  FieldGridAsset *mapFieldGrid;
  longlong scaledGridX;
  longlong scaledGridY;
  PckArmyAssetIdCatalog lookupToken;
  dword placementSubMode;
  CommandPayloadDword04 commandPayload;
  int cellX;
  uint snappedWorldY;
  uint worldCoordinateTerm;
  int cellY;
  FieldGridNearestPointRegsCf13 nearestTerrainPoint;
  
  if ((armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) &&
     (armyRuntimeUnderPointer->runtimeStateA4 != 0)) {
    armyRuntimeUnderPointer = (ArmyRuntimeSlot *)0x0;
  }
                    // WARNING: Switch is manually overridden
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      mapControl->runtimeFlags = mapControl->runtimeFlags & 0xffefffff;
      if (pointerRegionCode == 0x7fffffff) {
        g_UiCommandDragStartScreenX = 0x7fffffff;
        return;
      }
      g_UiCommandDragStartScreenX = mapControl->extendedCoordinate160;
      g_UiCommandDragStartScreenY = mapControl->extendedCoordinate164;
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      g_UiCommandDragAnchorWorldXQ12 =
           (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff &
           0xfffff000;
      g_UiCommandDragAnchorWorldYQ12 = worldCoordinateTerm * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(0x1f20,0,0,0);
      return;
    }
    if (g_UiCommandModeC == 1) {
      mapControl->runtimeFlags = mapControl->runtimeFlags & 0xffefffff;
      if (pointerRegionCode == 0x7fffffff) {
        g_UiCommandDragStartScreenX = 0x7fffffff;
        return;
      }
      g_UiCommandDragStartScreenX = mapControl->extendedCoordinate160;
      g_UiCommandDragStartScreenY = mapControl->extendedCoordinate164;
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      g_UiCommandDragAnchorWorldXQ12 =
           (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff &
           0xfffff000;
      g_UiCommandDragAnchorWorldYQ12 = worldCoordinateTerm * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(0x1f20,0,0,0);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ResetLocalInfluenceState(g_LocalPlayerRuntimeId,0,0,0);
        return;
      }
      InGameCommandQueue_AppendLocalPlayerCommand(0x2a80,0,0,0);
      return;
    }
    break;
  case 1:
    if (g_UiCommandModeD != 3) {
      if (g_UiCommandModeD == 1) {
        if (pointerRegionCode == 0x7fffffff) {
          return;
        }
        nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
        scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
        snappedWorldY = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
        worldCoordinateTerm = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - snappedWorldY) + 0x3ff &
                0xfffff000;
        snappedWorldY = snappedWorldY * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedMatchingRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x28e0,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
        return;
      }
      if (g_UiCommandModeD != 2) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainEditBuffer_CopyCellMaterialBytes(g_LocalPlayerRuntimeId,0,0,0);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand(0x2700,0,0,0);
        return;
      }
      if (pointerRegionCode != 0x7fffffff) {
        nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
        scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
        snappedWorldY = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
        worldCoordinateTerm = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - snappedWorldY) + 0x3ff &
                0xfffff000;
        snappedWorldY = snappedWorldY * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedNonTargetRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x29b0,g_UiCommandAbsoluteSelectionIndex,snappedWorldY,worldCoordinateTerm);
        return;
      }
      return;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      if (pointerRegionCode != 0x7fffffff) {
        g_UiCommandDragStartScreenX = mapControl->extendedCoordinate160;
        g_UiCommandDragStartScreenY = mapControl->extendedCoordinate164;
        nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
        scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
        worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
        g_UiCommandDragAnchorWorldXQ12 =
             (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff &
             0xfffff000;
        g_UiCommandDragAnchorWorldYQ12 = worldCoordinateTerm * 2 + 0x3ff & 0xfffff000;
        return;
      }
      g_UiCommandDragStartScreenX = 0x7fffffff;
      return;
    }
    if (g_UiCommandModeE == 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      mapFieldGrid = mapControl->fieldGrid;
      cellX = (int)((((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff
                   ) >> 0xc;
      if (cellX < 0) {
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        return;
      }
      cellY = (int)(worldCoordinateTerm * 2 + 0x3ff) >> 0xc;
      if (cellY < 0) {
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        return;
      }
      if (cellX < (int)mapFieldGrid->gridWidth) {
        if (cellY < (int)mapFieldGrid->gridHeight) {
          g_UiCommandTerrainMaskToggleValue =
               mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
               FIELD_CELL_FLUID_RECEIVER_EXCLUDED ^ FIELD_CELL_FLUID_RECEIVER_EXCLUDED;
          return;
        }
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        return;
      }
      g_UiCommandTerrainMaskToggleValue = 0x20000000;
      return;
    }
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    mapFieldGrid = mapControl->fieldGrid;
    cellX = (int)((((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff)
            >> 0xc;
    if (cellX < 0) {
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    cellY = (int)(worldCoordinateTerm * 2 + 0x3ff) >> 0xc;
    if (cellY < 0) {
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    if (cellX < (int)mapFieldGrid->gridWidth) {
      if (cellY < (int)mapFieldGrid->gridHeight) {
        g_UiCommandTerrainMaskToggleValue =
             mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
             FIELD_CELL_FLUID_SOURCE_EXCLUDED ^ FIELD_CELL_FLUID_SOURCE_EXCLUDED;
        return;
      }
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    g_UiCommandTerrainMaskToggleValue = 0x40000000;
    return;
  case 3:
    commandPayload = g_UiCommandModeGOwnerFactionIndex;
    lookupToken = g_UiCommandModeGArmyAssetId;
    placementSubMode = g_UiCommandModeA;
    goto joined_r0x005701bf;
  case 4:
    commandPayload = 0;
    lookupToken = g_UiCommandMode4ArmyAssetId;
    placementSubMode = g_UiCommandModeB;
joined_r0x005701bf:
    if (placementSubMode == 0) {
      if (pointerRegionCode != 0x7fffffff) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetState8090(g_LocalPlayerRuntimeId,0,0,commandPayload);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ec0,0,0,commandPayload);
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_ResolveAndStoreState8094
                    (g_LocalPlayerRuntimeId,pointerX,pointerY,lookupToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2e50,pointerX,pointerY,lookupToken);
        }
        g_UiCommandDragReferenceX = pointerY;
        g_UiCommandDragReferenceY = pointerX;
        g_UiCommandDragStartScreenX = mapControl->extendedCoordinate160;
        g_UiCommandDragStartScreenY = mapControl->extendedCoordinate164;
        return;
      }
      return;
    }
    if (placementSubMode == 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
        commandPayload = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ApplyEntryOrAll(g_LocalPlayerRuntimeId,0,0,commandPayload);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand(0x2de0,0,0,commandPayload);
        return;
      }
    }
    else {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
        g_UiCommandDragStartScreenX = mapControl->extendedCoordinate160;
        g_UiCommandDragStartScreenY = mapControl->extendedCoordinate164;
        commandPayload = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetState8094(g_LocalPlayerRuntimeId,0,0,commandPayload);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ef0,0,0,commandPayload);
        }
        g_UiCommandDragReferenceX = pointerY;
        g_UiCommandDragReferenceY = pointerX;
        return;
      }
    }
    mapControl->runtimeFlags = mapControl->runtimeFlags | 0x80;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
    InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
    return;
  case 5:
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    mapFieldGrid = mapControl->fieldGrid;
    cellX = (int)((((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm) + 0x3ff)
            >> 0xc;
    if (cellX < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    cellY = (int)(worldCoordinateTerm * 2 + 0x3ff) >> 0xc;
    if (cellY < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    if (cellX < (int)mapFieldGrid->gridWidth) {
      if ((int)mapFieldGrid->gridHeight <= cellY) {
        g_UiCommandCallerMaskHighBit = 0;
        return;
      }
      if ((mapFieldGrid->cells[cellY * mapFieldGrid->gridWidth + cellX].flagsAndMaterial &
          0x800 << ((byte)g_UiCommandModeF & 0x1f)) != 0) {
        g_UiCommandCallerMaskHighBit = 0x80000000;
        return;
      }
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    g_UiCommandCallerMaskHighBit = 0;
    return;
  }
  g_UiCommandSelectionAnchorWorldXQ12 = 0x7fffffff;
  if (pointerRegionCode != 0x7fffffff) {
    if ((g_KeyboardStateMask & 0xf) == 0) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1ed0,0,0,0);
      }
    }
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    worldCoordinateTerm = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    g_UiCommandSelectionAnchorWorldXQ12 =
         ((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - worldCoordinateTerm;
    g_UiCommandSelectionAnchorWorldYQ12 = worldCoordinateTerm * 2;
    g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    g_UiCommandSelectionCurrentWorldYQ12 = g_UiCommandSelectionAnchorWorldYQ12;
  }
  return;
}


/* Address: 0x005703D0.
   Ownership: ui/ingame/runtime.
   Purpose: Typed parameters: p2 pointerRegionCode→UiPointerRegionCode_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p3 pointerX→GraphicsScreenCoordinate_V307, p4
   pointerY→GraphicsScreenCoordinate_V307. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p7
   mapControl→WorldRuntimeExtendedMapControlAddress32_V345.
   Cross-module calls: WorldRuntimeNode_IsPositionInsideBoundsCf [world/runtime/core], SelectionInfo_FindEntryCf
   [gameplay/selection/runtime], InGameCommandQueue_ContainsTripletValueCf [network/protocol/commands],
   FrontendPlayerSelection_RemoveThreeEntriesAndRefresh [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerSelection_InsertThreeEntriesAndRefresh [ui/frontend/player].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_UpdateInteractionByMode
          (UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,dword reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView170 *mapControl)

{
  longlong scaledGridX;
  longlong scaledGridY;
  dword placementSubMode;
  int boundWorldX;
  CommandPayloadDword04 payloadDword04;
  int boundWorldY;
  int workValue;
  uint encodedValue;
  int upperWorldY;
  CommandPayloadDword08 rowOrDeltaValue;
  uint columnValue;
  int lowerWorldY;
  InGameCommandPayloadTripletValue32 payloadValue;
  undefined4 *tripletClearCursor;
  WorldOwnerListNode100 *runtimeNode;
  CommandPayloadDword04 *tripletEntry;
  bool conditionResult;
  FieldGridNearestPointRegsCf13 nearestTerrainPoint;
  GameEntityRuntime *entry;
  
  if ((mapControl->runtimeFlags & 0x80) != 0) {
    tripletClearCursor = (undefined4 *)&g_InGameSelectionInsertTripletDwords;
    for (workValue = 0x1a; workValue != 0; workValue = workValue + -1) {
      *tripletClearCursor = 0;
      tripletClearCursor = tripletClearCursor + 1;
    }
    runtimeNode = (WorldOwnerListNode100 *)mapControl->ownerListHead;
    workValue = mapControl->activeFactionRuntimeIndex;
    if (runtimeNode == (WorldOwnerListNode100 *)0x0) {
      return;
    }
    do {
      if ((((runtimeNode->runtimeFlags & 2) != 0) &&
          (entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
          (runtimeNode->runtimeFlags & 0x20) != 0)) &&
         (workValue == (entry->common).ownership.ownerIndex)) {
        payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
        conditionResult = WorldRuntimeNode_IsPositionInsideBoundsCf(runtimeNode,mapControl);
        if (conditionResult) {
          conditionResult = SelectionInfo_FindEntryCf(entry);
          encodedValue = g_InGameSelectionInsertTripletDwordCount;
          if (((conditionResult) &&
              (conditionResult = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90), !conditionResult))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionInsertTripletDwords + encodedValue * 4) = payloadValue, encodedValue < 0xb))
          {
            g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount + 1;
          }
        }
        else {
          conditionResult = SelectionInfo_FindEntryCf(entry);
          encodedValue = g_InGameSelectionRemoveTripletDwordCount;
          if (((!conditionResult) &&
              (conditionResult = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30), !conditionResult))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionRemoveTripletDwords + encodedValue * 4) = payloadValue, encodedValue < 0xb))
          {
            g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount + 1;
          }
        }
      }
      runtimeNode = runtimeNode->nextNode;
    } while (runtimeNode != (WorldOwnerListNode100 *)0x0);
    if (g_InGameSelectionRemoveTripletDwordCount != 0) {
      tripletEntry = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
      do {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                    (g_LocalPlayerRuntimeId,tripletEntry[2],tripletEntry[1],*tripletEntry);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xb00,tripletEntry[2],tripletEntry[1],*tripletEntry);
        }
        encodedValue = g_InGameSelectionRemoveTripletDwordCount;
        tripletEntry = tripletEntry + 3;
        g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount - 3;
      } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)encodedValue);
    }
    if (g_InGameSelectionInsertTripletDwordCount == 0) {
      return;
    }
    tripletEntry = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
    do {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,tripletEntry[2],tripletEntry[1],*tripletEntry);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xa60,tripletEntry[2],tripletEntry[1],*tripletEntry);
      }
      encodedValue = g_InGameSelectionInsertTripletDwordCount;
      tripletEntry = tripletEntry + 3;
      g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount - 3;
    } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)encodedValue);
    return;
  }
                    // WARNING: Switch is manually overridden
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      encodedValue = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        encodedValue = 0;
      }
      encodedValue = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX & encodedValue |
              (mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY) * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x1f70,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyPositiveCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeC == 1) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      encodedValue = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        encodedValue = 0;
      }
      encodedValue = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX & encodedValue |
              (mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY) * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x2290,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyNegativeCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      columnValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      encodedValue = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - columnValue) + 0x3ff &
              0xfffff000;
      columnValue = columnValue * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2ae0,0,columnValue,encodedValue);
        return;
      }
      FieldGrid_RebuildLocalInfluenceState(g_LocalPlayerRuntimeId,0,columnValue,encodedValue);
      return;
    }
    break;
  case 1:
    if (g_UiCommandModeD != 3) {
      if (g_TerrainMaterialTextureSets[g_UiCommandAbsoluteSelectionIndex] ==
          (GraphicsTextureSet *)0x0) {
        return;
      }
      if (g_UiCommandModeD == 1) {
        return;
      }
      if (g_UiCommandModeD == 2) {
        return;
      }
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      columnValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      encodedValue = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - columnValue) + 0x3ff &
              0xfffff000;
      columnValue = columnValue * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x2770,g_UiCommandAbsoluteSelectionIndex,columnValue,encodedValue);
        return;
      }
      FieldGrid_ApplyLocalCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,columnValue,encodedValue);
      return;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      encodedValue = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        encodedValue = 0;
      }
      columnValue = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX;
      workValue = mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY;
      g_UiCommandDragStartScreenX = g_UiCommandDragStartScreenX + columnValue;
      g_UiCommandDragStartScreenY = g_UiCommandDragStartScreenY + workValue;
      encodedValue = columnValue & encodedValue | workValue * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x3260,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,encodedValue);
        return;
      }
      FieldGrid_ApplyEncodedCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,encodedValue);
      return;
    }
    if (g_UiCommandModeE != 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
      scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
      columnValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
      encodedValue = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - columnValue) + 0x3ff &
              0xfffff000;
      columnValue = columnValue * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x32e0,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
        return;
      }
      FieldGrid_ApplyMaskBFFFFFFF
                (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
      return;
    }
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    columnValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    encodedValue = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - columnValue) + 0x3ff &
            0xfffff000;
    columnValue = columnValue * 2 + 0x3ff & 0xfffff000;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0x32a0,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
      return;
    }
    FieldGrid_ApplyMaskDFFFFFFF
              (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,columnValue,encodedValue);
    return;
  case 3:
    placementSubMode = g_UiCommandModeA;
    goto joined_r0x00570b27;
  case 4:
    placementSubMode = g_UiCommandModeB;
joined_r0x00570b27:
    if ((placementSubMode != 0) && (placementSubMode == 1)) {
      return;
    }
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    if ((g_CursorButtonState & 4) != 0) {
      g_UiCommandDragStartScreenX = mapControl->extendedCoordinate168;
      g_UiCommandDragStartScreenY = mapControl->extendedCoordinate16C;
      payloadDword04 = pointerY - g_UiCommandDragReferenceX;
      rowOrDeltaValue = pointerX - g_UiCommandDragReferenceY;
      g_UiCommandDragReferenceX = g_UiCommandDragReferenceX + payloadDword04;
      g_UiCommandDragReferenceY = g_UiCommandDragReferenceY + rowOrDeltaValue;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2f20,0,rowOrDeltaValue,payloadDword04);
        return;
      }
      SelectionPlayerRuntime_ReissuePrimarySelectionPosition
                (g_LocalPlayerRuntimeId,0,rowOrDeltaValue,payloadDword04);
      return;
    }
    workValue = (*(code *)g_PointerSetPosition)(g_UiCommandDragStartScreenY,g_UiCommandDragStartScreenX);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand(0x30f0,0,0,workValue << 6);
      return;
    }
    SelectionPlayerRuntime_AdvancePrimarySelectionCycle(g_LocalPlayerRuntimeId,0,0,workValue << 6);
    return;
  case 5:
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    columnValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    encodedValue = (((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - columnValue) + 0x3ff &
            0xfffff000;
    columnValue = columnValue * 2 + 0x3ff & 0xfffff000;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0x3320,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,columnValue,encodedValue);
      return;
    }
    FieldGrid_ApplyCallerMask
              (g_LocalPlayerRuntimeId,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,columnValue,encodedValue);
    return;
  }
  if ((pointerRegionCode != 0x7fffffff) && (g_UiCommandSelectionAnchorWorldXQ12 != 0x7fffffff)) {
    nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    scaledGridX = (longlong)(int)nearestTerrainPoint.eax * 0x1c6e9c;
    scaledGridY = (longlong)(int)nearestTerrainPoint.ecx * -0x20c8cc;
    encodedValue = (int)((ulonglong)scaledGridY >> 0x20) << 0xb | (uint)scaledGridY >> 0x15;
    boundWorldY = ((int)((ulonglong)scaledGridX >> 0x20) << 0xc | (uint)scaledGridX >> 0x14) - encodedValue;
    workValue = encodedValue * 2;
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
    encodedValue = boundWorldX + 0x3ffU & 0xfffff000;
    rowOrDeltaValue = lowerWorldY + 0x3ffU & 0xfffff000;
    columnValue = g_UiCommandSelectionCurrentWorldXQ12 + 0x3ffU & 0xfffff000;
    g_UiCommandSelectionCurrentWorldXQ12 = boundWorldY;
    g_UiCommandSelectionCurrentWorldYQ12 = workValue;
    if ((int)encodedValue <= (int)columnValue) {
      for (; (int)rowOrDeltaValue <= (int)(upperWorldY + 0x3ffU & 0xfffff000); rowOrDeltaValue = rowOrDeltaValue + 0x1000) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_RemoveRange(g_LocalPlayerRuntimeId,columnValue,rowOrDeltaValue,encodedValue);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d40,columnValue,rowOrDeltaValue,encodedValue);
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
    encodedValue = workValue + 0x3ffU & 0xfffff000;
    rowOrDeltaValue = upperWorldY + 0x3ffU & 0xfffff000;
    columnValue = boundWorldX + 0x3ffU & 0xfffff000;
    if ((int)encodedValue <= (int)columnValue) {
      for (; (int)rowOrDeltaValue <= (int)(boundWorldY + 0x3ffU & 0xfffff000); rowOrDeltaValue = rowOrDeltaValue + 0x1000) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_InsertRange(g_LocalPlayerRuntimeId,columnValue,rowOrDeltaValue,encodedValue);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d00,columnValue,rowOrDeltaValue,encodedValue);
        }
      }
    }
  }
  return;
}


/* Address: 0x00570D60.
   Ownership: ui/ingame/runtime.
   Purpose: Handles in game ui command end interaction by mode.
   Cross-module calls: TerrainEditBuffer_ConvertHeightsToDeltas [world/terrain/editing],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   TerrainEditBuffer_SubtractCurrentCellMaterialBytes [world/terrain/editing], PlayerRuntime_ClearState8094
   [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_EndInteractionByMode
          (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3,
          WorldOwnerListNode100 *worldNode,WorldRuntimeContext *worldRuntime)

{
  dword activeMode;
  
  activeMode = g_UiCommandModeG;
  worldRuntime->runtimeFlags = worldRuntime->runtimeFlags & 0xffffff7f;
                    // WARNING: Switch is manually overridden
  switch(activeMode) {
  case 0:
    if (g_UiCommandModeC == 0) {
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags | 0x100000;
    }
    else if (g_UiCommandModeC == 1) {
      worldRuntime->runtimeFlags = worldRuntime->runtimeFlags | 0x100000;
    }
    else if (g_UiCommandModeC == 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_ConvertHeightsToDeltas(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2c90,0,0,0);
      }
    }
    break;
  case 1:
    if (((g_UiCommandModeD != 3) && (g_UiCommandModeD != 1)) && (g_UiCommandModeD != 2)) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_SubtractCurrentCellMaterialBytes(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2800,0,0,0);
      }
    }
    break;
  case 3:
    activeMode = g_UiCommandModeA;
    goto joined_r0x00570ef7;
  case 4:
    activeMode = g_UiCommandModeB;
joined_r0x00570ef7:
    if ((activeMode == 0) || (activeMode != 1)) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        PlayerRuntime_ClearState8094(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x3190,0,0,0);
      }
    }
  }
  return;
}


/* Address: 0x00570F30.
   Ownership: ui/ingame/runtime.
   Purpose: Handles in game ui command reset interaction by mode.
   Cross-module calls: SelectionPlayerRuntime_ClearTerrainEditSelectionState [gameplay/selection/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerSelection_ClearAndRefreshLocalPanels [ui/frontend/player].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime)

{
                    // WARNING: Switch is manually overridden
  switch(g_UiCommandModeG) {
  case 0:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1ed0,0,0,0);
    }
    break;
  case 1:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1ed0,0,0,0);
    }
    break;
  case 3:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
    }
  }
  return;
}


/* Address: 0x005609F0.
   Ownership: ui/ingame/runtime.
   Purpose: Activates or deactivates the in-game command/UI interaction subsystem. The immutable body synchronizes
   page-stack active state, installs the command/camera dispatch roots, rebuilds command-mode/grid state and
   selection presentation, and tears the subsystem down on the inactive path.
   Local calls: InGameSelectionDetailPanel_Rebuild.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiPageStack_ActivePageNotInListCf
   [ui/controls/lists], Movie_Close [movie/runtime/playback], UiCommandMatrix_SelectIndex [ui/ingame/commands],
   ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf [assets/army/catalog],
   ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf [assets/army/catalog].
*/

void __thandor_void_preserve_eax_ecx_edx
InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  WorldRuntimeFlags *runtimeFlagsField;
  WorldRuntimeContext *node;
  GraphicsTextureSet *materialTextureSet;
  ArmyAssetRecordPrefix *armyAsset;
  byte *pageStackBlock;
  InGameRuntimeRootImageC3E4 *root;
  dword modeOrValue;
  int remainingCount;
  GraphicsTextureSourceAsset *textureSourceValue;
  InGameNotificationQueueRecord20 *queueSlotCursor;
  TerrainDirectionRecord *directionRecord;
  FieldGridCell *fieldCellCursor;
  ArmyAssetRecordPrefix **registrySlot;
  StatusValueEaxCf5 pageNotInListResult;
  ArmyRegistryIdEaxCf5_5719f0 normalizedModeGArmy;
  ArmyRegistryIdEaxCf5_571c30 normalizedMode4Army;
  FieldGridAsset *worldFieldGrid;
  
  modeOrValue = g_UiCommandModeG;
  root = g_InGameRuntimeRoot;
  if ((commandArg3 & 4) == 0) {
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      pageStackBlock = g_InGameRuntimeRoot->opaque9A74_9B4B;
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 5;
      runtimeFlagsField = &(g_InGameRuntimeRoot->worldRuntime0A30).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | 0x400000;
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGPrimaryPageIndices[modeOrValue],(UiPageStackControl *)(pageStackBlock + 0x18));
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGSecondaryPageIndices[modeOrValue],
                 (UiPageStackControl *)root->opaque9EE0_9FAB);
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGTertiaryPageIndices[modeOrValue],
                 (UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
      pageNotInListResult = UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
      if (pageNotInListResult.valueOrError == 0) {
        UiPageStack_SetActiveIndex(1,&root->optionalUiPageStack4530);
        UiPageStack_SetActiveIndex(1,&root->optionalUiPageStack4644);
      }
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)(root->opaque4100_452F + 0x22c));
      root->worldOverlayCallback0B8C = (InGameWorldOverlayRebuildCallbackProc *)0x0;
      (root->worldRuntime0A30).selection.dispatchCommandCallback =
           InGameCameraCommand_DispatchByCodeAndModifierFlagsCf;
      (root->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
           InGameUiCommand_ResolveCursorCodeByMode;
      (root->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
           InGameUiCommand_ResolveCursorCodeByMode;
      (root->worldRuntime0A30).selection.beginPointerCaptureCallback =
           InGameUiCommand_BeginInteractionByMode;
      (root->worldRuntime0A30).selection.updateDragSelectionCallback =
           InGameUiCommand_UpdateInteractionByMode;
      (root->worldRuntime0A30).selection.commitPointerActionCallback =
           InGameUiCommand_EndInteractionByMode;
      (root->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
           UiCommandRuntime_CallbackNoOp;
      (root->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
           InGameUiCommand_ResetInteractionByMode;
      g_UiRootCallbacks_0054FBC0.keyboardFallbackCf =
           InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf;
      (*(code *)g_UiCommandModeGHandlers[modeOrValue])
                (root->opaque0058_017B + g_UiCommandModeGControlOffsets[modeOrValue] + -0x58);
      queueSlotCursor = root->notificationQueue9E60;
      for (remainingCount = 0x20; remainingCount != 0; remainingCount = remainingCount + -1) {
        queueSlotCursor->notificationMovieId00 = 0;
        queueSlotCursor = (InGameNotificationQueueRecord20 *)&queueSlotCursor->priority04;
      }
      Movie_Close();
      modeOrValue = g_UiCommandAbsoluteSelectionIndex;
      textureSourceValue = g_InGamePanelTextureSource;
      if (root->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE) {
        root->sessionNotificationInteractionState9B4C = NOTIFICATION_INTERACTION_NONE;
      }
      materialTextureSet = g_TerrainMaterialTextureSets[modeOrValue];
      root->observedSessionNotificationValue9B50 = (dword)textureSourceValue;
      textureSourceValue = (GraphicsTextureSourceAsset *)0x0;
      if (materialTextureSet != (GraphicsTextureSet *)0x0) {
        textureSourceValue = materialTextureSet->entries[0].sourceAsset;
      }
      root->notificationPlaybackCompletionCode9B54 = 0x25;
      *(GraphicsTextureSourceAsset **)(root->opaque9B58_9E3F + 0xb8) = textureSourceValue;
      UiCommandMatrix_SelectIndex(g_UiCommandAbsoluteSelectionIndex,(UiNodeBase *)root);
      normalizedModeGArmy = ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
      g_UiCommandModeGArmyAssetId = normalizedModeGArmy.eax;
      modeOrValue = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x1cc) = modeOrValue;
      normalizedMode4Army = ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
      g_UiCommandMode4ArmyAssetId = normalizedMode4Army.eax;
      modeOrValue = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x284) = modeOrValue;
      FieldGrid_SetOccupancyMaskByteBit0AllCells
                ((root->worldRuntime0A30).activeFactionRuntimeIndex,
                 (root->worldRuntime0A30).fieldGrid);
      WorldRuntime_ForEachNodeInOwnerListD8
                (&root->worldRuntime0A30,
                 ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
                 &root->worldRuntime0A30);
      FieldGrid_ClassifyCellFlagsToRuntimeByte
                ((root->worldRuntime0A30).activeFactionRuntimeIndex,
                 (root->worldRuntime0A30).fieldGrid);
      remainingCount = 0x100;
      directionRecord = g_TerrainDirectionRecordTable256;
      do {
        directionRecord->angleAComponent0ScaledQ28 = 0;
        directionRecord->angleAComponent1ScaledQ28 = 0;
        directionRecord->angleBComponent0ScaledQ28 = 0;
        directionRecord = directionRecord + 1;
        remainingCount = remainingCount + -1;
      } while (remainingCount != 0);
      worldFieldGrid = (root->worldRuntime0A30).fieldGrid;
      remainingCount = worldFieldGrid->gridWidth * worldFieldGrid->gridHeight;
      fieldCellCursor = worldFieldGrid->cells;
      do {
        fieldCellCursor->armyRuntimeSavedOffset6C = 0;
        fieldCellCursor->resourceExtractionDescriptor7C = 0;
        fieldCellCursor = fieldCellCursor + 1;
        remainingCount = remainingCount + -1;
      } while (remainingCount != 0);
    }
  }
  else if ((g_UiCommandRuntimeFlags & 4) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffffffa;
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x18));
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)root->opaque9EE0_9FAB);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
    pageNotInListResult = UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
    if (pageNotInListResult.valueOrError == 0) {
      UiPageStack_SetActiveIndex(0,&root->optionalUiPageStack4530);
      UiPageStack_SetActiveIndex(0,&root->optionalUiPageStack4644);
    }
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(root->opaque4100_452F + 0x22c));
    UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)&root->worldRuntime0A30);
    root->worldOverlayCallback0B8C = InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
    (root->worldRuntime0A30).selection.dispatchCommandCallback =
         InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
    (root->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
         InGameWorldInput_ResolveContextActionAndCursorCf;
    (root->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
         InGameWorldInput_ResolveContextActionAndCursorCf;
    (root->worldRuntime0A30).selection.beginPointerCaptureCallback =
         InGameWorldInput_BeginPointerCaptureCf;
    (root->worldRuntime0A30).selection.updateDragSelectionCallback =
         InGameWorldInput_UpdateDragSelectionAndCameraCf;
    (root->worldRuntime0A30).selection.commitPointerActionCallback =
         InGameWorldInput_CommitPointerActionCf;
    (root->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
         InGameUiRuntime_ClearTransientState1BCallback;
    (root->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
         InGameUiRuntime_DispatchWorldContextActionCallback;
    runtimeFlagsField = &(root->worldRuntime0A30).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | 0x400;
    g_UiRootCallbacks_0054FBC0.keyboardFallbackCf = EndGameResultsUiRuntime_DispatchCommandByFlagsCf
    ;
    registrySlot = g_ArmyAssetRecordRegistry;
    remainingCount = 0x300;
    do {
      armyAsset = *registrySlot;
      if (armyAsset != (ArmyAssetRecordPrefix *)0x0) {
        (*g_MemoryApi.free)((void *)armyAsset[2].byteSize);
        armyAsset[2].byteSize = 0;
      }
      registrySlot = registrySlot + 1;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    if ((g_UiCommandRuntimeFlags & 8) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells
                ((root->worldRuntime0A30).activeFactionRuntimeIndex,
                 (root->worldRuntime0A30).fieldGrid);
    }
    WorldRuntime_ForEachNodeInOwnerListD8
              (&root->worldRuntime0A30,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
               ,&root->worldRuntime0A30);
    node = &root->worldRuntime0A30;
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((root->worldRuntime0A30).activeFactionRuntimeIndex,(root->worldRuntime0A30).fieldGrid
              );
    UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)node);
    UiCommandModeG_ClearNodeFlag00200000((UiNodeBase *)node);
    UiCommandModeG_SetNodeFlag00000400((UiNodeBase *)node);
    UiCommandModeG_ClearNodeFlag01000000((UiNodeBase *)node);
    UiCommandModeG_ApplyRawColorVariant(node);
    UiCommandModeG_ClearNodeFlag00800000((UiNodeBase *)node);
    UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
    runtimeFlagsField = &(root->worldRuntime0A30).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & 0xffbfffff;
    TerrainDirectionTable_AdvanceAndRebuildVectors();
    g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)0x0;
    InGameSelectionDetailPanel_Rebuild();
  }
  return;
}


/* Address: 0x005622F0.
   Ownership: ui/ingame/runtime.
   Purpose: Command helper that saves the current field-grid/runtime asset image and the level-runtime asset image,
   routing carry-flag failures through the installed fatal-error path.
   Cross-module calls: FieldGrid_SaveAssetImageFromRuntimeStateCf [world/terrain/grid],
   InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf [gameplay/session/level].
*/
void __thandor_preserve_eax
InGameUiCommand_SaveFieldAndLevelAssetImages
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  StatusValueEaxCf5 saveStatus;
  
  runtimeRoot = g_InGameRuntimeRoot;
  saveStatus = FieldGrid_SaveAssetImageFromRuntimeStateCf
                    ((dword *)(g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid);
  if (saveStatus.carry) {
    (*g_FatalErrorRuntimeDispatchCf)(saveStatus.valueOrError,true);
  }
  saveStatus = InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf
                    ((InGameLevelSaveWorldView *)&runtimeRoot->worldRuntime0A30);
  if (saveStatus.carry) {
    (*g_FatalErrorRuntimeDispatchCf)(saveStatus.valueOrError,true);
  }
  return;
}


/* Address: 0x00567040.
   Ownership: ui/ingame/runtime.
   Purpose: Takes a UTF-16 text pointer in EAX, inserts it into the shared recent-text history, and rebuilds the
   in-game eight-entry pointer list at activeGameState+0x9B8. EAX is preserved.
   Cross-module calls: RecentTextHistory_Insert [ui/support/runtime], RecentTextHistory_SortAndBuildPointerList
   [ui/support/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRecentTextHistory_InsertAndRebuild8(word *text)

{
  RecentTextHistoryPointerList *output;
  
  output = &g_InGameRuntimeRoot->recentTextHistory09B8;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(8,output);
  return;
}


/* Address: 0x0056B850.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, clears suppression on the command-page container at root+0xA78, and selects page 0
   in the page stack at root+0xBD0. Queued UI action handler for INGAME_PAGE10[2] (0x1002). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void __thandor_preserve_eax InGameSevenSlotCommand_ClosePage(UiNodeBase *source)

{
  UiNodeBase *parentCursor;
  
  parentCursor = source->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentCursor = source->parent;
  }
  source[0x23].top = source[0x23].top & 0xfffffff7;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&source[0x27].bottomAnchorQ31);
  return;
}


/* Address: 0x0056B890.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, converts the rich UTF-16 command text at root+0x1D04 into a 48-byte payload, derives
   a seven-slot mask from the selected mode, sends the mask and four twelve-byte payload chunks through the active
   backend, finalizes the command, and clears the form state. Queued UI action handler for INGAME_PAGE10[4]
   (0x1004). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext],
   UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], FrontendPlayerTextCommand_SetPackedState [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerTextCommand_AppendTripleClamped [ui/frontend/player].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source)

{
  sdword *formBase;
  int *controlOffsetEntry;
  UiNodeBase *ancestorParent;
  int offsetOrCount;
  uint slotIndex;
  CommandPayloadDword04 packedState;
  uint slotBit;
  UiAnchorFractionQ31 *textCursor;
  bool isSelected;
  UiSelectableNodeEaxEcxCf9 visibleSelection;
  
  ancestorParent = source->parent;
  while (ancestorParent != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    ancestorParent = source->parent;
  }
  formBase = &source[0x60].right;
  RichTextCommandStream_CopyToNarrowCf
            (0x30,g_UiSevenSlotCommandPayloadText.textBytes,(word *)&source[0x61].rightAnchorQ31);
  visibleSelection = UiSelectableGroup_NoneVisibleSelectedCf(3,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x2ac),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x24c),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x1ec));
  offsetOrCount = (int)visibleSelection.node - (int)formBase;
  if (offsetOrCount == 0x1ec) {
    slotIndex = 0;
    packedState = 0;
    slotBit = 0x100;
    do {
      controlOffsetEntry = g_UiSevenSlotSelectionControlOffsets + slotIndex;
      slotBit = slotBit * 2;
      slotIndex = slotIndex + 1;
      isSelected = (bool)UiSelectableControl_IsSelectedCf
                              ((UiSelectableControl *)(*controlOffsetEntry + -0x1c98 + (int)formBase));
      if (isSelected) {
        packedState = packedState | slotBit;
      }
    } while (slotIndex < 7);
  }
  else if (offsetOrCount == 0x24c) {
    slotIndex = 0;
    packedState = 0;
    slotBit = 0x8000;
    do {
      controlOffsetEntry = g_UiSevenSlotSelectionControlOffsets + slotIndex;
      slotBit = slotBit * 2;
      slotIndex = slotIndex + 1;
      isSelected = (bool)UiSelectableControl_IsSelectedCf
                              ((UiSelectableControl *)(*controlOffsetEntry + -0x1c98 + (int)formBase));
      if (isSelected) {
        packedState = packedState | slotBit;
      }
    } while (slotIndex < 7);
  }
  else {
    packedState = 0xffffff00;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_SetPackedState(g_LocalPlayerRuntimeId,0,0,packedState);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(6000,0,0,packedState);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (0x17a0,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (0x17a0,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (0x17a0,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_AppendTripleClamped
              (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (0x17a0,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
               g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
               g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_PublishConditionalRichText(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x1810,0,0,0);
  }
  source[0x61].rightOffset = 0;
  source[0x61].bottomOffset = 0;
  source[0x61].leftAnchorQ31 = 0;
  textCursor = &source[0x61].rightAnchorQ31;
  for (offsetOrCount = 0x18; offsetOrCount != 0; offsetOrCount = offsetOrCount + -1) {
    *textCursor = 0;
    textCursor = textCursor + 1;
  }
  return;
}


/* Address: 0x005669B0.
   Ownership: ui/ingame/runtime.
   Purpose: Rebuilds the in-game selection-detail page for no selection, one selected entity, multiple selected
   entities, or the hover record. It updates action 0x1010, localized weapon/name fields, costs, icons, and detail
   pages.
   Cross-module calls: FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf [ui/frontend/player],
   UiNodeList_UnsuppressActionId [ui/controls/lists], Technology_IsAvailableForFactionCf
   [gameplay/technology/runtime], UiNodeList_SuppressActionId [ui/controls/lists], ArmyAssetRegistry_FindByIdCf
   [assets/army/catalog], UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSelectionDetailPanel_Rebuild(void)

{
  UiPageStackControl *stack;
  byte *clearedControlBytes;
  GraphicsTextureSourceAsset *hoverTextureSource;
  uint buildDuration;
  ModelRuntimeSlot *attachedModelRuntime;
  UiCommandRuntimeRecordPrefix *definitionNode;
  int selectedCountOrCounter;
  dword detailValue;
  EnergyDemandQ4 displayedEnergy;
  word *sourceText;
  uint metricValue;
  ArmyAssetRecordPrefix *linkedArmyAsset;
  int slotCounterOrOffset;
  int workValue;
  ModelDefinitionHierarchyNodeAddress32 hierarchyNodeAddress;
  ModelLinkedDefinitionBranchView18 *linkedDefinitionListView;
  InGameRuntimeRootImageC3E4 *rootCursor;
  GameEntityRuntime *lastSelectedEntity;
  int *recordCursor;
  GameEntityRuntime **entitySlot;
  word *destinationText;
  bool conditionResult;
  ArmyRegistryEaxCf5_51b6d0 foundArmyAsset;
  FatalErrorEaxCf5 armyAssetResult;
  TextResourceResolveEaxCf5 resolvedText;
  ModelDefinitionLookupEaxCf5 unlockedDefinition;
  ModelRuntimeSlot *selectedModelRuntime;
  ModelRuntimeSlot *selectedModelRuntimeTail;
  ModelRuntimeSlot *selectedModelRuntimeTail2;
  
  definitionNode = g_UiHoverSelectionRecord;
  rootCursor = g_InGameRuntimeRoot;
  if (g_InGameRuntimeRoot == (InGameRuntimeRootImageC3E4 *)0x0) {
    return;
  }
  slotCounterOrOffset = 0x20;
  workValue = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  selectedCountOrCounter = 0;
  lastSelectedEntity = (GameEntityRuntime *)0x0;
  entitySlot = g_SelectionInfoEntitySlots->entries;
  do {
    if (*entitySlot != (GameEntityRuntime *)0x0) {
      selectedCountOrCounter = selectedCountOrCounter + 1;
      lastSelectedEntity = *entitySlot;
    }
    entitySlot = entitySlot + 1;
    slotCounterOrOffset = slotCounterOrOffset + -1;
  } while (slotCounterOrOffset != 0);
  stack = &g_InGameRuntimeRoot->selectionDetailPageStack9FAC;
  if (g_UiHoverSelectionRecord == (UiCommandRuntimeRecordPrefix *)0x0) {
    if (selectedCountOrCounter == 1) {
      if (workValue == (lastSelectedEntity->common).ownership.ownerIndex) {
        recordCursor = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        workValue = *recordCursor;
        conditionResult = FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf
                           ((RuntimeToken)recordCursor,
                            (g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId)
        ;
        if (!conditionResult) {
          UiNodeList_UnsuppressActionId(0x1010,(UiNodeBase *)rootCursor);
          selectedCountOrCounter = 0x1c;
          do {
            conditionResult = Technology_IsAvailableForFactionCf
                               (*(PckTechnologyIdCatalog *)(workValue + 0x1c4 + selectedCountOrCounter * 4),
                                (lastSelectedEntity->common).ownership.ownerIndex);
            if (conditionResult)
            goto InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails;
            selectedCountOrCounter = selectedCountOrCounter + -1;
          } while (selectedCountOrCounter != 0);
        }
        UiNodeList_SuppressActionId(0x1010,(UiNodeBase *)rootCursor);
InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails:
        foundArmyAsset = ArmyAssetRegistry_FindByIdCf((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId)
        ;
        armyAssetResult = (*g_FatalErrorPrimaryDispatchCf)((dword)foundArmyAsset.eax,foundArmyAsset.carry);
        UiPageStack_SetActiveIndex(1,stack);
        detailValue = *(dword *)(armyAssetResult.eax + 0x1c);
        workValue = ModelRuntimeHierarchy_SumMetric3C((int *)lastSelectedEntity);
        rootCursor->selectionDetailArmyAssetValueA060 = detailValue;
        rootCursor->selectionDetailEntityA068 = lastSelectedEntity;
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,workValue,g_InGameSelectionDetailArmourTextUtf16
                  );
        metricValue = ModelRuntime_QueryActiveHierarchyMetric((ArmyRuntimeSlot *)lastSelectedEntity);
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,metricValue >> 4,
                   g_InGameSelectionDetailEnergyTextUtf16);
        *(int *)(rootCursor->opaqueA06C_C3E3 + 0x54) = *(int *)(armyAssetResult.eax + 4) + 0x18002c;
        resolvedText = TextResource_Resolve
                           (*(int *)(*(int *)(lastSelectedEntity->common).ownership.
                                             definitionOrClassRecord + 4) + 0x18004f);
        sourceText = resolvedText.eax;
        destinationText = g_InGameSelectionDetailNameTextUtf16;
        for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
          *destinationText = *sourceText;
          sourceText = sourceText + 1;
          destinationText = destinationText + 1;
        }
        resolvedText = TextResource_Resolve(0x18004e);
        sourceText = resolvedText.eax;
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName0TextUtf16,sourceText);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName1TextUtf16,sourceText);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName2TextUtf16,sourceText);
        g_InGameSelectionDetailTextSlot05Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot05Utf16[1] = 0;
        g_InGameSelectionDetailTextSlot09Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot09Utf16[1] = 0;
        selectedModelRuntime = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        if (((selectedModelRuntime->classState).classStateEC & 0x40) != 0) {
          foundArmyAsset = ArmyAssetRegistry_FindByIdCf
                             ((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId);
          workValue = *(int *)selectedModelRuntime->reserved100_117;
          *(ArmySelectionDetailTemplateVariantIndex *)(rootCursor->opaqueA06C_C3E3 + 0x54) =
               (foundArmyAsset.eax)->selectionDetailTemplateVariantIndex + 0x18003c;
          resolvedText = TextResource_Resolve(workValue * 2 + 0x300000);
          RichTextCommandStream_CopyExpandedCf
                    (0x80,g_InGameSelectionDetailTextSlot09Utf16,resolvedText.eax);
        }
        if (selectedModelRuntime->attachmentCount0C != 0) {
          attachedModelRuntime = selectedModelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
          if (attachedModelRuntime != (ModelRuntimeSlot *)0x0) {
            resolvedText = TextResource_Resolve
                               (((attachedModelRuntime->definitionOrSavedId).definition)->flags + 0x18004f);
            sourceText = resolvedText.eax;
            destinationText = g_InGameSelectionDetailWeaponName0TextUtf16;
            for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
              *destinationText = *sourceText;
              sourceText = sourceText + 1;
              destinationText = destinationText + 1;
            }
          }
          selectedModelRuntimeTail = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
          if (1 < selectedModelRuntimeTail->attachmentCount0C) {
            attachedModelRuntime = selectedModelRuntimeTail->attachments140[1].childModelRuntimeOrSavedOffset00;
            if (attachedModelRuntime != (ModelRuntimeSlot *)0x0) {
              resolvedText = TextResource_Resolve
                                 (((attachedModelRuntime->definitionOrSavedId).definition)->flags + 0x18004f);
              sourceText = resolvedText.eax;
              destinationText = g_InGameSelectionDetailWeaponName1TextUtf16;
              for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
                *destinationText = *sourceText;
                sourceText = sourceText + 1;
                destinationText = destinationText + 1;
              }
            }
            selectedModelRuntimeTail2 = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
            if ((2 < selectedModelRuntimeTail2->attachmentCount0C) &&
               (attachedModelRuntime = selectedModelRuntimeTail2->attachments140[2].
                         childModelRuntimeOrSavedOffset00, attachedModelRuntime != (ModelRuntimeSlot *)0x0)) {
              resolvedText = TextResource_Resolve
                                 (((attachedModelRuntime->definitionOrSavedId).definition)->flags + 0x18004f);
              sourceText = resolvedText.eax;
              destinationText = g_InGameSelectionDetailWeaponName2TextUtf16;
              for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
                *destinationText = *sourceText;
                sourceText = sourceText + 1;
                destinationText = destinationText + 1;
              }
            }
          }
        }
        recordCursor = (lastSelectedEntity->common).ownership.definitionOrClassRecord;
        workValue = *(int *)(*recordCursor + 0x4c);
        if (workValue == 0x16) {
          if (recordCursor[0x2b] != 1) {
            return;
          }
          foundArmyAsset = ArmyAssetRegistry_FindByIdCf(recordCursor[0x18]);
          linkedArmyAsset = foundArmyAsset.eax;
        }
        else if (workValue == 0xb) {
          if (recordCursor[0x2e] != 1) {
            return;
          }
          foundArmyAsset = ArmyAssetRegistry_FindByIdCf(recordCursor[0x18]);
          linkedArmyAsset = foundArmyAsset.eax;
        }
        else {
          if (workValue != 0xd) {
            if (workValue != 0xe) {
              return;
            }
            (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,recordCursor[0x18],
                       g_InGameSelectionDetailWeaponName0TextUtf16);
            return;
          }
          if (recordCursor[0x2e] != 1) {
            return;
          }
          foundArmyAsset = ArmyAssetRegistry_FindByIdCf(recordCursor[0x18]);
          linkedArmyAsset = foundArmyAsset.eax;
        }
        linkedDefinitionListView =
             (ModelLinkedDefinitionBranchView18 *)linkedArmyAsset->rootNodeOffsetOrPointer;
        if (linkedArmyAsset->selectionDetailTemplateVariantIndex < 8) {
          *(ArmySelectionDetailTemplateVariantIndex *)(rootCursor->opaqueA06C_C3E3 + 0x54) =
               *(int *)(rootCursor->opaqueA06C_C3E3 + 0x54) +
               linkedArmyAsset->selectionDetailTemplateVariantIndex;
        }
        unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                            (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
        resolvedText = TextResource_Resolve((unlockedDefinition.modelDefinition)->flags + 0x18004f);
        sourceText = resolvedText.eax;
        destinationText = g_InGameSelectionDetailTextSlot05Utf16;
        goto InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots;
      }
    }
    else if ((selectedCountOrCounter != 0) && (workValue == (lastSelectedEntity->common).ownership.ownerIndex)) {
      UiPageStack_SetActiveIndex(2,stack);
      selectedCountOrCounter = 0x20;
      recordCursor = g_InGameSelectionDetailGridCellOffsets;
      workValue = 0xc;
      entitySlot = g_SelectionInfoEntitySlots->entries;
      do {
        lastSelectedEntity = *entitySlot;
        if ((lastSelectedEntity != (GameEntityRuntime *)0x0) && (workValue != 0)) {
          slotCounterOrOffset = *recordCursor;
          *(GameEntityRuntime **)(rootCursor->opaque0058_017B + slotCounterOrOffset + 4) = lastSelectedEntity;
          foundArmyAsset = ArmyAssetRegistry_FindByIdCf
                             ((lastSelectedEntity->common).runtimeIdentityOrArmyAssetId);
          *(PckArmyAssetIdCatalog *)(rootCursor->opaque0058_017B + slotCounterOrOffset + -4) =
               foundArmyAsset.eax[1].registryId;
          rootCursor = (InGameRuntimeRootImageC3E4 *)((int)rootCursor + (slotCounterOrOffset - *recordCursor));
          workValue = workValue + -1;
          recordCursor = recordCursor + 1;
        }
        entitySlot = entitySlot + 1;
        selectedCountOrCounter = selectedCountOrCounter + -1;
      } while (selectedCountOrCounter != 0);
      for (; workValue != 0; workValue = workValue + -1) {
        clearedControlBytes = rootCursor->opaque0058_017B + *recordCursor + -4;
        clearedControlBytes[0] = 0;
        clearedControlBytes[1] = 0;
        clearedControlBytes[2] = 0;
        clearedControlBytes[3] = 0;
        recordCursor = recordCursor + 1;
      }
      g_InGameSelectionDetailTextSlot05Utf16[0] = (word)THANDOR_PART(dword, g_InGameSelectionDetailTextSlot05Utf16, 0)
      ;
      g_InGameSelectionDetailTextSlot05Utf16[1] =
           SUB42(THANDOR_PART(dword, g_InGameSelectionDetailTextSlot05Utf16, 0),2);
      g_InGameSelectionDetailTextSlot09Utf16[0] = (word)THANDOR_PART(dword, g_InGameSelectionDetailTextSlot09Utf16, 0)
      ;
      g_InGameSelectionDetailTextSlot09Utf16[1] =
           SUB42(THANDOR_PART(dword, g_InGameSelectionDetailTextSlot09Utf16, 0),2);
      return;
    }
    UiPageStack_SetActiveIndex(0,stack);
  }
  else {
    UiPageStack_SetActiveIndex(3,stack);
    hoverTextureSource = definitionNode->textureSource;
    detailValue = ArmyAssetHierarchy_SumFactionUnlockedArmour
                      ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    *(GraphicsTextureSourceAsset **)(rootCursor->opaqueA06C_C3E3 + 0x128) = hoverTextureSource;
    metricValue = definitionNode->buildXeniteCostQ4;
    buildDuration = definitionNode->buildDurationQ5;
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,detailValue,g_InGameSelectionDetailArmourTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,metricValue >> 4,
               g_InGameSelectionDetailBuildXeniteCostTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,buildDuration >> 5,
               g_InGameSelectionDetailBuildTimeTextUtf16);
    displayedEnergy = ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
                      ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,displayedEnergy >> 4,g_InGameSelectionDetailEnergyTextUtf16
              );
    workValue = *(int *)(definitionNode->reserved00_07 + 4) + 0x180045;
    *(int *)(rootCursor->opaqueA06C_C3E3 + 0x184) = workValue;
    *(int *)(rootCursor->opaqueA06C_C3E3 + 0x1070) = workValue;
    linkedDefinitionListView = *(ModelLinkedDefinitionBranchView18 **)definitionNode->reserved0C_1B;
    unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                        (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
    resolvedText = TextResource_Resolve((unlockedDefinition.modelDefinition)->flags + 0x18004f);
    sourceText = resolvedText.eax;
    destinationText = g_InGameSelectionDetailNameTextUtf16;
InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots:
    for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
      *destinationText = *sourceText;
      sourceText = sourceText + 1;
      destinationText = destinationText + 1;
    }
    resolvedText = TextResource_Resolve(0x18004e);
    sourceText = resolvedText.eax;
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName0TextUtf16,sourceText);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName1TextUtf16,sourceText);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName2TextUtf16,sourceText);
    if (linkedDefinitionListView->childListCount != 0) {
      unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                         ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                          linkedDefinitionListView->childList0Address);
      resolvedText = TextResource_Resolve((unlockedDefinition.modelDefinition)->flags + 0x18004f);
      sourceText = resolvedText.eax;
      destinationText = g_InGameSelectionDetailWeaponName0TextUtf16;
      for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
        *destinationText = *sourceText;
        sourceText = sourceText + 1;
        destinationText = destinationText + 1;
      }
      if (1 < linkedDefinitionListView->childListCount) {
        unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                            linkedDefinitionListView->childList1Address);
        resolvedText = TextResource_Resolve((unlockedDefinition.modelDefinition)->flags + 0x18004f);
        sourceText = resolvedText.eax;
        destinationText = g_InGameSelectionDetailWeaponName1TextUtf16;
        for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
          *destinationText = *sourceText;
          sourceText = sourceText + 1;
          destinationText = destinationText + 1;
        }
        if (2 < linkedDefinitionListView->childListCount) {
          unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                             ((rootCursor->worldRuntime0A30).activeFactionRuntimeIndex,
                              linkedDefinitionListView->childList2Address);
          resolvedText = TextResource_Resolve((unlockedDefinition.modelDefinition)->flags + 0x18004f);
          sourceText = resolvedText.eax;
          destinationText = g_InGameSelectionDetailWeaponName2TextUtf16;
          for (workValue = 0x40; workValue != 0; workValue = workValue + -1) {
            *destinationText = *sourceText;
            sourceText = sourceText + 1;
            destinationText = destinationText + 1;
          }
        }
      }
    }
  }
  return;
}

