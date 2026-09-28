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

/* 0x004B49A0 */
void __thandor_void_preserve_eax_ecx_edx
UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control);

/* 0x004B4D40 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control);

/* 0x004B3420 */
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control);

/* 0x004AF890 */
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void);

/* 0x004AF920 */
void __thandor_void_preserve_eax_ecx_edx UiFrame_ProcessAndPresent(void);

/* 0x004B48D0 */
void __thandor_void_preserve_eax_ecx_edx
UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack);

/* 0x004B52D0 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004B5770 */
bool __thandor_cf_preserve_eax_ecx_edx
UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control);

/* 0x004B1000 */
void __thandor_void_preserve_eax_ecx_edx
UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

/* 0x004B1110 */
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_Pop(UiRootNode *root);

/* 0x004B2790 */
void __thandor_void_preserve_eax_ecx_edx
UiWindowControl_RelocateWithFrameInset
          (UiSerializedRelocationDelta relocationDelta,UiWindowControl *control);

/* 0x004B36C0 */
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control);

/* 0x004B3C00 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildrenOrNoneA
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B3C80 */
void __thandor_void_preserve_eax_ecx_edx
UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control);

/* 0x004B46A0 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control);

/* 0x004B4700 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control);

/* 0x004B4790 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildrenOrNoneB
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B47B0 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

/* 0x004B4800 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control);

/* 0x004B4D10 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control);

/* 0x004B53E0 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004BC660 */
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_LayoutChildrenToParent(UiImageControl *control);

/* 0x004AF3B0 */
void __thandor_void_preserve_eax_ecx_edx UiFrame_FlushInputAndResetPendingTicks(void);

/* 0x004AF9D0 */
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_BringToFront(UiRootNode *root);

/* 0x004B0F30 */
void __thandor_preserve_eax UiWindowResources_Init(void);

/* 0x004B1240 */
void __thandor_preserve_eax_edx UiRootStack_Relayout(void);

/* 0x004B3EE0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiContainer_PointerMoveReturnCode6
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B4740 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control);

/* 0x004B5120 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

/* 0x004B5660 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control);

/* 0x00569A80 */
UiGridDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx
UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount);

/* 0x00569AE0 */
UiGridDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx
UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount);

/* 0x004B0940 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004B0990 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004B1420 */
void __thandor_void_preserve_eax_ecx
UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode);

/* 0x004B4850 */
void __thandor_void_preserve_eax_ecx_edx
UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root);

/* 0x004B4890 */
void __thandor_void_preserve_eax_ecx_edx UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root);

/* 0x004B50D0 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control);

/* 0x004AF680 */
void __thandor_void_preserve_eax_ecx UiFrame_Update(UiStopMessageCode stopMessageCode);

/* 0x004AF7E0 */
void __thandor_void_preserve_eax_ecx_edx UiFrame_Draw(void);

/* 0x004B0800 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildren
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B13B0 */
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledInterior
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom
          ,UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node);

/* 0x004B0510 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B05B0 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

/* 0x004B1350 */
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledVerticalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

/* 0x004B0640 */
void __thandor_void_preserve_eax_ecx_edx UiContainer_LayoutChildren(UiNodeBase *control);

/* 0x004B12F0 */
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

/* 0x004B14D0 */
void __thandor_void_preserve_ecx_edx UiRootStack_InvalidateAll(void);


/* 0x004AF950 */
void __thandor_void_preserve_eax_ecx_edx UiFrame_RunUntilRootClosedAndPresentFinalFrame(void);

#endif /* THANDOR_UI_CONTROLS_LAYOUT_H */
