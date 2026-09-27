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
   The row controls are reached through g_FrontendTaskAssignmentControlOffsets; relative to taskRowControls04C
   (+0x4C) an offset - 4 is the control's nodeFlags, + 0 its state flags and + 8 its text resource id.
*/

void __thandor_void_preserve_eax_ecx_edx
FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView26C4 *frontendRootPage)

{
  UiNodeFlags *menuRoomContextFlags;
  uint32_t rowControlOffset;
  UiNodeVtable *rootVtable;
  int localPlayerRuntimeId;
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  uint32_t activeCountOffsetOrLocalRow; /* active factions left, then a control offset, then the local row */
  uint32_t rowOrAssignmentIndex;
  uint32_t rowCursor;
  uint8_t *rowTextIdBytes;
  uint32_t assignableCountOrOffset; /* assignable factions left, then a control offset */
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResolveResult titleText;
  TextResolveResult templateText;
  FrontendPlayerRuntimeBlockCount remainingPlayerRecords;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_FACTION_SETUP,&frontendRootPage->primaryPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    /* compactLayoutControl.nodeFlags is +0x3B4, the menu room view's contextFlags */
    menuRoomContextFlags = &(frontendRootPage->compactLayoutControl).nodeFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  activeCountOffsetOrLocalRow = (g_FrontendLoadedLevelAsset->runtimeTail2E0).activeFactionCount;
  assignableCountOrOffset = (g_FrontendLoadedLevelAsset->runtimeTail2E0).assignableFactionCount;
  /* Rows 1..assignable count: factions a player may take; mode button active, caption "Computer". */
  rowOrAssignmentIndex = 0;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowOrAssignmentIndex + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & ~UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) & ~FRONTEND_CONTROL_INACTIVE;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + rowControlOffset + 8;
    *(uint32_t *)rowTextIdBytes = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowOrAssignmentIndex + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) & ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowOrAssignmentIndex + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & ~UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) &
         ~(FRONTEND_CONTROL_INACTIVE | UI_SELECTABLE_SELECTED_OR_CHECKED);
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowOrAssignmentIndex + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & ~UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xffffffbf;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowOrAssignmentIndex + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & ~UI_NODE_SUPPRESSED;
    rowCursor = rowOrAssignmentIndex + 1;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowOrAssignmentIndex + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    activeCountOffsetOrLocalRow--;
    assignableCountOrOffset--;
    rowOrAssignmentIndex = rowCursor;
  } while (assignableCountOrOffset != 0);
  /* Further active factions (computer only): mode button active, the rest of the row hidden and inactive. */
  for (; activeCountOffsetOrLocalRow != 0; activeCountOffsetOrLocalRow--) {
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) & ~UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & ~FRONTEND_CONTROL_INACTIVE;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + assignableCountOrOffset + 8;
    *(uint32_t *)rowTextIdBytes = TEXT_ID_FACTION_MODE_COMPUTER;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & ~FRONTEND_CONTROL_INACTIVE;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) &
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) | FRONTEND_CONTROL_INACTIVE;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xffffffbf;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
  }
  /* Unused rows up to 7: everything hidden and inactive, caption "No-one", faction slot cleared. */
  for (; rowCursor < 7; rowCursor++) {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | FRONTEND_CONTROL_INACTIVE;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow + 8;
    *(uint32_t *)rowTextIdBytes = TEXT_ID_FACTION_MODE_NOBODY;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | FRONTEND_CONTROL_INACTIVE;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) &
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | FRONTEND_CONTROL_INACTIVE;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x40;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor + 1];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | UI_NODE_SUPPRESSED;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x40;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] = 0;
  }
  /* Colour buttons of rows 7..1 show the faction name of the level's player slot; none is selected. */
  do {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow + 8) =
         *(int *)((int)&loadedLevel->playerSlots[0].aiClassOrMode +
                 g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowCursor - 1]) + TEXT_ID_FACTION_NAME_BASE +
         rowCursor;
    *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint32_t *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) &
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
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowOrAssignmentIndex];
    (playerRecord->factionAssignment).factionAssignmentIndex = rowOrAssignmentIndex;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 0x4c);
    (playerRecord->factionAssignment).readyOrWaitState = 0;
    (playerRecord->factionAssignment).consensusValue = 0;
    *(uint32_t *)(rowTextIdBytes + 0x54) = TEXT_ID_FACTION_MODE_PLAYER;
    if (localPlayerRuntimeId == playerRecord->playerRuntimeId) {
      activeCountOffsetOrLocalRow = rowOrAssignmentIndex - 1;
    }
    rowOrAssignmentIndex++;
    playerRecord++;
    if ((loadedLevel->runtimeTail2E0).assignableFactionCount < rowOrAssignmentIndex) {
      rowOrAssignmentIndex = rowOrAssignmentIndex - (loadedLevel->runtimeTail2E0).assignableFactionCount;
    }
    remainingPlayerRecords--;
  } while (remainingPlayerRecords != 0);
  /* tick and show the local player's play checkbox */
  *(uint32_t *)(frontendRootPage->taskRowControls04C +
           g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1]) =
       *(uint32_t *)(frontendRootPage->taskRowControls04C +
                g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1]) |
       UI_SELECTABLE_SELECTED_OR_CHECKED;
  *(uint32_t *)(frontendRootPage->taskRowControls04C +
           (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1] - 4)) =
       *(uint32_t *)(frontendRootPage->taskRowControls04C +
                (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1] - 4)) &
       ~UI_NODE_SUPPRESSED;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls((UiRootNode *)frontendRootPage);
  rootVtable = (frontendRootPage->rootNode).vtable;
  /* taskPageControlState55C[i] is page offset 0x55C + 4 * i: [0x176]/[0x177] nodeFlags/state flags of
     factionSetupBackButton, [0x18E]/[0x18F] of factionSetupNextButton, [0x1BE]/[0x1BF] of
     factionSetupFinishButton, [0x1CC]/[0x1CE] leftOffset/rightOffset of factionRosterTable. */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: roster left/right offsets 96 (no participant column); Back and Next, no Finish */
    frontendRootPage->taskPageControlState55C[0x1cc] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] | UI_NODE_SUPPRESSED;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] & ~UI_NODE_SUPPRESSED;
    frontendRootPage->taskPageControlState55C[0x176] =
         frontendRootPage->taskPageControlState55C[0x176] & ~UI_NODE_SUPPRESSED;
    frontendRootPage->taskPageControlState55C[0x177] =
         frontendRootPage->taskPageControlState55C[0x177] & ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    /* network game: full-width roster, Finish instead of Next; a client cannot go back */
    frontendRootPage->taskPageControlState55C[0x1cc] = 0;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] & ~UI_NODE_SUPPRESSED;
    frontendRootPage->taskPageControlState55C[0x1bf] =
         frontendRootPage->taskPageControlState55C[0x1bf] & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] | UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] | FRONTEND_CONTROL_INACTIVE;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] | UI_NODE_SUPPRESSED;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] | FRONTEND_CONTROL_INACTIVE;
    }
    else {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] & ~FRONTEND_CONTROL_INACTIVE;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] & ~UI_NODE_SUPPRESSED;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] & ~FRONTEND_CONTROL_INACTIVE;
    }
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  rootVtable->layout(&frontendRootPage->rootNode);
  titleText = TextResource_Resolve((loadedLevel->header).titleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE);
  *titleText.text = 0x8000;
  templateText = TextResource_Resolve(TEXT_ID_FACTION_SETUP_TASK_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,titleText.text,templateText.text);
}


