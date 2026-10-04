/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/container.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/container.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Shows page pageIndex of a page stack (tabbed dialog pages): the visible page is the stack's only child
   (firstChild), so switching replaces that link, moving the keyboard focus out of the old page and into the
   new one, and redraws. Out-of-range indices and the already shown page are ignored.
*/
void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack)

{
  UiNodeBase *pageNode;
  
  if (pageIndex >= stack->pageCount) {
    return;
  }
  pageNode = (&stack->pages)[pageIndex];
  if (pageNode != (stack->base).firstChild) {
    UiNodeSubtree_ReleaseKeyboardFocus(&stack->base);
    (stack->base).firstChild = pageNode;
    UiNodeSubtree_AcquireKeyboardFocusDefaults(&stack->base);
    UiNode_InvalidateRoot(&stack->base);
  }
}

/* relocate of g_UiLayoutContainerControlVtable (the page stack, UiPageStackControl): turns the page links
   from image offsets into pointers, relocates every page's tree by making it the stack's firstChild in
   turn, and leaves page 0 as the shown page. The original page-stack methods loop with do-while and assume
   at least one page; here every page loop is a while loop because a page count of 0 would wrap and walk
   ~2^32 page slots (logged once here, at load).
*/
void UiLayoutContainerControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  Ptr32<UiNodeBase> *pageSlot;
  Ptr32<UiNodeBase> *pageCursor;

  if (control->pageCount == 0) {
    static int s_loggedEmptyPageStack;
    if (s_loggedEmptyPageStack == 0) {
      s_loggedEmptyPageStack = 1;
      Thandor_Log("page stack %p: page count 0, page loops skipped",(void *)control);
    }
    control->base.firstChild = UI_NODE_NONE; /* no page to show; the page slot holds an unrelocated offset */
    return;
  }
  pageSlot = &control->pages;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    if (*pageSlot != UI_NODE_NONE) {
      *pageSlot = (UiNodeBase *)((uint8_t *)*pageSlot + relocationDelta);
    }
    pageSlot = pageSlot + 1;
    remainingCount--;
  }
  pageCursor = &control->pages;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    control->base.firstChild = *pageCursor;
    UiContainer_RelocateChildren(relocationDelta,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  }
  control->base.firstChild = control->pages;
}

/* layout of g_UiLayoutContainerControlVtable: lays out every page of the page stack, hidden ones included,
   by making each the stack's firstChild in turn; the shown page is restored afterwards.
*/
void UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control)

{
  UiPageCount remainingCount;
  Ptr32<UiNodeBase> *pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    control->base.firstChild = *pageCursor;
    UiContainer_LayoutChildren(&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  }
  control->base.firstChild = shownPage;
}

/* hitTest of g_UiLayoutContainerControlVtable (the page stack): hit-tests the shown page like
   UiContainer_HitTestChildren, but the stack itself is never hit (UI_NODE_NONE instead).
*/
UiNodeBase * UiLayoutContainerControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;

  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = UI_NODE_NONE;
  }
  return hitNode;
}

/* suppressActionId of g_UiLayoutContainerControlVtable: suppresses the controls carrying actionId on every
   page of the page stack, hidden ones included (each page is made firstChild in turn).
*/
void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  Ptr32<UiNodeBase> *pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    control->base.firstChild = *pageCursor;
    UiContainer_SuppressActionId(actionId,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  }
  control->base.firstChild = shownPage;
}

/* unsuppressActionId of g_UiLayoutContainerControlVtable: the counterpart of
   UiLayoutContainerControl_SuppressActionIdRecursive for every page of the page stack.
*/
void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  Ptr32<UiNodeBase> *pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    control->base.firstChild = *pageCursor;
    UiContainer_UnsuppressActionId(actionId,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  }
  control->base.firstChild = shownPage;
}

/* applyFlags of g_UiLayoutContainerControlVtable: applies the node-flag masks (UiNode_ApplyFlagsRecursive)
   to every page of the page stack, hidden ones included, by making each the stack's firstChild in turn.
*/
void UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  Ptr32<UiNodeBase> *pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  while (remainingCount != 0) {
    control->base.firstChild = *pageCursor;
    UiNode_ApplyFlagsRecursive(setMask,retainMask,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  }
  control->base.firstChild = shownPage;
}

