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
   Handler of the seven technology area tabs, actions INGAME_ACTION_TECHNOLOGY_AREA_TAB1..7 (0x1014..0x101A,
   slots 20..26 of g_InGameUiActionHandlersPage10): a tab that is now selected deselects the other six, then the
   technology window is rebuilt for the chosen area (or the general text when the tab was deselected).
*/
void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl)

{
  UiRootNode *inGameRoot;
  bool isSelected;

  /* climb to the in-game root */
  inGameRoot = (UiRootNode *)selectableControl;
  while ((inGameRoot->base).parent != UI_NODE_NONE) {
    inGameRoot = (UiRootNode *)(inGameRoot->base).parent;
  }
  isSelected = (bool)UiSelectableControl_IsSelected(selectableControl);
  if (isSelected) {
    UiSelectableGroup_SelectExclusive(TECHNOLOGY_AREA_TAB_COUNT,&selectableControl->base,
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
   Opens the technology panel for the first selected entity: suppresses the world view and releases its keyboard
   focus, deselects the seven area tabs and, when the entity is researching (runtime flags +0xEC bit 0x40/0x80),
   selects the tab of the area that holds its current technology; then resets the shown technology to the basic
   one and rebuilds the panel.
*/
void InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot)

{
  uint32_t *tabStateFlags;
  ModelRuntimeSlot *selectedModelRuntime;
  GameEntityRuntime *firstSelectedEntity;
  ModelDefinition *slotView;
  int remainingCount;
  uint32_t areaIndex;

  INGAME_UI(inGameRoot,worldView)->nodeFlags |= UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)INGAME_UI(inGameRoot,worldView));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  selectedModelRuntime = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab1))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab2))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab3))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab4))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab5))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab6))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab7))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (((selectedModelRuntime->classState).stateFlags &
       (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)) != 0) {
    /* The definition lists 28 technology ids from +0x1C8, cycling through the seven areas; the one equal to
       the entity's current technology picks the tab. slotView walks the definition by one slot (4 bytes), so
       researchTechnologyIds1C4[1] seen from it is the current slot (indexing the slot directly changes the
       generated code). */
    slotView = (selectedModelRuntime->definitionOrSavedId).runtimeDefinition;
    areaIndex = 0xffffffff;
    for (remainingCount = TECHNOLOGY_DEFINITION_SLOT_COUNT; remainingCount != 0;
         slotView = (ModelDefinition *)((uint32_t *)slotView + 1), remainingCount--) {
      areaIndex++;
      if (TECHNOLOGY_AREA_TAB_COUNT - 1 < areaIndex) {
        areaIndex = 0;
      }
      if (slotView->researchTechnologyIds1C4[1] == selectedModelRuntime->researchTechnologyId100) {
        tabStateFlags = (uint32_t *)&((UiFramedTextButtonControl *)
                         THANDOR_UI_AT(inGameRoot,g_TechnologyPanelRowFlagOffsets[areaIndex]))->selectable.stateFlags;
        *tabStateFlags = *tabStateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
        break;
      }
    }
  }
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  InGameTechnologyPanel_Rebuild(inGameRoot);
}


/* Address: 0x00569DF0.
   Rebuilds the build catalog (48 entries): the production capabilities come from the selected buildings, or,
   with none selected, from all own models (class 22 adds capability 8, class 13 its definition's flags at
   +0xC4). Every registered army asset with flag 1, a texture and a matching capability that passes the
   technology and ownership/unlock tests (CF results of ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction,
   FactionRuntime_HasArmyAssetOrActiveStructure, ArmyAssetRecord_HasFactionUnlockedLinkedDefinition) gets a
   slot, in a grid of at most eight columns with its texture and Xenite cost; the frame is sized to the grid
   (smaller margins below 800 pixels width) and the panel hidden when the catalog is empty.
*/
void UiCatalogGroup48_RebuildGrid(UiNodeBase *node)

