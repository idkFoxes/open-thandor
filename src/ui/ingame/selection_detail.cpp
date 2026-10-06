/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_detail.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_detail.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/core/bytes.h>

/* Module data. */

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailNameTextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailArmourTextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName0TextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName1TextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName2TextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot05Utf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildXeniteCostTextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildTimeTextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailEnergyTextUtf16 = {};

UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot09Utf16 = {};

/* Byte offsets of the 12 grid cells of the multi-selection page (multiSelectionCell00..11, UiArmyMetricsPanel) in
   the in-game UI image. */
int g_InGameSelectionDetailGridCellOffsets[12] = {
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell00)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell01)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell02)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell03)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell04)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell05)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell06)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell07)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell08)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell09)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell10)),
    static_cast<int>(offsetof(InGameUiImage,multiSelectionCell11))};

/* Copies a 64-character name text into one of the selection detail text slots. */
static void InGameSelectionDetailPanel_CopyName(uint16_t *destination,const uint16_t *source)
{
  int remaining;

  for (remaining = 64; remaining != 0; remaining--) {
    *destination = *source;
    source++;
    destination++;
  }
}

/* Shared tail of pages 1 and 3: copies nameSource into nameDestination, resets the three weapon names to the
   "no weapon" text and then names the faction-unlocked definitions of up to three child lists of
   linkedDefinitionListView. */
static void InGameSelectionDetailPanel_FillLinkedDefinitionNames
          (InGameRuntimeRoot *root,uint16_t *nameDestination,const uint16_t *nameSource,
           ArmyModelTreeNodeAddressView *linkedDefinitionListView)
{
  uint16_t *noWeaponText;
  ModelDefinitionRecordPrefix *unlockedDefinition;

  InGameSelectionDetailPanel_CopyName(nameDestination,nameSource);
  noWeaponText = TextResource_Resolve(TEXT_ID_SELECTION_DETAIL_NO_WEAPON);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName0TextUtf16,noWeaponText,nullptr);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName1TextUtf16,noWeaponText,nullptr);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName2TextUtf16,noWeaponText,nullptr);
  if (linkedDefinitionListView->childListCount == 0) {
    return;
  }
  unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                     (root->worldRuntime.activeFactionRuntimeIndex,linkedDefinitionListView->childList0Address);
  InGameSelectionDetailPanel_CopyName
            (g_InGameSelectionDetailWeaponName0TextUtf16,
             TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE));
  if (linkedDefinitionListView->childListCount <= 1) {
    return;
  }
  unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                     (root->worldRuntime.activeFactionRuntimeIndex,linkedDefinitionListView->childList1Address);
  InGameSelectionDetailPanel_CopyName
            (g_InGameSelectionDetailWeaponName1TextUtf16,
             TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE));
  if (linkedDefinitionListView->childListCount <= 2) {
    return;
  }
  unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                     (root->worldRuntime.activeFactionRuntimeIndex,linkedDefinitionListView->childList2Address);
  InGameSelectionDetailPanel_CopyName
            (g_InGameSelectionDetailWeaponName2TextUtf16,
             TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE));
}

