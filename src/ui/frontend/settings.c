/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/settings.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/settings.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/frontend/settings. */

/* Address: 0x00549250.
   Opens the "Choose faction" page (FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE) for the loaded level. The seven
   roster rows are set up from the level: assignable factions get an active mode button ("Computer"), the other
   active factions a visible but inactive row, unused rows are hidden and their faction slot cleared. The players
   are then spread round-robin over the assignable factions (mode "Player"), the local player's play checkbox is
   ticked, and the page's buttons are arranged for a local game, a network host or a client.
   The row controls are reached through g_FrontendTaskAssignmentControlOffsets, node offsets from the frontend
   root: mode and colour buttons (UiFramedTextButtonControl), play checkboxes (UiTextButtonControl), row number
   and participant labels (UiSingleLineTextControl).
*/

void FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView *frontendRootPage)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t rowControlOffset;
  UiNodeVtable *rootVtable;
  int localPlayerRuntimeId;
  FrontendLoadedLevelAsset *loadedLevel;
  uint32_t activeCountOffsetOrLocalRow; /* active factions left, then a control offset, then the local row */
  uint32_t rowOrAssignmentIndex;
  uint32_t rowCursor;
  UiTextResourceId *rowTextId;
  uint32_t assignableCountOrOffset; /* assignable factions left, then a control offset */
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResolveResult titleText;
  TextResolveResult templateText;
  FrontendPlayerRuntimeBlockCount remainingPlayerRecords;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_FACTION_SETUP,&frontendRootPage->primaryPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  activeCountOffsetOrLocalRow = g_FrontendLoadedLevelAsset->worldSettings.activeFactionCount;
  assignableCountOrOffset = g_FrontendLoadedLevelAsset->worldSettings.assignableFactionCount;
  /* Rows 1..assignable count: factions a player may take; mode button active, caption "Computer". */
  rowOrAssignmentIndex = 0;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowOrAssignmentIndex];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowOrAssignmentIndex];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowOrAssignmentIndex];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~(FRONTEND_CONTROL_INACTIVE | UI_SELECTABLE_SELECTED_OR_CHECKED);
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowOrAssignmentIndex];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowOrAssignmentIndex];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    rowCursor = rowOrAssignmentIndex + 1;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowOrAssignmentIndex + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    activeCountOffsetOrLocalRow--;
    assignableCountOrOffset--;
    rowOrAssignmentIndex = rowCursor;
  } while (assignableCountOrOffset != 0);
  /* Further active factions (computer only): mode button active, the rest of the row hidden and inactive. */
  for (; activeCountOffsetOrLocalRow != 0; activeCountOffsetOrLocalRow--) {
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
  }
  /* Unused rows up to 7: everything hidden and inactive, caption "No-one", faction slot cleared. */
  for (; rowCursor < 7; rowCursor++) {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_NOBODY;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] = 0;
  }
  /* Colour buttons of rows 7..1 show the faction name of the level's player slot; none is selected. */
  do {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor - 1];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->textResourceId =
         ((LevelPlayerSlotRecord *)((uint8_t *)loadedLevel->playerSlots +
                                    g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowCursor - 1]))->aiClassOrMode +
         TEXT_ID_FACTION_NAME_BASE + rowCursor;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,activeCountOffsetOrLocalRow))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    localPlayerRuntimeId = g_LocalPlayerRuntimeId;
    rowCursor--;
  } while (rowCursor != 0);
  /* Players round-robin over the assignable factions (as FrontendPlayerRuntime_InitializeFactionAssignments);
     their rows switch to "Player". activeCountOffsetOrLocalRow ends as the local player's zero-based row. */
  rowOrAssignmentIndex = 1;
  activeCountOffsetOrLocalRow = 0;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingPlayerRecords = g_FrontendPlayerRuntimeBlockCount;
  do {
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowOrAssignmentIndex - 1];
    playerRecord->factionAssignment.factionAssignmentIndex = rowOrAssignmentIndex;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,assignableCountOrOffset))->textResourceId;
    playerRecord->factionAssignment.readyOrWaitState = 0;
    playerRecord->factionAssignment.consensusValue = 0;
    *rowTextId = TEXT_ID_FACTION_MODE_PLAYER;
    if (localPlayerRuntimeId == playerRecord->playerRuntimeId) {
      activeCountOffsetOrLocalRow = rowOrAssignmentIndex - 1;
    }
    rowOrAssignmentIndex++;
    playerRecord++;
    if (loadedLevel->worldSettings.assignableFactionCount < rowOrAssignmentIndex) {
      rowOrAssignmentIndex = rowOrAssignmentIndex - loadedLevel->worldSettings.assignableFactionCount;
    }
    remainingPlayerRecords--;
  } while (remainingPlayerRecords != 0);
  /* tick and show the local player's play checkbox */
  ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow]))
       ->selectable.stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow]))
       ->selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls((UiRootNode *)frontendRootPage);
  rootVtable = frontendRootPage->rootNode.vtable;
  /* frontendRootPage is the frontend root (FrontendUiImage). */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: roster left/right offsets 96 (no participant column); Back and Next, no Finish */
    FRONTEND_UI(frontendRootPage,factionRosterTable)->leftOffset = 96;
    FRONTEND_UI(frontendRootPage,factionRosterTable)->rightOffset = 96;
    FRONTEND_UI(frontendRootPage,factionSetupFinishButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    FRONTEND_UI(frontendRootPage,factionSetupNextButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    /* network game: full-width roster, Finish instead of Next; a client cannot go back */
    FRONTEND_UI(frontendRootPage,factionRosterTable)->leftOffset = 0;
    FRONTEND_UI(frontendRootPage,factionRosterTable)->rightOffset = 0;
    FRONTEND_UI(frontendRootPage,factionSetupFinishButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupFinishButton))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    FRONTEND_UI(frontendRootPage,factionSetupNextButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupNextButton))->selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
      FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags |= UI_NODE_SUPPRESSED;
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
    }
    else {
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupNextButton))->selectable.stateFlags &=
           ~FRONTEND_CONTROL_INACTIVE;
      FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags &=
           ~FRONTEND_CONTROL_INACTIVE;
    }
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  rootVtable->layout(&frontendRootPage->rootNode);
  titleText = TextResource_Resolve(loadedLevel->header.titleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE);
  *titleText.text = FRONTEND_TEXT_STYLE_NORMAL;
  templateText = TextResource_Resolve(TEXT_ID_FACTION_SETUP_TASK_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,titleText.text,templateText.text);
}


/* Address: 0x0054BA90.
   Handler of the ten resolution choices of the display settings page (actions 0x2022..0x202B, slots 34-43 of
   g_FrontendUiActionHandlersPage20): takes the clicked button's width/height pair as the pending resolution and
   refreshes which choices are available. Nothing is applied before the apply action (0x2031).
*/
void FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       ((UiNumericPairTextButton *)optionButton)->firstValue;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       ((UiNumericPairTextButton *)optionButton)->secondValue;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(optionButton);
  return;
}


/* Address: 0x0054BAC0.
   Handler of the four colour-depth choices of the display settings page (actions 0x201E..0x2021, slots 30-33 of
   g_FrontendUiActionHandlersPage20): takes the clicked button's bits per pixel as the pending colour depth and
   refreshes which choices are available.
*/
void FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *optionButton)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = ((UiNumericPairTextButton *)optionButton)->firstValue;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(optionButton);
  return;
}


