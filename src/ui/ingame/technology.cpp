/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/technology.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/technology.h>
#include <thandor/thandor.h>

/* Module data. */

PckTechnologyIdCatalog g_InGameSelectedTechnologyId = 0;

/* one rich-text stream, patched in as payload 0 of the technology label:
   [0] command unit 0x8006 (RICHTEXT_OP_LITERAL_COLOR), [1..8] its eight colour digits
   [9..24] the xenite cost text (RICHTEXT_RECORD_UNITS_LITERAL_COLOR units in); the interpreter reads on from the
   colour command into the text */
static uint16_t g_InGameTechnologyCostRichText[25] = {32774};

static UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyEnergyCostTextUtf16 = {0};

static UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyResearchTimeTextUtf16 = {0};

static int g_TechnologyPanelRowFlagOffsets[7] = {5444, 5548, 5652, 5756, 5860, 5964, 6068};

static int g_TechnologyPanelRowValueOffsets[7] = {6164, 6256, 6348, 6440, 6532, 6624, 6716};

/* Parts of the cost rich-text stream g_InGameTechnologyCostRichText: the literal-colour command, its eight colour
   digits, then the xenite cost text. */
enum {
  TECHNOLOGY_COST_TEXT_COLOR_DIGITS = 1,
  TECHNOLOGY_COST_TEXT_XENITE = RICHTEXT_RECORD_UNITS_LITERAL_COLOR
};

/* Implementation ownership: ui/ingame/technology. */

/* Handler of the seven technology area tabs, actions INGAME_ACTION_TECHNOLOGY_AREA_TAB1..7 (0x1014..0x101A,
   slots 20..26 of g_InGameUiActionHandlersPage10): a tab that is now selected deselects the other six, then the
   technology window is rebuilt for the chosen area (or the general text when the tab was deselected).
*/
void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl)

{
  UiRootNode *inGameRoot;
  Bool8 isSelected;

  /* climb to the in-game root */
  inGameRoot = (UiRootNode *)selectableControl;
  while ((inGameRoot->base).parent != UI_NODE_NONE) {
    inGameRoot = (UiRootNode *)(inGameRoot->base).parent;
  }
  isSelected = (Bool8)UiSelectableControl_IsSelected(selectableControl);
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

/* Opens the technology panel for the first selected entity: suppresses the world view and releases its keyboard
   focus, deselects the seven area tabs and, when the entity is researching (classState.stateFlags ARMY_MODEL_STATE_RESEARCHING or
   ARMY_MODEL_STATE_RESEARCH_UNPAID),
   selects the tab of the area that holds its current technology; then resets the shown technology to the basic
   one and rebuilds the panel.
*/
void InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot)

{
  ModelRuntimeSlot *selectedModelRuntime;
  GameEntityRuntime *firstSelectedEntity;
  ModelDefinition *definition;
  int slotIndex;
  uint32_t areaIndex;

  INGAME_UI(inGameRoot,worldView)->nodeFlags |= UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)INGAME_UI(inGameRoot,worldView));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  selectedModelRuntime = (ModelRuntimeSlot *)(firstSelectedEntity->common).ownership.definitionOrClassRecord;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab1))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab2))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab3))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab4))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab5))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab6))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab7))->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (((selectedModelRuntime->classState).stateFlags &
       (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)) != 0) {
    /* The definition lists 28 technology ids (researchTechnologyIds[1..28]), cycling through the seven areas
       0..6; the one equal to the entity's current technology picks the tab. */
    definition = (selectedModelRuntime->definitionOrSavedId).runtimeDefinition;
    areaIndex = 0;
    for (slotIndex = 1; slotIndex <= TECHNOLOGY_DEFINITION_SLOT_COUNT; slotIndex++) {
      if (definition->researchTechnologyIds[slotIndex] == selectedModelRuntime->researchTechnologyId) {
        ((UiFramedTextButtonControl *)THANDOR_UI_AT(inGameRoot,g_TechnologyPanelRowFlagOffsets[areaIndex]))->
          selectable.stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
        break;
      }
      areaIndex++;
      if (TECHNOLOGY_AREA_TAB_COUNT - 1 < areaIndex) {
        areaIndex = 0;
      }
    }
  }
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  InGameTechnologyPanel_Rebuild(inGameRoot);
}

