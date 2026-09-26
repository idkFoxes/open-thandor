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
   Ownership: ui/frontend/settings.
   Purpose: Activates task page 11, configures seven faction/player rows, selects the local player row, updates
   responsive visibility, and patches localized task-description template 0x218C.
   Local calls: FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/

void __thandor_void_preserve_eax_ecx_edx
FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView26C4 *frontendRootPage)

{
  UiNodeFlags *compactLayoutFlags;
  dword rowControlOffset;
  UiNodeVtable *rootVtable;
  int localPlayerRuntimeId;
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  dword activeCountOffsetOrLocalRow;
  uint rowOrAssignmentIndex;
  uint rowCursor;
  byte *rowTextIdBytes;
  dword assignableCountOrOffset;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceResolveEaxCf5 titleText;
  TextResourceResolveEaxCf5 templateText;
  FrontendPlayerRuntimeBlockCount remainingPlayerRecords;
  
  UiPageStack_SetActiveIndex(0xb,&frontendRootPage->primaryPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    compactLayoutFlags = &(frontendRootPage->compactLayoutControl).nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | 0x2000;
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  activeCountOffsetOrLocalRow = (g_FrontendLoadedLevelAsset->runtimeTail2E0).activeFactionCount;
  assignableCountOrOffset = (g_FrontendLoadedLevelAsset->runtimeTail2E0).assignableFactionCount;
  rowOrAssignmentIndex = 0;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowOrAssignmentIndex + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xfffffbff;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + rowControlOffset + 8;
    rowTextIdBytes[0] = 0x9a;
    rowTextIdBytes[1] = 0x21;
    rowTextIdBytes[2] = 0;
    rowTextIdBytes[3] = 0;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowOrAssignmentIndex + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xfffffbff;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowOrAssignmentIndex + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xfffffbfd;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowOrAssignmentIndex + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xffffffbf;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowOrAssignmentIndex + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (rowControlOffset - 4)) & 0xfffffff7;
    rowCursor = rowOrAssignmentIndex + 1;
    *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + rowControlOffset) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowOrAssignmentIndex + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    activeCountOffsetOrLocalRow = activeCountOffsetOrLocalRow - 1;
    assignableCountOrOffset = assignableCountOrOffset - 1;
    rowOrAssignmentIndex = rowCursor;
  } while (assignableCountOrOffset != 0);
  for (; activeCountOffsetOrLocalRow != 0; activeCountOffsetOrLocalRow = activeCountOffsetOrLocalRow - 1) {
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) & 0xfffffff7;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xfffffbff;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + assignableCountOrOffset + 8;
    rowTextIdBytes[0] = 0x9a;
    rowTextIdBytes[1] = 0x21;
    rowTextIdBytes[2] = 0;
    rowTextIdBytes[3] = 0;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xfffffbff;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xfffffffd;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) | 0x400;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xffffffbf;
    assignableCountOrOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (assignableCountOrOffset - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) =
         *(uint *)(frontendRootPage->taskRowControls04C + assignableCountOrOffset) & 0xffffffbf;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor = rowCursor + 1;
  }
  for (; rowCursor < 7; rowCursor = rowCursor + 1) {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x400;
    rowTextIdBytes = frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow + 8;
    rowTextIdBytes[0] = 0x99;
    rowTextIdBytes[1] = 0x21;
    rowTextIdBytes[2] = 0;
    rowTextIdBytes[3] = 0;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x400;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) & 0xfffffffd;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x400;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x40;
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor + 1];
    *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) =
         *(uint *)(frontendRootPage->taskRowControls04C + (activeCountOffsetOrLocalRow - 4)) | 8;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) | 0x40;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] = 0;
  }
  do {
    activeCountOffsetOrLocalRow = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow + 8) =
         *(int *)((int)&loadedLevel->playerSlots[0].aiClassOrMode +
                 g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowCursor - 1]) + 0x2173 + rowCursor;
    *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) =
         *(uint *)(frontendRootPage->taskRowControls04C + activeCountOffsetOrLocalRow) & 0xfffffffd;
    localPlayerRuntimeId = g_LocalPlayerRuntimeId;
    rowCursor = rowCursor - 1;
  } while (rowCursor != 0);
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
    rowTextIdBytes[0x54] = 0x98;
    rowTextIdBytes[0x55] = 0x21;
    rowTextIdBytes[0x56] = 0;
    rowTextIdBytes[0x57] = 0;
    if (localPlayerRuntimeId == playerRecord->playerRuntimeId) {
      activeCountOffsetOrLocalRow = rowOrAssignmentIndex - 1;
    }
    rowOrAssignmentIndex = rowOrAssignmentIndex + 1;
    playerRecord = playerRecord + 1;
    if ((loadedLevel->runtimeTail2E0).assignableFactionCount < rowOrAssignmentIndex) {
      rowOrAssignmentIndex = rowOrAssignmentIndex - (loadedLevel->runtimeTail2E0).assignableFactionCount;
    }
    remainingPlayerRecords = remainingPlayerRecords - 1;
  } while (remainingPlayerRecords != 0);
  *(uint *)(frontendRootPage->taskRowControls04C +
           g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1]) =
       *(uint *)(frontendRootPage->taskRowControls04C +
                g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1]) | 2;
  *(uint *)(frontendRootPage->taskRowControls04C +
           (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1] - 4)) =
       *(uint *)(frontendRootPage->taskRowControls04C +
                (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[activeCountOffsetOrLocalRow + 1] - 4)) &
       0xfffffff7;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls((UiRootNode *)frontendRootPage);
  rootVtable = (frontendRootPage->rootNode).vtable;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    frontendRootPage->taskPageControlState55C[0x1cc] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0x60;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] | 8;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x176] =
         frontendRootPage->taskPageControlState55C[0x176] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x177] =
         frontendRootPage->taskPageControlState55C[0x177] & 0xfffffbff;
  }
  else {
    frontendRootPage->taskPageControlState55C[0x1cc] = 0;
    frontendRootPage->taskPageControlState55C[0x1ce] = 0;
    frontendRootPage->taskPageControlState55C[0x1be] =
         frontendRootPage->taskPageControlState55C[0x1be] & 0xfffffff7;
    frontendRootPage->taskPageControlState55C[0x1bf] =
         frontendRootPage->taskPageControlState55C[0x1bf] & 0xfffffffd;
    frontendRootPage->taskPageControlState55C[0x18e] =
         frontendRootPage->taskPageControlState55C[0x18e] | 8;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] | 0x400;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] | 8;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] | 0x400;
    }
    else {
      frontendRootPage->taskPageControlState55C[399] =
           frontendRootPage->taskPageControlState55C[399] & 0xfffffbff;
      frontendRootPage->taskPageControlState55C[0x176] =
           frontendRootPage->taskPageControlState55C[0x176] & 0xfffffff7;
      frontendRootPage->taskPageControlState55C[0x177] =
           frontendRootPage->taskPageControlState55C[0x177] & 0xfffffbff;
    }
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  (*rootVtable->layout)(&frontendRootPage->rootNode);
  titleText = TextResource_Resolve((loadedLevel->header).titleTextResourceIndex + 0x2230);
  *titleText.eax = 0x8000;
  templateText = TextResource_Resolve(0x218c);
  RichTextCommandStream_PatchPayloadBySelector(0,titleText.eax,templateText.eax);
  return;
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
FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *displaySettingsRoot)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       displaySettingsRoot[1].top;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       displaySettingsRoot[1].right;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
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
FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *displaySettingsRoot)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = displaySettingsRoot[1].top;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(displaySettingsRoot);
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
  dword previousWidth;
  dword previousHeight;
  dword previousAdapterIndex;
  undefined4 selectedAdapterIndex;
  undefined4 selectedWidth;
  undefined4 selectedHeight;
  undefined4 selectedBitsPerPixel;
  int colorBitsCounterOrParentLink;
  GraphicsTextureSourceAsset **fontTextureSource;
  DisplayModeEaxCf5 selectedModeResult;
  DisplayModeEaxCf5 restoredModeResult;
  
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
  selectedModeResult = (*g_GraphicsDisplayModeHook)
                    (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.bitsPerPixel,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width);
  if (selectedModeResult.carry) {
    restoredModeResult = (*g_GraphicsDisplayModeHook)(previousAdapterIndex,colorBitsCounterOrParentLink + 0xfU & 0xfffffff0,previousHeight,previousWidth);
    (*g_FatalErrorPrimaryDispatchCf)(restoredModeResult.eax,restoredModeResult.carry);
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    (*g_FatalErrorRuntimeDispatchCf)(selectedModeResult.eax,true);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    adapterIndex = PersistentSettings_ReadDword(1,0);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
         PersistentSettings_ReadDword(0x280,4);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
         PersistentSettings_ReadDword(0x1e0,8);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    bitsPerPixel = PersistentSettings_ReadDword(0x10,0xc);
    FrontendDisplaySettingsPage_UpdateModeActionAvailability(control);
    return;
  }
  PersistentSettings_WriteDword(selectedAdapterIndex,0);
  PersistentSettings_WriteDword(selectedWidth,4);
  PersistentSettings_WriteDword(selectedHeight,8);
  PersistentSettings_WriteDword(selectedBitsPerPixel,0xc);
  UiRootStack_Relayout();
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_FrontendMenuTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowTextureSource);
  (*g_GraphicsTextureSourceConvertPaletteEntries)
            ((GraphicsPaletteTextureSourceAsset *)g_UiWindowClassTextureSource);
  fontTextureSource = g_FontTextureSources;
  colorBitsCounterOrParentLink = 2;
  do {
    (*g_GraphicsTextureSourceConvertPaletteEntries)((GraphicsPaletteTextureSourceAsset *)*fontTextureSource);
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
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)((int)control + 0x3b4) = *(uint *)((int)control + 0x3b4) | 0x2000;
  }
  else {
    *(uint *)((int)control + 0x3b4) = *(uint *)((int)control + 0x3b4) & 0xffffdfff;
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
  dword *sourceDwordCursor;
  dword *playerNameDwordCursor;
  
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
    PersistentSettings_WriteDwords(0x28,(dword *)control->textPrefix6C,0x60);
    sourceDwordCursor = (dword *)control->textPrefix6C;
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
  PersistentSettings_WriteDword(control->boundValue,0x44);
  return;
}