/* Address: 0x0054BA90.
   Ownership: ui/frontend/settings.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[34]@00545938;
   g_UiActionPage20InitializedHandlers[35]@00545938; g_UiActionPage20InitializedHandlers[36]@00545938;
   g_UiActionPage20InitializedHandlers[37]@00545938; g_UiActionPage20InitializedHandlers[38]@00545938;
   g_UiActionPage20InitializedHandlers[39]@00545938; g_UiActionPage20InitializedHandlers[40]@00545938;
   g_UiActionPage20InitializedHandlers[41]@00545938; g_UiActionPage20InitializedHandlers[42]@00545938;
   g_UiActionPage20InitializedHandlers[43]@00545938. Queued UI action handler for FRONTEND_PAGE20[34],FRONTEND_PAGE
   20[35],FRONTEND_PAGE20[36],FRONTEND_PAGE20[37],FRONTEND_PAGE20[38],FRONTEND_PAGE20[39],FRONTEND_PAGE20[40],FRONT
   END_PAGE20[41],FRONTEND_PAGE20[42],FRONTEND_PAGE20[43]
   (0x2022,0x2023,0x2024,0x2025,0x2026,0x2027,0x2028,0x2029,0x202A,0x202B). Return datatype is preserved for non-
   queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
*/
void __thandor_preserve_eax_edx
FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       ((UiNumericPairTextButton *)optionButton)->firstValue;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       ((UiNumericPairTextButton *)optionButton)->secondValue;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(optionButton);
  return;
}


/* Address: 0x0054BAC0.
   Ownership: ui/frontend/settings.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[30]@00545938;
   g_UiActionPage20InitializedHandlers[31]@00545938; g_UiActionPage20InitializedHandlers[32]@00545938;
   g_UiActionPage20InitializedHandlers[33]@00545938. Queued UI action handler for
   FRONTEND_PAGE20[30],FRONTEND_PAGE20[31],FRONTEND_PAGE20[32],FRONTEND_PAGE20[33] (0x201E,0x201F,0x2020,0x2021).
   Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
*/
void __thandor_preserve_eax
FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *optionButton)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = ((UiNumericPairTextButton *)optionButton)->firstValue;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(optionButton);
  return;
}


/* Address: 0x0054BAF0.
   Ownership: ui/frontend/settings.
   Purpose: Attempts the selected display mode, persists adapter/width/height/bits-per-pixel at offsets 0x00
   through 0x0C on success, and restores the prior mode on failure. Queued UI action handler for
   FRONTEND_PAGE20[49] (0x2031). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], PersistentSettings_WriteDword
   [core/settings/persistent], UiRootStack_Relayout [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendDisplaySettings_ApplyMode(void *control)

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
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
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
    restoredModeResult = g_GraphicsSetDisplayMode(previousAdapterIndex,colorBitsCounterOrParentLink + 0xfU & 0xfffffff0,previousHeight,previousWidth);
    FatalError_ExitIfFailed(restoredModeResult.valueOrError,restoredModeResult.failed);
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    FatalError_ReportIfFailed(selectedModeResult.valueOrError,true);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    adapterIndex = PersistentSettings_Read(1,0);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
         PersistentSettings_Read(0x280,4);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
         PersistentSettings_Read(0x1e0,8);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    bitsPerPixel = PersistentSettings_Read(0x10,0xc);
    FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
    return;
  }
  PersistentSettings_Write(selectedAdapterIndex,0);
  PersistentSettings_Write(selectedWidth,4);
  PersistentSettings_Write(selectedHeight,8);
  PersistentSettings_Write(selectedBitsPerPixel,0xc);
  UiRootStack_Relayout();
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_FrontendMenuTextureSource);
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowTextureSource);
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowClassTextureSource);
  fontTextureSource = g_FontTextureSources;
  colorBitsCounterOrParentLink = 2;
  do {
    g_GraphicsTextureSourceConvertPaletteEntries((GraphicsPaletteTextureSourceAsset *)*fontTextureSource);
    fontTextureSource = fontTextureSource + 1;
    colorBitsCounterOrParentLink = colorBitsCounterOrParentLink + -1;
  } while (colorBitsCounterOrParentLink != 0);
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
  colorBitsCounterOrParentLink = *(int *)((int)control + 8);
  while (colorBitsCounterOrParentLink != -1) {
    control = *(void **)((int)control + 8);
    colorBitsCounterOrParentLink = *(int *)((int)control + 8);
  }
  /* control is now the frontend template root. */
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(control,menuRoomModelView,0x4C,uint32_t) =
         FRONTEND_UI_FIELD(control,menuRoomModelView,0x4C,uint32_t) | 0x2000;
  }
  else {
    FRONTEND_UI_FIELD(control,menuRoomModelView,0x4C,uint32_t) =
         FRONTEND_UI_FIELD(control,menuRoomModelView,0x4C,uint32_t) & 0xffffdfff;
  }
  return;
}


