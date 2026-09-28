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
   Adds a chat message to the recent-text history: it replaces the oldest slot (lowest serial, an empty
   slot has serial 0; on ties the last one) and gets the current serial, the text being copied with its
   rich-text commands expanded, truncated to the slot's 256 bytes.
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
  slotsRemaining = RECENT_TEXT_HISTORY_SLOT_COUNT;
  currentIndex = 0;
  oldestIndex = -1;
  do {
    if (*serialCursor <= oldestSerial) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor++;
    currentIndex++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = g_RecentTextSerialCounter;
    RichTextCommandStream_CopyExpanded
              (sizeof g_RecentTextSlotStorage[0].text,g_RecentTextSlotStorage[oldestIndex].text,text);
  }
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
   Loads a 64x64 8-bit PCX picture named by sourcePath (a leaf name; path and wildcard characters are
   dropped, the extension becomes .pcx, the file is looked up next to the executable) into outputPreview:
   its 256-colour RGB palette followed by the 4096 pixel indices. CF set when the file is missing, cannot
   be decoded or has another size.
*/
bool __thandor_cf_preserve_eax_ecx_edx
PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath)

{
  uint16_t pathChar;
  int headerOrPixelDataOffset;
  void *sourceBytes;
  void *decodedImage;
  int dwordsRemaining;
  uint32_t *paletteEntryCursor;
  uint32_t *pixelDwordCursor;
  uint16_t *sanitizedPathCursor;
  PcxDecodeResult pcxDecodeResult;
  ResourceLoadResult resourceLoadResult;

  /* copy the leaf, dropping every character that is not allowed in a file name, and '.' */
  sanitizedPathCursor = g_LevelEndingMovieSourcePath;
  while( true ) {
    pathChar = *sourcePath;
    *sanitizedPathCursor = pathChar;
    sourcePath++;
    if (pathChar == 0) break;
    if ((((pathChar != '*') && (pathChar != '.')) &&
        ((pathChar != '?' && ((pathChar != '/' && (pathChar != '\\')))))) &&
       ((pathChar != '<' &&
        ((((pathChar != '>' && (pathChar != '"')) && (pathChar != ':')) && (pathChar != '|')))))) {
      sanitizedPathCursor++;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_LevelResourcePathScratchUtf16,g_LevelEndingMovieSourcePath,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_SetExtensionCode(0x786370,(uint16_t *)&g_LevelResourcePathScratchUtf16); /* ".pcx" */
  resourceLoadResult = Resource_Load((uint16_t *)&g_LevelResourcePathScratchUtf16);
  sourceBytes = (void *)resourceLoadResult.bufferOrError;
  if (!resourceLoadResult.failed) {
    pcxDecodeResult = g_PcxFunctionExport2(g_PcxFunctionModule,resourceLoadResult.byteCount,sourceBytes);
    decodedImage = pcxDecodeResult.decodedImageOrError;
    if (!pcxDecodeResult.failed) {
      /* the decoder's image record: +0xB8 offset of the image header, which holds +0x08 (must be 0),
         +0x0C the offset of the pixels, +0x18 width and +0x1C height; the palette is at +0x200 with
         one 8-byte entry per colour */
      headerOrPixelDataOffset = *(int *)((int)decodedImage + 0xb8);
      if (((*(int *)((int)decodedImage + headerOrPixelDataOffset + 8) == 0) &&
          (*(int *)((int)decodedImage + headerOrPixelDataOffset + 0x18) == 64)) &&
         (*(int *)((int)decodedImage + headerOrPixelDataOffset + 0x1c) == 64)) {
        paletteEntryCursor = (uint32_t *)((int)decodedImage + 0x200);
        dwordsRemaining = 256;
        headerOrPixelDataOffset = *(int *)((int)decodedImage + headerOrPixelDataOffset + 0xc);
        /* each colour is stored as a whole dword and the output advances by 3 bytes: the fourth byte is
           overwritten by the next colour (the last one by the first pixel dword) */
        do {
          *(uint32_t *)outputPreview->paletteRgbTriplets256 = *paletteEntryCursor;
          paletteEntryCursor = paletteEntryCursor + 2;
          outputPreview = (PcxPreview64 *)(outputPreview->paletteRgbTriplets256 + 1);
          dwordsRemaining--;
        } while (dwordsRemaining != 0);
        /* 64 * 64 pixel bytes as 1024 dwords; the output cursor steps 4 bytes (triplet + 1) */
        pixelDwordCursor = (uint32_t *)((int)decodedImage + headerOrPixelDataOffset);
        for (dwordsRemaining = 1024; dwordsRemaining != 0; dwordsRemaining--) {
          *(uint32_t *)outputPreview->paletteRgbTriplets256 = *pixelDwordCursor;
          pixelDwordCursor++;
          outputPreview = (PcxPreview64 *)&outputPreview->paletteRgbTriplets256[1].green;
        }
        g_MemoryApi.free(decodedImage);
        Resource_Release(sourceBytes);
        return false;
      }
      g_MemoryApi.free(decodedImage);
    }
    Resource_Release(sourceBytes);
  }
  return true;
}


/* Address: 0x0050F1A0.
   Swaps two entries of the recent-text history, for the sort in RecentTextHistory_SortAndBuildPointerList:
   their serials and their whole 256-byte text slots (in 32 steps of two dwords, swapped with XCHG as in the
   original: the history is rebuilt on the main thread and on the timer thread, FrontendSession_PeriodicTick).
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
  dwordPairsRemaining = 32;
  do {
    secondHighDword = secondSlotDwords[1];
    serialOrFirstLowDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords,*secondSlotDwords);
    firstHighDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords + 1,secondHighDword);
    *secondSlotDwords = serialOrFirstLowDword;
    secondSlotDwords[1] = firstHighDword;
    firstSlotDwords = firstSlotDwords + 2;
    secondSlotDwords = secondSlotDwords + 2;
    dwordPairsRemaining--;
  } while (dwordPairsRemaining != 0);
}