/* Address: 0x0054A810.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x2049. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[73] (0x2049). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 4;
  }
  else {
    value = optionFlags & 0xfffffffb;
  }
  PersistentSettings_WriteDword(value,0x40);
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
  PersistentSettings_WriteDword(control->boundValue,0x48);
  return;
}


/* Address: 0x0054A870.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203C. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[60] (0x203C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 1;
  }
  else {
    value = optionFlags & 0xfffffffe;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0054A8B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203D. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[61] (0x203D). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x40);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 2;
  }
  else {
    value = optionFlags & 0xfffffffd;
  }
  PersistentSettings_WriteDword(value,0x40);
  return;
}


/* Address: 0x0054A8F0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203E. Updates bit 0 of optionFlags5C. Selecting it suppresses the paired
   action 0x203F; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[62] (0x203E).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 1;
    UiNodeList_SuppressActionId(0x203f,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffe;
    UiNodeList_UnsuppressActionId(0x203f,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A950.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x203F. Updates bit 1 of optionFlags5C. Selecting it suppresses the paired
   action 0x203E; clearing it restores that action. The original label remains unresolved. Queued UI action handler
   for FRONTEND_PAGE20[63] (0x203F).
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 2;
    UiNodeList_SuppressActionId(0x203e,(control->base).parent);
  }
  else {
    value = optionFlags & 0xfffffffd;
    UiNodeList_UnsuppressActionId(0x203e,(control->base).parent);
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A9B0.
   Ownership: ui/frontend/settings.
   Purpose: Registered as UI action 0x2051. The original user-facing label remains unresolved. Queued UI action
   handler for FRONTEND_PAGE20[81] (0x2051). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiSelectableControl_IsSelectedCf
   [ui/controls/lists], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  dword optionFlags;
  PersistentSettingsDwordValue value;
  bool isSelected;
  
  optionFlags = PersistentSettings_ReadDword(0,0x5c);
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    value = optionFlags | 4;
  }
  else {
    value = optionFlags & 0xfffffffb;
  }
  PersistentSettings_WriteDword(value,0x5c);
  return;
}


/* Address: 0x0054A9F0.
   Ownership: ui/frontend/settings.
   Purpose: Selects gameplay-settings page five, applies compact layout, restores optionFlags40 and optionFlags5C
   controls with mutual-exclusion suppression, and restores the camera-scroll setting.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], PersistentSettings_ReadDword
   [core/settings/persistent], UiSelectableControl_SetSelected [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  sdword *compactLayoutFlags;
  dword persistedValue;
  
  UiPageStack_SetActiveIndex(5,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    compactLayoutFlags = &FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,sdword);
    *compactLayoutFlags = *compactLayoutFlags | 0x2000;
  }
  persistedValue = PersistentSettings_ReadDword(0,0x40);
  UiSelectableControl_SetSelected
            (persistedValue & 1,(UiSelectableControl *)FRONTEND_UI(frontendRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected
            (persistedValue & 2,(UiSelectableControl *)FRONTEND_UI(frontendRoot,autoRotationOffCheckbox));
  UiSelectableControl_SetSelected(persistedValue & 4,(UiSelectableControl *)FRONTEND_UI(frontendRoot,hidePanelCheckbox));
  persistedValue = PersistentSettings_ReadDword(0,0x5c);
  if ((persistedValue & 1) != 0) {
    UiNodeList_SuppressActionId(0x203f,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected
            (persistedValue & 1,(UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationZoomCheckbox));
  if ((persistedValue & 2) != 0) {
    UiNodeList_SuppressActionId(0x203e,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected(persistedValue & 2,(UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationTiltCheckbox));
  persistedValue = PersistentSettings_ReadDword(0x20,0x48);
  FRONTEND_UI_FIELD(frontendRoot,scrollSpeedSlider,0x58,dword) = persistedValue;
  return;
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
  dword persistedValue;
  dword shadingDepthQuarter;
  int shadingDepth;
  UiNodeBase *parentCursorOrSelectedRow;
  
  UiPageStack_SetActiveIndex(7,(UiPageStackControl *)(source[-2].reserved4C_1067 + 0xa20));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)(source[-2].reserved4C_1067 + 0x8cc) =
         *(uint *)(source[-2].reserved4C_1067 + 0x8cc) | 0x2000;
  }
  persistedValue = PersistentSettings_ReadDword(1,0x1c);
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
  persistedValue = PersistentSettings_ReadDword(0x20,0x10);
  shadingDepthQuarter = PersistentSettings_ReadDword(0x10,0x18);
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
      THANDOR_UI_AT(source,0x1324),
      THANDOR_UI_AT(source,0x12bc),
      THANDOR_UI_AT(source,0x1254),
      THANDOR_UI_AT(source,0x11ec),
      THANDOR_UI_AT(source,0x1184),
      THANDOR_UI_AT(source,0x111c));
  persistedValue = PersistentSettings_ReadDword(1,0x30);
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
      THANDOR_UI_AT(source,0x161c),
      THANDOR_UI_AT(source,0x15bc),
      THANDOR_UI_AT(source,0x155c));
  persistedValue = PersistentSettings_ReadDword(0x10000,0x34);
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
  dword audioFlags;
  dword gainValue;
  
  UiPageStack_SetActiveIndex(8,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    compactLayoutFlags = &THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->pageRoot.nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | 0x2000;
  }
  audioFlags = PersistentSettings_ReadDword(3,0x20);
  UiSelectableControl_SetSelected(audioFlags & 1,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 2,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & 4,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->reverseStereoControl);
  gainValue = PersistentSettings_ReadDword(0x8000,0x24);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->soundEffectsGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_ReadDword(0x8000,0x28);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage417C, sourceNode)->movieDefaultAudioGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_ReadDword(0x8000,0x2c);
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
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists], PersistentSettings_WriteDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx
FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  byte isSelected;
  UiNodeBase *parentCursor;
  
  isSelected = UiSelectableControl_IsSelectedCf(control);
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
  PersistentSettings_WriteDword(isSelected & 1,0x1c);
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
  UiNodeVtable *shadingGridSize;
  UiNodeBase *shadingGroupRoot;
  uint shadingDepthQuarter;
  UiNodeBase *selectedControl;
  
  shadingGridSize = control[1].base.vtable;
  shadingDepthQuarter = (uint)control[1].base.left >> 2;
  PersistentSettings_WriteDword((int)shadingGridSize * 2,0x14);
  PersistentSettings_WriteDword((PersistentSettingsDwordValue)shadingGridSize,0x10);
  PersistentSettings_WriteDword(shadingDepthQuarter,0x18);
  shadingGroupRoot = (control->base).parent;
  if (shadingGridSize == (UiNodeVtable *)0x20) {
    selectedControl = (UiNodeBase *)&shadingGroupRoot[1].parent;
    if (shadingDepthQuarter == 0x10) {
      selectedControl = (UiNodeBase *)&shadingGroupRoot[2].topOffset;
    }
    else if (shadingDepthQuarter == 0x20) {
      selectedControl = (UiNodeBase *)&shadingGroupRoot[3].layoutWidth;
    }
  }
  else if (shadingGridSize == (UiNodeVtable *)0x40) {
    selectedControl = (UiNodeBase *)&shadingGroupRoot[5].left;
    if (shadingDepthQuarter == 0x20) {
      selectedControl = (UiNodeBase *)&shadingGroupRoot[6].bottomOffset;
    }
  }
  else {
    selectedControl = (UiNodeBase *)&shadingGroupRoot[7].nodeFlags;
  }
  UiSelectableGroup_SelectExclusive(6,selectedControl,
      THANDOR_UI_AT((control->base).parent,0x25c),
      THANDOR_UI_AT((control->base).parent,0x1f4),
      THANDOR_UI_AT((control->base).parent,0x18c),
      THANDOR_UI_AT((control->base).parent,0x124),
      THANDOR_UI_AT((control->base).parent,0xbc),
      THANDOR_UI_AT((control->base).parent,0x54));
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
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x34);
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
  UiNodeBase *graphicsSettingsRoot;
  
  graphicsSettingsRoot = (control->base).parent;
  if ((UiSelectableControl *)&graphicsSettingsRoot[1].parent == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[1].parent;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[2].bottom == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[2].bottom;
  }
  if ((UiSelectableControl *)&graphicsSettingsRoot[3].leftAnchorQ31 == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = (UiNodeBase *)&graphicsSettingsRoot[3].leftAnchorQ31;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      THANDOR_UI_AT((control->base).parent,0x114),
      THANDOR_UI_AT((control->base).parent,0xb4),
      THANDOR_UI_AT((control->base).parent,0x54));
  PersistentSettings_WriteDword(qualityLevel,0x30);
  g_TextureDownsampleShift = qualityLevel >> 1;
  (*g_GraphicsRebuildAllStagingTextures)();
  return;
}


/* Address: 0x0054BE90.
   Ownership: ui/frontend/settings.
   Purpose: Queued UI action handler for FRONTEND_PAGE20[24] (0x2018). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx
FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  dword audioFlags;
  AudioMixerGainQ15 effectsGain;
  MovieAudioGainQ15 movieDefaultGain;
  MovieAudioGainQ15 movieAlternateGain;
  bool isSelected;
  
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  audioFlags = PersistentSettings_ReadDword(3,0x20);
  PersistentSettings_WriteDword((uint)isSelected | audioFlags & 0xfffffffe,0x20);
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
    effectsGain = PersistentSettings_ReadDword(0x8000,0x24);
  }
  movieDefaultGain = 0;
  g_UiSoundGainQ15 = effectsGain;
  g_SoundEffectsGainQ15 = effectsGain;
  if (isSelected) {
    movieDefaultGain = PersistentSettings_ReadDword(0x8000,0x28);
  }
  movieAlternateGain = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGain;
  if (isSelected) {
    movieAlternateGain = PersistentSettings_ReadDword(0x8000,0x4c);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGain;
  return;
}


/* Address: 0x0054BFD0.
   Ownership: ui/frontend/settings.
   Purpose: Updates SOUND_OPTIONS_MUSIC_ENABLED, creates or stops the frontend looping-music voice, applies
   musicGainQ15, and updates related frontend controls. Queued UI action handler for FRONTEND_PAGE20[25] (0x2019).
   Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], Resource_Load
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
  dword gainOrAudioFlags;
  uint musicEnabledBit;
  uint newAudioFlags;
  bool isSelected;
  SoundCreateSampleVoiceSetEaxCf5 createVoiceResult;
  SoundPlayVoiceEaxCf5 playResult;
  ResourceLoadEaxEcxCf9 loadResult;
  
  musicEnabledBit = 0;
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    musicEnabledBit = 2;
    (*g_GraphicsCursorSetFrame)(6);
    loadResult = Resource_Load((word *)u_sound_music00_sam_00545c4e);
    musicSample = (SoundSampleAsset *)loadResult.eax;
    activeMusicBuffer = g_FrontendMusicActiveBuffer;
    if (!loadResult.carry) {
      createVoiceResult = (*g_SoundCreateSampleVoiceSet)(musicSample);
      musicVoiceSet = createVoiceResult.eax;
      if (createVoiceResult.carry) {
        Resource_Release(musicSample);
        activeMusicBuffer = g_FrontendMusicActiveBuffer;
      }
      else {
        g_FrontendMusicVoiceSet = musicVoiceSet;
        Resource_Release(musicSample);
        gainOrAudioFlags = PersistentSettings_ReadDword(0x8000,0x2c);
        playResult = (*g_SoundPlayLooping)(gainOrAudioFlags,gainOrAudioFlags,musicVoiceSet);
        activeMusicBuffer = playResult.eax;
        if (playResult.carry) {
          (*g_SoundReleaseSampleVoiceSet)(musicVoiceSet);
          g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
          activeMusicBuffer = g_FrontendMusicActiveBuffer;
        }
      }
    }
    g_FrontendMusicActiveBuffer = activeMusicBuffer;
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    (*g_SoundStopVoice)(g_FrontendMusicActiveBuffer);
    (*g_SoundReleaseSampleVoiceSet)(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = (IDirectSoundBuffer *)0x0;
    g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
  }
  gainOrAudioFlags = PersistentSettings_ReadDword(3,0x20);
  newAudioFlags = musicEnabledBit | gainOrAudioFlags & 0xfffffffd;
  PersistentSettings_WriteDword(newAudioFlags,0x20);
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
   Cross-module calls: UiSelectableControl_IsSelectedCf [ui/controls/lists], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  dword currentAudioFlags;
  uint reverseStereoBit;
  sdword reverseStereoMask;
  bool isSelected;
  
  reverseStereoBit = 0;
  reverseStereoMask = 0;
  isSelected = (bool)UiSelectableControl_IsSelectedCf(control);
  if (isSelected) {
    reverseStereoBit = 4;
    reverseStereoMask = -1;
  }
  currentAudioFlags = PersistentSettings_ReadDword(3,0x20);
  g_ReverseStereoMask = reverseStereoMask;
  PersistentSettings_WriteDword(reverseStereoBit | currentAudioFlags & 0xfffffffb,0x20);
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
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x24);
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
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x28);
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
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x4c);
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
  PersistentSettingsDwordValue value;
  dword musicGainQ15;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x2c);
  (*g_SoundSetVoiceGains)(value,value,g_FrontendMusicActiveBuffer);
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
  PersistentSettingsDwordValue value;
  
  value = control->boundValue;
  PersistentSettings_WriteDword(value,0x3c);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,value,
             (word *)&g_FrontendNetworkPlayerCountTextUtf16);
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
    PersistentSettings_WriteDwords(0x28,(dword *)control->textPrefix6C,0x88);
  }
  return;
}


/* Address: 0x0054D250.
   Ownership: ui/frontend/settings.
   Purpose: Updates action 0x2002 availability from the network-settings control state and player-name presence,
   then publishes the player descriptor when the control dirty bit requires it. Queued UI action handler for
   FRONTEND_PAGE20[9] (0x2009). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf.
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
    if (((uint)(networkSettings->textEditView).textEdit.base.parent & 4) != 0) {
      dirtyFlagsSlot = &(networkSettings->textEditView).textEdit.base.parent;
      *dirtyFlagsSlot = (UiNodeBase *)((uint)*dirtyFlagsSlot & 0xfffffffb);
      FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
                ((FrontendNetworkSettingsControlView250 *)((int)firstNode + 0x4980));
    }
  }
  return;
}


/* Address: 0x00549620.
   Ownership: ui/frontend/settings.
   Purpose: Rebuilds faction and player control availability, selection state, labels, assignment buffers, and the
   localized task-description payload for the active frontend task-assignment page.
*/
void
FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot)