/* Page 1: one selected entity of the active faction. */
static void InGameSelectionDetailPanel_ShowSingleEntity
          (InGameRuntimeRoot *root,UiPageStackControl *stack,GameEntityRuntime *entity)
{
  int *classRecordWords;
  ModelDefinition *entityDefinition;
  int technologySlot;
  uint32_t armyLookupError;
  ArmyAssetRecordPrefix *foundArmyAsset;
  ArmyAssetRecord *armyAsset;
  uint32_t selectionDetailValue;
  int armour;
  uint32_t activeMetric;
  uint16_t *noWeaponText;
  ModelRuntimeSlot *modelRuntime;
  ModelRuntimeSlot *attachedModelRuntime;
  uint32_t researchTechnologyId;
  int runtimeClassId;
  ArmyAssetRecordPrefix *linkedArmyAsset;
  ArmyModelTreeNodeAddressView *linkedDefinitionListView;
  ModelDefinitionRecordPrefix *unlockedDefinition;

  classRecordWords = static_cast<int *>(entity->common.ownership.definitionOrClassRecord.get());
  entityDefinition = entity->common.ownership.modelRuntime()->definitionOrSavedId.runtimeDefinition;
  if (!FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
         ((uintptr_t)classRecordWords,g_InGameRuntimeRoot->worldRuntime.selection.activePlayerRuntimeId)) {
    /* The technology button stays available when any of the 28 technology slots is available. */
    UiNodeList_UnsuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,reinterpret_cast<UiNodeBase *>(root));
    for (technologySlot = 28; technologySlot != 0; technologySlot--) {
      if (Technology_IsAvailableForFaction
            ((PckTechnologyIdCatalog)entityDefinition->researchTechnologyIds[technologySlot],
             entity->common.ownership.ownerIndex)) {
        break;
      }
    }
    if (technologySlot == 0) {
      UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,reinterpret_cast<UiNodeBase *>(root));
    }
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_WINDOW,reinterpret_cast<UiNodeBase *>(root));
  }
  armyLookupError = ArmyAssetRegistry_FindById(entity->common.runtimeIdentityOrArmyAssetId,&foundArmyAsset);
  armyAsset = reinterpret_cast<ArmyAssetRecord *>(FatalError_ExitIfFailed
                (armyLookupError != 0 ? armyLookupError : (uintptr_t)foundArmyAsset,armyLookupError != 0));
  UiPageStack_SetActiveIndex(1,stack);
  selectionDetailValue = armyAsset->selectionDetailValue;
  armour = ModelRuntimeHierarchy_SumArmour((int *)entity);
  root->selectionDetailArmyAssetValue = selectionDetailValue;
  root->selectionDetailEntity = entity;
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armour,g_InGameSelectionDetailArmourTextUtf16);
  activeMetric = ModelRuntime_QueryActiveHierarchyMetric(ModelView_Cast<ArmyRuntimeSlot>(entity));
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,activeMetric >> 4,g_InGameSelectionDetailEnergyTextUtf16);
  InGameUi_Image(root)->singleSelectionStatsText.text = reinterpret_cast<uint16_t *>
       (static_cast<uintptr_t>(armyAsset->selectionDetailTemplateVariantIndex + TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE));
  InGameSelectionDetailPanel_CopyName
            (g_InGameSelectionDetailNameTextUtf16,
             TextResource_Resolve(entity->common.ownership.modelRuntime()->definitionOrSavedId.runtimeDefinition->
                                  nameTextIndex + TEXT_ID_MODEL_NAME_BASE));
  /* text 0x18004E fills unused weapon slots; name texts are 0x18004F + the definition's name index */
  noWeaponText = TextResource_Resolve(TEXT_ID_SELECTION_DETAIL_NO_WEAPON);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName0TextUtf16,noWeaponText,nullptr);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName1TextUtf16,noWeaponText,nullptr);
  RichTextCommandStream_CopyExpanded(128,g_InGameSelectionDetailWeaponName2TextUtf16,noWeaponText,nullptr);
  g_InGameSelectionDetailTextSlot05Utf16[0] = L'-';
  g_InGameSelectionDetailTextSlot05Utf16[1] = 0;
  g_InGameSelectionDetailTextSlot09Utf16[0] = L'-';
  g_InGameSelectionDetailTextSlot09Utf16[1] = 0;
  modelRuntime = entity->common.ownership.modelRuntime();
  if ((modelRuntime->classState.stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) {
    /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
       foundArmyAsset) */
    ArmyAssetRegistry_FindById(entity->common.runtimeIdentityOrArmyAssetId,&foundArmyAsset);
    researchTechnologyId = modelRuntime->researchTechnologyId;
    InGameUi_Image(root)->singleSelectionStatsText.text = reinterpret_cast<uint16_t *>
         (static_cast<uintptr_t>(foundArmyAsset->selectionDetailTemplateVariantIndex +
                                 TEXT_ID_SELECTION_DETAIL_RESEARCH_TEMPLATE_BASE));
    RichTextCommandStream_CopyExpanded
              (128,g_InGameSelectionDetailTextSlot09Utf16,
               TextResource_Resolve(researchTechnologyId * 2 + TECHNOLOGY_TEXT_ID_BASE),nullptr);
  }
  /* Weapon names: the attached models of the first three attachment slots (the model runtime is re-read from
     the entity before each further slot). */
  if (modelRuntime->attachmentCount != 0) {
    attachedModelRuntime = modelRuntime->attachments[ARMY_WEAPON_SLOT_PRIMARY].childModelRuntimeOrSavedOffset;
    if (attachedModelRuntime != nullptr) {
      InGameSelectionDetailPanel_CopyName
                (g_InGameSelectionDetailWeaponName0TextUtf16,
                 TextResource_Resolve(attachedModelRuntime->definitionOrSavedId.definition->nameTextIndex +
                                      TEXT_ID_MODEL_NAME_BASE));
    }
    modelRuntime = entity->common.ownership.modelRuntime();
    if (1 < modelRuntime->attachmentCount) {
      attachedModelRuntime = modelRuntime->attachments[ARMY_WEAPON_SLOT_SECONDARY].childModelRuntimeOrSavedOffset;
      if (attachedModelRuntime != nullptr) {
        InGameSelectionDetailPanel_CopyName
                  (g_InGameSelectionDetailWeaponName1TextUtf16,
                   TextResource_Resolve(attachedModelRuntime->definitionOrSavedId.definition->nameTextIndex +
                                        TEXT_ID_MODEL_NAME_BASE));
      }
      modelRuntime = entity->common.ownership.modelRuntime();
      if (2 < modelRuntime->attachmentCount) {
        attachedModelRuntime = modelRuntime->attachments[ARMY_WEAPON_SLOT_TERTIARY].childModelRuntimeOrSavedOffset;
        if (attachedModelRuntime != nullptr) {
          InGameSelectionDetailPanel_CopyName
                    (g_InGameSelectionDetailWeaponName2TextUtf16,
                     TextResource_Resolve(attachedModelRuntime->definitionOrSavedId.definition->nameTextIndex +
                                          TEXT_ID_MODEL_NAME_BASE));
        }
      }
    }
  }
  /* Linked army asset: class 0x16 checks word 43, classes 0x0B/0x0D check word 46 of the class record; class
     0x0E only prints word 24 as a number. */
  classRecordWords = static_cast<int *>(entity->common.ownership.definitionOrClassRecord.get());
  runtimeClassId = entity->common.ownership.modelRuntime()->definitionOrSavedId.runtimeDefinition->runtimeClassId;
  if (runtimeClassId == MODEL_RUNTIME_CLASS_22) {
    if (classRecordWords[43] != 1) {
      return;
    }
  }
  else if ((runtimeClassId == MODEL_RUNTIME_CLASS_11) || (runtimeClassId == MODEL_RUNTIME_CLASS_13)) {
    if (classRecordWords[46] != 1) {
      return;
    }
  }
  else {
    if (runtimeClassId == MODEL_RUNTIME_CLASS_14) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,classRecordWords[24],
                 g_InGameSelectionDetailWeaponName0TextUtf16);
    }
    return;
  }
  /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
     linkedArmyAsset) */
  ArmyAssetRegistry_FindById(classRecordWords[24],&linkedArmyAsset);
  linkedDefinitionListView = Thandor_U32ToPointer<ArmyModelTreeNodeAddressView>(linkedArmyAsset->rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer */
  if (linkedArmyAsset->selectionDetailTemplateVariantIndex < 8) {
    InGameUi_Image(root)->singleSelectionStatsText.text = reinterpret_cast<uint16_t *>
         (static_cast<uintptr_t>(InGameUi_Image(root)->singleSelectionStatsText.text) +
          linkedArmyAsset->selectionDetailTemplateVariantIndex);
  }
  unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                     (root->worldRuntime.activeFactionRuntimeIndex,
                      (uintptr_t)linkedDefinitionListView);
  InGameSelectionDetailPanel_FillLinkedDefinitionNames
            (root,g_InGameSelectionDetailTextSlot05Utf16,
             TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE),
             linkedDefinitionListView);
}

