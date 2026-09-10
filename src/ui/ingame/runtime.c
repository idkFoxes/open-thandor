#include <thandor/ui/ingame/runtime.h>

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
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf
               (dword keyboardStateMask,dword keyboardEventCode,UiRootNode *uiRoot)

{
  sdword *psVar1;
  wchar_t wVar2;
  wchar_t wVar3;
  int extraout_EAX;
  UiNodeVtable *pUVar4;
  dword dVar5;
  GraphicsCapturedTextureSourceAsset *source;
  UiCommandModeIndex UVar6;
  sdword in_EDX;
  UiPageStackControl *stack;
  UiPageStackControl *stack_00;
  UiPageStackControl *stack_01;
  int iVar7;
  dword unaff_EBP;
  dword unaff_ESI;
  sdword unaff_EDI;
  dword *pdVar8;
  dword *pdVar9;
  bool bVar10;
  
  pdVar9 = (dword *)0x56e410;
  do {
    while( true ) {
      do {
        pdVar8 = pdVar9;
        if (*pdVar8 == 0) {
          return;
        }
        pdVar9 = pdVar8 + 3;
      } while (*pdVar8 != keyboardEventCode);
      if (pdVar8[1] == 0) break;
      if ((keyboardStateMask & pdVar8[1]) != 0) goto override_jmp_0056e406_switch;
    }
  } while ((keyboardStateMask & 0x3c) != 0);
override_jmp_0056e406_switch:
  bVar10 = false;
                    
  switch(pdVar8[2]) {
  case 0x56e5e0:
    UiPageStack_ActivePageNotInListCf((UiPageStackControl *)&uiRoot[0xbc].base.vtable);
    if (extraout_EAX == 0) {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&uiRoot[0xc9].base.right);
      UiPageStack_SetActiveIndex(2,stack_01);
      uiRoot[0x1d].base.vtable = (UiNodeVtable *)0x0;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    else {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&uiRoot[0xc9].base.right);
      UiPageStack_SetActiveIndex(1,stack_00);
      uiRoot[0x1d].base.vtable = (UiNodeVtable *)uiRoot[0xbd].base.bottomOffset;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    break;
  case 0x56e670:
    psVar1 = &uiRoot[0x68].base.right;
    *psVar1 = *psVar1 + 1;
    if (0x117 < (uint)uiRoot[0x68].base.right) {
      uiRoot[0x68].base.right = 0x112;
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
        UVar6 = g_UiCommandAbsoluteSelectionIndex - 1;
        if ((int)UVar6 < 0) {
          UVar6 = 0x19;
        }
        while (g_TerrainMaterialTextureSets[UVar6] == (GraphicsTextureSet *)0x0) {
          UVar6 = UVar6 - 1;
          if ((int)UVar6 < 0) {
            UVar6 = 0x19;
          }
        }
        UiCommandMatrix_SelectIndex(UVar6,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        g_UiCommandModeGArmyAssetId =
             ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf(g_UiCommandModeGArmyAssetId)
        ;
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        g_UiHoverSelectionRecord =
             (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        g_UiCommandMode4ArmyAssetId =
             ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf(g_UiCommandMode4ArmyAssetId);
        dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        uiRoot[0x1cb].base.top = dVar5;
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
        UVar6 = g_UiCommandAbsoluteSelectionIndex + 1;
        if (0x19 < UVar6) {
          UVar6 = 0;
        }
        while (g_TerrainMaterialTextureSets[UVar6] == (GraphicsTextureSet *)0x0) {
          UVar6 = UVar6 + 1;
          if (0x19 < UVar6) {
            UVar6 = 0;
          }
        }
        UiCommandMatrix_SelectIndex(UVar6,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        g_UiCommandModeGArmyAssetId =
             ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(g_UiCommandModeGArmyAssetId);
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        g_UiHoverSelectionRecord =
             (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        g_UiCommandMode4ArmyAssetId =
             ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(g_UiCommandMode4ArmyAssetId);
        dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        uiRoot[0x1cb].base.top = dVar5;
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
        UVar6 = g_UiCommandAbsoluteSelectionIndex - 1;
        iVar7 = 3;
        if ((int)UVar6 < 0) {
          UVar6 = 0x19;
        }
        while ((g_TerrainMaterialTextureSets[UVar6] == (GraphicsTextureSet *)0x0 ||
               (iVar7 = iVar7 + -1, iVar7 != 0))) {
          UVar6 = UVar6 - 1;
          if ((int)UVar6 < 0) {
            UVar6 = 0x19;
          }
        }
        UiCommandMatrix_SelectIndex(UVar6,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        g_UiCommandModeGArmyAssetId =
             ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        g_UiHoverSelectionRecord =
             (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        g_UiCommandMode4ArmyAssetId =
             ArmyAssetRegistry_StepForwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        uiRoot[0x1cb].base.top = dVar5;
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
        UVar6 = g_UiCommandAbsoluteSelectionIndex + 1;
        iVar7 = 3;
        if (0x19 < UVar6) {
          UVar6 = 0;
        }
        while ((g_TerrainMaterialTextureSets[UVar6] == (GraphicsTextureSet *)0x0 ||
               (iVar7 = iVar7 + -1, iVar7 != 0))) {
          UVar6 = UVar6 + 1;
          if (0x19 < UVar6) {
            UVar6 = 0;
          }
        }
        UiCommandMatrix_SelectIndex(UVar6,&uiRoot->base);
      }
      else if (g_UiCommandModeG == 3) {
        g_UiCommandModeGArmyAssetId =
             ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        g_UiHoverSelectionRecord =
             (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        g_UiCommandMode4ArmyAssetId =
             ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
        uiRoot[0x1cb].base.top = dVar5;
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
      if ((uint)g_GameFactionRuntimeImage.tail.reserved00_03 <
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
        g_UiCommandModeGOwnerFactionIndex =
             (FactionRuntimeIndex)g_GameFactionRuntimeImage.tail.reserved00_03;
      }
      ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected((dword)uiRoot);
    }
    break;
  case 0x56ee00:
    InGameCommandModeG_Select0((UiSelectableControl *)&uiRoot[0xdc].base.firstChild);
    InGameCommandModeC_Select0((UiSpriteButtonControl *)&uiRoot[0x211].base.rightAnchorQ31);
    break;
  case 0x56ee20:
    InGameCommandModeG_Select0((UiSelectableControl *)&uiRoot[0xdc].base.firstChild);
    InGameCommandModeC_Select1((UiSpriteButtonControl *)(uiRoot + 0x213));
    break;
  case 0x56ee40:
    InGameCommandModeG_Select0((UiSelectableControl *)&uiRoot[0xdc].base.firstChild);
    InGameCommandModeC_Select2((UiSpriteButtonControl *)&uiRoot[0x214].base.leftOffset);
    break;
  case 0x56ee60:
    if (g_UiCommandModeG == 1) {
      InGameCommandModeG_Select1((UiSelectableControl *)&uiRoot[0xdd].base.topOffset);
      InGameCommandModeD_Select3((UiSpriteButtonControl *)&uiRoot[0x21b].base.left);
    }
    else {
      InGameCommandModeG_Select0((UiSelectableControl *)&uiRoot[0xdc].base.firstChild);
      InGameCommandModeC_Select3((UiSpriteButtonControl *)&uiRoot[0x215].base.layoutWidth);
    }
    break;
  case 0x56eeb0:
    InGameCommandModeG_Select1((UiSelectableControl *)&uiRoot[0xdd].base.topOffset);
    InGameCommandModeD_Select0((UiSpriteButtonControl *)&uiRoot[0x217].base.parent);
    break;
  case 0x56eed0:
    InGameCommandModeG_Select1((UiSelectableControl *)&uiRoot[0xdd].base.topOffset);
    InGameCommandModeD_Select1((UiSpriteButtonControl *)&uiRoot[0x218].base.rightOffset);
    break;
  case 0x56eef0:
    InGameCommandModeG_Select1((UiSelectableControl *)&uiRoot[0xdd].base.topOffset);
    InGameCommandModeD_Select2((UiSpriteButtonControl *)&uiRoot[0x219].base.nodeFlags);
    break;
  case 0x56ef10:
    InGameCommandModeG_Select2((UiSelectableControl *)&uiRoot[0xde].base.layoutHeight);
    InGameCommandModeE_Select0((UiSpriteButtonControl *)&uiRoot[0x21c].base.leftAnchorQ31);
    break;
  case 0x56ef30:
    InGameCommandModeG_Select2((UiSelectableControl *)&uiRoot[0xde].base.layoutHeight);
    InGameCommandModeE_Select1((UiSpriteButtonControl *)&uiRoot[0x21d].callbacks);
    break;
  case 0x56ef50:
    InGameCommandModeG_Select2((UiSelectableControl *)&uiRoot[0xde].base.layoutHeight);
    InGameCommandModeE_Select2((UiSpriteButtonControl *)&uiRoot[0x21f].base.right);
    break;
  case 0x56ef70:
    InGameCommandModeG_Select2((UiSelectableControl *)&uiRoot[0xde].base.layoutHeight);
    InGameCommandRange_DispatchState0((UiNodeBase *)&uiRoot[0x220].base.rightAnchorQ31);
    break;
  case 0x56ef90:
    InGameCommandModeG_Select2((UiSelectableControl *)&uiRoot[0xde].base.layoutHeight);
    InGameCommandRange_DispatchState1(&uiRoot[0x222].base);
    break;
  case 0x56efb0:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select0((UiSpriteButtonControl *)&uiRoot[0x227].base.rightOffset);
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)&uiRoot[0x1bd].base.rightAnchorQ31);
      InGameCommandModeA_Select0((UiSpriteButtonControl *)&uiRoot[0x223].base.leftOffset);
      ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f020:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select1((UiSpriteButtonControl *)&uiRoot[0x22a].base.left);
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)&uiRoot[0x1bd].base.rightAnchorQ31);
      InGameCommandModeA_Select1((UiSpriteButtonControl *)&uiRoot[0x226].base.parent);
      ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f090:
    if (g_UiCommandModeG == 4) {
      InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
      InGameCommandModeB_Select2((UiSpriteButtonControl *)&uiRoot[0x228].base.nodeFlags);
    }
    else {
      InGameCommandModeG_Select3((UiSelectableControl *)&uiRoot[0x1bd].base.rightAnchorQ31);
      InGameCommandModeA_Select2((UiSpriteButtonControl *)&uiRoot[0x224].base.layoutWidth);
      ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
      InGameSelectionDetailPanel_Rebuild();
    }
    break;
  case 0x56f100:
    InGameCommandModeG_Select3((UiSelectableControl *)&uiRoot[0x1bd].base.rightAnchorQ31);
    break;
  case 0x56f120:
    InGameCommandModeG_Select4((UiSelectableControl *)(uiRoot + 0x1bf));
    break;
  case 0x56f140:
    InGameCommandModeG_Select5((UiSelectableControl *)&uiRoot[0x1bc].base.right);
    break;
  case 0x56f160:
    source = (*g_GraphicsFramebufferCaptureRegion)(unaff_EBP,unaff_ESI,unaff_EDI,in_EDX);
    if (!bVar10) {
      FileSystem_WriteBufferToPathCf
                ((source->common).allocationSizeBytes,source,(word *)(u_Dscreen00_pcx_00572e3a + 1))
      ;
      wVar3 = u_Dscreen00_pcx_00572e3a[8];
      wVar2 = u_Dscreen00_pcx_00572e3a[7];
      u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\x01';
      if (0x39 < (ushort)u_Dscreen00_pcx_00572e3a[8]) {
        u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + L'\x01';
        u_Dscreen00_pcx_00572e3a[8] = wVar3 + L'\xfff7';
        if (0x39 < (ushort)u_Dscreen00_pcx_00572e3a[7]) {
          u_Dscreen00_pcx_00572e3a[7] = wVar2 + L'\xfff7';
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
void InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  int iVar1;
  uint uVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_EDX;
  uint packedState;
  uint extraout_EDX_00;
  uint uVar3;
  int *piVar4;
  word *pwVar5;
  bool bVar6;
  
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)commandTextEdit);
  if ((commandTextEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      iVar1 = 0x10;
      bVar6 = true;
      piVar4 = (int *)0x5631de;
      pwVar5 = commandTextEdit->textBuffer;
      do {
        if (iVar1 == 0) break;
        iVar1 = iVar1 + -1;
        bVar6 = *piVar4 == *(int *)pwVar5;
        piVar4 = piVar4 + 1;
        pwVar5 = pwVar5 + 2;
      } while (bVar6);
      if (bVar6) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 0x40000;
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x80000;
        InGameRecentTextHistory_InsertAndRebuild8();
      }
    }
    else {
      RichTextCommandStream_CopyToNarrowCf
                (0x30,g_UiSevenSlotCommandPayloadText.textBytes,commandTextEdit->textBuffer);
      iVar1 = UiSelectableGroup_NoneVisibleSelectedCf(3);
      if (iVar1 - (int)commandTextEdit == 0x1dd4) {
        uVar2 = 0;
        uVar3 = 0x100;
        do {
          bVar6 = CARRY4(uVar3,uVar3);
          uVar3 = uVar3 * 2;
          UiSelectableControl_IsSelectedCf
                    ((UiSelectableControl *)
                     (g_UiSevenSlotSelectionControlOffsets[uVar2] + -0xb0 + (int)commandTextEdit));
          packedState = extraout_EDX;
          if (bVar6) {
            packedState = extraout_EDX | uVar3;
          }
          uVar2 = extraout_ECX;
        } while (extraout_ECX < 7);
      }
      else if (iVar1 - (int)commandTextEdit == 0x1e34) {
        uVar2 = 0;
        uVar3 = 0x8000;
        do {
          bVar6 = CARRY4(uVar3,uVar3);
          uVar3 = uVar3 * 2;
          UiSelectableControl_IsSelectedCf
                    ((UiSelectableControl *)
                     (g_UiSevenSlotSelectionControlOffsets[uVar2] + -0xb0 + (int)commandTextEdit));
          packedState = extraout_EDX_00;
          if (bVar6) {
            packedState = extraout_EDX_00 | uVar3;
          }
          uVar2 = extraout_ECX_00;
        } while (extraout_ECX_00 < 7);
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
      pwVar5 = commandTextEdit->textBuffer;
      for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
        pwVar5[0] = 0;
        pwVar5[1] = 0;
        pwVar5 = pwVar5 + 2;
      }
    }
  }
  UiPageStack_SetActiveIndex(0,&ADJ(commandTextEdit).commandPageStack);
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
void InGameUiAction1210_ResourceRegistrationHelper(void *runtimeBase,void *resourcePath)

{
  word *arg0;
  void *handle;
  void *sourceData;
  void *sourceData_00;
  dword *sourceData_01;
  dword dVar1;
  int extraout_ECX;
  FrontendPlayerRuntimeBlockCount FVar2;
  int iVar3;
  PckDecodedByteCount unpackedSize;
  void *destination;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  void *source;
  FrontendPlayerRuntimeRecord *pFVar4;
  dword *pdVar5;
  dword *pdVar6;
  undefined1 uVar7;
  byte bVar8;
  bool bVar9;
  RuntimeImagePointerByteSizeEdxEax8 RVar10;
  ResourceRegistrationImagePair RVar11;
  uint uVar12;
  
  g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + '\x01';
  uVar7 = false;
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar4 = g_FrontendPlayerRuntimeBlocks;
  while (FVar2 != 0) {
    GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
              (pFVar4->playerRuntimeId,0,0,(pFVar4->factionAssignment).factionAssignmentIndex);
    uVar7 = (FrontendPlayerRuntimeRecord *)0xffffec4f < pFVar4;
    pFVar4 = pFVar4 + 1;
    FVar2 = extraout_ECX - 1;
  }
  handle = (void *)ResourceRegistration_OpenSourceCf(resourcePath);
  if ((bool)uVar7) {
    WidePath_SplitParentAndLeaf((word *)g_PackageScratchBuffer,(word *)0x50dcc4,resourcePath);
    (*(code *)g_FileSystemCreateDirectoryRecursiveCf)(1,0x50dcc4);
    if (((bool)uVar7) ||
       (handle = (void *)ResourceRegistration_OpenSourceCf(resourcePath), (bool)uVar7))
    goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  bVar8 = 0;
  RVar10 = ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(RVar10 >> 0x20),
                      (void *)RVar10,(word *)u_army_hex_0050dfb4,(EngineFileHandle)handle);
  uVar12 = (uint)(bVar8 & 1);
  ArmyRuntimePool_RebaseAfterLoad();
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ModelRuntimePool_UnrebaseBeforeSave();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,unpackedSize,sourceData,
                      (word *)u_modul_hex_0050dfee,(EngineFileHandle)handle);
  uVar12 = (uint)(bVar8 & 1);
  ModelRuntimePool_RebaseAfterLoad();
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar11 = ResourceRegistration_QueryDomain2Pair();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar11,
                      (void *)(RVar11 >> 0x20),(word *)u_shot_hex_0050dfdc,(EngineFileHandle)handle)
  ;
  uVar12 = (uint)(bVar8 & 1);
  ShotRuntime_RebaseSlotsAfterLoad();
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar11 = ResourceRegistration_QueryDomain1Pair();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar11,
                      (void *)(RVar11 >> 0x20),(word *)u_effect_hex_0050dfc6,
                      (EngineFileHandle)handle);
  uVar12 = (uint)(bVar8 & 1);
  EffectRuntime_RebaseSlotsAfterLoad();
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar11 = ResourceRegistration_SelectDomainPair(runtimeBase);
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar11,
                      (void *)(RVar11 >> 0x20),(word *)u_widget_hex_0050e02a,
                      (EngineFileHandle)handle);
  uVar12 = (uint)(bVar8 & 1);
  InGameConditionRuntime_RebaseLoadedRecords(runtimeBase);
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar10 = RuntimeHexSegment_GetLightImageAndToggleFlagRegs();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(RVar10 >> 0x20),
                      (void *)RVar10,(word *)u_light_hex_0050e016,(EngineFileHandle)handle);
  uVar12 = (uint)(bVar8 & 1);
  RuntimeHexSegment_ToggleLightImageFlag();
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar11 = RuntimeHexSegment_GetFieldImageRegs(runtimeBase);
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar11,
                      (void *)(RVar11 >> 0x20),(word *)u_field_hex_0050e002,(EngineFileHandle)handle
                     );
  uVar12 = (uint)(bVar8 & 1);
  RuntimeHexSegment_AfterFieldImageNoOp(runtimeBase);
  bVar8 = (uVar12 & 1) != 0;
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ResourceRegistration_ResolveRuntimeRecord(runtimeBase);
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                      *(PckDecodedByteCount *)((int)sourceData_00 + 0xdc),sourceData_00,
                      (word *)u_level_hex_0050e040,(EngineFileHandle)handle);
  if ((bool)bVar8) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar11 = ResourceRegistration_QueryDomain0Pair();
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar11,
                      (void *)(RVar11 >> 0x20),(word *)u_daten_hex_0050e054,(EngineFileHandle)handle
                     );
  uVar12 = (uint)(bVar8 & 1);
  GameFactionRuntime_RebaseLoadedArmyReferences();
  if ((uVar12 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  bVar9 = false;
  if (g_FrontendLoadedCampaignAsset == (void *)0x0) {
    Package_DeleteEntry((word *)u_campagne_hex_0050e068,(EngineFileHandle)handle);
  }
  else {
    Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                        *(PckDecodedByteCount *)((int)g_FrontendLoadedCampaignAsset + 4),
                        g_FrontendLoadedCampaignAsset,(word *)u_campagne_hex_0050e068,
                        (EngineFileHandle)handle);
    if (bVar9) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,0x38000,g_GameStatTableImage,
                      (word *)u_stat_hex_0050e082,(EngineFileHandle)handle);
  uVar7 = false;
  if (g_OldUnitRecordCount == 0) {
    iVar3 = 0x40;
    uVar7 = false;
    bVar9 = true;
    pdVar5 = g_OldUnitSecondaryTable;
    do {
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      uVar7 = *pdVar5 != 0;
      bVar9 = *pdVar5 == 0;
      pdVar5 = pdVar5 + 1;
    } while (bVar9);
    if (!bVar9) goto InGameResourceRegistration_SerializeOldUnitTables;
    Package_DeleteEntry((word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
  }
  else {
InGameResourceRegistration_SerializeOldUnitTables:
    sourceData_01 = (*g_MemoryApi.alloc)(0x4104);
    pdVar5 = g_OldUnitPrimaryTable;
    if ((bool)uVar7) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
    *sourceData_01 = g_OldUnitRecordCount;
    pdVar6 = sourceData_01;
    for (iVar3 = 0x1000; pdVar6 = pdVar6 + 1, iVar3 != 0; iVar3 = iVar3 + -1) {
      *pdVar6 = *pdVar5;
      pdVar5 = pdVar5 + 1;
    }
    pdVar5 = g_OldUnitSecondaryTable;
    for (iVar3 = 0x40; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pdVar6 = *pdVar5;
      pdVar5 = pdVar5 + 1;
      pdVar6 = pdVar6 + 1;
    }
    uVar7 = pdVar6 < sourceData_01;
    Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(int)pdVar6 - (int)sourceData_01,sourceData_01,
                        (word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
    (*g_MemoryApi.free)(sourceData_01);
  }
  (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
  if ((!(bool)uVar7) && ((*g_FileSystemReadExactCf)(0x200,destination,handle), !(bool)uVar7)) {
    WidePath_SplitParentAndLeaf
              ((word *)(extraout_EDX + 0x100),(word *)(extraout_EDX + 0x200),resourcePath);
    dVar1 = (*g_LocaleGetPackedCurrentDate)();
    *(dword *)(extraout_EDX_00 + 0x1f0) = dVar1;
    dVar1 = (*g_LocaleGetPackedCurrentTime)();
    *(dword *)(extraout_EDX_01 + 500) = dVar1;
    dVar1 = (*g_LocaleFormatCurrentDateUtf16)((word *)(extraout_EDX_01 + 0x1c0));
    arg0 = (word *)(dVar1 + 4 + extraout_EDX_01 + 0x1c0);
    arg0[-0xffffffff00000002] = 0x2c;
    arg0[-0xffffffff00000001] = 0x20;
    (*g_LocaleFormatCurrentTimeUtf16)(arg0);
    dVar1 = g_InGameLevelCampaignAssociationIndex;
    if (g_FrontendLoadedCampaignAsset == (void *)0x0) {
      dVar1 = 0xffffffff;
    }
    bVar9 = false;
    *(undefined4 *)(extraout_EDX_02 + 0x170) = g_InGameLevelTitleTextResourceIndex;
    *(dword *)(extraout_EDX_02 + 400) = dVar1;
    (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
    if ((!bVar9) && ((*g_FileSystemWriteExactOrFlushCf)(0x200,source,handle), !bVar9)) {
      Package_Unmount((EngineFileHandle)handle);
      g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + -1;
      return;
    }
  }
InGameResourceRegistration_DecrementBusyCountAndReturn:
  g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + -1;
  return;
}

/* Address: 0x0053D9F0.
   Ownership: ui/ingame/runtime.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[0]@005624A0. Queued UI action handler
   for INGAME_PAGE10[0] (0x1000). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   mapControl→InGameMapViewControlAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
undefined8 InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl)

{
  longlong lVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_EDX;
  int iVar3;
  
  iVar3 = *(int *)(mapControl + 0x6c);
  *(int *)(mapControl + 0x50) = *(int *)(mapControl + 0x68);
  *(int *)(mapControl + 0x54) = iVar3;
  lVar1 = (longlong)(iVar3 + *(int *)(mapControl + 0x68) * 2) * 0x901;
  iVar2 = ((int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd) -
          *(int *)(mapControl + -0x8f6c);
  iVar3 = ((int)((ulonglong)((longlong)iVar3 * -1999) >> 0x20) << 0x14 |
          (uint)((longlong)iVar3 * -1999) >> 0xc) - *(int *)(mapControl + -0x8f68);
  *(int *)(mapControl + -0x8f6c) = *(int *)(mapControl + -0x8f6c) + iVar2;
  *(int *)(mapControl + -0x8f68) = *(int *)(mapControl + -0x8f68) + iVar3;
  *(int *)(mapControl + -0x8f8c) = *(int *)(mapControl + -0x8f8c) + iVar2;
  *(int *)(mapControl + -0x8f88) = *(int *)(mapControl + -0x8f88) + iVar3;
  WorldRuntime_ClearFieldGridDirtyFlag((WorldRuntimeContext *)(mapControl + -0x8fec));
  return CONCAT44(in_EDX,in_EAX);
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
void InGameUiRuntime_InitializeControlTreeResourcesCf(UiRootNode *inGameRoot)

{
  sdword *psVar1;
  UiAnchorFractionQ31 *pUVar2;
  UiNodeFlags *pUVar3;
  UiNodeBase **ppUVar4;
  UiNodeVtable **ppUVar5;
  UiRootFlags UVar6;
  UiNodeFlags UVar7;
  UiRootCallbacks *pUVar8;
  sdword sVar9;
  UiAnchorFractionQ31 UVar10;
  UiAnchorFractionQ31 UVar11;
  int iVar12;
  GraphicsTextureSourceAsset *pGVar13;
  sdword sVar14;
  undefined *puVar15;
  GraphicsTextureSourceAsset *pGVar16;
  UiRootNode *pUVar17;
  UiNodeVtable *pUVar18;
  word *stream;
  int iVar19;
  dword in_ECX;
  uint uVar20;
  uint extraout_ECX;
  dword arg0;
  dword arg0_00;
  dword arg0_01;
  dword in_EDX;
  UiRootNode *pUVar21;
  UiNodeBase *pUVar22;
  int iVar23;
  UiNodeBase *pUVar24;
  dword arg1;
  UiRootNode *arg1_00;
  UiNodeBase *arg1_01;
  GraphicsTextureSourceAsset *arg1_02;
  uint uVar25;
  bool bVar26;
  undefined1 uVar27;
  qword qVar28;
  undefined8 uVar29;
  
  if ((g_FramebufferWidth < 800) || (g_FramebufferHeight < 600)) {
    bVar26 = true;
    u_gfx_panel_panel0_gfx_005630d0[0xf] = L'0';
    u_gfx_panel_diagram0_gfx_00563120[0x11] = L'0';
    inGameRoot[0xd5].rootFlags = 0x24;
    inGameRoot[0xd5].callbacks = (UiRootCallbacks *)0x6;
    inGameRoot[0xd5].previousRoot = (UiRootNode *)0x5e;
    inGameRoot[0xd6].base.nextSibling = (UiNodeBase *)0xd;
    inGameRoot[0xd7].base.topAnchorQ31 = 0x24;
    inGameRoot[0xd7].base.rightAnchorQ31 = 0x11;
    inGameRoot[0xd7].base.bottomAnchorQ31 = 0x5e;
    inGameRoot[0xd7].base.layoutWidth = 0x18;
    inGameRoot[0xd9].base.bottom = 0x24;
    inGameRoot[0xd9].base.leftOffset = 0x1c;
    inGameRoot[0xd9].base.topOffset = 0x5e;
    inGameRoot[0xd9].base.rightOffset = 0x23;
    inGameRoot[0xdb].base.bottom = 4;
    inGameRoot[0xdb].base.leftOffset = 5;
    inGameRoot[0xdb].base.topOffset = 0x1f;
    inGameRoot[0xdb].base.rightOffset = 0xd;
  }
  else {
    bVar26 = true;
    if ((g_FramebufferWidth < 0x400) || (bVar26 = g_FramebufferHeight < 0x300, bVar26)) {
      u_gfx_panel_panel0_gfx_005630d0[0xf] = L'1';
      u_gfx_panel_diagram0_gfx_00563120[0x11] = L'1';
      inGameRoot[0xd5].rootFlags = 0x2c;
      inGameRoot[0xd5].callbacks = (UiRootCallbacks *)0x9;
      inGameRoot[0xd5].previousRoot = (UiRootNode *)0x6e;
      inGameRoot[0xd6].base.nextSibling = (UiNodeBase *)0x10;
      inGameRoot[0xd7].base.topAnchorQ31 = 0x2c;
      inGameRoot[0xd7].base.rightAnchorQ31 = 0x17;
      inGameRoot[0xd7].base.bottomAnchorQ31 = 0x6e;
      inGameRoot[0xd7].base.layoutWidth = 0x1e;
      inGameRoot[0xd9].base.bottom = 0x2c;
      inGameRoot[0xd9].base.leftOffset = 0x25;
      inGameRoot[0xd9].base.topOffset = 0x6e;
      inGameRoot[0xd9].base.rightOffset = 0x2c;
      inGameRoot[0xdb].base.bottom = 4;
      inGameRoot[0xdb].base.leftOffset = 7;
      inGameRoot[0xdb].base.topOffset = 0x27;
      inGameRoot[0xdb].base.rightOffset = 0xf;
    }
    else {
      u_gfx_panel_panel0_gfx_005630d0[0xf] = L'2';
      u_gfx_panel_diagram0_gfx_00563120[0x11] = L'2';
      inGameRoot[0xd5].rootFlags = 0x2c;
      inGameRoot[0xd5].callbacks = (UiRootCallbacks *)0x9;
      inGameRoot[0xd5].previousRoot = (UiRootNode *)0x6e;
      inGameRoot[0xd6].base.nextSibling = (UiNodeBase *)0x10;
      inGameRoot[0xd7].base.topAnchorQ31 = 0x2c;
      inGameRoot[0xd7].base.rightAnchorQ31 = 0x17;
      inGameRoot[0xd7].base.bottomAnchorQ31 = 0x6e;
      inGameRoot[0xd7].base.layoutWidth = 0x1e;
      inGameRoot[0xd9].base.bottom = 0x2c;
      inGameRoot[0xd9].base.leftOffset = 0x25;
      inGameRoot[0xd9].base.topOffset = 0x6e;
      inGameRoot[0xd9].base.rightOffset = 0x2c;
      inGameRoot[0xdb].base.bottom = 4;
      inGameRoot[0xdb].base.leftOffset = 7;
      inGameRoot[0xdb].base.topOffset = 0x27;
      inGameRoot[0xdb].base.rightOffset = 0xf;
    }
  }
  pGVar16 = (*g_GraphicsTextureSourceLoadPackageAsset)
                      (in_ECX,in_EDX,(word *)u_gfx_panel_panel0_gfx_005630d0);
  pGVar13 = g_InGamePanelTextureSource;
  if (!bVar26) {
    LOCK();
    UNLOCK();
    g_InGamePanelTextureSource = pGVar16;
    (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0,pGVar16);
    g_InGamePanelTextureSubresource00Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(1,pGVar16);
    g_InGamePanelTextureSubresource01Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(2,pGVar16);
    g_InGamePanelTextureSubresource02Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(6,pGVar16);
    g_InGamePanelTextureSubresource06Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(7,pGVar16);
    g_InGamePanelTextureSubresource07Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1b,pGVar16);
    g_InGamePanelTextureSubresource27Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1c,pGVar16);
    g_InGamePanelTextureSubresource28Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,pGVar16);
    g_InGamePanelTextureSubresource19Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x14,pGVar16);
    g_InGamePanelTextureSubresource20Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,pGVar16);
    g_InGamePanelTextureSubresource34Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,pGVar16);
    g_InGamePanelTextureSubresource32Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x21,pGVar16);
    g_InGamePanelTextureSubresource33Width = (sdword)qVar28;
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(2,pGVar16);
    g_InGamePanelTextureSubresource02Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(3,pGVar16);
    g_InGamePanelTextureSubresource03Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(4,pGVar16);
    g_InGamePanelTextureSubresource04Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(5,pGVar16);
    g_InGamePanelTextureSubresource05Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x24,pGVar16);
    g_InGamePanelTextureSubresource36Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x25,pGVar16);
    g_InGamePanelTextureSubresource37Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(6,pGVar16);
    g_InGamePanelTextureSubresource06Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0,pGVar16);
    g_InGamePanelTextureSubresource00Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(7,pGVar16);
    g_InGamePanelTextureSubresource07Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1a,pGVar16);
    g_InGamePanelTextureSubresource26Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1f,pGVar16);
    g_InGamePanelTextureSubresource31Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x12,pGVar16);
    g_InGamePanelTextureSubresource18Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x17,pGVar16);
    g_InGamePanelTextureSubresource23Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,pGVar16);
    g_InGamePanelTextureSubresource34Height = (sdword)(qVar28 >> 0x20);
    qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,pGVar16);
    pGVar13 = g_InGamePanelTextureSource;
    g_InGamePanelTextureSubresource32Height = (sdword)(qVar28 >> 0x20);
    inGameRoot[0xbd].base.bottomOffset = 0;
    inGameRoot[0xbd].base.leftAnchorQ31 = 0;
    inGameRoot[0xbd].base.topAnchorQ31 = 0;
    inGameRoot[0xbd].base.rightAnchorQ31 = 0;
    inGameRoot[0xbe].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0xbe].base.leftAnchorQ31 = 0;
    inGameRoot[0xbe].base.topAnchorQ31 = 0;
    inGameRoot[0xbe].base.rightAnchorQ31 = 0;
    inGameRoot[0xbe].base.bottomAnchorQ31 = 0;
    inGameRoot[0xbf].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0xbf].base.topAnchorQ31 = 0;
    inGameRoot[0xbf].base.rightAnchorQ31 = 0;
    inGameRoot[0xbf].base.bottomAnchorQ31 = 0;
    inGameRoot[0xbf].base.layoutWidth = 0;
    inGameRoot[0xc0].base.left = (sdword)pGVar13;
    inGameRoot[0xc0].base.rightAnchorQ31 = 0;
    inGameRoot[0xc0].base.bottomAnchorQ31 = 0;
    inGameRoot[0xc0].base.layoutWidth = 0;
    inGameRoot[0xc0].base.layoutHeight = 0;
    inGameRoot[0xc1].base.top = (sdword)pGVar13;
    inGameRoot[0xc1].base.bottomAnchorQ31 = 0;
    inGameRoot[0xc1].base.layoutWidth = 0;
    inGameRoot[0xc1].base.layoutHeight = 0;
    inGameRoot[0xc1].base.nodeFlags = 0;
    inGameRoot[0xc2].base.right = (sdword)pGVar13;
    inGameRoot[0xc2].base.layoutWidth = 0;
    inGameRoot[0xc2].base.layoutHeight = 0;
    inGameRoot[0xc2].base.nodeFlags = 0;
    inGameRoot[0xc2].rootFlags = 0;
    inGameRoot[0xc3].base.bottom = (sdword)pGVar13;
    inGameRoot[0xca].base.bottomAnchorQ31 = 0;
    inGameRoot[0xca].base.layoutWidth = 0;
    inGameRoot[0xca].base.layoutHeight = 0;
    inGameRoot[0xca].base.nodeFlags = 0;
    inGameRoot[0xcb].base.right = (sdword)pGVar13;
    inGameRoot[0xcc].base.bottom = (sdword)pGVar13;
    inGameRoot[0xcd].base.nodeFlags = 0;
    inGameRoot[0xcd].rootFlags = 0;
    inGameRoot[0xcd].callbacks = (UiRootCallbacks *)0x0;
    inGameRoot[0xcd].previousRoot = (UiRootNode *)0x0;
    inGameRoot[0xce].base.topOffset = (sdword)pGVar13;
    inGameRoot[0xcf].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0xd1].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0xd3].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0xe2].base.right = (sdword)pGVar13;
    inGameRoot[0x112].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x15a].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x19a].base.right = (sdword)pGVar13;
    inGameRoot[0xd0].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0xdd].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0xd2].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0xde].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0xd5].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0xdf].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0xc5].base.topOffset = (sdword)pGVar13;
    inGameRoot[0xc6].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[200].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0xe1].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x1bd].base.top = (sdword)pGVar13;
    inGameRoot[0x111].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x1be].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x159].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x1bf].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x199].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x206].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x208].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x209].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x20b].base.bottom = (sdword)pGVar13;
    inGameRoot[0x20c].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x20e].base.top = (sdword)pGVar13;
    inGameRoot[0x20f].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x211].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x212].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x213].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x215].base.bottom = (sdword)pGVar13;
    inGameRoot[0x216].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x218].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x219].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x21a].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x21c].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x21d].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x21e].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x220].base.top = (sdword)pGVar13;
    inGameRoot[0x221].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x222].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x224].base.bottom = (sdword)pGVar13;
    inGameRoot[0x225].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x227].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x228].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x229].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x22b].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x22c].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x22d].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x1c0].base.bottomAnchorQ31 = 0;
    inGameRoot[0x1c0].base.layoutWidth = 0;
    inGameRoot[0x1c0].base.layoutHeight = 0;
    inGameRoot[0x1c0].base.nodeFlags = 0;
    inGameRoot[0x1c1].previousRoot = (UiRootNode *)0x0;
    inGameRoot[0x1c2].base.nextSibling = (UiNodeBase *)0x0;
    inGameRoot[0x1c2].base.firstChild = (UiNodeBase *)0x0;
    inGameRoot[0x1c2].base.parent = (UiNodeBase *)0x0;
    inGameRoot[0x1c3].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x1c4].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x1c7].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x1cc].base.right = (sdword)pGVar13;
    inGameRoot[0x1ce].base.leftAnchorQ31 = 0;
    inGameRoot[0x1ce].base.topAnchorQ31 = 0;
    inGameRoot[0x1ce].base.rightAnchorQ31 = 0;
    inGameRoot[0x1ce].base.bottomAnchorQ31 = 0;
    inGameRoot[0x1d4].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x1d0].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x1e5].base.top = (sdword)pGVar13;
    inGameRoot[0x1e6].base.right = (sdword)pGVar13;
    inGameRoot[0x200].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x201].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x203].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x204].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x114].base.left = (sdword)pGVar13;
    inGameRoot[0x115].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x117].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x118].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x11a].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0x11b].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x11c].callbacks = (UiRootCallbacks *)pGVar13;
    inGameRoot[0x11e].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0x11f].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x121].base.right = (sdword)pGVar13;
    inGameRoot[0x122].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x124].base.left = (sdword)pGVar13;
    inGameRoot[0x125].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x127].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x128].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x12a].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[299].base.rightOffset = (sdword)pGVar13;
    inGameRoot[300].callbacks = (UiRootCallbacks *)pGVar13;
    inGameRoot[0x12e].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0x12f].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x131].base.right = (sdword)pGVar13;
    inGameRoot[0x132].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x134].base.left = (sdword)pGVar13;
    inGameRoot[0x135].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x137].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x138].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x13a].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0x13b].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x13c].callbacks = (UiRootCallbacks *)pGVar13;
    inGameRoot[0x13e].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0x13f].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x141].base.right = (sdword)pGVar13;
    inGameRoot[0x142].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x144].base.left = (sdword)pGVar13;
    inGameRoot[0x145].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x147].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x148].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x14a].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0x14b].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x14c].callbacks = (UiRootCallbacks *)pGVar13;
    inGameRoot[0x14e].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0x14f].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x151].base.right = (sdword)pGVar13;
    inGameRoot[0x152].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x154].base.left = (sdword)pGVar13;
    inGameRoot[0x155].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x157].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x158].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x15c].base.bottom = (sdword)pGVar13;
    inGameRoot[0x15d].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x15f].base.top = (sdword)pGVar13;
    inGameRoot[0x160].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x162].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x163].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x165].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x166].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x167].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x169].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x16a].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x16c].base.bottom = (sdword)pGVar13;
    inGameRoot[0x16d].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x16f].base.top = (sdword)pGVar13;
    inGameRoot[0x170].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x172].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x173].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x175].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x176].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x177].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x179].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x17a].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x17c].base.bottom = (sdword)pGVar13;
    inGameRoot[0x17d].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x17f].base.top = (sdword)pGVar13;
    inGameRoot[0x180].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x182].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x183].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x185].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x186].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x187].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x189].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x18a].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x18c].base.bottom = (sdword)pGVar13;
    inGameRoot[0x18d].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[399].base.top = (sdword)pGVar13;
    inGameRoot[400].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x192].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x193].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x195].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x196].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x197].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x19b].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x19d].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x19e].base.leftAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x19f].previousRoot = (UiRootNode *)pGVar13;
    inGameRoot[0x1a1].base.leftOffset = (sdword)pGVar13;
    inGameRoot[0x1a2].base.layoutHeight = (sdword)pGVar13;
    inGameRoot[0x1a4].base.left = (sdword)pGVar13;
    inGameRoot[0x1a5].base.topAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x1a7].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0x1a8].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x1a9].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x1ab].base.top = (sdword)pGVar13;
    inGameRoot[0x1ac].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x1ae].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x1af].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x1b0].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x1b2].base.right = (sdword)pGVar13;
    inGameRoot[0x1b3].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x1b5].base.parent = (UiNodeBase *)pGVar13;
    inGameRoot[0x1b6].base.bottomOffset = (sdword)pGVar13;
    inGameRoot[0x1b7].callbacks = (UiRootCallbacks *)pGVar13;
    inGameRoot[0x1b9].base.bottom = (sdword)pGVar13;
    inGameRoot[0x1ba].base.layoutWidth = (sdword)pGVar13;
    inGameRoot[0x1bc].base.vtable = (UiNodeVtable *)pGVar13;
    inGameRoot[0x108].base.nextSibling = (UiNodeBase *)pGVar13;
    inGameRoot[0x107].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x109].base.topOffset = (sdword)pGVar13;
    inGameRoot[0x109].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x10a].base.nodeFlags = (UiNodeFlags)pGVar13;
    inGameRoot[0x10a].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x10c].base.top = (sdword)pGVar13;
    inGameRoot[0x10b].rootFlags = (UiRootFlags)pGVar13;
    inGameRoot[0x10d].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x10d].base.right = (sdword)pGVar13;
    inGameRoot[0x10f].base.firstChild = (UiNodeBase *)pGVar13;
    inGameRoot[0x10e].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar13;
    inGameRoot[0x110].base.rightOffset = (sdword)pGVar13;
    inGameRoot[0x110].base.parent = (UiNodeBase *)pGVar13;
    sVar9 = g_InGamePanelTextureSubresource01Width;
    psVar1 = &inGameRoot[0xbd].base.bottomOffset;
    *psVar1 = *psVar1 - g_InGamePanelTextureSubresource01Width;
    pUVar2 = &inGameRoot[0xbd].base.topAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xbe].base.leftAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xbf].base.topAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xbf].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xc0].base.rightAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    psVar1 = &inGameRoot[0xc0].base.layoutWidth;
    *psVar1 = *psVar1 - sVar9;
    pUVar2 = &inGameRoot[0xc1].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    psVar1 = &inGameRoot[0xc1].base.layoutHeight;
    *psVar1 = *psVar1 - sVar9;
    psVar1 = &inGameRoot[0xc2].base.layoutWidth;
    *psVar1 = *psVar1 - sVar9;
    pUVar3 = &inGameRoot[0xc2].base.nodeFlags;
    *pUVar3 = *pUVar3 - sVar9;
    pUVar2 = &inGameRoot[0x1c0].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    psVar1 = &inGameRoot[0x1c0].base.layoutHeight;
    *psVar1 = *psVar1 - sVar9;
    inGameRoot[0x1c1].previousRoot = (UiRootNode *)((int)inGameRoot[0x1c1].previousRoot - sVar9);
    ppUVar4 = &inGameRoot[0x1c2].base.firstChild;
    *ppUVar4 = (UiNodeBase *)((int)*ppUVar4 - sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.leftAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0x1ce].base.rightAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    sVar9 = g_InGamePanelTextureSubresource02Width;
    psVar1 = &inGameRoot[0xbd].base.bottomOffset;
    *psVar1 = *psVar1 - g_InGamePanelTextureSubresource02Width;
    pUVar2 = &inGameRoot[0xbd].base.topAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xbf].base.topAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xc0].base.rightAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar2 = &inGameRoot[0xc1].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    psVar1 = &inGameRoot[0xc2].base.layoutWidth;
    *psVar1 = *psVar1 - sVar9;
    pUVar2 = &inGameRoot[0x1c0].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    inGameRoot[0x1c1].previousRoot = (UiRootNode *)((int)inGameRoot[0x1c1].previousRoot - sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.leftAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    psVar1 = &inGameRoot[0xbd].base.bottomOffset;
    *psVar1 = *psVar1 - g_InGamePanelTextureSubresource00Width;
    pUVar2 = &inGameRoot[0xca].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - g_InGamePanelTextureSubresource06Width;
    pUVar3 = &inGameRoot[0xcd].base.nodeFlags;
    *pUVar3 = *pUVar3 - g_InGamePanelTextureSubresource07Width;
    sVar9 = g_InGamePanelTextureSubresource02Height;
    psVar1 = &inGameRoot[0xbf].base.layoutWidth;
    *psVar1 = *psVar1 + g_InGamePanelTextureSubresource02Height;
    pUVar2 = &inGameRoot[0xc0].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    psVar1 = &inGameRoot[0xc0].base.layoutHeight;
    *psVar1 = *psVar1 + sVar9;
    psVar1 = &inGameRoot[0xc1].base.layoutWidth;
    *psVar1 = *psVar1 + sVar9;
    pUVar3 = &inGameRoot[0xc1].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    psVar1 = &inGameRoot[0x1c0].base.layoutWidth;
    *psVar1 = *psVar1 + sVar9;
    pUVar3 = &inGameRoot[0x1c0].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    inGameRoot[0x1c2].base.nextSibling =
         (UiNodeBase *)((int)&(inGameRoot[0x1c2].base.nextSibling)->nextSibling + sVar9);
    ppUVar4 = &inGameRoot[0x1c2].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)&(*ppUVar4)->nextSibling + sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource36Height;
    pUVar2 = &inGameRoot[0xc0].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 + g_InGamePanelTextureSubresource36Height;
    psVar1 = &inGameRoot[0xc0].base.layoutHeight;
    *psVar1 = *psVar1 + sVar9;
    psVar1 = &inGameRoot[0xc1].base.layoutWidth;
    *psVar1 = *psVar1 + sVar9;
    pUVar3 = &inGameRoot[0xc1].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    pUVar3 = &inGameRoot[0x1c0].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    inGameRoot[0x1c2].base.nextSibling =
         (UiNodeBase *)((int)&(inGameRoot[0x1c2].base.nextSibling)->nextSibling + sVar9);
    ppUVar4 = &inGameRoot[0x1c2].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)&(*ppUVar4)->nextSibling + sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource03Height;
    psVar1 = &inGameRoot[0xc0].base.layoutHeight;
    *psVar1 = *psVar1 + g_InGamePanelTextureSubresource03Height;
    psVar1 = &inGameRoot[0xc1].base.layoutWidth;
    *psVar1 = *psVar1 + sVar9;
    pUVar3 = &inGameRoot[0xc1].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    inGameRoot[0x1c2].base.nextSibling =
         (UiNodeBase *)((int)&(inGameRoot[0x1c2].base.nextSibling)->nextSibling + sVar9);
    ppUVar4 = &inGameRoot[0x1c2].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)&(*ppUVar4)->nextSibling + sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource37Height;
    psVar1 = &inGameRoot[0xc1].base.layoutWidth;
    *psVar1 = *psVar1 + g_InGamePanelTextureSubresource37Height;
    pUVar3 = &inGameRoot[0xc1].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    ppUVar4 = &inGameRoot[0x1c2].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)&(*ppUVar4)->nextSibling + sVar9);
    pUVar2 = &inGameRoot[0x1ce].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource04Height;
    pUVar3 = &inGameRoot[0xc1].base.nodeFlags;
    *pUVar3 = *pUVar3 + g_InGamePanelTextureSubresource04Height;
    pUVar2 = &inGameRoot[0x1ce].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource00Height;
    psVar1 = &inGameRoot[0xc2].base.layoutHeight;
    *psVar1 = *psVar1 + g_InGamePanelTextureSubresource00Height;
    inGameRoot[0xc2].rootFlags = inGameRoot[0xc2].rootFlags + sVar9;
    pUVar2 = &inGameRoot[0x1ce].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 + sVar9;
    sVar9 = g_InGamePanelTextureSubresource05Height;
    psVar1 = &inGameRoot[0xc2].base.layoutHeight;
    *psVar1 = *psVar1 - g_InGamePanelTextureSubresource05Height;
    pUVar2 = &inGameRoot[0x1ce].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 - sVar9;
    pUVar3 = &inGameRoot[0xca].base.nodeFlags;
    *pUVar3 = *pUVar3 + g_InGamePanelTextureSubresource06Height;
    inGameRoot[0xcd].rootFlags =
         inGameRoot[0xcd].rootFlags - g_InGamePanelTextureSubresource07Height;
    UVar6 = inGameRoot[0xc0].base.bottomAnchorQ31;
    pUVar17 = (UiRootNode *)inGameRoot[0xc0].base.layoutHeight;
    inGameRoot[0xc4].rootFlags = UVar6;
    inGameRoot[0xc4].previousRoot = pUVar17;
    inGameRoot[0xc6].base.top = UVar6;
    inGameRoot[0xc6].base.bottom = (sdword)pUVar17;
    inGameRoot[199].base.topAnchorQ31 = UVar6;
    inGameRoot[199].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    UVar7 = inGameRoot[0xbd].base.bottomOffset;
    pUVar8 = (UiRootCallbacks *)inGameRoot[0xbe].base.rightAnchorQ31;
    inGameRoot[0xc4].base.nodeFlags = UVar7;
    inGameRoot[0xc4].callbacks = pUVar8;
    inGameRoot[0xc6].base.left = UVar7;
    inGameRoot[0xc6].base.right = (sdword)pUVar8;
    inGameRoot[199].base.leftAnchorQ31 = UVar7;
    inGameRoot[199].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar8;
    pUVar18 = (UiNodeVtable *)inGameRoot[0xc1].base.layoutWidth;
    pUVar17 = (UiRootNode *)inGameRoot[0xc1].base.nodeFlags;
    inGameRoot[0x206].base.topOffset = (sdword)pUVar18;
    inGameRoot[0x206].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x207].rootFlags = (UiRootFlags)pUVar18;
    inGameRoot[0x207].previousRoot = pUVar17;
    inGameRoot[0x209].base.bottom = (sdword)pUVar18;
    inGameRoot[0x209].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x20a].base.layoutHeight = (sdword)pUVar18;
    inGameRoot[0x20a].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x20c].base.top = (sdword)pUVar18;
    inGameRoot[0x20c].base.bottom = (sdword)pUVar17;
    inGameRoot[0x20d].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar18;
    inGameRoot[0x20d].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x20f].base.vtable = pUVar18;
    inGameRoot[0x20f].base.top = (sdword)pUVar17;
    inGameRoot[0x210].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar18;
    inGameRoot[0x210].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    pUVar22 = (UiNodeBase *)inGameRoot[0xbd].base.bottomOffset;
    pUVar8 = (UiRootCallbacks *)inGameRoot[0xbe].base.rightAnchorQ31;
    inGameRoot[0x206].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x206].base.rightOffset = (sdword)pUVar8;
    inGameRoot[0x207].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x207].callbacks = pUVar8;
    inGameRoot[0x209].base.right = (sdword)pUVar22;
    inGameRoot[0x209].base.leftOffset = (sdword)pUVar8;
    inGameRoot[0x20a].base.layoutWidth = (sdword)pUVar22;
    inGameRoot[0x20a].base.nodeFlags = (UiNodeFlags)pUVar8;
    inGameRoot[0x20c].base.left = (sdword)pUVar22;
    inGameRoot[0x20c].base.right = (sdword)pUVar8;
    inGameRoot[0x20d].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x20d].base.layoutWidth = (sdword)pUVar8;
    inGameRoot[0x20f].base.parent = pUVar22;
    inGameRoot[0x20f].base.left = (sdword)pUVar8;
    inGameRoot[0x210].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x210].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar8;
    psVar1 = &inGameRoot[0x206].base.leftOffset;
    *psVar1 = *psVar1 + 5;
    pUVar3 = &inGameRoot[0x207].base.nodeFlags;
    *pUVar3 = *pUVar3 + (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_HAS_KEYBOARD_FOCUS);
    psVar1 = &inGameRoot[0x209].base.right;
    *psVar1 = *psVar1 + 0x42;
    psVar1 = &inGameRoot[0x20a].base.layoutWidth;
    *psVar1 = *psVar1 + 0x61;
    psVar1 = &inGameRoot[0x20c].base.left;
    *psVar1 = *psVar1 + 5;
    pUVar2 = &inGameRoot[0x20d].base.rightAnchorQ31;
    *pUVar2 = *pUVar2 + 0x24;
    ppUVar4 = &inGameRoot[0x20f].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)&(*ppUVar4)->layoutWidth + 2);
    pUVar2 = &inGameRoot[0x210].base.leftAnchorQ31;
    *pUVar2 = *pUVar2 + 0x61;
    psVar1 = &inGameRoot[0x206].base.topOffset;
    *psVar1 = *psVar1 + 0x11;
    inGameRoot[0x207].rootFlags = inGameRoot[0x207].rootFlags + 0x11;
    psVar1 = &inGameRoot[0x209].base.bottom;
    *psVar1 = *psVar1 + 0x11;
    psVar1 = &inGameRoot[0x20a].base.layoutHeight;
    *psVar1 = *psVar1 + 0x11;
    psVar1 = &inGameRoot[0x20c].base.top;
    *psVar1 = *psVar1 + 0x28;
    pUVar2 = &inGameRoot[0x20d].base.bottomAnchorQ31;
    *pUVar2 = *pUVar2 + 0x28;
    ppUVar5 = &inGameRoot[0x20f].base.vtable;
    *ppUVar5 = (UiNodeVtable *)&(*ppUVar5)->pointerMove;
    pUVar2 = &inGameRoot[0x210].base.topAnchorQ31;
    *pUVar2 = *pUVar2 + 0x28;
    pUVar18 = (UiNodeVtable *)inGameRoot[0xbd].base.bottomOffset;
    inGameRoot[0x1d].base.firstChild = (UiNodeBase *)0x0;
    inGameRoot[0x1d].base.parent = (UiNodeBase *)0x0;
    inGameRoot[0x1d].base.vtable = pUVar18;
    inGameRoot[0x1d].base.left = 0;
    inGameRoot[0xbc].base.bottomOffset = (sdword)pUVar18;
    inGameRoot[0xbc].base.leftAnchorQ31 = 0;
    inGameRoot[0xbc].base.topAnchorQ31 = 0;
    inGameRoot[0xbc].base.rightAnchorQ31 = 0;
    pUVar17 = (UiRootNode *)-g_InGamePanelTextureSubresource28Width;
    pUVar21 = (UiRootNode *)-g_InGamePanelTextureSubresource31Height;
    inGameRoot[0x113].base.bottom = (sdword)pUVar17;
    inGameRoot[0x113].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x114].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x116].base.right = (sdword)pUVar21;
    inGameRoot[0x117].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x119].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x11e].previousRoot = pUVar17;
    inGameRoot[0x124].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x12a].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x130].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x136].base.right = (sdword)pUVar21;
    inGameRoot[0x13f].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x147].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x150].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x15b].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x15b].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x15c].previousRoot = pUVar21;
    inGameRoot[0x15e].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x15f].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x161].base.right = (sdword)pUVar17;
    inGameRoot[0x167].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x16c].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x172].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x178].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x17e].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x184].base.top = (sdword)pUVar21;
    inGameRoot[0x18e].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x19a].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x19a].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x19c].base.bottom = (sdword)pUVar21;
    inGameRoot[0x19d].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x19f].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x1a0].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x1a6].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x1ab].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x1b1].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x1b7].base.firstChild = (UiNodeBase *)pUVar17;
    pUVar17 = (UiRootNode *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar21 = (UiRootNode *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x113].base.top = (sdword)pUVar17;
    inGameRoot[0x113].base.right = (sdword)pUVar21;
    inGameRoot[0x114].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x114].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x116].base.left = (sdword)pUVar21;
    inGameRoot[0x117].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x119].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x119].base.left = (sdword)pUVar21;
    inGameRoot[0x11a].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x11a].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x11c].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x11d].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x11e].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x120].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x124].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x126].base.top = (sdword)pUVar17;
    inGameRoot[0x12a].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[300].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x130].base.bottom = (sdword)pUVar17;
    inGameRoot[0x131].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x136].base.left = (sdword)pUVar21;
    inGameRoot[0x137].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x13e].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x140].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x147].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x149].base.left = (sdword)pUVar21;
    inGameRoot[0x150].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x151].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x15b].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x15b].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x15c].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x15c].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x15e].base.bottom = (sdword)pUVar21;
    inGameRoot[0x15f].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x161].base.left = (sdword)pUVar17;
    inGameRoot[0x161].base.bottom = (sdword)pUVar21;
    inGameRoot[0x162].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x162].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x164].base.top = (sdword)pUVar21;
    inGameRoot[0x165].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x167].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x168].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x16c].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x16e].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x172].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x174].base.left = (sdword)pUVar17;
    inGameRoot[0x178].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x17a].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x17e].base.right = (sdword)pUVar17;
    inGameRoot[0x17f].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x184].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x185].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x18e].base.bottom = (sdword)pUVar21;
    inGameRoot[399].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x19a].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x19a].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x19c].base.right = (sdword)pUVar17;
    inGameRoot[0x19c].base.top = (sdword)pUVar21;
    inGameRoot[0x19d].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x19f].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x1a0].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x1a0].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x1a1].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x1a1].previousRoot = pUVar21;
    inGameRoot[0x1a3].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x1a4].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x1a6].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x1a7].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x1ab].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x1ad].base.left = (sdword)pUVar17;
    inGameRoot[0x1b1].base.bottom = (sdword)pUVar17;
    inGameRoot[0x1b2].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x1b6].previousRoot = pUVar17;
    inGameRoot[0x1b8].base.rightOffset = (sdword)pUVar17;
    pUVar17 = (UiRootNode *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar22 = (UiNodeBase *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x114].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x116].base.top = (sdword)pUVar17;
    inGameRoot[0x119].base.parent = pUVar22;
    inGameRoot[0x11a].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x11a].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x11c].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x11c].base.nextSibling = pUVar22;
    inGameRoot[0x11d].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0x11f].base.nextSibling = pUVar22;
    inGameRoot[0x120].base.bottom = (sdword)pUVar17;
    inGameRoot[0x120].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0x121].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x121].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x123].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x126].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x127].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[299].previousRoot = pUVar17;
    inGameRoot[0x12d].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x131].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x133].base.bottom = (sdword)pUVar17;
    inGameRoot[0x137].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x139].base.left = (sdword)pUVar22;
    inGameRoot[0x140].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x141].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x149].base.parent = pUVar22;
    inGameRoot[0x14a].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x151].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x153].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x15c].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x15e].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x161].base.top = (sdword)pUVar22;
    inGameRoot[0x162].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x162].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x164].base.left = (sdword)pUVar17;
    inGameRoot[0x164].base.vtable = (UiNodeVtable *)pUVar22;
    inGameRoot[0x165].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x167].base.vtable = (UiNodeVtable *)pUVar22;
    inGameRoot[0x168].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x168].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x16a].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x16a].base.firstChild = pUVar22;
    inGameRoot[0x16b].base.bottomOffset = (sdword)pUVar22;
    inGameRoot[0x16e].base.right = (sdword)pUVar17;
    inGameRoot[0x16f].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x174].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x175].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x179].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x17b].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x17f].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x181].base.right = (sdword)pUVar17;
    inGameRoot[0x185].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x187].base.vtable = (UiNodeVtable *)pUVar22;
    inGameRoot[399].base.layoutHeight = (sdword)pUVar22;
    inGameRoot[0x191].base.bottom = (sdword)pUVar22;
    inGameRoot[0x19c].base.left = (sdword)pUVar17;
    inGameRoot[0x19d].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x1a0].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0x1a1].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x1a1].rootFlags = (UiRootFlags)pUVar22;
    inGameRoot[0x1a3].base.bottom = (sdword)pUVar17;
    inGameRoot[0x1a3].base.right = (sdword)pUVar22;
    inGameRoot[0x1a4].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1a6].base.left = (sdword)pUVar22;
    inGameRoot[0x1a7].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x1a7].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1a8].previousRoot = pUVar17;
    inGameRoot[0x1a9].base.nextSibling = pUVar22;
    inGameRoot[0x1aa].base.topOffset = (sdword)pUVar22;
    inGameRoot[0x1ad].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x1ae].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x1b2].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x1b4].base.top = (sdword)pUVar17;
    inGameRoot[0x1b8].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x1b9].rootFlags = (UiRootFlags)pUVar17;
    pUVar17 = (UiRootNode *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar21 = (UiRootNode *)((int)pUVar22 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x116].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x117].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x11b].previousRoot = pUVar17;
    inGameRoot[0x11d].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x11e].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x120].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x121].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x121].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x123].base.bottom = (sdword)pUVar17;
    inGameRoot[0x123].base.right = (sdword)pUVar21;
    inGameRoot[0x124].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x126].base.right = (sdword)pUVar21;
    inGameRoot[0x127].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x127].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x129].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x129].base.left = (sdword)pUVar21;
    inGameRoot[0x12d].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x12e].previousRoot = pUVar17;
    inGameRoot[0x133].base.top = (sdword)pUVar17;
    inGameRoot[0x134].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x139].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x13a].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x141].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x143].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x14a].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x14c].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x153].base.right = (sdword)pUVar21;
    inGameRoot[0x154].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x15e].base.right = (sdword)pUVar17;
    inGameRoot[0x15f].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x164].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x165].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x167].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x168].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x169].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x169].previousRoot = pUVar21;
    inGameRoot[0x16b].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x16b].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x16c].previousRoot = pUVar21;
    inGameRoot[0x16e].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x16f].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x16f].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x171].base.right = (sdword)pUVar17;
    inGameRoot[0x171].base.bottom = (sdword)pUVar21;
    inGameRoot[0x175].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x177].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x17b].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x17c].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x181].base.left = (sdword)pUVar17;
    inGameRoot[0x182].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x187].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x188].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x191].base.top = (sdword)pUVar21;
    inGameRoot[0x192].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x19d].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x19f].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x1a3].base.top = (sdword)pUVar17;
    inGameRoot[0x1a4].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x1a6].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x1a7].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x1a8].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x1a8].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x1aa].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x1aa].base.bottom = (sdword)pUVar21;
    inGameRoot[0x1ab].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x1ad].base.top = (sdword)pUVar21;
    inGameRoot[0x1ae].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x1ae].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x1b0].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x1b0].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x1b4].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x1b5].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x1b9].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x1bb].base.right = (sdword)pUVar17;
    pUVar22 = (UiNodeBase *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar21 = (UiRootNode *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x117].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x11d].base.topOffset = (sdword)pUVar22;
    inGameRoot[0x123].base.top = (sdword)pUVar22;
    inGameRoot[0x124].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x126].base.left = (sdword)pUVar21;
    inGameRoot[0x127].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x129].base.firstChild = pUVar22;
    inGameRoot[0x129].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x12a].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[300].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x12d].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x12e].rootFlags = (UiRootFlags)pUVar22;
    inGameRoot[0x12f].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x134].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x136].base.top = (sdword)pUVar22;
    inGameRoot[0x137].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x139].base.vtable = (UiNodeVtable *)pUVar22;
    inGameRoot[0x13a].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x13a].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x13c].base.firstChild = pUVar22;
    inGameRoot[0x13c].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x13d].base.bottomOffset = (sdword)pUVar22;
    inGameRoot[0x143].base.right = (sdword)pUVar21;
    inGameRoot[0x144].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x14c].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x14d].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x154].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x156].base.right = (sdword)pUVar21;
    inGameRoot[0x15f].base.layoutWidth = (sdword)pUVar22;
    inGameRoot[0x165].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x16b].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x16c].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x16e].base.bottom = (sdword)pUVar21;
    inGameRoot[0x16f].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x171].base.left = (sdword)pUVar22;
    inGameRoot[0x171].base.top = (sdword)pUVar21;
    inGameRoot[0x172].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x174].base.top = (sdword)pUVar21;
    inGameRoot[0x175].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x177].base.nextSibling = pUVar22;
    inGameRoot[0x177].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x17c].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x182].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x184].base.left = (sdword)pUVar22;
    inGameRoot[0x185].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x187].base.parent = pUVar22;
    inGameRoot[0x188].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x188].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x18a].base.nextSibling = pUVar22;
    inGameRoot[0x18a].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x18b].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0x18c].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x192].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x194].base.top = (sdword)pUVar21;
    inGameRoot[0x19f].base.nextSibling = pUVar22;
    inGameRoot[0x1a4].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1aa].base.right = (sdword)pUVar22;
    inGameRoot[0x1ab].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x1ad].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x1ae].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x1af].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x1af].previousRoot = pUVar21;
    inGameRoot[0x1b1].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x1b2].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x1b4].base.right = (sdword)pUVar21;
    inGameRoot[0x1b5].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1b5].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x1bb].base.left = (sdword)pUVar22;
    pUVar17 = (UiRootNode *)((int)pUVar22 - g_InGamePanelTextureSubresource34Width);
    pUVar21 = (UiRootNode *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x12a].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[300].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x12d].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x12e].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x130].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x131].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x133].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x134].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x136].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x137].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x139].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x13a].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x13b].previousRoot = pUVar17;
    inGameRoot[0x13c].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x13d].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x13d].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x13e].previousRoot = pUVar17;
    inGameRoot[0x140].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x141].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x143].base.bottom = (sdword)pUVar17;
    inGameRoot[0x144].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x144].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x146].base.top = (sdword)pUVar17;
    inGameRoot[0x146].base.right = (sdword)pUVar21;
    inGameRoot[0x14d].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x14f].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x156].base.left = (sdword)pUVar21;
    inGameRoot[0x157].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x172].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x174].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x175].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x177].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x178].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x17a].base.firstChild = (UiNodeBase *)pUVar21;
    inGameRoot[0x17b].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x17c].previousRoot = pUVar21;
    inGameRoot[0x184].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x185].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x187].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x188].base.rightOffset = (sdword)pUVar17;
    inGameRoot[0x189].callbacks = (UiRootCallbacks *)pUVar17;
    inGameRoot[0x189].previousRoot = pUVar21;
    inGameRoot[0x18b].base.leftOffset = (sdword)pUVar17;
    inGameRoot[0x18b].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x18c].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x18e].base.leftOffset = (sdword)pUVar17;
    inGameRoot[399].base.nodeFlags = (UiNodeFlags)pUVar17;
    inGameRoot[0x191].base.right = (sdword)pUVar17;
    inGameRoot[0x192].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x194].base.left = (sdword)pUVar17;
    inGameRoot[0x194].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x195].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x195].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x197].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x1b1].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x1b2].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x1b4].base.left = (sdword)pUVar21;
    inGameRoot[0x1b5].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x1b7].base.parent = (UiNodeBase *)pUVar21;
    inGameRoot[0x1b8].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x1b9].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x1bb].base.bottom = (sdword)pUVar21;
    pUVar17 = (UiRootNode *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar21 = (UiRootNode *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x130].base.leftOffset = (sdword)pUVar21;
    inGameRoot[0x131].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x133].base.right = (sdword)pUVar21;
    inGameRoot[0x134].base.layoutWidth = (sdword)pUVar21;
    inGameRoot[0x13d].base.rightOffset = (sdword)pUVar21;
    inGameRoot[0x13e].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x140].base.bottom = (sdword)pUVar17;
    inGameRoot[0x141].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x143].base.top = (sdword)pUVar17;
    inGameRoot[0x144].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x146].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x146].base.left = (sdword)pUVar21;
    inGameRoot[0x147].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x149].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x14a].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x14c].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x14d].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x14e].previousRoot = pUVar17;
    inGameRoot[0x14e].callbacks = (UiRootCallbacks *)pUVar21;
    inGameRoot[0x157].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x178].base.bottomOffset = (sdword)pUVar21;
    inGameRoot[0x179].previousRoot = pUVar21;
    inGameRoot[0x17b].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x17c].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x17e].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x17f].rootFlags = (UiRootFlags)pUVar21;
    inGameRoot[0x181].base.bottom = (sdword)pUVar21;
    inGameRoot[0x182].base.layoutHeight = (sdword)pUVar21;
    inGameRoot[0x18b].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x18c].previousRoot = pUVar21;
    inGameRoot[0x18e].base.right = (sdword)pUVar17;
    inGameRoot[399].base.layoutWidth = (sdword)pUVar17;
    inGameRoot[0x191].base.left = (sdword)pUVar17;
    inGameRoot[0x192].base.rightAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x194].base.parent = (UiNodeBase *)pUVar17;
    inGameRoot[0x195].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x195].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar21;
    inGameRoot[0x197].base.nextSibling = (UiNodeBase *)pUVar17;
    inGameRoot[0x197].base.vtable = (UiNodeVtable *)pUVar21;
    inGameRoot[0x1b7].base.nextSibling = (UiNodeBase *)pUVar21;
    inGameRoot[0x1b8].base.topOffset = (sdword)pUVar21;
    inGameRoot[0x1b9].base.nodeFlags = (UiNodeFlags)pUVar21;
    inGameRoot[0x1bb].base.top = (sdword)pUVar21;
    pUVar17 = (UiRootNode *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    pUVar22 = (UiNodeBase *)((int)pUVar21 - g_InGamePanelTextureSubresource34Height);
    inGameRoot[0x147].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x149].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x14a].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x14b].previousRoot = pUVar17;
    inGameRoot[0x14d].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x14e].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x150].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x151].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x153].base.bottom = (sdword)pUVar17;
    inGameRoot[0x154].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x156].base.top = (sdword)pUVar17;
    inGameRoot[0x157].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x17e].base.bottom = (sdword)pUVar22;
    inGameRoot[0x17f].base.layoutHeight = (sdword)pUVar22;
    inGameRoot[0x181].base.top = (sdword)pUVar22;
    inGameRoot[0x182].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x18c].rootFlags = (UiRootFlags)pUVar22;
    inGameRoot[0x197].base.firstChild = pUVar22;
    pUVar18 = (UiNodeVtable *)((int)pUVar17 - g_InGamePanelTextureSubresource34Width);
    inGameRoot[0x150].base.bottom = (sdword)pUVar18;
    inGameRoot[0x151].base.layoutHeight = (sdword)pUVar18;
    inGameRoot[0x153].base.top = (sdword)pUVar18;
    inGameRoot[0x154].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar18;
    inGameRoot[0x156].base.vtable = pUVar18;
    inGameRoot[0x157].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar18;
    sVar9 = g_InGamePanelTextureSubresource34Width;
    psVar1 = &inGameRoot[0x4e].base.layoutHeight;
    *psVar1 = *psVar1 + g_InGamePanelTextureSubresource34Width;
    pUVar3 = &inGameRoot[0x4d].base.nodeFlags;
    *pUVar3 = *pUVar3 + sVar9;
    sVar14 = g_InGamePanelTextureSubresource23Height;
    sVar9 = g_InGamePanelTextureSubresource19Width;
    UVar6 = -g_InGamePanelTextureSubresource20Width;
    inGameRoot[0xe2].callbacks =
         (UiRootCallbacks *)
         ((int)inGameRoot[0xe2].callbacks - g_InGamePanelTextureSubresource23Height);
    inGameRoot[0xe2].base.layoutHeight = sVar9;
    iVar23 = sVar14 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe2].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe2].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe3].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe3].callbacks - iVar23);
    inGameRoot[0xe3].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe3].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe3].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe4].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe4].callbacks - iVar23);
    inGameRoot[0xe4].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe4].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe4].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe5].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe5].callbacks - iVar23);
    inGameRoot[0xe5].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe5].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe5].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe6].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe6].callbacks - iVar23);
    inGameRoot[0xe6].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe6].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe6].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe7].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe7].callbacks - iVar23);
    inGameRoot[0xe7].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe7].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe7].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    inGameRoot[0xe8].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xe8].callbacks - iVar23);
    inGameRoot[0xe8].base.layoutHeight = sVar9;
    iVar23 = iVar23 + g_InGamePanelTextureSubresource32Height;
    inGameRoot[0xe8].rootFlags = UVar6;
    pUVar3 = &inGameRoot[0xe8].base.nodeFlags;
    *pUVar3 = *pUVar3 - iVar23;
    pUVar22 = (UiNodeBase *)-g_InGamePanelTextureSubresource33Width;
    inGameRoot[0x107].base.firstChild = pUVar22;
    inGameRoot[0xe9].rootFlags = (int)&pUVar22->nextSibling + inGameRoot[0xe9].rootFlags;
    psVar1 = &inGameRoot[0xf8].base.bottomOffset;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    pUVar3 = &inGameRoot[0xff].base.nodeFlags;
    *pUVar3 = (int)&pUVar22->nextSibling + *pUVar3;
    psVar1 = &inGameRoot[0xf1].base.left;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x108].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0xea].callbacks = (UiRootCallbacks *)((int)inGameRoot[0xea].callbacks + (int)pUVar22)
    ;
    pUVar2 = &inGameRoot[0xf9].base.leftAnchorQ31;
    *pUVar2 = (int)&pUVar22->nextSibling + *pUVar2;
    inGameRoot[0x100].rootFlags = (int)&pUVar22->nextSibling + inGameRoot[0x100].rootFlags;
    psVar1 = &inGameRoot[0xf2].base.top;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x109].rootFlags = (UiRootFlags)pUVar22;
    inGameRoot[0xeb].previousRoot =
         (UiRootNode *)((int)inGameRoot[0xeb].previousRoot + (int)pUVar22);
    pUVar2 = &inGameRoot[0xfa].base.topAnchorQ31;
    *pUVar2 = (int)&pUVar22->nextSibling + *pUVar2;
    inGameRoot[0x101].callbacks =
         (UiRootCallbacks *)((int)inGameRoot[0x101].callbacks + (int)pUVar22);
    psVar1 = &inGameRoot[0xf3].base.right;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x10b].base.right = (sdword)pUVar22;
    inGameRoot[0xed].base.nextSibling =
         (UiNodeBase *)((int)&pUVar22->nextSibling + (int)inGameRoot[0xed].base.nextSibling);
    pUVar2 = &inGameRoot[0xfb].base.rightAnchorQ31;
    *pUVar2 = (int)&pUVar22->nextSibling + *pUVar2;
    inGameRoot[0x102].previousRoot =
         (UiRootNode *)((int)inGameRoot[0x102].previousRoot + (int)pUVar22);
    psVar1 = &inGameRoot[0xf4].base.bottom;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x10c].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    ppUVar4 = &inGameRoot[0xee].base.firstChild;
    *ppUVar4 = (UiNodeBase *)((int)*ppUVar4 + (int)pUVar22);
    pUVar2 = &inGameRoot[0xfc].base.bottomAnchorQ31;
    *pUVar2 = (int)&pUVar22->nextSibling + *pUVar2;
    inGameRoot[0x104].base.nextSibling =
         (UiNodeBase *)((int)&pUVar22->nextSibling + (int)inGameRoot[0x104].base.nextSibling);
    psVar1 = &inGameRoot[0xf5].base.leftOffset;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x10e].base.parent = pUVar22;
    ppUVar4 = &inGameRoot[0xef].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)*ppUVar4 + (int)pUVar22);
    psVar1 = &inGameRoot[0xfd].base.layoutWidth;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    ppUVar4 = &inGameRoot[0x105].base.firstChild;
    *ppUVar4 = (UiNodeBase *)((int)*ppUVar4 + (int)pUVar22);
    psVar1 = &inGameRoot[0xf6].base.topOffset;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    inGameRoot[0x10f].base.bottomOffset = (sdword)pUVar22;
    ppUVar5 = &inGameRoot[0xf0].base.vtable;
    *ppUVar5 = (UiNodeVtable *)((int)*ppUVar5 + (int)pUVar22);
    psVar1 = &inGameRoot[0xfe].base.layoutHeight;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    ppUVar4 = &inGameRoot[0x106].base.parent;
    *ppUVar4 = (UiNodeBase *)((int)*ppUVar4 + (int)pUVar22);
    psVar1 = &inGameRoot[0xf7].base.rightOffset;
    *psVar1 = (int)&pUVar22->nextSibling + *psVar1;
    sVar9 = inGameRoot[0xca].base.layoutWidth;
    inGameRoot[0xcb].base.layoutWidth = inGameRoot[0xca].base.bottomAnchorQ31;
    inGameRoot[0xcb].base.layoutHeight = sVar9;
    UVar6 = inGameRoot[0xca].base.nodeFlags;
    inGameRoot[0xcb].base.nodeFlags = inGameRoot[0xca].base.layoutHeight;
    inGameRoot[0xcb].rootFlags = UVar6;
    inGameRoot[0xce].rootFlags = inGameRoot[0xcd].base.nodeFlags;
    inGameRoot[0xce].callbacks = (UiRootCallbacks *)inGameRoot[0xcd].rootFlags;
    inGameRoot[0xce].previousRoot = (UiRootNode *)inGameRoot[0xcd].callbacks;
    inGameRoot[0xcf].base.nextSibling = &(inGameRoot[0xcd].previousRoot)->base;
    pUVar22 = (UiNodeBase *)inGameRoot[0x206].base.leftOffset;
    pUVar17 = (UiRootNode *)inGameRoot[0x206].base.topOffset;
    inGameRoot[0x212].base.nextSibling = pUVar22;
    inGameRoot[0x212].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x217].base.rightOffset = (sdword)pUVar22;
    inGameRoot[0x217].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x21c].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x21c].previousRoot = pUVar17;
    inGameRoot[0x223].base.layoutWidth = (sdword)pUVar22;
    inGameRoot[0x223].base.layoutHeight = (sdword)pUVar17;
    inGameRoot[0x227].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x227].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x22b].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x22b].previousRoot = pUVar17;
    pUVar22 = (UiNodeBase *)inGameRoot[0x206].base.rightOffset;
    pUVar17 = (UiRootNode *)inGameRoot[0x206].base.bottomOffset;
    inGameRoot[0x212].base.parent = pUVar22;
    inGameRoot[0x212].base.vtable = (UiNodeVtable *)pUVar17;
    inGameRoot[0x217].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x217].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar17;
    inGameRoot[0x21d].base.nextSibling = pUVar22;
    inGameRoot[0x21d].base.firstChild = (UiNodeBase *)pUVar17;
    inGameRoot[0x223].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x223].rootFlags = (UiRootFlags)pUVar17;
    inGameRoot[0x227].callbacks = (UiRootCallbacks *)pUVar22;
    inGameRoot[0x227].previousRoot = pUVar17;
    inGameRoot[0x22c].base.nextSibling = pUVar22;
    inGameRoot[0x22c].base.firstChild = (UiNodeBase *)pUVar17;
    pUVar22 = (UiNodeBase *)inGameRoot[0x207].base.nodeFlags;
    pUVar18 = (UiNodeVtable *)inGameRoot[0x207].rootFlags;
    inGameRoot[0x213].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x213].base.topOffset = (sdword)pUVar18;
    inGameRoot[0x218].base.nodeFlags = (UiNodeFlags)pUVar22;
    inGameRoot[0x218].rootFlags = (UiRootFlags)pUVar18;
    inGameRoot[0x21e].base.right = (sdword)pUVar22;
    inGameRoot[0x21e].base.bottom = (sdword)pUVar18;
    inGameRoot[0x225].base.parent = pUVar22;
    inGameRoot[0x225].base.vtable = pUVar18;
    inGameRoot[0x229].base.left = (sdword)pUVar22;
    inGameRoot[0x229].base.top = (sdword)pUVar18;
    inGameRoot[0x22d].base.right = (sdword)pUVar22;
    inGameRoot[0x22d].base.bottom = (sdword)pUVar18;
    pUVar8 = inGameRoot[0x207].callbacks;
    pUVar17 = inGameRoot[0x207].previousRoot;
    inGameRoot[0x213].base.rightOffset = (sdword)pUVar8;
    inGameRoot[0x213].base.bottomOffset = (sdword)pUVar17;
    inGameRoot[0x218].callbacks = pUVar8;
    inGameRoot[0x218].previousRoot = pUVar17;
    inGameRoot[0x21e].base.leftOffset = (sdword)pUVar8;
    inGameRoot[0x21e].base.topOffset = (sdword)pUVar17;
    inGameRoot[0x225].base.left = (sdword)pUVar8;
    inGameRoot[0x225].base.top = (sdword)pUVar17;
    inGameRoot[0x229].base.right = (sdword)pUVar8;
    inGameRoot[0x229].base.bottom = (sdword)pUVar17;
    inGameRoot[0x22d].base.leftOffset = (sdword)pUVar8;
    inGameRoot[0x22d].base.topOffset = (sdword)pUVar17;
    UVar10 = inGameRoot[0x209].base.right;
    UVar11 = inGameRoot[0x209].base.bottom;
    inGameRoot[0x214].base.layoutWidth = UVar10;
    inGameRoot[0x214].base.layoutHeight = UVar11;
    inGameRoot[0x21a].base.left = UVar10;
    inGameRoot[0x21a].base.top = UVar11;
    inGameRoot[0x21f].base.rightAnchorQ31 = UVar10;
    inGameRoot[0x21f].base.bottomAnchorQ31 = UVar11;
    inGameRoot[0x226].base.rightOffset = UVar10;
    inGameRoot[0x226].base.bottomOffset = UVar11;
    inGameRoot[0x22a].base.leftAnchorQ31 = UVar10;
    inGameRoot[0x22a].base.topAnchorQ31 = UVar11;
    UVar7 = inGameRoot[0x209].base.leftOffset;
    UVar6 = inGameRoot[0x209].base.topOffset;
    inGameRoot[0x214].base.nodeFlags = UVar7;
    inGameRoot[0x214].rootFlags = UVar6;
    inGameRoot[0x21a].base.right = UVar7;
    inGameRoot[0x21a].base.bottom = UVar6;
    inGameRoot[0x21f].base.layoutWidth = UVar7;
    inGameRoot[0x21f].base.layoutHeight = UVar6;
    inGameRoot[0x226].base.leftAnchorQ31 = UVar7;
    inGameRoot[0x226].base.topAnchorQ31 = UVar6;
    inGameRoot[0x22a].base.rightAnchorQ31 = UVar7;
    inGameRoot[0x22a].base.bottomAnchorQ31 = UVar6;
    pUVar22 = (UiNodeBase *)inGameRoot[0x20f].base.vtable;
    inGameRoot[0x221].base.nextSibling = inGameRoot[0x20f].base.parent;
    inGameRoot[0x221].base.firstChild = pUVar22;
    pUVar18 = (UiNodeVtable *)inGameRoot[0x20f].base.top;
    inGameRoot[0x221].base.parent = (UiNodeBase *)inGameRoot[0x20f].base.left;
    inGameRoot[0x221].base.vtable = pUVar18;
    pUVar22 = (UiNodeBase *)inGameRoot[0x210].base.leftAnchorQ31;
    pUVar18 = (UiNodeVtable *)inGameRoot[0x210].base.topAnchorQ31;
    inGameRoot[0x216].base.parent = pUVar22;
    inGameRoot[0x216].base.vtable = pUVar18;
    inGameRoot[0x21b].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x21b].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar18;
    inGameRoot[0x222].base.leftOffset = (sdword)pUVar22;
    inGameRoot[0x222].base.topOffset = (sdword)pUVar18;
    UVar10 = inGameRoot[0x210].base.rightAnchorQ31;
    UVar11 = inGameRoot[0x210].base.bottomAnchorQ31;
    inGameRoot[0x216].base.left = UVar10;
    inGameRoot[0x216].base.top = UVar11;
    inGameRoot[0x21b].base.rightAnchorQ31 = UVar10;
    inGameRoot[0x21b].base.bottomAnchorQ31 = UVar11;
    inGameRoot[0x222].base.rightOffset = UVar10;
    inGameRoot[0x222].base.bottomOffset = UVar11;
    sVar9 = g_InGamePanelTextureSubresource34Height;
    pUVar22 = (UiNodeBase *)(g_InGamePanelTextureSubresource34Width + 2);
    pUVar24 = (UiNodeBase *)(g_InGamePanelTextureSubresource34Height + 2);
    inGameRoot[0x1d2].base.firstChild = pUVar22;
    inGameRoot[0x1d2].base.parent = pUVar24;
    inGameRoot[0x1d1].previousRoot = (UiRootNode *)0x2;
    inGameRoot[0x1d2].base.nextSibling = (UiNodeBase *)0x2;
    inGameRoot[0x1d5].base.leftAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1d5].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar24;
    inGameRoot[0x1d5].base.rightOffset = 2;
    inGameRoot[0x1d5].base.bottomOffset = 2;
    pUVar22 = (UiNodeBase *)(g_InGamePanelTextureSubresource02Width - 4);
    inGameRoot[0x1d3].base.firstChild = (UiNodeBase *)0x2;
    inGameRoot[0x1d3].base.parent = (UiNodeBase *)(sVar9 + 4);
    inGameRoot[0x1d3].base.vtable = (UiNodeVtable *)0xfffffffe;
    inGameRoot[0x1d3].base.topAnchorQ31 = (UiAnchorFractionQ31)pUVar22;
    inGameRoot[0x1d6].base.bottomOffset = 2;
    inGameRoot[0x1d6].base.leftAnchorQ31 = (UiAnchorFractionQ31)(sVar9 + 4);
    inGameRoot[0x1d6].base.topAnchorQ31 = 0xfffffffe;
    inGameRoot[0x1d7].base.firstChild = pUVar22;
    inGameRoot[0x201].callbacks = (UiRootCallbacks *)0x2;
    inGameRoot[0x201].previousRoot = (UiRootNode *)0x2;
    inGameRoot[0x202].base.nextSibling = (UiNodeBase *)0xfffffffe;
    inGameRoot[0x202].base.rightOffset = (sdword)pUVar22;
    uVar20 = 0x18002c;
    do {
      stream = TextResource_Resolve(uVar20);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (0,g_InGameSelectionDetailNameTextUtf16,stream);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (1,g_InGameSelectionDetailArmourTextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (2,g_InGameSelectionDetailWeaponName0TextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (3,g_InGameSelectionDetailWeaponName1TextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (4,g_InGameSelectionDetailWeaponName2TextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (5,g_InGameSelectionDetailTextSlot05Utf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (6,g_InGameSelectionDetailBuildXeniteCostTextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (7,g_InGameSelectionDetailBuildTimeTextUtf16,(word *)uVar29);
      uVar29 = RichTextCommandStream_PatchPayloadBySelector
                         (8,g_InGameSelectionDetailEnergyTextUtf16,(word *)uVar29);
      RichTextCommandStream_PatchPayloadBySelector
                (9,g_InGameSelectionDetailTextSlot09Utf16,(word *)uVar29);
      uVar20 = extraout_ECX;
    } while (extraout_ECX < 0x18004f);
    arg0 = 3;
    iVar23 = (int)((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
    uVar20 = 0;
    iVar19 = 0;
    arg1 = 0;
    do {
      iVar12 = (&g_InGameSelectionDetailControlOffsetTable32)[uVar20];
      *(int *)((int)&(inGameRoot->base).leftOffset + iVar12) = iVar19;
      *(dword *)((int)&(inGameRoot->base).topOffset + iVar12) = arg1;
      iVar19 = iVar19 + iVar23;
      arg1 = arg1 + iVar23;
      *(int *)((int)&(inGameRoot->base).rightOffset + iVar12) = iVar19;
      *(dword *)((int)&(inGameRoot->base).bottomOffset + iVar12) = arg1;
      uVar20 = uVar20 + 1;
      arg0 = arg0 - 1;
      if (arg0 == 0) {
        arg0 = 3;
        iVar19 = 0;
      }
      else {
        arg1 = arg1 - iVar23;
      }
      uVar27 = uVar20 < 0xc;
    } while ((bool)uVar27);
    pGVar16 = (*g_GraphicsTextureSourceLoadPackageAsset)
                        (arg0,arg1,(word *)u_gfx_panel_diagram0_gfx_00563120);
    pGVar13 = g_InGameDiagramTextureSource;
    if (!(bool)uVar27) {
      LOCK();
      UNLOCK();
      g_InGameDiagramTextureSource = pGVar16;
      (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
      inGameRoot[0xd6].base.bottomOffset = (sdword)arg1_00;
      inGameRoot[0xd8].base.top = (sdword)arg1_00;
      inGameRoot[0xd9].previousRoot = arg1_00;
      pGVar16 = (*g_GraphicsTextureSourceLoadPackageAsset)
                          (arg0_00,(dword)arg1_00,(word *)u_gfx_panel_window_gfx_0056318e);
      pGVar13 = g_InGameWindowTextureSource;
      if (!(bool)uVar27) {
        LOCK();
        UNLOCK();
        g_InGameWindowTextureSource = pGVar16;
        (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
        inGameRoot[0x2a].callbacks = (UiRootCallbacks *)arg1_01;
        inGameRoot[0x52].base.firstChild = arg1_01;
        inGameRoot[0x24].base.rightAnchorQ31 = (UiAnchorFractionQ31)arg1_01;
        inGameRoot[0x25].base.bottomAnchorQ31 = (UiAnchorFractionQ31)arg1_01;
        inGameRoot[0x26].base.layoutWidth = (sdword)arg1_01;
        inGameRoot[0x27].base.layoutHeight = (sdword)arg1_01;
        inGameRoot[0x28].base.nodeFlags = (UiNodeFlags)arg1_01;
        inGameRoot[0x29].rootFlags = (UiRootFlags)arg1_01;
        pGVar16 = (*g_GraphicsTextureSourceLoadPackageAsset)
                            (arg0_01,(dword)arg1_01,(word *)u_gfx_panel_tech_gfx_005630fa);
        pGVar13 = g_InGameTechnologyTextureSource;
        if (!(bool)uVar27) {
          LOCK();
          UNLOCK();
          g_InGameTechnologyTextureSource = pGVar16;
          (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
          inGameRoot[0x47].base.nextSibling = (UiNodeBase *)arg1_02;
          inGameRoot[0x48].base.firstChild = (UiNodeBase *)arg1_02;
          inGameRoot[0x49].base.parent = (UiNodeBase *)arg1_02;
          inGameRoot[0x4a].base.vtable = (UiNodeVtable *)arg1_02;
          inGameRoot[0x4b].base.left = (sdword)arg1_02;
          inGameRoot[0x4c].base.top = (sdword)arg1_02;
          inGameRoot[0x4d].base.right = (sdword)arg1_02;
          qVar28 = (*g_GraphicsTextureSourceGetLogicalSize)(0,arg1_02);
          uVar25 = (uint)(qVar28 >> 0x20);
          psVar1 = &inGameRoot[0x3e].base.right;
          *psVar1 = *psVar1 - uVar25;
          psVar1 = &inGameRoot[0x3f].base.rightOffset;
          *psVar1 = *psVar1 - uVar25;
          pUVar2 = &inGameRoot[0x40].base.rightAnchorQ31;
          *pUVar2 = *pUVar2 - uVar25;
          pUVar3 = &inGameRoot[0x41].base.nodeFlags;
          *pUVar3 = *pUVar3 - uVar25;
          inGameRoot[0x43].base.nextSibling =
               (UiNodeBase *)((int)inGameRoot[0x43].base.nextSibling - uVar25);
          psVar1 = &inGameRoot[0x44].base.left;
          *psVar1 = *psVar1 - uVar25;
          psVar1 = &inGameRoot[0x45].base.leftOffset;
          *psVar1 = *psVar1 - uVar25;
          inGameRoot[0x4e].callbacks = (UiRootCallbacks *)((int)inGameRoot[0x4e].callbacks - uVar25)
          ;
          uVar20 = (uint)((int)qVar28 * 7) >> 1;
          psVar1 = &inGameRoot[0x2a].base.bottom;
          *psVar1 = *psVar1 - uVar20;
          psVar1 = &inGameRoot[0x2a].base.topOffset;
          *psVar1 = *psVar1 + uVar20;
          psVar1 = &inGameRoot[0x2a].base.leftOffset;
          *psVar1 = *psVar1 - (uVar25 >> 1);
          psVar1 = &inGameRoot[0x2a].base.rightOffset;
          *psVar1 = *psVar1 + (uVar25 >> 1);
          inGameRoot[0x50].previousRoot =
               (UiRootNode *)
               ((inGameRoot[0x2a].base.topOffset - inGameRoot[0x2a].base.bottom) + -0x18 +
               (inGameRoot[0x4e].rootFlags - inGameRoot[0x4e].base.layoutHeight));
          puVar15 = g_UiButtonSoundVoiceSets7[0];
          inGameRoot[0xd0].base.layoutWidth = (sdword)g_UiButtonSoundVoiceSets7[0];
          inGameRoot[0xd3].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0xd5].base.topOffset = (sdword)puVar15;
          inGameRoot[0xe1].base.bottom = (sdword)puVar15;
          inGameRoot[0x111].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x159].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x199].base.bottom = (sdword)puVar15;
          inGameRoot[0xdd].base.bottom = (sdword)puVar15;
          inGameRoot[0xde].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0xe0].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x1bd].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x1be].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x1c0].base.right = (sdword)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[1];
          inGameRoot[0xc5].base.layoutWidth = (sdword)g_UiButtonSoundVoiceSets7[1];
          inGameRoot[199].base.parent = (UiNodeBase *)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[2];
          inGameRoot[0x207].base.right = (sdword)g_UiButtonSoundVoiceSets7[2];
          inGameRoot[0x208].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x20a].base.left = (sdword)puVar15;
          inGameRoot[0x20b].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x20d].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x20e].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x210].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x211].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x212].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x214].base.right = (sdword)puVar15;
          inGameRoot[0x215].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x217].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x218].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x219].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x21b].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x21c].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x21d].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x21f].base.left = (sdword)puVar15;
          inGameRoot[0x220].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x223].base.right = (sdword)puVar15;
          inGameRoot[0x224].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x226].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x227].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x228].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x22a].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x22b].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x22c].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x22e].base.left = (sdword)puVar15;
          inGameRoot[0x1d5].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x107].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x109].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x10a].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x10c].base.left = (sdword)puVar15;
          inGameRoot[0x10d].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x10f].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x110].base.topOffset = (sdword)puVar15;
          inGameRoot[0x114].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x115].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x117].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x118].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x119].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x11b].base.topOffset = (sdword)puVar15;
          inGameRoot[0x11c].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x11e].base.bottom = (sdword)puVar15;
          inGameRoot[0x11f].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x121].base.top = (sdword)puVar15;
          inGameRoot[0x122].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x124].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x125].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x127].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x128].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x129].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[299].base.topOffset = (sdword)puVar15;
          inGameRoot[300].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x12e].base.bottom = (sdword)puVar15;
          inGameRoot[0x12f].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x131].base.top = (sdword)puVar15;
          inGameRoot[0x132].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x134].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x135].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x137].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x138].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x139].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x13b].base.topOffset = (sdword)puVar15;
          inGameRoot[0x13c].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x13e].base.bottom = (sdword)puVar15;
          inGameRoot[0x13f].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x141].base.top = (sdword)puVar15;
          inGameRoot[0x142].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x144].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x145].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x147].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x148].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x149].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x14b].base.topOffset = (sdword)puVar15;
          inGameRoot[0x14c].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x14e].base.bottom = (sdword)puVar15;
          inGameRoot[0x14f].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x151].base.top = (sdword)puVar15;
          inGameRoot[0x152].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x154].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x155].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x157].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x158].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x15c].base.right = (sdword)puVar15;
          inGameRoot[0x15d].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x15f].base.left = (sdword)puVar15;
          inGameRoot[0x160].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x162].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x163].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x165].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x166].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x167].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x169].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x16a].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x16c].base.right = (sdword)puVar15;
          inGameRoot[0x16d].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x16f].base.left = (sdword)puVar15;
          inGameRoot[0x170].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x172].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x173].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x175].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x176].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x177].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x179].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x17a].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x17c].base.right = (sdword)puVar15;
          inGameRoot[0x17d].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x17f].base.left = (sdword)puVar15;
          inGameRoot[0x180].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x182].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x183].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x185].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x186].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x187].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x189].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x18a].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x18c].base.right = (sdword)puVar15;
          inGameRoot[0x18d].base.layoutWidth = (sdword)puVar15;
          inGameRoot[399].base.left = (sdword)puVar15;
          inGameRoot[400].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x192].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x193].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x195].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x196].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x197].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x19b].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x19d].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x19e].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x19f].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x1a1].base.bottom = (sdword)puVar15;
          inGameRoot[0x1a2].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x1a4].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0x1a5].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x1a6].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x1a8].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x1a9].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x1ab].base.left = (sdword)puVar15;
          inGameRoot[0x1ac].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x1ae].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x1af].base.topOffset = (sdword)puVar15;
          inGameRoot[0x1b0].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x1b2].base.top = (sdword)puVar15;
          inGameRoot[0x1b3].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x1b5].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x1b6].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x1b7].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x1b9].base.right = (sdword)puVar15;
          inGameRoot[0x1ba].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x1bc].base.parent = (UiNodeBase *)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[3];
          inGameRoot[0x11].base.parent = (UiNodeBase *)g_UiButtonSoundVoiceSets7[3];
          inGameRoot[0x12].base.left = (sdword)puVar15;
          inGameRoot[0x13].base.right = (sdword)puVar15;
          inGameRoot[0x14].base.leftOffset = (sdword)puVar15;
          inGameRoot[0x15].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x16].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x17].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x6d].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x6f].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x6e].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x70].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x7e].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x7f].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x81].previousRoot = (UiRootNode *)puVar15;
          inGameRoot[0x83].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x8d].base.right = (sdword)puVar15;
          inGameRoot[0x8f].base.topOffset = (sdword)puVar15;
          inGameRoot[0x90].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x91].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x92].base.bottomAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0xa6].base.bottomOffset = (sdword)puVar15;
          inGameRoot[0x58].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x57].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x56].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x3d].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x3c].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x2d].base.parent = (UiNodeBase *)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[4];
          inGameRoot[0x78].base.bottom = (sdword)g_UiButtonSoundVoiceSets7[4];
          inGameRoot[0x79].base.topOffset = (sdword)puVar15;
          inGameRoot[0x7b].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x7c].base.leftAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x7d].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x94].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0xa3].base.top = (sdword)puVar15;
          inGameRoot[0xa4].base.bottom = (sdword)puVar15;
          inGameRoot[0xa5].base.topOffset = (sdword)puVar15;
          inGameRoot[0xa8].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0xa9].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0xaa].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x96].rootFlags = (UiRootFlags)puVar15;
          inGameRoot[0x98].base.firstChild = (UiNodeBase *)puVar15;
          inGameRoot[0x99].base.top = (sdword)puVar15;
          inGameRoot[0x9a].base.topOffset = (sdword)puVar15;
          inGameRoot[0x9b].base.topAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x9c].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0x59].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x5c].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x5a].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x60].base.layoutWidth = (sdword)puVar15;
          inGameRoot[0x61].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x62].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[100].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x65].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x66].base.left = (sdword)puVar15;
          inGameRoot[0x67].base.right = (sdword)puVar15;
          inGameRoot[0x3e].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x40].base.parent = (UiNodeBase *)puVar15;
          inGameRoot[0x41].base.right = (sdword)puVar15;
          inGameRoot[0x42].base.rightOffset = (sdword)puVar15;
          inGameRoot[0x43].base.rightAnchorQ31 = (UiAnchorFractionQ31)puVar15;
          inGameRoot[0x44].base.nodeFlags = (UiNodeFlags)puVar15;
          inGameRoot[0x46].base.nextSibling = (UiNodeBase *)puVar15;
          inGameRoot[0x2e].base.left = (sdword)puVar15;
          inGameRoot[0x2f].base.right = (sdword)puVar15;
          inGameRoot[0x30].base.leftOffset = (sdword)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[5];
          inGameRoot[0xa1].base.left = (sdword)g_UiButtonSoundVoiceSets7[5];
          inGameRoot[0xaf].base.vtable = (UiNodeVtable *)puVar15;
          inGameRoot[0xb3].base.rightOffset = (sdword)puVar15;
          inGameRoot[0xb7].base.layoutHeight = (sdword)puVar15;
          inGameRoot[0xbc].base.parent = (UiNodeBase *)puVar15;
          puVar15 = g_UiButtonSoundVoiceSets7[6];
          inGameRoot[0x8a].base.topOffset = (sdword)g_UiButtonSoundVoiceSets7[6];
          inGameRoot[0x85].callbacks = (UiRootCallbacks *)puVar15;
          inGameRoot[0x54].base.leftOffset = (sdword)puVar15;
          inGameRoot[3].base.left = (sdword)puVar15;
        }
      }
    }
  }
  return;
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
void InGameHud_UpdateStatusCountersAndSessionPrompts(void)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  ulonglong uVar2;
  dword value;
  int iVar3;
  word *pwVar4;
  int iVar5;
  WideNumberSignedValue32 value_00;
  WideNumberSignedValue32 value_01;
  uint uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  WideNumberDenominator32 extraout_EDX;
  WideNumberDenominator32 denominator;
  WideNumberDenominator32 denominator_00;
  WideNumberDenominator32 denominator_01;
  WideNumberSignedValue32 value_02;
  WideNumberSignedValue32 value_03;
  FrontendPlayerRuntimeBlockCount extraout_EDX_00;
  FrontendPlayerRuntimeBlockCount FVar7;
  WorldRuntimeContext *world;
  FrontendPlayerRuntimeRecord *pFVar8;
  int iVar9;
  int iVar10;
  word *pwVar11;
  word *destination;
  bool bVar12;
  undefined1 uVar13;
  undefined8 uVar14;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  g_DebugOverlayCounterRefreshCountdown = g_DebugOverlayCounterRefreshCountdown - 1;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 0x14;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    denominator = extraout_EDX;
    if (extraout_EDX == 0) {
      denominator = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator_00,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator_01,
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
  gameRuntime1 = g_InGameRuntimeRoot;
  world = &g_InGameRuntimeRoot->worldRuntime0A30;
  uVar14 = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,(WideNumberSignedValue32)uVar14,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_00,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_02,g_FrontendDebugOverlayTextSlot06Utf16);
  uVar14 = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,(WideNumberSignedValue32)uVar14,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_01,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_03,g_FrontendDebugOverlayTextSlot09Utf16);
  if (*(int *)((gameRuntime1->worldRuntime0A30).selection.reserved04_1F + 0x10) == 0x7fffffff) {
    g_FrontendDebugOverlayTextSlot10Utf16[0] = 0x2d;
    g_FrontendDebugOverlayTextSlot10Utf16[1] = 0;
    g_FrontendDebugOverlayTextSlot11Utf16[0] = 0x2d;
    g_FrontendDebugOverlayTextSlot11Utf16[1] = 0;
  }
  else {
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,*(WideNumberSignedValue32 *)
                     ((gameRuntime1->worldRuntime0A30).selection.reserved04_1F + 8),
               g_FrontendDebugOverlayTextSlot10Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,*(WideNumberSignedValue32 *)
                     ((gameRuntime1->worldRuntime0A30).selection.reserved04_1F + 0xc),
               g_FrontendDebugOverlayTextSlot11Utf16);
  }
  value = (*g_MemoryApi.queryFreeBytes)();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,value,
             g_FrontendDebugOverlayTextSlot12Utf16);
  uVar2 = (ulonglong)(g_GameFactionRuntimeImage.tail.simulationTick + 0x4af) / 0x4b0;
  (*g_LocaleFormatTimeFieldsUtf16)
            ((dword)(uVar2 / 0x3c),(dword)(uVar2 % 0x3c),g_FrontendDebugOverlayTextSlot13Utf16);
  uVar6 = 1;
  iVar9 = 0x50fa80;
  destination = (word *)g_InGameFactionStatusTextScratchUtf16;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] != 0) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] <
        FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(iVar9 + 0x90) + *(int *)(iVar9 + 0x94),(word *)0x55056e);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster:
        TextResource_Resolve(0x21d4);
      }
      else {
        iVar10 = 0;
        iVar5 = extraout_ECX;
        FVar7 = g_FrontendPlayerRuntimeBlockCount;
        pFVar8 = g_FrontendPlayerRuntimeBlocks;
        pwVar4 = (word *)g_InGamePlayerListTextScratchUtf16;
        do {
          pwVar11 = pwVar4;
          if (iVar5 == (pFVar8->factionAssignment).factionAssignmentIndex) {
            if (iVar10 != 0) {
              pwVar4[0] = 0x2c;
              pwVar4[1] = 0x20;
              pwVar11 = pwVar4 + 2;
            }
            bVar12 = iVar10 != 0 && (word *)0xfffffffb < pwVar4;
            iVar10 = iVar10 + 1;
            iVar3 = RichTextCommandStream_CopyExpandedCf
                              (0x28,pwVar11,(pFVar8->playerName).textUtf16);
            iVar5 = extraout_ECX_00;
            FVar7 = extraout_EDX_00;
            if (!bVar12) {
              pwVar11 = (word *)((int)pwVar11 + iVar3);
              pSVar1 = g_SelectionPlayerRuntimeBlockPointers[pFVar8->playerRuntimeId];
              uVar6 = pSVar1->simulationStepTicks;
              if ((pSVar1->sessionFlags & 1) != 0) {
                pwVar11[0] = 0x20;
                pwVar11[1] = 0x20;
                pwVar11[2] = 0x50;
                pwVar11[3] = 0;
                pwVar11 = pwVar11 + 3;
              }
              if (1 < uVar6) {
                pwVar11[0] = 0x20;
                pwVar11[1] = 0x20;
                *(uint *)(pwVar11 + 2) = uVar6 * 0x10000 + 0x300078;
                pwVar11 = pwVar11 + 4;
              }
              if ((pSVar1->sessionFlags & 2) != 0) {
                pwVar11[0] = 0x20;
                pwVar11[1] = 0x20;
                pwVar11[2] = 0x8004;
                pwVar11[3] = 0x8003;
                pwVar11[4] = 0x57;
                pwVar11[5] = 0x8005;
                pwVar11 = pwVar11 + 6;
              }
            }
          }
          pFVar8 = pFVar8 + 1;
          FVar7 = FVar7 - 1;
          pwVar4 = pwVar11;
        } while (FVar7 != 0);
        *pwVar11 = 0;
        if (iVar10 == 0)
        goto 
        InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster
        ;
        pwVar4 = TextResource_Resolve(0x21d3);
        RichTextCommandStream_PatchPayloadBySelector(0,g_InGamePlayerListTextScratchUtf16,pwVar4);
      }
      uVar13 = 0xffffde8c < *(uint *)(iVar9 + 0x38);
      pwVar4 = TextResource_Resolve(*(uint *)(iVar9 + 0x38) + 0x2173);
      pwVar11 = TextResource_Resolve(0x21d2);
      uVar14 = RichTextCommandStream_PatchPayloadBySelector(0,pwVar4,pwVar11);
      uVar14 = RichTextCommandStream_PatchPayloadBySelector
                         (1,(void *)((ulonglong)uVar14 >> 0x20),(word *)uVar14);
      uVar14 = RichTextCommandStream_PatchPayloadBySelector(2,(void *)0x55056e,(word *)uVar14);
      iVar5 = RichTextCommandStream_CopyExpandedCf(0x400,destination,(word *)uVar14);
      uVar6 = extraout_ECX_01;
      if (!(bool)uVar13) {
        destination = (word *)((int)destination + iVar5);
      }
    }
    uVar6 = uVar6 + 1;
    iVar9 = iVar9 + 0x740;
    if (7 < uVar6) {
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
void InGamePanel_RebuildPlayerStatusRows(void *uiState)

{
  FrontendPlayerRuntimeRecord *pFVar1;
  TextResourceId resourceId;
  word *stream;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  int iVar2;
  InGamePlayerStatusTextSlot *destination;
  FrontendPlayerNameUtf16_28 *replacementPayload;
  RichTextExtentRegs RVar3;
  undefined8 uVar4;
  
  (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
  pFVar1 = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    replacementPayload = &pFVar1->playerName;
    RVar3 = RichTextCommandStream_MeasureRegs
                      (g_UiTextStyleNormal,(word *)u_gfx_panel_panel0_gfx_005630d0);
    iVar2 = (RVar3.heightPixels * extraout_ECX >> 1) + extraout_EDX;
    destination = g_InGamePlayerStatusTextSlots;
    *(int *)((int)uiState + 0x93c) = extraout_ECX;
    *(int *)((int)uiState + 0x910) = iVar2;
    *(int *)((int)uiState + 0x908) = -iVar2;
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
      stream = TextResource_Resolve(resourceId);
      uVar4 = RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
      RichTextCommandStream_CopyExpandedCf(0x80,destination->text,(word *)uVar4);
      destination = destination + 1;
      replacementPayload = replacementPayload + 0x7e;
    } while (extraout_ECX_00 != 1);
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
void InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf
               (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
               WorldRuntimeContext *inGameRuntime)

{
  UiCommandDispatchRecord *currentDispatchRecord;
  UiCommandDispatchRecord *dispatchRecordCursor;
  uint recordModifierFlags;
  
  dispatchRecordCursor = g_InGameCommandDispatchRecords_00_Code00030073_Modifier33;

  InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  :
  while( true ) {
    do {
      currentDispatchRecord = dispatchRecordCursor;
      recordModifierFlags = currentDispatchRecord->modifierClassFlags;
      if (currentDispatchRecord->commandCode == 0) {
        return;
      }
      dispatchRecordCursor = currentDispatchRecord + 1;
    } while (currentDispatchRecord->commandCode != commandCode);
    if (recordModifierFlags != 0) break;
    if ((modifierFlags & 0x3f) == 0) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  if ((recordModifierFlags & 3) == 0) {
    if ((modifierFlags & 3) != 0)
    goto 
    InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
    ;
  }
  else if ((modifierFlags & 3) == 0)
  goto 
  InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  ;
  if ((recordModifierFlags & 0x3c) == 0) {
    if ((modifierFlags & 0x3c) == 0) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  else if ((recordModifierFlags & 0x30) == 0) {
    if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) == 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  else if ((recordModifierFlags & 0xc) == 0) {
    if (((modifierFlags & 0xc) == 0) && ((modifierFlags & 0x30) != 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  else if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) != 0)) {
    (*(code *)currentDispatchRecord->continuationEntryAddress)();
    return;
  }
  goto 
  InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  ;
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
  bool bVar1;
  
  if ((((g_UiCommandRuntimeFlags & 0x101) == 0) &&
      (((context->interaction).interactionFlags48 & 8) == 0)) &&
     ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
    if ((context->runtimeFlags & 0x10) == 0) {
      bVar1 = false;
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        SelectionInfo_ValidateOwnerType16AndAnyActiveCf(context->activeFactionRuntimeIndex);
        if (bVar1) {
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
void InGameNotificationQueue_InsertPriorityRecord
               (InGameNotificationPayloadKind payloadKind,dword payloadReserved10,
               dword orientationOrPresentationValue0C,AngleTurn32 primaryOrientationAngle08,
               Q12 secondaryWorldCoordinateQ12_04,Q12 primaryWorldCoordinateQ12_00,
               InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId)

{
  Q12 *pQVar1;
  AngleTurn32 *pAVar2;
  dword *pdVar3;
  InGameNotificationPayloadKind *pIVar4;
  InGameNotificationPayloadKind IVar5;
  dword dVar6;
  dword dVar7;
  AngleTurn32 AVar8;
  Q12 QVar9;
  Q12 QVar10;
  InGameNotificationMovieId IVar11;
  uint uVar12;
  InGameNotificationMovieId IVar13;
  InGameNotificationQueueRecord20 *pIVar14;
  
  pIVar14 = g_InGameRuntimeRoot->notificationQueue9E60;
  IVar13 = 4;
  IVar5 = payloadKind;
  dVar6 = payloadReserved10;
  dVar7 = orientationOrPresentationValue0C;
  AVar8 = primaryOrientationAngle08;
  QVar9 = secondaryWorldCoordinateQ12_04;
  QVar10 = primaryWorldCoordinateQ12_00;
  IVar11 = notificationMovieId;
  while (IVar11 != 0) {
    IVar11 = notificationMovieId;
    uVar12 = priority;
    payloadKind = IVar5;
    payloadReserved10 = dVar6;
    orientationOrPresentationValue0C = dVar7;
    primaryOrientationAngle08 = AVar8;
    secondaryWorldCoordinateQ12_04 = QVar9;
    primaryWorldCoordinateQ12_00 = QVar10;
    if (pIVar14->priority04 < priority) {
      LOCK();
      IVar11 = pIVar14->notificationMovieId00;
      pIVar14->notificationMovieId00 = notificationMovieId;
      UNLOCK();
      LOCK();
      uVar12 = pIVar14->priority04;
      pIVar14->priority04 = priority;
      UNLOCK();
      LOCK();
      primaryWorldCoordinateQ12_00 = (pIVar14->payload08).primaryWorldCoordinateQ12_00;
      (pIVar14->payload08).primaryWorldCoordinateQ12_00 = QVar10;
      UNLOCK();
      LOCK();
      pQVar1 = &(pIVar14->payload08).secondaryWorldCoordinateQ12_04;
      secondaryWorldCoordinateQ12_04 = *pQVar1;
      *pQVar1 = QVar9;
      UNLOCK();
      LOCK();
      pAVar2 = &(pIVar14->payload08).primaryOrientationAngle08;
      primaryOrientationAngle08 = *pAVar2;
      *pAVar2 = AVar8;
      UNLOCK();
      LOCK();
      pdVar3 = &(pIVar14->payload08).orientationOrPresentationValue0C;
      orientationOrPresentationValue0C = *pdVar3;
      *pdVar3 = dVar7;
      UNLOCK();
      LOCK();
      pdVar3 = &(pIVar14->payload08).reserved10;
      payloadReserved10 = *pdVar3;
      *pdVar3 = dVar6;
      UNLOCK();
      LOCK();
      pIVar4 = &(pIVar14->payload08).payloadKind14;
      payloadKind = *pIVar4;
      *pIVar4 = IVar5;
      UNLOCK();
    }
    pIVar14 = pIVar14 + 1;
    IVar13 = IVar13 - 1;
    IVar5 = payloadKind;
    dVar6 = payloadReserved10;
    dVar7 = orientationOrPresentationValue0C;
    AVar8 = primaryOrientationAngle08;
    QVar9 = secondaryWorldCoordinateQ12_04;
    QVar10 = primaryWorldCoordinateQ12_00;
    priority = uVar12;
    notificationMovieId = IVar11;
    IVar11 = IVar13;
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
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  dword dVar3;
  SessionNetworkRoleFlags SVar4;
  UiControlCount UVar5;
  int iVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  dword dVar7;
  uint extraout_ECX_01;
  byte abVar8 [4];
  int iVar9;
  uint uVar10;
  SessionNetworkRoleFlags SVar11;
  FrontendPlayerRuntimeRecord *pFVar12;
  uint uVar13;
  UiGridDimensionsEdxEax8 UVar14;
  UiControlCount local_20;
  
  pUVar2 = node->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    pUVar2 = node->parent;
  }
  iVar6 = 1;
  local_20 = 0;
  abVar8 = g_GameFactionRuntimeImage.tail.reserved00_03;
  do {
    if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar6] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (iVar6 != node[0x23].bottom)) &&
       ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
      local_20 = local_20 + 1;
    }
    iVar6 = iVar6 + 1;
    abVar8 = (byte  [4])((int)abVar8 + -1);
  } while (abVar8 != (byte  [4])0x0);
  UVar14 = UiGrid_OneColumnDimensionsPacked(local_20);
  iVar6 = (int)UVar14 * g_InGamePanelTextureSubresource32Width +
          g_InGamePanelTextureSubresource19Width + g_InGamePanelTextureSubresource20Width;
  iVar9 = (int)(UVar14 >> 0x20) * g_InGamePanelTextureSubresource32Height +
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
  node[0x105].bottom = node[0x105].bottom - iVar6;
  node[0x105].leftOffset = node[0x105].leftOffset - iVar9;
  node[0x104].topOffset = node[0x104].topOffset | 8;
  if ((extraout_ECX != 0) && ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) {
    node[0x104].topOffset = node[0x104].topOffset & 0xfffffff7;
  }
  (**(code **)(node[0x103].topAnchorQ31 + 0xc))(&node[0x103].rightOffset);
  uVar13 = 0;
  if (extraout_ECX_00 != 0) {
    dVar7 = 1;
    iVar6 = 0x50fa80;
    do {
      UVar5 = local_20 + 1;
      if ((dVar7 != node[0x23].bottom) &&
         (g_GameFactionRuntimeImage.tail.factionLifecycleStates[dVar7] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE)) {
        g_UiAction1012TargetPlayerIndices[uVar13] = dVar7;
        UiPageStack_SetActiveIndex
                  (0,(UiPageStackControl *)
                     ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[uVar13]));
        *(int *)((int)&node[1].parent + g_UiAction1012PlayerLabelTextOffsets[uVar13]) =
             *(int *)(iVar6 + 0x38) + 0x2173;
        *(uint *)((int)&node[1].parent + g_UiAction1012PlayerIndexTextOffsets[uVar13]) =
             extraout_ECX_01 + 0x2190;
        uVar10 = g_GameFactionRuntimeImage.records[node[0x23].bottom].packedRelationStates >>
                 ((byte)(extraout_ECX_01 << 2) & 0x1f) & 0xf;
        dVar7 = extraout_ECX_01 & 0x3fffffff;
        *(uint *)((int)&node[1].parent + g_UiAction1012StateTextOffsets[uVar13]) = uVar10 + 0x21a3;
        iVar9 = g_UiAction1012IconImageOffsets[uVar13];
        *(undefined **)((int)&node[1].parent + iVar9) = &g_EmptyFrontendPlayerNameUtf16;
        pFVar12 = g_FrontendPlayerRuntimeBlocks;
        SVar11 = g_FrontendPlayerRuntimeBlockCount;
        SVar4 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (SVar4 != SESSION_NETWORK_ROLE_LOCAL) {
          if ((pFVar12->factionAssignment).factionAssignmentIndex == dVar7) {
            *(FrontendPlayerNameUtf16_28 **)((int)&node[1].parent + iVar9) = &pFVar12->playerName;
            break;
          }
          pFVar12 = pFVar12 + 1;
          SVar11 = SVar11 - SESSION_NETWORK_ROLE_CLIENT;
          SVar4 = SVar11;
        }
        iVar9 = g_UiAction1012ControlOffsets[uVar13];
        dVar3 = g_UiAction1012SubresourceByState[uVar10];
        uVar13 = uVar13 + 1;
        puVar1 = (uint *)((int)&node->nodeFlags + iVar9);
        *puVar1 = *puVar1 & 0xfffffff7;
        *(dword *)((int)&node[1].vtable + iVar9) = dVar3;
        UVar5 = local_20;
        if (((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) != 0) &&
           ((7 < uVar10 ||
            (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) != 0 &&
             ((3 < uVar10 || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) != 0)))))))) {
          puVar1 = (uint *)((int)&node->nodeFlags + iVar9);
          *puVar1 = *puVar1 | 8;
        }
      }
      local_20 = UVar5;
      dVar7 = dVar7 + 1;
      iVar6 = iVar6 + 0x740;
      local_20 = local_20 - 1;
    } while (local_20 != 0);
    if (6 < uVar13) {
      return;
    }
  }
  do {
    UiPageStack_SetActiveIndex
              (1,(UiPageStackControl *)
                 ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[uVar13]));
    uVar13 = uVar13 + 1;
  } while (uVar13 < 7);
  return;
}

/* Address: 0x0056A460.
   Ownership: ui/ingame/runtime.
   Purpose: Walks world entries and computes the weighted suitability score for a music track class. Typed
   parameters: p0 trackClassId→MusicTrackClassId_V343. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
dword InGameMusic_ComputeTrackSuitabilityScore
                (MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  ArmyAssetRecordPrefix *armyDefinition1;
  uint extraout_ECX;
  uint uVar2;
  uint extraout_EDX;
  int iVar3;
  int iVar4;
  bool bVar5;
  int iStack_28;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  WorldRuntimeNode *worldNode1;
  
  dVar1 = 0;
  iStack_1c = 0;
  iStack_20 = 0;
  iStack_24 = 0;
  iStack_28 = 0;
  if (trackClassId != 0) {
    uVar2 = worldRuntime->activeFactionRuntimeIndex;
    for (worldNode1 = worldRuntime->ownerListHead; worldNode1 != (WorldRuntimeNode *)0x0;
        worldNode1 = (worldNode1->common).nextNode) {
      if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        iVar3 = *(int *)((int)worldNode1->runtimePayload + 8);
        bVar5 = uVar2 < *(uint *)(iVar3 + 0xc);
        if ((uVar2 == *(uint *)(iVar3 + 0xc)) &&
           (armyDefinition1 = ArmyAssetRegistry_FindByIdCf(*(PckArmyAssetIdCatalog *)(iVar3 + 0xa0))
           , uVar2 = extraout_ECX, trackClassId = extraout_EDX, !bVar5)) {
          iVar4 = 1;
          if ((*(uint *)(iVar3 + 0x2c) & 1) != 0) {
            iVar4 = 3;
          }
          iStack_1c = iStack_1c + armyDefinition1[7].byteSize;
          iStack_20 = iStack_20 + iVar4 * armyDefinition1[7].registryId;
          iVar3 = 0x32;
          if ((armyDefinition1[1].selectionDetailTemplateVariantIndex & 0x10) == 0) {
            iVar3 = 0;
          }
          iStack_24 = iStack_24 + armyDefinition1[7].selectionDetailTemplateVariantIndex;
          iStack_28 = iStack_28 + iVar3;
        }
      }
    }
    if (trackClassId < 0x14) {
      dVar1 = iStack_28 * 0x80 + iStack_24 * 0x100 + iStack_20 * 0x280 + iStack_1c * 0x100;
    }
    else if (trackClassId < 0x32) {
      dVar1 = iStack_28 * -0x100 + iStack_24 * 0x40 + 0x32000 + iStack_20 * 0x10 + iStack_1c * 0x80;
    }
    else if (trackClassId < 0x46) {
      dVar1 = iStack_28 * 0x80 + iStack_24 * 0x100 + iStack_20 * 0x20 + iStack_1c * 0x300;
    }
    else {
      dVar1 = iStack_28 * 0x100 + iStack_24 * 0x200 + iStack_20 * 0x180 + iStack_1c * 0x10;
    }
  }
  return dVar1;
}

/* Address: 0x0056A8A0.
   Ownership: ui/ingame/runtime.
   Purpose: Recovered action-table target INGAME_PAGE10[31] (0x101F).
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiSelectableControl_SetSelected [ui/controls/lists], UiKeyboardFocus_ReleaseNode
   [ui/controls/input], TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureWrappedBlockRegs
   [assets/text/richtext].
*/
void InGameUiAction101F_Handler(void *source)

{
  int iVar1;
  void *pvVar2;
  UiPageStackControl *stack;
  UiPageStackControl *stack_00;
  word *pwVar3;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  bool bVar4;
  RichTextExtentRegs RVar5;
  undefined8 uVar6;
  
  iVar1 = *(int *)((int)source + 8);
  pvVar2 = source;
  while (iVar1 != -1) {
    pvVar2 = *(void **)((int)pvVar2 + 8);
    iVar1 = *(int *)((int)pvVar2 + 8);
  }
  bVar4 = (void *)0xfffff42f < pvVar2;
  UiSelectableControl_IsSelectedCf(source);
  if (!bVar4) {
    UiPageStack_SetActiveIndex(0,stack);
    *(uint *)(extraout_EDX_02 + 0xa78) = *(uint *)(extraout_EDX_02 + 0xa78) & 0xfffffff7;
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)(extraout_EDX + 0x4388));
  *(uint *)(extraout_EDX_00 + 0xa78) = *(uint *)(extraout_EDX_00 + 0xa78) | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)(extraout_EDX_00 + 0xa30));
  UiPageStack_SetActiveIndex(8,stack_00);
  *(int *)(extraout_EDX_01 + 0x11e4) =
       *(int *)(extraout_EDX_01 + 0xa80) + 0x230017 +
       *(int *)(g_InGameConditionRuntime[1].reserved5C_AB + 0x38) * 0x10;
  pwVar3 = TextResource_Resolve(*(TextResourceId *)(extraout_EDX_01 + 0x11e4));
  RVar5 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,pwVar3,*(UiPixelExtent *)(extraout_EDX_01 + 0x11e0));
  *(dword *)(extraout_EDX_01 + 0x11b8) = RVar5.widthPixels + 6;
  *(dword *)(extraout_EDX_01 + 0x11bc) = RVar5.heightPixels + 6;
  uVar6 = UiScrollableControl_RebuildViewportAndScrollbars
                    ((UiScrollableControl *)(extraout_EDX_01 + 0x1100));
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar6);
  pwVar3 = TextResource_Resolve(*(TextResourceId *)(extraout_EDX_01 + 0x12d0));
  RVar5 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,pwVar3,*(UiPixelExtent *)(extraout_EDX_01 + 0x12cc));
  *(dword *)(extraout_EDX_01 + 0x12a4) = RVar5.widthPixels + 6;
  *(dword *)(extraout_EDX_01 + 0x12a8) = RVar5.heightPixels + 6;
  uVar6 = UiScrollableControl_RebuildViewportAndScrollbars
                    ((UiScrollableControl *)(extraout_EDX_01 + 0x11ec));
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar6);
  pwVar3 = TextResource_Resolve(*(TextResourceId *)(extraout_EDX_01 + 0x1418));
  RVar5 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,pwVar3,*(UiPixelExtent *)(extraout_EDX_01 + 0x1414));
  *(dword *)(extraout_EDX_01 + 0x13ec) = RVar5.widthPixels + 6;
  *(dword *)(extraout_EDX_01 + 0x13f0) = RVar5.heightPixels + 6;
  uVar6 = UiScrollableControl_RebuildViewportAndScrollbars
                    ((UiScrollableControl *)(extraout_EDX_01 + 0x1334));
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar6);
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
void InGameUiAction101C_Handler(UiSelectableControl *selectableControl)

