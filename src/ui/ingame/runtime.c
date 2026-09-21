/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf
          (dword keyboardStateMask,dword keyboardEventCode,UiRootNode *uiRoot)

{
  UiNodeVtable **stack;
  sdword *psVar1;
  wchar_t wVar2;
  wchar_t wVar3;
  UiNodeVtable *pUVar4;
  dword dVar5;
  UiCommandModeIndex UVar6;
  sdword unaff_EDX;
  int iVar7;
  dword unaff_EBP;
  dword unaff_ESI;
  sdword unaff_EDI;
  dword *pdVar8;
  dword *pdVar9;
  StatusValueEaxCf5 SVar10;
  ArmyRegistryIdEaxCf5_571b00 AVar11;
  ArmyRegistryIdEaxCf5_571d40 AVar12;
  ArmyRegistryIdEaxCf5_571ab0 AVar13;
  ArmyRegistryIdEaxCf5_571cf0 AVar14;
  ArmyRegistryIdEaxCf5_571a10 AVar15;
  ArmyRegistryIdEaxCf5_571c50 AVar16;
  ArmyRegistryIdEaxCf5_571a60 AVar17;
  ArmyRegistryIdEaxCf5_571ca0 AVar18;
  ArmyRegistryEaxCf5_51b6d0 AVar19;
  FatalErrorEaxCf5 FVar20;
  GraphicsFramebufferCaptureEaxCf5 GVar21;
  
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
                    // WARNING: Switch is manually overridden
  switch(pdVar8[2]) {
  case 0x56e5e0:
    stack = &uiRoot[0xbc].base.vtable;
    SVar10 = UiPageStack_ActivePageNotInListCf((UiPageStackControl *)stack);
    if (SVar10.valueOrError == 0) {
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&uiRoot[0xc9].base.right);
      UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&uiRoot[0xcc].base.topOffset);
      uiRoot[0x1d].base.vtable = (UiNodeVtable *)0x0;
      UiContainer_LayoutChildren(&uiRoot->base);
    }
    else {
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)stack);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&uiRoot[0xc9].base.right);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&uiRoot[0xcc].base.topOffset);
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
        AVar11 = ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf
                           (g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = AVar11.eax;
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        AVar12 = ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf
                           (g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = AVar12.eax;
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
        AVar13 = ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(g_UiCommandModeGArmyAssetId)
        ;
        g_UiCommandModeGArmyAssetId = AVar13.eax;
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        AVar14 = ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = AVar14.eax;
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
        AVar15 = ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = AVar15.eax;
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        AVar16 = ArmyAssetRegistry_StepForwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = AVar16.eax;
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
        AVar17 = ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
        g_UiCommandModeGArmyAssetId = AVar17.eax;
        pUVar4 = (UiNodeVtable *)
                 ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
        uiRoot[0x1c9].base.vtable = pUVar4;
        AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
        FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
        g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
        InGameSelectionDetailPanel_Rebuild();
      }
      else if (g_UiCommandModeG == 4) {
        AVar18 = ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
        g_UiCommandMode4ArmyAssetId = AVar18.eax;
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
      AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
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
      AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
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
      AVar19 = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
      FVar20 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar19.eax,AVar19.carry);
      g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)FVar20.eax;
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
    GVar21 = (*g_GraphicsFramebufferCaptureRegion)(unaff_EBP,unaff_ESI,unaff_EDI,unaff_EDX);
    if (!GVar21.carry) {
      FileSystem_WriteBufferToPathCf
                (((GVar21.eax)->common).allocationSizeBytes,GVar21.eax,
                 (word *)(u_Dscreen00_pcx_00572e3a + 1));
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
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  int iVar1;
  uint uVar2;
  CommandPayloadDword04 packedState;
  uint uVar3;
  int *piVar4;
  word *pwVar5;
  bool bVar6;
  UiSelectableNodeEaxEcxCf9 UVar7;
  
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
        InGameRecentTextHistory_InsertAndRebuild8((word *)u_Hmmm__na_gut________0056321e);
      }
    }
    else {
      RichTextCommandStream_CopyToNarrowCf
                (0x30,g_UiSevenSlotCommandPayloadText.textBytes,commandTextEdit->textBuffer);
      UVar7 = UiSelectableGroup_NoneVisibleSelectedCf(3);
      iVar1 = (int)UVar7.node - (int)commandTextEdit;
      if (iVar1 == 0x1dd4) {
        uVar2 = 0;
        packedState = 0;
        uVar3 = 0x100;
        do {
          piVar4 = g_UiSevenSlotSelectionControlOffsets + uVar2;
          uVar3 = uVar3 * 2;
          uVar2 = uVar2 + 1;
          bVar6 = (bool)UiSelectableControl_IsSelectedCf
                                  ((UiSelectableControl *)(*piVar4 + -0xb0 + (int)commandTextEdit));
          if (bVar6) {
            packedState = packedState | uVar3;
          }
        } while (uVar2 < 7);
      }
      else if (iVar1 == 0x1e34) {
        uVar2 = 0;
        packedState = 0;
        uVar3 = 0x8000;
        do {
          piVar4 = g_UiSevenSlotSelectionControlOffsets + uVar2;
          uVar3 = uVar3 * 2;
          uVar2 = uVar2 + 1;
          bVar6 = (bool)UiSelectableControl_IsSelectedCf
                                  ((UiSelectableControl *)(*piVar4 + -0xb0 + (int)commandTextEdit));
          if (bVar6) {
            packedState = packedState | uVar3;
          }
        } while (uVar2 < 7);
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

void __thandor_void_preserve_eax_ecx_edx
InGameUiAction1210_ResourceRegistrationHelper(void *runtimeBase,void *resourcePath)

{
  word *arg0;
  byte *destination;
  InGameLevelConditionStorageView800 *sourceData;
  void *handle;
  dword *sourceData_00;
  dword *sourceData_01;
  dword dVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  int iVar3;
  PckDecodedByteCount unpackedSize;
  dword *headerDwords;
  FrontendPlayerRuntimeRecord *pFVar4;
  dword *pdVar5;
  dword *pdVar6;
  bool bVar7;
  RuntimeImagePointerByteSizeEdxEax8 RVar8;
  StatusValueEaxCf5 SVar9;
  ArenaAllocEaxCf5 AVar10;
  FileSystemSeekEaxCf5 FVar11;
  FileSystemReadEaxCf5 FVar12;
  FileSystemWriteEaxCf5 FVar13;
  ResourceRegistrationImagePair RVar14;
  uint uVar15;
  
  g_InGameResourceRegistrationBusyCount = g_InGameResourceRegistrationBusyCount + '\x01';
  pFVar4 = g_FrontendPlayerRuntimeBlocks;
  for (FVar2 = g_FrontendPlayerRuntimeBlockCount; FVar2 != 0; FVar2 = FVar2 - 1) {
    GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
              (pFVar4->playerRuntimeId,0,0,(pFVar4->factionAssignment).factionAssignmentIndex);
    pFVar4 = pFVar4 + 1;
  }
  SVar9 = ResourceRegistration_OpenSourceCf(resourcePath);
  handle = (void *)SVar9.valueOrError;
  if (SVar9.carry) {
    WidePath_SplitParentAndLeaf((word *)g_PackageScratchBuffer,(word *)0x50dcc4,resourcePath);
    SVar9 = (*g_FileSystemCreateDirectoryRecursiveCf)
                      (FILESYSTEM_CREATE_DIRECTORY_RECURSIVE,(word *)0x50dcc4);
    if (SVar9.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
    SVar9 = ResourceRegistration_OpenSourceCf(resourcePath);
    handle = (void *)SVar9.valueOrError;
    if (SVar9.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  RVar8 = ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(RVar8 >> 0x20),
                              (dword *)RVar8,(word *)u_army_hex_0050dfb4,(EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  ArmyRuntimePool_RebaseAfterLoad();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ModelRuntimePool_UnrebaseBeforeSave();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,unpackedSize,sourceData_00,
                              (word *)u_modul_hex_0050dfee,(EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  ModelRuntimePool_RebaseAfterLoad();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar14 = ResourceRegistration_QueryDomain2Pair();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar14,
                              (dword *)(RVar14 >> 0x20),(word *)u_shot_hex_0050dfdc,
                              (EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  ShotRuntime_RebaseSlotsAfterLoad();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar14 = ResourceRegistration_QueryDomain1Pair();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar14,
                              (dword *)(RVar14 >> 0x20),(word *)u_effect_hex_0050dfc6,
                              (EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  EffectRuntime_RebaseSlotsAfterLoad();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar14 = ResourceRegistration_SelectDomainPair(runtimeBase);
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar14,
                              (dword *)(RVar14 >> 0x20),(word *)u_widget_hex_0050e02a,
                              (EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  ResourceRegistrationRuntime_RebaseLoadedRecords(runtimeBase);
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar8 = RuntimeHexSegment_GetLightImageAndToggleFlagRegs();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)(RVar8 >> 0x20),
                              (dword *)RVar8,(word *)u_light_hex_0050e016,(EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  RuntimeHexSegment_ToggleLightImageFlag();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar14 = RuntimeHexSegment_GetFieldImageRegs(runtimeBase);
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar14,
                              (dword *)(RVar14 >> 0x20),(word *)u_field_hex_0050e002,
                              (EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  RuntimeHexSegment_AfterFieldImageNoOp(runtimeBase);
  sourceData = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  ResourceRegistration_ResolveRuntimeRecord(runtimeBase);
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (sourceData->levelImage).header.resourceTables.
                              runtimePrefixByteSizeAndInitialArmyPlacementOffset,(dword *)sourceData
                              ,(word *)u_level_hex_0050e040,(EngineFileHandle)handle);
  if (SVar9.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  RVar14 = ResourceRegistration_QueryDomain0Pair();
  SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)RVar14,
                              (dword *)(RVar14 >> 0x20),(word *)u_daten_hex_0050e054,
                              (EngineFileHandle)handle);
  uVar15 = (uint)(SVar9.carry & 1);
  GameFactionRuntime_RebaseLoadedArmyReferences();
  if ((uVar15 & 1) != 0) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  if (g_FrontendLoadedCampaignAsset == (dword *)0x0) {
    Package_DeleteEntry((word *)u_campagne_hex_0050e068,(EngineFileHandle)handle);
  }
  else {
    SVar9 = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,g_FrontendLoadedCampaignAsset[1],
                                g_FrontendLoadedCampaignAsset,(word *)u_campagne_hex_0050e068,
                                (EngineFileHandle)handle);
    if (SVar9.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,0x38000,g_GameStatTableImage,
                      (word *)u_stat_hex_0050e082,(EngineFileHandle)handle);
  if (g_OldUnitRecordCount == 0) {
    iVar3 = 0x40;
    bVar7 = true;
    pdVar5 = g_OldUnitSecondaryTable;
    do {
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      bVar7 = *pdVar5 == 0;
      pdVar5 = pdVar5 + 1;
    } while (bVar7);
    if (!bVar7) goto InGameResourceRegistration_SerializeOldUnitTables;
    Package_DeleteEntry((word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
  }
  else {
InGameResourceRegistration_SerializeOldUnitTables:
    AVar10 = (*g_MemoryApi.alloc)(0x4104);
    pdVar5 = g_OldUnitPrimaryTable;
    sourceData_01 = (dword *)AVar10.eax;
    if (AVar10.carry) goto InGameResourceRegistration_DecrementBusyCountAndReturn;
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
    Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(int)pdVar6 - (int)sourceData_01,sourceData_01,
                        (word *)u_oldunit_hex_0050e094,(EngineFileHandle)handle);
    (*g_MemoryApi.free)(sourceData_01);
  }
  destination = g_PackageScratchBuffer;
  FVar11 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
  if ((!FVar11.carry) &&
     (FVar12 = (*g_FileSystemReadExactCf)(0x200,destination,handle), !FVar12.carry)) {
    WidePath_SplitParentAndLeaf
              ((word *)(destination + 0x100),(word *)(destination + 0x200),resourcePath);
    dVar1 = (*g_LocaleGetPackedCurrentDate)();
    *(dword *)(destination + 0x1f0) = dVar1;
    dVar1 = (*g_LocaleGetPackedCurrentTime)();
    *(dword *)(destination + 500) = dVar1;
    dVar1 = (*g_LocaleFormatCurrentDateUtf16)((word *)(destination + 0x1c0));
    arg0 = (word *)(dVar1 + 4 + (int)(destination + 0x1c0));
    arg0[-0xffffffff00000002] = 0x2c;
    arg0[-0xffffffff00000001] = 0x20;
    (*g_LocaleFormatCurrentTimeUtf16)(arg0);
    dVar1 = g_InGameLevelCampaignAssociationIndex;
    if (g_FrontendLoadedCampaignAsset == (dword *)0x0) {
      dVar1 = 0xffffffff;
    }
    headerDwords[0x5c] = g_InGameLevelTitleTextResourceIndex;
    headerDwords[100] = dVar1;
    FVar11 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,handle);
    if ((!FVar11.carry) &&
       (FVar13 = (*g_FileSystemWriteExactOrFlushCf)(0x200,headerDwords,handle), !FVar13.carry)) {
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
void __thandor_void_preserve_eax_ecx_edx
InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl)

{
  longlong lVar1;
  int iVar2;
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
  DirectSoundVoiceSet *pDVar15;
  UiRootNode *pUVar17;
  UiNodeVtable *pUVar18;
  word *stream;
  int iVar19;
  TextResourceId resourceId;
  int iVar20;
  UiRootNode *pUVar21;
  UiNodeBase *pUVar22;
  int iVar23;
  UiNodeBase *pUVar24;
  int iVar25;
  uint uVar26;
  uint uVar27;
  GraphicsTextureSourceLoadEaxCf5 GVar28;
  TextResourceResolveEaxCf5 TVar29;
  StatusValueEaxCf5 SVar30;
  GraphicsTextureSizeEaxEdxCf9 GVar31;
  GraphicsTextureSourceAsset *pGVar16;
  
  if ((g_FramebufferWidth < 800) || (g_FramebufferHeight < 600)) {
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
  else if ((g_FramebufferWidth < 0x400) || (g_FramebufferHeight < 0x300)) {
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
  GVar28 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_panel0_gfx_005630d0);
  pGVar13 = g_InGamePanelTextureSource;
  pGVar16 = GVar28.eax;
  if (!GVar28.carry) {
    LOCK();
    UNLOCK();
    g_InGamePanelTextureSource = pGVar16;
    (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0,pGVar16);
    g_InGamePanelTextureSubresource00Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(1,pGVar16);
    g_InGamePanelTextureSubresource01Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(2,pGVar16);
    g_InGamePanelTextureSubresource02Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(6,pGVar16);
    g_InGamePanelTextureSubresource06Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(7,pGVar16);
    g_InGamePanelTextureSubresource07Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1b,pGVar16);
    g_InGamePanelTextureSubresource27Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1c,pGVar16);
    g_InGamePanelTextureSubresource28Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,pGVar16);
    g_InGamePanelTextureSubresource19Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x14,pGVar16);
    g_InGamePanelTextureSubresource20Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,pGVar16);
    g_InGamePanelTextureSubresource34Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,pGVar16);
    g_InGamePanelTextureSubresource32Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x21,pGVar16);
    g_InGamePanelTextureSubresource33Width = GVar31.logicalWidthPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(2,pGVar16);
    g_InGamePanelTextureSubresource02Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(3,pGVar16);
    g_InGamePanelTextureSubresource03Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(4,pGVar16);
    g_InGamePanelTextureSubresource04Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(5,pGVar16);
    g_InGamePanelTextureSubresource05Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x24,pGVar16);
    g_InGamePanelTextureSubresource36Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x25,pGVar16);
    g_InGamePanelTextureSubresource37Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(6,pGVar16);
    g_InGamePanelTextureSubresource06Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0,pGVar16);
    g_InGamePanelTextureSubresource00Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(7,pGVar16);
    g_InGamePanelTextureSubresource07Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1a,pGVar16);
    g_InGamePanelTextureSubresource26Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x1f,pGVar16);
    g_InGamePanelTextureSubresource31Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x12,pGVar16);
    g_InGamePanelTextureSubresource18Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x17,pGVar16);
    g_InGamePanelTextureSubresource23Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x22,pGVar16);
    g_InGamePanelTextureSubresource34Height = GVar31.logicalHeightPixels;
    GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0x20,pGVar16);
    pGVar13 = g_InGamePanelTextureSource;
    g_InGamePanelTextureSubresource32Height = GVar31.logicalHeightPixels;
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
    resourceId = 0x18002c;
    do {
      TVar29 = TextResource_Resolve(resourceId);
      stream = TVar29.eax;
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
    iVar20 = 3;
    iVar23 = (int)((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
    uVar27 = 0;
    iVar19 = 0;
    iVar25 = 0;
    do {
      iVar12 = (&g_InGameSelectionDetailControlOffsetTable32)[uVar27];
      *(int *)((int)&(inGameRoot->base).leftOffset + iVar12) = iVar19;
      *(int *)((int)&(inGameRoot->base).topOffset + iVar12) = iVar25;
      iVar19 = iVar19 + iVar23;
      iVar25 = iVar25 + iVar23;
      *(int *)((int)&(inGameRoot->base).rightOffset + iVar12) = iVar19;
      *(int *)((int)&(inGameRoot->base).bottomOffset + iVar12) = iVar25;
      uVar27 = uVar27 + 1;
      iVar20 = iVar20 + -1;
      if (iVar20 == 0) {
        iVar20 = 3;
        iVar19 = 0;
      }
      else {
        iVar25 = iVar25 - iVar23;
      }
    } while (uVar27 < 0xc);
    GVar28 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_diagram0_gfx_00563120);
    pGVar13 = g_InGameDiagramTextureSource;
    pGVar16 = GVar28.eax;
    if (!GVar28.carry) {
      LOCK();
      UNLOCK();
      g_InGameDiagramTextureSource = pGVar16;
      (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
      inGameRoot[0xd6].base.bottomOffset = (sdword)pGVar16;
      inGameRoot[0xd8].base.top = (sdword)pGVar16;
      inGameRoot[0xd9].previousRoot = (UiRootNode *)pGVar16;
      GVar28 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_window_gfx_0056318e);
      pGVar13 = g_InGameWindowTextureSource;
      pGVar16 = GVar28.eax;
      if (!GVar28.carry) {
        LOCK();
        UNLOCK();
        g_InGameWindowTextureSource = pGVar16;
        (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
        inGameRoot[0x2a].callbacks = (UiRootCallbacks *)pGVar16;
        inGameRoot[0x52].base.firstChild = (UiNodeBase *)pGVar16;
        inGameRoot[0x24].base.rightAnchorQ31 = (UiAnchorFractionQ31)pGVar16;
        inGameRoot[0x25].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar16;
        inGameRoot[0x26].base.layoutWidth = (sdword)pGVar16;
        inGameRoot[0x27].base.layoutHeight = (sdword)pGVar16;
        inGameRoot[0x28].base.nodeFlags = (UiNodeFlags)pGVar16;
        inGameRoot[0x29].rootFlags = (UiRootFlags)pGVar16;
        GVar28 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_tech_gfx_005630fa);
        pGVar13 = g_InGameTechnologyTextureSource;
        pGVar16 = GVar28.eax;
        if (!GVar28.carry) {
          LOCK();
          UNLOCK();
          g_InGameTechnologyTextureSource = pGVar16;
          (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(pGVar13);
          inGameRoot[0x47].base.nextSibling = (UiNodeBase *)pGVar16;
          inGameRoot[0x48].base.firstChild = (UiNodeBase *)pGVar16;
          inGameRoot[0x49].base.parent = (UiNodeBase *)pGVar16;
          inGameRoot[0x4a].base.vtable = (UiNodeVtable *)pGVar16;
          inGameRoot[0x4b].base.left = (sdword)pGVar16;
          inGameRoot[0x4c].base.top = (sdword)pGVar16;
          inGameRoot[0x4d].base.right = (sdword)pGVar16;
          GVar31 = (*g_GraphicsTextureSourceGetLogicalSize)(0,pGVar16);
          uVar26 = GVar31.logicalHeightPixels;
          psVar1 = &inGameRoot[0x3e].base.right;
          *psVar1 = *psVar1 - uVar26;
          psVar1 = &inGameRoot[0x3f].base.rightOffset;
          *psVar1 = *psVar1 - uVar26;
          pUVar2 = &inGameRoot[0x40].base.rightAnchorQ31;
          *pUVar2 = *pUVar2 - uVar26;
          pUVar3 = &inGameRoot[0x41].base.nodeFlags;
          *pUVar3 = *pUVar3 - uVar26;
          inGameRoot[0x43].base.nextSibling =
               (UiNodeBase *)((int)inGameRoot[0x43].base.nextSibling - uVar26);
          psVar1 = &inGameRoot[0x44].base.left;
          *psVar1 = *psVar1 - uVar26;
          psVar1 = &inGameRoot[0x45].base.leftOffset;
          *psVar1 = *psVar1 - uVar26;
          inGameRoot[0x4e].callbacks = (UiRootCallbacks *)((int)inGameRoot[0x4e].callbacks - uVar26)
          ;
          uVar27 = GVar31.logicalWidthPixels * 7 >> 1;
          psVar1 = &inGameRoot[0x2a].base.bottom;
          *psVar1 = *psVar1 - uVar27;
          psVar1 = &inGameRoot[0x2a].base.topOffset;
          *psVar1 = *psVar1 + uVar27;
          psVar1 = &inGameRoot[0x2a].base.leftOffset;
          *psVar1 = *psVar1 - (uVar26 >> 1);
          psVar1 = &inGameRoot[0x2a].base.rightOffset;
          *psVar1 = *psVar1 + (uVar26 >> 1);
          inGameRoot[0x50].previousRoot =
               (UiRootNode *)
               ((inGameRoot[0x2a].base.topOffset - inGameRoot[0x2a].base.bottom) + -0x18 +
               (inGameRoot[0x4e].rootFlags - inGameRoot[0x4e].base.layoutHeight));
          pDVar15 = g_UiButtonSoundVoiceSets7[0];
          inGameRoot[0xd0].base.layoutWidth = (sdword)g_UiButtonSoundVoiceSets7[0];
          inGameRoot[0xd3].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0xd5].base.topOffset = (sdword)pDVar15;
          inGameRoot[0xe1].base.bottom = (sdword)pDVar15;
          inGameRoot[0x111].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x159].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x199].base.bottom = (sdword)pDVar15;
          inGameRoot[0xdd].base.bottom = (sdword)pDVar15;
          inGameRoot[0xde].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0xe0].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x1bd].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x1be].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x1c0].base.right = (sdword)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[1];
          inGameRoot[0xc5].base.layoutWidth = (sdword)g_UiButtonSoundVoiceSets7[1];
          inGameRoot[199].base.parent = (UiNodeBase *)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[2];
          inGameRoot[0x207].base.right = (sdword)g_UiButtonSoundVoiceSets7[2];
          inGameRoot[0x208].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x20a].base.left = (sdword)pDVar15;
          inGameRoot[0x20b].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x20d].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x20e].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x210].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x211].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x212].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x214].base.right = (sdword)pDVar15;
          inGameRoot[0x215].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x217].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x218].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x219].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x21b].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x21c].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x21d].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x21f].base.left = (sdword)pDVar15;
          inGameRoot[0x220].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x223].base.right = (sdword)pDVar15;
          inGameRoot[0x224].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x226].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x227].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x228].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x22a].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x22b].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x22c].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x22e].base.left = (sdword)pDVar15;
          inGameRoot[0x1d5].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x107].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x109].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x10a].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x10c].base.left = (sdword)pDVar15;
          inGameRoot[0x10d].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x10f].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x110].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x114].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x115].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x117].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x118].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x119].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x11b].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x11c].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x11e].base.bottom = (sdword)pDVar15;
          inGameRoot[0x11f].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x121].base.top = (sdword)pDVar15;
          inGameRoot[0x122].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x124].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x125].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x127].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x128].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x129].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[299].base.topOffset = (sdword)pDVar15;
          inGameRoot[300].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x12e].base.bottom = (sdword)pDVar15;
          inGameRoot[0x12f].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x131].base.top = (sdword)pDVar15;
          inGameRoot[0x132].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x134].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x135].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x137].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x138].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x139].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x13b].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x13c].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x13e].base.bottom = (sdword)pDVar15;
          inGameRoot[0x13f].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x141].base.top = (sdword)pDVar15;
          inGameRoot[0x142].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x144].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x145].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x147].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x148].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x149].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x14b].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x14c].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x14e].base.bottom = (sdword)pDVar15;
          inGameRoot[0x14f].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x151].base.top = (sdword)pDVar15;
          inGameRoot[0x152].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x154].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x155].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x157].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x158].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x15c].base.right = (sdword)pDVar15;
          inGameRoot[0x15d].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x15f].base.left = (sdword)pDVar15;
          inGameRoot[0x160].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x162].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x163].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x165].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x166].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x167].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x169].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x16a].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x16c].base.right = (sdword)pDVar15;
          inGameRoot[0x16d].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x16f].base.left = (sdword)pDVar15;
          inGameRoot[0x170].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x172].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x173].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x175].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x176].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x177].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x179].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x17a].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x17c].base.right = (sdword)pDVar15;
          inGameRoot[0x17d].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x17f].base.left = (sdword)pDVar15;
          inGameRoot[0x180].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x182].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x183].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x185].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x186].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x187].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x189].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x18a].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x18c].base.right = (sdword)pDVar15;
          inGameRoot[0x18d].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[399].base.left = (sdword)pDVar15;
          inGameRoot[400].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x192].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x193].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x195].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x196].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x197].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x19b].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x19d].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x19e].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x19f].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x1a1].base.bottom = (sdword)pDVar15;
          inGameRoot[0x1a2].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x1a4].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0x1a5].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x1a6].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x1a8].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x1a9].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x1ab].base.left = (sdword)pDVar15;
          inGameRoot[0x1ac].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x1ae].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x1af].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x1b0].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x1b2].base.top = (sdword)pDVar15;
          inGameRoot[0x1b3].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x1b5].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x1b6].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x1b7].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x1b9].base.right = (sdword)pDVar15;
          inGameRoot[0x1ba].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x1bc].base.parent = (UiNodeBase *)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[3];
          inGameRoot[0x11].base.parent = (UiNodeBase *)g_UiButtonSoundVoiceSets7[3];
          inGameRoot[0x12].base.left = (sdword)pDVar15;
          inGameRoot[0x13].base.right = (sdword)pDVar15;
          inGameRoot[0x14].base.leftOffset = (sdword)pDVar15;
          inGameRoot[0x15].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x16].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x17].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x6d].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x6f].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x6e].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x70].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x7e].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x7f].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x81].previousRoot = (UiRootNode *)pDVar15;
          inGameRoot[0x83].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x8d].base.right = (sdword)pDVar15;
          inGameRoot[0x8f].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x90].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x91].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x92].base.bottomAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0xa6].base.bottomOffset = (sdword)pDVar15;
          inGameRoot[0x58].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x57].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x56].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x3d].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x3c].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x2d].base.parent = (UiNodeBase *)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[4];
          inGameRoot[0x78].base.bottom = (sdword)g_UiButtonSoundVoiceSets7[4];
          inGameRoot[0x79].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x7b].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x7c].base.leftAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x7d].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x94].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0xa3].base.top = (sdword)pDVar15;
          inGameRoot[0xa4].base.bottom = (sdword)pDVar15;
          inGameRoot[0xa5].base.topOffset = (sdword)pDVar15;
          inGameRoot[0xa8].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0xa9].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0xaa].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x96].rootFlags = (UiRootFlags)pDVar15;
          inGameRoot[0x98].base.firstChild = (UiNodeBase *)pDVar15;
          inGameRoot[0x99].base.top = (sdword)pDVar15;
          inGameRoot[0x9a].base.topOffset = (sdword)pDVar15;
          inGameRoot[0x9b].base.topAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x9c].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0x59].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x5c].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x5a].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x60].base.layoutWidth = (sdword)pDVar15;
          inGameRoot[0x61].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x62].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[100].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x65].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x66].base.left = (sdword)pDVar15;
          inGameRoot[0x67].base.right = (sdword)pDVar15;
          inGameRoot[0x3e].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x40].base.parent = (UiNodeBase *)pDVar15;
          inGameRoot[0x41].base.right = (sdword)pDVar15;
          inGameRoot[0x42].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0x43].base.rightAnchorQ31 = (UiAnchorFractionQ31)pDVar15;
          inGameRoot[0x44].base.nodeFlags = (UiNodeFlags)pDVar15;
          inGameRoot[0x46].base.nextSibling = (UiNodeBase *)pDVar15;
          inGameRoot[0x2e].base.left = (sdword)pDVar15;
          inGameRoot[0x2f].base.right = (sdword)pDVar15;
          inGameRoot[0x30].base.leftOffset = (sdword)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[5];
          inGameRoot[0xa1].base.left = (sdword)g_UiButtonSoundVoiceSets7[5];
          inGameRoot[0xaf].base.vtable = (UiNodeVtable *)pDVar15;
          inGameRoot[0xb3].base.rightOffset = (sdword)pDVar15;
          inGameRoot[0xb7].base.layoutHeight = (sdword)pDVar15;
          inGameRoot[0xbc].base.parent = (UiNodeBase *)pDVar15;
          pDVar15 = g_UiButtonSoundVoiceSets7[6];
          inGameRoot[0x8a].base.topOffset = (sdword)g_UiButtonSoundVoiceSets7[6];
          inGameRoot[0x85].callbacks = (UiRootCallbacks *)pDVar15;
          inGameRoot[0x54].base.leftOffset = (sdword)pDVar15;
          inGameRoot[3].base.left = (sdword)pDVar15;
          GVar28.carry = false;
          GVar28.eax = (GraphicsTextureSourceAsset *)pDVar15;
        }
      }
    }
  }
  SVar30.valueOrError = (dword)GVar28.eax;
  SVar30.carry = GVar28.carry;
  return SVar30;
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
  SelectionPlayerRuntimeBlock *pSVar1;
  uint uVar2;
  ulonglong uVar3;
  dword value;
  word *stream;
  uint uVar4;
  FrontendPlayerRuntimeBlockCount FVar5;
  WorldRuntimeContext *world;
  FrontendPlayerRuntimeRecord *pFVar6;
  int iVar7;
  int iVar8;
  word *pwVar9;
  word *destination;
  RichTextCopyExpandedEaxCf5 RVar10;
  TextResourceResolveEaxCf5 TVar11;
  TextResourceResolveEaxCf5 TVar12;
  WorldVector0EaxEcxEdx12 WVar13;
  WorldVector1EaxEcxEdx12 WVar14;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  uVar4 = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown = g_DebugOverlayCounterRefreshCountdown - 1;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 0x14;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    if (uVar4 == 0) {
      uVar4 = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,uVar4,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,uVar4,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,uVar4,
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
  WVar13 = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar13.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar13.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar13.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  WVar14 = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar14.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar14.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar14.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
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
  uVar3 = (ulonglong)(g_GameFactionRuntimeImage.tail.simulationTick + 0x4af) / 0x4b0;
  (*g_LocaleFormatTimeFieldsUtf16)
            ((dword)(uVar3 / 0x3c),(dword)(uVar3 % 0x3c),g_FrontendDebugOverlayTextSlot13Utf16);
  uVar4 = 1;
  iVar7 = 0x50fa80;
  destination = g_InGameFactionStatusTextScratchUtf16;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar4] != 0) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar4] <
        FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(iVar7 + 0x90) + *(int *)(iVar7 + 0x94),(word *)0x55056e);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster:
        TVar11 = TextResource_Resolve(0x21d4);
        pwVar9 = TVar11.eax;
      }
      else {
        iVar8 = 0;
        FVar5 = g_FrontendPlayerRuntimeBlockCount;
        pFVar6 = g_FrontendPlayerRuntimeBlocks;
        pwVar9 = g_InGamePlayerListTextScratchUtf16;
        do {
          if (uVar4 == (pFVar6->factionAssignment).factionAssignmentIndex) {
            if (iVar8 != 0) {
              pwVar9[0] = 0x2c;
              pwVar9[1] = 0x20;
              pwVar9 = pwVar9 + 2;
            }
            iVar8 = iVar8 + 1;
            RVar10 = RichTextCommandStream_CopyExpandedCf
                               (0x28,pwVar9,(pFVar6->playerName).textUtf16);
            if (!RVar10.carry) {
              pwVar9 = (word *)((int)pwVar9 + RVar10.eax);
              pSVar1 = g_SelectionPlayerRuntimeBlockPointers[pFVar6->playerRuntimeId];
              uVar2 = pSVar1->simulationStepTicks;
              if ((pSVar1->sessionFlags & 1) != 0) {
                pwVar9[0] = 0x20;
                pwVar9[1] = 0x20;
                pwVar9[2] = 0x50;
                pwVar9[3] = 0;
                pwVar9 = pwVar9 + 3;
              }
              if (1 < uVar2) {
                pwVar9[0] = 0x20;
                pwVar9[1] = 0x20;
                *(uint *)(pwVar9 + 2) = uVar2 * 0x10000 + 0x300078;
                pwVar9 = pwVar9 + 4;
              }
              if ((pSVar1->sessionFlags & 2) != 0) {
                pwVar9[0] = 0x20;
                pwVar9[1] = 0x20;
                pwVar9[2] = 0x8004;
                pwVar9[3] = 0x8003;
                pwVar9[4] = 0x57;
                pwVar9[5] = 0x8005;
                pwVar9 = pwVar9 + 6;
              }
            }
          }
          pFVar6 = pFVar6 + 1;
          FVar5 = FVar5 - 1;
        } while (FVar5 != 0);
        *pwVar9 = 0;
        if (iVar8 == 0)
        goto 
        InGameHud_UpdateStatusCountersAndSessionPrompts_ResolveFactionStatusTemplateWithoutPlayerRoster
        ;
        TVar11 = TextResource_Resolve(0x21d3);
        pwVar9 = TVar11.eax;
        RichTextCommandStream_PatchPayloadBySelector(0,g_InGamePlayerListTextScratchUtf16,pwVar9);
      }
      TVar11 = TextResource_Resolve(*(int *)(iVar7 + 0x38) + 0x2173);
      TVar12 = TextResource_Resolve(0x21d2);
      stream = TVar12.eax;
      RichTextCommandStream_PatchPayloadBySelector(0,TVar11.eax,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,pwVar9,stream);
      RichTextCommandStream_PatchPayloadBySelector(2,(void *)0x55056e,stream);
      RVar10 = RichTextCommandStream_CopyExpandedCf(0x400,destination,stream);
      if (!RVar10.carry) {
        destination = (word *)((int)destination + RVar10.eax);
      }
    }
    uVar4 = uVar4 + 1;
    iVar7 = iVar7 + 0x740;
    if (7 < uVar4) {
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
  FrontendPlayerRuntimeRecord *pFVar1;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount FVar2;
  int iVar3;
  InGamePlayerStatusTextSlot *destination;
  FrontendPlayerNameUtf16_28 *replacementPayload;
  RichTextExtentRegs RVar4;
  TextResourceResolveEaxCf5 TVar5;
  GraphicsTextureSizeEaxEdxCf9 GVar6;
  
  (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar1 = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    replacementPayload = &pFVar1->playerName;
    RVar4 = RichTextCommandStream_MeasureRegs
                      (g_UiTextStyleNormal,(word *)u_gfx_panel_panel0_gfx_005630d0);
    iVar3 = (RVar4.heightPixels * FVar2 >> 1) + GVar6.logicalHeightPixels;
    destination = g_InGamePlayerStatusTextSlots;
    *(FrontendPlayerRuntimeBlockCount *)((int)uiState + 0x93c) = FVar2;
    *(int *)((int)uiState + 0x910) = iVar3;
    *(int *)((int)uiState + 0x908) = -iVar3;
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
      TVar5 = TextResource_Resolve(resourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,TVar5.eax);
      RichTextCommandStream_CopyExpandedCf(0x80,destination->text,TVar5.eax);
      destination = destination + 1;
      replacementPayload = replacementPayload + 0x7e;
      FVar2 = FVar2 - 1;
    } while (FVar2 != 0);
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
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        bVar1 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(context->activeFactionRuntimeIndex);
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
void __thandor_void_preserve_eax_ecx_edx
InGameNotificationQueue_InsertPriorityRecord
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
void __thandor_void_preserve_eax_ecx_edx
InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  SessionNetworkRoleFlags SVar3;
  UiControlCount UVar4;
  int iVar5;
  uint uVar6;
  dword dVar7;
  int iVar8;
  uint uVar9;
  SessionNetworkRoleFlags SVar10;
  FrontendPlayerRuntimeRecord *pFVar11;
  uint uVar12;
  UiGridDimensionsEdxEax8 UVar13;
  UiControlCount local_20;
  
  pUVar2 = node->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    pUVar2 = node->parent;
  }
  iVar5 = 1;
  local_20 = 0;
  dVar7 = g_GameFactionRuntimeImage.tail.activeFactionCount;
  do {
    if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar5] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (iVar5 != node[0x23].bottom)) &&
       ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
      local_20 = local_20 + 1;
    }
    iVar5 = iVar5 + 1;
    dVar7 = dVar7 - 1;
  } while (dVar7 != 0);
  UVar13 = UiGrid_OneColumnDimensionsPacked(local_20);
  iVar5 = (int)UVar13 * g_InGamePanelTextureSubresource32Width +
          g_InGamePanelTextureSubresource19Width + g_InGamePanelTextureSubresource20Width;
  iVar8 = (int)(UVar13 >> 0x20) * g_InGamePanelTextureSubresource32Height +
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
  node[0x105].bottom = node[0x105].bottom - iVar5;
  node[0x105].leftOffset = node[0x105].leftOffset - iVar8;
  node[0x104].topOffset = node[0x104].topOffset | 8;
  if ((local_20 != 0) && ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) {
    node[0x104].topOffset = node[0x104].topOffset & 0xfffffff7;
  }
  (**(code **)(node[0x103].topAnchorQ31 + 0xc))((UiNodeBase *)&node[0x103].rightOffset);
  uVar12 = 0;
  if (local_20 != 0) {
    uVar6 = 1;
    iVar5 = 0x50fa80;
    do {
      UVar4 = local_20 + 1;
      if ((uVar6 != node[0x23].bottom) &&
         (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE)) {
        g_UiAction1012TargetPlayerIndices[uVar12] = uVar6;
        UiPageStack_SetActiveIndex
                  (0,(UiPageStackControl *)
                     ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[uVar12]));
        *(int *)((int)&node[1].parent + g_UiAction1012PlayerLabelTextOffsets[uVar12]) =
             *(int *)(iVar5 + 0x38) + 0x2173;
        *(uint *)((int)&node[1].parent + g_UiAction1012PlayerIndexTextOffsets[uVar12]) =
             uVar6 + 0x2190;
        uVar9 = g_GameFactionRuntimeImage.records[node[0x23].bottom].packedRelationStates >>
                ((byte)(uVar6 << 2) & 0x1f) & 0xf;
        uVar6 = uVar6 & 0x3fffffff;
        *(uint *)((int)&node[1].parent + g_UiAction1012StateTextOffsets[uVar12]) = uVar9 + 0x21a3;
        iVar8 = g_UiAction1012IconImageOffsets[uVar12];
        *(undefined **)((int)&node[1].parent + iVar8) = &g_EmptyFrontendPlayerNameUtf16;
        pFVar11 = g_FrontendPlayerRuntimeBlocks;
        SVar10 = g_FrontendPlayerRuntimeBlockCount;
        SVar3 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (SVar3 != SESSION_NETWORK_ROLE_LOCAL) {
          if ((pFVar11->factionAssignment).factionAssignmentIndex == uVar6) {
            *(FrontendPlayerNameUtf16_28 **)((int)&node[1].parent + iVar8) = &pFVar11->playerName;
            break;
          }
          pFVar11 = pFVar11 + 1;
          SVar10 = SVar10 - SESSION_NETWORK_ROLE_CLIENT;
          SVar3 = SVar10;
        }
        iVar8 = g_UiAction1012ControlOffsets[uVar12];
        dVar7 = g_UiAction1012SubresourceByState[uVar9];
        uVar12 = uVar12 + 1;
        puVar1 = (uint *)((int)&node->nodeFlags + iVar8);
        *puVar1 = *puVar1 & 0xfffffff7;
        *(dword *)((int)&node[1].vtable + iVar8) = dVar7;
        UVar4 = local_20;
        if (((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) != 0) &&
           ((7 < uVar9 ||
            (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) != 0 &&
             ((3 < uVar9 || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) != 0)))))))) {
          puVar1 = (uint *)((int)&node->nodeFlags + iVar8);
          *puVar1 = *puVar1 | 8;
        }
      }
      local_20 = UVar4;
      uVar6 = uVar6 + 1;
      iVar5 = iVar5 + 0x740;
      local_20 = local_20 - 1;
    } while (local_20 != 0);
    if (6 < uVar12) {
      return;
    }
  }
  do {
    UiPageStack_SetActiveIndex
              (1,(UiPageStackControl *)
                 ((int)&node->nextSibling + g_UiAction1012SlotPageOffsets[uVar12]));
    uVar12 = uVar12 + 1;
  } while (uVar12 < 7);
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
  int iVar1;
  dword dVar2;
  ArmyAssetRecordPrefix *armyDefinition1;
  int iVar3;
  int iVar4;
  ArmyRegistryEaxCf5_51b6d0 AVar5;
  int iStack_28;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  WorldOwnerListNode100 *worldNode1;
  
  dVar2 = 0;
  iStack_1c = 0;
  iStack_20 = 0;
  iStack_24 = 0;
  iStack_28 = 0;
  if (trackClassId != 0) {
    iVar1 = worldRuntime->activeFactionRuntimeIndex;
    for (worldNode1 = worldRuntime->ownerListHead; worldNode1 != (WorldOwnerListNode100 *)0x0;
        worldNode1 = worldNode1->nextNode) {
      if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (iVar3 = *(int *)((int)worldNode1->runtimePayload + 8), iVar1 == *(int *)(iVar3 + 0xc))) {
        AVar5 = ArmyAssetRegistry_FindByIdCf(*(PckArmyAssetIdCatalog *)(iVar3 + 0xa0));
        armyDefinition1 = AVar5.eax;
        if (!AVar5.carry) {
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
      dVar2 = iStack_28 * 0x80 + iStack_24 * 0x100 + iStack_20 * 0x280 + iStack_1c * 0x100;
    }
    else if (trackClassId < 0x32) {
      dVar2 = iStack_28 * -0x100 + iStack_24 * 0x40 + 0x32000 + iStack_20 * 0x10 + iStack_1c * 0x80;
    }
    else if (trackClassId < 0x46) {
      dVar2 = iStack_28 * 0x80 + iStack_24 * 0x100 + iStack_20 * 0x20 + iStack_1c * 0x300;
    }
    else {
      dVar2 = iStack_28 * 0x100 + iStack_24 * 0x200 + iStack_20 * 0x180 + iStack_1c * 0x10;
    }
  }
  return dVar2;
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
  WorldInteractionFlags *pWVar1;
  UiNodeBase *pUVar2;
  bool bVar3;
  RichTextExtentRegs RVar4;
  TextResourceResolveEaxCf5 TVar5;
  InGameAction101FRootView43DC *uiRoot;
  
  pUVar2 = source->parent;
  uiRoot = (InGameAction101FRootView43DC *)source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    uiRoot = (InGameAction101FRootView43DC *)(uiRoot->rootUi0000).base.parent;
    pUVar2 = (uiRoot->rootUi0000).base.parent;
  }
  bVar3 = (bool)UiSelectableControl_IsSelectedCf((UiSelectableControl *)source);
  if (!bVar3) {
    UiPageStack_SetActiveIndex(0,&uiRoot->technologyPageStack0BD0);
    pWVar1 = &(uiRoot->worldRuntime0A30).interaction.interactionFlags48;
    *pWVar1 = *pWVar1 & 0xfffffff7;
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
  pWVar1 = &(uiRoot->worldRuntime0A30).interaction.interactionFlags48;
  *pWVar1 = *pWVar1 | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&uiRoot->worldRuntime0A30);
  UiPageStack_SetActiveIndex(8,&uiRoot->technologyPageStack0BD0);
  (uiRoot->textPanel0_1100).textResourceIdE4 =
       (uiRoot->worldRuntime0A30).activeFactionRuntimeIndex + 0x230017 +
       ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).header.
       titleTextResourceIndex * 0x10;
  TVar5 = TextResource_Resolve((uiRoot->textPanel0_1100).textResourceIdE4);
  RVar4 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,TVar5.eax,(uiRoot->textPanel0_1100).wrapWidthE0);
  (uiRoot->textPanel0_1100).measuredWidthB8 = RVar4.widthPixels + 6;
  (uiRoot->textPanel0_1100).measuredHeightBC = RVar4.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->textPanel0_1100).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->textPanel0_1100).scrollable);
  TVar5 = TextResource_Resolve((uiRoot->textPanel1_11EC).textResourceIdE4);
  RVar4 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,TVar5.eax,(uiRoot->textPanel1_11EC).wrapWidthE0);
  (uiRoot->textPanel1_11EC).measuredWidthB8 = RVar4.widthPixels + 6;
  (uiRoot->textPanel1_11EC).measuredHeightBC = RVar4.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->textPanel1_11EC).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->textPanel1_11EC).scrollable);
  TVar5 = TextResource_Resolve((uiRoot->textPanel2_1334).textResourceIdE4);
  RVar4 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,TVar5.eax,(uiRoot->textPanel2_1334).wrapWidthE0);
  (uiRoot->textPanel2_1334).measuredWidthB8 = RVar4.widthPixels + 6;
  (uiRoot->textPanel2_1334).measuredHeightBC = RVar4.heightPixels + 6;
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
  UiSelectableNodeEaxEcxCf9 UVar1;
  int parentNodeAddress;
  void *rootNodeCursor;
  
  parentNodeAddress = (int)(selectableControl->base).parent;
  rootNodeCursor = selectableControl;
  while (parentNodeAddress != -1) {
    rootNodeCursor = (((UiSelectableControl *)rootNodeCursor)->base).parent;
    parentNodeAddress = *(int *)((int)rootNodeCursor + 8);
  }
  UiSelectableGroup_SelectExclusive(3,&selectableControl->base);
  UVar1 = UiSelectableGroup_NoneVisibleSelectedCf(3);
  UiPageStack_SetActiveIndex
            (UVar1.controlIndexOrCount,
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
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_ToggleAndRefreshPage2(UiNodeBase *source)

{
  sdword *psVar1;
  UiNodeBase *pUVar2;
  void *pvVar3;
  UiPageIndex pageIndex;
  GameEntityRuntime *pGVar4;
  CommandPayloadDword04 modelOffset;
  StatusValueEaxCf5 SVar5;
  
  pUVar2 = source->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    source = (((UiRootNode *)source)->base).parent;
    pUVar2 = (((UiRootNode *)source)->base).parent;
  }
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    psVar1 = &(((UiRootNode *)((int)source + 0xa50))->base).rightOffset;
    *psVar1 = *psVar1 & 0xfffffff7;
    psVar1 = &(((UiRootNode *)((int)source + 0xbb0))->base).leftOffset;
    SVar5 = UiPageStack_ActivePageNotInListCf((UiPageStackControl *)psVar1);
    if (SVar5.valueOrError == 2) {
      pageIndex = 0;
    }
    else {
      pageIndex = 2;
    }
    UiPageStack_SetActiveIndex(pageIndex,(UiPageStackControl *)psVar1);
    if ((pageIndex == 2) &&
       (pGVar4 = SelectionInfo_GetFirstEntry(), pGVar4 != (GameEntityRuntime *)0x0)) {
      pvVar3 = (pGVar4->common).ownership.definitionOrClassRecord;
      InGameTechnologyPanel_ResetAndSelectCurrentArea((UiRootNode *)source);
      modelOffset = (int)pvVar3 - g_ModelRuntimeRebaseDelta;
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
  uint *puVar1;
  UiNodeBase *pUVar2;
  UiNodeBase *pUVar3;
  int iVar4;
  word *stream;
  TextResourceId resourceId;
  uint uVar5;
  int iVar6;
  uint uVar7;
  TextResourceResolveEaxCf5 TVar8;
  
  pUVar2 = source->parent;
  pUVar3 = source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    pUVar3 = pUVar3->parent;
    pUVar2 = pUVar3->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&pUVar3[0x6a].bottomOffset);
  resourceId = 0x216d;
  uVar7 = 0;
  uVar5 = 1;
  iVar6 = 0x50fa80;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar5] != 0) {
      TVar8 = TextResource_Resolve(resourceId);
      stream = TVar8.eax;
      iVar4 = *(int *)(iVar6 + 0x38);
      resourceId = resourceId + 1;
      puVar1 = (uint *)((int)&pUVar3->nodeFlags + g_UiSevenSlotSelectionControlOffsets[uVar7]);
      *puVar1 = *puVar1 & 0xfffffff7;
      uVar7 = uVar7 + 1;
      TVar8 = TextResource_Resolve(iVar4 + 0x2173);
      RichTextCommandStream_PatchPayloadBySelector(0,TVar8.eax,stream);
    }
    uVar5 = uVar5 + 1;
    iVar6 = iVar6 + 0x740;
  } while (uVar5 < 8);
  pUVar3[0x6e].left = uVar7 * 0x18;
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)&pUVar3[0x6b].rightAnchorQ31);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)&pUVar3[0x6b].rightAnchorQ31);
  for (; uVar7 < 7; uVar7 = uVar7 + 1) {
    puVar1 = (uint *)((int)&pUVar3->nodeFlags + g_UiSevenSlotSelectionControlOffsets[uVar7]);
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
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  UiNodeBase *pUVar3;
  SelectionPlayerRuntimeBlock *pSVar4;
  TextResourceId resourceId;
  uint uVar5;
  TextResourceResolveEaxCf5 TVar6;
  
  pUVar2 = source->parent;
  pUVar3 = source;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    pUVar3 = pUVar3->parent;
    pUVar2 = pUVar3->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&pUVar3[0x6a].bottomOffset);
  resourceId = 0x216d;
  uVar5 = 0;
  do {
    TVar6 = TextResource_Resolve(resourceId);
    pSVar4 = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[uVar5].playerRuntimeId];
    resourceId = resourceId + 1;
    puVar1 = (uint *)((int)&pUVar3->nodeFlags + g_UiSevenSlotSelectionControlOffsets[uVar5]);
    *puVar1 = *puVar1 & 0xfffffff7;
    uVar5 = uVar5 + 1;
    RichTextCommandStream_PatchPayloadBySelector(0,pSVar4->reserved80B0_8117 + 0x40,TVar6.eax);
    if (6 < uVar5) break;
  } while (uVar5 < g_FrontendPlayerRuntimeBlockCount);
  pUVar3[0x6e].left = uVar5 * 0x18;
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)&pUVar3[0x6b].rightAnchorQ31);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)&pUVar3[0x6b].rightAnchorQ31);
  for (; uVar5 < 7; uVar5 = uVar5 + 1) {
    puVar1 = (uint *)((int)&pUVar3->nodeFlags + g_UiSevenSlotSelectionControlOffsets[uVar5]);
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
  UiSelectableGroup_SelectExclusive(3,source);
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
  SelectionPlayerRuntimeBlock *pSVar1;
  dword dVar2;
  bool bVar3;
  ArmyRuntimeCreateEaxCf5 AVar4;
  
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
    dVar2 = g_UiCommandModeA;
    goto joined_r0x0056fa27;
  case 4:
    dVar2 = g_UiCommandModeB;
joined_r0x0056fa27:
    if (dVar2 == 0) {
      if (pointerRegionCode == 0x7fffffff) {
        return 0x18;
      }
      pSVar1 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      dVar2 = 0x17;
      if (pSVar1->primarySelectionEntityOffset8094 == 0) {
        AVar4 = ArmyRuntime_CreateInstanceFromAssetCf
                          (1,0,pointerWorldXQ12,pointerWorldYQ12,g_UiCommandModeGOwnerFactionIndex,
                           g_UiCommandModeGArmyAssetId,worldRuntime);
        if (!AVar4.carry) {
          bVar3 = ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)AVar4.eax,worldRuntime);
          if (bVar3) {
            dVar2 = 0x18;
          }
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar4.eax);
          return dVar2;
        }
        return 0x17;
      }
    }
    else {
      if (dVar2 == 1) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x19;
        }
        return 0x1a;
      }
      pSVar1 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
      dVar2 = 0x15;
      if (pSVar1->primarySelectionEntityOffset8094 == 0) {
        if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
          return 0x15;
        }
        return 0x16;
      }
    }
    bVar3 = ArmyRuntimeNode_DispatchTypedCallback
                      ((ArmyRuntimeSlot **)
                       (pSVar1->primarySelectionEntityOffset8094 +
                       (int)g_ArmyRuntimeRebaseBaseMinusOne),worldRuntime);
    if (bVar3) {
      return dVar2 + 1;
    }
    return dVar2;
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
  FieldGridAsset *pFVar1;
  longlong lVar2;
  longlong lVar3;
  PckArmyAssetIdCatalog lookupToken;
  dword dVar4;
  CommandPayloadDword04 CVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  FieldGridNearestPointRegsCf13 FVar10;
  
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
      FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
      lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
      uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
      g_UiCommandDragAnchorWorldXQ12 =
           (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff &
           0xfffff000;
      g_UiCommandDragAnchorWorldYQ12 = uVar8 * 2 + 0x3ff & 0xfffff000;
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
      FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
      lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
      uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
      g_UiCommandDragAnchorWorldXQ12 =
           (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff &
           0xfffff000;
      g_UiCommandDragAnchorWorldYQ12 = uVar8 * 2 + 0x3ff & 0xfffff000;
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
        FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
        lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
        uVar7 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
        uVar8 = (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar7) + 0x3ff &
                0xfffff000;
        uVar7 = uVar7 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedMatchingRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar7,uVar8);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x28e0,g_UiCommandAbsoluteSelectionIndex,uVar7,uVar8);
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
        FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
        lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
        uVar7 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
        uVar8 = (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar7) + 0x3ff &
                0xfffff000;
        uVar7 = uVar7 * 2 + 0x3ff & 0xfffff000;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          TerrainMaterialEdit_SeedNonTargetRegionReplacement
                    (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar7,uVar8);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x29b0,g_UiCommandAbsoluteSelectionIndex,uVar7,uVar8);
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
        FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
        lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
        lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
        uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
        g_UiCommandDragAnchorWorldXQ12 =
             (((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff &
             0xfffff000;
        g_UiCommandDragAnchorWorldYQ12 = uVar8 * 2 + 0x3ff & 0xfffff000;
        return;
      }
      g_UiCommandDragStartScreenX = 0x7fffffff;
      return;
    }
    if (g_UiCommandModeE == 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
      lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
      uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
      pFVar1 = mapControl->fieldGrid;
      iVar6 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff
                   ) >> 0xc;
      if (iVar6 < 0) {
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        return;
      }
      iVar9 = (int)(uVar8 * 2 + 0x3ff) >> 0xc;
      if (iVar9 < 0) {
        g_UiCommandTerrainMaskToggleValue = 0x20000000;
        return;
      }
      if (iVar6 < (int)pFVar1->gridWidth) {
        if (iVar9 < (int)pFVar1->gridHeight) {
          g_UiCommandTerrainMaskToggleValue =
               pFVar1->cells[iVar9 * pFVar1->gridWidth + iVar6].flagsAndMaterial &
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
    FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
    lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
    uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
    pFVar1 = mapControl->fieldGrid;
    iVar6 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff)
            >> 0xc;
    if (iVar6 < 0) {
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    iVar9 = (int)(uVar8 * 2 + 0x3ff) >> 0xc;
    if (iVar9 < 0) {
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    if (iVar6 < (int)pFVar1->gridWidth) {
      if (iVar9 < (int)pFVar1->gridHeight) {
        g_UiCommandTerrainMaskToggleValue =
             pFVar1->cells[iVar9 * pFVar1->gridWidth + iVar6].flagsAndMaterial &
             FIELD_CELL_FLUID_SOURCE_EXCLUDED ^ FIELD_CELL_FLUID_SOURCE_EXCLUDED;
        return;
      }
      g_UiCommandTerrainMaskToggleValue = 0x40000000;
      return;
    }
    g_UiCommandTerrainMaskToggleValue = 0x40000000;
    return;
  case 3:
    CVar5 = g_UiCommandModeGOwnerFactionIndex;
    lookupToken = g_UiCommandModeGArmyAssetId;
    dVar4 = g_UiCommandModeA;
    goto joined_r0x005701bf;
  case 4:
    CVar5 = 0;
    lookupToken = g_UiCommandMode4ArmyAssetId;
    dVar4 = g_UiCommandModeB;
joined_r0x005701bf:
    if (dVar4 == 0) {
      if (pointerRegionCode != 0x7fffffff) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetState8090(g_LocalPlayerRuntimeId,0,0,CVar5);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ec0,0,0,CVar5);
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
    if (dVar4 == 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      if (armyRuntimeUnderPointer != (ArmyRuntimeSlot *)0x0) {
        CVar5 = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ApplyEntryOrAll(g_LocalPlayerRuntimeId,0,0,CVar5);
          return;
        }
        InGameCommandQueue_AppendLocalPlayerCommand(0x2de0,0,0,CVar5);
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
        CVar5 = *(int *)(armyRuntimeUnderPointer->runtimeState48 + 8) -
                (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerRuntime_SetState8094(g_LocalPlayerRuntimeId,0,0,CVar5);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x2ef0,0,0,CVar5);
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
    FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
    lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
    uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
    pFVar1 = mapControl->fieldGrid;
    iVar6 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x3ff)
            >> 0xc;
    if (iVar6 < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    iVar9 = (int)(uVar8 * 2 + 0x3ff) >> 0xc;
    if (iVar9 < 0) {
      g_UiCommandCallerMaskHighBit = 0;
      return;
    }
    if (iVar6 < (int)pFVar1->gridWidth) {
      if ((int)pFVar1->gridHeight <= iVar9) {
        g_UiCommandCallerMaskHighBit = 0;
        return;
      }
      if ((pFVar1->cells[iVar9 * pFVar1->gridWidth + iVar6].flagsAndMaterial &
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
    FVar10 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar2 = (longlong)(int)FVar10.eax * 0x1c6e9c;
    lVar3 = (longlong)(int)FVar10.ecx * -0x20c8cc;
    uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
    g_UiCommandSelectionAnchorWorldXQ12 =
         ((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8;
    g_UiCommandSelectionAnchorWorldYQ12 = uVar8 * 2;
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
  longlong lVar1;
  longlong lVar2;
  dword dVar3;
  int iVar4;
  CommandPayloadDword04 payloadDword04;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  CommandPayloadDword08 CVar9;
  uint uVar10;
  int iVar11;
  InGameCommandPayloadTripletValue32 payloadValue;
  undefined4 *puVar12;
  WorldOwnerListNode100 *runtimeNode;
  CommandPayloadDword04 *pCVar13;
  bool bVar14;
  FieldGridNearestPointRegsCf13 FVar15;
  GameEntityRuntime *entry;
  
  if ((mapControl->runtimeFlags & 0x80) != 0) {
    puVar12 = (undefined4 *)&g_InGameSelectionInsertTripletDwords;
    for (iVar6 = 0x1a; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar12 = 0;
      puVar12 = puVar12 + 1;
    }
    runtimeNode = (WorldOwnerListNode100 *)mapControl->ownerListHead;
    iVar6 = mapControl->activeFactionRuntimeIndex;
    if (runtimeNode == (WorldOwnerListNode100 *)0x0) {
      return;
    }
    do {
      if ((((runtimeNode->runtimeFlags & 2) != 0) &&
          (entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
          (runtimeNode->runtimeFlags & 0x20) != 0)) &&
         (iVar6 == (entry->common).ownership.ownerIndex)) {
        payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
        bVar14 = WorldRuntimeNode_IsPositionInsideBoundsCf(runtimeNode,mapControl);
        if (bVar14) {
          bVar14 = SelectionInfo_FindEntryCf(entry);
          uVar7 = g_InGameSelectionInsertTripletDwordCount;
          if (((bVar14) &&
              (bVar14 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90), !bVar14))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionInsertTripletDwords + uVar7 * 4) = payloadValue, uVar7 < 0xb))
          {
            g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount + 1;
          }
        }
        else {
          bVar14 = SelectionInfo_FindEntryCf(entry);
          uVar7 = g_InGameSelectionRemoveTripletDwordCount;
          if (((!bVar14) &&
              (bVar14 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30), !bVar14))
             && (*(InGameCommandPayloadTripletValue32 *)
                  (&g_InGameSelectionRemoveTripletDwords + uVar7 * 4) = payloadValue, uVar7 < 0xb))
          {
            g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount + 1;
          }
        }
      }
      runtimeNode = runtimeNode->nextNode;
    } while (runtimeNode != (WorldOwnerListNode100 *)0x0);
    if (g_InGameSelectionRemoveTripletDwordCount != 0) {
      pCVar13 = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
      do {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                    (g_LocalPlayerRuntimeId,pCVar13[2],pCVar13[1],*pCVar13);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xb00,pCVar13[2],pCVar13[1],*pCVar13);
        }
        uVar7 = g_InGameSelectionRemoveTripletDwordCount;
        pCVar13 = pCVar13 + 3;
        g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount - 3;
      } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)uVar7);
    }
    if (g_InGameSelectionInsertTripletDwordCount == 0) {
      return;
    }
    pCVar13 = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
    do {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,pCVar13[2],pCVar13[1],*pCVar13);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xa60,pCVar13[2],pCVar13[1],*pCVar13);
      }
      uVar7 = g_InGameSelectionInsertTripletDwordCount;
      pCVar13 = pCVar13 + 3;
      g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount - 3;
    } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)uVar7);
    return;
  }
                    // WARNING: Switch is manually overridden
  switch(g_UiCommandModeG) {
  case 0:
    if (g_UiCommandModeC == 0) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      uVar7 = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        uVar7 = 0;
      }
      uVar7 = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX & uVar7 |
              (mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY) * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x1f70,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar7);
        return;
      }
      FieldGrid_ApplyPositiveCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,uVar7);
      return;
    }
    if (g_UiCommandModeC == 1) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      uVar7 = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        uVar7 = 0;
      }
      uVar7 = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX & uVar7 |
              (mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY) * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x2290,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar7);
        return;
      }
      FieldGrid_ApplyNegativeCellDeltas
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,uVar7);
      return;
    }
    if (g_UiCommandModeC == 2) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
      lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
      uVar10 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
      uVar7 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar10) + 0x3ff &
              0xfffff000;
      uVar10 = uVar10 * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2ae0,0,uVar10,uVar7);
        return;
      }
      FieldGrid_RebuildLocalInfluenceState(g_LocalPlayerRuntimeId,0,uVar10,uVar7);
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
      FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
      lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
      uVar10 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
      uVar7 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar10) + 0x3ff &
              0xfffff000;
      uVar10 = uVar10 * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x2770,g_UiCommandAbsoluteSelectionIndex,uVar10,uVar7);
        return;
      }
      FieldGrid_ApplyLocalCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandAbsoluteSelectionIndex,uVar10,uVar7);
      return;
    }
    break;
  case 2:
    if (g_UiCommandModeE == 0) {
      if (g_UiCommandDragStartScreenX == 0x7fffffff) {
        return;
      }
      uVar7 = 0xffff;
      if ((g_KeyboardStateMask & 0xf) != 0) {
        uVar7 = 0;
      }
      uVar10 = mapControl->extendedCoordinate168 - g_UiCommandDragStartScreenX;
      iVar6 = mapControl->extendedCoordinate16C - g_UiCommandDragStartScreenY;
      g_UiCommandDragStartScreenX = g_UiCommandDragStartScreenX + uVar10;
      g_UiCommandDragStartScreenY = g_UiCommandDragStartScreenY + iVar6;
      uVar7 = uVar10 & uVar7 | iVar6 * 0x10000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x3260,g_UiCommandDragAnchorWorldYQ12,g_UiCommandDragAnchorWorldXQ12,uVar7);
        return;
      }
      FieldGrid_ApplyEncodedCellUpdate
                (g_LocalPlayerRuntimeId,g_UiCommandDragAnchorWorldYQ12,
                 g_UiCommandDragAnchorWorldXQ12,uVar7);
      return;
    }
    if (g_UiCommandModeE != 1) {
      if (pointerRegionCode == 0x7fffffff) {
        return;
      }
      FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
      lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
      lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
      uVar10 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
      uVar7 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar10) + 0x3ff &
              0xfffff000;
      uVar10 = uVar10 * 2 + 0x3ff & 0xfffff000;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x32e0,g_UiCommandTerrainMaskToggleValue,uVar10,uVar7);
        return;
      }
      FieldGrid_ApplyMaskBFFFFFFF
                (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,uVar10,uVar7);
      return;
    }
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
    lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
    uVar10 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
    uVar7 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar10) + 0x3ff &
            0xfffff000;
    uVar10 = uVar10 * 2 + 0x3ff & 0xfffff000;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0x32a0,g_UiCommandTerrainMaskToggleValue,uVar10,uVar7);
      return;
    }
    FieldGrid_ApplyMaskDFFFFFFF
              (g_LocalPlayerRuntimeId,g_UiCommandTerrainMaskToggleValue,uVar10,uVar7);
    return;
  case 3:
    dVar3 = g_UiCommandModeA;
    goto joined_r0x00570b27;
  case 4:
    dVar3 = g_UiCommandModeB;
