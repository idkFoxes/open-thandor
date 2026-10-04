/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/catalog_entry.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/catalog_entry.h>
#include <thandor/thandor.h>
#include <stdarg.h>

/* Module data. */

uint32_t g_UiCatalogGroup48ColumnCount = 0;

uint32_t g_UiCatalogGroup42ColumnCount = 0;

int32_t *g_UiCatalogGroup48OffsetTables[9] = {
    /* 0 */ THANDOR_PTR(&g_UiCatalogGroup48OffsetsDefault),
    /* 1 */ THANDOR_PTR(&g_UiCatalogGroup48OffsetsDefault),
    /* 2 */ THANDOR_PTR(&g_UiCatalogGroup48OffsetsDefault),
    /* 3 */ THANDOR_PTR(&g_UiCatalogGroup48OffsetsDefault),
    /* 4 */ THANDOR_PTR(&g_UiCatalogGroup48OffsetsDefault),
    /* 5 */ THANDOR_PTR(&g_UiCatalogGroup48Offsets5Columns),
    /* 6 */ THANDOR_PTR(&g_UiCatalogGroup48Offsets6Columns),
    /* 7 */ THANDOR_PTR(&g_UiCatalogGroup48Offsets7Columns),
    /* 8 */ THANDOR_PTR(&g_UiCatalogGroup48Offsets8Columns)};

int32_t *g_UiCatalogGroup42OffsetTables[7] = {
    /* 0 */ THANDOR_PTR(&g_UiCatalogGroup42OffsetsDefault),
    /* 1 */ THANDOR_PTR(&g_UiCatalogGroup42OffsetsDefault),
    /* 2 */ THANDOR_PTR(&g_UiCatalogGroup42OffsetsDefault),
    /* 3 */ THANDOR_PTR(&g_UiCatalogGroup42OffsetsDefault),
    /* 4 */ THANDOR_PTR(&g_UiCatalogGroup42OffsetsDefault),
    /* 5 */ THANDOR_PTR(&g_UiCatalogGroup42Offsets5Columns),
    /* 6 */ THANDOR_PTR(&g_UiCatalogGroup42Offsets6Columns)};

int32_t g_UiCatalogGroup48OffsetsDefault[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 24700, 24828, 24956, 25084,
    /*  8 */ 25212, 25340, 25468, 25596, 25724, 25852, 25980, 26108,
    /* 16 */ 26236, 26364, 26492, 26620, 26748, 26876, 27004, 27132,
    /* 24 */ 27260, 27388, 27516, 27644, 27772, 27900, 28028, 28156,
    /* 32 */ 28284, 28412, 28540, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

int32_t g_UiCatalogGroup48Offsets5Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 24700, 24828, 24956,
    /*  8 */ 25084, 27388, 25212, 25340, 25468, 25596, 27516, 25724,
    /* 16 */ 25852, 25980, 26108, 27644, 26236, 26364, 26492, 26620,
    /* 24 */ 27772, 26748, 26876, 27004, 27132, 27900, 28028, 28156,
    /* 32 */ 28284, 28412, 28540, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

int32_t g_UiCatalogGroup48Offsets6Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 24700, 24828,
    /*  8 */ 24956, 25084, 27388, 28156, 25212, 25340, 25468, 25596,
    /* 16 */ 27516, 28284, 25724, 25852, 25980, 26108, 27644, 28412,
    /* 24 */ 26236, 26364, 26492, 26620, 27772, 28540, 26748, 26876,
    /* 32 */ 27004, 27132, 27900, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

int32_t g_UiCatalogGroup48Offsets7Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 28796, 24700,
    /*  8 */ 24828, 24956, 25084, 27388, 28156, 28924, 25212, 25340,
    /* 16 */ 25468, 25596, 27516, 28284, 29052, 25724, 25852, 25980,
    /* 24 */ 26108, 27644, 28412, 29180, 26236, 26364, 26492, 26620,
    /* 32 */ 27772, 28540, 29308, 26748, 26876, 27004, 27132, 27900,
    /* 40 */ 28668, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

int32_t g_UiCatalogGroup48Offsets8Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 28796, 29564,
    /*  8 */ 24700, 24828, 24956, 25084, 27388, 28156, 28924, 29692,
    /* 16 */ 25212, 25340, 25468, 25596, 27516, 28284, 29052, 29820,
    /* 24 */ 25724, 25852, 25980, 26108, 27644, 28412, 29180, 29948,
    /* 32 */ 26236, 26364, 26492, 26620, 27772, 28540, 29308, 30076,
    /* 40 */ 26748, 26876, 27004, 27132, 27900, 28668, 29436, 30204};