{
  UiPageIndex pageIndex;
  UiPageStackControl *stack;
  int parentNodeAddress;
  void *rootNodeCursor;
  
  parentNodeAddress = (int)(selectableControl->base).parent;
  rootNodeCursor = selectableControl;
  while (parentNodeAddress != -1) {
    rootNodeCursor = (((UiSelectableControl *)rootNodeCursor)->base).parent;
    parentNodeAddress = *(int *)((int)rootNodeCursor + 8);
  }
  UiSelectableGroup_SelectExclusive(3,&selectableControl->base);
  UiSelectableGroup_NoneVisibleSelectedCf(3);
  UiPageStack_SetActiveIndex(pageIndex,stack);
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
void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control)

{
  UiNodeBase *pUVar1;
  UiCommandSpriteButtonControl *pUVar2;
  CommandPayloadDword08 payloadDword08;
  GraphicsTextureSourceAsset *pGVar3;
  int iVar4;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    pUVar1 = (control->sprite).selectable.base.parent;
    pUVar2 = control;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      pUVar2 = (UiCommandSpriteButtonControl *)(pUVar2->sprite).selectable.base.parent;
      pUVar1 = (pUVar2->sprite).selectable.base.parent;
    }
    iVar4 = 6;
    while ((int)control - (int)pUVar2 != g_UiAction1012ControlOffsets[iVar4]) {
      iVar4 = iVar4 + -1;
      if (iVar4 < 0) {
        return;
      }
    }
    payloadDword08 = g_UiAction1012TargetPlayerIndices[iVar4];
    if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0) {
      pGVar3 = pUVar2[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_AdvancePairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,payloadDword08,(FactionRuntimeIndex)pGVar3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x660,0,payloadDword08,(CommandPayloadDword04)pGVar3);
      }
    }
    else {
      pGVar3 = pUVar2[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ResetPairwiseRelationState
                  (g_LocalPlayerRuntimeId,0,payloadDword08,(FactionRuntimeIndex)pGVar3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x7e0,0,payloadDword08,(CommandPayloadDword04)pGVar3);
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
void InGameSelectionPage_ToggleAndRefreshPage2(UiNodeBase *source)

{
  sdword *psVar1;
  UiNodeBase *pUVar2;
  int extraout_EAX;
  UiPageIndex pageIndex;
  int extraout_EAX_00;
  GameEntityRuntime *pGVar3;
  UiPageStackControl *stack;
  CommandPayloadDword04 modelOffset;
  undefined8 uVar4;
  
  pUVar2 = source->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    source = (((UiRootNode *)source)->base).parent;
    pUVar2 = (((UiRootNode *)source)->base).parent;
  }
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    psVar1 = &(((UiRootNode *)((int)source + 0xa50))->base).rightOffset;
    *psVar1 = *psVar1 & 0xfffffff7;
    UiPageStack_ActivePageNotInListCf
              ((UiPageStackControl *)&(((UiRootNode *)((int)source + 0xbb0))->base).leftOffset);
    if (extraout_EAX == 2) {
      pageIndex = 0;
    }
    else {
      pageIndex = 2;
    }
    UiPageStack_SetActiveIndex(pageIndex,stack);
    if ((extraout_EAX_00 == 2) &&
       (pGVar3 = SelectionInfo_GetFirstEntry(), pGVar3 != (GameEntityRuntime *)0x0)) {
      uVar4 = InGameTechnologyPanel_ResetAndSelectCurrentArea((UiRootNode *)source);
      modelOffset = (int)((ulonglong)uVar4 >> 0x20) - g_ModelRuntimeRebaseDelta;
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
void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  UiNodeBase *pUVar3;
  int iVar4;
  int iVar5;
  word *stream;
  word *replacementPayload;
  TextResourceId resourceId;
  TextResourceId extraout_ECX;
  uint uVar6;
  int iVar7;
  uint uVar8;
  undefined8 uVar9;
  
  pUVar2 = source->parent;
  pUVar3 = source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    pUVar3 = pUVar3->parent;
    pUVar2 = pUVar3->parent;
  }
  iVar5 = UiSelectableGroup_SelectExclusive(3,source);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(iVar5 + 0x1fa4));
  resourceId = 0x216d;
  uVar8 = 0;
  uVar6 = 1;
  iVar7 = 0x50fa80;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] != 0) {
      stream = TextResource_Resolve(resourceId);
      iVar4 = *(int *)(iVar7 + 0x38);
      puVar1 = (uint *)(iVar5 + g_UiSevenSlotSelectionControlOffsets[uVar8] + 0x48);
      *puVar1 = *puVar1 & 0xfffffff7;
      uVar8 = uVar8 + 1;
      replacementPayload = TextResource_Resolve(iVar4 + 0x2173);
      uVar9 = RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
      uVar6 = (uint)((ulonglong)uVar9 >> 0x20);
      resourceId = extraout_ECX;
    }
    uVar6 = uVar6 + 1;
    iVar7 = iVar7 + 0x740;
  } while (uVar6 < 8);
  *(uint *)(iVar5 + 0x20b8) = uVar8 * 0x18;
  uVar9 = UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)(iVar5 + 0x1ffc));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)((ulonglong)uVar9 >> 0x20));
  for (; uVar8 < 7; uVar8 = uVar8 + 1) {
    puVar1 = (uint *)(g_UiSevenSlotSelectionControlOffsets[uVar8] + 0x48 + iVar5);
    *puVar1 = *puVar1 | 8;
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
void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  UiNodeBase *pUVar3;
  SelectionPlayerRuntimeBlock *pSVar4;
  int iVar5;
  word *stream;
  TextResourceId resourceId;
  TextResourceId extraout_ECX;
  uint uVar6;
  undefined8 uVar7;
  
  pUVar2 = source->parent;
  pUVar3 = source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    pUVar3 = pUVar3->parent;
    pUVar2 = pUVar3->parent;
  }
  iVar5 = UiSelectableGroup_SelectExclusive(3,source);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(iVar5 + 0x1fa4));
  resourceId = 0x216d;
  uVar6 = 0;
  do {
    stream = TextResource_Resolve(resourceId);
    pSVar4 = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[uVar6].playerRuntimeId];
    puVar1 = (uint *)(iVar5 + g_UiSevenSlotSelectionControlOffsets[uVar6] + 0x48);
    *puVar1 = *puVar1 & 0xfffffff7;
    uVar6 = uVar6 + 1;
    RichTextCommandStream_PatchPayloadBySelector(0,pSVar4->reserved80B0_8117 + 0x40,stream);
    if (6 < uVar6) break;
    resourceId = extraout_ECX;
  } while (uVar6 < g_FrontendPlayerRuntimeBlockCount);
  *(uint *)(iVar5 + 0x20b8) = uVar6 * 0x18;
  uVar7 = UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)(iVar5 + 0x1ffc));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)((ulonglong)uVar7 >> 0x20));
  for (; uVar6 < 7; uVar6 = uVar6 + 1) {
    puVar1 = (uint *)(g_UiSevenSlotSelectionControlOffsets[uVar6] + 0x48 + iVar5);
    *puVar1 = *puVar1 | 8;
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
void InGameSelectionPage_ShowSubpage1(UiNodeBase *source)

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
  selectionGroupAddress = UiSelectableGroup_SelectExclusive(3,source);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)(selectionGroupAddress + 0x1fa4));
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
void InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView)

