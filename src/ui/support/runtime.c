/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/support/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/support/runtime.h>
#include <thandor/thandor.h>
#include <thandor/graphics/resources/pcx.h>
#include <string.h>

/* Implementation ownership: ui/support/runtime. */

/* Builds the list of chat messages to show (output, newest first, at most maxEntries) and ages the history:
   sorts the slots by serial, newest first, and lists them until an empty slot, an expired one (older than
   RECENT_TEXT_HISTORY_LIFETIME) or maxEntries is reached. In the last two cases the remaining slots are
   emptied, so messages beyond maxEntries are forgotten too. Finally the serial counter advances.
*/
void RecentTextHistory_SortAndBuildPointerList
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
    if (g_RecentTextEntrySerials[outputIndex] == 0) { /* the rest is empty already */
      g_RecentTextSerialCounter++;
      return;
    }
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
  g_RecentTextSerialCounter++;
  return;
}


/* Adds a chat message to the recent-text history: it replaces the oldest slot (lowest serial, an empty
   slot has serial 0; on ties the last one) and gets the current serial, the text being copied with its
   rich-text commands expanded, truncated to the slot's 256 bytes.
*/
void RecentTextHistory_Insert(uint16_t *text)

{
  uint32_t oldestSerial;
  int slotsRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;

  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = UINT32_MAX;
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
              (sizeof g_RecentTextSlotStorage[0].text,g_RecentTextSlotStorage[oldestIndex].text,text,NULL);
  }
}


/* Drops the oldest chat message from the recent-text history by emptying its slot (serial 0; on ties the last
   one); the text itself stays. Used when the message lines are clicked away (InGameRecentText_TrimHistoryToThree,
   FrontendRecentText_TrimAndSortTopFive).
*/
void RecentTextHistory_RemoveOldest(void)

{
  uint32_t oldestSerial;
  int entriesRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;

  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = UINT32_MAX;
  currentIndex = 0;
  oldestIndex = -1;
  for (entriesRemaining = RECENT_TEXT_HISTORY_SLOT_COUNT; entriesRemaining != 0; entriesRemaining--) {
    if (*serialCursor != 0 && *serialCursor <= oldestSerial) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor++;
    currentIndex++;
  }
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = 0;
  }
  return;
}


/* Opens the credits screen (FRONTEND_PAGE_ACTION_CREDITS): loads gfx\panel\credits.gfx and two work buffers of
   its width * height bytes for the mask effect, then switches the frontend view to the credits page and hides
   the cursor. On any failure the partial resources are released and the menu stays as it was.
*/
void CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView)

