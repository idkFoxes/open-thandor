/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/support/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/support/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/support/runtime. */

/* Address: 0x0050F220.
   Builds the list of chat messages to show (output, newest first, at most maxEntries) and ages the history:
   sorts the slots by serial, newest first, and lists them until an empty slot, an expired one (older than
   RECENT_TEXT_HISTORY_LIFETIME) or maxEntries is reached. In the last two cases the remaining slots are
   emptied, so messages beyond maxEntries are forgotten too. Finally the serial counter advances.
*/
void __thandor_void_preserve_eax_ecx_edx
RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output)

{
  uint32_t currentSerial;
  RecentTextHistorySlot *slotCursor;
  UiListRowIndex firstIndex;
  UiListRowIndex secondIndex;
  uint32_t outputIndex;
  int minimumRetainedSerial;
  
  /* selection sort, newest (highest serial) first */
  secondIndex = 0;
  do {
    firstIndex = secondIndex + 1;
    currentSerial = g_RecentTextEntrySerials[secondIndex];
    do {
      if (currentSerial < g_RecentTextEntrySerials[firstIndex]) {
        RecentTextHistory_SwapSlots(firstIndex,secondIndex);
        currentSerial = g_RecentTextEntrySerials[secondIndex];
      }
      slotCursor = g_RecentTextSlotStorage;
      firstIndex++;
    } while (firstIndex < RECENT_TEXT_HISTORY_SLOT_COUNT);
    secondIndex++;
  } while (secondIndex < RECENT_TEXT_HISTORY_SLOT_COUNT - 1);
  outputIndex = 0;
  minimumRetainedSerial = g_RecentTextSerialCounter - RECENT_TEXT_HISTORY_LIFETIME;
  output->count = 0;
  do {
    if (g_RecentTextEntrySerials[outputIndex] == 0)
    goto RecentTextHistory_SortAndBuildPointerList_AdvanceSerialAfterBuildOrEmptyStop;
    if ((int)g_RecentTextEntrySerials[outputIndex] < minimumRetainedSerial) break;
    output->entries[outputIndex] = slotCursor;
    output->count++;
    outputIndex++;
    slotCursor++;
    maxEntries--;
  } while (maxEntries != 0);
  for (; outputIndex < RECENT_TEXT_HISTORY_SLOT_COUNT; outputIndex++) {
    g_RecentTextEntrySerials[outputIndex] = 0;
  }
RecentTextHistory_SortAndBuildPointerList_AdvanceSerialAfterBuildOrEmptyStop:
  g_RecentTextSerialCounter++;
  return;
}


/* Address: 0x0050F130.
   Ownership: ui/support/runtime.
   Purpose: Finds the slot with the lowest serial, assigns the current serial counter, and copies up to 0x100 bytes
   of UTF-16 text into that slot.
   Cross-module calls: RichTextCommandStream_CopyExpanded [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx RecentTextHistory_Insert(uint16_t *text)

{
  uint32_t oldestSerial;
  int slotsRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;
  
  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = 0xffffffff;
  slotsRemaining = 8;
  currentIndex = 0;
  oldestIndex = -1;
  do {
    if (*serialCursor <= oldestSerial) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor = serialCursor + 1;
    currentIndex = currentIndex + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = g_RecentTextSerialCounter;
    RichTextCommandStream_CopyExpanded(0x100,g_RecentTextSlotStorage[oldestIndex].text,text);
  }
  return;
}


/* Address: 0x0050F2E0.
   Ownership: ui/support/runtime.
   Purpose: Finds the smallest nonzero recent-text serial and clears that slot's serial, leaving the text storage
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx RecentTextHistory_RemoveOldest(void)

{
  uint32_t oldestSerial;
  int entriesRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;
  
  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = 0xffffffff;
  currentIndex = 0;
  oldestIndex = -1;
  entriesRemaining = 8;
  do {
    if ((*serialCursor != 0) && (*serialCursor <= oldestSerial)) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor = serialCursor + 1;
    currentIndex = currentIndex + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = 0;
  }
  return;
}


/* Address: 0x00548EC0.
   Opens the credits screen (FRONTEND_PAGE_ACTION_CREDITS): loads gfx\panel\credits.gfx and two work buffers of
   its width * height bytes for the mask effect, then switches the frontend view to the credits page and hides
   the cursor. On any failure the partial resources are released and the menu stays as it was.
*/
void __thandor_void_preserve_eax_ecx_edx
CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView)