{
  sdword *rosterLayoutFlags;
  uint *controlFlags;
  word scannedChar;
  dword controlOffset;
  uint factionIndexOrSetMask;
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
  word *rosterTextCursor;
  word *rosterScanCursor;
  uint controlClearMask;
  uint controlSetMask;
  
  controlSetMask = 0;
  controlClearMask = 0xfffffff7;
  localFactionIndex = (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingSearchCount = g_FrontendPlayerRuntimeBlockCount;
  networkedOrRemainingCount = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (networkedOrRemainingCount != SESSION_NETWORK_ROLE_LOCAL) {
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localFactionIndex = (playerRecord->factionAssignment).factionAssignmentIndex;
      if ((playerRecord->factionAssignment).consensusValue != 0) {
        controlSetMask = 8;
        controlClearMask = 0xffffffff;
      }
      break;
    }
    playerRecord = playerRecord + 1;
    remainingSearchCount = remainingSearchCount - SESSION_NETWORK_ROLE_CLIENT;
    networkedOrRemainingCount = remainingSearchCount;
  }
  rowIndexOrCount = 7;
  FRONTEND_UI_FIELD(taskAssignmentRoot,taskDescriptionText,0x54,struct UiNodeBase *) =
       (UiNodeBase *)
       (localFactionIndex + 0x230010 + (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10);
  do {
    if ((*(uint *)((int)&taskAssignmentRoot->rootFlags +
                  g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount]) & 2) != 0)
    goto FrontendTaskAssignment_ApplyEligibleFactionControlState;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL)
         && (controlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount],
            (*(uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset) & 8) != 0)) {
        if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & 0x40) != 0)
        goto FrontendTaskAssignment_DisableUnavailableFactionControl;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
               SESSION_NETWORK_ROLE_LOCAL) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
        controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags | 8;
        controlFlags = (uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
        *controlFlags = *controlFlags & 0xfffffbff;
        goto FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop;
      }