int32_t g_UiCatalogGroup42OffsetsDefault[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 31048, 31176, 31304, 31432,
    /*  8 */ 31560, 31688, 31816, 31944, 32072, 32200, 32328, 32456,
    /* 16 */ 32584, 32712, 32840, 32968, 33096, 33224, 33352, 33480,
    /* 24 */ 33608, 33736, 33864, 33992, 34120, 34248, 34376, 34504,
    /* 32 */ 34632, 34760, 34888, 35016, 35144, 35272, 35400, 35528,
    /* 40 */ 35656, 35784};

int32_t g_UiCatalogGroup42Offsets5Columns[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 34120, 31048, 31176, 31304,
    /*  8 */ 31432, 34248, 31560, 31688, 31816, 31944, 34376, 32072,
    /* 16 */ 32200, 32328, 32456, 34504, 32584, 32712, 32840, 32968,
    /* 24 */ 34632, 33096, 33224, 33352, 33480, 34760, 33608, 33736,
    /* 32 */ 33864, 33992, 34888, 35016, 35144, 35272, 35400, 35528,
    /* 40 */ 35656, 35784};

int32_t g_UiCatalogGroup42Offsets6Columns[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 34120, 35016, 31048, 31176,
    /*  8 */ 31304, 31432, 34248, 35144, 31560, 31688, 31816, 31944,
    /* 16 */ 34376, 35272, 32072, 32200, 32328, 32456, 34504, 35400,
    /* 24 */ 32584, 32712, 32840, 32968, 34632, 35528, 33096, 33224,
    /* 32 */ 33352, 33480, 34760, 35656, 33608, 33736, 33864, 33992,
    /* 40 */ 34888, 35784};

static uint16_t g_UiCatalogEntryRichTextScratchUtf16[16] = {0};

UiNodeVtable g_UiCatalogEntryControlVtable = {
        .relocate = UI_SLOT(UiSpriteButtonControl_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiCatalogEntryControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .nonRightRelease = UI_SLOT(UiCatalogEntryControl_NonRightRelease),
        .rightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .rightRelease = UI_SLOT(UiCommandSpriteButtonControl_RightRelease),
        .nonRightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .rightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .pointerMove = UI_SLOT(UiCatalogEntryControl_PointerMove),
        .hitTest = UI_SLOT(UiSpriteButtonControl_HitTestOpaque),
        .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

/* Implementation ownership: ui/ingame/catalog_entry. */

/* How many entries of the faction's secondary army-asset list are the given catalog record. */
static int UiCatalogEntryControl_CountOwnedAssets(int factionIndex,const UiCommandRuntimeRecordPrefix *catalogRecord)
{
  FactionArmyAssetCount assetSlotIndex;
  int assetCount;

  assetCount = 0;
  for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       assetSlotIndex != 0; assetSlotIndex--) {
    if (catalogRecord ==
        THANDOR_PTR32_AT(UiCommandRuntimeRecordPrefix,
         (factionIndex * sizeof(GameFactionRuntimeRecord) +
          THANDOR_ADDR(g_GameFactionRuntimeImage,offsetof(GameFactionRuntimeRecord,secondaryArmyAssetPointersOrIds) - 4) +
          assetSlotIndex * 4))) {
      assetCount++;
    }
  }
  return assetCount;
}

/* Draws the owned count " n " at the top left of the entry when it is not zero. */
static void UiCatalogEntryControl_DrawOwnedCount
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control,UiPackedTextStyle textStyle,int assetCount)
{
  uint32_t textLength;
  int entryTop;

  if (assetCount == 0) {
    return;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCount,g_UiCatalogEntryRichTextScratchUtf16 + 1);
  entryTop = (control->command).sprite.selectable.base.top;
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = ' ';
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,textStyle,g_UiCatalogEntryRichTextScratchUtf16,entryTop + 2,
             (control->command).sprite.selectable.base.left);
}

/* Takes a building model's progress (elapsed * 100 / required ticks) when it is at least the best so far;
   the text style then follows that model (alert colour when it is switched off). */