{
  int extraout_ECX;
  uint currentEntryCount;
  
  currentEntryCount = (historyView->recentTextPointerList).count;
  while (4 < currentEntryCount) {
    RecentTextHistory_RemoveOldest();
    currentEntryCount = extraout_ECX - 1;
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
                undefined4 param_4,ArmyRuntimeSlot *armyRuntimeUnderPointer,
                WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  SelectionPlayerRuntimeBlock *pSVar2;
  ArmyRuntimeSlot *modelRuntimeHolder;
  GameEntityRuntime *entityRuntime;
  dword extraout_EDX;
  dword extraout_EDX_00;
  dword extraout_EDX_01;
  bool bVar3;
  
  if ((armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) &&
     (armyRuntimeUnderPointer->runtimeStateA4 != 0)) {
    armyRuntimeUnderPointer = (ArmyRuntimeSlot *)0x0;
  }
                    
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
    dVar1 = g_UiCommandModeA;
    goto joined_r0x0056fa27;
  case 4:
    dVar1 = g_UiCommandModeB;
joined_r0x0056fa27:
    if (dVar1 == 0) {
      if (pointerRegionCode == 0x7fffffff) {
        return 0x18;
      }
      pSVar2 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      bVar3 = false;
      if (pSVar2->primarySelectionEntityOffset8094 == 0) {
        modelRuntimeHolder =
             ArmyRuntime_CreateInstanceFromAssetCf
                       (1,0,pointerWorldXQ12,pointerWorldYQ12,g_UiCommandModeGOwnerFactionIndex,
                        g_UiCommandModeGArmyAssetId,worldRuntime);
        if (!bVar3) {
          ArmyRuntimeNode_DispatchTypedCallback(&modelRuntimeHolder->definitionOrAsset,worldRuntime)
          ;
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
          return extraout_EDX_01;
        }
        return extraout_EDX_00;
      }
    }
    else {
      if (dVar1 == 1) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x19;
        }
        return 0x1a;
      }
      pSVar2 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      if (pSVar2->primarySelectionEntityOffset8094 == 0) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x15;
        }
        return 0x16;
      }
    }
    bVar3 = CARRY4(pSVar2->primarySelectionEntityOffset8094,(uint)g_ArmyRuntimeRebaseBaseMinusOne);
    ArmyRuntimeNode_DispatchTypedCallback
              ((ArmyRuntimeSlot **)
               (pSVar2->primarySelectionEntityOffset8094 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               worldRuntime);
    if (bVar3) {
      return extraout_EDX + 1;
    }
    return extraout_EDX;
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
void InGameUiCommand_BeginInteractionByMode
               (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,undefined4 param_4,
               ArmyRuntimeSlot *armyRuntimeUnderPointer,
               WorldRuntimeExtendedMapControlAddress32 mapControl)

{
  int iVar1;
  longlong lVar2;
  CommandPayloadDword04 CVar3;
  CommandPayloadDword08 extraout_EAX;
  CommandPayloadDword08 extraout_EAX_00;
  CommandPayloadDword08 payloadDword08;
  undefined4 extraout_EAX_01;
  undefined4 in_ECX;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  CommandPayloadDword04 extraout_ECX_07;
  CommandPayloadDword04 extraout_ECX_08;
  int extraout_ECX_09;
  int iVar4;
  undefined4 in_EDX;
  uint uVar5;
  CommandPayloadDword0C extraout_EDX;
  CommandPayloadDword0C extraout_EDX_00;
  CommandPayloadDword0C payloadDword0C;
  undefined4 extraout_EDX_01;
  uint uVar6;
  int iVar7;
  undefined8 uVar8;
  qword qVar9;
  
  uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
  if ((armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) &&
     (armyRuntimeUnderPointer->runtimeStateA4 != 0)) {
    armyRuntimeUnderPointer = (ArmyRuntimeSlot *)0x0;
  }
                    
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      *(uint *)(mapControl + 0x4c) = *(uint *)(mapControl + 0x4c) & 0xffefffff;
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      g_UiCommandDragStartScreenX = 0x7fffffff;
      if (pointerRegionCode != 0x7fffffff) {
        g_UiCommandDragStartScreenX = *(undefined4 *)(mapControl + 0x160);
        g_UiCommandDragStartScreenY = *(undefined4 *)(mapControl + 0x164);
        qVar9 = FieldGrid_GetNearestTerrainPoint
                          (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
        uVar6 = (int)((ulonglong)((longlong)extraout_ECX_00 * -0x20c8cc) >> 0x20) << 0xb |
                (uint)((longlong)extraout_ECX_00 * -0x20c8cc) >> 0x15;
        g_UiCommandDragAnchorWorldXQ12 =
             (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) + 0x3ff &
             0xfffff000;
        g_UiCommandDragAnchorWorldYQ12 = uVar6 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1f20,0,0,0);
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    if (g_UiCommandModeC == 1) {
      *(uint *)(mapControl + 0x4c) = *(uint *)(mapControl + 0x4c) & 0xffefffff;
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      g_UiCommandDragStartScreenX = 0x7fffffff;
      if (pointerRegionCode != 0x7fffffff) {
        g_UiCommandDragStartScreenX = *(undefined4 *)(mapControl + 0x160);
        g_UiCommandDragStartScreenY = *(undefined4 *)(mapControl + 0x164);
        qVar9 = FieldGrid_GetNearestTerrainPoint
                          (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
        uVar6 = (int)((ulonglong)((longlong)extraout_ECX_01 * -0x20c8cc) >> 0x20) << 0xb |
                (uint)((longlong)extraout_ECX_01 * -0x20c8cc) >> 0x15;
        g_UiCommandDragAnchorWorldXQ12 =
             (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) + 0x3ff &
             0xfffff000;
        g_UiCommandDragAnchorWorldYQ12 = uVar6 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ClearPlayerScratchPlane(g_LocalPlayerRuntimeId,0,0,0);
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1f20,0,0,0);
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    if (g_UiCommandModeC == 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ResetLocalInfluenceState(g_LocalPlayerRuntimeId,0,0,0);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2a80,0,0,0);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    break;
  case 1:
    if (g_UiCommandModeD != 3) {
      if (g_UiCommandModeD == 1) {
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        if (pointerRegionCode != 0x7fffffff) {
          qVar9 = FieldGrid_GetNearestTerrainPoint
                            (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
          lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
          uVar5 = (int)((ulonglong)((longlong)extraout_ECX_02 * -0x20c8cc) >> 0x20) << 0xb |
                  (uint)((longlong)extraout_ECX_02 * -0x20c8cc) >> 0x15;
          uVar6 = (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar5) + 0x3ff &
                  0xfffff000;
          uVar5 = uVar5 * 2 + 0x3ff & 0xfffff000;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            TerrainMaterialEdit_SeedMatchingRegionReplacement
                      (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar5,uVar6);
            uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand
                      (0x28e0,g_UiCommandAbsoluteSelectionIndex,uVar5,uVar6);
            uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
          }
        }
      }
      else if (g_UiCommandModeD == 2) {
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        if (pointerRegionCode != 0x7fffffff) {
          qVar9 = FieldGrid_GetNearestTerrainPoint
                            (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
          lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
          uVar5 = (int)((ulonglong)((longlong)extraout_ECX_03 * -0x20c8cc) >> 0x20) << 0xb |
                  (uint)((longlong)extraout_ECX_03 * -0x20c8cc) >> 0x15;
          uVar6 = (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar5) + 0x3ff &
                  0xfffff000;
          uVar5 = uVar5 * 2 + 0x3ff & 0xfffff000;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            TerrainMaterialEdit_SeedNonTargetRegionReplacement
                      (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar5,uVar6);
            uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand
                      (0x29b0,g_UiCommandAbsoluteSelectionIndex,uVar5,uVar6);
            uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
          }
        }
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
               SESSION_NETWORK_ROLE_LOCAL) {
        TerrainEditBuffer_CopyCellMaterialBytes(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2700,0,0,0);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      g_UiCommandDragStartScreenX = 0x7fffffff;
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      if (pointerRegionCode != 0x7fffffff) {
        g_UiCommandDragStartScreenX = *(undefined4 *)(mapControl + 0x160);
        g_UiCommandDragStartScreenY = *(undefined4 *)(mapControl + 0x164);
        qVar9 = FieldGrid_GetNearestTerrainPoint
                          (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
        uVar6 = (int)((ulonglong)((longlong)extraout_ECX_06 * -0x20c8cc) >> 0x20) << 0xb |
                (uint)((longlong)extraout_ECX_06 * -0x20c8cc) >> 0x15;
        g_UiCommandDragAnchorWorldXQ12 =
             (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) + 0x3ff &
             0xfffff000;
        g_UiCommandDragAnchorWorldYQ12 = uVar6 * 2 + 0x3ff & 0xfffff000;
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
    }
    else if (g_UiCommandModeE == 1) {
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      if (pointerRegionCode != 0x7fffffff) {
        qVar9 = FieldGrid_GetNearestTerrainPoint
                          (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
        uVar6 = (int)((ulonglong)((longlong)extraout_ECX_05 * -0x20c8cc) >> 0x20) << 0xb |
                (uint)((longlong)extraout_ECX_05 * -0x20c8cc) >> 0x15;
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        iVar1 = *(int *)(mapControl + 0x54);
        iVar4 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) +
                     0x3ff) >> 0xc;
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        if (((-1 < iVar4) &&
            (iVar7 = (int)(uVar6 * 2 + 0x3ff) >> 0xc,
            uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX), -1 < iVar7)) &&
           ((uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
            iVar4 < *(int *)(iVar1 + 0xb8) &&
            (uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
            iVar7 < *(int *)(iVar1 + 0xbc))))) {
          g_UiCommandTerrainMaskToggleValue =
               *(uint *)((iVar7 * *(int *)(iVar1 + 0xb8) + iVar4) * 0x80 + 0x250 + iVar1) &
               0x20000000 ^ 0x20000000;
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
      }
    }
    else {
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      if (pointerRegionCode != 0x7fffffff) {
        qVar9 = FieldGrid_GetNearestTerrainPoint
                          (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
        uVar6 = (int)((ulonglong)((longlong)extraout_ECX_04 * -0x20c8cc) >> 0x20) << 0xb |
                (uint)((longlong)extraout_ECX_04 * -0x20c8cc) >> 0x15;
        g_UiCommandTerrainMaskToggleValue = 0x40000000;
        iVar1 = *(int *)(mapControl + 0x54);
        iVar4 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) +
                     0x3ff) >> 0xc;
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        if ((((-1 < iVar4) &&
             (iVar7 = (int)(uVar6 * 2 + 0x3ff) >> 0xc,
             uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX), -1 < iVar7)) &&
            (uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
            iVar4 < *(int *)(iVar1 + 0xb8))) &&
           (uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
           iVar7 < *(int *)(iVar1 + 0xbc))) {
          g_UiCommandTerrainMaskToggleValue =
               *(uint *)((iVar7 * *(int *)(iVar1 + 0xb8) + iVar4) * 0x80 + 0x250 + iVar1) &
               0x40000000 ^ 0x40000000;
          uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
        }
      }
    }
    goto InGameUiCommand_FinalizeBeginInteractionState;
  case 3:
    CVar3 = g_UiCommandModeGOwnerFactionIndex;
    if (g_UiCommandModeA == 0) {
InGameUiCommand_ApplyModeGStateAtPointer:
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      if (pointerRegionCode != 0x7fffffff) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetState8090(g_LocalPlayerRuntimeId,0,0,CVar3);
          payloadDword08 = extraout_EAX_00;
          CVar3 = extraout_ECX_08;
          payloadDword0C = extraout_EDX_00;
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ec0,0,0,CVar3);
          payloadDword08 = extraout_EAX;
          CVar3 = extraout_ECX_07;
          payloadDword0C = extraout_EDX;
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          uVar8 = PlayerRuntime_ResolveAndStoreState8094
                            (g_LocalPlayerRuntimeId,payloadDword0C,payloadDword08,CVar3);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2e50,payloadDword0C,payloadDword08,CVar3);
          uVar8 = CONCAT44(extraout_EDX_01,extraout_EAX_01);
        }
        g_UiCommandDragStartScreenX = *(undefined4 *)(mapControl + 0x160);
        g_UiCommandDragStartScreenY = *(undefined4 *)(mapControl + 0x164);
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    if (g_UiCommandModeA != 1) goto InGameUiCommand_BeginSelectionOrPointerTargetAction;
InGameUiCommand_UseExistingPrimarySelection:
    uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    if (pointerRegionCode == 0x7fffffff) goto InGameUiCommand_FinalizeBeginInteractionState;
    if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
      CVar3 = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
              (int)g_ArmyRuntimeRebaseBaseMinusOne;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_ApplyEntryOrAll(g_LocalPlayerRuntimeId,0,0,CVar3);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2de0,0,0,CVar3);
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
    goto InGameUiCommand_ResolvePointerArmyAndCommandPayload;
  case 4:
    CVar3 = 0;
    if (g_UiCommandModeB == 0) goto InGameUiCommand_ApplyModeGStateAtPointer;
    if (g_UiCommandModeB == 1) goto InGameUiCommand_UseExistingPrimarySelection;
InGameUiCommand_BeginSelectionOrPointerTargetAction:
    uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    if (pointerRegionCode == 0x7fffffff) goto InGameUiCommand_FinalizeBeginInteractionState;
    if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
      g_UiCommandDragStartScreenX = *(undefined4 *)(mapControl + 0x160);
      g_UiCommandDragStartScreenY = *(undefined4 *)(mapControl + 0x164);
      CVar3 = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
              (int)g_ArmyRuntimeRebaseBaseMinusOne;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        PlayerRuntime_SetState8094(g_LocalPlayerRuntimeId,0,0,CVar3);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2ef0,0,0,CVar3);
      }
      uVar8 = CONCAT44(pointerX,pointerY);
      goto InGameUiCommand_FinalizeBeginInteractionState;
    }
