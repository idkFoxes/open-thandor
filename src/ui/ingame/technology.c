/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/technology.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/technology.h>

/* Implementation ownership: ui/ingame/technology. */

/* Address: 0x0056AE70.
   Ownership: ui/ingame/technology.
   Purpose: Recovered action-table target INGAME_PAGE10[20],INGAME_PAGE10[21],INGAME_PAGE10[22],INGAME_PAGE10[23],I
   NGAME_PAGE10[24],INGAME_PAGE10[25],INGAME_PAGE10[26] (0x1014,0x1015,0x1016,0x1017,0x1018,0x1019,0x101A).
   Local calls: InGameTechnologyPanel_Rebuild.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl)

{
  UiRootNode *inGameRoot;
  bool sourceIsSelected;
  bool bVar1;
  UiNodeBase *parentCursor;
  
  parentCursor = (selectableControl->base).parent;
  inGameRoot = (UiRootNode *)selectableControl;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    inGameRoot = (UiRootNode *)(inGameRoot->base).parent;
    parentCursor = (inGameRoot->base).parent;
  }
  bVar1 = (bool)UiSelectableControl_IsSelectedCf(selectableControl);
  if (bVar1) {
    UiSelectableGroup_SelectExclusive(7,&selectableControl->base);
  }
  InGameTechnologyPanel_Rebuild(inGameRoot);
  return;
}


/* Address: 0x0056B450.
   Ownership: ui/ingame/technology.
   Purpose: Releases technology-panel focus, clears all seven area selections, maps the selected entity technology
   area to one control, resets detail state, and rebuilds the panel.
   Local calls: InGameTechnologyPanel_Rebuild.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input], SelectionInfo_GetFirstEntry
   [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot)

{
  sdword *psVar1;
  UiNodeBase **ppUVar2;
  UiAnchorFractionQ31 *pUVar3;
  UiNodeFlags *pUVar4;
  uint *puVar5;
  int *piVar6;
  GameEntityRuntime *pGVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  
  psVar1 = &inGameRoot[0x1e].base.rightOffset;
  *psVar1 = *psVar1 | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&inGameRoot[0x1d].base.rightAnchorQ31);
  pGVar7 = SelectionInfo_GetFirstEntry();
  piVar6 = (pGVar7->common).ownership.definitionOrClassRecord;
  psVar1 = &inGameRoot[0x3e].base.layoutWidth;
  *psVar1 = *psVar1 & 0xfffffffd;
  inGameRoot[0x3f].callbacks = (UiRootCallbacks *)((uint)inGameRoot[0x3f].callbacks & 0xfffffffd);
  ppUVar2 = &inGameRoot[0x41].base.parent;
  *ppUVar2 = (UiNodeBase *)((uint)*ppUVar2 & 0xfffffffd);
  psVar1 = &inGameRoot[0x42].base.right;
  *psVar1 = *psVar1 & 0xfffffffd;
  psVar1 = &inGameRoot[0x43].base.rightOffset;
  *psVar1 = *psVar1 & 0xfffffffd;
  pUVar3 = &inGameRoot[0x44].base.rightAnchorQ31;
  *pUVar3 = *pUVar3 & 0xfffffffd;
  pUVar4 = &inGameRoot[0x45].base.nodeFlags;
  *pUVar4 = *pUVar4 & ~UI_NODE_PREFERRED_FOCUS_TARGET;
  if ((piVar6[0x3b] & 0xc0U) != 0) {
    iVar8 = *piVar6;
    uVar10 = 0xffffffff;
    iVar9 = 0x1c;
    do {
      uVar10 = uVar10 + 1;
      if (6 < uVar10) {
        uVar10 = 0;
      }
      if (*(int *)(iVar8 + 0x1c8) == piVar6[0x40]) {
        puVar5 = (uint *)((int)&inGameRoot->rootFlags + *(int *)(uVar10 * 4 + 0x562d68));
        *puVar5 = *puVar5 | 2;
        break;
      }
      iVar8 = iVar8 + 4;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
  }
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  InGameTechnologyPanel_Rebuild(inGameRoot);
  return;
}


/* Address: 0x00569DF0.
   Ownership: ui/ingame/technology.
   Purpose: Collects up to 48 matching runtime records, chooses a compact grid, updates catalog controls from each
   record's textureSource and catalogDisplayValueQ4, suppresses unused controls, and relayouts the container.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], SelectionInfo_CollectCapabilityFlags
   [gameplay/selection/runtime], ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
   [assets/model/definitions], FactionRuntime_HasArmyAssetOrActiveStructureCf [gameplay/faction/runtime],
   ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf [assets/army/catalog], UiGrid_ComputeDimensionsPacked
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx UiCatalogGroup48_RebuildGrid(UiNodeBase *node)

{
  UiNodeFlags *pUVar1;
  WorldOwnerListNode100 *pWVar2;
  UiCommandRuntimeRecordPrefix *pUVar3;
  sdword *psVar4;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  uint uVar5;
  ArmyAssetRecordPrefix *armyAssetRecord;
  dword dVar6;
  GraphicsTextureSourceAsset *pGVar7;
  int iVar8;
  uint itemCount;
  int factionIndex;
  int iVar9;
  UiCommandRuntimeRecordPrefix **ppUVar10;
  ArmyAssetRecordPrefix **ppAVar11;
  bool bVar12;
  UiGridDimensionsEdxEax8 UVar13;
  ArmyBuildXeniteCostQ4 AVar14;
  
  inGameUiGridView = (InGameRuntimeUiGridViewC3E4 *)UiNode_GetRoot(node);
  pWVar2 = (inGameUiGridView->worldRuntime0A30).ownerListHead;
  uVar5 = SelectionInfo_CollectCapabilityFlags();
  if (uVar5 == 0) {
    for (; pWVar2 != (WorldOwnerListNode100 *)0x0; pWVar2 = pWVar2->nextNode) {
      if ((pWVar2->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (iVar8 = *(int *)pWVar2->runtimePayload,
         *(int *)(*(int *)((int)pWVar2->runtimePayload + 8) + 0xc) == factionIndex)) {
        if (*(int *)(iVar8 + 0x4c) == 0x16) {
          uVar5 = uVar5 | 8;
        }
        else if (*(int *)(iVar8 + 0x4c) == 0xd) {
          uVar5 = uVar5 | *(uint *)(iVar8 + 0xc4);
        }
      }
    }
  }
  ppUVar10 = g_UiCatalogGroup48Records;
  for (iVar8 = 0x30; iVar8 != 0; iVar8 = iVar8 + -1) {
    *ppUVar10 = (UiCommandRuntimeRecordPrefix *)0x0;
    ppUVar10 = ppUVar10 + 1;
  }
  ppUVar10 = g_UiCatalogGroup48Records;
  itemCount = 0;
  ppAVar11 = g_ArmyAssetRecordRegistry;
  iVar8 = 0x300;
  do {
    pUVar3 = (UiCommandRuntimeRecordPrefix *)*ppAVar11;
    if ((((((pUVar3 != (UiCommandRuntimeRecordPrefix *)0x0) &&
           ((*(uint *)((int)pUVar3->reserved0C_1B + 8) & 1) != 0)) &&
          (bVar12 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                              (factionIndex,(ModelDefinitionHierarchyNodeAddress32)pUVar3), !bVar12)
          ) && (((*(uint *)((int)pUVar3->reserved0C_1B + 8) & 0xee) != 0 &&
                (*(int *)((int)pUVar3->reserved0C_1B + 0x10) != 0)))) &&
        ((itemCount < 0x30 && ((*(uint *)((int)pUVar3->reserved0C_1B + 8) & uVar5) != 0)))) &&
       ((bVar12 = FactionRuntime_HasArmyAssetOrActiveStructureCf
                            (factionIndex,(ArmyAssetRecordPrefix *)pUVar3), !bVar12 ||
        (bVar12 = ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                            (factionIndex,uVar5,(ArmyAssetRecordPrefix *)pUVar3), !bVar12)))) {
      *ppUVar10 = pUVar3;
      itemCount = itemCount + 1;
      ppUVar10 = ppUVar10 + 1;
    }
    ppAVar11 = ppAVar11 + 1;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  UVar13 = UiGrid_ComputeDimensionsPacked(6,itemCount);
  dVar6 = (dword)UVar13;
  if (8 < dVar6) {
    dVar6 = 8;
  }
  iVar8 = dVar6 * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  iVar9 = (int)(UVar13 >> 0x20) * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup48ColumnCount = dVar6;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset = -0x1f;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).rightOffset = -0x1f;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset = -0x47;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).bottomOffset = -0x47;
  }
  else {
    (inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset = -0x27;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).rightOffset = -0x27;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset = -0x59;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).bottomOffset = -0x59;
  }
  psVar4 = &(inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset;
  *psVar4 = *psVar4 - iVar8;
  psVar4 = &(inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset;
  *psVar4 = *psVar4 - iVar9;
  if (itemCount == 0) {
    pUVar1 = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
  }
  else {
    pUVar1 = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
  }
  psVar4 = g_UiCatalogGroup48OffsetTables[dVar6];
  uVar5 = 0;
  ppUVar10 = g_UiCatalogGroup48Records;
  do {
    iVar8 = psVar4[uVar5];
    pUVar3 = *ppUVar10;
    if (uVar5 < itemCount) {
      *(uint *)(inGameUiGridView->opaque0058_017B + iVar8 + -0x10) =
           *(uint *)(inGameUiGridView->opaque0058_017B + iVar8 + -0x10) & 0xfffffff7;
      AVar14 = pUVar3->buildXeniteCostQ4;
      pGVar7 = pUVar3->textureSource;
    }
    else {
      *(uint *)(inGameUiGridView->opaque0058_017B + iVar8 + -0x10) =
           *(uint *)(inGameUiGridView->opaque0058_017B + iVar8 + -0x10) | 8;
      AVar14 = 0;
      pGVar7 = (GraphicsTextureSourceAsset *)0x0;
    }
    uVar5 = uVar5 + 1;
    *(GraphicsTextureSourceAsset **)(inGameUiGridView->opaque0058_017B + iVar8 + -4) = pGVar7;
    *(ArmyBuildXeniteCostQ4 *)(inGameUiGridView->opaque0058_017B + iVar8 + 0x24) = AVar14;
    ppUVar10 = ppUVar10 + 1;
  } while (uVar5 < 0x30);
  (*((inGameUiGridView->catalogGroup48GridNode5DB4).vtable)->layout)
            (&inGameUiGridView->catalogGroup48GridNode5DB4);
  return;
}


/* Address: 0x0056A050.
   Ownership: ui/ingame/technology.
   Purpose: Collects up to 42 matching runtime records, chooses a compact grid, updates catalog controls from each
   record's textureSource and catalogDisplayValueQ4, suppresses unused controls, and relayouts the container.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
   [assets/model/definitions], FactionRuntime_HasArmyAssetOrActiveStructureCf [gameplay/faction/runtime],
   ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf [assets/army/catalog], UiGrid_ComputeDimensionsPacked
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx UiCatalogGroup42_RebuildGrid(UiNodeBase *node)

{
  UiNodeFlags *pUVar1;
  WorldOwnerListNode100 *pWVar2;
  UiCommandRuntimeRecordPrefix *pUVar3;
  sdword *psVar4;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  ArmyAssetRecordPrefix *armyAssetRecord;
  dword dVar5;
  int iVar6;
  GraphicsTextureSourceAsset *pGVar7;
  int iVar8;
  uint itemCount;
  uint uVar9;
  int iVar10;
  UiCommandRuntimeRecordPrefix **ppUVar11;
  ArmyAssetRecordPrefix **ppAVar12;
  bool bVar13;
  UiGridDimensionsEdxEax8 UVar14;
  ArmyBuildXeniteCostQ4 AVar15;
  
  inGameUiGridView = (InGameRuntimeUiGridViewC3E4 *)UiNode_GetRoot(node);
  iVar6 = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
  iVar10 = 0;
  for (pWVar2 = (inGameUiGridView->worldRuntime0A30).ownerListHead;
      pWVar2 != (WorldOwnerListNode100 *)0x0; pWVar2 = pWVar2->nextNode) {
    if (((pWVar2->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
        (*(int *)(*(int *)pWVar2->runtimePayload + 0x4c) == 0xb)) &&
       (*(int *)(*(int *)((int)pWVar2->runtimePayload + 8) + 0xc) == iVar6)) {
      iVar10 = iVar10 + 1;
    }
  }
  ppUVar11 = g_UiCatalogGroup42Records;
  for (iVar8 = 0x2a; iVar8 != 0; iVar8 = iVar8 + -1) {
    *ppUVar11 = (UiCommandRuntimeRecordPrefix *)0x0;
    ppUVar11 = ppUVar11 + 1;
  }
  ppUVar11 = g_UiCatalogGroup42Records;
  itemCount = 0;
  ppAVar12 = g_ArmyAssetRecordRegistry;
  iVar8 = 0x300;
  do {
    pUVar3 = (UiCommandRuntimeRecordPrefix *)*ppAVar12;
    if (((((pUVar3 != (UiCommandRuntimeRecordPrefix *)0x0) &&
          ((*(uint *)((int)pUVar3->reserved0C_1B + 8) & 1) != 0)) &&
         ((bVar13 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                              (iVar6,(ModelDefinitionHierarchyNodeAddress32)pUVar3), !bVar13 &&
          (((*(uint *)((int)pUVar3->reserved0C_1B + 8) & 0x10) != 0 &&
           (*(int *)((int)pUVar3->reserved0C_1B + 0x10) != 0)))))) && (itemCount < 0x2a)) &&
       ((iVar10 != 0 &&
        ((bVar13 = FactionRuntime_HasArmyAssetOrActiveStructureCf
                             (iVar6,(ArmyAssetRecordPrefix *)pUVar3), !bVar13 ||
         (bVar13 = ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                             (iVar6,0x10,(ArmyAssetRecordPrefix *)pUVar3), !bVar13)))))) {
      *ppUVar11 = pUVar3;
      itemCount = itemCount + 1;
      ppUVar11 = ppUVar11 + 1;
    }
    ppAVar12 = ppAVar12 + 1;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  UVar14 = UiGrid_ComputeDimensionsPacked(7,itemCount);
  dVar5 = (dword)UVar14;
  if (6 < dVar5) {
    dVar5 = 6;
  }
  iVar6 = dVar5 * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  iVar10 = (int)(UVar14 >> 0x20) * g_InGamePanelTextureSubresource34Height +
           g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup42ColumnCount = dVar5;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset = -0x1f;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).rightOffset = -0x1f;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset = -0x2a;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).bottomOffset = -0x2a;
  }
  else {
    (inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset = -0x27;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).rightOffset = -0x27;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset = -0x37;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).bottomOffset = -0x37;
  }
  psVar4 = &(inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset;
  *psVar4 = *psVar4 - iVar6;
  psVar4 = &(inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset;
  *psVar4 = *psVar4 - iVar10;
  if (itemCount == 0) {
    iVar6 = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
    pUVar1 = &(inGameUiGridView->catalogGroup42GridNode7680).nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    if (g_GameFactionRuntimeImage.records[iVar6].primaryArmyAssetCount == 0) {
      pUVar1 = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
      *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    }
    else {
      pUVar1 = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
      *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    }
  }
  else {
    pUVar1 = &(inGameUiGridView->catalogGroup42GridNode7680).nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    pUVar1 = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
  }
  psVar4 = g_UiCatalogGroup42OffsetTables[dVar5];
  uVar9 = 0;
  ppUVar11 = g_UiCatalogGroup42Records;
  do {
    iVar6 = psVar4[uVar9];
    pUVar3 = *ppUVar11;
    if (uVar9 < itemCount) {
      *(uint *)(inGameUiGridView->opaque0058_017B + iVar6 + -0x10) =
           *(uint *)(inGameUiGridView->opaque0058_017B + iVar6 + -0x10) & 0xfffffff7;
      AVar15 = pUVar3->buildXeniteCostQ4;
      pGVar7 = pUVar3->textureSource;
    }
    else {
      *(uint *)(inGameUiGridView->opaque0058_017B + iVar6 + -0x10) =
           *(uint *)(inGameUiGridView->opaque0058_017B + iVar6 + -0x10) | 8;
      AVar15 = 0;
      pGVar7 = (GraphicsTextureSourceAsset *)0x0;
    }
    uVar9 = uVar9 + 1;
    *(GraphicsTextureSourceAsset **)(inGameUiGridView->opaque0058_017B + iVar6 + -4) = pGVar7;
    *(ArmyBuildXeniteCostQ4 *)(inGameUiGridView->opaque0058_017B + iVar6 + 0x24) = AVar15;
    ppUVar11 = ppUVar11 + 1;
  } while (uVar9 < 0x2a);
  (*((inGameUiGridView->catalogGroup42GridNode7680).vtable)->layout)
            (&inGameUiGridView->catalogGroup42GridNode7680);
  return;
}


/* Address: 0x0056AEF0.
   Ownership: ui/ingame/technology.
   Purpose: Recovered action-table target INGAME_PAGE10[19] (0x1013).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], SelectionInfo_GetFirstEntry
   [gameplay/selection/runtime], UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists],
   FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx InGameTechnologyResearch_StartSelected(void *source)

{
  int iVar1;
  GameEntityRuntime *entityRuntime1;
  uint uVar2;
  CommandPayloadDword04 modelOffset;
  UiSelectableNodeEaxEcxCf9 UVar3;
  
  iVar1 = *(int *)((int)source + 8);
  while (iVar1 != -1) {
    source = *(void **)((int)source + 8);
    iVar1 = *(int *)((int)source + 8);
  }
  *(uint *)((int)source + 0xa78) = *(uint *)((int)source + 0xa78) & 0xfffffff7;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)((int)source + 0xbd0));
  entityRuntime1 = SelectionInfo_GetFirstEntry();
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    modelOffset = (int)(entityRuntime1->common).ownership.definitionOrClassRecord -
                  g_ModelRuntimeRebaseDelta;
    uVar2 = 0;
    UVar3 = UiSelectableGroup_NoneVisibleSelectedCf(7);
    if (!UVar3.carry) {
      uVar2 = UVar3.node[-1].layoutHeight - 0x300000;
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,uVar2 >> 1,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1700,0,uVar2 >> 1,modelOffset);
    }
  }
  return;
}


/* Address: 0x0056B050.
   Ownership: ui/ingame/technology.
   Purpose: Rebuilds the selected entity technology panel, toggles actions 0x1013 through 0x101A, formats available
   technology names and Xenite costs through resources 0x217C and 0x2181, and refreshes detail layout. Immutable
   entry: 0x0056B050. Closed fields: TEC +0x20 Xenite cost Q4, +0x24 Energy cost Q4, +0x28 research duration Q5.
   Binary sites 0x0056B284 and 0x0056B2DE perform the exact shifts and the UI labels the first cost through the
   Xenite resource path. Boundary: research source/category/level/direction and attachment routing remain
   independent axes; gameplay branch names require a complete STR/TEC/ARM/MDL/SPR/SHT/EFF chain.
   Cross-module calls: SelectionInfo_GetFirstEntry [gameplay/selection/runtime], UiNodeList_UnsuppressActionId
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext],
   ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot)

{
  sdword *psVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  TechnologyXeniteCostQ4 TVar5;
  SelectionPlayerRuntimeBlock *pSVar6;
  TechnologyAsset *pTVar7;
  PckTechnologyIdCatalog PVar8;
  GameEntityRuntime *entityRuntime1;
  word *pwVar9;
  PckTechnologyIdCatalog PVar10;
  dword dVar11;
  UiNodeBase *resourceId;
  int iVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  int iVar16;
  bool bVar17;
  RichTextExtentRegs RVar18;
  TextResourceResolveEaxCf5 TVar19;
  TextResourceResolveEaxCf5 TVar20;
  ArmyRegistryEaxCf5_51b6d0 AVar21;
  UiSelectableNodeEaxEcxCf9 UVar22;
  word *pwVar23;
  word *pwVar24;
  word *stream;
  word *source;
  UiActionId actionId;
  UiRootNode *firstNode;
  
  PVar8 = g_InGameSelectedTechnologyId;
  entityRuntime1 = SelectionInfo_GetFirstEntry();
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    piVar2 = (entityRuntime1->common).ownership.definitionOrClassRecord;
    iVar13 = *piVar2;
    if ((piVar2[0x3b] & 0x40U) == 0) {
      UiNodeList_UnsuppressActionId(0x1013,&inGameRoot->base);
    }
    else {
      UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
    }
    TVar19 = TextResource_Resolve(0x217c);
    TVar20 = TextResource_Resolve(*(int *)(iVar13 + 4) + 0x18004f);
    RichTextCommandStream_PatchPayloadBySelector(0,TVar20.eax,TVar19.eax);
    AVar21 = ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
    inGameRoot[0x4e].base.bottom = AVar21.eax[1].rootNodeOffsetOrPointer;
    UiNodeList_SuppressActionId(0x1014,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1015,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1016,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1017,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1018,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1019,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x101a,&inGameRoot->base);
    iVar12 = 0x1c;
    iVar16 = 6;
    do {
      bVar17 = Technology_IsAvailableForFactionCf
                         (*(PckTechnologyIdCatalog *)(iVar13 + 0x1c4 + iVar12 * 4),
                          (entityRuntime1->common).ownership.ownerIndex);
      if (bVar17) {
        iVar3 = *(int *)(iVar13 + 0x1c4 + iVar12 * 4);
        *(int *)((int)&inGameRoot[1].base.nextSibling + *(int *)(iVar16 * 4 + 0x562d84)) = iVar3;
        iVar4 = *(int *)(iVar16 * 4 + 0x562d68);
        actionId = *(UiActionId *)((int)&inGameRoot->callbacks + iVar4);
        *(int *)((int)inGameRoot + iVar4 + -8) = iVar3 * 2 + 0x300000;
        firstNode = inGameRoot;
        TVar19 = TextResource_Resolve(0x2181);
        pwVar9 = TVar19.eax;
        pwVar23 = pwVar9;
        pwVar24 = pwVar9;
        stream = pwVar9;
        source = pwVar9;
        TVar19 = TextResource_Resolve(*(TextResourceId *)((int)inGameRoot + iVar4 + -8));
        RichTextCommandStream_PatchPayloadBySelector(0,TVar19.eax,pwVar9);
        pwVar9 = *(word **)((int)inGameRoot + iVar4 + -4);
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                   (int)g_TechnologyAsset->records[*(int *)(iVar13 + 0x1c4 + iVar12 * 4)].
                        xeniteCostQ4 >> 4,pwVar23 + 0xc0);
        RichTextCommandStream_PatchPayloadBySelector(1,pwVar24 + 0xc0,stream);
        RichTextCommandStream_CopyExpandedCf(0x180,pwVar9,source);
        UiNodeList_UnsuppressActionId(actionId,&firstNode->base);
      }
      iVar16 = iVar16 + -1;
      if (iVar16 < 0) {
        iVar16 = 6;
      }
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    UVar22 = UiSelectableGroup_NoneVisibleSelectedCf(7);
    if (UVar22.carry) {
      pSVar6 = g_SelectionPlayerRuntimeBlockPointers[(int)inGameRoot[0x20].base.vtable];
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      inGameRoot[0x51].base.nextSibling = (UiNodeBase *)0x217f;
      inGameRoot[0x3d].base.rightAnchorQ31 = 0x2180;
      inGameRoot[0x50].base.bottomOffset = 6;
      inGameRoot[0x50].base.leftAnchorQ31 = 6;
      if ((pSVar6->assignmentFlags80A4 & 0x80) == 0) {
        UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
      }
      psVar1 = &inGameRoot[0x4e].base.topOffset;
      UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)psVar1);
      UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)psVar1);
    }
    else {
      inGameRoot[0x3d].base.rightAnchorQ31 = 0x217e;
      pTVar7 = g_TechnologyAsset;
      iVar13 = UVar22.node[-1].layoutHeight;
      resourceId = (UiNodeBase *)(iVar13 + 1);
      PVar10 = iVar13 - 0x300000U >> 1;
      TVar5 = g_TechnologyAsset->records[PVar10].xeniteCostQ4;
      uVar15 = g_RichTextColorPalette0Argb;
      if ((int)g_GameFactionRuntimeImage.records[(entityRuntime1->common).ownership.ownerIndex].
               xeniteCurrentQ4 < (int)TVar5) {
        uVar15 = g_RichTextInsufficientResourceColorArgb;
      }
      iVar13 = (int)g_TechnologyAsset->records[PVar10].energyCostQ4 >> 4;
      g_InGameSelectedTechnologyId = PVar10;
      dVar11 = (*g_WideNumberFormatUtf16)
                         (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int)TVar5 >> 4,
                          g_InGameTechnologyXeniteCostTextUtf16);
      *(undefined4 *)((int)g_InGameTechnologyXeniteCostTextUtf16 + dVar11) = 0x8000;
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar13,g_InGameTechnologyEnergyCostTextUtf16);
      uVar14 = iVar13 << 0x10 | uVar15 >> 0x10;
      g_InGameTechnologyCostColorWords8[1] = (word)uVar15;
      g_InGameTechnologyCostColorWords8[5] = (word)(uVar15 >> 0x10);
      g_InGameTechnologyCostColorWords8[0] = (word)(uVar15 >> 4);
      g_InGameTechnologyCostColorWords8[4] = (word)(uVar14 >> 4);
      g_InGameTechnologyCostColorWords8[3] = (word)(uVar15 >> 8);
      g_InGameTechnologyCostColorWords8[7] = (word)(uVar14 >> 8);
      g_InGameTechnologyCostColorWords8[2] = (word)(uVar15 >> 0xc);
      g_InGameTechnologyCostColorWords8[6] = (word)(uVar14 >> 0xc);
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 (int)pTVar7->records[PVar10].researchDurationQ5 >> 5,
                 g_InGameTechnologyResearchTimeTextUtf16);
      TVar19 = TextResource_Resolve((TextResourceId)resourceId);
      pwVar9 = TVar19.eax;
      RichTextCommandStream_PatchPayloadBySelector(0,&g_InGameTechnologyCostRichTextScratch,pwVar9);
      RichTextCommandStream_PatchPayloadBySelector(1,g_InGameTechnologyEnergyCostTextUtf16,pwVar9);
      RichTextCommandStream_PatchPayloadBySelector(2,g_InGameTechnologyResearchTimeTextUtf16,pwVar9)
      ;
      RVar18 = RichTextCommandStream_MeasureWrappedBlockRegs
                         (g_UiTextStyleNormal,pwVar9,(UiPixelExtent)inGameRoot[0x50].previousRoot);
      inGameRoot[0x50].base.bottomOffset = RVar18.widthPixels + 6;
      inGameRoot[0x50].base.leftAnchorQ31 = RVar18.heightPixels + 6;
      psVar1 = &inGameRoot[0x4e].base.topOffset;
      if (g_InGameSelectedTechnologyId != PVar8) {
        UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)psVar1);
        UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)psVar1);
      }
      inGameRoot[0x51].base.nextSibling = resourceId;
    }
  }
  return;
}

