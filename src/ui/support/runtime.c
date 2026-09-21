/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/support/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/support/runtime.h>

/* Implementation ownership: ui/support/runtime. */

/* Address: 0x0050F220.
   Ownership: ui/support/runtime.
   Purpose: Sorts all eight slots by descending serial, writes up to maxEntries slot pointers, clears stale
   trailing serials, and increments the serial counter. Typed parameters: p0
   maxEntries→RecentTextHistoryEntryLimit_V343. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RecentTextHistory_SwapSlots.
*/
void __thandor_void_preserve_eax_ecx_edx
RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output)

{
  dword currentSerial;
  RecentTextHistorySlot *slotCursor;
  UiListRowIndex firstIndex;
  UiListRowIndex secondIndex;
  uint outputIndex;
  int minimumRetainedSerial;
  
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
      firstIndex = firstIndex + 1;
    } while (firstIndex < 8);
    secondIndex = secondIndex + 1;
  } while (secondIndex < 7);
  outputIndex = 0;
  minimumRetainedSerial = g_RecentTextSerialCounter - 0x100;
  output->count = 0;
  do {
    if (g_RecentTextEntrySerials[outputIndex] == 0)
    goto RecentTextHistory_SortAndBuildPointerList_AdvanceSerialAfterBuildOrEmptyStop;
    if ((int)g_RecentTextEntrySerials[outputIndex] < minimumRetainedSerial) break;
    output->entries[outputIndex] = slotCursor;
    output->count = output->count + 1;
    outputIndex = outputIndex + 1;
    slotCursor = slotCursor + 1;
    maxEntries = maxEntries - 1;
  } while (maxEntries != 0);
  for (; outputIndex < 8; outputIndex = outputIndex + 1) {
    g_RecentTextEntrySerials[outputIndex] = 0;
  }
RecentTextHistory_SortAndBuildPointerList_AdvanceSerialAfterBuildOrEmptyStop:
  g_RecentTextSerialCounter = g_RecentTextSerialCounter + 1;
  return;
}