/* Address: 0x0054CD70.
   Ownership: ui/frontend/settings.
   Purpose: When the player-name edit is accepted, writes 0x28 bytes to settings offset 0x60 and mirrors the same
   twenty UTF-16 code units to the active player-name buffer. Queued UI action handler for FRONTEND_PAGE20[50]
   (0x2032). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish.
   Cross-module calls: UiTextControl_UpdateNonEmptyValidity [ui/controls/text], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDwords
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx
FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control)

{
  UiNodeBase *parentCursor;
  UiTextEditControl *firstNode;
  int remainingDwords;
  uint32_t *sourceDwordCursor;
  uint32_t *playerNameDwordCursor;
  
  parentCursor = (control->base).parent;
  firstNode = control;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    firstNode = (UiTextEditControl *)(firstNode->base).parent;
    parentCursor = (firstNode->base).parent;
  }
  UiTextControl_UpdateNonEmptyValidity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(0x2001,&firstNode->base);
    UiNodeList_SuppressActionId(0x2002,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2001,&firstNode->base);
    FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
              ((FrontendNetworkSettingsControlView250 *)&firstNode[0x96].activationSound);
    PersistentSettings_WriteBlock(0x28,(uint32_t *)control->textPrefix6C,0x60);
    sourceDwordCursor = (uint32_t *)control->textPrefix6C;
    playerNameDwordCursor = (void *)g_FrontendLocalPlayerNameUtf16;
    for (remainingDwords = 10; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      *playerNameDwordCursor = *sourceDwordCursor;
      sourceDwordCursor = sourceDwordCursor + 1;
      playerNameDwordCursor = playerNameDwordCursor + 1;
    }
  }
  return;
}


/* Address: 0x00548E70.
   Ownership: ui/frontend/settings.
   Purpose: Applies control+0x58 to the frontend game-speed service and persists gameSpeedPercent at settings
   offset 0x44. Queued UI action handler for FRONTEND_PAGE20[74] (0x204A). Return datatype is preserved for non-
   queue direct callers.
   Cross-module calls: FrontendSession_SetGameSpeedPercent [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_SetGameSpeedPercent(g_LocalPlayerRuntimeId,0,0,control->boundValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x300,0,0,control->boundValue);
  }
  PersistentSettings_Write(control->boundValue,0x44);
  return;
}


/* Address: 0x0054A810.
   Handler of the gameplay settings checkbox with action 0x2049: stores its state as
   PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL in the persistent map/mouse option flags, which the session
   reads when it starts.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

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
   Ownership: ui/frontend/settings.
   Purpose: Persists control+0x58 as cameraScrollStep at settings offset 0x48. Queued UI action handler for
   FRONTEND_PAGE20[75] (0x204B). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentSettings_Write(control->boundValue,0x48);
  return;
}


/* Address: 0x0054A870.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203C. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[60] (0x203C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelected
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | 1;
  }
  else {
    value = optionFlags & 0xfffffffe;
  }
  PersistentSettings_Write(value,0x40);
  return;
}


/* Address: 0x0054A8B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203D. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[61] (0x203D). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelected
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | 2;
  }
  else {
    value = optionFlags & 0xfffffffd;
  }
  PersistentSettings_Write(value,0x40);
  return;
}


/* Address: 0x0054A8F0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203E. Updates bit 0 of optionFlags5C. Selecting it suppresses the paired
   action 0x203F; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[62] (0x203E).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelected
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | 1;
    UiNodeList_SuppressActionId(0x203f,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffe;
    UiNodeList_UnsuppressActionId(0x203f,(control->base).parent);
  }
  PersistentSettings_Write(value,0x5c);
  return;
}


/* Address: 0x0054A950.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203F. Updates bit 1 of optionFlags5C. Selecting it suppresses the paired
   action 0x203E; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[63] (0x203F).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelected
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | 2;
    UiNodeList_SuppressActionId(0x203e,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffd;
    UiNodeList_UnsuppressActionId(0x203e,(control->base).parent);
  }
  PersistentSettings_Write(value,0x5c);
  return;
}


/* Address: 0x0054A9B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x2051. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[81] (0x2051). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelected
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_Read(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | 4;
  }
  else {
    value = optionFlags & 0xfffffffb;
  }
  PersistentSettings_Write(value,0x5c);
  return;
}


/* Address: 0x0054A9F0.
   Opens the options page (FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) and loads its controls from the
   persistent settings: map and mouse option checkboxes and the scroll speed. The two "link rotation" options
   exclude each other, so the one that is set hides the other checkbox.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  int32_t *menuRoomContextFlags;
  uint32_t persistedValue;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags = &FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,int32_t);
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  UiSelectableControl_SetSelected
            (persistedValue & 1,(UiSelectableControl *)FRONTEND_UI(frontendRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected
            (persistedValue & 2,(UiSelectableControl *)FRONTEND_UI(frontendRoot,autoRotationOffCheckbox));
  /* Bit 4 is "right button does not scroll" (its action 0x2049 handler is
     FrontendGameplaySettings_SetRightButtonDoesNotScroll); the template calls this control hidePanelCheckbox. */
  UiSelectableControl_SetSelected(persistedValue & 4,(UiSelectableControl *)FRONTEND_UI(frontendRoot,hidePanelCheckbox));
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  if ((persistedValue & 1) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected
            (persistedValue & 1,(UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationZoomCheckbox));
  if ((persistedValue & 2) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected(persistedValue & 2,(UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationTiltCheckbox));
  /* bit 4 of this word (hide panel, action 0x2051) is not loaded into its checkbox here */
  persistedValue = PersistentSettings_Read(0x20,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  ((UiRangeSliderControl *)FRONTEND_UI(frontendRoot,scrollSpeedSlider))->value = persistedValue;
}


/* Address: 0x0054B740.
   Ownership: ui/frontend/settings.
   Purpose: Recovered action-table target FRONTEND_PAGE20[18] (0x2012).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState167C *source)

{
  FrontendGraphicsRuntimeSettingsPageState167C *firstNode;
  uint32_t persistedValue;
  uint32_t shadingDepthQuarter;
  int shadingDepth;
  UiNodeBase *parentCursorOrSelectedRow;
  /* source is the frontend template's settings3DButton (+0x2794). */
  FrontendUiImage *frontendUi;
  
  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(source,-0x2794);
  UiPageStack_SetActiveIndex(7,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint32_t) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint32_t) | 0x2000;
  }
  persistedValue = PersistentSettings_Read(1,0x1c);
  UiSelectableControl_SetSelected(persistedValue,&source->shadingEnabledControl);
  parentCursorOrSelectedRow = (source->base).parent;
  firstNode = source;
  while (parentCursorOrSelectedRow != (UiNodeBase *)0xffffffff) {
    firstNode = (FrontendGraphicsRuntimeSettingsPageState167C *)(firstNode->base).parent;
    parentCursorOrSelectedRow = (firstNode->base).parent;
  }
  if (persistedValue == 0) {
    UiNodeList_SuppressActionId(0x2015,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&firstNode->base);
  }
  persistedValue = PersistentSettings_Read(0x20,0x10);
  shadingDepthQuarter = PersistentSettings_Read(0x10,0x18);
  shadingDepth = shadingDepthQuarter * 4;
  if (persistedValue == 0x20) {
    parentCursorOrSelectedRow = (UiNodeBase *)&source->shadingResolutionRows;
    if (shadingDepth == 0x40) {
      parentCursorOrSelectedRow = (UiNodeBase *)((source->shadingResolutionRows).rows + 1);
    }
    else if (shadingDepth == 0x80) {
      parentCursorOrSelectedRow = (UiNodeBase *)((source->shadingResolutionRows).rows + 2);
    }
  }
  else if (persistedValue == 0x40) {
    parentCursorOrSelectedRow = (UiNodeBase *)((source->shadingResolutionRows).rows + 3);
    if (shadingDepth == 0x80) {
      parentCursorOrSelectedRow = (UiNodeBase *)((source->shadingResolutionRows).rows + 4);
    }
  }
  else {
    parentCursorOrSelectedRow = (UiNodeBase *)((source->shadingResolutionRows).rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendUi,shadingLevelGrid128Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth32));
  persistedValue = PersistentSettings_Read(1,0x30);
  if (persistedValue == 0) {
    parentCursorOrSelectedRow = (UiNodeBase *)((source->textureResolutionRows).rows + 2);
  }
  else if (persistedValue == 1) {
    parentCursorOrSelectedRow = (UiNodeBase *)((source->textureResolutionRows).rows + 1);
  }
  else {
    parentCursorOrSelectedRow = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendUi,textureQualityHigh),
      FRONTEND_UI(frontendUi,textureQualityMedium),
      FRONTEND_UI(frontendUi,textureQualityLow));
  persistedValue = PersistentSettings_Read(0x10000,0x34);
  source->polygonResolutionLodThresholdQ8 = persistedValue;
  return;
}