/* Address: 0x0054BAF0.
   Handler of the display settings page's apply action (FRONTEND_ACTION_APPLY_DISPLAY_MODE, slot 49 of
   g_FrontendUiActionHandlersPage20): switches to the pending adapter/resolution/colour depth. On success the mode
   is saved in the persistent settings, the UI is laid out again and the palette-based UI textures are converted
   to the new pixel format; on failure the previous mode is restored (fatal if that fails too), the error is
   reported and the pending selection is reset to the saved one.
*/
void FrontendDisplaySettings_ApplyMode(void *control)

{
  uint32_t previousWidth;
  uint32_t previousHeight;
  uint32_t previousAdapterIndex;
  uint32_t selectedAdapterIndex;
  uint32_t selectedWidth;
  uint32_t selectedHeight;
  uint32_t selectedBitsPerPixel;
  int colorBitsCounterOrParentLink;
  GraphicsTextureSourceAsset **fontTextureSource;
  DisplayModeResult selectedModeResult;
  DisplayModeResult restoredModeResult;
  
  selectedBitsPerPixel = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             bitsPerPixel;
  selectedHeight = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             height;
  selectedWidth = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             width;
  selectedAdapterIndex = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
          adapterIndex;
  previousAdapterIndex = g_ActiveGraphicsAdapterIndex;
  previousHeight = g_FramebufferHeight;
  previousWidth = g_FramebufferWidth;
  g_CursorVisibilityToken--;
  /* the current colour depth: the RGB bits of the pixel format, rounded up to a multiple of 16 below */
  colorBitsCounterOrParentLink = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
          g_SoftwarePixelFormatConfig.blueBitCount;
  selectedModeResult = g_GraphicsSetDisplayMode
                    (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.bitsPerPixel,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width);
  if (selectedModeResult.failed) {
    restoredModeResult = g_GraphicsSetDisplayMode(previousAdapterIndex,colorBitsCounterOrParentLink + 15U & ~15U,previousHeight,previousWidth);
    FatalError_ExitIfFailed(restoredModeResult.valueOrError,restoredModeResult.failed);
    g_CursorVisibilityToken++;
    FatalError_ReportIfFailed(selectedModeResult.valueOrError,true);
    /* note the default adapter 1 here (ProcessEntry uses 0) */
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    adapterIndex = PersistentSettings_Read(1,PERSISTENT_SETTING_ADAPTER_INDEX);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
         PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
         PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
    FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
    return;
  }
  PersistentSettings_Write(selectedAdapterIndex,PERSISTENT_SETTING_ADAPTER_INDEX);
  PersistentSettings_Write(selectedWidth,PERSISTENT_SETTING_DISPLAY_WIDTH);
  PersistentSettings_Write(selectedHeight,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  PersistentSettings_Write(selectedBitsPerPixel,PERSISTENT_SETTING_BITS_PER_PIXEL);
  UiRootStack_Relayout();
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_FrontendMenuTextureSource);
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowTextureSource);
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowClassTextureSource);
  fontTextureSource = g_FontTextureSources;
  colorBitsCounterOrParentLink = 2; /* both fonts */
  do {
    g_GraphicsTextureSourceConvertPaletteEntries((GraphicsPaletteTextureSourceAsset *)*fontTextureSource);
    fontTextureSource++;
    colorBitsCounterOrParentLink--;
  } while (colorBitsCounterOrParentLink != 0);
  g_CursorVisibilityToken++;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
  /* walk up the parent links to the frontend template root */
  colorBitsCounterOrParentLink = (int)((UiNodeBase *)control)->parent;
  while (colorBitsCounterOrParentLink != -1) {
    control = ((UiNodeBase *)control)->parent;
    colorBitsCounterOrParentLink = (int)((UiNodeBase *)control)->parent;
  }
  /* the new resolution decides whether the dialog pages cover the menu room */
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(control,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  else {
    ((FrontendModelPointerContext *)FRONTEND_UI(control,menuRoomModelView))->contextFlags &=
         ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  return;
}


/* Address: 0x0054CD70.
   Change handler of the network game page's player-name edit (playerNameEdit, action 0x2032, slot 50 of
   g_FrontendUiActionHandlersPage20). An empty name hides Host and Join; a valid one shows Host, lets the session
   list decide about Join, and is saved as PERSISTENT_SETTING_PLAYER_NAME and copied to the local player's name
   (20 UTF-16 code units).
*/
void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control)

{
  UiNodeBase *parentCursor;
  UiTextEditControl *rootNode;
  int remainingDwords;
  uint32_t *sourceDwordCursor;
  uint32_t *playerNameDwordCursor;

  parentCursor = control->base.parent;
  rootNode = control;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = (UiTextEditControl *)(rootNode->base).parent;
    parentCursor = rootNode->base.parent;
  }
  UiTextControl_UpdateNonEmptyValidity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_HOST_GAME,&rootNode->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_HOST_GAME,&rootNode->base);
    FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick
              ((FrontendNetworkSettingsControlView *)FRONTEND_UI(rootNode,sessionList));
    PersistentSettings_WriteBlock(PERSISTENT_SETTINGS_NAME_BYTES,(uint32_t *)control->textBuffer,
                                  PERSISTENT_SETTING_PLAYER_NAME);
    sourceDwordCursor = (uint32_t *)control->textBuffer;
    playerNameDwordCursor = (void *)g_FrontendLocalPlayerNameUtf16;
    for (remainingDwords = sizeof(FrontendPlayerNameUtf16) / sizeof(uint32_t); remainingDwords != 0;
         remainingDwords--) {
      *playerNameDwordCursor = *sourceDwordCursor;
      sourceDwordCursor++;
      playerNameDwordCursor++;
    }
  }
  return;
}


/* Address: 0x00548E70.
   Handler of the mission briefing page's game speed slider (FRONTEND_ACTION_GAME_SPEED, slot 74 of
   g_FrontendUiActionHandlersPage20): applies the percentage directly in a local game and as
   FRONTEND_COMMAND_SET_GAME_SPEED in a network game, and saves it as PERSISTENT_SETTING_GAME_SPEED_PERCENT.
*/
void FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_SetGameSpeedPercent(g_LocalPlayerRuntimeId,0,0,control->boundValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SET_GAME_SPEED,0,0,control->boundValue);
  }
  PersistentSettings_Write(control->boundValue,PERSISTENT_SETTING_GAME_SPEED_PERCENT);
  return;
}


/* Address: 0x0054A810.
   Handler of the gameplay settings checkbox with action 0x2049: stores its state as
   PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL in the persistent map/mouse option flags, which the session
   reads when it starts.
*/
void FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL;
  }
  else {
    value = optionFlags & ~PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A850.
   Handler of the options page's scroll-speed slider (scrollSpeedSlider, action 0x204B, slot 75 of
   g_FrontendUiActionHandlersPage20): saves the value as PERSISTENT_SETTING_CAMERA_SCROLL_STEP.
*/
void FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentSettings_Write(control->boundValue,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  return;
}


