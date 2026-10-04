/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/root_stack.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/root_stack.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_UiInvalidationSuppressed = 0;

UiRootNode *g_UiRootNode = UI_ROOT_STACK_END;

/* Implementation ownership: ui/controls/root_stack. */

/* Closes the UI roots of an ending session: pops the front root until the stack is empty; a root that vetoes
   its close stops the loop and is reported by returning true. The original also stops at the dword after
   g_UiRootNode (g_UiWindowTextureSource), which is never a root, so in practice this pops every
   root.
*/
Bool8 UiRootStack_PopUntilWindowTextureBoundary(void)

{
  Bool8 popStopped;

  while (((GraphicsTextureSourceAsset *)g_UiRootNode != g_UiWindowTextureSource &&
         (g_UiRootNode != UI_ROOT_STACK_END))) {
    popStopped = UiRootStack_Pop(g_UiRootNode);
    if (popStopped) {
      return true;
    }
  }
  return false;
}

/* Opens a dialog or screen: puts the serialized UI tree root on top of the root stack. Its rectangle is
   computed from the framebuffer size and its anchors, its callbacks are attached and its tree pointers
   relocated. The previous top root loses UI_NODE_IN_FRONT_ROOT (windows draw as inactive), the new one is
   laid out, gets the flag and the initial keyboard focus; pointer capture and tooltip are reset.
*/
void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root)