static void UiCatalogEntryControl_TakeBuildProgress
          (const ModelRuntimeSlot *model,int *bestPercent,UiPackedTextStyle *textStyle)
{
  int percent;

  percent = (int)(((int64_t)(int)(model->classLinkState).classState64 * 100) /
                  (int64_t)(int)(model->classLinkState).classState68);
  if (*bestPercent <= percent) {
    *textStyle = UI_CATALOG_TEXT_STYLE_NORMAL;
    *bestPercent = percent;
    if (((model->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) != 0) {
      *textStyle = UI_CATALOG_TEXT_STYLE_ALERT;
    }
  }
}

/* Highest build progress of this group-42 asset among the active faction's class-11 factories, -1 when none
   builds it. */
static int UiCatalogEntryControl_FindGroup42BuildPercent
          (PckArmyAssetIdCatalog catalogArmyAssetId,UiPackedTextStyle *textStyle)
{
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *factoryModelRuntime;
  int bestPercent;

  bestPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime).ownerListHead; modelNode != nullptr;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    factoryModelRuntime = (modelNode->runtimePayload).modelRuntime;
    if ((factoryModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) &&
        ((factoryModelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING) &&
        ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex ==
         factoryModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
        (catalogArmyAssetId == (factoryModelRuntime->classLinkState).modelLinkOrState.classState)) {
      UiCatalogEntryControl_TakeBuildProgress(factoryModelRuntime,&bestPercent,textStyle);
    }
  }
  return bestPercent;
}

/* Highest build progress of this group-48 asset among the active faction's class-22 pads and class-13
   factories, -1 when none builds it. */
static int UiCatalogEntryControl_FindGroup48BuildPercent
          (PckArmyAssetIdCatalog catalogArmyAssetId,UiPackedTextStyle *textStyle)
{
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *slotModelRuntime;
  int factionIndex;
  int bestPercent;

  factionIndex = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  bestPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime).ownerListHead; modelNode != nullptr;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    slotModelRuntime = (modelNode->runtimePayload).modelRuntime;
    if (slotModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      /* class-22 pad (ModelRuntimeLinkedChildSpawnAndBuildView): classState.classStateAC == 1 while it builds,
         classLinkState modelLinkOrState.classState / classState64 / classState68 the selected secondary asset
         and its elapsed / required build ticks */
      if (((slotModelRuntime->classState).classStateAC == 1) &&
          (factionIndex == slotModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
          (catalogArmyAssetId == (slotModelRuntime->classLinkState).modelLinkOrState.classState)) {
        UiCatalogEntryControl_TakeBuildProgress(slotModelRuntime,&bestPercent,textStyle);
      }
    }
    else if ((slotModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
             ((slotModelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING) &&
             (factionIndex == slotModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
             (catalogArmyAssetId == (slotModelRuntime->classLinkState).modelLinkOrState.classState)) {
      UiCatalogEntryControl_TakeBuildProgress(slotModelRuntime,&bestPercent,textStyle);
    }
  }
  return bestPercent;
}

/* Draws the build progress " n% " at the top right of the entry when some model builds the asset. */
static void UiCatalogEntryControl_DrawBuildPercent
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control,UiPackedTextStyle textStyle,int percent)
{
  uint32_t textLength;
  RichTextExtent textExtent;

  if (percent <= -1) {
    return;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,percent,g_UiCatalogEntryRichTextScratchUtf16 + 1);
  *(uint16_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = '%';
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 4) = ' ';
  textExtent = RichTextCommandStream_MeasureLine(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,textStyle,g_UiCatalogEntryRichTextScratchUtf16,
             (control->command).sprite.selectable.base.top + 2,
             (control->command).sprite.selectable.base.right - textExtent.widthPixels);
}

/* Draws a build catalog entry of the in-game command panel (g_UiCatalogEntryControlVtable drawClipped): the
   sprite button, its price (runtimeDisplayValueQ4 in whole units, in the alert colour when the active
   faction's xenite does not cover it), how many of this army asset the faction already owns (top left) and
   the highest build progress (elapsed / required ticks, classLinkState.classState64 / classState68 of the model runtime) among the faction's
   factories (class 11, 13) and pads (class 22) currently building this asset (top right, alert colour when that
   model is switched off). The entry is looked up by its
   offset in the in-game root in the group-42 and group-48 catalog tables.
*/
void UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control)

