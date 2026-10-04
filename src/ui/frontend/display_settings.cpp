/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/display_settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/display_settings.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

#include <algorithm>
#include <cstddef>
#include <cstring>

/* Module data. */

FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch = {};

/* Not in the original: the pending display mode kind (PERSISTENT_DISPLAY_MODE_*) of the display settings page. */
static uint32_t s_pendingDisplayModeKind = PERSISTENT_DISPLAY_MODE_FULLSCREEN;

/* Not in the original: the pending adapter (renderer) and display mode kind as saved (SdlVideo_SavedAdapterIndex,
   SdlVideo_SavedDisplayModeKind; the renderer is not kept in the original's adapter index). */
static void FrontendDisplaySettingsPage_ReadSavedRendererAndKind()
{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex =
       SdlVideo_SavedAdapterIndex();
  s_pendingDisplayModeKind = SdlVideo_SavedDisplayModeKind();
}

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

/* Not in the original: the resolution list of the display settings page. The original listed the ten smallest
   distinct resolutions in ten fixed radio rows (displayResolutionOption1..10); here every distinct resolution
   of the mode table is listed, in a scroll frame (displayResolutionScrollBox) inside the "Aufloesung" box whose
   content (displayResolutionRowPanel) holds the same radio rows: displayResolutionOption1..10, then as many
   displayResolutionExtraOptions as needed, each a copy of displayResolutionOption1 one row lower. */

/* the number of resolution rows listed (the distinct resolutions of the mode table) */
static uint32_t s_resolutionRowCount = 0;

static const std::size_t kTemplateResolutionRowOffsets[FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS] = {
    offsetof(FrontendUiImage,displayResolutionOption1),offsetof(FrontendUiImage,displayResolutionOption2),
    offsetof(FrontendUiImage,displayResolutionOption3),offsetof(FrontendUiImage,displayResolutionOption4),
    offsetof(FrontendUiImage,displayResolutionOption5),offsetof(FrontendUiImage,displayResolutionOption6),
    offsetof(FrontendUiImage,displayResolutionOption7),offsetof(FrontendUiImage,displayResolutionOption8),
    offsetof(FrontendUiImage,displayResolutionOption9),offsetof(FrontendUiImage,displayResolutionOption10)};

/* Resolution row rowIndex (0 .. FRONTEND_DISPLAY_RESOLUTION_OPTIONS - 1) of the frontend root. */
static UiNumericPairTextButton *FrontendDisplaySettingsPage_ResolutionRow(UiNodeBase *frontendRoot,uint32_t rowIndex)
{
  if (rowIndex < FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS) {
    return (UiNumericPairTextButton *)((uint8_t *)frontendRoot + kTemplateResolutionRowOffsets[rowIndex]);
  }
  return &(*FRONTEND_UI(frontendRoot,displayResolutionExtraOptions))
              [rowIndex - FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS];
}

/* Fills the resolution rows: the distinct resolutions of g_GraphicsDisplayModes (all adapters, 32 bits per
   pixel), ascending by width, then height; with more than FRONTEND_DISPLAY_RESOLUTION_OPTIONS the smallest ones
   are left out. The rows used are linked as the panel's children in this order and the panel gets their height.
   An extra row is made from displayResolutionOption1 the first time it is needed (the mode table does not change
   while the game runs) and takes action FRONTEND_ACTION_RESOLUTION_OPTION1. */