/* Address: 0x0054B8D0.
   Ownership: ui/frontend/settings.
   Purpose: Recovered action-table target FRONTEND_PAGE20[19] (0x2013).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_OpenAndSynchronize
          (FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeFlags *compactLayoutFlags;
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  uint32_t gainValue;
  
  UiPageStack_SetActiveIndex(8,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    compactLayoutFlags = &THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->pageRoot.nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | 0x2000;
  }
  audioFlags = PersistentSettings_Read(3,0x20);
  UiSelectableControl_SetSelected(audioFlags & 1,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 2,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 4,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->reverseStereoControl);
  gainValue = PersistentSettings_Read(0x8000,0x24);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->soundEffectsGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(0x8000,0x28);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->movieDefaultAudioGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(0x8000,0x2c);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->musicGainControl).currentValue = gainValue;
  parentCursor = settingsSourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    settingsSourceNode = settingsSourceNode->parent;
    parentCursor = settingsSourceNode->parent;
  }
  if ((audioFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_SuppressActionId(0x201c,settingsSourceNode);
    UiNodeList_SuppressActionId(0x204e,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x201c,settingsSourceNode);
    UiNodeList_UnsuppressActionId(0x204e,settingsSourceNode);
  }
  if ((audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,settingsSourceNode);
  }
  if ((audioFlags & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,settingsSourceNode);
  }
  return;
}


/* Address: 0x0054BCB0.
   Ownership: ui/frontend/settings.
   Purpose: Updates the user-facing Shading toggle, persists shadingEnabled at settings offset 0x1C, and mirrors it
   to frontend runtime bit 0x00020000. Queued UI action handler for FRONTEND_PAGE20[20] (0x2014). Return datatype
   is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx
FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  uint8_t isSelected;
  UiNodeBase *parentCursor;
  
  isSelected = UiSelectableControl_IsSelected(control);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((isSelected & 1) == 0) {
    UiNodeList_SuppressActionId(0x2015,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2015,&control->base);
  }
  PersistentSettings_Write(isSelected & 1,0x1c);
  return;
}


/* Address: 0x0054BD10.
   Ownership: ui/frontend/settings.
   Purpose: Applies the selected Shading Level. Persists the selected grid value at 0x10, twice that value at 0x14,
   and the selected depth divided by four at 0x18. Queued UI action handler for FRONTEND_PAGE20[21] (0x2015).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent], UiSelectableGroup_SelectExclusive
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  int32_t shadingGridSize;
  UiNodeBase *shadingLevelGroup;
  uint32_t shadingDepthQuarter;
  UiNodeBase *selectedControl;
  
  shadingGridSize = ((UiNumericPairTextButton *)control)->firstValue;
  shadingDepthQuarter = (uint32_t)((UiNumericPairTextButton *)control)->secondValue >> 2;
  PersistentSettings_Write((int)shadingGridSize * 2,0x14);
  PersistentSettings_Write((PersistentSettingsValue)shadingGridSize,0x10);
  PersistentSettings_Write(shadingDepthQuarter,0x18);
  /* The parent is the frontend template's shadingLevelGroup (+0x385C). */
  shadingLevelGroup = (control->base).parent;
  if (shadingGridSize == 0x20) {
    selectedControl = THANDOR_UI_AT(shadingLevelGroup,0x54) /* shadingLevelGrid32Depth32 */;
    if (shadingDepthQuarter == 0x10) {
      selectedControl = THANDOR_UI_AT(shadingLevelGroup,0xbc) /* shadingLevelGrid32Depth64 */;
    }
    else if (shadingDepthQuarter == 0x20) {
      selectedControl = THANDOR_UI_AT(shadingLevelGroup,0x124) /* shadingLevelGrid32Depth128 */;
    }
  }
  else if (shadingGridSize == 0x40) {
    selectedControl = THANDOR_UI_AT(shadingLevelGroup,0x18c) /* shadingLevelGrid64Depth64 */;
    if (shadingDepthQuarter == 0x20) {
      selectedControl = THANDOR_UI_AT(shadingLevelGroup,0x1f4) /* shadingLevelGrid64Depth128 */;
    }
  }
  else {
    selectedControl = THANDOR_UI_AT(shadingLevelGroup,0x25c) /* shadingLevelGrid128Depth128 */;
  }
  UiSelectableGroup_SelectExclusive(6,selectedControl,
      THANDOR_UI_AT((control->base).parent,0x25c) /* shadingLevelGrid128Depth128 */,
      THANDOR_UI_AT((control->base).parent,0x1f4) /* shadingLevelGrid64Depth128 */,
      THANDOR_UI_AT((control->base).parent,0x18c) /* shadingLevelGrid64Depth64 */,
      THANDOR_UI_AT((control->base).parent,0x124) /* shadingLevelGrid32Depth128 */,
      THANDOR_UI_AT((control->base).parent,0xbc) /* shadingLevelGrid32Depth64 */,
      THANDOR_UI_AT((control->base).parent,0x54) /* shadingLevelGrid32Depth32 */);
  return;
}


