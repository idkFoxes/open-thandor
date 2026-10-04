/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/display_settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/display_settings.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch = {0};

/* Not in the original: the pending display mode kind (PERSISTENT_DISPLAY_MODE_*) of the display settings page. */
static uint32_t s_pendingDisplayModeKind = PERSISTENT_DISPLAY_MODE_FULLSCREEN;

/* Not in the original: the pending adapter (renderer) and display mode kind as saved (SdlVideo_SavedAdapterIndex,
   SdlVideo_SavedDisplayModeKind; the renderer is not kept in the original's adapter index). */
static void FrontendDisplaySettingsPage_ReadSavedRendererAndKind(void)
{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex =
       SdlVideo_SavedAdapterIndex();
  s_pendingDisplayModeKind = SdlVideo_SavedDisplayModeKind();
}

/* Implementation ownership: ui/frontend/display_settings. */

/* Adapter row adapterIndex of the display settings page: driver description and device name (the SDL3 backend's
   adapters are its renderers: "Vulkan", "DirectX 12", "Software" with "GPU" / "CPU"). */
static void FrontendDisplaySettingsPage_FillAdapterRow
          (FrontendDisplaySettingsPageOptionState *source,uint32_t adapterIndex)
{
  source->adapterRows.rows[adapterIndex].adapterDescriptionUtf16 =
       g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16;
  /* not in the original: every adapter is a renderer, labelled "<renderer> (GPU|CPU)" */
  source->adapterRows.rows[adapterIndex].deviceNameUtf16 = SdlVideo_AdapterDetailUtf16(adapterIndex);
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
    UiDisplayModeCandidates_InsertSortedUnique(candidates,4,displayMode->bitsPerPixel);
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
    UiDisplayModeCandidates_InsertSortedUnique
              (candidates,10,displayMode->width * UI_DISPLAY_MODE_WIDTH_SCALE + displayMode->height);
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  for (rowIndex = 0; rowIndex < 10; rowIndex++) {
    source->resolutionRows.rows[rowIndex].width = candidates[rowIndex] >> 16;
    source->resolutionRows.rows[rowIndex].height = candidates[rowIndex] & UI_DISPLAY_MODE_HEIGHT_MASK;
  }
  /* Not in the original: the adapter is the saved renderer (the original read its adapter index here), and the
     display mode kind choice */
  FrontendDisplaySettingsPage_ReadSavedRendererAndKind();
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

/* Handler of the ten resolution choices of the display settings page (actions 0x2022..0x202B, slots 34-43 of
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

/* Handler of the four colour-depth choices of the display settings page (actions 0x201E..0x2021, slots 30-33 of
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

/* Handler of the display settings page's apply action (FRONTEND_ACTION_APPLY_DISPLAY_MODE, slot 49 of
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
  int currentColorBits;
  int remainingFonts;
  UiNodeBase *parentCursor;
  GraphicsTextureSourceAsset **fontTextureSource;
  uint32_t selectedModeError;
  uint32_t restoredModeError;
  uint32_t previousDisplayModeKind;

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
  /* not in the original: the display mode kind is applied by the same switch */
  previousDisplayModeKind = SdlVideo_DisplayModeKind();
  SdlVideo_SetDisplayModeKind(s_pendingDisplayModeKind);
  g_CursorVisibilityToken--;
  /* the current colour depth: the RGB bits of the pixel format, rounded up to a multiple of 16 below */
  currentColorBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
          g_SoftwarePixelFormatConfig.blueBitCount;
  if (!g_GraphicsSetDisplayMode
                    (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.bitsPerPixel,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,&selectedModeError)) {
    if (!g_GraphicsSetDisplayMode(previousAdapterIndex,currentColorBits + 15U & ~15U,previousHeight,
                                  previousWidth,&restoredModeError)) {
      FatalError_ExitIfFailed(restoredModeError,true);
    }
    g_CursorVisibilityToken++;
    FatalError_ReportIfFailed(selectedModeError,true);
    /* Not in the original: the saved renderer and display mode kind (see
       FrontendDisplaySettingsAction_OpenPageAndListModes); the previous kind is applied again */
    SdlVideo_SetDisplayModeKind(previousDisplayModeKind);
    FrontendDisplaySettingsPage_ReadSavedRendererAndKind();
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
         PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
         PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
    FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)control);
    return;
  }
  /* not in the original: the renderer and the display mode kind are saved in their own settings */
  SdlVideo_SaveAdapterIndex(g_ActiveGraphicsAdapterIndex);
  SdlVideo_SaveDisplayModeKind(s_pendingDisplayModeKind);
  (void)selectedAdapterIndex;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex =
       g_ActiveGraphicsAdapterIndex; /* a renderer that fell back shows the one that runs */
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
  for (remainingFonts = 2; remainingFonts != 0; remainingFonts--) { /* both fonts */
    g_GraphicsTextureSourceConvertPaletteEntries((GraphicsPaletteTextureSourceAsset *)*fontTextureSource);
    fontTextureSource++;
  }
  g_CursorVisibilityToken++;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)control);
  /* walk up the parent links to the frontend template root */
  parentCursor = ((UiNodeBase *)control)->parent;
  while (parentCursor != UI_NODE_NONE) {
    control = ((UiNodeBase *)control)->parent;
    parentCursor = ((UiNodeBase *)control)->parent;
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

/* Refreshes the display settings page after the pending mode changed (called by the colour-depth, resolution
   and apply handlers here and by ui/frontend/runtime). Every colour-depth, resolution and adapter choice is
   hidden unless the adapter offers it together with the other two pending values, the choices matching the
   pending mode are selected, and the apply button is only offered while the pending mode differs from the saved
   one.
   How the selection works in the original: it first pushes all 19 choice controls on the machine stack
   (adapter 1..5, resolution 1..10, colour depth 1..4, so colour depth 4 ends on top), then per group pushes
   every choice whose value equals the pending one (colour depth: bits per pixel == the button's firstValue;
   resolution: width == firstValue and height == secondValue; adapter: pending adapter == 0..4) and calls
   UiSelectableGroup_SelectExclusive(count, <top of stack>, <the count entries below it>), which pops count
   and the selected control, and then the caller pops count entries. With exactly one match per group this selects the matching choice. Quirk of the original:
   when a group has no match, the entry on top (without earlier shifts: that group's last choice) is taken as
   the selected control but is not in the list, so it keeps its state; the group's other choices plus the
   next group's first choice are deselected, and every later group works on a stack shifted by one entry
   (two matches in one group shift it the other way). modeStack models that stack exactly. Once a shift
   reaches past the 19 pushed controls the original also deselects and redraws whatever the values
   saved below them on its stack point at; this C leaves those entries out (listCount is cut at the last control).
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
  Bool8 modeCheckCarry;
  /* the original's stack: modeStack[modeStackTop] is the top; the 5 NULL entries after the 19 controls stand
     for the values the original saved below them */
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
  while (frontendRoot->parent != UI_NODE_NONE) {
    frontendRoot = frontendRoot->parent;
  }
  /* push all 19 choices */
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
  if (bitsPerPixel == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayColorDepthOption4))->firstValue) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayColorDepthOption4);
  }
  /* SelectExclusive(4, top, next 4), then pop 1 + 4 */
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
  if ((pendingWidth == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->firstValue) &&
     (pendingHeight == ((UiNumericPairTextButton *)FRONTEND_UI(frontendRoot,displayResolutionOption10))->secondValue)) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayResolutionOption10);
  }
  /* SelectExclusive(10, top, next 10), then pop 1 + 10 */
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
  if (adapterIndex == 4) {
    modeStack[--modeStackTop] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption5);
  }
  /* SelectExclusive(5, top, next 5), then pop 1 + 5 (the stack is dropped at return anyway) */
  listCount = DISPLAY_MODE_STACK_END - (modeStackTop + 1);
  if (listCount > 5) {
    listCount = 5;
  }
  UiSelectableGroup_SelectExclusive(listCount,modeStack[modeStackTop],
      modeStack[modeStackTop + 1],modeStack[modeStackTop + 2],modeStack[modeStackTop + 3],
      modeStack[modeStackTop + 4],modeStack[modeStackTop + 5]);
  /* not in the original: the display mode kind choices (one selected) */
  UiNodeList_SuppressActionId(FRONTEND_ACTION_DISPLAY_MODE_KIND_WINDOW,frontendRoot);
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_DISPLAY_MODE_KIND_WINDOW,frontendRoot);
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_DISPLAY_MODE_KIND_BORDERLESS,frontendRoot);
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_DISPLAY_MODE_KIND_FULLSCREEN,frontendRoot);
  UiSelectableGroup_SelectExclusive(3,
      (s_pendingDisplayModeKind == PERSISTENT_DISPLAY_MODE_WINDOW) ?
           (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindWindow) :
      (s_pendingDisplayModeKind == PERSISTENT_DISPLAY_MODE_FULLSCREEN) ?
           (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindFullscreen) :
           (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindBorderless),
      (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindFullscreen),
      (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindBorderless),
      (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindWindow));
  /* the original compared the saved adapter index; here the saved renderer and display mode kind */
  persistedValue = SdlVideo_SavedAdapterIndex();
  if ((((persistedValue == adapterIndex) && (SdlVideo_SavedDisplayModeKind() == s_pendingDisplayModeKind) &&
       (persistedValue = PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH), persistedValue == pendingWidth)) &&
      (persistedValue = PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT), persistedValue == pendingHeight)) &&
     (persistedValue = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL), persistedValue == bitsPerPixel)) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
  return;
}

/* Not in the original: handler of the display mode kind choices "Fenster", "Vollbildfenster" and "Vollbild"
   (FRONTEND_ACTION_DISPLAY_MODE_KIND_*, handlers58_5A of g_FrontendUiActionHandlersPage20): takes the kind as the
   pending one and refreshes the page; the apply action switches to it together with the pending mode. */
void FrontendDisplaySettingsAction_SelectDisplayModeKind(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = sourceNode;
  while (frontendRoot->parent != UI_NODE_NONE) {
    frontendRoot = frontendRoot->parent;
  }
  if (sourceNode == (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindWindow)) {
    s_pendingDisplayModeKind = PERSISTENT_DISPLAY_MODE_WINDOW;
  }
  else if (sourceNode == (UiNodeBase *)FRONTEND_UI(frontendRoot,displayModeKindFullscreen)) {
    s_pendingDisplayModeKind = PERSISTENT_DISPLAY_MODE_FULLSCREEN;
  }
  else {
    s_pendingDisplayModeKind = PERSISTENT_DISPLAY_MODE_BORDERLESS;
  }
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(sourceNode);
}