FrontendTaskAssignment_ApplyEligibleFactionControlState:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags & 0xfffffbff;
    }
    else {
FrontendTaskAssignment_DisableUnavailableFactionControl:
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount];
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | 8;
      controlFlags = (uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags | 0x400;
    }
FrontendTaskAssignment_DisablePlayerControlAndAdvanceFactionLoop:
    controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount]);
    *controlFlags = *controlFlags | 8;
    rowIndexOrCount = rowIndexOrCount + -1;
  } while (rowIndexOrCount != 0);
  rowIndexOrCount = 7;
  do {
    controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount];
    *(undefined4 *)((int)&taskAssignmentRoot->previousRoot + controlOffset) = 0x2199;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowIndexOrCount] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
       (*(undefined4 *)((int)&taskAssignmentRoot->previousRoot + controlOffset) = 0x219a,
       (*(uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                 g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount]) & 8) ==
       0)) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | controlSetMask;
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags & controlClearMask;
      controlFlags = (uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags & 0xfffffbff;
    }
    else {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
      *controlFlags = *controlFlags | 8;
      controlFlags = (uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset);
      *controlFlags = *controlFlags | 0x400;
    }
    loadedLevelOrClearMask = g_FrontendLoadedLevelAsset;
    rowIndexOrCount = rowIndexOrCount + -1;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (rowIndexOrCount != 0);
  do {
    factionIndexOrSetMask = (playerRecord->factionAssignment).factionAssignmentIndex;
    *(undefined4 *)
     ((int)&taskAssignmentRoot->previousRoot +
     g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[factionIndexOrSetMask]) = 0x2198;
    remainingPlayers = remainingPlayers - 1;
    playerRecord = playerRecord + 1;
  } while (remainingPlayers != 0);
  rowIndexOrCount = 7;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if ((*(uint *)((int)&taskAssignmentRoot->rootFlags +
                    g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowIndexOrCount]) &
          0x40) == 0) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndexOrCount];
        controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags | factionIndexOrSetMask;
        controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
        *controlFlags = *controlFlags & (uint)loadedLevelOrClearMask;
      }
      rowIndexOrCount = rowIndexOrCount + -1;
      remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
    } while (rowIndexOrCount != 0);
    do {
      controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets
                       [(playerRecord->factionAssignment).factionAssignmentIndex]);
      *controlFlags = *controlFlags | 8;
      remainingPlayers = remainingPlayers - 1;
      playerRecord = playerRecord + 1;
    } while (remainingPlayers != 0);
  }
  textRowCursor = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows + 1;
  for (rowIndexOrCount = 0x8c; rowIndexOrCount != 0; rowIndexOrCount = rowIndexOrCount + -1) {
    textRowCursor->textUtf16[0] = 0;
    textRowCursor->textUtf16[1] = 0;
    textRowCursor = (FrontendTaskAssignmentGeneratedFactionTextRow50 *)(textRowCursor->textUtf16 + 2);
  }
  rosterLayoutFlags = &FRONTEND_UI_FIELD(taskAssignmentRoot,rosterParticipantHeader,0x48,sdword);
  *rosterLayoutFlags = *rosterLayoutFlags | 8;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  rosterLayoutFlags = &FRONTEND_UI_FIELD(taskAssignmentRoot,rosterParticipantHeader,0x48,sdword);
  *rosterLayoutFlags = *rosterLayoutFlags & 0xfffffff7;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerName = &playerRecord->playerName;
  do {
    rosterTextCursor = (word *)((int)&g_FrontendUiDisplayModeAndTaskAssignmentScratch +
                      *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(playerName + 1) + 1) + 8) * 0x50);
    rowIndexOrCount = 0x28;
    if (*(int *)rosterTextCursor == 0) {
FrontendTaskAssignment_AppendPlayerNameToFactionRosterText:
      if (0x15 < rowIndexOrCount) {
        rowIndexOrCount = 0x14;
      }
      if (*(FrontendConsensusValue *)
           ((int)((UiTransferEndpointDescriptor *)(playerName + 1) + 1) + 0xc) == 0) {
        *rosterTextCursor = 0x8001;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount = rowIndexOrCount + -1) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16_28 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor = rosterTextCursor + 1;
        }
        rosterTextCursor[0] = 0x8000;
        rosterTextCursor[1] = 0;
      }
      else {
        *rosterTextCursor = 0x8000;
        nameCharCursor = playerName;
        for (; rowIndexOrCount != 0; rowIndexOrCount = rowIndexOrCount + -1) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16_28 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor = rosterTextCursor + 1;
        }
        *rosterTextCursor = 0;
      }
    }
    else {
      do {
        rosterScanCursor = rosterTextCursor;
        if (rowIndexOrCount == 0) break;
        rowIndexOrCount = rowIndexOrCount + -1;
        rosterScanCursor = rosterTextCursor + 1;
        scannedChar = *rosterTextCursor;
        rosterTextCursor = rosterScanCursor;
      } while (scannedChar != 0);
      rosterTextCursor = rosterScanCursor + 1;
      rowIndexOrCount = rowIndexOrCount + -1;
      if (rowIndexOrCount != 0) {
        rosterScanCursor[-0xffffffff00000001] = 0x2c;
        rosterScanCursor[0] = 0x20;
        goto FrontendTaskAssignment_AppendPlayerNameToFactionRosterText;
      }
    }
    remainingPlayers = remainingPlayers - 1;
    playerName = playerName + 0x7e;
    if (remainingPlayers == 0) {
      rowIndexOrCount = 7;
      if (((uint)FRONTEND_UI_FIELD(taskAssignmentRoot,factionSetupFinishButton,0x4C,struct UiRootCallbacks *) & 2) == 0) {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
          if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & 0x400) == 0) {
            controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
            *controlFlags = *controlFlags & 0xfffffff7;
          }
          rowIndexOrCount = rowIndexOrCount + -1;
          remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
        } while (rowIndexOrCount != 0);
      }
      else {
        do {
          controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndexOrCount];
          if ((*(uint *)((int)&taskAssignmentRoot->rootFlags + controlOffset) & 0x400) == 0) {
            controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags + controlOffset);
            *controlFlags = *controlFlags | 8;
          }
          rowIndexOrCount = rowIndexOrCount + -1;
          remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
        } while (rowIndexOrCount != 0);
      }
      do {
        if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
          rowIndexOrCount = (playerRecord->factionAssignment).factionAssignmentIndex;
          otherPlayerRecord = g_FrontendPlayerRuntimeBlocks;
          while ((rowIndexOrCount != (otherPlayerRecord->factionAssignment).factionAssignmentIndex ||
                 ((int)(playerRecord->factionAssignment).readyOrWaitState <=
                  (int)(otherPlayerRecord->factionAssignment).readyOrWaitState))) {
            otherPlayerRecord = otherPlayerRecord + 1;
            remainingPlayers = remainingPlayers - 1;
            if (remainingPlayers == 0) {
              return;
            }
          }
          controlFlags = (uint *)((int)&(taskAssignmentRoot->base).nodeFlags +
                           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndexOrCount]);
          *controlFlags = *controlFlags | 8;
          return;
        }
        remainingPlayers = remainingPlayers - 1;
        playerRecord = playerRecord + 1;
      } while (remainingPlayers != 0);
      return;
    }
  } while( true );
}


