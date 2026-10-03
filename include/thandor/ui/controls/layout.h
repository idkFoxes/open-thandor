/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/layout.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_LAYOUT_H
#define THANDOR_UI_CONTROLS_LAYOUT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/layout. */
/* Functions are grouped by semantic ownership. */

/* End marker of the UI root stack: g_UiRootNode holds it when no root is open, and the bottom root's
   previousRoot link holds it. */
#define UI_ROOT_STACK_END ((UiRootNode *)(intptr_t)-1)

/* "No node" in the UI tree links (firstChild, nextSibling, parent) and the node-list pointers. The same
   all-bits-set value as UI_TEMPLATE_NO_LINK, which the templates store (0xffffffff in the original). */
#ifndef UI_NODE_NONE
#define UI_NODE_NONE UI_TEMPLATE_NO_LINK
#endif
/* nodeFlags bit 0 (not in the UiNodeFlags enum): set on every node of the top root of the stack by
   UiRootStack_Push/Pop/BringToFront through applyFlags; window frames draw their inactive variant without it. */
#define UI_NODE_IN_FRONT_ROOT 0x01u

/* UiRootNode.rootFlags of panels (g_UiPanelControlVtable) and resizable windows
   (g_UiResizableWindowControlVtable), from their draw and pointer methods in ui/controls/layout.c.
   0x100 is UI_ROOT_DISABLE_POINTER_HIT_TEST (generated UiRootFlags enum). */
#define UI_ROOT_TILED_BACKGROUND 0x1
#define UI_ROOT_FRAME 0x2
#define UI_ROOT_TITLE_BAR 0x4
#define UI_ROOT_CLOSE_BUTTON 0x8
#define UI_ROOT_MAXIMIZE_BUTTON 0x10
#define UI_ROOT_MOVABLE 0x20
#define UI_ROOT_RESIZABLE 0x40
#define UI_ROOT_MAXIMIZED 0x80
#define UI_ROOT_ALTERNATE_BACKGROUND 0x200 /* panels: second background and frame style */
#define UI_ROOT_CLOSE_PRESSED 0x800 /* the button went down over the close button */
#define UI_ROOT_MAXIMIZE_PRESSED 0x1000
#define UI_ROOT_MOVING 0x2000
#define UI_ROOT_RESIZING 0x4000
#define UI_ROOT_CLOSE_ARMED 0x80000 /* pressed and the pointer is still over the close button */
#define UI_ROOT_MAXIMIZE_ARMED 0x100000
/* While resizing, the top byte holds the grabbed border, clockwise from the top edge; the masks select the
   grabs that move one edge. */
#define UI_ROOT_RESIZE_TOP 0x1000000
#define UI_ROOT_RESIZE_TOP_RIGHT 0x2000000
#define UI_ROOT_RESIZE_RIGHT 0x4000000
#define UI_ROOT_RESIZE_BOTTOM_RIGHT 0x8000000
#define UI_ROOT_RESIZE_BOTTOM 0x10000000
#define UI_ROOT_RESIZE_BOTTOM_LEFT 0x20000000
#define UI_ROOT_RESIZE_LEFT 0x40000000
#define UI_ROOT_RESIZE_TOP_LEFT 0x80000000
#define UI_ROOT_RESIZE_EDGES 0xff000000
#define UI_ROOT_RESIZE_MOVES_TOP 0x83000000
#define UI_ROOT_RESIZE_MOVES_RIGHT 0xe000000
#define UI_ROOT_RESIZE_MOVES_BOTTOM 0x38000000
#define UI_ROOT_RESIZE_MOVES_LEFT 0xe0000000
/* Everything a button release ends: pressed, armed, moving, resizing and the grabbed border */
#define UI_ROOT_POINTER_STATE 0xff187800
/* Smallest width and height a window can be resized to */
#define UI_WINDOW_MINIMUM_SIZE 0x40

/* Action 0 (page 0, handler 0 = UiRootStack_Pop, installed by UiWindowResources_Init): closes the window
   whose node is the source. */
#define UI_ACTION_CLOSE_ROOT 0

/* Pieces of g_UiWindowTextureSource (win.gfx) drawn by the windows, panels and gauges of this file. A frame is
   eight consecutive pieces (see UI_WINDOW_FRAME_PIECE_COUNT); UI_WINDOW_FRAME_* select one of them. */