/* Page 2: fills up to 12 UiArmyMetricsPanel grid cells (entity, and the army asset's dword into textureSource)
   from the selection slots, then clears the remaining cells. */
static void InGameSelectionDetailPanel_ShowEntityGrid(InGameRuntimeRoot *root,UiPageStackControl *stack)
{
  int *gridCellOffset;
  int remainingCells;
  int slotIndex;
  UiArmyMetricsPanel *cell;
  Ptr32<GameEntityRuntime> *entitySlots;
  GameEntityRuntime *entity;
  ArmyAssetRecordPrefix *foundArmyAsset;
  uint8_t *clearedControlBytes;

  UiPageStack_SetActiveIndex(2,stack);
  gridCellOffset = g_InGameSelectionDetailGridCellOffsets;
  remainingCells = 12;
  entitySlots = g_SelectionInfoEntitySlots->entries;
  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    entity = entitySlots[slotIndex];
    if ((entity != nullptr) && (remainingCells != 0)) {
      cell = Thandor_At<UiArmyMetricsPanel>(root,*gridCellOffset);
      cell->entity = (RuntimeModelFactionPrefix *)entity;
      /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
         foundArmyAsset) */
      ArmyAssetRegistry_FindById(entity->common.runtimeIdentityOrArmyAssetId,&foundArmyAsset);
      cell->base.textureSource =
           Thandor_U32ToPointer<GraphicsTextureSourceAsset>(foundArmyAsset[1].registryId); /* 5f-format: ArmyAssetRecord +0x18 (dword read as texture source) */
      remainingCells--;
      gridCellOffset++;
    }
  }
  for (; remainingCells != 0; remainingCells--) {
    clearedControlBytes = Thandor_Bytes(&Thandor_At<UiArmyMetricsPanel>(root,*gridCellOffset)->base.textureSource);
    clearedControlBytes[0] = 0;
    clearedControlBytes[1] = 0;
    clearedControlBytes[2] = 0;
    clearedControlBytes[3] = 0;
    gridCellOffset++;
  }
  /* (Text slots 05/09 are not touched on this page.) */
}

