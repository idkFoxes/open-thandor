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
void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl)

{
  UiRootNode *inGameRoot;
  UiNodeBase *selectedControl;
  bool sourceIsSelected;
  UiNodeBase *parentCursor;
  
  parentCursor = (selectableControl->base).parent;
  inGameRoot = (UiRootNode *)selectableControl;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    inGameRoot = (UiRootNode *)(inGameRoot->base).parent;
    parentCursor = (inGameRoot->base).parent;
  }
  sourceIsSelected = false;
  UiSelectableControl_IsSelectedCf(selectableControl);
  if (sourceIsSelected) {
    UiSelectableGroup_SelectExclusive(7,selectedControl);
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
undefined8 InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot)

{
  sdword *psVar1;
  UiNodeBase **ppUVar2;
  UiAnchorFractionQ31 *pUVar3;
  UiNodeFlags *pUVar4;
  uint *puVar5;
  int *piVar6;
  undefined4 in_EAX;
  GameEntityRuntime *pGVar7;
  int iVar8;
  int iVar9;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,in_EAX);
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
void UiCatalogGroup48_RebuildGrid(UiNodeBase *node)

{
  uint *puVar1;
  UiNodeVtable *pUVar2;
  ArmyAssetRecordPrefix *definitionNode;
  sdword *psVar3;
  UiCommandRuntimeRecordPrefix *pUVar4;
  UiNodeBase *pUVar5;
  uint uVar6;
  ArmyAssetRecordPrefix *armyAssetRecord;
  undefined4 extraout_EAX;
  GraphicsTextureSourceAsset *pGVar7;
  int iVar8;
  UiControlCount UVar9;
  UiControlCount extraout_ECX;
  UiControlCount extraout_ECX_00;
  uint extraout_ECX_01;
  int factionIndex;
  dword dVar10;
  dword extraout_EDX;
  int iVar11;
  UiCommandRuntimeRecordPrefix **ppUVar12;
  ArmyAssetRecordPrefix **ppAVar13;
  bool bVar14;
  undefined8 uVar15;
  UiGridDimensionsEdxEax8 UVar16;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar17;
  ArmyBuildXeniteCostQ4 AVar18;
  
  pUVar5 = UiNode_GetRoot(node);
  pUVar2 = pUVar5[0x25].vtable;
  uVar6 = SelectionInfo_CollectCapabilityFlags();
  if (uVar6 == 0) {
    for (; pUVar2 != (UiNodeVtable *)0x0; pUVar2 = pUVar2->method04) {
      if ((pUVar2[2].nonRightRelease == (UiNodeNonRightReleaseCallbackProc *)0x0) &&
         (iVar8 = *(int *)pUVar2[1].relocate,
         *(int *)(*(int *)(pUVar2[1].relocate + 8) + 0xc) == factionIndex)) {
        if (*(int *)(iVar8 + 0x4c) == 0x16) {
          uVar6 = uVar6 | 8;
        }
        else if (*(int *)(iVar8 + 0x4c) == 0xd) {
          uVar6 = uVar6 | *(uint *)(iVar8 + 0xc4);
        }
      }
    }
  }
  ppUVar12 = g_UiCatalogGroup48Records;
  for (iVar8 = 0x30; iVar8 != 0; iVar8 = iVar8 + -1) {
    *ppUVar12 = (UiCommandRuntimeRecordPrefix *)0x0;
    ppUVar12 = ppUVar12 + 1;
  }
  ppUVar12 = g_UiCatalogGroup48Records;
  UVar9 = 0;
  ppAVar13 = g_ArmyAssetRecordRegistry;
  dVar10 = 0x300;
  do {
    definitionNode = *ppAVar13;
    if ((definitionNode != (ArmyAssetRecordPrefix *)0x0) &&
       (bVar14 = false, (definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      MVar17 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                         (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
      dVar10 = MVar17.preservedEdxCallerValue;
      UVar9 = MVar17.preservedEcxCallerValue;
      armyAssetRecord = (ArmyAssetRecordPrefix *)MVar17.preservedEaxDefinitionNodeAddress;
      if (((!bVar14) &&
          ((((armyAssetRecord[1].selectionDetailTemplateVariantIndex & 0xee) != 0 &&
            (armyAssetRecord[1].rootNodeOffsetOrPointer != 0)) && (UVar9 < 0x30)))) &&
         (bVar14 = false, (armyAssetRecord[1].selectionDetailTemplateVariantIndex & uVar6) != 0)) {
        uVar15 = FactionRuntime_HasArmyAssetOrActiveStructureCf(factionIndex,armyAssetRecord);
        UVar9 = extraout_ECX;
        if (bVar14) {
          ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                    (factionIndex,uVar6,(ArmyAssetRecordPrefix *)uVar15);
          uVar15 = CONCAT44(extraout_EDX,extraout_EAX);
          UVar9 = extraout_ECX_00;
          dVar10 = extraout_EDX;
          if (bVar14) goto UiCatalogGroup48_RebuildGrid_AdvanceAfterArmyAssetEligibilityDecision;
        }
        dVar10 = (dword)((ulonglong)uVar15 >> 0x20);
        *ppUVar12 = (UiCommandRuntimeRecordPrefix *)uVar15;
        UVar9 = UVar9 + 1;
        ppUVar12 = ppUVar12 + 1;
      }
    }
UiCatalogGroup48_RebuildGrid_AdvanceAfterArmyAssetEligibilityDecision:
    ppAVar13 = ppAVar13 + 1;
    dVar10 = dVar10 - 1;
    if (dVar10 == 0) {
      UVar16 = UiGrid_ComputeDimensionsPacked(6,UVar9);
      dVar10 = (dword)UVar16;
      if (8 < dVar10) {
        dVar10 = 8;
      }
      iVar8 = dVar10 * g_InGamePanelTextureSubresource34Width +
              g_InGamePanelTextureSubresource27Width + g_InGamePanelTextureSubresource28Width;
      iVar11 = (int)(UVar16 >> 0x20) * g_InGamePanelTextureSubresource34Height +
               g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
      g_UiCatalogGroup48ColumnCount = dVar10;
      if ((int)g_FramebufferWidth < 800) {
        pUVar5[0x13d].topOffset = -0x1f;
        pUVar5[0x13d].bottomOffset = -0x1f;
        pUVar5[0x13d].rightOffset = -0x47;
        pUVar5[0x13d].leftAnchorQ31 = 0xffffffb9;
      }
      else {
        pUVar5[0x13d].topOffset = -0x27;
        pUVar5[0x13d].bottomOffset = -0x27;
        pUVar5[0x13d].rightOffset = -0x59;
        pUVar5[0x13d].leftAnchorQ31 = 0xffffffa7;
      }
      pUVar5[0x13d].topOffset = pUVar5[0x13d].topOffset - iVar8;
      pUVar5[0x13d].rightOffset = pUVar5[0x13d].rightOffset - iVar11;
      if (extraout_ECX_01 == 0) {
        pUVar5[0x13c].bottomOffset = pUVar5[0x13c].bottomOffset | 8;
      }
      else {
        pUVar5[0x13c].bottomOffset = pUVar5[0x13c].bottomOffset & 0xfffffff7;
      }
      psVar3 = g_UiCatalogGroup48OffsetTables[dVar10];
      uVar6 = 0;
      ppUVar12 = g_UiCatalogGroup48Records;
      do {
        iVar8 = psVar3[uVar6];
        pUVar4 = *ppUVar12;
        if (uVar6 < extraout_ECX_01) {
          puVar1 = (uint *)((int)&pUVar5->nodeFlags + iVar8);
          *puVar1 = *puVar1 & 0xfffffff7;
          AVar18 = pUVar4->buildXeniteCostQ4;
          pGVar7 = pUVar4->textureSource;
        }
        else {
          puVar1 = (uint *)((int)&pUVar5->nodeFlags + iVar8);
          *puVar1 = *puVar1 | 8;
          AVar18 = 0;
          pGVar7 = (GraphicsTextureSourceAsset *)0x0;
        }
        uVar6 = uVar6 + 1;
        *(GraphicsTextureSourceAsset **)((int)&pUVar5[1].parent + iVar8) = pGVar7;
        *(ArmyBuildXeniteCostQ4 *)((int)&pUVar5[1].leftAnchorQ31 + iVar8) = AVar18;
        ppUVar12 = ppUVar12 + 1;
      } while (uVar6 < 0x30);
      (**(code **)(pUVar5[0x13b].bottomAnchorQ31 + 0xc))(&pUVar5[0x13b].leftAnchorQ31);
      return;
    }
  } while( true );
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
void UiCatalogGroup42_RebuildGrid(UiNodeBase *node)

{
  uint *puVar1;
  UiNodeVtable *pUVar2;
  ArmyAssetRecordPrefix *definitionNode;
  sdword *psVar3;
  UiCommandRuntimeRecordPrefix *pUVar4;
  UiNodeBase *pUVar5;
  ArmyAssetRecordPrefix *armyAssetRecord;
  undefined4 extraout_EAX;
  int iVar6;
  GraphicsTextureSourceAsset *pGVar7;
  int iVar8;
  UiControlCount UVar9;
  UiControlCount extraout_ECX;
  UiControlCount extraout_ECX_00;
  uint extraout_ECX_01;
  dword dVar10;
  dword extraout_EDX;
  uint uVar11;
  int iVar12;
  UiCommandRuntimeRecordPrefix **ppUVar13;
  ArmyAssetRecordPrefix **ppAVar14;
  bool bVar15;
  undefined8 uVar16;
  UiGridDimensionsEdxEax8 UVar17;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar18;
  ArmyBuildXeniteCostQ4 AVar19;
  
  pUVar5 = UiNode_GetRoot(node);
  iVar6 = pUVar5[0x23].bottom;
  iVar12 = 0;
  for (pUVar2 = pUVar5[0x25].vtable; pUVar2 != (UiNodeVtable *)0x0; pUVar2 = pUVar2->method04) {
    if (((pUVar2[2].nonRightRelease == (UiNodeNonRightReleaseCallbackProc *)0x0) &&
        (*(int *)(*(int *)pUVar2[1].relocate + 0x4c) == 0xb)) &&
       (*(int *)(*(int *)(pUVar2[1].relocate + 8) + 0xc) == iVar6)) {
      iVar12 = iVar12 + 1;
    }
  }
  ppUVar13 = g_UiCatalogGroup42Records;
  for (iVar8 = 0x2a; iVar8 != 0; iVar8 = iVar8 + -1) {
    *ppUVar13 = (UiCommandRuntimeRecordPrefix *)0x0;
    ppUVar13 = ppUVar13 + 1;
  }
  ppUVar13 = g_UiCatalogGroup42Records;
  UVar9 = 0;
  ppAVar14 = g_ArmyAssetRecordRegistry;
  dVar10 = 0x300;
  do {
    definitionNode = *ppAVar14;
    if ((definitionNode != (ArmyAssetRecordPrefix *)0x0) &&
       (bVar15 = false, (definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      MVar18 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                         (iVar6,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
      dVar10 = MVar18.preservedEdxCallerValue;
      UVar9 = MVar18.preservedEcxCallerValue;
      armyAssetRecord = (ArmyAssetRecordPrefix *)MVar18.preservedEaxDefinitionNodeAddress;
      if ((((!bVar15) &&
           (((armyAssetRecord[1].selectionDetailTemplateVariantIndex & 0x10) != 0 &&
            (armyAssetRecord[1].rootNodeOffsetOrPointer != 0)))) && (UVar9 < 0x2a)) &&
         ((bVar15 = false, iVar12 != 0 &&
          ((uVar16 = FactionRuntime_HasArmyAssetOrActiveStructureCf(iVar6,armyAssetRecord),
           UVar9 = extraout_ECX, !bVar15 ||
           (ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                      (iVar6,0x10,(ArmyAssetRecordPrefix *)uVar16),
           uVar16 = CONCAT44(extraout_EDX,extraout_EAX), UVar9 = extraout_ECX_00,
           dVar10 = extraout_EDX, !bVar15)))))) {
        dVar10 = (dword)((ulonglong)uVar16 >> 0x20);
        *ppUVar13 = (UiCommandRuntimeRecordPrefix *)uVar16;
        UVar9 = UVar9 + 1;
        ppUVar13 = ppUVar13 + 1;
      }
    }
    ppAVar14 = ppAVar14 + 1;
    dVar10 = dVar10 - 1;
  } while (dVar10 != 0);
  UVar17 = UiGrid_ComputeDimensionsPacked(7,UVar9);
  dVar10 = (dword)UVar17;
  if (6 < dVar10) {
    dVar10 = 6;
  }
  iVar6 = dVar10 * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  iVar12 = (int)(UVar17 >> 0x20) * g_InGamePanelTextureSubresource34Height +
           g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup42ColumnCount = dVar10;
  if ((int)g_FramebufferWidth < 800) {
    pUVar5[0x191].nextSibling = (UiNodeBase *)0xffffffe1;
    pUVar5[0x191].parent = (UiNodeBase *)0xffffffe1;
    pUVar5[0x191].firstChild = (UiNodeBase *)0xffffffd6;
    pUVar5[0x191].vtable = (UiNodeVtable *)0xffffffd6;
  }
  else {
    pUVar5[0x191].nextSibling = (UiNodeBase *)0xffffffd9;
    pUVar5[0x191].parent = (UiNodeBase *)0xffffffd9;
    pUVar5[0x191].firstChild = (UiNodeBase *)0xffffffc9;
    pUVar5[0x191].vtable = (UiNodeVtable *)0xffffffc9;
  }
  pUVar5[0x191].nextSibling = (UiNodeBase *)((int)pUVar5[0x191].nextSibling - iVar6);
  pUVar5[0x191].firstChild = (UiNodeBase *)((int)pUVar5[0x191].firstChild - iVar12);
  if (extraout_ECX_01 == 0) {
    iVar6 = pUVar5[0x23].bottom;
    pUVar5[400].parent = (UiNodeBase *)((uint)pUVar5[400].parent | 8);
    if (g_GameFactionRuntimeImage.records[iVar6].primaryArmyAssetCount == 0) {
      pUVar5[0x1d9].rightOffset = pUVar5[0x1d9].rightOffset | 8;
    }
    else {
      pUVar5[0x1d9].rightOffset = pUVar5[0x1d9].rightOffset & 0xfffffff7;
    }
  }
  else {
    pUVar5[400].parent = (UiNodeBase *)((uint)pUVar5[400].parent & 0xfffffff7);
    pUVar5[0x1d9].rightOffset = pUVar5[0x1d9].rightOffset & 0xfffffff7;
  }
  psVar3 = g_UiCatalogGroup42OffsetTables[dVar10];
  uVar11 = 0;
  ppUVar13 = g_UiCatalogGroup42Records;
  do {
    iVar6 = psVar3[uVar11];
    pUVar4 = *ppUVar13;
    if (uVar11 < extraout_ECX_01) {
      puVar1 = (uint *)((int)&pUVar5->nodeFlags + iVar6);
      *puVar1 = *puVar1 & 0xfffffff7;
      AVar19 = pUVar4->buildXeniteCostQ4;
      pGVar7 = pUVar4->textureSource;
    }
    else {
      puVar1 = (uint *)((int)&pUVar5->nodeFlags + iVar6);
      *puVar1 = *puVar1 | 8;
      AVar19 = 0;
      pGVar7 = (GraphicsTextureSourceAsset *)0x0;
    }
    uVar11 = uVar11 + 1;
    *(GraphicsTextureSourceAsset **)((int)&pUVar5[1].parent + iVar6) = pGVar7;
    *(ArmyBuildXeniteCostQ4 *)((int)&pUVar5[1].leftAnchorQ31 + iVar6) = AVar19;
    ppUVar13 = ppUVar13 + 1;
  } while (uVar11 < 0x2a);
  (**(code **)(pUVar5[399].right + 0xc))(&pUVar5[399].vtable);
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
void InGameTechnologyResearch_StartSelected(void *source)

{
  int iVar1;
  GameEntityRuntime *entityRuntime1;
  uint uVar2;
  CommandPayloadDword04 modelOffset;
  bool bVar3;
  undefined8 uVar4;
  
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
    bVar3 = false;
    uVar4 = UiSelectableGroup_NoneVisibleSelectedCf(7);
    uVar2 = (uint)((ulonglong)uVar4 >> 0x20);
    if (!bVar3) {
      uVar2 = *(int *)((int)uVar4 + -8) - 0x300000;
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
void InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot)

{
  TechnologyXeniteCostQ4 TVar1;
  SelectionPlayerRuntimeBlock *pSVar2;
  TechnologyAsset *pTVar3;
  PckTechnologyIdCatalog PVar4;
  GameEntityRuntime *entityRuntime1;
  word *pwVar5;
  ArmyAssetRecordPrefix *pAVar6;
  word *replacementPayload;
  int iVar7;
  PckTechnologyIdCatalog PVar8;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  TextResourceId resourceId;
  UiNodeBase *extraout_ECX_04;
  UiNodeBase *extraout_ECX_05;
  UiNodeBase *pUVar9;
  word *stream;
  int iVar10;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  uint uVar11;
  uint uVar12;
  int iVar13;
  undefined1 uVar14;
  undefined1 uVar15;
  dword dVar20;
  undefined8 uVar16;
  RichTextExtentRegs RVar17;
  word *pwVar18;
  word *pwVar19;
  word *stream_00;
  word *source;
  UiActionId actionId;
  UiRootNode *firstNode;
  
  PVar4 = g_InGameSelectedTechnologyId;
  entityRuntime1 = SelectionInfo_GetFirstEntry();
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    if ((*(uint *)((int)(entityRuntime1->common).ownership.definitionOrClassRecord + 0xec) & 0x40)
        == 0) {
      UiNodeList_UnsuppressActionId(0x1013,&inGameRoot->base);
    }
    else {
      UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
    }
    TextResource_Resolve(0x217c);
    uVar14 = 0xffe7ffb0 < *(uint *)(extraout_ECX + 4);
    pwVar5 = TextResource_Resolve(*(uint *)(extraout_ECX + 4) + 0x18004f);
    RichTextCommandStream_PatchPayloadBySelector(0,pwVar5,stream);
    pAVar6 = ArmyAssetRegistry_FindByIdCf((entityRuntime1->common).runtimeIdentityOrArmyAssetId);
    inGameRoot[0x4e].base.bottom = pAVar6[1].rootNodeOffsetOrPointer;
    UiNodeList_SuppressActionId(0x1014,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1015,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1016,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1017,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1018,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1019,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x101a,&inGameRoot->base);
    iVar10 = 0x1c;
    iVar13 = 6;
    iVar7 = extraout_ECX_00;
    do {
      Technology_IsAvailableForFactionCf
                (*(PckTechnologyIdCatalog *)(iVar7 + 0x1c4 + iVar10 * 4),
                 (entityRuntime1->common).ownership.ownerIndex);
      uVar15 = 0;
      iVar7 = extraout_ECX_01;
      iVar10 = extraout_EDX;
      if ((bool)uVar14) {
        iVar7 = *(int *)(extraout_ECX_01 + 0x1c4 + extraout_EDX * 4);
        *(int *)((int)&inGameRoot[1].base.nextSibling + *(int *)(iVar13 * 4 + 0x562d84)) = iVar7;
        iVar10 = *(int *)(iVar13 * 4 + 0x562d68);
        actionId = *(UiActionId *)((int)&inGameRoot->callbacks + iVar10);
        *(int *)((int)inGameRoot + iVar10 + -8) = iVar7 * 2 + 0x300000;
        firstNode = inGameRoot;
        pwVar5 = TextResource_Resolve(0x2181);
        pwVar18 = pwVar5;
        pwVar19 = pwVar5;
        stream_00 = pwVar5;
        source = pwVar5;
        replacementPayload =
             TextResource_Resolve(*(TextResourceId *)((int)inGameRoot + iVar10 + -8));
        uVar16 = RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,pwVar5);
        pwVar5 = *(word **)((int)inGameRoot + iVar10 + -4);
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                   (int)g_TechnologyAsset->records
                        [*(int *)(extraout_ECX_02 + 0x1c4 + (int)((ulonglong)uVar16 >> 0x20) * 4)].
                        xeniteCostQ4 >> 4,pwVar18 + 0xc0);
        uVar15 = (word *)0xfffffe7f < pwVar19;
        RichTextCommandStream_PatchPayloadBySelector(1,pwVar19 + 0xc0,stream_00);
        RichTextCommandStream_CopyExpandedCf(0x180,pwVar5,source);
        UiNodeList_UnsuppressActionId(actionId,&firstNode->base);
        iVar7 = extraout_ECX_03;
        iVar10 = extraout_EDX_00;
      }
      iVar13 = iVar13 + -1;
      if (iVar13 < 0) {
        iVar13 = 6;
      }
      iVar10 = iVar10 + -1;
      uVar14 = uVar15;
    } while (iVar10 != 0);
    iVar7 = UiSelectableGroup_NoneVisibleSelectedCf(7);
    if ((bool)uVar15) {
      pSVar2 = g_SelectionPlayerRuntimeBlockPointers[(int)inGameRoot[0x20].base.vtable];
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      inGameRoot[0x51].base.nextSibling = (UiNodeBase *)0x217f;
      inGameRoot[0x3d].base.rightAnchorQ31 = 0x2180;
      inGameRoot[0x50].base.bottomOffset = 6;
      inGameRoot[0x50].base.leftAnchorQ31 = 6;
      if ((pSVar2->assignmentFlags80A4 & 0x80) == 0) {
        UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
      }
      uVar16 = UiScrollableControl_RebuildViewportAndScrollbars
                         ((UiScrollableControl *)&inGameRoot[0x4e].base.topOffset);
      UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar16);
    }
    else {
      inGameRoot[0x3d].base.rightAnchorQ31 = 0x217e;
      pTVar3 = g_TechnologyAsset;
      PVar8 = *(int *)(iVar7 + -8) - 0x300000U >> 1;
      TVar1 = g_TechnologyAsset->records[PVar8].xeniteCostQ4;
      uVar12 = g_RichTextColorPalette0Argb;
      if ((int)g_GameFactionRuntimeImage.records[(entityRuntime1->common).ownership.ownerIndex].
               xeniteCurrentQ4 < (int)TVar1) {
        uVar12 = g_RichTextInsufficientResourceColorArgb;
      }
      g_InGameSelectedTechnologyId = PVar8;
      _dVar20 = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int)TVar1 >> 4,
                           g_InGameTechnologyXeniteCostTextUtf16);
      *(undefined4 *)((int)g_InGameTechnologyXeniteCostTextUtf16 + (dword)_dVar20) = 0x8000;
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)((ulonglong)_dVar20 >> 0x20),
                 g_InGameTechnologyEnergyCostTextUtf16);
      uVar11 = extraout_EDX_01 << 0x10 | uVar12 >> 0x10;
      g_InGameTechnologyCostColorWords8[1] = (word)uVar12;
      g_InGameTechnologyCostColorWords8[5] = (word)(uVar12 >> 0x10);
      g_InGameTechnologyCostColorWords8[0] = (word)(uVar12 >> 4);
      g_InGameTechnologyCostColorWords8[4] = (word)(uVar11 >> 4);
      g_InGameTechnologyCostColorWords8[3] = (word)(uVar12 >> 8);
      g_InGameTechnologyCostColorWords8[7] = (word)(uVar11 >> 8);
      g_InGameTechnologyCostColorWords8[2] = (word)(uVar12 >> 0xc);
      g_InGameTechnologyCostColorWords8[6] = (word)(uVar11 >> 0xc);
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 (int)pTVar3->records[PVar8].researchDurationQ5 >> 5,
                 g_InGameTechnologyResearchTimeTextUtf16);
      pwVar5 = TextResource_Resolve(resourceId);
      uVar16 = RichTextCommandStream_PatchPayloadBySelector
                         (0,&g_InGameTechnologyCostRichTextScratch,pwVar5);
      uVar16 = RichTextCommandStream_PatchPayloadBySelector
                         (1,g_InGameTechnologyEnergyCostTextUtf16,(word *)uVar16);
      uVar16 = RichTextCommandStream_PatchPayloadBySelector
                         (2,g_InGameTechnologyResearchTimeTextUtf16,(word *)uVar16);
      RVar17 = RichTextCommandStream_MeasureWrappedBlockRegs
                         (g_UiTextStyleNormal,(word *)uVar16,
                          (UiPixelExtent)inGameRoot[0x50].previousRoot);
      inGameRoot[0x50].base.bottomOffset = RVar17.widthPixels + 6;
      inGameRoot[0x50].base.leftAnchorQ31 = RVar17.heightPixels + 6;
      pUVar9 = extraout_ECX_04;
      if (g_InGameSelectedTechnologyId != PVar4) {
        uVar16 = UiScrollableControl_RebuildViewportAndScrollbars
                           ((UiScrollableControl *)&inGameRoot[0x4e].base.topOffset);
        UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)uVar16);
        pUVar9 = extraout_ECX_05;
      }
      inGameRoot[0x51].base.nextSibling = pUVar9;
    }
  }
  return;
}