#define UI_WINDOW_FRAME_TOP_LEFT 0
#define UI_WINDOW_FRAME_TOP_RIGHT 1
#define UI_WINDOW_FRAME_BOTTOM_LEFT 2
#define UI_WINDOW_FRAME_BOTTOM_RIGHT 3
#define UI_WINDOW_FRAME_TOP 4
#define UI_WINDOW_FRAME_LEFT 5
#define UI_WINDOW_FRAME_RIGHT 6
#define UI_WINDOW_FRAME_BOTTOM 7
#define UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR 0
#define UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON 1
#define UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_INACTIVE 2 /* window not in the front root */
#define UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_ARMED 3
#define UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON 4
#define UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_INACTIVE 5
#define UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_ARMED 6
#define UI_WINDOW_RESTORE_BUTTON_OFFSET 3 /* a maximized window shows the restore button (7..9) instead */
/* Title bar: left cap, then +UI_WINDOW_TITLE_BAR_MIDDLE the tiled middle and +UI_WINDOW_TITLE_BAR_RIGHT the
   right cap; the middle piece of the active bar gives the title bar height. */
#define UI_WINDOW_SUBRESOURCE_TITLE_BAR 0xA
#define UI_WINDOW_SUBRESOURCE_TITLE_BAR_INACTIVE 0xB
#define UI_WINDOW_TITLE_BAR_MIDDLE 2
#define UI_WINDOW_TITLE_BAR_RIGHT 4
#define UI_WINDOW_SUBRESOURCE_WINDOW_FRAME 0x10
#define UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME 0x24 /* UI_ROOT_ALTERNATE_BACKGROUND */
#define UI_WINDOW_SUBRESOURCE_ALTERNATE_INTERIOR 0x2C
#define UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME 0x36 /* UiTitledWindowControl */
#define UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT 0x3E /* caps left and right of the title text */
#define UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT 0x3F
#define UI_WINDOW_SUBRESOURCE_GAUGE_LEFT 0x7C /* UiHorizontalGaugeControl: track caps and tiled middle */
#define UI_WINDOW_SUBRESOURCE_GAUGE_TRACK 0x7D
#define UI_WINDOW_SUBRESOURCE_GAUGE_RIGHT 0x7E
#define UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT 0x7F /* fill caps and tiled middle */
#define UI_WINDOW_SUBRESOURCE_GAUGE_FILL 0x80
#define UI_WINDOW_SUBRESOURCE_GAUGE_FILL_RIGHT 0x81
/* UiTitledWindowControl.titleFlags and UiHorizontalGaugeControl.gaugeFlags bits. */
#define UI_TITLED_WINDOW_CENTERED_TITLE 0x1
#define UI_HORIZONTAL_GAUGE_SHOW_PERCENT 0x1
/* UiFrame_Update calls between two DirectInputMouse_RefreshDeviceIfIdle calls (g_DirectInputMouseRefreshCountdown). */
#define UI_FRAME_DIRECT_INPUT_REFRESH_INTERVAL 48

void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control);

void UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control);

void UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control);

void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void);

void UiFrame_ProcessAndPresent(void);

void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack);

void UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

bool UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control);

void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

bool UiRootStack_Pop(UiRootNode *root);

void UiWindowControl_RelocateWithFrameInset(UiSerializedRelocationDelta relocationDelta,UiWindowControl *control);

void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control);

UiNodeBase * UiFillPanelControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control);

void UiLayoutContainerControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control);

void UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control);

UiNodeBase * UiLayoutContainerControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

void UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control);

void UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

void UiImageControl_LayoutChildrenToParent(UiImageControl *control);

void UiFrame_FlushInputAndResetPendingTicks(void);

bool UiRootStack_BringToFront(UiRootNode *root);

void UiWindowResources_Init(void);

void UiRootStack_Relayout(void);

GraphicsCursorFrameIndex UiHorizontalGaugeControl_PointerMoveBusyCursor
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control);

void UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control);

UiGridDimensions UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount);

UiGridDimensions UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount);

void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control);

void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control);

void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode);

void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root);

void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root);

void UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control);

void UiFrame_Update(UiStopMessageCode stopMessageCode);

void UiFrame_Draw(void);

UiNodeBase * UiContainer_HitTestChildren(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiWindow_BlitTiledInterior(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom
          ,UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node);

void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

void UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

void UiWindow_BlitTiledVerticalEdge(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

void UiContainer_LayoutChildren(UiNodeBase *control);

void UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

void UiRootStack_InvalidateAll(void);

extern UiNodeVtable g_UiTitledWindowControlVtable;
extern UiNodeVtable g_UiFillPanelControlVtable;
extern UiNodeVtable g_UiHorizontalGaugeControlVtable;
extern UiNodeVtable g_UiLayoutContainerControlVtable;
extern UiNodeVtable g_UiPanelControlVtable;
extern UiNodeVtable g_UiResizableWindowControlVtable;

extern uint32_t g_UiPendingFrameTicks;
extern uint32_t g_UiInvalidationSuppressed;
extern UiRootNode *g_UiRootNode;
extern GraphicsTextureSourceAsset *g_UiWindowTextureSource;
extern GraphicsTextureSourceAsset *g_UiWindowClassTextureSource;

#endif /* THANDOR_UI_CONTROLS_LAYOUT_H */