/* Address: 0x0054BDE0.
   Ownership: ui/frontend/settings.
   Purpose: Persists the custom-slider Q8 model-LOD depth threshold at settings offset 0x34 and mirrors it to
   g_ModelLodDepthThresholdQ8. Queued UI action handler for FRONTEND_PAGE20[22] (0x2016). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x34);
  g_ModelLodDepthThresholdQ8 = value;
  return;
}


/* Address: 0x0054BE10.
   Ownership: ui/frontend/settings.
   Purpose: Maps the Texture Quality choices High, Medium, and Low to persisted values 0, 1, and 2 at settings
   offset 0x30, then rebuilds texture staging resources. Queued UI action handler for FRONTEND_PAGE20[23] (0x2017).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl;
  UiNodeBase *textureQualityGroup;
  
  /* The parent is the frontend template's textureQualityGroup (+0x3C9C). */
  textureQualityGroup = (control->base).parent;
  if ((UiSelectableControl *)THANDOR_UI_AT(textureQualityGroup,0x54) /* textureQualityLow */ == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = THANDOR_UI_AT(textureQualityGroup,0x54) /* textureQualityLow */;
  }
  if ((UiSelectableControl *)THANDOR_UI_AT(textureQualityGroup,0xb4) /* textureQualityMedium */ == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = THANDOR_UI_AT(textureQualityGroup,0xb4) /* textureQualityMedium */;
  }
  if ((UiSelectableControl *)THANDOR_UI_AT(textureQualityGroup,0x114) /* textureQualityHigh */ == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = THANDOR_UI_AT(textureQualityGroup,0x114) /* textureQualityHigh */;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      THANDOR_UI_AT((control->base).parent,0x114) /* textureQualityHigh */,
      THANDOR_UI_AT((control->base).parent,0xb4) /* textureQualityMedium */,
      THANDOR_UI_AT((control->base).parent,0x54) /* textureQualityLow */);
  PersistentSettings_Write(qualityLevel,0x30);
  g_TextureDownsampleShift = qualityLevel >> 1;
  g_GraphicsRebuildAllStagingTextures();
  return;
}


/* Address: 0x0054BE90.
   Ownership: ui/frontend/settings.
   Purpose: Queued UI action handler for FRONTEND_PAGE20[24] (0x2018). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: UiSelectableControl_IsSelected [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx
FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  AudioMixerGainQ15 effectsGain;
  MovieAudioGainQ15 movieDefaultGain;
  MovieAudioGainQ15 movieAlternateGain;
  bool isSelected;
  
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  audioFlags = PersistentSettings_Read(3,0x20);
  PersistentSettings_Write((uint32_t)isSelected | audioFlags & 0xfffffffe,0x20);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if (isSelected) {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,&control->base);
    UiNodeList_UnsuppressActionId(0x204e,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,&control->base);
    UiNodeList_SuppressActionId(0x204e,&control->base);
  }
  if ((audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,&control->base);
  }
  if (isSelected == 0 && (audioFlags & 2) == 0) {
    UiNodeList_SuppressActionId(0x201a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,&control->base);
  }
  effectsGain = 0;
  if (isSelected) {
    effectsGain = PersistentSettings_Read(0x8000,0x24);
  }
  movieDefaultGain = 0;
  g_UiSoundGainQ15 = effectsGain;
  g_SoundEffectsGainQ15 = effectsGain;
  if (isSelected) {
    movieDefaultGain = PersistentSettings_Read(0x8000,0x28);
  }
  movieAlternateGain = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGain;
  if (isSelected) {
    movieAlternateGain = PersistentSettings_Read(0x8000,0x4c);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGain;
  return;
}


/* Address: 0x0054BFD0.
   Ownership: ui/frontend/settings.
   Purpose: Updates SOUND_OPTIONS_MUSIC_ENABLED, creates or stops the frontend looping-music voice, applies
   musicGainQ15, and updates related frontend controls. Queued UI action handler for FRONTEND_PAGE20[25] (0x2019).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelected [ui/controls/lists], Resource_Load
   [assets/resource/runtime], Resource_Release [assets/resource/runtime], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control)

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
    musicEnabledBit = 2;
    g_GraphicsCursorSetFrame(6);
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
        gainOrAudioFlags = PersistentSettings_Read(0x8000,0x2c);
        playResult = g_SoundPlayLooping(gainOrAudioFlags,gainOrAudioFlags,musicVoiceSet);
        activeMusicBuffer = playResult.soundBuffer;
        if (playResult.failed) {
          g_SoundReleaseSampleVoiceSet(musicVoiceSet);
          g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
          activeMusicBuffer = g_FrontendMusicActiveBuffer;
        }
      }
    }
    g_FrontendMusicActiveBuffer = activeMusicBuffer;
    g_GraphicsCursorSetFrame(0);
  }
  else {
    g_SoundStopVoice(g_FrontendMusicActiveBuffer);
    g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = (IDirectSoundBuffer *)0x0;
    g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
  }
  gainOrAudioFlags = PersistentSettings_Read(3,0x20);
  newAudioFlags = musicEnabledBit | gainOrAudioFlags & 0xfffffffd;
  PersistentSettings_Write(newAudioFlags,0x20);
  parentCursor = (control->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = (control->base).parent;
  }
  if ((newAudioFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x201b,&control->base);
    UiNodeList_SuppressActionId(0x201c,&control->base);
    UiNodeList_SuppressActionId(0x204e,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201b,&control->base);
    UiNodeList_UnsuppressActionId(0x201c,&control->base);
    UiNodeList_UnsuppressActionId(0x204e,&control->base);
  }
  if ((musicEnabledBit & 2) == 0) {
    UiNodeList_SuppressActionId(0x201d,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201d,&control->base);
  }
  if ((newAudioFlags & 3) == 0) {
    UiNodeList_SuppressActionId(0x201a,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201a,&control->base);
  }
  return;
}


/* Address: 0x0054C150.
   Ownership: ui/frontend/settings.
   Purpose: Updates SOUND_OPTIONS_REVERSE_STEREO and writes g_ReverseStereoMask as zero or 0xFFFFFFFF. Queued UI
   action handler for FRONTEND_PAGE20[26] (0x201A). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelected [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  uint32_t currentAudioFlags;
  uint32_t reverseStereoBit;
  int32_t reverseStereoMask;
  bool isSelected;
  
  reverseStereoBit = 0;
  reverseStereoMask = 0;
  isSelected = (bool)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    reverseStereoBit = 4;
    reverseStereoMask = -1;
  }
  currentAudioFlags = PersistentSettings_Read(3,0x20);
  g_ReverseStereoMask = reverseStereoMask;
  PersistentSettings_Write(reverseStereoBit | currentAudioFlags & 0xfffffffb,0x20);
  return;
}


/* Address: 0x0054C1A0.
   Ownership: ui/frontend/settings.
   Purpose: Writes soundEffectsGainQ15 from control+0x58 and updates the positional/effect and UI-sound gain
   globals. Queued UI action handler for FRONTEND_PAGE20[27] (0x201B). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x24);
  g_SoundEffectsGainQ15 = value;
  g_UiSoundGainQ15 = value;
  return;
}


/* Address: 0x0054C1D0.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieDefaultAudioGainQ15 from control+0x58 and updates the movie default-gain global. Queued UI
   action handler for FRONTEND_PAGE20[28] (0x201C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x28);
  g_MovieDefaultAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C200.
   Ownership: ui/frontend/settings.
   Purpose: Writes movieAlternateAudioGainQ15 from control+0x58 and updates the alternate movie-gain global used by
   timed movie playback events. Queued UI action handler for FRONTEND_PAGE20[78] (0x204E). Return datatype is
   preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x4c);
  g_MovieAlternateAudioGainQ15 = value;
  return;
}


/* Address: 0x0054C230.
   Ownership: ui/frontend/settings.
   Purpose: Writes musicGainQ15 from control+0x58 and immediately applies equal left/right gains to the active
   frontend music voice. Queued UI action handler for FRONTEND_PAGE20[29] (0x201D). Return datatype is preserved
   for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  uint32_t musicGainQ15;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x2c);
  g_SoundSetVoiceGains(value,value,g_FrontendMusicActiveBuffer);
  return;
}


/* Address: 0x0054D170.
   Ownership: ui/frontend/settings.
   Purpose: Persists control+0x58 as networkPlayerCount at settings offset 0x3C. Queued UI action handler for
   FRONTEND_PAGE20[7] (0x2007). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;
  
  value = control->boundValue;
  PersistentSettings_Write(value,0x3c);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,value,
             (uint16_t *)&g_FrontendNetworkPlayerCountTextUtf16);
  return;
}


/* Address: 0x0054D1F0.
   Ownership: ui/frontend/settings.
   Purpose: Writes the game/session name as 0x28 bytes at settings offset 0x88. Queued UI action handler for
   FRONTEND_PAGE20[8] (0x2008). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDwords [core/settings/persistent].
*/
void __thandor_preserve_eax FrontendNetworkSettings_SetGameName(UiTextEditControl *control)