{
  uint32_t spriteSubresource;
  uint32_t textLength;
  int recordIndex;
  int factionIndex;
  int buildPercent;
  PckArmyAssetIdCatalog catalogArmyAssetId;
  RichTextExtent textExtent;
  uint32_t backgroundSubresource;
  GraphicsTextureSourceAsset *spriteTextureSource;
  SoftwareFramebufferAccess *framebuffer;
  UiPackedTextStyle overlayTextStyle;

  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if ((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) &&
      (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) != 0)) {
    return;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  spriteTextureSource = (control->command).sprite.primaryTextureSource;
  framebuffer = g_FramebufferAccess;
  if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    spriteSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
  }
  else {
    if ((((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) &&
       (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE) != 0)) {
      spriteTextureSource = (control->command).sprite.alternateTextureSource;
    }
    spriteSubresource = (control->command).sprite.selectedSubresourceStart;
    if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED) != 0) {
      backgroundSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
      if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        backgroundSubresource = backgroundSubresource + (control->command).sprite.animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->command).sprite.selectable.base.top,
                 (control->command).sprite.selectable.base.left,backgroundSubresource,
                 (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
    }
  }
  if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    spriteSubresource = spriteSubresource + (control->command).sprite.animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->command).sprite.selectable.base.top,
             (control->command).sprite.selectable.base.left,spriteSubresource,spriteTextureSource,framebuffer);
  factionIndex = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  if ((int)g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 <
      (int)control->runtimeDisplayValueQ4) {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
  }
  else {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                     g_UiCatalogEntryRichTextScratchUtf16 + 1);
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = ' ';
  textExtent = RichTextCommandStream_MeasureLine(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
             ((control->command).sprite.selectable.base.bottom - textExtent.heightPixels) - 2,
             ((int)((control->command).sprite.selectable.base.layoutWidth - textExtent.widthPixels) >> 1)
             + (control->command).sprite.selectable.base.left);
  for (recordIndex = 42 - 1; recordIndex >= 0; recordIndex--) {
    if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
        g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
      catalogArmyAssetId = g_UiCatalogGroup42Records[recordIndex]->armyAssetId;
      UiCatalogEntryControl_DrawOwnedCount
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,
                 UiCatalogEntryControl_CountOwnedAssets(factionIndex,g_UiCatalogGroup42Records[recordIndex]));
      buildPercent = UiCatalogEntryControl_FindGroup42BuildPercent(catalogArmyAssetId,&overlayTextStyle);
      UiCatalogEntryControl_DrawBuildPercent
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,buildPercent);
      g_GraphicsFramebufferEndAccess();
      return;
    }
  }
  for (recordIndex = 48 - 1; recordIndex >= 0; recordIndex--) {
    if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
        g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][recordIndex]) {
      catalogArmyAssetId = g_UiCatalogGroup48Records[recordIndex]->armyAssetId;
      UiCatalogEntryControl_DrawOwnedCount
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,
                 UiCatalogEntryControl_CountOwnedAssets(factionIndex,g_UiCatalogGroup48Records[recordIndex]));
      buildPercent = UiCatalogEntryControl_FindGroup48BuildPercent(catalogArmyAssetId,&overlayTextStyle);
      UiCatalogEntryControl_DrawBuildPercent
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,buildPercent);
      g_GraphicsFramebufferEndAccess();
      return;
    }
  }
  g_GraphicsFramebufferEndAccess();
}

/* Pointer over a build catalog entry (g_UiCatalogEntryControlVtable pointerMove): finds the entry in the group-42
   or group-48 catalog tables, makes its record the hover selection and rebuilds the selection detail panel,
   so the panel describes the hovered asset. Returns cursor frame 10, or 12 while Ctrl is held.
*/
GraphicsCursorFrameIndex UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;
  int group48Index;
  
  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 42 - 1; /* the last group-42 record */
    do {
      if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
          g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup42Records[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex--;
    } while (-1 < recordIndex);
    if (recordIndex < 0) {
      group48Index = 48 - 1; /* the last group-48 record */
      do {
        if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
            g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][group48Index]) {
          g_UiHoverSelectionRecord = g_UiCatalogGroup48Records[group48Index];
          InGameSelectionDetailPanel_Rebuild();
          break;
        }
        group48Index--;
      } while (-1 < group48Index);
    }
  }
  cursorFrame = 10;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    cursorFrame = 12;
  }
  return cursorFrame;
}

/* Left-button release on a pressed build catalog entry (g_UiCatalogEntryControlVtable nonRightRelease): releases
   the button, records the modifier keys held (g_KeyboardStateMask into activationInputState) for the action
   handler, plays the activation sound and queues the entry's action.
*/
void UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  uint32_t activationInputState;
  
  activationInputState = g_KeyboardStateMask;
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->command).sprite.selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    (control->command).activationInputState = activationInputState;
    if ((((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->command).sprite.activationSound != nullptr)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->command).sprite.activationSound,nullptr);
    }
    UiActionQueue_Enqueue((control->command).sprite.selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
}