{
  uint32_t bufferBytes;
  TextureSourceLoadResult textureLoadResult;
  ArenaAllocResult bufferAllocResult;
  TextureSizeResult textureSizeResult;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).unresolved64 = 0;
  (frontendCreditsView->creditsMaskRuntime).patternState54 = 0;
  (frontendCreditsView->creditsMaskRuntime).patternState58 = 0;
  (frontendCreditsView->creditsMaskRuntime).tickCounter = 0;
  textureLoadResult = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_credits_gfx_00545c22);
  if (!textureLoadResult.failed) {
    (frontendCreditsView->creditsMaskRuntime).textureSource = textureLoadResult.textureSource;
    textureSizeResult = g_GraphicsTextureSourceGetLogicalSize(0,textureLoadResult.textureSource);
    bufferBytes = textureSizeResult.logicalHeightPixels * textureSizeResult.logicalWidthPixels;
    bufferAllocResult = g_MemoryApi.alloc(bufferBytes);
    if (!bufferAllocResult.failed) {
      (frontendCreditsView->creditsMaskRuntime).maskPixels = (uint8_t *)bufferAllocResult.payloadOrError;
      bufferAllocResult = g_MemoryApi.alloc(bufferBytes);
      if (!bufferAllocResult.failed) {
        /* unresolved64 is the second work buffer */
        (frontendCreditsView->creditsMaskRuntime).unresolved64 = bufferAllocResult.payloadOrError;
        UiFrame_FlushInputAndResetPendingTicks();
        SoftwareMaskBuffer_Clear(&frontendCreditsView->creditsMaskRuntime);
        /* page 1 of the frontend view-mode stack: the full-screen view instead of the menu room */
        UiPageStack_SetActiveIndex(1,&frontendCreditsView->pageStack);
        g_CursorVisibilityToken--;
        return;
      }
    }
  }
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage
            ((frontendCreditsView->creditsMaskRuntime).textureSource);
  g_MemoryApi.free((frontendCreditsView->creditsMaskRuntime).maskPixels);
  g_MemoryApi.free((void *)(frontendCreditsView->creditsMaskRuntime).unresolved64);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).unresolved64 = 0;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}