/* Address: 0x0054A870.
   Handler of the "Automatic zoom off" checkbox (autoZoomOffCheckbox, action 0x203C, slot 60 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF.
*/
void FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF;
  }
  else {
    value = optionFlags & ~PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A8B0.
   Handler of the "Automatic rotation off" checkbox (autoRotationOffCheckbox, action 0x203D, slot 61 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF.
*/
void FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF;
  }
  else {
    value = optionFlags & ~PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A8F0.
   Handler of the "Link rotation/zoom" checkbox (FRONTEND_ACTION_LINK_ROTATION_ZOOM, slot 62 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_LINK_OPTION_ROTATION_ZOOM. The two link
   options exclude each other, so while this one is set the "Link rotation/tilt" checkbox is hidden.
*/
void FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_ROTATION_ZOOM;
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,control->base.parent);
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_ROTATION_ZOOM;
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,control->base.parent);
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A950.
   Handler of the "Link rotation/tilt" checkbox (FRONTEND_ACTION_LINK_ROTATION_TILT, slot 63 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_LINK_OPTION_ROTATION_TILT and, while set,
   hides the "Link rotation/zoom" checkbox.
*/
void FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_ROTATION_TILT;
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,control->base.parent);
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_ROTATION_TILT;
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,control->base.parent);
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A9B0.
   Handler of action 0x2051 (slot 81 of g_FrontendUiActionHandlersPage20): stores the checkbox state as
   PERSISTENT_LINK_OPTION_HIDE_PANEL. The frontend template calls the 0x2051 checkbox "Right button does not
   scroll" and the 0x2049 one "Hide panel", the opposite of what this handler and
   FrontendGameplaySettings_SetRightButtonDoesNotScroll store.
*/
void FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_HIDE_PANEL;
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_HIDE_PANEL;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
}


/* Address: 0x0054A9F0.
   Opens the options page (FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) and loads its controls from the
   persistent settings: map and mouse option checkboxes and the scroll speed. The two "link rotation" options
   exclude each other, so the one that is set hides the other checkbox.
*/
void FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t persistedValue;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,autoRotationOffCheckbox));
  /* Bit 4 is "right button does not scroll" (its action 0x2049 handler is
     FrontendGameplaySettings_SetRightButtonDoesNotScroll); the template calls this control hidePanelCheckbox. */
  UiSelectableControl_SetSelected(persistedValue & PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL,
                                  (UiSelectableControl *)FRONTEND_UI(frontendRoot,hidePanelCheckbox));
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  if ((persistedValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationZoomCheckbox));
  if ((persistedValue & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected(persistedValue & PERSISTENT_LINK_OPTION_ROTATION_TILT,
                                  (UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationTiltCheckbox));
  /* bit 4 of this word (hide panel, action 0x2051) is not loaded into its checkbox here */
  persistedValue = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  ((UiRangeSliderControl *)FRONTEND_UI(frontendRoot,scrollSpeedSlider))->value = persistedValue;
}


/* Address: 0x0054B740.
   Handler of the options page's "3D" button (settings3DButton, action 0x2012, slot 18 of
   g_FrontendUiActionHandlersPage20): opens the graphics settings page and loads its controls from the
   persistent settings: the shading toggle (the shading levels are only offered while it is on), the shading
   level matching the saved grid size and depth, the texture quality and the polygon detail (LOD) slider.
*/
void FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState *source)

{
  FrontendGraphicsRuntimeSettingsPageState *rootNode;
  uint32_t persistedValue;
  uint32_t shadingDepthQuarter;
  int shadingDepth;
  UiNodeBase *parentCursorOrSelectedRow;
  /* source is the frontend template's settings3DButton (+0x2794). */
  FrontendUiImage *frontendUi;
  
  frontendUi = (FrontendUiImage *)((uint8_t *)source - offsetof(FrontendUiImage,settings3DButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_GRAPHICS_SETTINGS,
                             (UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  persistedValue = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
  UiSelectableControl_SetSelected(persistedValue,&source->shadingEnabledControl);
  parentCursorOrSelectedRow = source->base.parent;
  rootNode = source;
  /* climb to the root of the control's UI tree */
  while (parentCursorOrSelectedRow != UI_NODE_NONE) {
    rootNode = (FrontendGraphicsRuntimeSettingsPageState *)(rootNode->base).parent;
    parentCursorOrSelectedRow = rootNode->base.parent;
  }
  if (persistedValue == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&rootNode->base);
  }
  /* the six shading level rows: grid/depth 32/32, 32/64, 32/128, 64/64, 64/128, 128/128 (the saved depth is
     stored divided by four) */
  persistedValue = PersistentSettings_Read(32,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  shadingDepthQuarter = PersistentSettings_Read(16,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  shadingDepth = shadingDepthQuarter * 4;
  if (persistedValue == 32) {
    parentCursorOrSelectedRow = (UiNodeBase *)&source->shadingResolutionRows;
    if (shadingDepth == 64) {
      parentCursorOrSelectedRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 1);
    }
    else if (shadingDepth == 128) {
      parentCursorOrSelectedRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 2);
    }
  }
  else if (persistedValue == 64) {
    parentCursorOrSelectedRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 3);
    if (shadingDepth == 128) {
      parentCursorOrSelectedRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 4);
    }
  }
  else {
    parentCursorOrSelectedRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendUi,shadingLevelGrid128Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth32));
  /* texture rows: low, medium, high */
  persistedValue = PersistentSettings_Read(TEXTURE_QUALITY_MEDIUM,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (persistedValue == TEXTURE_QUALITY_HIGH) {
    parentCursorOrSelectedRow = (UiNodeBase *)(source->textureResolutionRows.rows + 2);
  }
  else if (persistedValue == TEXTURE_QUALITY_MEDIUM) {
    parentCursorOrSelectedRow = (UiNodeBase *)(source->textureResolutionRows.rows + 1);
  }
  else {
    parentCursorOrSelectedRow = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendUi,textureQualityHigh),
      FRONTEND_UI(frontendUi,textureQualityMedium),
      FRONTEND_UI(frontendUi,textureQualityLow));
  persistedValue = PersistentSettings_Read(PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  source->polygonResolutionLodThresholdQ8 = persistedValue;
  return;
}


/* Address: 0x0054B8D0.
   Handler of the options page's "Sound" button (soundSettingsButton, action 0x2013, slot 19 of
   g_FrontendUiActionHandlersPage20): opens the audio settings page and loads its toggles and volume sliders
   from the persistent settings. The effect and movie volumes are only offered with effects on, the music volume
   only with music on, and reverse stereo only while either is on. The movie-event slider is not loaded.
*/
void FrontendAudioSettings_OpenAndSynchronize(FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeFlags *compactLayoutFlags;
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  uint32_t gainValue;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_AUDIO_SETTINGS,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags = &THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->pageRoot.nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_MUSIC,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_REVERSE_STEREO,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->reverseStereoControl);
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN); /* Q15 1.0 */
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->soundEffectsGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->movieDefaultAudioGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->musicGainControl).currentValue = gainValue;
  parentCursor = settingsSourceNode->parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    settingsSourceNode = settingsSourceNode->parent;
    parentCursor = settingsSourceNode->parent;
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,settingsSourceNode);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,settingsSourceNode);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,settingsSourceNode);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,settingsSourceNode);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,settingsSourceNode);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,settingsSourceNode);
  }
  if ((audioFlags & (PERSISTENT_SOUND_OPTION_EFFECTS | PERSISTENT_SOUND_OPTION_MUSIC)) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
  return;
}