InGameUiCommand_ResolvePointerArmyAndCommandPayload:
    *(uint *)(mapControl + 0x4c) = *(uint *)(mapControl + 0x4c) | 0x80;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    }
    goto InGameUiCommand_FinalizeBeginInteractionState;
  case 5:
    uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    if (pointerRegionCode != 0x7fffffff) {
      qVar9 = FieldGrid_GetNearestTerrainPoint
                        (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
      lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
      uVar6 = (int)((ulonglong)((longlong)extraout_ECX_09 * -0x20c8cc) >> 0x20) << 0xb |
              (uint)((longlong)extraout_ECX_09 * -0x20c8cc) >> 0x15;
      g_UiCommandCallerMaskHighBit = 0;
      iVar1 = *(int *)(mapControl + 0x54);
      iVar4 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6) + 0x3ff
                   ) >> 0xc;
      uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      if ((((-1 < iVar4) &&
           (iVar7 = (int)(uVar6 * 2 + 0x3ff) >> 0xc,
           uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX), -1 < iVar7)) &&
          (uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
          iVar4 < *(int *)(iVar1 + 0xb8))) &&
         ((uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
          iVar7 < *(int *)(iVar1 + 0xbc) &&
          (uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX),
          (*(uint *)((iVar7 * *(int *)(iVar1 + 0xb8) + iVar4) * 0x80 + 0x250 + iVar1) &
          0x800 << ((byte)g_UiCommandModeF & 0x1f)) != 0)))) {
        g_UiCommandCallerMaskHighBit = 0x80000000;
        uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
      }
    }
    goto InGameUiCommand_FinalizeBeginInteractionState;
  }
  g_UiCommandSelectionAnchorWorldXQ12 = 0x7fffffff;
  if (pointerRegionCode != 0x7fffffff) {
    if ((g_KeyboardStateMask & 0xf) == 0) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        SelectionPlayerRuntime_ClearTerrainEditSelectionState(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1ed0,0,0,0);
      }
    }
    qVar9 = FieldGrid_GetNearestTerrainPoint
                      (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
    uVar8 = CONCAT44(g_UiCommandDragReferenceY,g_UiCommandDragReferenceX);
    lVar2 = (longlong)(int)qVar9 * 0x1c6e9c;
    uVar6 = (int)((ulonglong)((longlong)extraout_ECX * -0x20c8cc) >> 0x20) << 0xb |
            (uint)((longlong)extraout_ECX * -0x20c8cc) >> 0x15;
    g_UiCommandSelectionAnchorWorldXQ12 =
         ((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar6;
    g_UiCommandSelectionAnchorWorldYQ12 = uVar6 * 2;
    g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    g_UiCommandSelectionCurrentWorldYQ12 = g_UiCommandSelectionAnchorWorldYQ12;
  }
InGameUiCommand_FinalizeBeginInteractionState:
  g_UiCommandDragReferenceY = (sdword)((ulonglong)uVar8 >> 0x20);
  g_UiCommandDragReferenceX = (sdword)uVar8;
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
undefined8 __fastcall
InGameUiCommand_UpdateInteractionByMode
          (undefined4 param_1,undefined4 param_2,UiPointerRegionCode pointerRegionCode,
          GraphicsScreenCoordinate pointerX,GraphicsScreenCoordinate pointerY,undefined4 param_6,
          int param_7,WorldRuntimeExtendedMapControlAddress32 mapControl)

{
  longlong lVar1;
  dword dVar2;
  undefined4 in_EAX;
  int iVar3;
  undefined4 extraout_EAX;
  undefined4 extraout_EAX_00;
  CommandPayloadDword04 CVar4;
  int extraout_ECX;
  int iVar5;
  CommandPayloadDword0C CVar6;
  CommandPayloadDword0C extraout_ECX_00;
  CommandPayloadDword0C extraout_ECX_01;
  CommandPayloadDword0C extraout_ECX_02;
  CommandPayloadDword0C extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  undefined4 extraout_ECX_08;
  int iVar7;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int extraout_ECX_14;
  int extraout_ECX_15;
  int extraout_ECX_16;
  int extraout_ECX_17;
  uint uVar8;
  int iVar9;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  CommandPayloadDword08 CVar10;
  void *extraout_EDX_01;
  void *extraout_EDX_02;
  void *extraout_EDX_03;
  void *extraout_EDX_04;
  void *extraout_EDX_05;
  void *extraout_EDX_06;
  void *pvVar11;
  uint uVar12;
  int iVar13;
  InGameCommandPayloadTripletValue32 payloadValue;
  undefined4 *puVar14;
  WorldRuntimeNode *runtimeNode;
  CommandPayloadDword04 *pCVar15;
  bool bVar16;
  undefined8 uVar17;
  qword qVar18;
  GameEntityRuntime *entry;
  
  if ((*(uint *)(mapControl + 0x4c) & 0x80) != 0) {
    puVar14 = (undefined4 *)&g_InGameSelectionInsertTripletDwords;
    for (iVar7 = 0x1a; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar14 = 0;
      puVar14 = puVar14 + 1;
    }
    runtimeNode = *(WorldRuntimeNode **)(mapControl + 0xd8);
    iVar7 = *(int *)(mapControl + 0x50);
    if (runtimeNode != (WorldRuntimeNode *)0x0) {
      do {
        pvVar11 = runtimeNode->runtimePayload;
        if ((((runtimeNode->runtimeFlags & 2) != 0) &&
            (entry = *(GameEntityRuntime **)((int)pvVar11 + 8),
            (runtimeNode->runtimeFlags & 0x20) != 0)) &&
           (iVar7 == (entry->common).ownership.ownerIndex)) {
          bVar16 = entry < g_ArmyRuntimeRebaseBaseMinusOne;
          payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
          WorldRuntimeNode_IsPositionInsideBoundsCf(runtimeNode,mapControl);
          if (bVar16) {
            SelectionInfo_FindEntryCf(entry);
            iVar7 = extraout_ECX_09;
            pvVar11 = extraout_EDX_01;
            if (bVar16) {
              uVar17 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90);
              pvVar11 = (void *)((ulonglong)uVar17 >> 0x20);
              iVar7 = extraout_ECX_10;
              if ((!bVar16) &&
                 (*(InGameCommandPayloadTripletValue32 *)
                   (&g_InGameSelectionInsertTripletDwords + (uint)uVar17 * 4) = payloadValue,
                 (uint)uVar17 < 0xb)) {
                g_InGameSelectionInsertTripletDwordCount =
                     g_InGameSelectionInsertTripletDwordCount + 1;
              }
            }
          }
          else {
            SelectionInfo_FindEntryCf(entry);
            iVar7 = extraout_ECX_11;
            pvVar11 = extraout_EDX_02;
            if (!bVar16) {
              uVar17 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30);
              pvVar11 = (void *)((ulonglong)uVar17 >> 0x20);
              iVar7 = extraout_ECX_12;
              if ((!bVar16) &&
                 (*(InGameCommandPayloadTripletValue32 *)
                   (&g_InGameSelectionRemoveTripletDwords + (uint)uVar17 * 4) = payloadValue,
                 (uint)uVar17 < 0xb)) {
                g_InGameSelectionRemoveTripletDwordCount =
                     g_InGameSelectionRemoveTripletDwordCount + 1;
              }
            }
          }
        }
        runtimeNode = (runtimeNode->common).nextNode;
      } while (runtimeNode != (WorldRuntimeNode *)0x0);
      if (g_InGameSelectionRemoveTripletDwordCount != 0) {
        pCVar15 = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
        do {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                      (g_LocalPlayerRuntimeId,pCVar15[2],pCVar15[1],*pCVar15);
            iVar7 = extraout_ECX_14;
            pvVar11 = extraout_EDX_04;
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0xb00,pCVar15[2],pCVar15[1],*pCVar15);
            iVar7 = extraout_ECX_13;
            pvVar11 = extraout_EDX_03;
          }
          iVar3 = g_InGameSelectionRemoveTripletDwordCount;
          pCVar15 = pCVar15 + 3;
          g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount + -3;
        } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < iVar3);
      }
      if (g_InGameSelectionInsertTripletDwordCount != 0) {
        pCVar15 = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
        do {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                      (iVar7,pvVar11,g_LocalPlayerRuntimeId,pCVar15[2],pCVar15[1],*pCVar15);
            iVar7 = extraout_ECX_16;
            pvVar11 = extraout_EDX_06;
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0xa60,pCVar15[2],pCVar15[1],*pCVar15);
            iVar7 = extraout_ECX_15;
            pvVar11 = extraout_EDX_05;
          }
          iVar3 = g_InGameSelectionInsertTripletDwordCount;
          pCVar15 = pCVar15 + 3;
          g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount + -3;
        } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < iVar3);
      }
    }
    goto InGameUiCommand_FinalizeInteractionUpdate;
  }
                    
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      if (g_UiCommandDragStartScreenX != 0x7fffffff) {
        uVar8 = 0xffff;
        if ((g_KeyboardStateMask & 0xf) != 0) {
          uVar8 = 0;
        }
        uVar8 = *(int *)(mapControl + 0x168) - g_UiCommandDragStartScreenX & uVar8 |
                (*(int *)(mapControl + 0x16c) - g_UiCommandDragStartScreenY) * 0x10000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ApplyPositiveCellDeltas
                    (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                     g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x1f70,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
      }
      goto InGameUiCommand_FinalizeInteractionUpdate;
    }
    if (g_UiCommandModeC == 1) {
      if (g_UiCommandDragStartScreenX != 0x7fffffff) {
        uVar8 = 0xffff;
        if ((g_KeyboardStateMask & 0xf) != 0) {
          uVar8 = 0;
        }
        uVar8 = *(int *)(mapControl + 0x168) - g_UiCommandDragStartScreenX & uVar8 |
                (*(int *)(mapControl + 0x16c) - g_UiCommandDragStartScreenY) * 0x10000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ApplyNegativeCellDeltas
                    (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                     g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x2290,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
      }
      goto InGameUiCommand_FinalizeInteractionUpdate;
    }
    if (g_UiCommandModeC == 2) {
      if (pointerRegionCode != 0x7fffffff) {
        qVar18 = FieldGrid_GetNearestTerrainPoint
                           (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
        uVar12 = (int)((ulonglong)((longlong)extraout_ECX_04 * -0x20c8cc) >> 0x20) << 0xb |
                 (uint)((longlong)extraout_ECX_04 * -0x20c8cc) >> 0x15;
        uVar8 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar12) + 0x3ff &
                0xfffff000;
        uVar12 = uVar12 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_RebuildLocalInfluenceState(g_LocalPlayerRuntimeId,0,uVar12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ae0,0,uVar12,uVar8);
        }
      }
      goto InGameUiCommand_FinalizeInteractionUpdate;
    }
    break;
  case 1:
    if (g_UiCommandModeD != 3) {
      if (((g_TerrainMaterialTextureSets[g_UiCommandAbsoluteSelectionIndex] !=
            (GraphicsTextureSet *)0x0) && (g_UiCommandModeD != 1)) &&
         ((g_UiCommandModeD != 2 && (pointerRegionCode != 0x7fffffff)))) {
        qVar18 = FieldGrid_GetNearestTerrainPoint
                           (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
        uVar12 = (int)((ulonglong)((longlong)extraout_ECX_05 * -0x20c8cc) >> 0x20) << 0xb |
                 (uint)((longlong)extraout_ECX_05 * -0x20c8cc) >> 0x15;
        uVar8 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar12) + 0x3ff &
                0xfffff000;
        uVar12 = uVar12 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ApplyLocalCellUpdate
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x2770,g_UiCommandAbsoluteSelectionIndex,uVar12,uVar8);
        }
      }
      goto InGameUiCommand_FinalizeInteractionUpdate;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      if (g_UiCommandDragStartScreenX != 0x7fffffff) {
        uVar8 = 0xffff;
        if ((g_KeyboardStateMask & 0xf) != 0) {
          uVar8 = 0;
        }
        uVar12 = *(int *)(mapControl + 0x168) - g_UiCommandDragStartScreenX;
        iVar7 = *(int *)(mapControl + 0x16c) - g_UiCommandDragStartScreenY;
        g_UiCommandDragStartScreenX = g_UiCommandDragStartScreenX + uVar12;
        g_UiCommandDragStartScreenY = g_UiCommandDragStartScreenY + iVar7;
        uVar8 = uVar12 & uVar8 | iVar7 * 0x10000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ApplyEncodedCellUpdate
                    (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                     g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x3260,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar8);
        }
      }
    }
    else if (g_UiCommandModeE == 1) {
      if (pointerRegionCode != 0x7fffffff) {
        qVar18 = FieldGrid_GetNearestTerrainPoint
                           (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
        lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
        uVar12 = (int)((ulonglong)((longlong)extraout_ECX_07 * -0x20c8cc) >> 0x20) << 0xb |
                 (uint)((longlong)extraout_ECX_07 * -0x20c8cc) >> 0x15;
        uVar8 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar12) + 0x3ff &
                0xfffff000;
        uVar12 = uVar12 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FieldGrid_ApplyMaskDFFFFFFF
                    (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,uVar12,uVar8);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x32a0,g_UiCommandTerrainMaskToggleValue,uVar12,uVar8);
        }
      }
    }
    else if (pointerRegionCode != 0x7fffffff) {
      qVar18 = FieldGrid_GetNearestTerrainPoint
                         (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
      lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
      uVar12 = (int)((ulonglong)((longlong)extraout_ECX_06 * -0x20c8cc) >> 0x20) << 0xb |
               (uint)((longlong)extraout_ECX_06 * -0x20c8cc) >> 0x15;
      uVar8 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar12) + 0x3ff &
              0xfffff000;
      uVar12 = uVar12 * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ApplyMaskBFFFFFFF
                  (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,uVar12,uVar8);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x32e0,g_UiCommandTerrainMaskToggleValue,uVar12,uVar8);
      }
    }
    goto InGameUiCommand_FinalizeInteractionUpdate;
  case 3:
    dVar2 = g_UiCommandModeA;
    goto joined_r0x00570b27;
  case 4:
    dVar2 = g_UiCommandModeB;