/* Handler of the technology window's research button, action INGAME_ACTION_TECHNOLOGY_RESEARCH (0x1013, slot 19
   of g_InGameUiActionHandlersPage10): closes the window (world view shown again, game-window page 0) and sends
   INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE for the first selected building with the technology of the selected area
   tab, which starts that research; with no tab selected the technology argument is 0 and nothing starts.
*/
void InGameTechnologyResearch_StartSelected(void *source)

{
  GameEntityRuntime *firstSelectedEntity;
  uint32_t doubledTechnologyId;
  CommandPayload modelOffset;
  UiNodeBase *selectedAreaTab;
  UiNodeBase *inGameRoot;

  /* climb to the in-game root */
  inGameRoot = (UiNodeBase *)source;
  while (inGameRoot->parent != UI_NODE_NONE) {
    inGameRoot = inGameRoot->parent;
  }
  INGAME_UI(inGameRoot,worldView)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(inGameRoot,gameWindowPageStack));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != nullptr) {
    modelOffset = (int)((intptr_t)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
                        (intptr_t)g_ModelRuntimeRebaseDelta);
    doubledTechnologyId = 0;
    if (UiSelectableGroup_FindVisibleSelected(&selectedAreaTab,nullptr,TECHNOLOGY_AREA_TAB_COUNT,
      INGAME_UI(inGameRoot,technologyAreaTab7),
      INGAME_UI(inGameRoot,technologyAreaTab6),
      INGAME_UI(inGameRoot,technologyAreaTab5),
      INGAME_UI(inGameRoot,technologyAreaTab4),
      INGAME_UI(inGameRoot,technologyAreaTab3),
      INGAME_UI(inGameRoot,technologyAreaTab2),
      INGAME_UI(inGameRoot,technologyAreaTab1))) {
      /* the selected area tab's technologyAreaTabN_prefix holds its name text id, TECHNOLOGY_TEXT_ID_BASE +
         2 * technology id (see InGameTechnologyPanel_Rebuild) */
      doubledTechnologyId = TECHNOLOGY_AREA_TAB_PREFIX(selectedAreaTab).nameTextResourceId - TECHNOLOGY_TEXT_ID_BASE;
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

/* Rebuilds the technology window for the first selected entity: the research button (off while the entity's
   classState.stateFlags has ARMY_MODEL_STATE_RESEARCHING), the unit picture, and one area tab per technology the owner may research (at most seven;
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
  uint16_t *tooltipText;
  uint16_t *descriptionText;
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
  Bool8 isAvailable;
  RichTextExtent textExtent;
  uint16_t *titleText;
  uint16_t *technologyName;
  uint16_t *resolvedName;
  ArmyAssetRecordPrefix *armyRecord;
  UiNodeBase *selectedAreaTab;
  uint16_t *labelTemplate;
  UiActionId actionId;
  
  previousTechnologyId = g_InGameSelectedTechnologyId;
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != nullptr) {
    entityModelRuntime = (ModelRuntimeSlot *)(firstSelectedEntity->common).ownership.definitionOrClassRecord;
    definition = (entityModelRuntime->definitionOrSavedId).runtimeDefinition;
    if (((entityModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      UiNodeList_UnsuppressActionId(INGAME_ACTION_TECHNOLOGY_RESEARCH,&inGameRoot->base);
    }
    else {
      UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_RESEARCH,&inGameRoot->base);
    }
    /* window title with the unit name patched in */
    titleText = TextResource_Resolve(TEXT_ID_TECHNOLOGY_WINDOW_TITLE);
    resolvedName = TextResource_Resolve(((ModelDefinitionRecordPrefix *)definition)->nameTextIndex + TEXT_ID_MODEL_NAME_BASE);
    RichTextCommandStream_PatchPayloadBySelector(0,resolvedName,titleText);
    /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in armyRecord) */
    ArmyAssetRegistry_FindById((firstSelectedEntity->common).runtimeIdentityOrArmyAssetId,&armyRecord);
    ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyDescriptionFrame))->textureSource =
         Thandor_U32ToPointer<GraphicsTextureSourceAsset>(armyRecord[1].rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecord +0x1C (dword read as texture source) */
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB1,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB2,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB3,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB4,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB5,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB6,&inGameRoot->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_TECHNOLOGY_AREA_TAB7,&inGameRoot->base);
    /* technology slots 28..1 (definition researchTechnologyIds), area tabs cycling 6..0 */
    areaIndex = TECHNOLOGY_AREA_TAB_COUNT - 1;
    for (slotIndex = TECHNOLOGY_DEFINITION_SLOT_COUNT; slotIndex != 0; slotIndex--) {
      isAvailable = Technology_IsAvailableForFaction
                         (definition->researchTechnologyIds[slotIndex],
                          (firstSelectedEntity->common).ownership.ownerIndex);
      if (isAvailable) {
        technologyId = definition->researchTechnologyIds[slotIndex];
        /* the area tab's icon (technologyAreaTabNIcon) shows subresource technologyId of tech.gfx */
        ((UiImagePanelControl *)
         THANDOR_UI_AT(inGameRoot,g_TechnologyPanelRowValueOffsets[areaIndex]))->
        subresource = technologyId;
        rowFlagOffset = g_TechnologyPanelRowFlagOffsets[areaIndex];
        /* rowFlagOffset is the area tab technologyAreaTabN; its technologyAreaTabN_prefix (the dwords 8 and 4
           bytes before it) holds the area's text id and a text pointer */
        actionId = ((UiSelectableControl *)THANDOR_UI_AT(inGameRoot,rowFlagOffset))->actionId;
        TECHNOLOGY_AREA_TAB_PREFIX(THANDOR_UI_AT(inGameRoot,rowFlagOffset)).nameTextResourceId =
             technologyId * 2 + TECHNOLOGY_TEXT_ID_BASE;
        /* tab label: the technology name (payload 0) and Xenite cost (payload 1), the number formatted into the
           label text buffer at word 0xC0; the expanded label becomes the tab's tooltip text */
        labelTemplate = TextResource_Resolve(TEXT_ID_TECHNOLOGY_AREA_TAB_LABEL);
        technologyName = TextResource_Resolve
                                 (TECHNOLOGY_AREA_TAB_PREFIX(THANDOR_UI_AT(inGameRoot,rowFlagOffset)).
                                  nameTextResourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,technologyName,labelTemplate);
        tooltipText = TECHNOLOGY_AREA_TAB_PREFIX(THANDOR_UI_AT(inGameRoot,rowFlagOffset)).tooltipText;
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                   (int)g_TechnologyAsset->records[definition->researchTechnologyIds[slotIndex]].xeniteCostQ4 >> 4,
                   labelTemplate + 192);
        RichTextCommandStream_PatchPayloadBySelector(1,labelTemplate + 192,labelTemplate);
        RichTextCommandStream_CopyExpanded(384,tooltipText,labelTemplate,nullptr);
        UiNodeList_UnsuppressActionId(actionId,&inGameRoot->base);
      }
      areaIndex--;
      if (areaIndex < 0) {
        areaIndex = TECHNOLOGY_AREA_TAB_COUNT - 1;
      }
    }
    if (!UiSelectableGroup_FindVisibleSelected(&selectedAreaTab,nullptr,TECHNOLOGY_AREA_TAB_COUNT,
      INGAME_UI(inGameRoot,technologyAreaTab7),
      INGAME_UI(inGameRoot,technologyAreaTab6),
      INGAME_UI(inGameRoot,technologyAreaTab5),
      INGAME_UI(inGameRoot,technologyAreaTab4),
      INGAME_UI(inGameRoot,technologyAreaTab3),
      INGAME_UI(inGameRoot,technologyAreaTab2),
      INGAME_UI(inGameRoot,technologyAreaTab1))) {
      playerBlock = g_SelectionPlayerRuntimeBlockPointers
                    [((WorldRuntimeContext *)INGAME_UI(inGameRoot,worldView))->selection.activePlayerRuntimeId];
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      /* the text field holds a text resource id here, resolved when drawn */
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)TEXT_ID_TECHNOLOGY_GENERAL_DESCRIPTION;
      ((UiTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->textResourceId =
           TEXT_ID_TECHNOLOGY_BUTTON_NO_AREA;
      INGAME_UI(inGameRoot,technologyDescriptionText)->rightOffset = 6;
      INGAME_UI(inGameRoot,technologyDescriptionText)->bottomOffset = 6;
      if ((playerBlock->heldResearchUnpaidFlag & ARMY_MODEL_STATE_RESEARCH_UNPAID) == 0) {
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
      nameTextId = TECHNOLOGY_AREA_TAB_PREFIX(selectedAreaTab).nameTextResourceId;
      descriptionTextId = nameTextId + 1;
      selectedTechnologyId = (nameTextId - (uint32_t)TECHNOLOGY_TEXT_ID_BASE) >> 1;
      xeniteCost = g_TechnologyAsset->records[selectedTechnologyId].xeniteCostQ4;
      costColor = g_RichTextColorPaletteArgb[0]; /* grey */
      if ((int)g_GameFactionRuntimeImage.records[(firstSelectedEntity->common).ownership.ownerIndex].
               xeniteCurrentQ4 < (int)xeniteCost) {
        costColor = g_RichTextColorPaletteArgb[5]; /* red: not affordable */
      }
      energyCost = (int)g_TechnologyAsset->records[selectedTechnologyId].energyCostQ4 >> 4;
      g_InGameSelectedTechnologyId = selectedTechnologyId;
      writtenBytes = g_WideNumberFormatUtf16
                         (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int)xeniteCost >> 4,
                          &g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_XENITE]);
      /* behind the number: palette colour 0 again, then the terminator */
      *(uint32_t *)((uint8_t *)&g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_XENITE] + writtenBytes) =
           RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,energyCost,g_InGameTechnologyEnergyCostTextUtf16);
      /* Original quirk: the upper colour words are built from the energy cost shifted left by 16 combined with the
         colour's high half (packedEnergyColor) instead of from the colour alone, so words 4, 6 and 7 get
         energy-cost bits shifted into their top nibbles. */
      packedEnergyColor = energyCost << 16 | costColor >> 16;
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 1] = (uint16_t)costColor;
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 5] = (uint16_t)(costColor >> 16);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 0] = (uint16_t)(costColor >> 4);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 4] = (uint16_t)(packedEnergyColor >> 4);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 3] = (uint16_t)(costColor >> 8);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 7] = (uint16_t)(packedEnergyColor >> 8);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 2] = (uint16_t)(costColor >> 12);
      g_InGameTechnologyCostRichText[TECHNOLOGY_COST_TEXT_COLOR_DIGITS + 6] = (uint16_t)(packedEnergyColor >> 12);
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 (int)technologyAsset->records[selectedTechnologyId].researchDurationQ5 >> 5,
                 g_InGameTechnologyResearchTimeTextUtf16);
      descriptionText = TextResource_Resolve(descriptionTextId);
      RichTextCommandStream_PatchPayloadBySelector(0,g_InGameTechnologyCostRichText,descriptionText);
      RichTextCommandStream_PatchPayloadBySelector(1,g_InGameTechnologyEnergyCostTextUtf16,descriptionText);
      RichTextCommandStream_PatchPayloadBySelector(2,g_InGameTechnologyResearchTimeTextUtf16,descriptionText);
      textExtent = RichTextCommandStream_MeasureWrappedBlock
                         (g_UiTextStyleNormal,descriptionText,
                          ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->wrapWidth);
      INGAME_UI(inGameRoot,technologyDescriptionText)->rightOffset = textExtent.widthPixels + 6;
      INGAME_UI(inGameRoot,technologyDescriptionText)->bottomOffset = textExtent.heightPixels + 6;
      scrollableControl = (UiScrollableControl *)INGAME_UI(inGameRoot,technologyDescriptionScroll);
      if (g_InGameSelectedTechnologyId != previousTechnologyId) {
        UiScrollableControl_RebuildViewportAndScrollbars(scrollableControl);
        UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,scrollableControl);
      }
      ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->text = (uint16_t *)(uintptr_t)descriptionTextId;
    }
  }
  return;
}