/* Address: 0x0054BCB0.
   Handler of the graphics settings page's shading toggle (shadingEnabledCheckbox, action 0x2014, slot 20 of
   g_FrontendUiActionHandlersPage20): offers the shading levels only while shading is on and saves the state as
   PERSISTENT_SETTING_SHADING_ENABLED.
*/
void FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  uint8_t isSelected;
  UiNodeBase *parentCursor;

  isSelected = UiSelectableControl_IsSelected(control);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if ((isSelected & 1) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&control->base);
  }
  PersistentSettings_Write(isSelected & 1,PERSISTENT_SETTING_SHADING_ENABLED);
  return;
}


/* Address: 0x0054BD10.
   Handler of the six shading level choices (FRONTEND_ACTION_SHADING_LEVEL, slot 21 of
   g_FrontendUiActionHandlersPage20): saves the clicked button's grid size as the shading grid half size, twice
   it as the shading texture dimension and its depth / 4 as the subresource count, then selects the matching
   choice exclusively.
*/
void FrontendShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  int32_t shadingGridSize;
  FrontendShadingLevelGroup *shadingLevelGroup;
  uint32_t shadingDepthQuarter;
  UiNodeBase *selectedControl;

  shadingGridSize = ((UiNumericPairTextButton *)control)->firstValue;
  shadingDepthQuarter = (uint32_t)((UiNumericPairTextButton *)control)->secondValue >> 2;
  PersistentSettings_Write((int)shadingGridSize * 2,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  PersistentSettings_Write((PersistentSettingsValue)shadingGridSize,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  PersistentSettings_Write(shadingDepthQuarter,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  /* The parent is the frontend template's shadingLevelGroup (+0x385C). */
  shadingLevelGroup = (FrontendShadingLevelGroup *)(control->base).parent;
  if (shadingGridSize == 32) {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[0] /* shadingLevelGrid32Depth32 */;
    if (shadingDepthQuarter == 64 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[1] /* shadingLevelGrid32Depth64 */;
    }
    else if (shadingDepthQuarter == 128 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[2] /* shadingLevelGrid32Depth128 */;
    }
  }
  else if (shadingGridSize == 64) {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[3] /* shadingLevelGrid64Depth64 */;
    if (shadingDepthQuarter == 128 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[4] /* shadingLevelGrid64Depth128 */;
    }
  }
  else {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[5] /* shadingLevelGrid128Depth128 */;
  }
  UiSelectableGroup_SelectExclusive(6,selectedControl,
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[5],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[4],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[3],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[2],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[1],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[0]);
  return;
}


/* Address: 0x0054BDE0.
   Handler of the graphics settings page's polygon detail slider (polygonDetailSlider, action 0x2016, slot 22 of
   g_FrontendUiActionHandlersPage20): saves the Q8 model LOD depth threshold as
   PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD and applies it at once (g_ModelLodDepthThresholdQ8).
*/
void FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  g_ModelLodDepthThresholdQ8 = value;
  return;
}


/* Address: 0x0054BE10.
   Handler of the three texture quality choices (action 0x2017, slot 23 of g_FrontendUiActionHandlersPage20):
   selects the clicked one, saves it as PERSISTENT_SETTING_TEXTURE_QUALITY (high 0, medium 1, low 2) and applies
   it at once: the downsample shift becomes level / 2 (only low halves the textures) and all staging textures
   are rebuilt.
*/
void FrontendTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl;
  FrontendTextureQualityGroup *textureQualityGroup;

  /* The parent is the frontend template's textureQualityGroup (+0x3C9C). */
  textureQualityGroup = (FrontendTextureQualityGroup *)(control->base).parent;
  if (&textureQualityGroup->low.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->low;
  }
  if (&textureQualityGroup->medium.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->medium;
  }
  if (&textureQualityGroup->high.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->high;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->high,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->medium,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->low);
  PersistentSettings_Write(qualityLevel,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = qualityLevel >> 1;
  g_GraphicsRebuildAllStagingTextures();
  return;
}


/* Address: 0x0054BE90.
   Handler of the audio settings page's effects toggle (soundEffectsEnabledCheckbox, action 0x2018, slot 24 of
   g_FrontendUiActionHandlersPage20): saves PERSISTENT_SOUND_OPTION_EFFECTS, offers the effect and movie volume
   sliders only while effects are on (the music slider and reverse stereo follow the saved music bit), and
   applies the saved effect, UI and movie gains, or silence while effects are off.
*/
void FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  AudioMixerGainQ15 effectsGain;
  MovieAudioGainQ15 movieDefaultGain;
  MovieAudioGainQ15 movieAlternateGain;
  bool isSelected;

  isSelected = (bool)UiSelectableControl_IsSelected(control);
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  /* isSelected is the PERSISTENT_SOUND_OPTION_EFFECTS bit */
  PersistentSettings_Write((uint32_t)isSelected | audioFlags & ~PERSISTENT_SOUND_OPTION_EFFECTS,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if (isSelected) {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  if (isSelected == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  effectsGain = 0;
  if (isSelected) {
    effectsGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  }
  movieDefaultGain = 0;
  g_UiSoundGainQ15 = effectsGain;
  g_SoundEffectsGainQ15 = effectsGain;
  if (isSelected) {
    movieDefaultGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  }
  movieAlternateGain = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGain;
  if (isSelected) {
    movieAlternateGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGain;
  return;
}


/* Address: 0x0054BFD0.
   Handler of the audio settings page's music toggle (musicEnabledCheckbox, action 0x2019, slot 25 of
   g_FrontendUiActionHandlersPage20). Switching on loads sound\music00.sam and starts it looping at the saved
   music gain (busy cursor meanwhile; any failure just leaves the music off); switching off stops and releases
   it. Then saves PERSISTENT_SOUND_OPTION_MUSIC and offers the volume sliders and reverse stereo accordingly.
*/
void FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  IDirectSoundBuffer *activeMusicBuffer;
  SoundSampleAsset *musicSample;
  DirectSoundVoiceSet *musicVoiceSet;
  uint32_t gainOrAudioFlags;
  uint32_t musicEnabledBit;
  uint32_t newAudioFlags;
  bool isSelected;
  SampleVoiceSetResult createVoiceResult;
  SoundPlayResult playResult;
  ResourceLoadResult loadResult;

  musicEnabledBit = 0;
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    musicEnabledBit = PERSISTENT_SOUND_OPTION_MUSIC;
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
    loadResult = Resource_Load((uint16_t *)u_sound_music00_sam_00545c4e);
    musicSample = (SoundSampleAsset *)loadResult.bufferOrError;
    activeMusicBuffer = g_FrontendMusicActiveBuffer;
    if (!loadResult.failed) {
      createVoiceResult = g_SoundCreateSampleVoiceSet(musicSample);
      musicVoiceSet = createVoiceResult.voiceSet;
      if (createVoiceResult.failed) {
        Resource_Release(musicSample);
        activeMusicBuffer = g_FrontendMusicActiveBuffer;
      }
      else {
        g_FrontendMusicVoiceSet = musicVoiceSet;
        Resource_Release(musicSample);
        gainOrAudioFlags = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
        playResult = g_SoundPlayLooping(gainOrAudioFlags,gainOrAudioFlags,musicVoiceSet);
        activeMusicBuffer = playResult.soundBuffer;
        if (playResult.failed) {
          g_SoundReleaseSampleVoiceSet(musicVoiceSet);
          g_FrontendMusicVoiceSet = NULL;
          activeMusicBuffer = g_FrontendMusicActiveBuffer;
        }
      }
    }
    g_FrontendMusicActiveBuffer = activeMusicBuffer;
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  }
  else {
    g_SoundStopVoice(g_FrontendMusicActiveBuffer);
    g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = NULL;
    g_FrontendMusicVoiceSet = NULL;
  }
  gainOrAudioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  newAudioFlags = musicEnabledBit | gainOrAudioFlags & ~PERSISTENT_SOUND_OPTION_MUSIC;
  PersistentSettings_Write(newAudioFlags,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if ((newAudioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  if ((musicEnabledBit & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  if ((newAudioFlags & (PERSISTENT_SOUND_OPTION_EFFECTS | PERSISTENT_SOUND_OPTION_MUSIC)) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  return;
}


/* Address: 0x0054C150.
   Handler of the audio settings page's reverse stereo toggle (FRONTEND_ACTION_REVERSE_STEREO, slot 26 of
   g_FrontendUiActionHandlersPage20): applies it at once (g_ReverseStereoMask all ones or zero) and saves
   PERSISTENT_SOUND_OPTION_REVERSE_STEREO.
*/
void FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  uint32_t currentAudioFlags;
  uint32_t reverseStereoBit;
  int32_t reverseStereoMask;
  bool isSelected;

  reverseStereoBit = 0;
  reverseStereoMask = 0;
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    reverseStereoBit = PERSISTENT_SOUND_OPTION_REVERSE_STEREO;
    reverseStereoMask = -1;
  }
  currentAudioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  g_ReverseStereoMask = reverseStereoMask;
  PersistentSettings_Write(reverseStereoBit | currentAudioFlags & ~PERSISTENT_SOUND_OPTION_REVERSE_STEREO,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  return;
}


/* Address: 0x0054C1A0.
   Handler of the effects volume slider (FRONTEND_ACTION_EFFECTS_GAIN, slot 27 of
   g_FrontendUiActionHandlersPage20): saves the Q15 gain as PERSISTENT_SETTING_EFFECTS_GAIN and applies it at
   once to the sound effects and the UI sounds.
*/
void FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_EFFECTS_GAIN);
  g_SoundEffectsGainQ15 = value;
  g_UiSoundGainQ15 = value;
  return;
}


/* Address: 0x0054C1D0.
   Handler of the movie volume slider (FRONTEND_ACTION_MOVIE_GAIN, slot 28 of g_FrontendUiActionHandlersPage20):
   saves the Q15 gain as PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN and applies it at once.
*/
void FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  g_MovieDefaultAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C200.
   Handler of the movie event volume slider (FRONTEND_ACTION_MOVIE_EVENT_GAIN, slot 78 of
   g_FrontendUiActionHandlersPage20): saves the Q15 gain used by timed movie events as
   PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN and applies it at once.
*/
void FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  g_MovieAlternateAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C230.
   Handler of the music volume slider (FRONTEND_ACTION_MUSIC_GAIN, slot 29 of g_FrontendUiActionHandlersPage20):
   saves the Q15 gain as PERSISTENT_SETTING_MUSIC_GAIN and sets it as left and right gain of the playing frontend
   music.
*/
void FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MUSIC_GAIN);
  g_SoundSetVoiceGains(value,value,g_FrontendMusicActiveBuffer);
  return;
}


/* Address: 0x0054D170.
   Handler of the host game setup page's player-count slider (maxPlayersSlider, action 0x2007, slot 7 of
   g_FrontendUiActionHandlersPage20): saves the value as PERSISTENT_SETTING_NETWORK_PLAYER_COUNT and formats it
   into the slider's number text.
*/
void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_NETWORK_PLAYER_COUNT);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,value,
             (uint16_t *)&g_FrontendNetworkPlayerCountTextUtf16);
  return;
}