{
  UiNodeFlags *gridNodeFlags;
  WorldOwnerListNode *ownerNode;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  int32_t *layoutOffset;
  int32_t *slotOffsets;
  UiCatalogEntryControl *slotControl;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  uint32_t capabilityFlags;
  uint32_t slotIndex;
  uint32_t columnCount;
  GraphicsTextureSourceAsset *textureAsset;
  ModelDefinition *definition;
  int remainingCount;
  int panelWidth;
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
  factionIndex = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
  capabilityFlags = SelectionInfo_CollectCapabilityFlags();
  if (capabilityFlags == 0) {
    /* runtimePayload is a ModelRuntimeSlot */
    for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (definition = (((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId).runtimeDefinition,
         (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset).armyRuntime->factionIndex ==
         factionIndex)) {
        if (definition->runtimeClassId4C == MODEL_RUNTIME_CLASS_22) {
          capabilityFlags = capabilityFlags | BUILD_CATALOG_ASSET_FLAG_CAPABILITY_8;
        }
        else if (definition->runtimeClassId4C == MODEL_RUNTIME_CLASS_13) {
          capabilityFlags = capabilityFlags | definition->classParameterC4;
        }
      }
    }
  }
  recordCursor = g_UiCatalogGroup48Records;
  for (remainingCount = BUILD_CATALOG_ENTRY_COUNT; remainingCount != 0; remainingCount--) {
    *recordCursor = NULL;
    recordCursor++;
  }
  recordCursor = g_UiCatalogGroup48Records;
  itemCount = 0;
  registryCursor = g_ArmyAssetRecordRegistry;
  for (remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT; remainingCount != 0; registryCursor++, remainingCount--) {
    catalogRecord = (UiCommandRuntimeRecordPrefix *)*registryCursor;
    if (catalogRecord != NULL &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_BUILDABLE) != 0 &&
        (checkResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                            (factionIndex,(ModelDefinitionHierarchyNodeAddress32)catalogRecord), !checkResult) &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_CAPABILITY_MASK) != 0 &&
        catalogRecord->textureSource != NULL &&
        itemCount < BUILD_CATALOG_ENTRY_COUNT &&
        (catalogRecord->assetFlags14 & capabilityFlags) != 0 &&
        ((checkResult = FactionRuntime_HasArmyAssetOrActiveStructure
                             (factionIndex,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult) ||
         (checkResult = ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
                             (factionIndex,capabilityFlags,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult))) {
      *recordCursor = catalogRecord;
      itemCount++;
      recordCursor++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = (uint32_t)gridDimensions;
  if (BUILD_CATALOG_MAX_COLUMNS < columnCount) {
    columnCount = BUILD_CATALOG_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)(gridDimensions >> 32) * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup48ColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset = -31;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).rightOffset = -31;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset = -71;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).bottomOffset = -71;
  }
  else {
    (inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset = -39;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).rightOffset = -39;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset = -89;
    (inGameUiGridView->catalogGroup48LayoutNode5E20).bottomOffset = -89;
  }
  /* reusing one pointer for both offsets keeps the original register allocation */
  layoutOffset = &(inGameUiGridView->catalogGroup48LayoutNode5E20).leftOffset;
  *layoutOffset = *layoutOffset - panelWidth;
  layoutOffset = &(inGameUiGridView->catalogGroup48LayoutNode5E20).topOffset;
  *layoutOffset = *layoutOffset - panelHeight;
  /* taking the flags' address in each branch keeps the original branches (a plain |= / &= becomes cmov) */
  if (itemCount == 0) {
    gridNodeFlags = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *gridNodeFlags = *gridNodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    gridNodeFlags = &(inGameUiGridView->catalogGroup48GridNode5DB4).nodeFlags;
    *gridNodeFlags = *gridNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  /* slot controls (UiCatalogEntryControl) sit at root + offset table entry */
  slotOffsets = g_UiCatalogGroup48OffsetTables[columnCount];
  recordCursor = g_UiCatalogGroup48Records;
  for (slotIndex = 0; slotIndex < BUILD_CATALOG_ENTRY_COUNT; ) {
    slotControl = (UiCatalogEntryControl *)THANDOR_UI_AT(inGameUiGridView,slotOffsets[slotIndex]);
    catalogRecord = *recordCursor;
    if (slotIndex < itemCount) {
      slotControl->command.sprite.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
      xeniteCost = catalogRecord->buildXeniteCostQ4;
      textureAsset = catalogRecord->textureSource;
    }
    else {
      slotControl->command.sprite.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
      xeniteCost = 0;
      textureAsset = NULL;
    }
    slotIndex++;
    slotControl->command.sprite.primaryTextureSource = textureAsset;
    slotControl->runtimeDisplayValueQ4 = xeniteCost;
    recordCursor++;
  }
  (*((inGameUiGridView->catalogGroup48GridNode5DB4).vtable)->layout)
            (&inGameUiGridView->catalogGroup48GridNode5DB4);
  return;
}