/* Picks a grid (columns and rows) for itemCount items: up to 4 items in one row, up to
   4 * maxRows items in rows of 4, more in maxRows rows (fewer when the last rows would stay empty) of as
   many columns as needed.
*/
UiGridDimensions UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount)

{
  uint32_t columnCount;
  uint32_t rowCount;
  UiGridDimensions dimensions;

  rowCount = 1;
  columnCount = itemCount;
  if (4 < itemCount) {
    if (maxRows << 2 < itemCount) {
      /* the mask mirrors the 32-bit maxRows << 2 of the comparison */
      rowCount = maxRows & 0x3fffffff;
      columnCount = (itemCount - 1) / rowCount + 1;
      do {
        if ((rowCount - 1) * columnCount < itemCount) break;
        rowCount = rowCount - 1;
      } while (rowCount != 0);
    }
    else {
      columnCount = 4;
      rowCount = itemCount + 3 >> 2;
    }
  }
  dimensions.columnCount = columnCount;
  dimensions.rowCount = rowCount;
  return dimensions;
}

/* The single-column counterpart of UiGrid_ComputeDimensionsPacked: itemCount rows, 1 column.
*/
UiGridDimensions UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount)

{
  UiGridDimensions dimensions;

  dimensions.columnCount = 1;
  dimensions.rowCount = itemCount;
  return dimensions;
}

/* suppressActionId of the plain containers (g_UiPanelControlVtable, g_UiTitledWindowControlVtable,
   g_UiResizableWindowControlVtable and most other container vtables): passes the request on to every child;
   the controls that carry an action id suppress themselves when it matches.
*/
void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    childNode->vtable->suppressActionId(actionId,childNode);
  }
}

/* unsuppressActionId of the same container vtables as UiContainer_SuppressActionId: passes the request on to
   every child.
*/
void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    childNode->vtable->unsuppressActionId(actionId,childNode);
  }
}

/* Relocates a UI tree loaded from a serialized image: for each node of the sibling chain from firstNode
   that is still unrelocated (layoutWidth -1, reset to 0 here) the sibling/child/parent links are turned
   from image offsets into pointers by adding imageDelta, the transient click and focus flags are cleared,
   and the node's own relocate method runs (containers relocate their children from there).
*/
void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode)

{
  for (; (firstNode != UI_NODE_NONE && (firstNode->layoutWidth == -1));
      firstNode = firstNode->nextSibling) {
    firstNode->layoutWidth = ~firstNode->layoutWidth;
    if (firstNode->nextSibling != UI_NODE_NONE) {
      firstNode->nextSibling = (UiNodeBase *)((uint8_t *)firstNode->nextSibling + imageDelta);
    }
    if (firstNode->firstChild != UI_NODE_NONE) {
      firstNode->firstChild = (UiNodeBase *)((uint8_t *)firstNode->firstChild + imageDelta);
    }
    if (firstNode->parent != UI_NODE_NONE) {
      firstNode->parent = (UiNodeBase *)((uint8_t *)firstNode->parent + imageDelta);
    }
    firstNode->nodeFlags =
         firstNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    firstNode->vtable->relocate(imageDelta,firstNode);
  }
}

/* Gives the keyboard focus, if nothing has it, to the first focus target below root (depth first), e.g. when
   a page becomes active.
*/
void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root)

{
  UiNodeBase *node;

  for (node = root->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
    UiKeyboardFocus_AcquireIfNone(node);
    UiNodeSubtree_AcquireKeyboardFocusDefaults(node);
  }
}

/* Takes the keyboard focus away from every node below root (depth first; see UiKeyboardFocus_ReleaseNode)
   before that subtree, e.g. a page, is deactivated.
*/
void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root)

{
  UiNodeBase *node;

  for (node = root->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
    UiKeyboardFocus_ReleaseNode(node);
    UiNodeSubtree_ReleaseKeyboardFocus(node);
  }
}

/* layout of g_UiResizableWindowControlVtable (also called after maximize/restore): lays out the children
   below the title bar when the window has one (UI_ROOT_TITLE_BAR), i.e. with top moved down by the bar
   height for the call, and adds that height back to layoutHeight afterwards.
*/
void UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control)