/* Address: 0x0054D1F0.
   Change handler of the host game setup page's game-name edit: the create button (FRONTEND_ACTION_CREATE_HOSTED_GAME)
   is only offered while the name is valid (non-empty), and a valid name is saved as PERSISTENT_SETTING_GAME_NAME.
*/
void FrontendNetworkSettings_SetGameName(UiTextEditControl *control)

{
  UiTextEditControl *rootNode;
  UiNodeBase *parentCursor;
  
  parentCursor = control->base.parent;
  rootNode = control;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = (UiTextEditControl *)(rootNode->base).parent;
    parentCursor = rootNode->base.parent;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_CREATE_HOSTED_GAME,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_CREATE_HOSTED_GAME,&rootNode->base);
    PersistentSettings_WriteBlock(PERSISTENT_SETTINGS_NAME_BYTES,(uint32_t *)control->textBuffer,
                                  PERSISTENT_SETTING_GAME_NAME);
  }
  return;
}


/* Address: 0x0054D250.
   Handler of the network game page's session list (sessionList, action 0x2009, slot 9 of
   g_FrontendUiActionHandlersPage20; also called by FrontendNetworkSettings_SetPlayerName). Join
   (FRONTEND_ACTION_JOIN_GAME) is offered only while the list has rows (+0x54), its selected row (+0x60) holds
   a session (row +0x14) and the local player has a name; if then bit 2 of the list's flags at +0x4C is set
   (presumably a double click), it is cleared and the join request is sent at once, as if Join had been pressed.
   The field names of FrontendNetworkSettingsControlView used here do not fit a list (text edit overlay).
*/
void FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick
          (FrontendNetworkSettingsControlView *networkSettings)

{
  UiListStateFlags *dirtyFlagsSlot;
  UiNodeBase *parentCursor;
  FrontendNetworkSettingsControlView *rootNode;

  parentCursor = networkSettings->commonState.commonPrefix.parent;
  rootNode = networkSettings;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = (FrontendNetworkSettingsControlView *)
                rootNode->commonState.commonPrefix.parent;
    parentCursor = rootNode->commonState.commonPrefix.parent;
  }
  /* networkSettings is the sessionList node (a UiListControl of FrontendSessionDiscoveryRecord rows). */
  if (((((UiListControl *)networkSettings)->rowCount == 0) ||
      (((FrontendSessionDiscoveryRecord *)*((UiListControl *)networkSettings)->selectedRowSlot)->advertisement.
       joinAvailableFlag == 0)) ||
     (g_FrontendLocalPlayerNameUtf16[0] == 0)) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,(UiNodeBase *)&rootNode->commonState);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_JOIN_GAME,(UiNodeBase *)&rootNode->commonState);
    if ((((UiListControl *)networkSettings)->listStateFlags & 4) != 0) {
      dirtyFlagsSlot = &((UiListControl *)networkSettings)->listStateFlags;
      *dirtyFlagsSlot = *dirtyFlagsSlot & ~4;
      FrontendNetworkSettings_PublishSelectedPlayerDescriptor
                ((FrontendNetworkSettingsControlView *)FRONTEND_UI(rootNode,networkGameJoinButton));
    }
  }
  return;
}


/* Address: 0x00549620.
   Refreshes the faction setup page (FRONTEND_PAGE_FACTION_SETUP) from the player records: the task description
   of the local player's faction, which of the seven faction rows can be chosen (only factions that are active
   in the level; a player who has confirmed is locked), the mode caption per row (player / nobody / computer),
   and in a network game the roster text naming the players of every faction plus the lock of the rows once the
   Finish button is pressed. When another player with a lower readyOrWaitState has the local player's faction,
   the local player's faction control is hidden.
*/
void
FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot)

