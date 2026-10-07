/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/dialogs/fatal_error.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/dialogs/fatal_error.h>
#include <algorithm>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static UiRootNode *g_FatalErrorUiRootTemplate = nullptr;

static uint32_t g_FatalErrorDialogDismissed = 0;

static UiRootCallbacks g_FatalErrorDialogRootCallbacks = {
    .method08 = UI_SLOT(FatalErrorDialog_BlockMissedPointerPress),
    .pointerMissPolicy = UI_SLOT(FatalErrorDialog_BlockMissedPointerMotion)};

FatalErrorUiImage g_FatalErrorUiRootTemplateImage = {
        { /* +0000 fatalErrorPanel g_UiPanelControlVtable */
            .root = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
                    .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -160, .rightOffset = 160, .bottomOffset = 44,
                    .leftAnchorQ31 = 0x50000000, .topAnchorQ31 = 0x50000000, .rightAnchorQ31 = 0x50000000, .bottomAnchorQ31 = 0x50000000,
                    .layoutWidth = -1, .layoutHeight = -1},
                .rootFlags = UI_ROOT_TILED_BACKGROUND | UI_ROOT_FRAME, .callbacks = THANDOR_PTR32_BITS(0xFFFFFFFF),
                .previousRoot = THANDOR_PTR32_BITS(0xFFFFFFFF)}},
        { /* +0058 errorMessageText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .leftOffset = 6, .topOffset = 6, .rightOffset = -6, .bottomOffset = -38,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015},
        { /* +00B4 okButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .leftOffset = -108, .topOffset = -32, .rightOffset = -12, .bottomOffset = -6,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_PREFERRED_FOCUS_TARGET},
        {.stateFlags = FromBits<UiSelectableStateFlags>(0x0000000C), .actionId = 0x00000001, .textResourceId = 0x00000100},
};

/* method08 of g_FatalErrorDialogRootCallbacks, the callbacks of the fatal-error dialog root: always returns true, so
   a pointer event that misses the dialog ends the root-stack hit test there instead of reaching the roots
   below (the dialog is modal).
*/
bool FatalErrorDialog_BlockMissedPointerPress(UiRootNode *root)

{
  return true;
}

/* pointerMissPolicy of g_FatalErrorDialogRootCallbacks (the fatal-error dialog root): the non-negative result
   stops the pointer traversal at the dialog, so the roots below it get no pointer input. The value 8 itself
   carries no meaning beyond being non-negative.
*/
int FatalErrorDialog_BlockMissedPointerMotion(UiRootNode *root)

{
  return 8;
}

/* Handler of UI action 1 (slot 1 of g_UiRootStackActionHandlerPage, installed as action page 0 by the UI
   setup in ui/controls/root_stack.cpp): closes the fatal-error dialog root and counts the dismissal, which ends
   the modal frame loop in FatalErrorRuntime_DispatchPendingError.
*/
void FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode)

{
  UiRootStack_Pop(rootNode);
  g_FatalErrorDialogDismissed++;
}

/* The in-game fatal-error handler behind FatalError_ReportIfFailed (installed by
   ErrorRuntime_InstallUiHandlerAndAllocateState): returns valueOrError unchanged. When failed is set it first
   builds the message like FatalError_Exit, opens it as a modal dialog sized to the text and runs UI frames
   until the dialog is dismissed, so the caller can carry on (the caller knows the failure from its own flag).
*/
uintptr_t FatalErrorRuntime_DispatchPendingError(uintptr_t valueOrError,Bool8 failed)

{
  UiRootNode *dialogRoot;
  uint16_t *stream;
  const uint32_t *templateImageCursor;
  uint32_t *templateCopyCursor;
  RichTextExtent wrappedExtent;
  uintptr_t error;

  if (!failed) {
    return valueOrError;
  }
  /* valueOrError is the FatalErrorPassThroughProc contract: a caller value passed through unchanged, or
     (failed set) the error code or rich-text stream, which is all it means from here on */
  error = valueOrError;
  if (g_FatalErrorUiRootTemplate == nullptr) {
    /* no dialog state allocated yet: FatalError_Exit, which does not return */
    error = FatalError_ExitIfFailed(error,true);
  }
  stream = reinterpret_cast<uint16_t *>(error); /* a rich-text stream passed as an address */
  if (FATAL_ERROR_IS_CODE(error)) {
    /* only resolved error-page texts get the payload selectors 0..3 patched (FatalError_Exit patches
       every stream) */
    stream = TextResource_Resolve((uint32_t)error);
    RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(2,g_FatalErrorDetail2Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(3,g_FatalErrorDetail3Utf16,stream);
  }
  /* the message text goes into the template image itself, which is then copied */
  g_FatalErrorUiRootTemplateImage.errorMessageText.text = stream;
  /* copy the dialog template image into the allocated root node, one dword per step */
  templateImageCursor = reinterpret_cast<const uint32_t *>(&g_FatalErrorUiRootTemplateImage);
  templateCopyCursor = reinterpret_cast<uint32_t *>(g_FatalErrorUiRootTemplate);
  std::copy_n(templateImageCursor,sizeof g_FatalErrorUiRootTemplateImage / sizeof(uint32_t),templateCopyCursor);
  dialogRoot = g_FatalErrorUiRootTemplate;
  /* the text height is subtracted from the dialog's top offset; the wrap width is the panel width (its
     right - left offset) narrowed by the text's left and right insets, all read from the template image */
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,
                     g_FatalErrorUiRootTemplateImage.errorMessageText.text,
                     ((g_FatalErrorUiRootTemplateImage.fatalErrorPanel.root.base.rightOffset -
                       g_FatalErrorUiRootTemplateImage.fatalErrorPanel.root.base.leftOffset) +
                     g_FatalErrorUiRootTemplateImage.errorMessageText.base.rightOffset) -
                    g_FatalErrorUiRootTemplateImage.errorMessageText.base.leftOffset);
  dialogRoot->base.topOffset = dialogRoot->base.topOffset - wrappedExtent.heightPixels;
  UiRootStack_Push(&g_FatalErrorDialogRootCallbacks,g_FatalErrorUiRootTemplate);
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  UiFrame_FlushInputAndResetPendingTicks();
  g_GraphicsCursorSetFrame(0);
  g_FatalErrorDialogDismissed = 0;
  do {
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresentWithLockTransition();
  } while (g_FatalErrorDialogDismissed == 0);
  return error;
}

/* Allocates the 0x110-byte root node of the fatal-error dialog (FatalErrorRuntime_DispatchPendingError
   fills it from g_FatalErrorUiRootTemplateImage) and, if that worked, switches FatalError_ReportIfFailed
   from FatalError_Exit to the in-game dialog. Without the allocation errors keep ending the process.
*/
void ErrorRuntime_InstallUiHandlerAndAllocateState()

{
  void *allocPayload;

  if (g_MemoryApi.alloc(sizeof g_FatalErrorUiRootTemplateImage,&allocPayload) == 0) {
    g_FatalErrorReportHandler = FatalErrorRuntime_DispatchPendingError;
    g_FatalErrorUiRootTemplate = static_cast<UiRootNode *>(allocPayload);
  }
}