{
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *oldFrontRoot;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  /* each edge = (framebuffer extent * anchorQ31) >> 31 + offset, i.e. a fraction of the screen plus pixels */
  anchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
  (root->base).right =
       (FIXED_PRODUCT_SHR(anchorPixelProductQ31, 31))
       + (root->base).rightOffset;
  currentAnchorPixelProductQ31 =
       (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
  (root->base).bottom =
       (FIXED_PRODUCT_SHR(currentAnchorPixelProductQ31, 31)) + (root->base).bottomOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
  (root->base).left =
       (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
       (root->base).leftOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
  (root->base).top =
       (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
       (root->base).topOffset;
  root->callbacks = callbacks;
  (root->base).nextSibling = UI_NODE_NONE;
  /* the serialized tree links are offsets from the root: relocate by the root's address */
  UiSerializedTree_Relocate((SerializedImageRelocationDelta)root,&root->base); /* 5f-format: UI template tree links (32-bit offsets relocated by the root address) */
  oldFrontRoot = g_UiRootNode;
  LOCK();
  g_UiRootNode = root;
  UNLOCK();
  root->previousRoot = oldFrontRoot;
  if (oldFrontRoot != UI_ROOT_STACK_END) {
    (oldFrontRoot->base).nextSibling = &root->base;
    (*((oldFrontRoot->base).vtable)->applyFlags)(0,~UI_NODE_IN_FRONT_ROOT,&oldFrontRoot->base);
  }
  (*((root->base).vtable)->layout)(&root->base);
  (*((root->base).vtable)->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&root->base);
  UiKeyboardFocus_SelectInitial(&root->base);
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiTooltipState.targetNode = NULL;
  return;
}

/* Closes the dialog or screen that contains root (any node of it may be passed): its close callback may
   veto (returns true). Otherwise the root below becomes the top again with UI_NODE_IN_FRONT_ROOT and its
   initial focus, pointer capture and hover are reset and the whole screen is redrawn. The closed root is
   assumed to be the top one: only g_UiRootNode is replaced.
*/
Bool8 UiRootStack_Pop(UiRootNode *root)

{
  Bool8 closeCallbackVetoed;
  UiRootNode *belowRoot;
  UiNodeBase *parentCursor;
  
  parentCursor = (root->base).parent;
  while (parentCursor != UI_NODE_NONE) {
    root = (UiRootNode *)(root->base).parent;
    parentCursor = (root->base).parent;
  }
  belowRoot = root->previousRoot;
  closeCallbackVetoed = false;
  if (root->callbacks->vetoClose != NULL) {
    closeCallbackVetoed = root->callbacks->vetoClose(root);
  }
  if (closeCallbackVetoed) {
    UiNode_InvalidateRoot(&belowRoot->base);
    return true;
  }
  g_UiKeyboardFocusNode = UI_NODE_NONE;
  g_UiRootNode = belowRoot;
  if (belowRoot != UI_ROOT_STACK_END) {
    (belowRoot->base).nextSibling = UI_NODE_NONE;
    (*((belowRoot->base).vtable)->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&belowRoot->base);
    UiKeyboardFocus_SelectInitial(&belowRoot->base);
  }
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiImageControlHoverTarget = NULL;
  UiRootStack_InvalidateAll();
  return false;
}

/* Moves an open root (window) to the top of the root stack: unlinks it from its position, links it above
   the current front root, gives it the initial keyboard focus, moves the in-front flag from the old front
   root to it and invalidates both. Always returns false.
*/
Bool8 UiRootStack_BringToFront(UiRootNode *root)

{
  UiRootNode *belowRoot;
  UiRootNode *oldFrontRoot;
  UiRootNode *nextRootLink;
  UiNodeBase *nextFrontRootLink;

  oldFrontRoot = g_UiRootNode;
  nextRootLink = (UiRootNode *)root->base.nextSibling;
  belowRoot = root->previousRoot;
  if (nextRootLink != UI_ROOT_STACK_END) {
    nextRootLink->previousRoot = belowRoot;
  }
  if (belowRoot != UI_ROOT_STACK_END) {
    belowRoot->base.nextSibling = &nextRootLink->base;
  }
  nextFrontRootLink = g_UiRootNode->base.nextSibling;
  root->previousRoot = g_UiRootNode;
  root->base.nextSibling = nextFrontRootLink;
  g_UiRootNode->base.nextSibling = &root->base;
  g_UiRootNode = root;
  UiKeyboardFocus_SelectInitial(&root->base);
  (*oldFrontRoot->base.vtable->applyFlags)(0,~UI_NODE_IN_FRONT_ROOT,&oldFrontRoot->base);
  (*root->base.vtable->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&root->base);
  UiNode_InvalidateRoot(&oldFrontRoot->base);
  UiNode_InvalidateRoot(&root->base);
  return false;
}

/* After a display mode change (UiDisplayModeAction_ApplyPendingMode, FrontendDisplaySettings_ApplyMode):
   recomputes the rectangle of every open root from the new framebuffer size, its Q31 anchors and pixel
   offsets (as UiRootStack_Push does) and lays it out again, from the front root down. Assumes at least one
   open root.
*/
void UiRootStack_Relayout(void)

{
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *rootNode;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  rootNode = g_UiRootNode;
  do {
    /* each edge = (framebuffer extent * anchorQ31) >> 31 + offset */
    anchorPixelProductQ31 =
         (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).rightAnchorQ31;
    (rootNode->base).right =
         (FIXED_PRODUCT_SHR(anchorPixelProductQ31, 31)) +
         (rootNode->base).rightOffset;
    currentAnchorPixelProductQ31 =
         (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).bottomAnchorQ31;
    (rootNode->base).bottom =
         (FIXED_PRODUCT_SHR(currentAnchorPixelProductQ31, 31)) + (rootNode->base).bottomOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).leftAnchorQ31;
    (rootNode->base).left =
         (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
         (rootNode->base).leftOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).topAnchorQ31;
    (rootNode->base).top =
         (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
         (rootNode->base).topOffset;
    (*((rootNode->base).vtable)->layout)(&rootNode->base);
    rootNode = rootNode->previousRoot;
  } while (rootNode != UI_ROOT_STACK_END);
  return;
}

/* Marks the whole screen for redraw: drops the collected dirty rectangles and invalidates every root on the UI
   root stack, top to bottom. Does nothing while invalidation is suppressed.
*/
void UiRootStack_InvalidateAll(void)

{
  UiRootNode *root;

  if (g_UiInvalidationSuppressed == 0) {
    g_UiDirtyRectCount = 0;
    for (root = g_UiRootNode; root != UI_ROOT_STACK_END; root = root->previousRoot) {
      UiNode_InvalidateRoot(&root->base);
    }
  }
  return;
}

__declspec(align(16)) UiRootStackActionHandlerPage2 g_UiRootStackActionHandlerPage = {.handlers = {THANDOR_FN(UiRootStack_Pop), THANDOR_FN(FatalErrorDialog_DismissAndPopRoot)}};