{
  uint32_t *controlFlags;
  uint16_t scannedChar;
  uint32_t controlOffset;
  uint32_t factionIndexOrSetMask;
  SessionNetworkRoleFlags networkedOrRemainingCount;
  FrontendLoadedLevelAsset *loadedLevelOrClearMask;
  FrontendFactionAssignmentIndex localFactionIndex;
  SessionNetworkRoleFlags remainingSearchCount;
  int rowIndexOrCount;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerNameUtf16 *playerName;
  FrontendPlayerNameUtf16 *nameCharCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeRecord *otherPlayerRecord;
  FrontendTaskAssignmentFactionTextRow *textRowCursor;
  uint16_t *rosterTextCursor;
  uint16_t *rosterScanCursor;
  uint32_t controlClearMask;
  uint32_t controlSetMask;
  
  /* the rows get these nodeFlags masks: shown, or hidden once the local player has confirmed */
  controlSetMask = 0;
  controlClearMask = ~UI_NODE_SUPPRESSED;
  localFactionIndex = g_FrontendPlayerRuntimeBlocks->factionAssignment.factionAssignmentIndex;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingSearchCount = g_FrontendPlayerRuntimeBlockCount;
  /* network game: find the local player's record (the first test is the network role, then the count) */
  networkedOrRemainingCount = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (networkedOrRemainingCount != SESSION_NETWORK_ROLE_LOCAL) {
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localFactionIndex = playerRecord->factionAssignment.factionAssignmentIndex;
      if (playerRecord->factionAssignment.consensusValue != 0) {
        controlSetMask = UI_NODE_SUPPRESSED;
        controlClearMask = 0xffffffff;
      }
      break;
    }
    playerRecord++;
    remainingSearchCount--;
    networkedOrRemainingCount = remainingSearchCount;
  }
  /* 0x230010 + 0x10 * level title + faction selects the task description of the local player's faction */
  rowIndexOrCount = 7;
  ((UiWrappedTextControl *)FRONTEND_UI(taskAssignmentRoot,taskDescriptionText))->text =
       (uint16_t *)
       (localFactionIndex + TEXT_ID_LEVEL_DESCRIPTION_BASE + g_FrontendLoadedLevelAsset->header.titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE);
  /* Offsets from the control tables are control offsets in the page: + nodeFlags (+0x48) gives the control's
     nodeFlags (UI_NODE_SUPPRESSED), + rootFlags (+0x4C) its stateFlags (UI_SELECTABLE_SELECTED_OR_CHECKED,
     FRONTEND_CONTROL_INACTIVE), + previousRoot (+0x54) its caption text id. Rows 7..1 (entry row - 1). */
  do {
    if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount - 1]))->stateFlags &
         UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)
    goto enableFactionControl;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL)
         && (controlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount - 1],
            (*(uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset) & UI_NODE_SUPPRESSED) != 0)) {
        if ((((UiSingleLineTextControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->labelFlags &
             UI_LABEL_HIDE_WHILE_SUPPRESSED) != 0)
        goto disableFactionControl;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
               SESSION_NETWORK_ROLE_LOCAL) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount - 1];
        controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
        *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
        controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
        *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
        goto hidePlayerControl;
      }
enableFactionControl:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount - 1];
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
disableFactionControl:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount - 1];
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
      *controlFlags = *controlFlags | FRONTEND_CONTROL_INACTIVE;
    }
hidePlayerControl:
    controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags +
                     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount - 1]);
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    rowIndexOrCount--;
  } while (rowIndexOrCount != 0);
  /* mode captions: nobody, computer for active factions, player where a player record has the faction */
  rowIndexOrCount = 7;
  do {
    controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount - 1];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->textResourceId = TEXT_ID_FACTION_MODE_NOBODY;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
       (((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->textResourceId = TEXT_ID_FACTION_MODE_COMPUTER,
       (*(uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags +
                 g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount - 1]) &
        UI_NODE_SUPPRESSED) == 0)) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount - 1];
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount - 1];
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
      *controlFlags = *controlFlags | FRONTEND_CONTROL_INACTIVE;
    }
    loadedLevelOrClearMask = g_FrontendLoadedLevelAsset;
    rowIndexOrCount--;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (rowIndexOrCount != 0);
  do {
    factionIndexOrSetMask = playerRecord->factionAssignment.factionAssignmentIndex;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[factionIndexOrSetMask - 1]))->textResourceId = TEXT_ID_FACTION_MODE_PLAYER;
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  /* Not a client: update the mode buttons, then hide those of factions taken by players. The original ORs
     the last player's faction index and ANDs the address of g_FrontendLoadedLevelAsset here (EAX/ESI still
     hold them, 0x00549878), where the set/clear masks were probably meant; kept as in the original. */
  rowIndexOrCount = 7;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if ((((UiSingleLineTextControl *)THANDOR_UI_AT(taskAssignmentRoot,
               g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount - 1]))->labelFlags &
           UI_LABEL_HIDE_WHILE_SUPPRESSED) == 0) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount - 1];
        controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
        *controlFlags = *controlFlags | factionIndexOrSetMask;
        controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
        *controlFlags = *controlFlags & (uint32_t)loadedLevelOrClearMask;
      }
      rowIndexOrCount--;
      remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
    } while (rowIndexOrCount != 0);
    do {
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags +
                       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets
                       [playerRecord->factionAssignment.factionAssignmentIndex - 1]);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      remainingPlayers--;
      playerRecord++;
    } while (remainingPlayers != 0);
  }
  /* clear the roster texts (0x8C dwords = seven 0x50-byte rows) */
  textRowCursor = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows + 1;
  for (rowIndexOrCount = 140; rowIndexOrCount != 0; rowIndexOrCount--) {
    textRowCursor->textUtf16[0] = 0;
    textRowCursor->textUtf16[1] = 0;
    textRowCursor = (FrontendTaskAssignmentFactionTextRow *)(textRowCursor->textUtf16 + 2); /* one dword */
  }
  /* the participant column exists only in a network game */
  FRONTEND_UI(taskAssignmentRoot,rosterParticipantHeader)->nodeFlags |= UI_NODE_SUPPRESSED;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  FRONTEND_UI(taskAssignmentRoot,rosterParticipantHeader)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerName = &playerRecord->playerName;
  /* Append every player's name to the roster row of its faction; playerName walks the player records. */
  do {
    rosterTextCursor = (uint16_t *)((int)&g_FrontendUiDisplayModeAndTaskAssignmentScratch +
                      FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.factionAssignmentIndex * 0x50);
    rowIndexOrCount = 40; /* code units left in the row */
    if (*(int *)rosterTextCursor != 0) {
      /* The faction row already names a player: find its end and append ", " while room is left. */
      do {
        rosterScanCursor = rosterTextCursor;
        if (rowIndexOrCount == 0) break;
        rowIndexOrCount--;
        rosterScanCursor = rosterTextCursor + 1;
        scannedChar = *rosterTextCursor;
        rosterTextCursor = rosterScanCursor;
      } while (scannedChar != 0);
      rosterTextCursor = rosterScanCursor + 1;
      rowIndexOrCount--;
      if (rowIndexOrCount != 0) {
        rosterScanCursor[-1] = ',';
        rosterScanCursor[0] = ' ';
      }
    }
    if (rowIndexOrCount != 0) {
      if (21 < rowIndexOrCount) {
        rowIndexOrCount = 20; /* at most the 20 code units of a player name */
      }
      if (FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.consensusValue == 0) {
        /* consensusValue 0: the name between rich-text codes 0x8001 and 0x8000, else after 0x8000 */
        *rosterTextCursor = FRONTEND_TEXT_STYLE_HIGHLIGHTED;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        rosterTextCursor[0] = FRONTEND_TEXT_STYLE_NORMAL;
        rosterTextCursor[1] = 0;
      }
      else {
        *rosterTextCursor = FRONTEND_TEXT_STYLE_NORMAL;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        *rosterTextCursor = 0;
      }
    }
    remainingPlayers--;
    /* next player record */
    playerName = playerName + sizeof(FrontendPlayerRuntimeRecord) / sizeof(FrontendPlayerNameUtf16);
    if (remainingPlayers == 0) {
      /* once the local player has pressed Finish, the faction rows are hidden */
      rowIndexOrCount = 7;
      if ((((UiSelectableControl *)FRONTEND_UI(taskAssignmentRoot,factionSetupFinishButton))->stateFlags &
           UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount - 1];
          if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
            controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
            *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
          }
          rowIndexOrCount--;
          remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
        } while (rowIndexOrCount != 0);
      }
      else {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount - 1];
          if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
            controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags + controlOffset);
            *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
          }
          rowIndexOrCount--;
          remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
        } while (rowIndexOrCount != 0);
      }
      /* another player with the local player's faction and a lower readyOrWaitState: hide the faction */
      do {
        if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
          rowIndexOrCount = playerRecord->factionAssignment.factionAssignmentIndex;
          otherPlayerRecord = g_FrontendPlayerRuntimeBlocks;
          while ((rowIndexOrCount != otherPlayerRecord->factionAssignment.factionAssignmentIndex ||
                 ((int)(playerRecord->factionAssignment).readyOrWaitState <=
                  (int)(otherPlayerRecord->factionAssignment).readyOrWaitState))) {
            otherPlayerRecord++;
            remainingPlayers--;
            if (remainingPlayers == 0) {
              return;
            }
          }
          controlFlags = (uint32_t *)((int)&taskAssignmentRoot->base.nodeFlags +
                           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount - 1]);
          *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
          return;
        }
        remainingPlayers--;
        playerRecord++;
      } while (remainingPlayers != 0);
      return;
    }
  } while (true);
}