{
  UiTextEditControl *firstNode;
  UiNodeBase *parentCursor;
  
  parentCursor = (control->base).parent;
  firstNode = control;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    firstNode = (UiTextEditControl *)(firstNode->base).parent;
    parentCursor = (firstNode->base).parent;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(0x2004,&firstNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2004,&firstNode->base);
    PersistentSettings_WriteBlock(0x28,(uint32_t *)control->textPrefix6C,0x88);
  }
  return;
}


/* Address: 0x0054D250.
   Ownership: ui/frontend/settings.
   Purpose: Updates action 0x2002 availability from the network-settings control state and player-name presence,
   then publishes the player descriptor when the control dirty bit requires it. Queued UI action handler for
   FRONTEND_PAGE20[9] (0x2009). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSettings_PublishSelectedPlayerDescriptor.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  UiNodeBase **dirtyFlagsSlot;
  UiNodeBase *parentCursor;
  FrontendNetworkSettingsControlView250 *firstNode;
  
  parentCursor = (networkSettings->commonState).commonPrefix.parent;
  firstNode = networkSettings;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    firstNode = (FrontendNetworkSettingsControlView250 *)
                (firstNode->commonState).commonPrefix.parent;
    parentCursor = (firstNode->commonState).commonPrefix.parent;
  }
  if ((((networkSettings->textEditView).textEdit.base.left == 0) ||
      (*(int *)(*(int *)(networkSettings->textEditView).textEdit.base.bottom + 0x14) == 0)) ||
     (g_FrontendLocalPlayerNameUtf16[0] == 0)) {
    UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&firstNode->commonState);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2002,(UiNodeBase *)&firstNode->commonState);
    if (((uint32_t)(networkSettings->textEditView).textEdit.base.parent & 4) != 0) {
      dirtyFlagsSlot = &(networkSettings->textEditView).textEdit.base.parent;
      *dirtyFlagsSlot = (UiNodeBase *)((uint32_t)*dirtyFlagsSlot & 0xfffffffb);
      FrontendNetworkSettings_PublishSelectedPlayerDescriptor
                ((FrontendNetworkSettingsControlView250 *)FRONTEND_UI(firstNode,networkGameJoinButton));
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
  int32_t *rosterLayoutFlags;
  uint32_t *controlFlags;
  uint16_t scannedChar;
  uint32_t controlOffset;
  uint32_t factionIndexOrSetMask;
  SessionNetworkRoleFlags networkedOrRemainingCount;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelOrClearMask;
  FrontendFactionAssignmentIndex localFactionIndex;
  SessionNetworkRoleFlags remainingSearchCount;
  int rowIndexOrCount;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerNameUtf16_28 *playerName;
  FrontendPlayerNameUtf16_28 *nameCharCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeRecord *otherPlayerRecord;
  FrontendTaskAssignmentGeneratedFactionTextRow50 *textRowCursor;
  uint16_t *rosterTextCursor;
  uint16_t *rosterScanCursor;
  uint32_t controlClearMask;
  uint32_t controlSetMask;
  
  /* the rows get these nodeFlags masks: shown, or hidden once the local player has confirmed */
  controlSetMask = 0;
  controlClearMask = ~UI_NODE_SUPPRESSED;
  localFactionIndex = (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingSearchCount = g_FrontendPlayerRuntimeBlockCount;
  /* network game: find the local player's record (the first test is the network role, then the count) */
  networkedOrRemainingCount = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (networkedOrRemainingCount != SESSION_NETWORK_ROLE_LOCAL) {
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localFactionIndex = (playerRecord->factionAssignment).factionAssignmentIndex;
      if ((playerRecord->factionAssignment).consensusValue != 0) {
        controlSetMask = UI_NODE_SUPPRESSED;
        controlClearMask = 0xffffffff;
      }
      break;
    }
    playerRecord++;
    remainingSearchCount = remainingSearchCount - 1;
    networkedOrRemainingCount = remainingSearchCount;
  }
  /* 0x230010 + 0x10 * level title + faction selects the task description of the local player's faction */
  rowIndexOrCount = 7;
  FRONTEND_UI_FIELD(taskAssignmentRoot,taskDescriptionText,0x54,struct UiNodeBase *) =
       (UiNodeBase *)
       (localFactionIndex + 0x230010 + (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10);
  /* Offsets from the control tables are control offsets in the page: + nodeFlags (+0x48) gives the control's
     nodeFlags (UI_NODE_SUPPRESSED), + rootFlags (+0x4C) its stateFlags (UI_SELECTABLE_SELECTED_OR_CHECKED,
     FRONTEND_CONTROL_INACTIVE), + previousRoot (+0x54) its caption text id. Rows 7..1; entry 0 is unused. */
  do {
    if ((*(uint32_t *)((int)&taskAssignmentRoot->rootFlags +
                  g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount]) &
         UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)
    goto FrontendTaskAssignment_ApplyEligibleFactionControlState;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL)
         && (controlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount],
            (*(uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset) & UI_NODE_SUPPRESSED) != 0)) {
        if ((*(uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & 0x40) != 0)
        goto FrontendTaskAssignment_DisableUnavailableFactionControl;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
               SESSION_NETWORK_ROLE_LOCAL) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
        controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
        controlFlags = (uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
        *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
        goto FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop;
      }
FrontendTaskAssignment_ApplyEligibleFactionControlState:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
FrontendTaskAssignment_DisableUnavailableFactionControl:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags | FRONTEND_CONTROL_INACTIVE;
    }
FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop:
    controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags +
                     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount]);
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    rowIndexOrCount--;
  } while (rowIndexOrCount != 0);
  /* mode captions: nobody, computer for active factions, player where a player record has the faction */
  rowIndexOrCount = 7;
  do {
    controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount];
    *(uint32_t *)((int)&taskAssignmentRoot->previousRoot + controlOffset) = TEXT_ID_FACTION_MODE_NOBODY;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
       (*(uint32_t *)((int)&taskAssignmentRoot->previousRoot + controlOffset) = TEXT_ID_FACTION_MODE_COMPUTER,
       (*(uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags +
                 g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount]) &
        UI_NODE_SUPPRESSED) == 0)) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = (uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags | FRONTEND_CONTROL_INACTIVE;
    }
    loadedLevelOrClearMask = g_FrontendLoadedLevelAsset;
    rowIndexOrCount--;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (rowIndexOrCount != 0);
  do {
    factionIndexOrSetMask = (playerRecord->factionAssignment).factionAssignmentIndex;
    *(uint32_t *)
     ((int)&taskAssignmentRoot->previousRoot +
     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[factionIndexOrSetMask]) = TEXT_ID_FACTION_MODE_PLAYER;
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  /* Not a client: update the mode buttons, then hide those of factions taken by players. The original ORs
     the last player's faction index and ANDs the address of g_FrontendLoadedLevelAsset here (EAX/ESI still
     hold them, 0x00549878), where the set/clear masks were probably meant; kept as in the original. */
  rowIndexOrCount = 7;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if ((*(uint32_t *)((int)&taskAssignmentRoot->rootFlags +
                    g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount]) &
          0x40) == 0) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount];
        controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags | factionIndexOrSetMask;
        controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags & (uint32_t)loadedLevelOrClearMask;
      }
      rowIndexOrCount--;
      remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
    } while (rowIndexOrCount != 0);
    do {
      controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags +
                       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets
                       [(playerRecord->factionAssignment).factionAssignmentIndex]);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      remainingPlayers--;
      playerRecord++;
    } while (remainingPlayers != 0);
  }
  /* clear the roster texts (0x8C dwords = seven 0x50-byte rows) */
  textRowCursor = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows + 1;
  for (rowIndexOrCount = 0x8c; rowIndexOrCount != 0; rowIndexOrCount--) {
    textRowCursor->textUtf16[0] = 0;
    textRowCursor->textUtf16[1] = 0;
    textRowCursor = (FrontendTaskAssignmentGeneratedFactionTextRow50 *)(textRowCursor->textUtf16 + 2);
  }
  /* the participant column exists only in a network game */
  rosterLayoutFlags = &FRONTEND_UI_FIELD(taskAssignmentRoot,rosterParticipantHeader,0x48,int32_t);
  *rosterLayoutFlags = *rosterLayoutFlags | UI_NODE_SUPPRESSED;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  rosterLayoutFlags = &FRONTEND_UI_FIELD(taskAssignmentRoot,rosterParticipantHeader,0x48,int32_t);
  *rosterLayoutFlags = *rosterLayoutFlags & ~UI_NODE_SUPPRESSED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerName = &playerRecord->playerName;
  /* Append every player's name to the roster row of its faction. playerName walks the records; +0x40 from it
     is the record's factionAssignmentIndex (+0x58), +0x44 its consensusValue (+0x5C). */
  do {
    rosterTextCursor = (uint16_t *)((int)&g_FrontendUiDisplayModeAndTaskAssignmentScratch +
                      *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(playerName + 1) + 1) + 8) * 0x50);
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
      if (*(FrontendConsensusValue *)
           ((int)((UiTransferEndpointDescriptor *)(playerName + 1) + 1) + 0xc) == 0) {
        /* consensusValue 0: the name between rich-text codes 0x8001 and 0x8000, else after 0x8000 */
        *rosterTextCursor = 0x8001;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16_28 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        rosterTextCursor[0] = 0x8000;
        rosterTextCursor[1] = 0;
      }
      else {
        *rosterTextCursor = 0x8000;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16_28 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        *rosterTextCursor = 0;
      }
    }
    remainingPlayers--;
    playerName = playerName + 0x7e; /* 0x7E names of 0x28 bytes = one 0x13B0-byte player record */
    if (remainingPlayers == 0) {
      /* once the local player has pressed Finish, the faction rows are hidden */
      rowIndexOrCount = 7;
      if (((uint32_t)FRONTEND_UI_FIELD(taskAssignmentRoot,factionSetupFinishButton,0x4C,struct UiRootCallbacks *) & 2) == 0) {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
          if ((*(uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & FRONTEND_CONTROL_INACTIVE) == 0) {
            controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
            *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
          }
          rowIndexOrCount--;
          remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
        } while (rowIndexOrCount != 0);
      }
      else {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
          if ((*(uint32_t *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & FRONTEND_CONTROL_INACTIVE) == 0) {
            controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
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
          rowIndexOrCount = (playerRecord->factionAssignment).factionAssignmentIndex;
          otherPlayerRecord = g_FrontendPlayerRuntimeBlocks;
          while ((rowIndexOrCount != (otherPlayerRecord->factionAssignment).factionAssignmentIndex ||
                 ((int)(playerRecord->factionAssignment).readyOrWaitState <=
                  (int)(otherPlayerRecord->factionAssignment).readyOrWaitState))) {
            otherPlayerRecord++;
            remainingPlayers--;
            if (remainingPlayers == 0) {
              return;
            }
          }
          controlFlags = (uint32_t *)((int)&(taskAssignmentRoot->base).nodeFlags +
                           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount]);
          *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
          return;
        }
        remainingPlayers--;
        playerRecord++;
      } while (remainingPlayers != 0);
      return;
    }
  } while( true );
}