{
  uint32_t bufferBytes;
  GraphicsTextureSourceAsset *creditsTexture;
  void *blendedBufferPayload;
  GraphicsTextureLogicalSize textureSizeResult;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  (frontendCreditsView->creditsMaskRuntime).outgoingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).incomingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).tickCounter = 0;
  creditsTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_CreditsTexturePathUtf16,NULL);
  if (creditsTexture != NULL) {
    (frontendCreditsView->creditsMaskRuntime).textureSource = creditsTexture;
    textureSizeResult = g_GraphicsTextureSourceGetLogicalSize(0,creditsTexture);
    bufferBytes = textureSizeResult.logicalHeightPixels * textureSizeResult.logicalWidthPixels;
    if (g_MemoryApi.alloc(bufferBytes,(void **)&(frontendCreditsView->creditsMaskRuntime).maskPixels) == 0) {
      if (g_MemoryApi.alloc(bufferBytes,&blendedBufferPayload) == 0) {
        /* blendedSourcePixels is the second work buffer */
        (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = (uint32_t)blendedBufferPayload;
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
  g_MemoryApi.free((void *)(frontendCreditsView->creditsMaskRuntime).blendedSourcePixels);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}


/* Loads a 64x64 8-bit PCX picture named by sourcePath (a leaf name; path and wildcard characters are
   dropped, the extension becomes .pcx, the file is looked up next to the executable) into outputPreview:
   its 256-colour palette (3 bytes per colour in the order blue, green, red) followed by the 4096 pixel
   indices. CF set when the file is missing, cannot be decoded (see Pcx_DecodeIndexed8: only 8-bit paletted
   files) or has another size.
*/
bool PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath)

{
  uint16_t pathChar;
  void *sourceBytes;
  int colorIndex;
  uint32_t color;
  uint16_t *sanitizedPathCursor;
  PcxIndexedImage image;
  uint32_t sourceByteCount;

  /* copy the leaf, dropping every character that is not allowed in a file name, and '.' (each character is
     written first and the cursor only advances past kept ones) */
  sanitizedPathCursor = g_LevelEndingMovieSourcePath;
  for (pathChar = *sourcePath++; pathChar != 0; pathChar = *sourcePath++) {
    *sanitizedPathCursor = pathChar;
    if (pathChar != '*' && pathChar != '.' && pathChar != '?' && pathChar != '/' && pathChar != '\\' &&
        pathChar != '<' && pathChar != '>' && pathChar != '"' && pathChar != ':' && pathChar != '|') {
      sanitizedPathCursor++;
    }
  }
  *sanitizedPathCursor = 0;
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,g_LevelEndingMovieSourcePath,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_PCX,g_LevelResourcePathScratchUtf16);
  if (Resource_Load(g_LevelResourcePathScratchUtf16,&sourceBytes,&sourceByteCount,NULL)) {
    /* the original called pcx.fnc export 2 here and accepted only a paletted record (direct-colour 3-plane
       files were rejected as well) */
    if (Pcx_DecodeIndexed8((const uint8_t *)sourceBytes,sourceByteCount,&image)) {
      if (image.width == 64 && image.height == 64) {
        /* Original quirk: each colour dword 0xFFRRGGBB was stored whole with the output advancing by 3
           bytes, the 0xFF byte being overwritten by the next colour (the last one by the first pixels).
           What remains are the bytes blue, green, red, i.e. the PcxRgb24 fields named red/green/blue
           receive blue/green/red. */
        for (colorIndex = 0; colorIndex < PCX_PALETTE_COLOR_COUNT; colorIndex++) {
          color = image.paletteColors[colorIndex];
          outputPreview->palette[colorIndex].red = (uint8_t)color;
          outputPreview->palette[colorIndex].green = (uint8_t)(color >> 8);
          outputPreview->palette[colorIndex].blue = (uint8_t)(color >> 16);
        }
        memcpy(outputPreview->pixels,image.pixels,sizeof outputPreview->pixels);
        Pcx_FreeIndexed8(&image);
        Resource_Release(sourceBytes);
        return false;
      }
      Pcx_FreeIndexed8(&image);
    }
    Resource_Release(sourceBytes);
  }
  return true;
}


/* Swaps two entries of the recent-text history, for the sort in RecentTextHistory_SortAndBuildPointerList:
   their serials and their whole 256-byte text slots (in 32 steps of two dwords, swapped with XCHG as in the
   original: the history is rebuilt on the main thread and on the timer thread, FrontendSession_PeriodicTick).
*/
void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex)

{
  uint32_t secondSerial;
  uint32_t firstLowDword;
  uint32_t firstHighDword;
  uint32_t secondHighDword;
  int dwordPairsRemaining;
  uint32_t *firstSlotDwords;
  uint32_t *secondSlotDwords;

  secondSerial = g_RecentTextEntrySerials[secondIndex];
  g_RecentTextEntrySerials[secondIndex] = g_RecentTextEntrySerials[firstIndex];
  g_RecentTextEntrySerials[firstIndex] = secondSerial;
  secondSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + secondIndex);
  firstSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + firstIndex);
  for (dwordPairsRemaining = 32; dwordPairsRemaining != 0; dwordPairsRemaining--) {
    secondHighDword = secondSlotDwords[1];
    firstLowDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords,*secondSlotDwords);
    firstHighDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords + 1,secondHighDword);
    secondSlotDwords[0] = firstLowDword;
    secondSlotDwords[1] = firstHighDword;
    firstSlotDwords += 2;
    secondSlotDwords += 2;
  }
}