/* Address: 0x0056A050.
   Rebuilds the special build catalog (42 entries), offered only while the active faction owns a model of
   runtime class 11: every registered army asset with flags 1 and 0x10 and a texture that passes the technology
   and ownership/unlock tests (CF results as in UiCatalogGroup48_RebuildGrid) gets a slot, in a grid of at most
   six columns with its texture and Xenite cost. The frame is sized to the grid (smaller margins below 800
   pixels width); an empty catalog hides its panel, and also the army stock panel when the stock is empty.
*/
void UiCatalogGroup42_RebuildGrid(UiNodeBase *node)

{
  UiNodeFlags *gridNodeFlags;
  WorldOwnerListNode *ownerNode;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  int32_t *layoutOffset;
  int32_t *slotOffsets;
  UiCatalogEntryControl *slotControl;
  InGameRuntimeUiGridViewC3E4 *inGameUiGridView;
  uint32_t columnCount;
  int factionIndex;
  int panelWidth;
  GraphicsTextureSourceAsset *textureAsset;
  int remainingCount;
  uint32_t itemCount;
  uint32_t slotIndex;
  int structureCount;
  int panelHeight;
  UiCommandRuntimeRecordPrefix **recordCursor;
  ArmyAssetRecordPrefix **registryCursor;
  bool checkResult;
  UiGridDimensionsEdxEax8 gridDimensions;
  ArmyBuildXeniteCostQ4 xeniteCost;
  
  inGameUiGridView = (InGameRuntimeUiGridViewC3E4 *)UiNode_GetRoot(node);
  factionIndex = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
  /* count the faction's class-11 models (runtimePayload is a ModelRuntimeSlot) */
  structureCount = 0;
  for (ownerNode = (inGameUiGridView->worldRuntime0A30).ownerListHead;
      ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL &&
        (((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId).runtimeDefinition->runtimeClassId4C ==
        MODEL_RUNTIME_CLASS_11 &&
        (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset).armyRuntime->factionIndex ==
        factionIndex) {
      structureCount++;
    }
  }
  recordCursor = g_UiCatalogGroup42Records;
  for (remainingCount = SPECIAL_BUILD_CATALOG_ENTRY_COUNT; remainingCount != 0; remainingCount--) {
    *recordCursor = NULL;
    recordCursor++;
  }
  recordCursor = g_UiCatalogGroup42Records;
  itemCount = 0;
  registryCursor = g_ArmyAssetRecordRegistry;
  for (remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT; remainingCount != 0; registryCursor++, remainingCount--) {
    catalogRecord = (UiCommandRuntimeRecordPrefix *)*registryCursor;
    if (catalogRecord != NULL &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_BUILDABLE) != 0 &&
        (checkResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                            (factionIndex,(ModelDefinitionHierarchyNodeAddress32)catalogRecord), !checkResult) &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_SPECIAL) != 0 &&
        catalogRecord->textureSource != NULL &&
        itemCount < SPECIAL_BUILD_CATALOG_ENTRY_COUNT &&
        structureCount != 0 &&
        ((checkResult = FactionRuntime_HasArmyAssetOrActiveStructure
                             (factionIndex,(ArmyAssetRecordPrefix *)catalogRecord), !checkResult) ||
         (checkResult = ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
                             (factionIndex,BUILD_CATALOG_ASSET_FLAG_SPECIAL,(ArmyAssetRecordPrefix *)catalogRecord),
          !checkResult))) {
      *recordCursor = catalogRecord;
      itemCount++;
      recordCursor++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(7,itemCount);
  columnCount = (uint32_t)gridDimensions;
  if (SPECIAL_BUILD_CATALOG_MAX_COLUMNS < columnCount) {
    columnCount = SPECIAL_BUILD_CATALOG_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)(gridDimensions >> 32) * g_InGamePanelTextureSubresource34Height +
           g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup42ColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset = -31;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).rightOffset = -31;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset = -42;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).bottomOffset = -42;
  }
  else {
    (inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset = -39;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).rightOffset = -39;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset = -55;
    (inGameUiGridView->catalogGroup42LayoutNode76EC).bottomOffset = -55;
  }
  /* pointer forms as in UiCatalogGroup48_RebuildGrid (they keep the original code) */
  layoutOffset = &(inGameUiGridView->catalogGroup42LayoutNode76EC).leftOffset;
  *layoutOffset = *layoutOffset - panelWidth;
  layoutOffset = &(inGameUiGridView->catalogGroup42LayoutNode76EC).topOffset;
  *layoutOffset = *layoutOffset - panelHeight;
  if (itemCount == 0) {
    factionIndex = (inGameUiGridView->worldRuntime0A30).activeFactionRuntimeIndex;
    gridNodeFlags = &(inGameUiGridView->catalogGroup42GridNode7680).nodeFlags;
    *gridNodeFlags = *gridNodeFlags | UI_NODE_SUPPRESSED;
    if (g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount == 0) {
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
  /* slot controls (UiCatalogEntryControl) as in UiCatalogGroup48_RebuildGrid */
  slotOffsets = g_UiCatalogGroup42OffsetTables[columnCount];
  recordCursor = g_UiCatalogGroup42Records;
  for (slotIndex = 0; slotIndex < SPECIAL_BUILD_CATALOG_ENTRY_COUNT; ) {
    slotControl = (UiCatalogEntryControl *)THANDOR_UI_AT(inGameUiGridView,slotOffsets[slotIndex]);
    catalogRecord = *recordCursor;
    if (slotIndex < itemCount) {
      slotControl->command.sprite.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
      xeniteCost = catalogRecord->buildXeniteCostQ4;
      textureAsset = catalogRecord->textureSource;
    }
    else {
      slotControl->command.sprite.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
      xeniteCost = 0;
      textureAsset = NULL;
    }
    slotIndex++;
    slotControl->command.sprite.primaryTextureSource = textureAsset;
    slotControl->runtimeDisplayValueQ4 = xeniteCost;
    recordCursor++;
  }
  (*((inGameUiGridView->catalogGroup42GridNode7680).vtable)->layout)
            (&inGameUiGridView->catalogGroup42GridNode7680);
  return;
}


/* Address: 0x0056AEF0.
   Handler of the technology window's research button, action INGAME_ACTION_TECHNOLOGY_RESEARCH (0x1013, slot 19
   of g_InGameUiActionHandlersPage10): closes the window (world view shown again, game-window page 0) and sends
   INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE for the first selected building with the technology of the selected area
   tab, which starts that research; with no tab selected the technology argument is 0 and nothing starts.
*/
void InGameTechnologyResearch_StartSelected(void *source)

{
  GameEntityRuntime *firstSelectedEntity;
  uint32_t doubledTechnologyId;
  CommandPayloadDword04 modelOffset;
  SelectableGroupNodeResult selectedArea;

  /* climb to the in-game root */
  while (((UiNodeBase *)source)->parent != UI_NODE_NONE) {
    source = ((UiNodeBase *)source)->parent;
  }
  INGAME_UI(source,worldView)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(source,gameWindowPageStack));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != NULL) {
    modelOffset = (int)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
                  g_ModelRuntimeRebaseDelta;
    doubledTechnologyId = 0;
    selectedArea = UiSelectableGroup_NoneVisibleSelected(TECHNOLOGY_AREA_TAB_COUNT,
      INGAME_UI(source,technologyAreaTab7),
      INGAME_UI(source,technologyAreaTab6),
      INGAME_UI(source,technologyAreaTab5),
      INGAME_UI(source,technologyAreaTab4),
      INGAME_UI(source,technologyAreaTab3),
      INGAME_UI(source,technologyAreaTab2),
      INGAME_UI(source,technologyAreaTab1));
    if (!selectedArea.noneSelected) {
      /* the dword 8 bytes before the selected area tab holds its name text id, TECHNOLOGY_TEXT_ID_BASE +
         2 * technology id (see InGameTechnologyPanel_Rebuild) */
      doubledTechnologyId = ((UiTechnologyAreaTabPrefix *)selectedArea.node)[-1].nameTextResourceId - TECHNOLOGY_TEXT_ID_BASE;
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,doubledTechnologyId >> 1,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE,0,doubledTechnologyId >> 1,
                                                  modelOffset);
    }
  }
  return;
}