static void FrontendDisplaySettingsPage_BuildResolutionRows(UiNodeBase *frontendRoot)
{
  uint32_t resolutions[GRAPHICS_DISPLAY_MODE_CAPACITY];
  uint32_t resolutionCount;
  uint32_t firstListed;
  uint32_t rowIndex;
  uint32_t modeIndex;
  UiNumericPairTextButton *row;
  UiNumericPairTextButton *templateRow;
  UiNodeBase *rowPanel;
  UiNodeBase *previousRow;
  int32_t rowTop;

  resolutionCount = 0;
  for (modeIndex = 0; (modeIndex < g_GraphicsDisplayModeCount) && (modeIndex < GRAPHICS_DISPLAY_MODE_CAPACITY);
       modeIndex++) {
    if (g_GraphicsDisplayModes[modeIndex].bitsPerPixel == PERSISTENT_DEFAULT_BITS_PER_PIXEL) {
      resolutions[resolutionCount++] = g_GraphicsDisplayModes[modeIndex].width * UI_DISPLAY_MODE_WIDTH_SCALE +
                                       g_GraphicsDisplayModes[modeIndex].height;
    }
  }
  std::sort(resolutions,resolutions + resolutionCount);
  resolutionCount = (uint32_t)(std::unique(resolutions,resolutions + resolutionCount) - resolutions);
  firstListed = 0;
  if (resolutionCount > FRONTEND_DISPLAY_RESOLUTION_OPTIONS) {
    firstListed = resolutionCount - FRONTEND_DISPLAY_RESOLUTION_OPTIONS;
    resolutionCount = FRONTEND_DISPLAY_RESOLUTION_OPTIONS;
  }
  rowPanel = FRONTEND_UI(frontendRoot,displayResolutionRowPanel);
  templateRow = FrontendDisplaySettingsPage_ResolutionRow(frontendRoot,0);
  previousRow = UI_NODE_NONE;
  for (rowIndex = 0; rowIndex < resolutionCount; rowIndex++) {
    row = FrontendDisplaySettingsPage_ResolutionRow(frontendRoot,rowIndex);
    if ((rowIndex >= FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS) &&
        (row->base.selectable.base.vtable == nullptr)) {
      std::memcpy(static_cast<void *>(row),templateRow,sizeof *row);
      row->base.selectable.base.firstChild = UI_NODE_NONE;
      row->base.selectable.base.nodeFlags &= ~(UI_NODE_HAS_KEYBOARD_FOCUS | UI_NODE_REPEAT_OR_DOUBLE_CLICK);
      row->base.selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
      row->base.selectable.actionId = FRONTEND_ACTION_RESOLUTION_OPTION1;
      rowTop = FRONTEND_DISPLAY_RESOLUTION_ROW_INSET + (int32_t)rowIndex * FRONTEND_DISPLAY_RESOLUTION_ROW_HEIGHT;
      row->base.selectable.base.topOffset = rowTop;
      row->base.selectable.base.bottomOffset = rowTop + FRONTEND_DISPLAY_RESOLUTION_ROW_HEIGHT;
    }
    row->base.selectable.base.parent = rowPanel;
    row->firstValue = (int32_t)(resolutions[firstListed + rowIndex] >> 16);
    row->secondValue = (int32_t)(resolutions[firstListed + rowIndex] & UI_DISPLAY_MODE_HEIGHT_MASK);
    if (previousRow == UI_NODE_NONE) {
      rowPanel->firstChild = &row->base.selectable.base;
    }
    else {
      previousRow->nextSibling = &row->base.selectable.base;
    }
    previousRow = &row->base.selectable.base;
  }
  if (previousRow != UI_NODE_NONE) {
    previousRow->nextSibling = UI_NODE_NONE;
  }
  else {
    rowPanel->firstChild = UI_NODE_NONE;
  }
  if ((resolutionCount != 0) && (resolutionCount != s_resolutionRowCount)) {
    Thandor_Log("display settings: %u resolutions listed, %ux%u .. %ux%u",resolutionCount,
                resolutions[firstListed] >> 16,resolutions[firstListed] & UI_DISPLAY_MODE_HEIGHT_MASK,
                resolutions[firstListed + resolutionCount - 1] >> 16,
                resolutions[firstListed + resolutionCount - 1] & UI_DISPLAY_MODE_HEIGHT_MASK);
  }
  s_resolutionRowCount = resolutionCount;
  rowPanel->bottomOffset =
       2 * FRONTEND_DISPLAY_RESOLUTION_ROW_INSET + (int32_t)resolutionCount * FRONTEND_DISPLAY_RESOLUTION_ROW_HEIGHT;
}