/* Address: 0x0050F130.
   Ownership: ui/support/runtime.
   Purpose: Finds the slot with the lowest serial, assigns the current serial counter, and copies up to 0x100 bytes
   of UTF-16 text into that slot.
   Cross-module calls: RichTextCommandStream_CopyExpandedCf [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx RecentTextHistory_Insert(word *text)

{
  uint oldestSerial;
  int slotsRemaining;
  int currentIndex;
  int oldestIndex;
  dword *serialCursor;
  
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
    RichTextCommandStream_CopyExpandedCf(0x100,g_RecentTextSlotStorage[oldestIndex].text,text);
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
  uint oldestSerial;
  int entriesRemaining;
  int currentIndex;
  int oldestIndex;
  dword *serialCursor;
  
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
   Ownership: ui/support/runtime.
   Purpose: Switches to the busy cursor, loads gfx\panel\credits.gfx, allocates two width*height work buffers,
   initializes the credits runtime, activates its UI page, and hides the cursor. Any failure releases partial
   resources, clears the three pointers, and restores cursor frame zero.
   Cross-module calls: UiFrame_FlushInputAndResetPendingTicks [ui/controls/layout], SoftwareMaskBuffer_Clear
   [graphics/backend/software], UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView)

{
  dword bytes;
  GraphicsTextureSourceLoadEaxCf5 GVar1;
  ArenaAllocEaxCf5 AVar2;
  GraphicsTextureSizeEaxEdxCf9 GVar3;
  
  (*g_GraphicsCursorSetFrame)(6);
  (frontendCreditsView->creditsMaskRuntime).textureSource = (GraphicsTextureSourceAsset *)0x0;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = (byte *)0x0;
  (frontendCreditsView->creditsMaskRuntime).unresolved64 = 0;
  (frontendCreditsView->creditsMaskRuntime).patternState54 = 0;
  (frontendCreditsView->creditsMaskRuntime).patternState58 = 0;
  (frontendCreditsView->creditsMaskRuntime).tickCounter = 0;
  GVar1 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_credits_gfx_00545c22);
  if (!GVar1.carry) {
    (frontendCreditsView->creditsMaskRuntime).textureSource = GVar1.eax;
    GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0,GVar1.eax);
    bytes = GVar3.logicalHeightPixels * GVar3.logicalWidthPixels;
    AVar2 = (*g_MemoryApi.alloc)(bytes);
    if (!AVar2.carry) {
      (frontendCreditsView->creditsMaskRuntime).maskPixels = (byte *)AVar2.eax;
      AVar2 = (*g_MemoryApi.alloc)(bytes);
      if (!AVar2.carry) {
        (frontendCreditsView->creditsMaskRuntime).unresolved64 = AVar2.eax;
        UiFrame_FlushInputAndResetPendingTicks();
        SoftwareMaskBuffer_Clear(&frontendCreditsView->creditsMaskRuntime);
        UiPageStack_SetActiveIndex(1,&frontendCreditsView->pageStack);
        g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
        return;
      }
    }
  }
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)
            ((frontendCreditsView->creditsMaskRuntime).textureSource);
  (*g_MemoryApi.free)((frontendCreditsView->creditsMaskRuntime).maskPixels);
  (*g_MemoryApi.free)((void *)(frontendCreditsView->creditsMaskRuntime).unresolved64);
  (frontendCreditsView->creditsMaskRuntime).textureSource = (GraphicsTextureSourceAsset *)0x0;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = (byte *)0x0;
  (frontendCreditsView->creditsMaskRuntime).unresolved64 = 0;
  (*g_GraphicsCursorSetFrame)(0);
  return;
}


/* Address: 0x0054D5D0.
   Ownership: ui/support/runtime.
   Purpose: Sanitizes a UTF-16 source leaf, loads and decodes its PCX resource, accepts only a 64 by 64 image, and
   copies 256 RGB palette entries plus 0x400 pixel dwords. CF clear means success.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode
   [core/text/path], Resource_Load [assets/resource/runtime], Resource_Release [assets/resource/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
PcxPreview_Load64x64PaletteAndPixelsCf(PcxPreview64 *outputPreview,word *sourcePath)

{
  word wVar1;
  int iVar2;
  void *sourceBytes;
  void *memory;
  int iVar3;
  dword *pcxDwordReadCursor;
  undefined4 *puVar4;
  word *pwVar5;
  PcxDecodeEaxCf5 PVar6;
  ResourceLoadEaxEcxCf9 RVar7;
  
  pwVar5 = g_LevelEndingMovieSourcePath;
  while( true ) {
    wVar1 = *sourcePath;
    *pwVar5 = wVar1;
    sourcePath = sourcePath + 1;
    if (wVar1 == 0) break;
    if ((((wVar1 != 0x2a) && (wVar1 != 0x2e)) &&
        ((wVar1 != 0x3f && ((wVar1 != 0x2f && (wVar1 != 0x5c)))))) &&
       ((wVar1 != 0x3c &&
        ((((wVar1 != 0x3e && (wVar1 != 0x22)) && (wVar1 != 0x3a)) && (wVar1 != 0x7c)))))) {
      pwVar5 = pwVar5 + 1;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_LevelResourcePathScratchUtf16,g_LevelEndingMovieSourcePath,
             (word *)&g_ExecutableDirectoryUtf16);
  WidePath_SetExtensionCode(0x786370,(word *)&g_LevelResourcePathScratchUtf16);
  RVar7 = Resource_Load((word *)&g_LevelResourcePathScratchUtf16);
  sourceBytes = (void *)RVar7.eax;
  if (!RVar7.carry) {
    PVar6 = (*g_PcxFunctionExport2)(g_PcxFunctionModule,RVar7.ecx,sourceBytes);
    memory = PVar6.decodedImageOrError;
    if (!PVar6.carry) {
      iVar2 = *(int *)((int)memory + 0xb8);
      if (((*(int *)((int)memory + iVar2 + 8) == 0) &&
          (*(int *)((int)memory + iVar2 + 0x18) == 0x40)) &&
         (*(int *)((int)memory + iVar2 + 0x1c) == 0x40)) {
        pcxDwordReadCursor = (dword *)((int)memory + 0x200);
        iVar3 = 0x100;
        iVar2 = *(int *)((int)memory + iVar2 + 0xc);
        do {
          *(dword *)outputPreview->paletteRgbTriplets256 = *pcxDwordReadCursor;
          pcxDwordReadCursor = pcxDwordReadCursor + 2;
          outputPreview = (PcxPreview64 *)(outputPreview->paletteRgbTriplets256 + 1);
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        puVar4 = (undefined4 *)((int)memory + iVar2);
        for (iVar3 = 0x400; iVar3 != 0; iVar3 = iVar3 + -1) {
          *(undefined4 *)outputPreview->paletteRgbTriplets256 = *puVar4;
          puVar4 = puVar4 + 1;
          outputPreview = (PcxPreview64 *)&outputPreview->paletteRgbTriplets256[1].green;
        }
        (*g_MemoryApi.free)(memory);
        Resource_Release(sourceBytes);
        return false;
      }
      (*g_MemoryApi.free)(memory);
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
  dword dVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  dword *firstSlotDwords;
  dword *secondSlotDwords;
  
  dVar2 = g_RecentTextEntrySerials[secondIndex];
  g_RecentTextEntrySerials[secondIndex] = g_RecentTextEntrySerials[firstIndex];
  g_RecentTextEntrySerials[firstIndex] = dVar2;
  secondSlotDwords = (dword *)(g_RecentTextSlotStorage + secondIndex);
  firstSlotDwords = (dword *)(g_RecentTextSlotStorage + firstIndex);
  iVar4 = 0x20;
  do {
    dVar3 = secondSlotDwords[1];
    LOCK();
    dVar2 = *firstSlotDwords;
    *firstSlotDwords = *secondSlotDwords;
    UNLOCK();
    LOCK();
    dVar1 = firstSlotDwords[1];
    firstSlotDwords[1] = dVar3;
    UNLOCK();
    *secondSlotDwords = dVar2;
    secondSlotDwords[1] = dVar1;
    firstSlotDwords = firstSlotDwords + 2;
    secondSlotDwords = secondSlotDwords + 2;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}