{
  uint32_t headerHeight;
  GraphicsTextureLogicalSize headerSize;

  if ((control->root.rootFlags & UI_ROOT_TITLE_BAR) == 0) {
    UiContainer_LayoutChildren((UiNodeBase *)control);
  }
  else {
    headerSize = g_GraphicsTextureSourceGetLogicalSize
                           (UI_WINDOW_SUBRESOURCE_TITLE_BAR + UI_WINDOW_TITLE_BAR_MIDDLE,g_UiWindowTextureSource);
    headerHeight = headerSize.logicalHeightPixels;
    control->root.base.top = control->root.base.top + headerHeight;
    UiContainer_LayoutChildren((UiNodeBase *)control);
    control->root.base.top = control->root.base.top - headerHeight;
    control->root.base.layoutHeight = control->root.base.layoutHeight + headerHeight;
  }
}

/* Eligible siblings from `child` on, hit-tested last first. The recursion first checks every remaining
   sibling for eligibility and only then calls hitTest on the way back, like the original, which pushes
   all eligible children before popping them. Returns UI_NODE_NONE when none claims the pointer. */
static UiNodeBase *UiContainer_HitTestEligibleSiblings
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *child) {
  UiNodeBase *hit;

  for (; child != UI_NODE_NONE; child = child->nextSibling) {
    if ((((child->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0) ||
         ((child->left <= pointerX && child->top <= pointerY) &&
          (pointerX < child->right && pointerY < child->bottom))) &&
        ((child->nodeFlags & UI_NODE_SUPPRESSED) == 0)) {
      hit = UiContainer_HitTestEligibleSiblings(pointerY,pointerX,child->nextSibling);
      if (hit != UI_NODE_NONE) {
        return hit;
      }
      return child->vtable->hitTest(pointerY,pointerX,child);
    }
  }
  return UI_NODE_NONE;
}

/* Finds the UI node under the pointer: every non-suppressed child that contains the pointer (or may be hit
   outside its bounds) is asked via its hitTest method, the last sibling (drawn on top) first. Returns the
   first hit, or the container itself when no child claims the pointer.
*/
UiNodeBase * UiContainer_HitTestChildren(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  /* Eligible children are found in sibling order and hit-tested in reverse (topmost first).
     Like the original there is no limit on the number of eligible children (the helper recurses). */
  UiNodeBase *hit;

  hit = UiContainer_HitTestEligibleSiblings(pointerY,pointerX,control->firstChild);
  if (hit != UI_NODE_NONE) {
    return hit;
  }
  return control;
}

/* relocate of the plain containers (g_UiPanelControlVtable, g_UiTitledWindowControlVtable and most other
   container vtables): relocates the children like UiSerializedTree_Relocate does for a root's siblings.
   Every child still unrelocated (layoutWidth -1, reset to 0 here) gets its sibling/child/parent links turned
   from image offsets into pointers and its transient click and focus flags cleared, then relocates its own
   subtree through its relocate method.
*/
void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild;
      (childNode != UI_NODE_NONE && (childNode->layoutWidth == -1));
      childNode = childNode->nextSibling) {
    childNode->layoutWidth = ~childNode->layoutWidth;
    if (childNode->nextSibling != UI_NODE_NONE) {
      childNode->nextSibling = (UiNodeBase *)((uint8_t *)childNode->nextSibling + relocationDelta);
    }
    if (childNode->firstChild != UI_NODE_NONE) {
      childNode->firstChild = (UiNodeBase *)((uint8_t *)childNode->firstChild + relocationDelta);
    }
    if (childNode->parent != UI_NODE_NONE) {
      childNode->parent = (UiNodeBase *)((uint8_t *)childNode->parent + relocationDelta);
    }
    childNode->nodeFlags =
         childNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    childNode->vtable->relocate(relocationDelta,childNode);
  }
}

/* drawClipped of g_UiLayoutContainerControlVtable and the tail of the container draw methods: draws each
   child whose rectangle intersects the clip rectangle, first child first (later siblings on top). The clip
   rectangle is passed on unchanged.
*/
void UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    if ((((childNode->left <= clipRight) && (childNode->top <= clipBottom)) &&
        (clipLeft < childNode->right)) && (clipTop < childNode->bottom)) {
      childNode->vtable->drawClipped(clipBottom,clipRight,clipTop,clipLeft,childNode);
    }
  }
}