/* Page 3: the hovered stock/build record. */
static void InGameSelectionDetailPanel_ShowHoverRecord
          (InGameRuntimeRoot *root,UiPageStackControl *stack,UiCommandRuntimeRecordPrefix *hoverRecord)
{
  GraphicsTextureSourceAsset *hoverTextureSource;
  uint32_t armour;
  uint32_t buildXeniteCostQ4;
  uint32_t buildDurationQ5;
  EnergyDemandQ4 displayedEnergy;
  int statsTemplateTextId;
  ArmyModelTreeNodeAddressView *linkedDefinitionListView;
  ModelDefinitionRecordPrefix *unlockedDefinition;

  UiPageStack_SetActiveIndex(3,stack);
  hoverTextureSource = hoverRecord->textureSource;
  armour = ArmyAssetHierarchy_SumFactionUnlockedArmour
             (root->worldRuntime.activeFactionRuntimeIndex,(ModelDefinitionHierarchyNodeAddress32)hoverRecord);
  InGameUi_Image(root)->hoverItemIcon.textureSource = hoverTextureSource;
  buildXeniteCostQ4 = hoverRecord->buildXeniteCostQ4;
  buildDurationQ5 = hoverRecord->buildDurationQ5;
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armour,g_InGameSelectionDetailArmourTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,buildXeniteCostQ4 >> 4,
             g_InGameSelectionDetailBuildXeniteCostTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,buildDurationQ5 >> 5,g_InGameSelectionDetailBuildTimeTextUtf16);
  displayedEnergy = ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
                      (root->worldRuntime.activeFactionRuntimeIndex,
                       (ModelDefinitionHierarchyNodeAddress32)hoverRecord);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,displayedEnergy >> 4,g_InGameSelectionDetailEnergyTextUtf16);
  statsTemplateTextId = hoverRecord->selectionDetailTemplateVariantIndex + TEXT_ID_SELECTION_DETAIL_HOVER_TEMPLATE_BASE;
  InGameUi_Image(root)->hoverItemStatsText.text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(statsTemplateTextId));
  InGameUi_Image(root)->unitPlacementStatsText.text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(statsTemplateTextId));
  linkedDefinitionListView = Thandor_U32ToPointer<ArmyModelTreeNodeAddressView>(hoverRecord->rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer (UiCommandRuntimeRecordPrefix view) */
  unlockedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                     (root->worldRuntime.activeFactionRuntimeIndex,
                      (uintptr_t)linkedDefinitionListView);
  InGameSelectionDetailPanel_FillLinkedDefinitionNames
            (root,g_InGameSelectionDetailNameTextUtf16,
             TextResource_Resolve(unlockedDefinition->nameTextIndex + TEXT_ID_MODEL_NAME_BASE),
             linkedDefinitionListView);
}

/* Rebuilds the selection detail panel (page stack: 0 empty, 1 one own entity, 2 grid of up to 12 own entities,
   3 the hovered stock/build record). Page 1 shows armour, energy, name and up to three weapon names of the
   entity plus the name of its linked army asset (definition classes 0x0B/0x0D/0x16), and enables the technology button only when a technology is
   available; page 3 shows the hovered record's armour, costs, build time, energy, name and weapons.
*/
void InGameSelectionDetailPanel_Rebuild()

{
  UiCommandRuntimeRecordPrefix *hoverRecord;
  InGameRuntimeRoot *root;
  int activeFactionIndex;
  int selectedCount;
  int slotIndex;
  Ptr32<GameEntityRuntime> *entitySlots;
  GameEntityRuntime *lastSelectedEntity;
  UiPageStackControl *stack;

  hoverRecord = g_UiHoverSelectionRecord;
  root = g_InGameRuntimeRoot;
  if (root == nullptr) {
    return;
  }
  activeFactionIndex = root->worldRuntime.activeFactionRuntimeIndex;
  selectedCount = 0;
  lastSelectedEntity = nullptr;
  entitySlots = g_SelectionInfoEntitySlots->entries;
  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    if (entitySlots[slotIndex] != nullptr) {
      selectedCount++;
      lastSelectedEntity = entitySlots[slotIndex];
    }
  }
  stack = &root->selectionDetailPageStack;
  if (hoverRecord != nullptr) {
    InGameSelectionDetailPanel_ShowHoverRecord(root,stack,hoverRecord);
    return;
  }
  if ((selectedCount == 1) && (activeFactionIndex == lastSelectedEntity->common.ownership.ownerIndex)) {
    InGameSelectionDetailPanel_ShowSingleEntity(root,stack,lastSelectedEntity);
    return;
  }
  if ((selectedCount > 1) && (activeFactionIndex == lastSelectedEntity->common.ownership.ownerIndex)) {
    InGameSelectionDetailPanel_ShowEntityGrid(root,stack);
    return;
  }
  UiPageStack_SetActiveIndex(0,stack);
}