/* Address: 0x0056B050.
   Rebuilds the technology window for the first selected entity: the research button (off for definitions with
   flag 0x40 at +0xEC), the unit picture, and one area tab per technology the owner may research (at most seven;
   the definition's 28 technology slots are dealt out cyclically over the tab slots 6..0), each labelled with the
   technology name and its Xenite cost. With no tab selected it shows the general text; otherwise the selected
   technology's description with its Xenite cost (red when unaffordable), Energy cost and research time.
*/
void InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot)

{
  UiScrollableControl *scrollableControl;
  ModelRuntimeSlot *entityModelRuntime;
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
  TextResourceId descriptionTextId;
  int slotIndex;
  ModelDefinition *definition;
  int nameTextId;
  int energyCost;
  uint32_t packedEnergyColor;
  uint32_t costColor;
  int areaIndex;
  bool isAvailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextResolveResult resolvedName;
  ArmyAssetLookupResult armyRecord;
  SelectableGroupNodeResult selectedArea;
  uint16_t *labelTemplate;
  UiActionId actionId;
  
  previousTechnologyId = g_InGameSelectedTechnologyId;
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != NULL) {
    entityModelRuntime = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
    definition = (entityModelRuntime->definitionOrSavedId).runtimeDefinition;
    if (((entityModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      UiNodeList_UnsuppressActionId(INGAME_ACTION_TECHNOLOGY_RESEARCH,&inGameRoot->base);
    }
    else {
      UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_RESEARCH,&inGameRoot->base);
    }
    /* window title with the unit name patched in */
    resolvedText = TextResource_Resolve(TEXT_ID_TECHNOLOGY_WINDOW_TITLE);
    resolvedName = TextResource_Resolve(((ModelDefinitionRecordPrefix *)definition)->flags + TEXT_ID_MODEL_NAME_BASE);
    RichTextCommandStream_PatchPayloadBySelector(0,resolvedName.text,resolvedText.text);
    armyRecord = ArmyAssetRegistry_FindById((firstSelectedEntity->common).runtimeIdentityOrArmyAssetId);
    ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyDescriptionFrame))->textureSource =
         (GraphicsTextureSourceAsset *)armyRecord.recordOrError[1].rootNodeOffsetOrPointer;
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB1,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB2,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB3,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB4,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB5,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB6,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB7,&inGameRoot->base);
    /* technology slots 28..1 (definition +0x1C8..), area tabs cycling 6..0 */
    slotIndex = TECHNOLOGY_DEFINITION_SLOT_COUNT;
    areaIndex = TECHNOLOGY_AREA_TAB_COUNT - 1;
    do {
      isAvailable = Technology_IsAvailableForFaction
                         (definition->researchTechnologyIds1C4[slotIndex],
                          (firstSelectedEntity->common).ownership.ownerIndex);
      if (isAvailable) {
        technologyId = definition->researchTechnologyIds1C4[slotIndex];
        /* the area tab's icon (technologyAreaTabNIcon) shows subresource technologyId of tech.gfx */
        ((UiImagePanelControl *)
         THANDOR_UI_AT(inGameRoot,g_TechnologyPanelRowValueOffsets[areaIndex]))->
        subresource = technologyId;
        rowFlagOffset = g_TechnologyPanelRowFlagOffsets[areaIndex];
        /* rowFlagOffset is the area tab technologyAreaTabN; the dwords 8 and 4 bytes before it (the +0x60/+0x64
           slots of the preceding 0x68-byte text button) hold the area's text id and a text pointer */
        actionId = ((UiSelectableControl *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))->actionId;
        ((UiTechnologyAreaTabPrefix *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))[-1].nameTextResourceId =
             technologyId * 2 + TECHNOLOGY_TEXT_ID_BASE;
        /* tab label: the technology name (payload 0) and Xenite cost (payload 1), the number formatted into the
           label text buffer at word 0xC0; the expanded label becomes the tab's tooltip text */
        resolvedText = TextResource_Resolve(TEXT_ID_TECHNOLOGY_AREA_TAB_LABEL);
        labelText = resolvedText.text;
        labelTemplate = labelText;
        resolvedText = TextResource_Resolve
                                 (((UiTechnologyAreaTabPrefix *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))[-1].
                                  nameTextResourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,resolvedText.text,labelText);
        labelText = ((UiTechnologyAreaTabPrefix *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))[-1].tooltipText;
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                   (int)g_TechnologyAsset->records[definition->researchTechnologyIds1C4[slotIndex]].xeniteCostQ4 >> 4,
                   labelTemplate + 0xc0);
        RichTextCommandStream_PatchPayloadBySelector(1,labelTemplate + 0xc0,labelTemplate);
        RichTextCommandStream_CopyExpanded(0x180,labelText,labelTemplate);
        UiNodeList_UnsuppressActionId(actionId,&inGameRoot->base);
      }
      areaIndex--;
      if (areaIndex < 0) {
        areaIndex = TECHNOLOGY_AREA_TAB_COUNT - 1;
      }
      slotIndex--;
    } while (slotIndex != 0);
    selectedArea = UiSelectableGroup_NoneVisibleSelected(TECHNOLOGY_AREA_TAB_COUNT,
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
      /* the text field holds a text resource id here, resolved when drawn */
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)TEXT_ID_TECHNOLOGY_GENERAL_DESCRIPTION;
      ((UiTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->textResourceId =
           TEXT_ID_TECHNOLOGY_BUTTON_NO_AREA;
      INGAME_UI(inGameRoot,technologyDescriptionText)->rightOffset = 6;
      INGAME_UI(inGameRoot,technologyDescriptionText)->bottomOffset = 6;
      if ((playerBlock->assignmentFlags80A4 & 0x80) == 0) {
        UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_RESEARCH,&inGameRoot->base);
      }
      scrollableControl = (UiScrollableControl *)INGAME_UI(inGameRoot,technologyDescriptionScroll);
      UiScrollableControl_RebuildViewportAndScrollbars(scrollableControl);
      UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,scrollableControl);
    }
    else {
      ((UiTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->textResourceId =
           TEXT_ID_TECHNOLOGY_BUTTON_AREA_SELECTED;
      technologyAsset = g_TechnologyAsset;
      /* the selected tab's name text id; the description is the next text */
      nameTextId = ((UiTechnologyAreaTabPrefix *)selectedArea.node)[-1].nameTextResourceId;
      descriptionTextId = nameTextId + 1;
      selectedTechnologyId = (nameTextId - (uint32_t)TECHNOLOGY_TEXT_ID_BASE) >> 1;
      xeniteCost = g_TechnologyAsset->records[selectedTechnologyId].xeniteCostQ4;
      costColor = g_RichTextColorPalette0Argb;
      if ((int)g_GameFactionRuntimeImage.records[(firstSelectedEntity->common).ownership.ownerIndex].
               xeniteCurrentQ4 < (int)xeniteCost) {
        costColor = g_RichTextInsufficientResourceColorArgb;
      }
      energyCost = (int)g_TechnologyAsset->records[selectedTechnologyId].energyCostQ4 >> 4;
      g_InGameSelectedTechnologyId = selectedTechnologyId;
      writtenBytes = g_WideNumberFormatUtf16
                         (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int)xeniteCost >> 4,
                          g_InGameTechnologyXeniteCostTextUtf16);
      /* behind the number: palette colour 0 again, then the terminator */
      *(uint32_t *)((uint8_t *)g_InGameTechnologyXeniteCostTextUtf16 + writtenBytes) =
           RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,energyCost,g_InGameTechnologyEnergyCostTextUtf16);
      packedEnergyColor = energyCost << 0x10 | costColor >> 0x10;
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
      resolvedText = TextResource_Resolve(descriptionTextId);
      labelText = resolvedText.text;
      RichTextCommandStream_PatchPayloadBySelector(0,&g_InGameTechnologyCostRichTextScratch,labelText);
      RichTextCommandStream_PatchPayloadBySelector(1,g_InGameTechnologyEnergyCostTextUtf16,labelText);
      RichTextCommandStream_PatchPayloadBySelector(2,g_InGameTechnologyResearchTimeTextUtf16,labelText);
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
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)descriptionTextId;
    }
  }
  return;
}

