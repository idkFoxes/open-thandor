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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* End marker of the UI root stack: g_UiRootNode holds it when no root is open, and the bottom root's
   previousRoot link holds it. */
#define UI_ROOT_STACK_END ((UiRootNode *)0xffffffff)

/* "No node" in the UI tree links (firstChild, nextSibling, parent) and the node-list pointers. */
#ifndef UI_NODE_NONE
#define UI_NODE_NONE ((UiNodeBase *)0xffffffff)
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

/* 0x004B49A0 */
void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control);

/* 0x004B4D40 */
void UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control);

/* 0x004B3420 */
void UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control);

/* 0x004AF890 */
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void);

/* 0x004AF920 */
void UiFrame_ProcessAndPresent(void);

/* 0x004B48D0 */
void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack);

/* 0x004B52D0 */
void UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004B5770 */
bool UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control);

/* 0x004B1000 */
void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

/* 0x004B1110 */
bool UiRootStack_Pop(UiRootNode *root);

/* 0x004B2790 */
void UiWindowControl_RelocateWithFrameInset(UiSerializedRelocationDelta relocationDelta,UiWindowControl *control);

/* 0x004B36C0 */
void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control);

/* 0x004B3C00 */
UiNodeBase * UiFillPanelControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B3C80 */
void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control);

/* 0x004B46A0 */
void UiLayoutContainerControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control);

/* 0x004B4700 */
void UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control);

/* 0x004B4790 */
UiNodeBase * UiLayoutContainerControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B47B0 */
void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

/* 0x004B4800 */
void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

/* 0x004B4D10 */
void UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control);

/* 0x004B53E0 */
void UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004BC660 */
void UiImageControl_LayoutChildrenToParent(UiImageControl *control);

/* 0x004AF3B0 */
void UiFrame_FlushInputAndResetPendingTicks(void);

/* 0x004AF9D0 */
bool UiRootStack_BringToFront(UiRootNode *root);

/* 0x004B0F30 */
void UiWindowResources_Init(void);

/* 0x004B1240 */
void UiRootStack_Relayout(void);

/* 0x004B3EE0 */
GraphicsCursorFrameIndex UiHorizontalGaugeControl_PointerMoveBusyCursor
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B4740 */
void UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control);

/* 0x004B5120 */
void UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004B5660 */
GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control);

/* 0x00569A80 */
UiGridDimensions UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount);

/* 0x00569AE0 */
UiGridDimensions UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount);

/* 0x004B0940 */
void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004B0990 */
void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004B1420 */
void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode);

/* 0x004B4850 */
void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root);

/* 0x004B4890 */
void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root);

/* 0x004B50D0 */
void UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control);

/* 0x004AF680 */
void UiFrame_Update(UiStopMessageCode stopMessageCode);

/* 0x004AF7E0 */
void UiFrame_Draw(void);

/* 0x004B0800 */
UiNodeBase * UiContainer_HitTestChildren(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B13B0 */
void UiWindow_BlitTiledInterior(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom
          ,UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node);

/* 0x004B0510 */
void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B05B0 */
void UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

/* 0x004B1350 */
void UiWindow_BlitTiledVerticalEdge(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

/* 0x004B0640 */
void UiContainer_LayoutChildren(UiNodeBase *control);

/* 0x004B12F0 */
void UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

/* 0x004B14D0 */
void UiRootStack_InvalidateAll(void);


/* 0x004AF950 */
void UiFrame_RunUntilRootClosedAndPresentFinalFrame(void);

#endif /* THANDOR_UI_CONTROLS_LAYOUT_H */
