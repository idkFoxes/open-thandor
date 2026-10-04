/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/display_settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/display_settings.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: ui/frontend/display_settings. */

/* Inserts value into the ascending list candidates[0..candidateCount-1] (UI_DISPLAY_MODE_NONE marks empty
   slots) unless it is already listed; the largest entry falls off the end. */
static void FrontendDisplayModeCandidates_InsertSortedUnique
          (uint32_t *candidates,uint32_t candidateCount,uint32_t value)
{
  uint32_t candidateIndex;
  uint32_t displacedValue;

  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value == candidates[candidateIndex]) {
      return;
    }
  }
  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value < candidates[candidateIndex]) {
      displacedValue = candidates[candidateIndex];
      candidates[candidateIndex] = value;
      value = displacedValue;
    }
  }
}

/* Adapter row adapterIndex of the display settings page: driver description and device name (every adapter is
   a software renderer device, which gets its text resource name). */
static void FrontendDisplaySettingsPage_FillAdapterRow
          (FrontendDisplaySettingsPageOptionState *source,uint32_t adapterIndex)
{
  source->adapterRows.rows[adapterIndex].adapterDescriptionUtf16 =
       g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16;
  source->adapterRows.rows[adapterIndex].deviceNameUtf16 = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
}

/* Handler of action 0x2011 (slot 17 of g_FrontendUiActionHandlersPage20.handlers00_54), the options page's
   "Graphics" button: opens the display settings page and fills its choices: the four smallest distinct colour
   depths and the ten smallest distinct resolutions (width << 16 | height) of g_GraphicsDisplayModes, each
   collected by an insertion into a sorted list with 0xFFFFFFFF as the empty mark, and the name and device of
   up to five adapters. The saved adapter, resolution and colour depth become the current selection.
*/
void FrontendDisplaySettingsAction_OpenPageAndListModes(FrontendDisplaySettingsPageOptionState *source)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t *candidates;
  uint32_t adapterIndex;
  uint32_t rowIndex;
  GraphicsDisplayModeCount remainingModes;
  GraphicsDisplayMode *displayMode;

  candidates = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues;
  /* source is the frontend template's graphicsSettingsButton */
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,
             (UiPageStackControl *)FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),
                                               frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)
           FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),menuRoomModelView))->
         contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  /* the four smallest distinct colour depths */
  for (rowIndex = 0; rowIndex < 4; rowIndex++) {
    candidates[rowIndex] = UI_DISPLAY_MODE_NONE;
  }
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    FrontendDisplayModeCandidates_InsertSortedUnique(candidates,4,displayMode->bitsPerPixel);
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  for (rowIndex = 0; rowIndex < 4; rowIndex++) {
    source->colorDepthRows.rows[rowIndex].bitsPerPixel = candidates[rowIndex];
  }
  /* name and device of up to five adapters (the first one is always listed) */
  FrontendDisplaySettingsPage_FillAdapterRow(source,0);
  for (adapterIndex = 1; (adapterIndex < 5) && (adapterIndex < g_GraphicsAdapterCount); adapterIndex++) {
    FrontendDisplaySettingsPage_FillAdapterRow(source,adapterIndex);
  }
  /* the ten smallest distinct resolutions */
  for (rowIndex = 0; rowIndex < 10; rowIndex++) {
    candidates[rowIndex] = UI_DISPLAY_MODE_NONE;
  }
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    FrontendDisplayModeCandidates_InsertSortedUnique
              (candidates,10,displayMode->width * UI_DISPLAY_MODE_WIDTH_SCALE + displayMode->height);
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  for (rowIndex = 0; rowIndex < 10; rowIndex++) {
    source->resolutionRows.rows[rowIndex].width = candidates[rowIndex] >> 16;
    source->resolutionRows.rows[rowIndex].height = candidates[rowIndex] & UI_DISPLAY_MODE_HEIGHT_MASK;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  adapterIndex = PersistentSettings_Read(1,PERSISTENT_SETTING_ADAPTER_INDEX);
  /* Not in the original: a saved index past the adapter list (the default 1 with a single adapter, or a
     hardware renderer device saved by the original game) selects adapter 0, as ProcessEntry does at startup */
  if (g_GraphicsAdapterCount <=
      (uint32_t)g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex) {
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex = 0;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
  FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)source);
  return;
}

/* Handler of actions 0x202C..0x2030 (slots 44..48 of g_FrontendUiActionHandlersPage20.handlers00_54), the five
   adapter choices of the display settings page: selects the adapter whose button was pressed (identified by
   its offset in the parent container, 0x68 bytes apart) and refreshes which modes can be chosen.
*/
void FrontendDisplaySettingsAction_SelectAdapter(UiNodeBase *sourceNode)

{
  int controlOffsetFromParent;
  FrontendDisplayAdapterIndex adapterIndex;

  controlOffsetFromParent = (int)((uintptr_t)sourceNode - (uintptr_t)sourceNode->parent);
  /* any button other than options 1..4 selects adapter 4 */
  if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption1)) {
    adapterIndex = 0;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption2)) {
    adapterIndex = 1;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption3)) {
    adapterIndex = 2;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption4)) {
    adapterIndex = 3;
  }
  else {
    adapterIndex = 4;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex = adapterIndex;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(sourceNode->parent);
  return;
}