/* Lays the resolution list out (the scroll frame decides whether it needs its bar; the rows are as wide as the
   view beside the bar) and, with scrollToSelected, scrolls it so the selected row is in the middle of the view
   as far as the list allows. */
static void FrontendDisplaySettingsPage_LayoutResolutionList(UiNodeBase *frontendRoot,bool scrollToSelected)
{
  UiScrollableControl *scrollBox;
  UiNodeBase *rowPanel;
  UiNumericPairTextButton *row;
  uint32_t rowIndex;

  scrollBox = (UiScrollableControl *)FRONTEND_UI(frontendRoot,displayResolutionScrollBox);
  rowPanel = FRONTEND_UI(frontendRoot,displayResolutionRowPanel);
  rowPanel->rightOffset = scrollBox->base.right - scrollBox->base.left;
  scrollBox->base.vtable->layout(&scrollBox->base);
  rowPanel->rightOffset = scrollBox->viewportWidth;
  scrollBox->base.vtable->layout(&scrollBox->base);
  if (scrollToSelected) {
    scrollBox->scrollOffsetY = 0;
    for (rowIndex = 0; rowIndex < s_resolutionRowCount; rowIndex++) {
      row = FrontendDisplaySettingsPage_ResolutionRow(frontendRoot,rowIndex);
      if ((row->base.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        scrollBox->scrollOffsetY =
             ((int32_t)scrollBox->viewportHeight - FRONTEND_DISPLAY_RESOLUTION_ROW_HEIGHT) / 2 -
             row->base.selectable.base.topOffset;
        break;
      }
    }
    UiScrollableControl_RefreshChildAndScrollThumbs(scrollBox);
  }
  UiNode_InvalidateRoot(&scrollBox->base);
}

/* Handler of action 0x2011 (slot 17 of g_FrontendUiActionHandlersPage20.handlers00_54), the options page's
   "Graphics" button: opens the display settings page and fills its choices: the resolutions and the name and
   device of up to five adapters. The saved adapter and resolution become the current selection. Not in the
   original: there is no colour depth choice any more (the original listed the four smallest distinct colour
   depths), the colour depth is always 32 bits; and the original listed only the ten smallest distinct
   resolutions (width << 16 | height, inserted into a sorted list with 0xFFFFFFFF as the empty mark), here all
   of them are listed in the scrollable list (FrontendDisplaySettingsPage_BuildResolutionRows), which opens
   scrolled to the selected resolution.
*/
void FrontendDisplaySettingsAction_OpenPageAndListModes(FrontendDisplaySettingsPageOptionState *source)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  UiNodeBase *frontendRoot;
  uint32_t adapterIndex;

  /* source is the frontend template's graphicsSettingsButton */
  frontendRoot = (UiNodeBase *)((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton));
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  /* not in the original: page 6 is displayPageStack, the display settings are its first page */
  UiPageStack_SetActiveIndex
            (FRONTEND_DISPLAY_SUBPAGE_DISPLAY,(UiPageStackControl *)FRONTEND_UI(frontendRoot,displayPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  /* name and device of up to five adapters (the first one is always listed) */
  FrontendDisplaySettingsPage_FillAdapterRow(source,0);
  for (adapterIndex = 1; (adapterIndex < 5) && (adapterIndex < g_GraphicsAdapterCount); adapterIndex++) {
    FrontendDisplaySettingsPage_FillAdapterRow(source,adapterIndex);
  }
  FrontendDisplaySettingsPage_BuildResolutionRows(frontendRoot);
  /* Not in the original: the adapter is the saved renderer (the original read its adapter index here), and the
     display mode kind choice */
  FrontendDisplaySettingsPage_ReadSavedRendererAndKind();
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(frontendRoot);
  FrontendDisplaySettingsPage_LayoutResolutionList(frontendRoot,true);
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
}

/* Handler of the resolution choices of the display settings page (actions 0x2022..0x202B, slots 34-43 of
   g_FrontendUiActionHandlersPage20; the extra rows of open-thandor use 0x2022): takes the clicked button's width/height pair as the pending resolution and
   refreshes which choices are available. Nothing is applied before the apply action (0x2031).
*/
void FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton)

{
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       ((UiNumericPairTextButton *)optionButton)->firstValue;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       ((UiNumericPairTextButton *)optionButton)->secondValue;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(optionButton);
}

/* The end of a successful display mode switch (FrontendDisplaySettings_ApplyMode; not in the original: also the
   advanced settings page's UI scale): lays the UI out again, converts the palette-based UI textures to the new
   pixel format, shows the cursor again, refreshes the display settings page and decides whether the dialog pages
   cover the menu room. control is any node of the frontend template. */
static void FrontendDisplaySettings_FinishModeSwitch(void *control)
{
  int remainingFonts;
  UiNodeBase *parentCursor;
  GraphicsTextureSourceAsset **fontTextureSource;

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
}

/* Handler of the display settings page's apply action (FRONTEND_ACTION_APPLY_DISPLAY_MODE, slot 49 of
   g_FrontendUiActionHandlersPage20): switches to the pending adapter/resolution (always 32-bit colour). On success the mode
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
  if (!g_GraphicsSetDisplayMode
                    (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.adapterIndex,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.bitsPerPixel,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.height,
                     g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.
                     persistentSelection.width,&selectedModeError)) {
    /* the original restored the current depth (the pixel format's RGB bits rounded up to a multiple of 16) */
    if (!g_GraphicsSetDisplayMode(previousAdapterIndex,PERSISTENT_DEFAULT_BITS_PER_PIXEL,previousHeight,
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
    bitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
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
  FrontendDisplaySettings_FinishModeSwitch(control);
}

/* Refreshes the display settings page after the pending mode changed (called by the resolution, adapter, display
   mode kind and apply handlers here and by ui/frontend/runtime). Every resolution and adapter choice is hidden
   unless the adapter offers it together with the other pending value, the choices matching the pending mode are
   selected, and the apply button is only offered while the pending mode differs from the saved one.
   How the selection works in the original: it first pushes all choice controls on the machine stack
   (adapter 1..5, resolution 1..10 and, gone here, colour depth 1..4), then per group pushes every choice whose
   value equals the pending one (resolution: width == firstValue and height == secondValue; adapter: pending
   adapter == 0..4) and calls UiSelectableGroup_SelectExclusive(count, <top of stack>, <the count entries below
   it>), which pops count and the selected control, and then the caller pops count entries. With exactly one
   match per group this selects the matching choice. Quirk of the original: when a group has no match, the entry
   on top (without earlier shifts: that group's last choice) is taken as the selected control but is not in the
   list, so it keeps its state; the group's other choices plus the next group's first choice are deselected, and
   every later group works on a stack shifted by one entry (two matches in one group shift it the other way).
   modeStack models that stack for the adapter group. Once a shift reaches past the pushed controls the original
   also deselects and redraws whatever the values saved below them on its stack point at; this C leaves those
   entries out (listCount is cut at the last control). Not in the original: the colour depth group (32-bit
   colour only) is gone, and the resolution group is the scrollable list: any number of rows, the one matching
   the pending resolution selected and every other one deselected, so no stack shift comes from it.
*/
#define DISPLAY_MODE_STACK_CONTROLS 5 /* adapter 1..5 */
#define DISPLAY_MODE_STACK_BASE DISPLAY_MODE_STACK_CONTROLS /* room for the pushed matches above the controls */
#define DISPLAY_MODE_STACK_END (DISPLAY_MODE_STACK_BASE + DISPLAY_MODE_STACK_CONTROLS)
void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot)

{
  uint32_t adapterIndex;
  uint32_t pendingWidth;
  uint32_t pendingHeight;
  uint32_t bitsPerPixel;
  uint32_t persistedValue;
  uint32_t rowIndex;
  UiNumericPairTextButton *row;
  Bool8 modeCheckCarry;
  /* the original's stack: modeStack[modeStackTop] is the top; the 5 NULL entries after the controls stand
     for the values the original saved below them */
  UiNodeBase *modeStack[DISPLAY_MODE_STACK_END + 5];
  int modeStackTop;
  UiControlCount listCount;

  bitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
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
  /* the resolution rows: offered when the pending adapter has the mode, selected when it is the pending one */
  for (rowIndex = 0; rowIndex < s_resolutionRowCount; rowIndex++) {
    row = FrontendDisplaySettingsPage_ResolutionRow(frontendRoot,rowIndex);
    modeCheckCarry = DisplayModeTable_ContainsExactMode
                      (bitsPerPixel,(FrontendDisplayDimensionPixels)row->secondValue,
                       (FrontendDisplayDimensionPixels)row->firstValue,adapterIndex);
    if (modeCheckCarry) {
      UiSelectableControl_SuppressIfActionId(row->base.selectable.actionId,&row->base.selectable);
    }
    else {
      UiSelectableControl_UnsuppressIfActionId(row->base.selectable.actionId,&row->base.selectable);
    }
    UiSelectableControl_SetSelected
              ((pendingWidth == (uint32_t)row->firstValue) && (pendingHeight == (uint32_t)row->secondValue),
               &row->base.selectable);
  }
  /* push the 5 adapter choices */
  modeStackTop = DISPLAY_MODE_STACK_BASE;
  modeStack[DISPLAY_MODE_STACK_BASE + 0] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption5);
  modeStack[DISPLAY_MODE_STACK_BASE + 1] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption4);
  modeStack[DISPLAY_MODE_STACK_BASE + 2] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption3);
  modeStack[DISPLAY_MODE_STACK_BASE + 3] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption2);
  modeStack[DISPLAY_MODE_STACK_BASE + 4] = (UiNodeBase *)FRONTEND_UI(frontendRoot,displayAdapterOption1);
  modeStack[DISPLAY_MODE_STACK_END + 0] = nullptr;
  modeStack[DISPLAY_MODE_STACK_END + 1] = nullptr;
  modeStack[DISPLAY_MODE_STACK_END + 2] = nullptr;
  modeStack[DISPLAY_MODE_STACK_END + 3] = nullptr;
  modeStack[DISPLAY_MODE_STACK_END + 4] = nullptr;
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
      (persistedValue = PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT), persistedValue == pendingHeight))) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
    return;
  }
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_APPLY_DISPLAY_MODE,frontendRoot);
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