joined_r0x00570b27:
    if (((dVar2 == 0) || (dVar2 != 1)) && (pointerRegionCode != 0x7fffffff)) {
      if ((g_CursorButtonState & 4) == 0) {
        uVar17 = (*g_PointerSetPosition)(g_UiCommandDragStartScreenY,g_UiCommandDragStartScreenX);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          SelectionPlayerRuntime_AdvancePrimarySelectionCycle
                    (extraout_ECX_08,(int)((ulonglong)uVar17 >> 0x20),g_LocalPlayerRuntimeId);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x30f0,0,0,(int)uVar17 << 6);
        }
      }
      else {
        g_UiCommandDragStartScreenX = *(int *)(mapControl + 0x168);
        g_UiCommandDragStartScreenY = *(int *)(mapControl + 0x16c);
        CVar4 = pointerY - g_UiCommandDragReferenceX;
        CVar10 = pointerX - g_UiCommandDragReferenceY;
        g_UiCommandDragReferenceX = g_UiCommandDragReferenceX + CVar4;
        g_UiCommandDragReferenceY = g_UiCommandDragReferenceY + CVar10;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          SelectionPlayerRuntime_ReissuePrimarySelectionPosition
                    (param_1,CVar10,g_LocalPlayerRuntimeId);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2f20,0,CVar10,CVar4);
        }
      }
    }
    goto InGameUiCommand_FinalizeInteractionUpdate;
  case 5:
    if (pointerRegionCode != 0x7fffffff) {
      qVar18 = FieldGrid_GetNearestTerrainPoint
                         (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
      lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
      uVar12 = (int)((ulonglong)((longlong)extraout_ECX_17 * -0x20c8cc) >> 0x20) << 0xb |
               (uint)((longlong)extraout_ECX_17 * -0x20c8cc) >> 0x15;
      uVar8 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar12) + 0x3ff &
              0xfffff000;
      uVar12 = uVar12 * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FieldGrid_ApplyCallerMask
                  (g_LocalPlayerRuntimeId,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,uVar12,
                   uVar8);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x3320,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,uVar12,uVar8);
      }
    }
    goto InGameUiCommand_FinalizeInteractionUpdate;
  }
  if ((pointerRegionCode != 0x7fffffff) && (g_UiCommandSelectionAnchorWorldXQ12 != 0x7fffffff)) {
    qVar18 = FieldGrid_GetNearestTerrainPoint
                       (pointerX,pointerY,*(FieldGridAsset **)(mapControl + 0x54));
    lVar1 = (longlong)(int)qVar18 * 0x1c6e9c;
    uVar8 = (int)((ulonglong)((longlong)extraout_ECX * -0x20c8cc) >> 0x20) << 0xb |
            (uint)((longlong)extraout_ECX * -0x20c8cc) >> 0x15;
    iVar5 = ((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar8;
    iVar7 = uVar8 * 2;
    LOCK();
    UNLOCK();
    LOCK();
    UNLOCK();
    iVar3 = g_UiCommandSelectionAnchorWorldXQ12;
    if (g_UiCommandSelectionCurrentWorldXQ12 < g_UiCommandSelectionAnchorWorldXQ12) {
      iVar3 = g_UiCommandSelectionCurrentWorldXQ12;
      g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    }
    iVar9 = g_UiCommandSelectionCurrentWorldYQ12;
    iVar13 = g_UiCommandSelectionAnchorWorldYQ12;
    if (g_UiCommandSelectionCurrentWorldYQ12 < g_UiCommandSelectionAnchorWorldYQ12) {
      iVar9 = g_UiCommandSelectionAnchorWorldYQ12;
      iVar13 = g_UiCommandSelectionCurrentWorldYQ12;
    }
    CVar4 = iVar3 + 0x3ffU & 0xfffff000;
    CVar10 = iVar13 + 0x3ffU & 0xfffff000;
    CVar6 = g_UiCommandSelectionCurrentWorldXQ12 + 0x3ffU & 0xfffff000;
    g_UiCommandSelectionCurrentWorldXQ12 = iVar5;
    g_UiCommandSelectionCurrentWorldYQ12 = iVar7;
    if (((int)CVar4 <= (int)CVar6) && ((int)CVar10 <= (int)(iVar9 + 0x3ffU & 0xfffff000))) {
      do {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          uVar17 = PlayerPairList_RemoveRange(g_LocalPlayerRuntimeId,CVar6,CVar10,CVar4);
          CVar6 = extraout_ECX_01;
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d40,CVar6,CVar10,CVar4);
          uVar17 = CONCAT44(extraout_EDX,extraout_EAX);
          CVar6 = extraout_ECX_00;
        }
        CVar4 = (CommandPayloadDword04)uVar17;
        CVar10 = CVar10 + 0x1000;
      } while ((int)CVar10 <= (int)((ulonglong)uVar17 >> 0x20));
    }
    iVar7 = g_UiCommandSelectionAnchorWorldXQ12;
    iVar3 = g_UiCommandSelectionCurrentWorldXQ12;
    if (g_UiCommandSelectionCurrentWorldXQ12 < g_UiCommandSelectionAnchorWorldXQ12) {
      iVar7 = g_UiCommandSelectionCurrentWorldXQ12;
      iVar3 = g_UiCommandSelectionAnchorWorldXQ12;
    }
    iVar5 = g_UiCommandSelectionCurrentWorldYQ12;
    iVar9 = g_UiCommandSelectionAnchorWorldYQ12;
    if (g_UiCommandSelectionCurrentWorldYQ12 < g_UiCommandSelectionAnchorWorldYQ12) {
      iVar5 = g_UiCommandSelectionAnchorWorldYQ12;
      iVar9 = g_UiCommandSelectionCurrentWorldYQ12;
    }
    CVar4 = iVar7 + 0x3ffU & 0xfffff000;
    CVar10 = iVar9 + 0x3ffU & 0xfffff000;
    CVar6 = iVar3 + 0x3ffU & 0xfffff000;
    if (((int)CVar4 <= (int)CVar6) && ((int)CVar10 <= (int)(iVar5 + 0x3ffU & 0xfffff000))) {
      do {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          uVar17 = PlayerPairList_InsertRange(g_LocalPlayerRuntimeId,CVar6,CVar10,CVar4);
          CVar6 = extraout_ECX_03;
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d00,CVar6,CVar10,CVar4);
          uVar17 = CONCAT44(extraout_EDX_00,extraout_EAX_00);
          CVar6 = extraout_ECX_02;
        }
        CVar4 = (CommandPayloadDword04)uVar17;
        CVar10 = CVar10 + 0x1000;
      } while ((int)CVar10 <= (int)((ulonglong)uVar17 >> 0x20));
    }
  }