/* Address: 0x0054CD20.
   Handler of the network game page's Join button (FRONTEND_ACTION_JOIN_GAME, slot 2 of
   g_FrontendUiActionHandlersPage20; also called by FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick):
   takes the session token (+0x04) and host endpoint (+0xA0, 16 bytes) of the session list's selected row
   (reached at +0x248 from the button, i.e. sessionList +0x60) and sends the join request (player descriptor
   packet 0x20002) to it. Returns the send's CF.
*/
bool FrontendNetworkSettings_PublishSelectedPlayerDescriptor(FrontendNetworkSettingsControlView *networkSettings)

{
  int remainingDwords;
  uint32_t *selectedPlayerRecordDwordCursor;
  uint32_t *selectedEndpointDwordCursor;
  bool sendCarry;
  
  /* networkSettings is the frontend template's networkGameJoinButton; the session list is a sibling. */
  g_FrontendSessionToken =
       ((FrontendSessionDiscoveryRecord *)
        *((UiListControl *)FRONTEND_UI((uint8_t *)networkSettings - offsetof(FrontendUiImage,networkGameJoinButton),
                                       sessionList))->selectedRowSlot)->advertisement.header.sequenceToken;
  selectedPlayerRecordDwordCursor =
       (uint32_t *)&((FrontendSessionDiscoveryRecord *)
                     *((UiListControl *)FRONTEND_UI((uint8_t *)networkSettings -
                                                    offsetof(FrontendUiImage,networkGameJoinButton),
                                                    sessionList))->selectedRowSlot)->senderEndpoint;
  selectedEndpointDwordCursor = (uint32_t *)&g_FrontendSelectedNetworkEndpoint;
  for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor++;
    selectedEndpointDwordCursor++;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  sendCarry = UiTransfer_SendPlayerDescriptor();
  return sendCarry;
}


/* Address: 0x0054B160.
   Refreshes the display settings page after the pending mode changed (called by the colour-depth, resolution
   and apply handlers here and by ui/frontend/runtime). Every colour-depth, resolution and adapter choice is
   hidden unless the adapter offers it together with the other two pending values, the choices matching the
   pending mode are selected, and the apply button is only offered while the pending mode differs from the saved
   one.
   How the selection works in the original (0x0054B1A3..0x0054B227 and the three groups): it first pushes all
   19 choice controls (adapter 1..5, resolution 1..10, colour depth 1..4, so colour depth 4 ends on top), then
   per group pushes every choice whose value equals the pending one (colour depth: bits per pixel == +0x60,
   e.g. 0x0054B287; resolution: width == +0x60 and height == +0x64, e.g. 0x0054B33B; adapter: pending adapter
   == 0..4, e.g. 0x0054B5E4) and calls UiSelectableGroup_SelectExclusive(count, <top of stack>, <the count
   entries below it>), which pops count and the selected control, and then pops count entries (ADD ESP 0x10 /
   0x28 / 0x14). With exactly one match per group this selects the matching choice. Quirk of the original:
   when a group has no match, the entry on top (without earlier shifts: that group's last choice) is taken as
   the selected control but is not in the list, so it keeps its state; the group's other choices plus the
   next group's first choice are deselected, and every later group works on a stack shifted by one entry
   (two matches in one group shift it the other way). modeStack models that stack exactly. Once a shift
   reaches past the 19 pushed controls the original also deselects and redraws whatever its saved
   EBP/ESI/EDI registers point at; this C leaves those entries out (listCount is cut at the last control).
*/
#define DISPLAY_MODE_STACK_BASE 19 /* room for the pushed matches above the 19 controls */
#define DISPLAY_MODE_STACK_END (DISPLAY_MODE_STACK_BASE + 19)
void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot)