/* Not in the original: the advanced settings page ("Erweitert", displayPageStack page 1, opened by the options
   page's fourth button). Every choice applies and saves its value at once: "3D-Kanten" ([graphics]
   gpu_rasterization, a running GPU renderer draws its next scene with it), "UI-Skalierung" ([graphics] ui_scale;
   with a GPU renderer running the display mode is set again at once, as "Anwenden" does: frame target at the new
   scale, the UI laid out again), "Bildratenbegrenzung"
   and "VSync" (SdlVideo_SetFrameLimit / SdlVideo_SetVsync). Edges and UI scale only matter for the GPU renderers:
   with the software renderer running they can still be chosen (kept for a later switch to Vulkan or DirectX 12;
   a suppressed radio row would be hidden, not greyed) and the note under the boxes says so. */

/* advancedFrameLimitOff, 60, 120, 144 (no 30: below 60 frames per second the simulation slows down) */
static const uint32_t kAdvancedFrameLimits[] = {0,60,120,144};
#define ADVANCED_FRAME_LIMIT_CHOICES 4

/* The frontend root of any of its nodes. */
static UiNodeBase *FrontendAdvancedSettingsPage_Root(UiNodeBase *node)
{
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  return node;
}

/* Selects the choice selected (one of count choices) and deselects the others; none selected for nullptr. */
static void FrontendAdvancedSettingsPage_SelectChoice(UiNodeBase *selected,UiNodeBase *const *choices,uint32_t count)
{
  uint32_t index;

  for (index = 0; index < count; index++) {
    UiSelectableControl_SetSelected(choices[index] == selected,(UiSelectableControl *)choices[index]);
  }
}