InGameUiCommand_FinalizeInteractionUpdate:
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x00570D60.
   Ownership: ui/ingame/runtime.
   Purpose: Handles in game ui command end interaction by mode.
   Cross-module calls: TerrainEditBuffer_ConvertHeightsToDeltas [world/terrain/editing],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   TerrainEditBuffer_SubtractCurrentCellMaterialBytes [world/terrain/editing], PlayerRuntime_ClearState8094
   [gameplay/faction/runtime].
*/
void InGameUiCommand_EndInteractionByMode
               (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3,
               WorldRuntimeNode *worldNode,WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  undefined4 in_ECX;
  undefined4 in_EDX;
  
  dVar1 = g_UiCommandModeG;
  worldRuntime->runtimeFlags = worldRuntime->runtimeFlags & 0xffffff7f;
                    
  switch(dVar1) {
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
        TerrainEditBuffer_ConvertHeightsToDeltas(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
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
        TerrainEditBuffer_SubtractCurrentCellMaterialBytes(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2800,0,0,0);
      }
    }
    break;
  case 3:
    dVar1 = g_UiCommandModeA;
    goto joined_r0x00570ef7;
  case 4:
    dVar1 = g_UiCommandModeB;
joined_r0x00570ef7:
    if ((dVar1 == 0) || (dVar1 != 1)) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        PlayerRuntime_ClearState8094(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
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
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime)

{
  undefined4 in_ECX;
  undefined4 in_EDX;

  switch(g_UiCommandModeG) {
  case 0:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1ed0,0,0,0);
    }
    break;
  case 1:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      SelectionPlayerRuntime_ClearTerrainEditSelectionState(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
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
void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
               (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  WorldRuntimeFlags *pWVar1;
  GraphicsTextureSet *pGVar2;
  byte *pbVar3;
  InGameRuntimeRootImageC3E4 *root;
  int extraout_EAX;
  dword dVar4;
  int extraout_EAX_00;
  int extraout_EAX_01;
  int iVar5;
  GraphicsTextureSourceAsset *pGVar6;
  int extraout_ECX;
  undefined4 extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  UiNodeBase *node_04;
  int extraout_EDX_03;
  InGameNotificationQueueRecord20 *pIVar7;
  TerrainDirectionRecord *pTVar8;
  FieldGridCell *gridCell1;
  ArmyAssetRecordPrefix **ppAVar9;
  FieldGridAsset *fieldGrid1;
  
  dVar4 = g_UiCommandModeG;
  root = g_InGameRuntimeRoot;
  if ((commandArg3 & 4) == 0) {
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      pbVar3 = g_InGameRuntimeRoot->opaque9A74_9B4B;
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 5;
      pWVar1 = &(g_InGameRuntimeRoot->worldRuntime0A30).runtimeFlags;
      *pWVar1 = *pWVar1 | 0x400000;
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGPrimaryPageIndices[dVar4],(UiPageStackControl *)(pbVar3 + 0x18));
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGSecondaryPageIndices[extraout_EDX],
                 (UiPageStackControl *)root->opaque9EE0_9FAB);
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGTertiaryPageIndices[extraout_EDX_00],
                 (UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
      UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
      if (extraout_EAX == 0) {
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
      (*g_UiCommandModeGHandlers[extraout_EDX_01])
                (root->opaque0058_017B + g_UiCommandModeGControlOffsets[extraout_EDX_01] + -0x58);
      pIVar7 = root->notificationQueue9E60;
      for (iVar5 = 0x20; iVar5 != 0; iVar5 = iVar5 + -1) {
        pIVar7->notificationMovieId00 = 0;
        pIVar7 = (InGameNotificationQueueRecord20 *)&pIVar7->priority04;
      }
      Movie_Close();
      dVar4 = g_UiCommandAbsoluteSelectionIndex;
      pGVar6 = g_InGamePanelTextureSource;
      if (root->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE) {
        root->sessionNotificationInteractionState9B4C = NONE;
      }
      pGVar2 = g_TerrainMaterialTextureSets[dVar4];
      root->observedSessionNotificationValue9B50 = (dword)pGVar6;
      pGVar6 = (GraphicsTextureSourceAsset *)0x0;
      if (pGVar2 != (GraphicsTextureSet *)0x0) {
        pGVar6 = pGVar2->entries[0].sourceAsset;
      }
      root->notificationPlaybackCompletionCode9B54 = 0x25;
      *(GraphicsTextureSourceAsset **)(root->opaque9B58_9E3F + 0xb8) = pGVar6;
      UiCommandMatrix_SelectIndex(g_UiCommandAbsoluteSelectionIndex,(UiNodeBase *)root);
      g_UiCommandModeGArmyAssetId =
           ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
      dVar4 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x1cc) = dVar4;
      g_UiCommandMode4ArmyAssetId =
           ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
      dVar4 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x284) = dVar4;
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
      iVar5 = 0x100;
      pTVar8 = g_TerrainDirectionRecordTable256;
      do {
        pTVar8->angleAComponent0ScaledQ28 = 0;
        pTVar8->angleAComponent1ScaledQ28 = 0;
        pTVar8->angleBComponent0ScaledQ28 = 0;
        pTVar8 = pTVar8 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      fieldGrid1 = (root->worldRuntime0A30).fieldGrid;
      iVar5 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
      gridCell1 = fieldGrid1->cells;
      do {
        gridCell1->runtime58_6F[0x14] = 0;
        gridCell1->runtime58_6F[0x15] = 0;
        gridCell1->runtime58_6F[0x16] = 0;
        gridCell1->runtime58_6F[0x17] = 0;
        gridCell1->runtime7C = 0;
        gridCell1 = gridCell1 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
    }
  }
  else if ((g_UiCommandRuntimeFlags & 4) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffffffa;
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x18));
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)root->opaque9EE0_9FAB);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
    UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
    if (extraout_EAX_00 == 0) {
      UiPageStack_SetActiveIndex(0,&root->optionalUiPageStack4530);
      UiPageStack_SetActiveIndex(0,&root->optionalUiPageStack4644);
    }
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(root->opaque4100_452F + 0x22c));
    UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)&root->worldRuntime0A30);
    *(code **)(extraout_EAX_01 + 0x15c) = InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
    *(code **)(extraout_EAX_01 + 0x100) = InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
    *(code **)(extraout_EAX_01 + 0x104) = InGameWorldInput_ResolveContextActionAndCursorCf;
    *(code **)(extraout_EAX_01 + 0x108) = InGameWorldInput_ResolveContextActionAndCursorCf;
    *(code **)(extraout_EAX_01 + 0x10c) = InGameWorldInput_BeginPointerCaptureCf;
    *(code **)(extraout_EAX_01 + 0x110) = InGameWorldInput_UpdateDragSelectionAndCameraCf;
    *(code **)(extraout_EAX_01 + 0x114) = InGameWorldInput_CommitPointerActionCf;
    *(code **)(extraout_EAX_01 + 0xb0) = InGameUiRuntime_ClearTransientState1BCallback;
    *(code **)(extraout_EAX_01 + 0x118) = InGameUiRuntime_DispatchWorldContextActionCallback;
    *(uint *)(extraout_EAX_01 + 0x4c) = *(uint *)(extraout_EAX_01 + 0x4c) | 0x400;
    g_UiRootCallbacks_0054FBC0.keyboardFallbackCf = EndGameResultsUiRuntime_DispatchCommandByFlagsCf
    ;
    ppAVar9 = g_ArmyAssetRecordRegistry;
    iVar5 = 0x300;
    do {
      if (*ppAVar9 != (ArmyAssetRecordPrefix *)0x0) {
        (*g_MemoryApi.free)((void *)(*ppAVar9)[2].byteSize);
        *(undefined4 *)(extraout_EDX_02 + 0x20) = 0;
        iVar5 = extraout_ECX;
      }
      ppAVar9 = ppAVar9 + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    if ((g_UiCommandRuntimeFlags & 8) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells
                ((root->worldRuntime0A30).activeFactionRuntimeIndex,
                 (root->worldRuntime0A30).fieldGrid);
    }
    WorldRuntime_ForEachNodeInOwnerListD8
              (&root->worldRuntime0A30,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
               ,&root->worldRuntime0A30);
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((root->worldRuntime0A30).activeFactionRuntimeIndex,(root->worldRuntime0A30).fieldGrid
              );
    UiCommandModeG_ClearNodeFlag00100000(node);
    UiCommandModeG_ClearNodeFlag00200000(node_00);
    UiCommandModeG_SetNodeFlag00000400(node_01);
    UiCommandModeG_ClearNodeFlag01000000(node_02);
    UiCommandModeG_ApplyRawColorVariant(visualState);
    UiCommandModeG_ClearNodeFlag00800000(node_03);
    UiCommandModeG_ClearNodeFlag02000000(node_04);
    *(uint *)(extraout_EDX_03 + 0x4c) = *(uint *)(extraout_EDX_03 + 0x4c) & 0xffbfffff;
    TerrainDirectionTable_AdvanceAndRebuildVectors(extraout_ECX_00,extraout_EDX_03);
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
void InGameUiCommand_SaveFieldAndLevelAssetImages
               (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  undefined1 in_CF;
  bool bVar1;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  gameRuntime1 = g_InGameRuntimeRoot;
  FieldGrid_SaveAssetImageFromRuntimeStateCf
            ((dword)(g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid);
  if ((bool)in_CF) {
    (*g_FatalErrorRuntimeDispatchCf)();
  }
  bVar1 = (InGameRuntimeRootImageC3E4 *)0xfffff5cf < gameRuntime1;
  InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf((int)&gameRuntime1->worldRuntime0A30);
  if (bVar1) {
    (*g_FatalErrorRuntimeDispatchCf)();
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
word * __fastcall InGameRecentTextHistory_InsertAndRebuild8(void)

{
  word *in_EAX;
  RecentTextHistoryPointerList *output;
  
  output = &g_InGameRuntimeRoot->recentTextHistory09B8;
  RecentTextHistory_Insert(in_EAX);
  RecentTextHistory_SortAndBuildPointerList(8,output);
  return in_EAX;
}

/* Address: 0x0056B850.
   Ownership: ui/ingame/runtime.
   Purpose: Finds the UI root, clears suppression on the command-page container at root+0xA78, and selects page 0
   in the page stack at root+0xBD0. Queued UI action handler for INGAME_PAGE10[2] (0x1002). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source)

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
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source)

{
  sdword *psVar1;
  UiNodeBase *pUVar2;
  int iVar3;
  uint uVar4;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_EDX;
  uint packedState;
  uint extraout_EDX_00;
  uint uVar5;
  UiAnchorFractionQ31 *pUVar6;
  bool bVar7;
  
  pUVar2 = source->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    pUVar2 = source->parent;
  }
  psVar1 = &source[0x60].right;
  RichTextCommandStream_CopyToNarrowCf
            (0x30,g_UiSevenSlotCommandPayloadText.textBytes,(word *)&source[0x61].rightAnchorQ31);
  iVar3 = UiSelectableGroup_NoneVisibleSelectedCf(3);
  if (iVar3 - (int)psVar1 == 0x1ec) {
    uVar4 = 0;
    uVar5 = 0x100;
    do {
      bVar7 = CARRY4(uVar5,uVar5);
      uVar5 = uVar5 * 2;
      UiSelectableControl_IsSelectedCf
                ((UiSelectableControl *)
                 (g_UiSevenSlotSelectionControlOffsets[uVar4] + -0x1c98 + (int)psVar1));
      packedState = extraout_EDX;
      if (bVar7) {
        packedState = extraout_EDX | uVar5;
      }
      uVar4 = extraout_ECX;
    } while (extraout_ECX < 7);
  }
  else if (iVar3 - (int)psVar1 == 0x24c) {
    uVar4 = 0;
    uVar5 = 0x8000;
    do {
      bVar7 = CARRY4(uVar5,uVar5);
      uVar5 = uVar5 * 2;
      UiSelectableControl_IsSelectedCf
                ((UiSelectableControl *)
                 (g_UiSevenSlotSelectionControlOffsets[uVar4] + -0x1c98 + (int)psVar1));
      packedState = extraout_EDX_00;
      if (bVar7) {
        packedState = extraout_EDX_00 | uVar5;
      }
      uVar4 = extraout_ECX_00;
    } while (extraout_ECX_00 < 7);
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
  pUVar6 = &source[0x61].rightAnchorQ31;
  for (iVar3 = 0x18; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pUVar6 = 0;
    pUVar6 = pUVar6 + 1;
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
void __cdecl InGameSelectionDetailPanel_Rebuild(void)

{
  UiPageStackControl *stack;
  byte *pbVar1;
  uint uVar2;
  undefined4 uVar3;
  dword dVar4;
  void *pvVar5;
  int iVar6;
  ArmyAssetRecordPrefix *armyDefinition1;
  int extraout_EAX;
  int extraout_EAX_00;
  uint uVar7;
  word *pwVar8;
  ModelDefinitionRecordPrefix *modelDefinition1;
  int iVar9;
  int extraout_ECX;
  sdword arg4;
  int extraout_ECX_00;
  word *source;
  word *source_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  UiPageStackControl *stack_00;
  word *source_01;
  word *source_02;
  int extraout_ECX_04;
  int extraout_EDX;
  int definitionNode;
  ModelDefinitionHierarchyNodeAddress32 definitionNode_00;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int extraout_EDX_03;
  int extraout_EDX_04;
  int extraout_EDX_05;
  int extraout_EDX_06;
  InGameRuntimeRootImageC3E4 *firstNode;
  GameEntityRuntime *entityRuntime1;
  int *piVar10;
  GameEntityRuntime **ppGVar11;
  word *pwVar12;
  undefined1 uVar13;
  ArmyUnlockedArmourAggregateEaxPreservedEdxCarrier64 AVar14;
  EnergyQ4AggregateEaxPreservedEdxCarrier64 EVar15;
  
  firstNode = g_InGameRuntimeRoot;
  if (g_InGameRuntimeRoot == (InGameRuntimeRootImageC3E4 *)0x0) {
    return;
  }
  iVar9 = 0x20;
  uVar7 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  iVar6 = 0;
  entityRuntime1 = (GameEntityRuntime *)0x0;
  ppGVar11 = g_SelectionInfoEntitySlots->entries;
  do {
    if (*ppGVar11 != (GameEntityRuntime *)0x0) {
      iVar6 = iVar6 + 1;
      entityRuntime1 = *ppGVar11;
    }
    ppGVar11 = ppGVar11 + 1;
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  stack = &g_InGameRuntimeRoot->selectionDetailPageStack9FAC;
  if (g_UiHoverSelectionRecord == (UiCommandRuntimeRecordPrefix *)0x0) {
    if (iVar6 == 1) {
      uVar2 = (entityRuntime1->common).ownership.ownerIndex;
      uVar13 = uVar7 < uVar2;
      if (uVar7 == uVar2) {
        FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf
                  ((RuntimeToken)(entityRuntime1->common).ownership.definitionOrClassRecord,
                   (g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId);
        if (!(bool)uVar13) {
          UiNodeList_UnsuppressActionId(0x1010,(UiNodeBase *)firstNode);
          iVar9 = 0x1c;
          iVar6 = extraout_EDX_04;
          do {
            Technology_IsAvailableForFactionCf
                      (*(PckTechnologyIdCatalog *)(iVar6 + 0x1c4 + iVar9 * 4),
                       (entityRuntime1->common).ownership.ownerIndex);
            if ((bool)uVar13)
            goto InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails;
            iVar9 = extraout_EAX + -1;
            uVar13 = 0;
            iVar6 = extraout_EDX_05;
          } while (iVar9 != 0);
        }
        UiNodeList_SuppressActionId(0x1010,(UiNodeBase *)firstNode);
InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails:
        ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
        (*g_FatalErrorPrimaryDispatchCf)();
        UiPageStack_SetActiveIndex(1,stack_00);
        dVar4 = *(dword *)(extraout_EAX_00 + 0x1c);
        iVar6 = ModelRuntimeHierarchy_SumMetric3C((int *)entityRuntime1);
        firstNode->selectionDetailArmyAssetValueA060 = dVar4;
        firstNode->selectionDetailEntityA068 = entityRuntime1;
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar6,g_InGameSelectionDetailArmourTextUtf16)
        ;
        uVar7 = ModelRuntime_QueryActiveHierarchyMetric((ArmyRuntimeSlot *)entityRuntime1);
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar7 >> 4,
                   g_InGameSelectionDetailEnergyTextUtf16);
        *(int *)(firstNode->opaqueA06C_C3E3 + 0x54) = *(int *)(extraout_EDX_06 + 4) + 0x18002c;
        pwVar8 = TextResource_Resolve
                           (*(int *)(*(int *)(entityRuntime1->common).ownership.
                                             definitionOrClassRecord + 4) + 0x18004f);
        pwVar12 = g_InGameSelectionDetailNameTextUtf16;
        for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
          *pwVar12 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar12 = pwVar12 + 1;
        }
        pwVar8 = TextResource_Resolve(0x18004e);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName0TextUtf16,pwVar8);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName1TextUtf16,source_01);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName2TextUtf16,source_02);
        g_InGameSelectionDetailTextSlot05Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot05Utf16[1] = 0;
        g_InGameSelectionDetailTextSlot09Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot09Utf16[1] = 0;
        pvVar5 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        if ((*(uint *)((int)pvVar5 + 0xec) & 0x40) != 0) {
          armyDefinition1 =
               ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
          iVar6 = *(int *)((int)pvVar5 + 0x100);
          *(ArmySelectionDetailTemplateVariantIndex *)(firstNode->opaqueA06C_C3E3 + 0x54) =
               armyDefinition1->selectionDetailTemplateVariantIndex + 0x18003c;
          pwVar8 = TextResource_Resolve(iVar6 * 2 + 0x300000);
          RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailTextSlot09Utf16,pwVar8);
        }
        if (*(int *)((int)pvVar5 + 0xc) != 0) {
          if (*(int **)((int)pvVar5 + 0x140) != (int *)0x0) {
            pwVar8 = TextResource_Resolve(*(int *)(**(int **)((int)pvVar5 + 0x140) + 4) + 0x18004f);
            pwVar12 = g_InGameSelectionDetailWeaponName0TextUtf16;
            for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pwVar12 = *pwVar8;
              pwVar8 = pwVar8 + 1;
              pwVar12 = pwVar12 + 1;
            }
          }
          pvVar5 = (entityRuntime1->common).ownership.definitionOrClassRecord;
          if (1 < *(uint *)((int)pvVar5 + 0xc)) {
            piVar10 = *(int **)((int)pvVar5 + 0x160);
            if (piVar10 != (int *)0x0) {
              pwVar8 = TextResource_Resolve(*(int *)(*piVar10 + 4) + 0x18004f);
              pwVar12 = g_InGameSelectionDetailWeaponName1TextUtf16;
              for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
                *pwVar12 = *pwVar8;
                pwVar8 = pwVar8 + 1;
                pwVar12 = pwVar12 + 1;
              }
            }
            pvVar5 = (entityRuntime1->common).ownership.definitionOrClassRecord;
            if ((2 < *(uint *)((int)pvVar5 + 0xc)) &&
               (piVar10 = *(int **)((int)pvVar5 + 0x180), piVar10 != (int *)0x0)) {
              pwVar8 = TextResource_Resolve(*(int *)(*piVar10 + 4) + 0x18004f);
              pwVar12 = g_InGameSelectionDetailWeaponName2TextUtf16;
              for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
                *pwVar12 = *pwVar8;
                pwVar8 = pwVar8 + 1;
                pwVar12 = pwVar12 + 1;
              }
            }
          }
        }
        piVar10 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        iVar6 = *(int *)(*piVar10 + 0x4c);
        if (iVar6 == 0x16) {
          if (piVar10[0x2b] != 1) {
            return;
          }
          armyDefinition1 = ArmyAssetRegistry_FindByIdCf(piVar10[0x18]);
        }
        else if (iVar6 == 0xb) {
          if (piVar10[0x2e] != 1) {
            return;
          }
          armyDefinition1 = ArmyAssetRegistry_FindByIdCf(piVar10[0x18]);
        }
        else {
          if (iVar6 != 0xd) {
            if (iVar6 != 0xe) {
              return;
            }
            (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,piVar10[0x18],
                       g_InGameSelectionDetailWeaponName0TextUtf16);
            return;
          }
          if (piVar10[0x2e] != 1) {
            return;
          }
          armyDefinition1 = ArmyAssetRegistry_FindByIdCf(piVar10[0x18]);
        }
        dVar4 = armyDefinition1->rootNodeOffsetOrPointer;
        if (armyDefinition1->selectionDetailTemplateVariantIndex < 8) {
          *(ArmySelectionDetailTemplateVariantIndex *)(firstNode->opaqueA06C_C3E3 + 0x54) =
               *(int *)(firstNode->opaqueA06C_C3E3 + 0x54) +
               armyDefinition1->selectionDetailTemplateVariantIndex;
        }
        modelDefinition1 =
             ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,dVar4);
        pwVar8 = TextResource_Resolve(extraout_ECX_04 + modelDefinition1->flags);
        pwVar12 = g_InGameSelectionDetailTextSlot05Utf16;
        goto InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots;
      }
    }
    else if ((iVar6 != 0) && (uVar7 == (entityRuntime1->common).ownership.ownerIndex)) {
      UiPageStack_SetActiveIndex(2,stack);
      iVar9 = 0x20;
      piVar10 = &g_InGameSelectionDetailControlOffsetTable32;
      iVar6 = 0xc;
      ppGVar11 = g_SelectionInfoEntitySlots->entries;
      do {
        entityRuntime1 = *ppGVar11;
        if ((entityRuntime1 != (GameEntityRuntime *)0x0) && (iVar6 != 0)) {
          iVar6 = *piVar10;
          *(GameEntityRuntime **)(firstNode->opaque0058_017B + iVar6 + 4) = entityRuntime1;
          armyDefinition1 =
               ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
          *(PckArmyAssetIdCatalog *)(firstNode->opaque0058_017B + iVar6 + -4) =
               armyDefinition1[1].registryId;
          firstNode = (InGameRuntimeRootImageC3E4 *)((int)firstNode + (iVar6 - *piVar10));
          iVar6 = extraout_EDX + -1;
          piVar10 = piVar10 + 1;
          iVar9 = extraout_ECX;
        }
        ppGVar11 = ppGVar11 + 1;
        iVar9 = iVar9 + -1;
      } while (iVar9 != 0);
      for (; iVar6 != 0; iVar6 = iVar6 + -1) {
        pbVar1 = firstNode->opaque0058_017B + *piVar10 + -4;
        pbVar1[0] = 0;
        pbVar1[1] = 0;
        pbVar1[2] = 0;
        pbVar1[3] = 0;
        piVar10 = piVar10 + 1;
      }
      g_InGameSelectionDetailTextSlot05Utf16[0] = (word)g_InGameSelectionDetailTextSlot05Utf16._0_4_
      ;
      g_InGameSelectionDetailTextSlot05Utf16[1] =
           SUB42(g_InGameSelectionDetailTextSlot05Utf16._0_4_,2);
      g_InGameSelectionDetailTextSlot09Utf16[0] = (word)g_InGameSelectionDetailTextSlot09Utf16._0_4_
      ;
      g_InGameSelectionDetailTextSlot09Utf16[1] =
           SUB42(g_InGameSelectionDetailTextSlot09Utf16._0_4_,2);
      return;
    }
    UiPageStack_SetActiveIndex(0,stack);
  }
  else {
    UiPageStack_SetActiveIndex(3,stack);
    uVar3 = *(undefined4 *)(definitionNode + 0x1c);
    AVar14 = ArmyAssetHierarchy_SumFactionUnlockedArmour
                       ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,definitionNode);
    *(undefined4 *)(firstNode->opaqueA06C_C3E3 + 0x128) = uVar3;
    uVar7 = *(uint *)((int)(AVar14 >> 0x20) + 0x24);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)AVar14,
               g_InGameSelectionDetailArmourTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,arg4,
               g_InGameSelectionDetailBuildXeniteCostTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar7 >> 5,
               g_InGameSelectionDetailBuildTimeTextUtf16);
    EVar15 = ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
                       ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,definitionNode_00);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)EVar15 >> 4,
               g_InGameSelectionDetailEnergyTextUtf16);
    iVar6 = *(int *)(extraout_EDX_00 + 4) + 0x180045;
    *(int *)(firstNode->opaqueA06C_C3E3 + 0x184) = iVar6;
    *(int *)(firstNode->opaqueA06C_C3E3 + 0x1070) = iVar6;
    modelDefinition1 =
         ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                   ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                    *(ModelLinkedDefinitionListAddress32 *)(extraout_EDX_00 + 0xc));
    pwVar8 = TextResource_Resolve(extraout_ECX_00 + modelDefinition1->flags);
    pwVar12 = g_InGameSelectionDetailNameTextUtf16;
InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots:
    for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
      *pwVar12 = *pwVar8;
      pwVar8 = pwVar8 + 1;
      pwVar12 = pwVar12 + 1;
    }
    pwVar8 = TextResource_Resolve(0x18004e);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName0TextUtf16,pwVar8);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName1TextUtf16,source);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName2TextUtf16,source_00)
    ;
    if (*(int *)(extraout_EDX_01 + 8) != 0) {
      modelDefinition1 =
           ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                     ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                      *(ModelLinkedDefinitionListAddress32 *)(extraout_EDX_01 + 0xc));
      pwVar8 = TextResource_Resolve(extraout_ECX_01 + modelDefinition1->flags);
      pwVar12 = g_InGameSelectionDetailWeaponName0TextUtf16;
      for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pwVar12 = *pwVar8;
        pwVar8 = pwVar8 + 1;
        pwVar12 = pwVar12 + 1;
      }
      if (1 < *(uint *)(extraout_EDX_02 + 8)) {
        modelDefinition1 =
             ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                        *(ModelLinkedDefinitionListAddress32 *)(extraout_EDX_02 + 0x10));
        pwVar8 = TextResource_Resolve(extraout_ECX_02 + modelDefinition1->flags);
        pwVar12 = g_InGameSelectionDetailWeaponName1TextUtf16;
        for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
          *pwVar12 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar12 = pwVar12 + 1;
        }
        if (2 < *(uint *)(extraout_EDX_03 + 8)) {
          modelDefinition1 =
               ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                         ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                          *(ModelLinkedDefinitionListAddress32 *)(extraout_EDX_03 + 0x14));
          pwVar8 = TextResource_Resolve(extraout_ECX_03 + modelDefinition1->flags);
          pwVar12 = g_InGameSelectionDetailWeaponName2TextUtf16;
          for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pwVar12 = *pwVar8;
            pwVar8 = pwVar8 + 1;
            pwVar12 = pwVar12 + 1;
          }
        }
      }
    }
  }
  return;
}