/* Address: 0x0054CD20.
   Ownership: ui/frontend/settings.
   Purpose: Copies the selected frontend player endpoint descriptor into the active transfer endpoint, publishes
   its sender context, resets the sequence token to -1, and invokes UiTransfer_SendPlayerDescriptorPacket20002Cf
   while preserving the backend CF result. Queued UI action handler for FRONTEND_PAGE20[2] (0x2002). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: UiTransfer_SendPlayerDescriptorPacket20002Cf [network/protocol/transfer].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
          (FrontendNetworkSettingsControlView250 *networkSettings)

{
  int remainingDwords;
  dword *selectedPlayerRecordDwordCursor;
  dword *selectedEndpointDwordCursor;
  bool sendCarry;
  
  g_FrontendSessionToken = *(undefined4 *)(**(int **)(networkSettings->raw + 0x248) + 4);
  selectedPlayerRecordDwordCursor = (dword *)(**(int **)(networkSettings->raw + 0x248) + 0xa0);
  selectedEndpointDwordCursor = (dword *)&g_FrontendSelectedNetworkEndpoint;
  for (remainingDwords = 4; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
    *selectedEndpointDwordCursor = *selectedPlayerRecordDwordCursor;
    selectedPlayerRecordDwordCursor = selectedPlayerRecordDwordCursor + 1;
    selectedEndpointDwordCursor = selectedEndpointDwordCursor + 1;
  }
  g_FrontendSelectedPlayerToken = 0xffffffff;
  sendCarry = UiTransfer_SendPlayerDescriptorPacket20002Cf();
  return sendCarry;
}


/* Address: 0x0054B160.
   Ownership: ui/frontend/settings.
   Purpose: Walks to the display-settings root, checks the pending adapter, width, height, and bit-depth tuple
   against enumerated modes, updates actions 0x201E through 0x2030, selects the matching groups, and gates action
   0x2031 when settings are unchanged.
   Cross-module calls: DisplayModeTable_ContainsExactModeCf [graphics/backend/directdraw],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists],
   UiSelectableGroup_SelectExclusive [ui/controls/lists], PersistentSettings_ReadDword [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *displaySettingsRoot)

{
  undefined4 adapterIndex;
  undefined4 pendingWidth;
  undefined4 pendingHeight;
  undefined4 bitsPerPixel;
  dword persistedValue;
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
  parentCursorOrSelectedRow = displaySettingsRoot->parent;
  while (parentCursorOrSelectedRow != (UiNodeBase *)0xffffffff) {
    displaySettingsRoot = displaySettingsRoot->parent;
    parentCursorOrSelectedRow = displaySettingsRoot->parent;
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb4].topAnchorQ31,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex);
  parentCursorOrSelectedRow = displaySettingsRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x201e,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201e,displaySettingsRoot);
  }
  if (bitsPerPixel == displaySettingsRoot[0xb4].topAnchorQ31) {
    parentCursorOrSelectedRow = (UiNodeBase *)&displaySettingsRoot[0xb3].leftOffset;
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    ((FrontendColorDepthBits)displaySettingsRoot[0xb6].firstChild,pendingHeight,pendingWidth
                     ,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x201f,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201f,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb7].leftOffset,pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2020,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2020,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (displaySettingsRoot[0xb8].bottomAnchorQ31,pendingHeight,pendingWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2021,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2021,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(4,parentCursorOrSelectedRow,
      THANDOR_UI_AT(displaySettingsRoot,0x367c),
      THANDOR_UI_AT(displaySettingsRoot,0x3614),
      THANDOR_UI_AT(displaySettingsRoot,0x35ac),
      THANDOR_UI_AT(displaySettingsRoot,0x3544));
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa5].nodeFlags,
                     displaySettingsRoot[0xa5].layoutHeight,adapterIndex);
  parentCursorOrSelectedRow = displaySettingsRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2022,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2022,displaySettingsRoot);
  }
  if ((pendingWidth == displaySettingsRoot[0xa5].layoutHeight) &&
     (pendingHeight == displaySettingsRoot[0xa5].nodeFlags)) {
    parentCursorOrSelectedRow = (UiNodeBase *)&displaySettingsRoot[0xa4].leftAnchorQ31;
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa7].right,displaySettingsRoot[0xa7].top,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2023,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2023,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xa8].topAnchorQ31,
                     displaySettingsRoot[0xa8].leftAnchorQ31,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2024,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2024,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].firstChild,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xaa].nextSibling,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2025,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2025,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xab].leftOffset,
                     displaySettingsRoot[0xab].bottom,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2026,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2026,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xac].bottomAnchorQ31,
                     displaySettingsRoot[0xac].rightAnchorQ31,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2027,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2027,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,(FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].vtable,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xae].parent,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2028,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2028,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xaf].rightOffset,
                     displaySettingsRoot[0xaf].topOffset,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2029,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2029,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xb0].layoutHeight,
                     displaySettingsRoot[0xb0].layoutWidth,adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202a,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202a,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf
                    (bitsPerPixel,displaySettingsRoot[0xb2].top,displaySettingsRoot[0xb2].left,
                     adapterIndex);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202b,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202b,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(10,parentCursorOrSelectedRow,
      THANDOR_UI_AT(displaySettingsRoot,0x3488),
      THANDOR_UI_AT(displaySettingsRoot,0x3420),
      THANDOR_UI_AT(displaySettingsRoot,0x33b8),
      THANDOR_UI_AT(displaySettingsRoot,0x3350),
      THANDOR_UI_AT(displaySettingsRoot,0x32e8),
      THANDOR_UI_AT(displaySettingsRoot,0x3280),
      THANDOR_UI_AT(displaySettingsRoot,0x3218),
      THANDOR_UI_AT(displaySettingsRoot,0x31b0),
      THANDOR_UI_AT(displaySettingsRoot,0x3148),
      THANDOR_UI_AT(displaySettingsRoot,0x30e0));
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,pendingHeight,pendingWidth,0);
  parentCursorOrSelectedRow = displaySettingsRoot;
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202c,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202c,displaySettingsRoot);
  }
  if (adapterIndex == 0) {
    parentCursorOrSelectedRow = (UiNodeBase *)&displaySettingsRoot[0x9c].topAnchorQ31;
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,pendingHeight,pendingWidth,1);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202d,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202d,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,pendingHeight,pendingWidth,2);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202e,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202e,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,pendingHeight,pendingWidth,3);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x202f,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202f,displaySettingsRoot);
  }
  modeCheckCarry = DisplayModeTable_ContainsExactModeCf(bitsPerPixel,pendingHeight,pendingWidth,4);
  if (modeCheckCarry) {
    UiNodeList_SuppressActionId(0x2030,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2030,displaySettingsRoot);
  }
  UiSelectableGroup_SelectExclusive(5,parentCursorOrSelectedRow,
      THANDOR_UI_AT(displaySettingsRoot,0x3024),
      THANDOR_UI_AT(displaySettingsRoot,0x2fbc),
      THANDOR_UI_AT(displaySettingsRoot,0x2f54),
      THANDOR_UI_AT(displaySettingsRoot,0x2eec),
      THANDOR_UI_AT(displaySettingsRoot,0x2e84));
  persistedValue = PersistentSettings_ReadDword(1,0);
  if ((((persistedValue == adapterIndex) &&
       (persistedValue = PersistentSettings_ReadDword(0x280,4), persistedValue == pendingWidth)) &&
      (persistedValue = PersistentSettings_ReadDword(0x1e0,8), persistedValue == pendingHeight)) &&
     (persistedValue = PersistentSettings_ReadDword(0x10,0xc), persistedValue == bitsPerPixel)) {
    UiNodeList_SuppressActionId(0x2031,displaySettingsRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(0x2031,displaySettingsRoot);
  return;
}