/* Shows the current values on the advanced settings page: the selected choices, the VSync checkbox and the note
   (only with the software renderer: edges and UI scale need a GPU renderer). */
static void FrontendAdvancedSettingsPage_Refresh(UiNodeBase *frontendRoot)
{
  UiNodeBase *edges[2];
  UiNodeBase *uiScales[4];
  UiNodeBase *frameLimits[ADVANCED_FRAME_LIMIT_CHOICES];
  UiNodeBase *selected;
  uint32_t index;
  uint32_t value;
  bool gpu;

  edges[0] = FRONTEND_UI(frontendRoot,advancedEdgesSmooth);
  edges[1] = FRONTEND_UI(frontendRoot,advancedEdgesExact);
  uiScales[0] = FRONTEND_UI(frontendRoot,advancedUiScaleAuto);
  uiScales[1] = FRONTEND_UI(frontendRoot,advancedUiScale1);
  uiScales[2] = FRONTEND_UI(frontendRoot,advancedUiScale2);
  uiScales[3] = FRONTEND_UI(frontendRoot,advancedUiScale3);
  frameLimits[0] = FRONTEND_UI(frontendRoot,advancedFrameLimitOff);
  frameLimits[1] = FRONTEND_UI(frontendRoot,advancedFrameLimit60);
  frameLimits[2] = FRONTEND_UI(frontendRoot,advancedFrameLimit120);
  frameLimits[3] = FRONTEND_UI(frontendRoot,advancedFrameLimit144);
  gpu = SdlVideo_GpuRendererActive();
  FrontendAdvancedSettingsPage_SelectChoice
            (edges[(SdlVideo_GpuRasterization() == PERSISTENT_GPU_RASTERIZATION_EXACT) ? 1 : 0],edges,2);
  FrontendAdvancedSettingsPage_SelectChoice(uiScales[SdlVideo_SavedUiScale()],uiScales,4);
  value = SdlVideo_GetFrameLimit();
  selected = nullptr;
  for (index = 0; index < ADVANCED_FRAME_LIMIT_CHOICES; index++) {
    if (kAdvancedFrameLimits[index] == value) {
      selected = frameLimits[index];
    }
  }
  /* another limit (ini, environment): none selected; the stored value is kept until a choice is clicked */
  FrontendAdvancedSettingsPage_SelectChoice(selected,frameLimits,ADVANCED_FRAME_LIMIT_CHOICES);
  UiSelectableControl_SetSelected(SdlVideo_GetVsync(),
                                  (UiSelectableControl *)FRONTEND_UI(frontendRoot,advancedVsyncCheckbox));
  FRONTEND_UI_FIELD(frontendRoot,advancedNoteLabel,0x54,TextResourceId) =
       gpu ? TEXT_RESOURCE_ID_NONE : TEXT_ID_ADVANCED_NOTE_SOFTWARE;
  UiNode_InvalidateRoot(FRONTEND_UI(frontendRoot,advancedSettingsPage));
}