/* Default layout of a container: stores its own width/height, then places every child. Each child edge is
   (parent extent * anchorQ31) >> 31 + offset from the parent's left/top, i.e. a fraction of the parent plus
   a pixel offset; then the child lays out its own children.
*/
void UiContainer_LayoutChildren(UiNodeBase *control)

{
  UiNodeBase *childNode;
  int64_t leftAnchorProduct;
  int64_t topAnchorProduct;
  int rightEdge;
  int bottomEdge;
  int leftEdge;
  int topEdge;
  int64_t bottomAnchorProduct;
  int64_t rightAnchorProduct;
  
  childNode = control->firstChild;
  control->layoutWidth = control->right - control->left;
  control->layoutHeight = control->bottom - control->top;
  for (; childNode != UI_NODE_NONE; childNode = childNode->nextSibling) {
    rightAnchorProduct =
         (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->rightAnchorQ31;
    rightEdge =
         (FIXED_PRODUCT_SHR(rightAnchorProduct, 31)
         ) + childNode->rightOffset + control->left;
    childNode->right = rightEdge;
    childNode->layoutWidth = rightEdge; /* minus the left edge below */
    bottomAnchorProduct =
         (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->bottomAnchorQ31;
    bottomEdge =
         (FIXED_PRODUCT_SHR(bottomAnchorProduct, 31)) + childNode->bottomOffset + control->top;
    childNode->bottom = bottomEdge;
    childNode->layoutHeight = bottomEdge;
    leftAnchorProduct = (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->leftAnchorQ31;
    leftEdge = (FIXED_PRODUCT_SHR(leftAnchorProduct, 31)) +
               childNode->leftOffset + control->left;
    childNode->left = leftEdge;
    childNode->layoutWidth = childNode->layoutWidth - leftEdge;
    topAnchorProduct = (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->topAnchorQ31;
    topEdge = (FIXED_PRODUCT_SHR(topAnchorProduct, 31)) +
              childNode->topOffset + control->top;
    childNode->top = topEdge;
    childNode->layoutHeight = childNode->layoutHeight - topEdge;
    childNode->vtable->layout(childNode);
  }
}

UiNodeVtable g_UiLayoutContainerControlVtable = {
        .relocate = UI_SLOT(UiLayoutContainerControl_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiContainer_DrawIntersectingChildren),
        .layout = UI_SLOT(UiLayoutContainerControl_LayoutChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiLayoutContainerControl_HitTestChildrenOnly),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiLayoutContainerControl_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiLayoutContainerControl_SuppressActionIdRecursive),
        .unsuppressActionId = UI_SLOT(UiLayoutContainerControl_UnsuppressActionIdRecursive),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

/* Re-enables the controls bound to actionId among firstNode and its following siblings: each node's
   unsuppressActionId method clears UI_NODE_SUPPRESSED when the action matches (containers recurse).
*/
void UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != UI_NODE_NONE; firstNode = firstNode->nextSibling) {
    firstNode->vtable->unsuppressActionId(actionId,firstNode);
  }
}

/* Disables (greys out) the controls bound to actionId among firstNode and its following siblings: each
   node's suppressActionId method sets UI_NODE_SUPPRESSED when the action matches (containers recurse).
*/
void UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != UI_NODE_NONE; firstNode = firstNode->nextSibling) {
    firstNode->vtable->suppressActionId(actionId,firstNode);
  }
}

/* Looks up the page stack's shown page (its first child) in its page array and returns the page's index;
   when the shown page is none of the stack's pages it returns pageCount (1 for an empty stack).
*/
uint32_t UiPageStack_ActivePageIndex(UiPageStackControl *stack)

{
  uint32_t pageIndex;

  /* Page 0 is always compared, even when pageCount is 0 (do/while as in the original). */
  pageIndex = 0;
  do {
    if ((stack->base).firstChild == (&stack->pages)[pageIndex]) break;
    pageIndex++;
  } while (pageIndex < stack->pageCount);
  return pageIndex;
}
