/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/technology.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/technology.h>
#include <thandor/thandor.h>

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
  bool isSelected;
  UiNodeBase *parentCursor;
  
  parentCursor = (selectableControl->base).parent;
  inGameRoot = (UiRootNode *)selectableControl;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    inGameRoot = (UiRootNode *)(inGameRoot->base).parent;
    parentCursor = (inGameRoot->base).parent;
  }
  isSelected = (bool)UiSelectableControl_IsSelectedCf(selectableControl);
  if (isSelected) {
    UiSelectableGroup_SelectExclusive(7,&selectableControl->base,
      INGAME_UI(inGameRoot,technologyAreaTab7),
      INGAME_UI(inGameRoot,technologyAreaTab6),
      INGAME_UI(inGameRoot,technologyAreaTab5),
      INGAME_UI(inGameRoot,technologyAreaTab4),
      INGAME_UI(inGameRoot,technologyAreaTab3),
      INGAME_UI(inGameRoot,technologyAreaTab2),
      INGAME_UI(inGameRoot,technologyAreaTab1));
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
  int32_t *flagSlot;
  UiNodeBase **parentFlagSlot;
  UiAnchorFractionQ31 *anchorFlagSlot;
  UiNodeFlags *nodeFlagsSlot;
  uint32_t *rowFlags;
  int *entityDefinition;
  GameEntityRuntime *firstSelectedEntity;
  int technologyCursor;
  int remainingCount;
  uint32_t areaIndex;
  
  flagSlot = &INGAME_UI_FIELD(inGameRoot,worldView,0x48,int32_t);
  *flagSlot = *flagSlot | 8;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)INGAME_UI(inGameRoot,worldView));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  entityDefinition = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
  flagSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab1,0x4C,int32_t);
  *flagSlot = *flagSlot & 0xfffffffd;
  INGAME_UI_FIELD(inGameRoot,technologyAreaTab2,0x4C,struct UiRootCallbacks *) = (UiRootCallbacks *)((uint32_t)INGAME_UI_FIELD(inGameRoot,technologyAreaTab2,0x4C,struct UiRootCallbacks *) & 0xfffffffd);
  parentFlagSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab3,0x4C,struct UiNodeBase *);
  *parentFlagSlot = (UiNodeBase *)((uint32_t)*parentFlagSlot & 0xfffffffd);
  flagSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab4,0x4C,int32_t);
  *flagSlot = *flagSlot & 0xfffffffd;
  flagSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab5,0x4C,int32_t);
  *flagSlot = *flagSlot & 0xfffffffd;
  anchorFlagSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab6,0x4C,uint32_t);
  *anchorFlagSlot = *anchorFlagSlot & 0xfffffffd;
  nodeFlagsSlot = &INGAME_UI_FIELD(inGameRoot,technologyAreaTab7,0x4C,enum UiNodeFlags);
  *nodeFlagsSlot = *nodeFlagsSlot & ~UI_NODE_PREFERRED_FOCUS_TARGET;
  if ((entityDefinition[0x3b] & 0xc0U) != 0) {
    technologyCursor = *entityDefinition;
    areaIndex = 0xffffffff;
    remainingCount = 0x1c;
    do {
      areaIndex = areaIndex + 1;
      if (6 < areaIndex) {
        areaIndex = 0;
      }
      if (*(int *)(technologyCursor + 0x1c8) == entityDefinition[0x40]) {
        rowFlags = (uint32_t *)((int)&inGameRoot->rootFlags + *(int *)(areaIndex * 4 + THANDOR_ADDR(g_TechnologyPanelRowFlagOffsets,0)));
        *rowFlags = *rowFlags | 2;
        break;
      }
      technologyCursor = technologyCursor + 4;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
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
  UiNodeFlags *gridNodeFlags;
  WorldOwnerListNode100 *ownerNode;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  int32_t *offsetSlotOrTable;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  uint32_t capabilityFlagsOrSlotIndex;
  ArmyAssetRecordPrefix *armyAssetRecord;
  uint32_t columnCount;
  GraphicsTextureSourceAsset *textureAsset;
  int modelOrCounterOrOffset;
  uint32_t itemCount;
  int factionIndex;
  int panelHeight;
  UiCommandRuntimeRecordPrefix **recordCursor;
  ArmyAssetRecordPrefix **registryCursor;
  bool checkResult;
  UiGridDimensionsEdxEax8 gridDimensions;
  ArmyBuildXeniteCostQ4 xeniteCost;
  
  inGameUiGridView = (InGameRuntimeUiGridViewC3E4 *)UiNode_GetRoot(node);
  ownerNode = (inGameUiGridView->worldRuntime0A30).ownerListHead;
  /* The original loads EDX = root[+0xA80] (the local faction) before the call, which preserves it;
     the decompiler lost that load. */
  factionIndex = *(int *)((uint8_t *)inGameUiGridView + 0xa80);
  capabilityFlagsOrSlotIndex = SelectionInfo_CollectCapabilityFlags();
  if (capabilityFlagsOrSlotIndex == 0) {
    for (; ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
      if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelOrCounterOrOffset = *(int *)ownerNode->runtimePayload,
         *(int *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc) == factionIndex)) {
        if (*(int *)(modelOrCounterOrOffset + 0x4c) == 0x16) {
          capabilityFlagsOrSlotIndex = capabilityFlagsOrSlotIndex | 8;
        }
        else if (*(int *)(modelOrCounterOrOffset + 0x4c) == 0xd) {
          capabilityFlagsOrSlotIndex = capabilityFlagsOrSlotIndex | *(uint32_t *)(modelOrCounterOrOffset + 0xc4);
        }
      }
    }
  }
  recordCursor = g_UiCatalogGroup48Records;
  for (modelOrCounterOrOffset = 0x30; modelOrCounterOrOffset != 0; modelOrCounterOrOffset = modelOrCounterOrOffset + -1) {
    *recordCursor = (UiCommandRuntimeRecordPrefix *)0x0;
    recordCursor = recordCursor + 1;
  }
  recordCursor = g_UiCatalogGroup48Records;
  itemCount = 0;
  registryCursor = g_ArmyAssetRecordRegistry;
  modelOrCounterOrOffset = 0x300;
  do {
    catalogRecord = (UiCommandRuntimeRecordPrefix *)*registryCursor;
    if ((((((catalogRecord != (UiCommandRuntimeRecordPrefix *)0x0) &&
           ((*(uint32_t *)((int)catalogRecord->reserved0C_1B + 8) & 1) != 0)) &&
          (checkResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                              (factionIndex,(ModelDefinitionHierarchyNodeAddress32)catalogRecord), !checkResult)
          ) && (((*(uint32_t *)((int)catalogRecord->reserved0C_1B + 8) & 0xee) != 0 &&
                (*(int *)((int)catalogRecord->reserved0C_1B + 0x10) != 0)))) &&
        ((itemCount < 0x30 && ((*(uint32_t *)((int)catalogRecord->reserved0C_1B + 8) & capabilityFlagsOrSlotIndex) != 0)))) &&
       ((checkResult = FactionRuntime_HasArmyAssetOrActiveStructureCf
                            (factionIndex,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult ||
        (checkResult = ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                            (factionIndex,capabilityFlagsOrSlotIndex,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult)))) {
      *recordCursor = catalogRecord;
      itemCount = itemCount + 1;
      recordCursor = recordCursor + 1;
    }
    registryCursor = registryCursor + 1;
    modelOrCounterOrOffset = modelOrCounterOrOffset + -1;
  } while (modelOrCounterOrOffset != 0);
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = (uint32_t)gridDimensions;
  if (8 < columnCount) {
    columnCount = 8;
  }
  modelOrCounterOrOffset = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)(gridDimensions >> 0x20) * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup48ColumnCount = columnCount;
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
  offsetSlotOrTable = &(inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset;
  *offsetSlotOrTable = *offsetSlotOrTable - modelOrCounterOrOffset;
  offsetSlotOrTable = &(inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset;
  *offsetSlotOrTable = *offsetSlotOrTable - panelHeight;
  if (itemCount == 0) {
    gridNodeFlags = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *gridNodeFlags = *gridNodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    gridNodeFlags = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *gridNodeFlags = *gridNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  offsetSlotOrTable = g_UiCatalogGroup48OffsetTables[columnCount];
  capabilityFlagsOrSlotIndex = 0;
  recordCursor = g_UiCatalogGroup48Records;
  do {
    modelOrCounterOrOffset = offsetSlotOrTable[capabilityFlagsOrSlotIndex];
    catalogRecord = *recordCursor;
    if (capabilityFlagsOrSlotIndex < itemCount) {
      *(uint32_t *)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + -0x10) =
           *(uint32_t *)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + -0x10) & 0xfffffff7;
      xeniteCost = catalogRecord->buildXeniteCostQ4;
      textureAsset = catalogRecord->textureSource;
    }
    else {
      *(uint32_t *)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + -0x10) =
           *(uint32_t *)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + -0x10) | 8;
      xeniteCost = 0;
      textureAsset = (GraphicsTextureSourceAsset *)0x0;
    }
    capabilityFlagsOrSlotIndex = capabilityFlagsOrSlotIndex + 1;
    *(GraphicsTextureSourceAsset **)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + -4) = textureAsset;
    *(ArmyBuildXeniteCostQ4 *)(inGameUiGridView->opaque0058_017B + modelOrCounterOrOffset + 0x24) = xeniteCost;
    recordCursor = recordCursor + 1;
  } while (capabilityFlagsOrSlotIndex < 0x30);
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
  UiNodeFlags *gridNodeFlags;
  WorldOwnerListNode100 *ownerNode;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  int32_t *offsetSlotOrTable;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  ArmyAssetRecordPrefix *armyAssetRecord;
  uint32_t columnCount;
  int factionOrExtentOrOffset;
  GraphicsTextureSourceAsset *textureAsset;
  int remainingCount;
  uint32_t itemCount;
  uint32_t slotIndex;
  int structureCountOrHeight;
  UiCommandRuntimeRecordPrefix **recordCursor;
  ArmyAssetRecordPrefix **registryCursor;
  bool checkResult;
  UiGridDimensionsEdxEax8 gridDimensions;
  ArmyBuildXeniteCostQ4 xeniteCost;
  
  inGameUiGridView = (InGameRuntimeUiGridViewC3E4 *)UiNode_GetRoot(node);
  factionOrExtentOrOffset = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
  structureCountOrHeight = 0;
  for (ownerNode = (inGameUiGridView->worldRuntime0A30).ownerListHead;
      ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
    if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
        (*(int *)(*(int *)ownerNode->runtimePayload + 0x4c) == 0xb)) &&
       (*(int *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc) == factionOrExtentOrOffset)) {
      structureCountOrHeight = structureCountOrHeight + 1;
    }
  }
  recordCursor = g_UiCatalogGroup42Records;
  for (remainingCount = 0x2a; remainingCount != 0; remainingCount = remainingCount + -1) {
    *recordCursor = (UiCommandRuntimeRecordPrefix *)0x0;
    recordCursor = recordCursor + 1;
  }
  recordCursor = g_UiCatalogGroup42Records;
  itemCount = 0;
  registryCursor = g_ArmyAssetRecordRegistry;
  remainingCount = 0x300;
  do {
    catalogRecord = (UiCommandRuntimeRecordPrefix *)*registryCursor;
    if (((((catalogRecord != (UiCommandRuntimeRecordPrefix *)0x0) &&
          ((*(uint32_t *)((int)catalogRecord->reserved0C_1B + 8) & 1) != 0)) &&
         ((checkResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                              (factionOrExtentOrOffset,(ModelDefinitionHierarchyNodeAddress32)catalogRecord), !checkResult &&
          (((*(uint32_t *)((int)catalogRecord->reserved0C_1B + 8) & 0x10) != 0 &&
           (*(int *)((int)catalogRecord->reserved0C_1B + 0x10) != 0)))))) && (itemCount < 0x2a)) &&
       ((structureCountOrHeight != 0 &&
        ((checkResult = FactionRuntime_HasArmyAssetOrActiveStructureCf
                             (factionOrExtentOrOffset,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult ||
         (checkResult = ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
                             (factionOrExtentOrOffset,0x10,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult)))))) {
      *recordCursor = catalogRecord;
      itemCount = itemCount + 1;
      recordCursor = recordCursor + 1;
    }
    registryCursor = registryCursor + 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  gridDimensions = UiGrid_ComputeDimensionsPacked(7,itemCount);
  columnCount = (uint32_t)gridDimensions;
  if (6 < columnCount) {
    columnCount = 6;
  }
  factionOrExtentOrOffset = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  structureCountOrHeight = (int)(gridDimensions >> 0x20) * g_InGamePanelTextureSubresource34Height +
           g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup42ColumnCount = columnCount;
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
  offsetSlotOrTable = &(inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset;
  *offsetSlotOrTable = *offsetSlotOrTable - factionOrExtentOrOffset;
  offsetSlotOrTable = &(inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset;
  *offsetSlotOrTable = *offsetSlotOrTable - structureCountOrHeight;
  if (itemCount == 0) {
    factionOrExtentOrOffset = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
    gridNodeFlags = &(inGameUiGridView->catalogGroup42GridNode7680).nodeFlags;
    *gridNodeFlags = *gridNodeFlags | UI_NODE_SUPPRESSED;
    if (g_GameFactionRuntimeImage.records[factionOrExtentOrOffset].primaryArmyAssetCount == 0) {
      gridNodeFlags = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
      *gridNodeFlags = *gridNodeFlags | UI_NODE_SUPPRESSED;
    }
    else {
      gridNodeFlags = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
      *gridNodeFlags = *gridNodeFlags & ~UI_NODE_SUPPRESSED;
    }
  }
  else {
    gridNodeFlags = &(inGameUiGridView->catalogGroup42GridNode7680).nodeFlags;
    *gridNodeFlags = *gridNodeFlags & ~UI_NODE_SUPPRESSED;
    gridNodeFlags = &(inGameUiGridView->commandSpriteVariantAGridNode8C4C).nodeFlags;
    *gridNodeFlags = *gridNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  offsetSlotOrTable = g_UiCatalogGroup42OffsetTables[columnCount];
  slotIndex = 0;
  recordCursor = g_UiCatalogGroup42Records;
  do {
    factionOrExtentOrOffset = offsetSlotOrTable[slotIndex];
    catalogRecord = *recordCursor;
    if (slotIndex < itemCount) {
      *(uint32_t *)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + -0x10) =
           *(uint32_t *)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + -0x10) & 0xfffffff7;
      xeniteCost = catalogRecord->buildXeniteCostQ4;
      textureAsset = catalogRecord->textureSource;
    }
    else {
      *(uint32_t *)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + -0x10) =
           *(uint32_t *)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + -0x10) | 8;
      xeniteCost = 0;
      textureAsset = (GraphicsTextureSourceAsset *)0x0;
    }
    slotIndex = slotIndex + 1;
    *(GraphicsTextureSourceAsset **)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + -4) = textureAsset;
    *(ArmyBuildXeniteCostQ4 *)(inGameUiGridView->opaque0058_017B + factionOrExtentOrOffset + 0x24) = xeniteCost;
    recordCursor = recordCursor + 1;
  } while (slotIndex < 0x2a);
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
  int parentLink;
  GameEntityRuntime *firstSelectedEntity;
  uint32_t doubledTechnologyId;
  CommandPayloadDword04 modelOffset;
  SelectableGroupNodeResult selectedArea;
  
  parentLink = *(int *)((int)source + 8);
  while (parentLink != -1) {
    source = *(void **)((int)source + 8);
    parentLink = *(int *)((int)source + 8);
  }
  INGAME_UI(source,worldView)->nodeFlags =
       INGAME_UI(source,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(source,gameWindowPageStack));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != (GameEntityRuntime *)0x0) {
    modelOffset = (int)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
                  g_ModelRuntimeRebaseDelta;
    doubledTechnologyId = 0;
    selectedArea = UiSelectableGroup_NoneVisibleSelectedCf(7,
      INGAME_UI(source,technologyAreaTab7),
      INGAME_UI(source,technologyAreaTab6),
      INGAME_UI(source,technologyAreaTab5),
      INGAME_UI(source,technologyAreaTab4),
      INGAME_UI(source,technologyAreaTab3),
      INGAME_UI(source,technologyAreaTab2),
      INGAME_UI(source,technologyAreaTab1));
    if (!selectedArea.noneSelected) {
      /* the dword 8 bytes before the selected area tab (see InGameTechnologyPanel_Rebuild) */
      doubledTechnologyId = THANDOR_UI_FIELD(selectedArea.node,-8,int32_t) - 0x300000;
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,doubledTechnologyId >> 1,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1700,0,doubledTechnologyId >> 1,modelOffset);
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
  UiScrollableControl *scrollableControl;
  int *entityDefinition;
  int technologyId;
  int rowFlagOffset;
  TechnologyXeniteCostQ4 xeniteCost;
  SelectionPlayerRuntimeBlock *playerBlock;
  TechnologyAsset *technologyAsset;
  PckTechnologyIdCatalog previousTechnologyId;
  GameEntityRuntime *firstSelectedEntity;
  uint16_t *labelText;
  PckTechnologyIdCatalog selectedTechnologyId;
  uint32_t writtenBytes;
  UiNodeBase *resourceId;
  int remainingCount;
  int definitionOrEnergyCost;
  uint32_t packedEnergyColor;
  uint32_t costColor;
  int areaIndex;
  bool isAvailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextResolveResult resolvedName;
  ArmyAssetLookupResult armyRecord;
  SelectableGroupNodeResult selectedArea;
  uint16_t *formatBuffer;
  uint16_t *formattedText;
  uint16_t *stream;
  uint16_t *source;
  UiActionId actionId;
  UiRootNode *firstNode;
  
  previousTechnologyId = g_InGameSelectedTechnologyId;
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != (GameEntityRuntime *)0x0) {
    entityDefinition = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
    definitionOrEnergyCost = *entityDefinition;
    if ((entityDefinition[0x3b] & 0x40U) == 0) {
      UiNodeList_UnsuppressActionId(0x1013,&inGameRoot->base);
    }
    else {
      UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
    }
    resolvedText = TextResource_Resolve(0x217c);
    resolvedName = TextResource_Resolve(*(int *)(definitionOrEnergyCost + 4) + 0x18004f);
    RichTextCommandStream_PatchPayloadBySelector(0,resolvedName.text,resolvedText.text);
    armyRecord = ArmyAssetRegistry_FindByIdCf((firstSelectedEntity->common).runtimeIdentityOrArmyAssetId);
    ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyDescriptionFrame))->textureSource =
         (GraphicsTextureSourceAsset *)armyRecord.recordOrError[1].rootNodeOffsetOrPointer;
    UiNodeList_SuppressActionId(0x1014,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1015,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1016,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1017,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1018,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x1019,&inGameRoot->base);
    UiNodeList_SuppressActionId(0x101a,&inGameRoot->base);
    remainingCount = 0x1c;
    areaIndex = 6;
    do {
      isAvailable = Technology_IsAvailableForFactionCf
                         (*(PckTechnologyIdCatalog *)(definitionOrEnergyCost + 0x1c4 + remainingCount * 4),
                          (firstSelectedEntity->common).ownership.ownerIndex);
      if (isAvailable) {
        technologyId = *(int *)(definitionOrEnergyCost + 0x1c4 + remainingCount * 4);
        /* the area tab's icon (technologyAreaTabNIcon) shows subresource technologyId of tech.gfx */
        ((UiImagePanelControl *)
         THANDOR_UI_AT(inGameRoot,*(int *)(areaIndex * 4 + THANDOR_ADDR(g_TechnologyPanelRowValueOffsets,0))))->
        subresource = technologyId;
        rowFlagOffset = *(int *)(areaIndex * 4 + THANDOR_ADDR(g_TechnologyPanelRowFlagOffsets,0));
        /* rowFlagOffset is the area tab technologyAreaTabN; the dwords 8 and 4 bytes before it (the +0x60/+0x64
           slots of the preceding 0x68-byte text button) hold the area's text id and a text pointer */
        actionId = ((UiSelectableControl *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))->actionId;
        THANDOR_UI_FIELD(inGameRoot,rowFlagOffset + -8,int) = technologyId * 2 + 0x300000;
        firstNode = inGameRoot;
        resolvedText = TextResource_Resolve(0x2181);
        labelText = resolvedText.text;
        formatBuffer = labelText;
        formattedText = labelText;
        stream = labelText;
        source = labelText;
        resolvedText = TextResource_Resolve(THANDOR_UI_FIELD(inGameRoot,rowFlagOffset + -8,TextResourceId));
        RichTextCommandStream_PatchPayloadBySelector(0,resolvedText.text,labelText);
        labelText = THANDOR_UI_FIELD(inGameRoot,rowFlagOffset + -4,uint16_t *);
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                   (int)g_TechnologyAsset->records[*(int *)(definitionOrEnergyCost + 0x1c4 + remainingCount * 4)].
                        xeniteCostQ4 >> 4,formatBuffer + 0xc0);
        RichTextCommandStream_PatchPayloadBySelector(1,formattedText + 0xc0,stream);
        RichTextCommandStream_CopyExpandedCf(0x180,labelText,source);
        UiNodeList_UnsuppressActionId(actionId,&firstNode->base);
      }
      areaIndex = areaIndex + -1;
      if (areaIndex < 0) {
        areaIndex = 6;
      }
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    selectedArea = UiSelectableGroup_NoneVisibleSelectedCf(7,
      INGAME_UI(inGameRoot,technologyAreaTab7),
      INGAME_UI(inGameRoot,technologyAreaTab6),
      INGAME_UI(inGameRoot,technologyAreaTab5),
      INGAME_UI(inGameRoot,technologyAreaTab4),
      INGAME_UI(inGameRoot,technologyAreaTab3),
      INGAME_UI(inGameRoot,technologyAreaTab2),
      INGAME_UI(inGameRoot,technologyAreaTab1));
    if (selectedArea.noneSelected) {
      playerBlock = g_SelectionPlayerRuntimeBlockPointers
                    [((WorldRuntimeContext *)INGAME_UI(inGameRoot,worldView))->selection.activePlayerRuntimeId];
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)0x217f;
      ((UiTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->textResourceId = 0x2180;
      INGAME_UI(inGameRoot,technologyDescriptionText)->rightOffset = 6;
      INGAME_UI(inGameRoot,technologyDescriptionText)->bottomOffset = 6;
      if ((playerBlock->assignmentFlags80A4 & 0x80) == 0) {
        UiNodeList_SuppressActionId(0x1013,&inGameRoot->base);
      }
      scrollableControl = (UiScrollableControl *)INGAME_UI(inGameRoot,technologyDescriptionScroll);
      UiScrollableControl_RebuildViewportAndScrollbars(scrollableControl);
      UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,scrollableControl);
    }
    else {
      ((UiTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->textResourceId = 0x217e;
      technologyAsset = g_TechnologyAsset;
      definitionOrEnergyCost = THANDOR_UI_FIELD(selectedArea.node,-8,int32_t);
      resourceId = (UiNodeBase *)(definitionOrEnergyCost + 1);
      selectedTechnologyId = definitionOrEnergyCost - 0x300000U >> 1;
      xeniteCost = g_TechnologyAsset->records[selectedTechnologyId].xeniteCostQ4;
      costColor = g_RichTextColorPalette0Argb;
      if ((int)g_GameFactionRuntimeImage.records[(firstSelectedEntity->common).ownership.ownerIndex].
               xeniteCurrentQ4 < (int)xeniteCost) {
        costColor = g_RichTextInsufficientResourceColorArgb;
      }
      definitionOrEnergyCost = (int)g_TechnologyAsset->records[selectedTechnologyId].energyCostQ4 >> 4;
      g_InGameSelectedTechnologyId = selectedTechnologyId;
      writtenBytes = g_WideNumberFormatUtf16
                         (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int)xeniteCost >> 4,
                          g_InGameTechnologyXeniteCostTextUtf16);
      *(uint32_t *)((int)g_InGameTechnologyXeniteCostTextUtf16 + writtenBytes) = 0x8000;
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionOrEnergyCost,g_InGameTechnologyEnergyCostTextUtf16);
      packedEnergyColor = definitionOrEnergyCost << 0x10 | costColor >> 0x10;
      g_InGameTechnologyCostColorWords8[1] = (uint16_t)costColor;
      g_InGameTechnologyCostColorWords8[5] = (uint16_t)(costColor >> 0x10);
      g_InGameTechnologyCostColorWords8[0] = (uint16_t)(costColor >> 4);
      g_InGameTechnologyCostColorWords8[4] = (uint16_t)(packedEnergyColor >> 4);
      g_InGameTechnologyCostColorWords8[3] = (uint16_t)(costColor >> 8);
      g_InGameTechnologyCostColorWords8[7] = (uint16_t)(packedEnergyColor >> 8);
      g_InGameTechnologyCostColorWords8[2] = (uint16_t)(costColor >> 0xc);
      g_InGameTechnologyCostColorWords8[6] = (uint16_t)(packedEnergyColor >> 0xc);
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 (int)technologyAsset->records[selectedTechnologyId].researchDurationQ5 >> 5,
                 g_InGameTechnologyResearchTimeTextUtf16);
      resolvedText = TextResource_Resolve((TextResourceId)resourceId);
      labelText = resolvedText.text;
      RichTextCommandStream_PatchPayloadBySelector(0,&g_InGameTechnologyCostRichTextScratch,labelText);
      RichTextCommandStream_PatchPayloadBySelector(1,g_InGameTechnologyEnergyCostTextUtf16,labelText);
      RichTextCommandStream_PatchPayloadBySelector(2,g_InGameTechnologyResearchTimeTextUtf16,labelText)
      ;
      textExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                         (g_UiTextStyleNormal,labelText,
                          ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->wrapWidth);
      INGAME_UI(inGameRoot,technologyDescriptionText)->rightOffset = textExtent.widthPixels + 6;
      INGAME_UI(inGameRoot,technologyDescriptionText)->bottomOffset = textExtent.heightPixels + 6;
      scrollableControl = (UiScrollableControl *)INGAME_UI(inGameRoot,technologyDescriptionScroll);
      if (g_InGameSelectedTechnologyId != previousTechnologyId) {
        UiScrollableControl_RebuildViewportAndScrollbars(scrollableControl);
        UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,scrollableControl);
      }
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)resourceId;
    }
  }
  return;
}