/* Handler of FRONTEND_ACTION_OPEN_ADVANCED_SETTINGS (the options page's "Erweitert" button): shows the advanced
   settings page (as the display settings page: the menu room is not drawn behind it in the compact layout). */
void FrontendAdvancedSettingsAction_OpenPage(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot;
  FrontendModelPointerContextFlags *menuRoomContextFlags;

  frontendRoot = FrontendAdvancedSettingsPage_Root(sourceNode);
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  UiPageStack_SetActiveIndex
            (FRONTEND_DISPLAY_SUBPAGE_ADVANCED,(UiPageStackControl *)FRONTEND_UI(frontendRoot,displayPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  FrontendAdvancedSettingsPage_Refresh(frontendRoot);
}

/* Handler of FRONTEND_ACTION_ADVANCED_EDGES ("Glatt" / "Original"). */
void FrontendAdvancedSettingsAction_SelectEdges(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = FrontendAdvancedSettingsPage_Root(sourceNode);

  SdlVideo_SetGpuRasterization((sourceNode == FRONTEND_UI(frontendRoot,advancedEdgesExact)) ?
                                    PERSISTENT_GPU_RASTERIZATION_EXACT : PERSISTENT_GPU_RASTERIZATION_SMOOTH);
  FrontendAdvancedSettingsPage_Refresh(frontendRoot);
}

/* Handler of FRONTEND_ACTION_ADVANCED_UI_SCALE ("Auto", "1x", "2x", "3x"): saves the scale and, with a GPU
   renderer running and a scale that differs from the one in use (OPEN_THANDOR_UI_SCALE wins), sets the current
   display mode (same renderer, size and kind) again, which makes the frame target at the new scale (auto: the
   largest whole one at which the mode fits the display), and finishes as "Anwenden" does. The page stays open. */
void FrontendAdvancedSettingsAction_SelectUiScale(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = FrontendAdvancedSettingsPage_Root(sourceNode);
  uint32_t scale;
  uint32_t modeError;

  if (sourceNode == FRONTEND_UI(frontendRoot,advancedUiScale1)) {
    scale = 1;
  }
  else if (sourceNode == FRONTEND_UI(frontendRoot,advancedUiScale2)) {
    scale = 2;
  }
  else if (sourceNode == FRONTEND_UI(frontendRoot,advancedUiScale3)) {
    scale = 3;
  }
  else {
    scale = PERSISTENT_UI_SCALE_AUTO;
  }
  SdlVideo_SaveUiScale(scale);
  if (SdlVideo_GpuRendererActive() && SdlVideo_UiScaleChangePending()) {
    g_CursorVisibilityToken--;
    if (!g_GraphicsSetDisplayMode(g_ActiveGraphicsAdapterIndex,PERSISTENT_DEFAULT_BITS_PER_PIXEL,g_FramebufferHeight,
                                  g_FramebufferWidth,&modeError)) {
      FatalError_ExitIfFailed(modeError,true); /* the mode in use could not be set again */
    }
    FrontendDisplaySettings_FinishModeSwitch(sourceNode);
  }
  FrontendAdvancedSettingsPage_Refresh(frontendRoot);
}

/* Handler of FRONTEND_ACTION_ADVANCED_FRAME_LIMIT ("Aus", 60, 120, 144 frames per second). */
void FrontendAdvancedSettingsAction_SelectFrameLimit(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = FrontendAdvancedSettingsPage_Root(sourceNode);
  UiNodeBase *const frameLimits[ADVANCED_FRAME_LIMIT_CHOICES] = {
      FRONTEND_UI(frontendRoot,advancedFrameLimitOff),FRONTEND_UI(frontendRoot,advancedFrameLimit60),
      FRONTEND_UI(frontendRoot,advancedFrameLimit120),FRONTEND_UI(frontendRoot,advancedFrameLimit144)};
  uint32_t index;

  for (index = 0; index < ADVANCED_FRAME_LIMIT_CHOICES; index++) {
    if (sourceNode == frameLimits[index]) {
      SdlVideo_SetFrameLimit(kAdvancedFrameLimits[index]);
    }
  }
  FrontendAdvancedSettingsPage_Refresh(frontendRoot);
}

/* Handler of FRONTEND_ACTION_ADVANCED_VSYNC (the "VSync" checkbox; the click has toggled it). */
void FrontendAdvancedSettingsAction_SetVsync(UiNodeBase *sourceNode)
{
  SdlVideo_SetVsync(UiSelectableControl_IsSelected((UiSelectableControl *)sourceNode) != 0);
  FrontendAdvancedSettingsPage_Refresh(FrontendAdvancedSettingsPage_Root(sourceNode));
}