joined_r0x00570b27:
    if ((dVar3 != 0) && (dVar3 == 1)) {
      return;
    }
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    if ((g_CursorButtonState & 4) != 0) {
      g_UiCommandDragStartScreenX = mapControl->extendedCoordinate168;
      g_UiCommandDragStartScreenY = mapControl->extendedCoordinate16C;
      payloadDword04 = pointerY - g_UiCommandDragReferenceX;
      CVar9 = pointerX - g_UiCommandDragReferenceY;
      g_UiCommandDragReferenceX = g_UiCommandDragReferenceX + payloadDword04;
      g_UiCommandDragReferenceY = g_UiCommandDragReferenceY + CVar9;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x2f20,0,CVar9,payloadDword04);
        return;
      }
      SelectionPlayerRuntime_ReissuePrimarySelectionPosition
                (g_LocalPlayerRuntimeId,0,CVar9,payloadDword04);
      return;
    }
    iVar6 = (*g_PointerSetPosition)(g_UiCommandDragStartScreenY,g_UiCommandDragStartScreenX);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand(0x30f0,0,0,iVar6 << 6);
      return;
    }
    SelectionPlayerRuntime_AdvancePrimarySelectionCycle(g_LocalPlayerRuntimeId,0,0,iVar6 << 6);
    return;
  case 5:
    if (pointerRegionCode == 0x7fffffff) {
      return;
    }
    FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
    lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
    uVar10 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
    uVar7 = (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar10) + 0x3ff &
            0xfffff000;
    uVar10 = uVar10 * 2 + 0x3ff & 0xfffff000;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
        SESSION_NETWORK_ROLE_LOCAL) {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0x3320,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,uVar10,uVar7);
      return;
    }
    FieldGrid_ApplyCallerMask
              (g_LocalPlayerRuntimeId,g_UiCommandModeF | g_UiCommandCallerMaskHighBit,uVar10,uVar7);
    return;
  }
  if ((pointerRegionCode != 0x7fffffff) && (g_UiCommandSelectionAnchorWorldXQ12 != 0x7fffffff)) {
    FVar15 = FieldGrid_GetNearestTerrainPoint(pointerX,pointerY,mapControl->fieldGrid);
    lVar1 = (longlong)(int)FVar15.eax * 0x1c6e9c;
    lVar2 = (longlong)(int)FVar15.ecx * -0x20c8cc;
    uVar7 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
    iVar5 = ((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - uVar7;
    iVar6 = uVar7 * 2;
    LOCK();
    UNLOCK();
    LOCK();
    UNLOCK();
    iVar4 = g_UiCommandSelectionAnchorWorldXQ12;
    if (g_UiCommandSelectionCurrentWorldXQ12 < g_UiCommandSelectionAnchorWorldXQ12) {
      iVar4 = g_UiCommandSelectionCurrentWorldXQ12;
      g_UiCommandSelectionCurrentWorldXQ12 = g_UiCommandSelectionAnchorWorldXQ12;
    }
    iVar8 = g_UiCommandSelectionCurrentWorldYQ12;
    iVar11 = g_UiCommandSelectionAnchorWorldYQ12;
    if (g_UiCommandSelectionCurrentWorldYQ12 < g_UiCommandSelectionAnchorWorldYQ12) {
      iVar8 = g_UiCommandSelectionAnchorWorldYQ12;
      iVar11 = g_UiCommandSelectionCurrentWorldYQ12;
    }
    uVar7 = iVar4 + 0x3ffU & 0xfffff000;
    CVar9 = iVar11 + 0x3ffU & 0xfffff000;
    uVar10 = g_UiCommandSelectionCurrentWorldXQ12 + 0x3ffU & 0xfffff000;
    g_UiCommandSelectionCurrentWorldXQ12 = iVar5;
    g_UiCommandSelectionCurrentWorldYQ12 = iVar6;
    if ((int)uVar7 <= (int)uVar10) {
      for (; (int)CVar9 <= (int)(iVar8 + 0x3ffU & 0xfffff000); CVar9 = CVar9 + 0x1000) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_RemoveRange(g_LocalPlayerRuntimeId,uVar10,CVar9,uVar7);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d40,uVar10,CVar9,uVar7);
        }
      }
    }
    iVar6 = g_UiCommandSelectionAnchorWorldXQ12;
    iVar4 = g_UiCommandSelectionCurrentWorldXQ12;
    if (g_UiCommandSelectionCurrentWorldXQ12 < g_UiCommandSelectionAnchorWorldXQ12) {
      iVar6 = g_UiCommandSelectionCurrentWorldXQ12;
      iVar4 = g_UiCommandSelectionAnchorWorldXQ12;
    }
    iVar5 = g_UiCommandSelectionCurrentWorldYQ12;
    iVar8 = g_UiCommandSelectionAnchorWorldYQ12;
    if (g_UiCommandSelectionCurrentWorldYQ12 < g_UiCommandSelectionAnchorWorldYQ12) {
      iVar5 = g_UiCommandSelectionAnchorWorldYQ12;
      iVar8 = g_UiCommandSelectionCurrentWorldYQ12;
    }
    uVar7 = iVar6 + 0x3ffU & 0xfffff000;
    CVar9 = iVar8 + 0x3ffU & 0xfffff000;
    uVar10 = iVar4 + 0x3ffU & 0xfffff000;
    if ((int)uVar7 <= (int)uVar10) {
      for (; (int)CVar9 <= (int)(iVar5 + 0x3ffU & 0xfffff000); CVar9 = CVar9 + 0x1000) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          PlayerPairList_InsertRange(g_LocalPlayerRuntimeId,uVar10,CVar9,uVar7);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1d00,uVar10,CVar9,uVar7);
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
  dword dVar1;
  
  dVar1 = g_UiCommandModeG;
  worldRuntime->runtimeFlags = worldRuntime->runtimeFlags & 0xffffff7f;
                    // WARNING: Switch is manually overridden
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
    dVar1 = g_UiCommandModeA;
    goto joined_r0x00570ef7;
  case 4:
    dVar1 = g_UiCommandModeB;