/* Address: 0x0054D5D0.
   Ownership: ui/support/runtime.
   Purpose: Sanitizes a UTF-16 source leaf, loads and decodes its PCX resource, accepts only a 64 by 64 image, and
   copies 256 RGB palette entries plus 0x400 pixel dwords. CF clear means success.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode
   [core/text/path], Resource_Load [assets/resource/runtime], Resource_Release [assets/resource/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath)

{
  uint16_t pathChar;
  int headerOrPixelDataOffset;
  void *sourceBytes;
  void *memory;
  int dwordsRemaining;
  uint32_t *pcxDwordReadCursor;
  uint32_t *pixelDwordCursor;
  uint16_t *sanitizedPathCursor;
  PcxDecodeResult pcxDecodeResult;
  ResourceLoadResult resourceLoadResult;
  
  sanitizedPathCursor = g_LevelEndingMovieSourcePath;
  while( true ) {
    pathChar = *sourcePath;
    *sanitizedPathCursor = pathChar;
    sourcePath = sourcePath + 1;
    if (pathChar == 0) break;
    if ((((pathChar != 0x2a) && (pathChar != 0x2e)) &&
        ((pathChar != 0x3f && ((pathChar != 0x2f && (pathChar != 0x5c)))))) &&
       ((pathChar != 0x3c &&
        ((((pathChar != 0x3e && (pathChar != 0x22)) && (pathChar != 0x3a)) && (pathChar != 0x7c)))))) {
      sanitizedPathCursor = sanitizedPathCursor + 1;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_LevelResourcePathScratchUtf16,g_LevelEndingMovieSourcePath,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_SetExtensionCode(0x786370,(uint16_t *)&g_LevelResourcePathScratchUtf16);
  resourceLoadResult = Resource_Load((uint16_t *)&g_LevelResourcePathScratchUtf16);
  sourceBytes = (void *)resourceLoadResult.bufferOrError;
  if (!resourceLoadResult.failed) {
    pcxDecodeResult = g_PcxFunctionExport2(g_PcxFunctionModule,resourceLoadResult.byteCount,sourceBytes);
    memory = pcxDecodeResult.decodedImageOrError;
    if (!pcxDecodeResult.failed) {
      headerOrPixelDataOffset = *(int *)((int)memory + 0xb8);
      if (((*(int *)((int)memory + headerOrPixelDataOffset + 8) == 0) &&
          (*(int *)((int)memory + headerOrPixelDataOffset + 0x18) == 0x40)) &&
         (*(int *)((int)memory + headerOrPixelDataOffset + 0x1c) == 0x40)) {
        pcxDwordReadCursor = (uint32_t *)((int)memory + 0x200);
        dwordsRemaining = 0x100;
        headerOrPixelDataOffset = *(int *)((int)memory + headerOrPixelDataOffset + 0xc);
        do {
          *(uint32_t *)outputPreview->paletteRgbTriplets256 = *pcxDwordReadCursor;
          pcxDwordReadCursor = pcxDwordReadCursor + 2;
          outputPreview = (PcxPreview64 *)(outputPreview->paletteRgbTriplets256 + 1);
          dwordsRemaining = dwordsRemaining + -1;
        } while (dwordsRemaining != 0);
        pixelDwordCursor = (uint32_t *)((int)memory + headerOrPixelDataOffset);
        for (dwordsRemaining = 0x400; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
          *(uint32_t *)outputPreview->paletteRgbTriplets256 = *pixelDwordCursor;
          pixelDwordCursor = pixelDwordCursor + 1;
          outputPreview = (PcxPreview64 *)&outputPreview->paletteRgbTriplets256[1].green;
        }
        g_MemoryApi.free(memory);
        Resource_Release(sourceBytes);
        return false;
      }
      g_MemoryApi.free(memory);
    }
    Resource_Release(sourceBytes);
  }
  return true;
}


/* Address: 0x0050F1A0.
   Ownership: ui/support/runtime.
   Purpose: Swaps two serial values and their complete 0x100-byte recent-text slots using thirty-two 8-byte
   exchanges. Typed parameters: p0 firstIndex→UiListRowIndex_V300, p1 secondIndex→UiListRowIndex_V300. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex)

{
  uint32_t firstHighDword;
  uint32_t serialOrFirstLowDword;
  uint32_t secondHighDword;
  int dwordPairsRemaining;
  uint32_t *firstSlotDwords;
  uint32_t *secondSlotDwords;
  
  serialOrFirstLowDword = g_RecentTextEntrySerials[secondIndex];
  g_RecentTextEntrySerials[secondIndex] = g_RecentTextEntrySerials[firstIndex];
  g_RecentTextEntrySerials[firstIndex] = serialOrFirstLowDword;
  secondSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + secondIndex);
  firstSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + firstIndex);
  dwordPairsRemaining = 0x20;
  do {
    secondHighDword = secondSlotDwords[1];
    LOCK();
    serialOrFirstLowDword = *firstSlotDwords;
    *firstSlotDwords = *secondSlotDwords;
    UNLOCK();
    LOCK();
    firstHighDword = firstSlotDwords[1];
    firstSlotDwords[1] = secondHighDword;
    UNLOCK();
    *secondSlotDwords = serialOrFirstLowDword;
    secondSlotDwords[1] = firstHighDword;
    firstSlotDwords = firstSlotDwords + 2;
    secondSlotDwords = secondSlotDwords + 2;
    dwordPairsRemaining = dwordPairsRemaining + -1;
  } while (dwordPairsRemaining != 0);
  return;
}

