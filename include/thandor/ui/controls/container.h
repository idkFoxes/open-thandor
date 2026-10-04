/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/container.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_CONTAINER_H
#define THANDOR_UI_CONTROLS_CONTAINER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/container. */

/* "No node" in the UI tree links (firstChild, nextSibling, parent) and the node-list pointers. The same
   all-bits-set value as UI_TEMPLATE_NO_LINK, which the templates store (0xffffffff in the original). */
#ifndef UI_NODE_NONE
#define UI_NODE_NONE UI_TEMPLATE_NO_LINK
#endif
/* nodeFlags bit 0 (not in the UiNodeFlags enum): set on every node of the top root of the stack by
   UiRootStack_Push/Pop/BringToFront through applyFlags; window frames draw their inactive variant without it. */
#define UI_NODE_IN_FRONT_ROOT 0x01u

/* Functions are grouped by semantic ownership. */

void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack);

void UiLayoutContainerControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control);

void UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control);

UiNodeBase * UiLayoutContainerControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

void UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control);

UiGridDimensions UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount);

UiGridDimensions UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount);

void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control);

void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control);

void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode);

void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root);

void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root);

void UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control);

UiNodeBase * UiContainer_HitTestChildren(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

void UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

void UiContainer_LayoutChildren(UiNodeBase *control);

extern UiNodeVtable g_UiLayoutContainerControlVtable;

#endif /* THANDOR_UI_CONTROLS_CONTAINER_H */