joined_r0x00570ef7:
    if ((dVar1 == 0) || (dVar1 != 1)) {
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
  WorldRuntimeFlags *pWVar1;
  WorldRuntimeContext *node;
  GraphicsTextureSet *pGVar2;
  ArmyAssetRecordPrefix *pAVar3;
  byte *pbVar4;
  InGameRuntimeRootImageC3E4 *root;
  dword dVar5;
  int iVar6;
  GraphicsTextureSourceAsset *pGVar7;
  InGameNotificationQueueRecord20 *pIVar8;
  TerrainDirectionRecord *pTVar9;
  FieldGridCell *gridCell1;
  ArmyAssetRecordPrefix **ppAVar10;
  StatusValueEaxCf5 SVar11;
  ArmyRegistryIdEaxCf5_5719f0 AVar12;
  ArmyRegistryIdEaxCf5_571c30 AVar13;
  FieldGridAsset *fieldGrid1;
  
  dVar5 = g_UiCommandModeG;
  root = g_InGameRuntimeRoot;
  if ((commandArg3 & 4) == 0) {
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      pbVar4 = g_InGameRuntimeRoot->opaque9A74_9B4B;
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 5;
      pWVar1 = &(g_InGameRuntimeRoot->worldRuntime0A30).runtimeFlags;
      *pWVar1 = *pWVar1 | 0x400000;
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGPrimaryPageIndices[dVar5],(UiPageStackControl *)(pbVar4 + 0x18));
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGSecondaryPageIndices[dVar5],
                 (UiPageStackControl *)root->opaque9EE0_9FAB);
      UiPageStack_SetActiveIndex
                (g_UiCommandModeGTertiaryPageIndices[dVar5],
                 (UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
      SVar11 = UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
      if (SVar11.valueOrError == 0) {
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
      (*g_UiCommandModeGHandlers[dVar5])
                (root->opaque0058_017B + g_UiCommandModeGControlOffsets[dVar5] + -0x58);
      pIVar8 = root->notificationQueue9E60;
      for (iVar6 = 0x20; iVar6 != 0; iVar6 = iVar6 + -1) {
        pIVar8->notificationMovieId00 = 0;
        pIVar8 = (InGameNotificationQueueRecord20 *)&pIVar8->priority04;
      }
      Movie_Close();
      dVar5 = g_UiCommandAbsoluteSelectionIndex;
      pGVar7 = g_InGamePanelTextureSource;
      if (root->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE) {
        root->sessionNotificationInteractionState9B4C = NONE;
      }
      pGVar2 = g_TerrainMaterialTextureSets[dVar5];
      root->observedSessionNotificationValue9B50 = (dword)pGVar7;
      pGVar7 = (GraphicsTextureSourceAsset *)0x0;
      if (pGVar2 != (GraphicsTextureSet *)0x0) {
        pGVar7 = pGVar2->entries[0].sourceAsset;
      }
      root->notificationPlaybackCompletionCode9B54 = 0x25;
      *(GraphicsTextureSourceAsset **)(root->opaque9B58_9E3F + 0xb8) = pGVar7;
      UiCommandMatrix_SelectIndex(g_UiCommandAbsoluteSelectionIndex,(UiNodeBase *)root);
      AVar12 = ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(g_UiCommandModeGArmyAssetId);
      g_UiCommandModeGArmyAssetId = AVar12.eax;
      dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x1cc) = dVar5;
      AVar13 = ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(g_UiCommandMode4ArmyAssetId);
      g_UiCommandMode4ArmyAssetId = AVar13.eax;
      dVar5 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
      *(dword *)(root->opaque9B58_9E3F + 0x284) = dVar5;
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
      iVar6 = 0x100;
      pTVar9 = g_TerrainDirectionRecordTable256;
      do {
        pTVar9->angleAComponent0ScaledQ28 = 0;
        pTVar9->angleAComponent1ScaledQ28 = 0;
        pTVar9->angleBComponent0ScaledQ28 = 0;
        pTVar9 = pTVar9 + 1;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
      fieldGrid1 = (root->worldRuntime0A30).fieldGrid;
      iVar6 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
      gridCell1 = fieldGrid1->cells;
      do {
        gridCell1->armyRuntimeSavedOffset6C = 0;
        gridCell1->resourceExtractionDescriptor7C = 0;
        gridCell1 = gridCell1 + 1;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
    }
  }
  else if ((g_UiCommandRuntimeFlags & 4) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffffffa;
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x18));
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)root->opaque9EE0_9FAB);
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
    SVar11 = UiPageStack_ActivePageNotInListCf(&root->optionalUiPageStack40AC);
    if (SVar11.valueOrError == 0) {
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
    pWVar1 = &(root->worldRuntime0A30).runtimeFlags;
    *pWVar1 = *pWVar1 | 0x400;
    g_UiRootCallbacks_0054FBC0.keyboardFallbackCf = EndGameResultsUiRuntime_DispatchCommandByFlagsCf
    ;
    ppAVar10 = g_ArmyAssetRecordRegistry;
    iVar6 = 0x300;
    do {
      pAVar3 = *ppAVar10;
      if (pAVar3 != (ArmyAssetRecordPrefix *)0x0) {
        (*g_MemoryApi.free)((void *)pAVar3[2].byteSize);
        pAVar3[2].byteSize = 0;
      }
      ppAVar10 = ppAVar10 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
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
    pWVar1 = &(root->worldRuntime0A30).runtimeFlags;
    *pWVar1 = *pWVar1 & 0xffbfffff;
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
  InGameRuntimeRootImageC3E4 *pIVar1;
  StatusValueEaxCf5 SVar2;
  
  pIVar1 = g_InGameRuntimeRoot;
  SVar2 = FieldGrid_SaveAssetImageFromRuntimeStateCf
                    ((dword *)(g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid);
  if (SVar2.carry) {
    (*g_FatalErrorRuntimeDispatchCf)(SVar2.valueOrError,true);
  }
  SVar2 = InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf
                    ((InGameLevelSaveWorldView *)&pIVar1->worldRuntime0A30);
  if (SVar2.carry) {
    (*g_FatalErrorRuntimeDispatchCf)(SVar2.valueOrError,true);
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
  sdword *psVar1;
  int *piVar2;
  UiNodeBase *pUVar3;
  int iVar4;
  uint uVar5;
  CommandPayloadDword04 packedState;
  uint uVar6;
  UiAnchorFractionQ31 *pUVar7;
  bool bVar8;
  UiSelectableNodeEaxEcxCf9 UVar9;
  
  pUVar3 = source->parent;
  while (pUVar3 != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    pUVar3 = source->parent;
  }
  psVar1 = &source[0x60].right;
  RichTextCommandStream_CopyToNarrowCf
            (0x30,g_UiSevenSlotCommandPayloadText.textBytes,(word *)&source[0x61].rightAnchorQ31);
  UVar9 = UiSelectableGroup_NoneVisibleSelectedCf(3);
  iVar4 = (int)UVar9.node - (int)psVar1;
  if (iVar4 == 0x1ec) {
    uVar5 = 0;
    packedState = 0;
    uVar6 = 0x100;
    do {
      piVar2 = g_UiSevenSlotSelectionControlOffsets + uVar5;
      uVar6 = uVar6 * 2;
      uVar5 = uVar5 + 1;
      bVar8 = (bool)UiSelectableControl_IsSelectedCf
                              ((UiSelectableControl *)(*piVar2 + -0x1c98 + (int)psVar1));
      if (bVar8) {
        packedState = packedState | uVar6;
      }
    } while (uVar5 < 7);
  }
  else if (iVar4 == 0x24c) {
    uVar5 = 0;
    packedState = 0;
    uVar6 = 0x8000;
    do {
      piVar2 = g_UiSevenSlotSelectionControlOffsets + uVar5;
      uVar6 = uVar6 * 2;
      uVar5 = uVar5 + 1;
      bVar8 = (bool)UiSelectableControl_IsSelectedCf
                              ((UiSelectableControl *)(*piVar2 + -0x1c98 + (int)psVar1));
      if (bVar8) {
        packedState = packedState | uVar6;
      }
    } while (uVar5 < 7);
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
  pUVar7 = &source[0x61].rightAnchorQ31;
  for (iVar4 = 0x18; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pUVar7 = 0;
    pUVar7 = pUVar7 + 1;
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
  byte *pbVar1;
  GraphicsTextureSourceAsset *pGVar2;
  uint uVar3;
  ModelRuntimeSlot *pMVar4;
  UiCommandRuntimeRecordPrefix *definitionNode;
  int iVar5;
  dword dVar6;
  EnergyDemandQ4 EVar7;
  word *pwVar8;
  uint uVar9;
  ArmyAssetRecordPrefix *pAVar10;
  int iVar11;
  int iVar12;
  ModelDefinitionHierarchyNodeAddress32 definitionNode_00;
  ModelLinkedDefinitionBranchView18 *linkedDefinitionListView;
  InGameRuntimeRootImageC3E4 *firstNode;
  GameEntityRuntime *entityRuntime1;
  int *piVar13;
  GameEntityRuntime **ppGVar14;
  word *pwVar15;
  bool bVar16;
  ArmyRegistryEaxCf5_51b6d0 AVar17;
  FatalErrorEaxCf5 FVar18;
  TextResourceResolveEaxCf5 TVar19;
  ModelDefinitionLookupEaxCf5 MVar20;
  ModelRuntimeSlot *selectedModelRuntime;
  ModelRuntimeSlot *selectedModelRuntimeTail;
  ModelRuntimeSlot *selectedModelRuntimeTail2;
  
  definitionNode = g_UiHoverSelectionRecord;
  firstNode = g_InGameRuntimeRoot;
  if (g_InGameRuntimeRoot == (InGameRuntimeRootImageC3E4 *)0x0) {
    return;
  }
  iVar11 = 0x20;
  iVar12 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  iVar5 = 0;
  entityRuntime1 = (GameEntityRuntime *)0x0;
  ppGVar14 = g_SelectionInfoEntitySlots->entries;
  do {
    if (*ppGVar14 != (GameEntityRuntime *)0x0) {
      iVar5 = iVar5 + 1;
      entityRuntime1 = *ppGVar14;
    }
    ppGVar14 = ppGVar14 + 1;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  stack = &g_InGameRuntimeRoot->selectionDetailPageStack9FAC;
  if (g_UiHoverSelectionRecord == (UiCommandRuntimeRecordPrefix *)0x0) {
    if (iVar5 == 1) {
      if (iVar12 == (entityRuntime1->common).ownership.ownerIndex) {
        piVar13 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        iVar12 = *piVar13;
        bVar16 = FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf
                           ((RuntimeToken)piVar13,
                            (g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId)
        ;
        if (!bVar16) {
          UiNodeList_UnsuppressActionId(0x1010,(UiNodeBase *)firstNode);
          iVar5 = 0x1c;
          do {
            bVar16 = Technology_IsAvailableForFactionCf
                               (*(PckTechnologyIdCatalog *)(iVar12 + 0x1c4 + iVar5 * 4),
                                (entityRuntime1->common).ownership.ownerIndex);
            if (bVar16)
            goto InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
        }
        UiNodeList_SuppressActionId(0x1010,(UiNodeBase *)firstNode);
InGameSelectionDetailPanel_Rebuild_ContinueWithSingleOwnedSelectionDetails:
        AVar17 = ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId)
        ;
        FVar18 = (*g_FatalErrorPrimaryDispatchCf)((dword)AVar17.eax,AVar17.carry);
        UiPageStack_SetActiveIndex(1,stack);
        dVar6 = *(dword *)(FVar18.eax + 0x1c);
        iVar12 = ModelRuntimeHierarchy_SumMetric3C((int *)entityRuntime1);
        firstNode->selectionDetailArmyAssetValueA060 = dVar6;
        firstNode->selectionDetailEntityA068 = entityRuntime1;
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar12,g_InGameSelectionDetailArmourTextUtf16
                  );
        uVar9 = ModelRuntime_QueryActiveHierarchyMetric((ArmyRuntimeSlot *)entityRuntime1);
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar9 >> 4,
                   g_InGameSelectionDetailEnergyTextUtf16);
        *(int *)(firstNode->opaqueA06C_C3E3 + 0x54) = *(int *)(FVar18.eax + 4) + 0x18002c;
        TVar19 = TextResource_Resolve
                           (*(int *)(*(int *)(entityRuntime1->common).ownership.
                                             definitionOrClassRecord + 4) + 0x18004f);
        pwVar8 = TVar19.eax;
        pwVar15 = g_InGameSelectionDetailNameTextUtf16;
        for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
          *pwVar15 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar15 = pwVar15 + 1;
        }
        TVar19 = TextResource_Resolve(0x18004e);
        pwVar8 = TVar19.eax;
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName0TextUtf16,pwVar8);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName1TextUtf16,pwVar8);
        RichTextCommandStream_CopyExpandedCf
                  (0x80,g_InGameSelectionDetailWeaponName2TextUtf16,pwVar8);
        g_InGameSelectionDetailTextSlot05Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot05Utf16[1] = 0;
        g_InGameSelectionDetailTextSlot09Utf16[0] = 0x2d;
        g_InGameSelectionDetailTextSlot09Utf16[1] = 0;
        selectedModelRuntime = (entityRuntime1->common).ownership.definitionOrClassRecord;
        if (((selectedModelRuntime->classState).classStateEC & 0x40) != 0) {
          AVar17 = ArmyAssetRegistry_FindByIdCf
                             ((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
          iVar12 = *(int *)selectedModelRuntime->reserved100_117;
          *(ArmySelectionDetailTemplateVariantIndex *)(firstNode->opaqueA06C_C3E3 + 0x54) =
               (AVar17.eax)->selectionDetailTemplateVariantIndex + 0x18003c;
          TVar19 = TextResource_Resolve(iVar12 * 2 + 0x300000);
          RichTextCommandStream_CopyExpandedCf
                    (0x80,g_InGameSelectionDetailTextSlot09Utf16,TVar19.eax);
        }
        if (selectedModelRuntime->attachmentCount0C != 0) {
          pMVar4 = selectedModelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
          if (pMVar4 != (ModelRuntimeSlot *)0x0) {
            TVar19 = TextResource_Resolve
                               (((pMVar4->definitionOrSavedId).definition)->flags + 0x18004f);
            pwVar8 = TVar19.eax;
            pwVar15 = g_InGameSelectionDetailWeaponName0TextUtf16;
            for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
              *pwVar15 = *pwVar8;
              pwVar8 = pwVar8 + 1;
              pwVar15 = pwVar15 + 1;
            }
          }
          selectedModelRuntimeTail = (entityRuntime1->common).ownership.definitionOrClassRecord;
          if (1 < selectedModelRuntimeTail->attachmentCount0C) {
            pMVar4 = selectedModelRuntimeTail->attachments140[1].childModelRuntimeOrSavedOffset00;
            if (pMVar4 != (ModelRuntimeSlot *)0x0) {
              TVar19 = TextResource_Resolve
                                 (((pMVar4->definitionOrSavedId).definition)->flags + 0x18004f);
              pwVar8 = TVar19.eax;
              pwVar15 = g_InGameSelectionDetailWeaponName1TextUtf16;
              for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
                *pwVar15 = *pwVar8;
                pwVar8 = pwVar8 + 1;
                pwVar15 = pwVar15 + 1;
              }
            }
            selectedModelRuntimeTail2 = (entityRuntime1->common).ownership.definitionOrClassRecord;
            if ((2 < selectedModelRuntimeTail2->attachmentCount0C) &&
               (pMVar4 = selectedModelRuntimeTail2->attachments140[2].
                         childModelRuntimeOrSavedOffset00, pMVar4 != (ModelRuntimeSlot *)0x0)) {
              TVar19 = TextResource_Resolve
                                 (((pMVar4->definitionOrSavedId).definition)->flags + 0x18004f);
              pwVar8 = TVar19.eax;
              pwVar15 = g_InGameSelectionDetailWeaponName2TextUtf16;
              for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
                *pwVar15 = *pwVar8;
                pwVar8 = pwVar8 + 1;
                pwVar15 = pwVar15 + 1;
              }
            }
          }
        }
        piVar13 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        iVar12 = *(int *)(*piVar13 + 0x4c);
        if (iVar12 == 0x16) {
          if (piVar13[0x2b] != 1) {
            return;
          }
          AVar17 = ArmyAssetRegistry_FindByIdCf(piVar13[0x18]);
          pAVar10 = AVar17.eax;
        }
        else if (iVar12 == 0xb) {
          if (piVar13[0x2e] != 1) {
            return;
          }
          AVar17 = ArmyAssetRegistry_FindByIdCf(piVar13[0x18]);
          pAVar10 = AVar17.eax;
        }
        else {
          if (iVar12 != 0xd) {
            if (iVar12 != 0xe) {
              return;
            }
            (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,piVar13[0x18],
                       g_InGameSelectionDetailWeaponName0TextUtf16);
            return;
          }
          if (piVar13[0x2e] != 1) {
            return;
          }
          AVar17 = ArmyAssetRegistry_FindByIdCf(piVar13[0x18]);
          pAVar10 = AVar17.eax;
        }
        linkedDefinitionListView =
             (ModelLinkedDefinitionBranchView18 *)pAVar10->rootNodeOffsetOrPointer;
        if (pAVar10->selectionDetailTemplateVariantIndex < 8) {
          *(ArmySelectionDetailTemplateVariantIndex *)(firstNode->opaqueA06C_C3E3 + 0x54) =
               *(int *)(firstNode->opaqueA06C_C3E3 + 0x54) +
               pAVar10->selectionDetailTemplateVariantIndex;
        }
        MVar20 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                            (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
        TVar19 = TextResource_Resolve((MVar20.modelDefinition)->flags + 0x18004f);
        pwVar8 = TVar19.eax;
        pwVar15 = g_InGameSelectionDetailTextSlot05Utf16;
        goto InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots;
      }
    }
    else if ((iVar5 != 0) && (iVar12 == (entityRuntime1->common).ownership.ownerIndex)) {
      UiPageStack_SetActiveIndex(2,stack);
      iVar5 = 0x20;
      piVar13 = &g_InGameSelectionDetailControlOffsetTable32;
      iVar12 = 0xc;
      ppGVar14 = g_SelectionInfoEntitySlots->entries;
      do {
        entityRuntime1 = *ppGVar14;
        if ((entityRuntime1 != (GameEntityRuntime *)0x0) && (iVar12 != 0)) {
          iVar11 = *piVar13;
          *(GameEntityRuntime **)(firstNode->opaque0058_017B + iVar11 + 4) = entityRuntime1;
          AVar17 = ArmyAssetRegistry_FindByIdCf
                             ((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
          *(PckArmyAssetIdCatalog *)(firstNode->opaque0058_017B + iVar11 + -4) =
               AVar17.eax[1].registryId;
          firstNode = (InGameRuntimeRootImageC3E4 *)((int)firstNode + (iVar11 - *piVar13));
          iVar12 = iVar12 + -1;
          piVar13 = piVar13 + 1;
        }
        ppGVar14 = ppGVar14 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      for (; iVar12 != 0; iVar12 = iVar12 + -1) {
        pbVar1 = firstNode->opaque0058_017B + *piVar13 + -4;
        pbVar1[0] = 0;
        pbVar1[1] = 0;
        pbVar1[2] = 0;
        pbVar1[3] = 0;
        piVar13 = piVar13 + 1;
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
    pGVar2 = definitionNode->textureSource;
    dVar6 = ArmyAssetHierarchy_SumFactionUnlockedArmour
                      ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    *(GraphicsTextureSourceAsset **)(firstNode->opaqueA06C_C3E3 + 0x128) = pGVar2;
    uVar9 = definitionNode->buildXeniteCostQ4;
    uVar3 = definitionNode->buildDurationQ5;
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,dVar6,g_InGameSelectionDetailArmourTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar9 >> 4,
               g_InGameSelectionDetailBuildXeniteCostTextUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar3 >> 5,
               g_InGameSelectionDetailBuildTimeTextUtf16);
    EVar7 = ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
                      ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)definitionNode);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,EVar7 >> 4,g_InGameSelectionDetailEnergyTextUtf16
              );
    iVar12 = *(int *)(definitionNode->reserved00_07 + 4) + 0x180045;
    *(int *)(firstNode->opaqueA06C_C3E3 + 0x184) = iVar12;
    *(int *)(firstNode->opaqueA06C_C3E3 + 0x1070) = iVar12;
    linkedDefinitionListView = *(ModelLinkedDefinitionBranchView18 **)definitionNode->reserved0C_1B;
    MVar20 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                        (ModelLinkedDefinitionListAddress32)linkedDefinitionListView);
    TVar19 = TextResource_Resolve((MVar20.modelDefinition)->flags + 0x18004f);
    pwVar8 = TVar19.eax;
    pwVar15 = g_InGameSelectionDetailNameTextUtf16;
InGameSelectionDetailPanel_Rebuild_CopyResolvedDefinitionNamesIntoDetailSlots:
    for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
      *pwVar15 = *pwVar8;
      pwVar8 = pwVar8 + 1;
      pwVar15 = pwVar15 + 1;
    }
    TVar19 = TextResource_Resolve(0x18004e);
    pwVar8 = TVar19.eax;
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName0TextUtf16,pwVar8);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName1TextUtf16,pwVar8);
    RichTextCommandStream_CopyExpandedCf(0x80,g_InGameSelectionDetailWeaponName2TextUtf16,pwVar8);
    if (linkedDefinitionListView->childListCount != 0) {
      MVar20 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                         ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                          linkedDefinitionListView->childList0Address);
      TVar19 = TextResource_Resolve((MVar20.modelDefinition)->flags + 0x18004f);
      pwVar8 = TVar19.eax;
      pwVar15 = g_InGameSelectionDetailWeaponName0TextUtf16;
      for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
        *pwVar15 = *pwVar8;
        pwVar8 = pwVar8 + 1;
        pwVar15 = pwVar15 + 1;
      }
      if (1 < linkedDefinitionListView->childListCount) {
        MVar20 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                            linkedDefinitionListView->childList1Address);
        TVar19 = TextResource_Resolve((MVar20.modelDefinition)->flags + 0x18004f);
        pwVar8 = TVar19.eax;
        pwVar15 = g_InGameSelectionDetailWeaponName1TextUtf16;
        for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
          *pwVar15 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar15 = pwVar15 + 1;
        }
        if (2 < linkedDefinitionListView->childListCount) {
          MVar20 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                             ((firstNode->worldRuntime0A30).activeFactionRuntimeIndex,
                              linkedDefinitionListView->childList2Address);
          TVar19 = TextResource_Resolve((MVar20.modelDefinition)->flags + 0x18004f);
          pwVar8 = TVar19.eax;
          pwVar15 = g_InGameSelectionDetailWeaponName2TextUtf16;
          for (iVar12 = 0x40; iVar12 != 0; iVar12 = iVar12 + -1) {
            *pwVar15 = *pwVar8;
            pwVar8 = pwVar8 + 1;
            pwVar15 = pwVar15 + 1;
          }
        }
      }
    }
  }
  return;
}