/* Address: 0x0054CD20.
   Ownership: ui/frontend/settings.
   Purpose: Copies the selected frontend player endpoint descriptor into the active transfer endpoint, publishes
   its sender context, resets the sequence token to -1, and invokes UiTransfer_SendPlayerDescriptorPacket20002
   while preserving the backend CF result. Queued UI action handler for FRONTEND_PAGE20[2] (0x2002). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: UiTransfer_SendPlayerDescriptorPacket20002 [network/protocol/transfer].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetworkSettings_PublishSelectedPlayerDescriptor
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  int remainingDwords;
  uint32_t *selectedPlayerRecordDwordCursor;
  uint32_t *selectedEndpointDwordCursor;
  bool sendCarry;
  
  g_FrontendSessionToken = *(uint32_t *)(**(int **)(networkSettings->raw + 0x248) + 4);
  selectedPlayerRecordDwordCursor = (uint32_t *)(**(int **)(networkSettings->raw + 0x248) + 0xa0);
  selectedEndpointDwordCursor = (uint32_t *)&g_FrontendSelectedNetworkEndpoint;
  for (remainingDwords = 4; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor = selectedPlayerRecordDwordCursor + 1;
    selectedEndpointDwordCursor = selectedEndpointDwordCursor + 1;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  sendCarry = UiTransfer_SendPlayerDescriptorPacket20002();
  return sendCarry;
}


/* Address: 0x0054B160.
   Ownership: ui/frontend/settings.
   Purpose: Walks to the display-settings root, checks the pending adapter, width, height, and bit-depth tuple
   against enumerated modes, updates actions 0x201E through 0x2030, selects the matching groups, and gates action
   0x2031 when settings are unchanged.
   Cross-module calls: DisplayModeTable_ContainsExactMode [graphics/backend/directdraw],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists],
   UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_ReadDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot)

{
  uint32_t adapterIndex;
  uint32_t pendingWidth;
  uint32_t pendingHeight;
  uint32_t bitsPerPixel;
  uint32_t persistedValue;
  FrontendDisplayDimensionPixels width;
  FrontendDisplayDimensionPixels height;
  bool modeCheckCarry;
  UiNodeBase *parentCursorOrSelectedRow;
  
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
  while (parentCursorOrSelectedRow != (UiNodeBase *)0xffffffff) {
    frontendRoot = frontendRoot->parent;
    parentCursorOrSelectedRow = frontendRoot->parent;
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (FRONTEND_UI_FIELD(frontendRoot,displayColorDepthOption1,0x60,uint32_t),
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex);
  parentCursorOrSelectedRow = frontendRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x201e,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201e,frontendRoot);
  }
  if (bitsPerPixel == FRONTEND_UI_FIELD(frontendRoot,displayColorDepthOption1,0x60,uint32_t)) {
    parentCursorOrSelectedRow = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    ((FrontendColorDepthBits)FRONTEND_UI_FIELD(frontendRoot,displayColorDepthOption2,0x60,struct UiNodeBase *),pendingHeight,pendingWidth
                     ,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x201f,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201f,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (FRONTEND_UI_FIELD(frontendRoot,displayColorDepthOption3,0x60,int32_t),pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2020,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2020,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (FRONTEND_UI_FIELD(frontendRoot,displayColorDepthOption4,0x60,uint32_t),pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2021,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2021,frontendRoot);
  }
  UiSelectableGroup_SelectExclusive(4,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendRoot,displayColorDepthOption4),
      FRONTEND_UI(frontendRoot,displayColorDepthOption3),
      FRONTEND_UI(frontendRoot,displayColorDepthOption2),
      FRONTEND_UI(frontendRoot,displayColorDepthOption1));
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption1,0x64,enum UiNodeFlags),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption1,0x60,int32_t),adapterIndex);
  parentCursorOrSelectedRow = frontendRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2022,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2022,frontendRoot);
  }
  if ((pendingWidth == FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption1,0x60,int32_t)) &&
     (pendingHeight == FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption1,0x64,enum UiNodeFlags))) {
    parentCursorOrSelectedRow = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption2,0x64,int32_t),FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption2,0x60,int32_t),
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2023,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2023,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption3,0x64,uint32_t),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption3,0x60,uint32_t),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2024,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2024,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,
                     (FrontendDisplayDimensionPixels)FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption4,0x64,struct UiNodeBase *),
                     (FrontendDisplayDimensionPixels)FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption4,0x60,struct UiNodeBase *),
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2025,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2025,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption5,0x64,int32_t),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption5,0x60,int32_t),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2026,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2026,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption6,0x64,uint32_t),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption6,0x60,uint32_t),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2027,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2027,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,(FrontendDisplayDimensionPixels)FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption7,0x64,struct UiNodeVtable *),
                     (FrontendDisplayDimensionPixels)FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption7,0x60,struct UiNodeBase *),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2028,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2028,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption8,0x64,int32_t),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption8,0x60,int32_t),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2029,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2029,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption9,0x64,int32_t),
                     FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption9,0x60,int32_t),adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202a,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202a,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode
                    (bitsPerPixel,FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption10,0x64,int32_t),FRONTEND_UI_FIELD(frontendRoot,displayResolutionOption10,0x60,int32_t),
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202b,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202b,frontendRoot);
  }
  UiSelectableGroup_SelectExclusive(10,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendRoot,displayResolutionOption10),
      FRONTEND_UI(frontendRoot,displayResolutionOption9),
      FRONTEND_UI(frontendRoot,displayResolutionOption8),
      FRONTEND_UI(frontendRoot,displayResolutionOption7),
      FRONTEND_UI(frontendRoot,displayResolutionOption6),
      FRONTEND_UI(frontendRoot,displayResolutionOption5),
      FRONTEND_UI(frontendRoot,displayResolutionOption4),
      FRONTEND_UI(frontendRoot,displayResolutionOption3),
      FRONTEND_UI(frontendRoot,displayResolutionOption2),
      FRONTEND_UI(frontendRoot,displayResolutionOption1));
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,0);
  parentCursorOrSelectedRow = frontendRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202c,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202c,frontendRoot);
  }
  if (adapterIndex == 0) {
    parentCursorOrSelectedRow = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption1);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,1);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202d,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202d,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,2);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202e,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202e,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,3);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202f,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202f,frontendRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactMode(bitsPerPixel,pendingHeight,pendingWidth,4);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2030,frontendRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2030,frontendRoot);
  }
  UiSelectableGroup_SelectExclusive(5,parentCursorOrSelectedRow,
      FRONTEND_UI(frontendRoot,displayAdapterOption5),
      FRONTEND_UI(frontendRoot,displayAdapterOption4),
      FRONTEND_UI(frontendRoot,displayAdapterOption3),
      FRONTEND_UI(frontendRoot,displayAdapterOption2),
      FRONTEND_UI(frontendRoot,displayAdapterOption1));
  persistedValue = PersistentSettings_Read(1,0);
  if ((((persistedValue == adapterIndex) &&
       (persistedValue = PersistentSettings_Read(0x280,4), persistedValue == pendingWidth)) &&
      (persistedValue = PersistentSettings_Read(0x1e0,8), persistedValue == pendingHeight)) &&
     (persistedValue = PersistentSettings_Read(0x10,0xc), persistedValue == bitsPerPixel)) {
    UiNodeList_SuppressActionId(0x2031,frontendRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(0x2031,frontendRoot);
  return;
}