{
  uint32_t adapterIndex;
  uint32_t pendingWidth;
  uint32_t pendingHeight;
  uint32_t bitsPerPixel;
  uint32_t persistedValue;
  bool modeCheckCarry;
  UiNodeBase *parentCursorOrSelectedRow;
  /* the original's stack: modeStack[modeStackTop] is the top; the 5 NULL entries after the 19 controls stand
     for the original's saved registers */
  UiNodeBase *modeStack[DISPLAY_MODE_STACK_END + 5];
  int modeStackTop;
  UiControlCount listCount;

  bitsPerPixel = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.bitsPerPixel;
  pendingHeight = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection
              .height;
  pendingWidth = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
             width;
  adapterIndex = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                 persistentSelection.adapterIndex;
  /* called with any node of the page: walk up to the frontend root */
  parentCursorOrSelectedRow = frontendRoot->parent;
  while (parentCursorOrSelectedRow != UI_NODE_NONE) {
    frontendRoot = frontendRoot->parent;
    parentCursorOrSelectedRow = frontendRoot->parent;
  }
  /* 0x0054B1A3..0x0054B227: push all 19 choices */
  modeStackTop = DISPLAY_MODE_STACK_BASE;
  modeStack[DISPLAY_MODE_STACK_BASE + 0] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption4);
  modeStack[DISPLAY_MODE_STACK_BASE + 1] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption3);
  modeStack[DISPLAY_MODE_STACK_BASE + 2] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption2);
  modeStack[DISPLAY_MODE_STACK_BASE + 3] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption1);
  modeStack[DISPLAY_MODE_STACK_BASE + 4] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption10);
  modeStack[DISPLAY_MODE_STACK_BASE + 5] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption9);
  modeStack[DISPLAY_MODE_STACK_BASE + 6] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption8);
  modeStack[DISPLAY_MODE_STACK_BASE + 7] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption7);
  modeStack[DISPLAY_MODE_STACK_BASE + 8] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption6);
  modeStack[DISPLAY_MODE_STACK_BASE + 9] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption5);
  modeStack[DISPLAY_MODE_STACK_BASE + 10] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption4);
  modeStack[DISPLAY_MODE_STACK_BASE + 11] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption3);
  modeStack[DISPLAY_MODE_STACK_BASE + 12] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption2);
  modeStack[DISPLAY_MODE_STACK_BASE + 13] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption1);
  modeStack[DISPLAY_MODE_STACK_BASE + 14] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption5);
  modeStack[DISPLAY_MODE_STACK_BASE + 15] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption4);
  modeStack[DISPLAY_MODE_STACK_BASE + 16] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption3);
  modeStack[DISPLAY_MODE_STACK_BASE + 17] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption2);
  modeStack[DISPLAY_MODE_STACK_BASE + 18] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption1);
  modeStack[DISPLAY_MODE_STACK_END + 0] = NULL;
  modeStack[DISPLAY_MODE_STACK_END + 1] = NULL;
  modeStack[DISPLAY_MODE_STACK_END + 2] = NULL;
  modeStack[DISPLAY_MODE_STACK_END + 3] = NULL;
  modeStack[DISPLAY_MODE_STACK_END + 4] = NULL;
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption1))->firstValue,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1,frontendRoot);
  }
  /* 0x0054B250 */
  if (bitsPerPixel == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption1))->firstValue) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    ((FrontendColorDepthBits)((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption2))->firstValue,pendingHeight,pendingWidth
                     ,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 1,frontendRoot);
  }
  /* 0x0054B287 */
  if (bitsPerPixel == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption2))->firstValue) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption2);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption3))->firstValue,pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 2,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 2,frontendRoot);
  }
  /* 0x0054B2BE */
  if (bitsPerPixel == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption3))->firstValue) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption3);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption4))->firstValue,pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 3,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_COLOR_DEPTH_OPTION1 + 3,frontendRoot);
  }
  /* 0x0054B2F5 */
  if (bitsPerPixel == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption4))->firstValue) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption4);
  }
  /* 0x0054B304: SelectExclusive(4, top, next 4), then pop 1 + 4 */
  listCount = DISPLAY_MODE_STACK_END - (modeStackTop + 1);
  if (listCount > 4) {
    listCount = 4;
  }
  UiSelectableGroup_SelectExclusive(listCount,modeStack[modeStackTop],
      modeStack[modeStackTop + 1],modeStack[modeStackTop + 2],modeStack[modeStackTop + 3],
      modeStack[modeStackTop + 4]);
  modeStackTop = modeStackTop + 5;
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption1))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption1))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1,frontendRoot);
  }
  /* 0x0054B33B */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption1))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption1))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption2))->secondValue,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption2))->firstValue,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 1,frontendRoot);
  }
  /* 0x0054B37F */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption2))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption2))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption2);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption3))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption3))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 2,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 2,frontendRoot);
  }
  /* 0x0054B3C3 */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption3))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption3))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption3);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,
                     (FrontendDisplayDimensionPixels)((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption4))->secondValue,
                     (FrontendDisplayDimensionPixels)((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption4))->firstValue,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 3,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 3,frontendRoot);
  }
  /* 0x0054B407 */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption4))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption4))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption4);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption5))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption5))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 4,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 4,frontendRoot);
  }
  /* 0x0054B44B */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption5))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption5))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption5);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption6))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption6))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 5,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 5,frontendRoot);
  }
  /* 0x0054B48F */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption6))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption6))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption6);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,(FrontendDisplayDimensionPixels)((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption7))->secondValue,
                     (FrontendDisplayDimensionPixels)((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption7))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 6,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 6,frontendRoot);
  }
  /* 0x0054B4D3 */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption7))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption7))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption7);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption8))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption8))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 7,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 7,frontendRoot);
  }
  /* 0x0054B517 */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption8))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption8))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption8);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption9))->secondValue,
                     ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption9))->firstValue,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 8,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 8,frontendRoot);
  }
  /* 0x0054B55B */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption9))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption9))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption9);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->secondValue,((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->firstValue,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 9,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_RESOLUTION_OPTION1 + 9,frontendRoot);
  }
  /* 0x0054B59F */
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption10);
  }
  /* 0x0054B5B6: SelectExclusive(10, top, next 10), then pop 1 + 10 */
  listCount = DISPLAY_MODE_STACK_END - (modeStackTop + 1);
  if (listCount > 10) {
    listCount = 10;
  }
  UiSelectableGroup_SelectExclusive(listCount,modeStack[modeStackTop],
      modeStack[modeStackTop + 1],modeStack[modeStackTop + 2],modeStack[modeStackTop + 3],
      modeStack[modeStackTop + 4],modeStack[modeStackTop + 5],modeStack[modeStackTop + 6],
      modeStack[modeStackTop + 7],modeStack[modeStackTop + 8],modeStack[modeStackTop + 9],
      modeStack[modeStackTop + 10]);
  modeStackTop = modeStackTop + 11;
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,0);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1,frontendRoot);
  }
  /* 0x0054B5E4 */
  if (adapterIndex == 0) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,1);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 1,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 1,frontendRoot);
  }
  /* 0x0054B613 */
  if (adapterIndex == 1) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption2);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,2);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 2,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 2,frontendRoot);
  }
  /* 0x0054B643 */
  if (adapterIndex == 2) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption3);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,3);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 3,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 3,frontendRoot);
  }
  /* 0x0054B673 */
  if (adapterIndex == 3) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption4);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,4);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 4,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_ADAPTER_OPTION1 + 4,frontendRoot);
  }
  /* 0x0054B6A3 */
  if (adapterIndex == 4) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption5);
  }
  /* 0x0054B6AF: SelectExclusive(5, top, next 5), then pop 1 + 5 (the stack is dropped at return anyway) */
  listCount = DISPLAY_MODE_STACK_END - (modeStackTop + 1);
  if (listCount > 5) {
    listCount = 5;
  }
  UiSelectableGroup_SelectExclusive(listCount,modeStack[modeStackTop],
      modeStack[modeStackTop + 1],modeStack[modeStackTop + 2],modeStack[modeStackTop + 3],
      modeStack[modeStackTop + 4],modeStack[modeStackTop + 5]);
  persistedValue = PersistentSettings_Read(1,PERSISTENT_SETTING_ADAPTER_INDEX);
  if ((((persistedValue == adapterIndex) &&
       (persistedValue = PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH), persistedValue == pendingWidth)) &&
      (persistedValue = PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT), persistedValue == pendingHeight)) &&
     (persistedValue = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL), persistedValue == bitsPerPixel)) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
  return;
}

