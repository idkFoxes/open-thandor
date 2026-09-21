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

/* 0x004B49A0 */
void __thandor_void_preserve_eax_ecx_edx
UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B4D40 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B3420 */
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

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
          UiNodeBase *control);

/* 0x004B5770 */
bool __thandor_cf_preserve_eax_ecx_edx
UiResizableWindowControl_HandleWindowHotkeysCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004B1000 */
void __thandor_void_preserve_eax_ecx_edx
UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

/* 0x004B1110 */
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_PopCf(UiRootNode *root);

/* 0x004B2790 */
void __thandor_void_preserve_eax_ecx_edx
UiWindowControl_RelocateWithFrameInset
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B36C0 */
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_LayoutFrameTitleAndChildren(UiNodeBase *control);

/* 0x004B3C00 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildrenOrNoneA
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B3C80 */
void __thandor_void_preserve_eax_ecx_edx
UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B46A0 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B4700 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_LayoutChildren(UiNodeBase *control);

/* 0x004B4790 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildrenOrNoneB
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B47B0 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control);

/* 0x004B4800 */
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control);

/* 0x004B4D10 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B53E0 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

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
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control);

/* 0x004B5120 */
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B5660 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

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
UiContainer_LayoutWithOptionalWindowHeaderOffset(UiNodeBase *control);

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
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileTop
          ,UiPixelCoordinate tileLeft,UiPixelCoordinate tileBottom,UiPixelCoordinate tileRight,
          void *node);

/* 0x004B0510 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B05B0 */
void __thandor_void_preserve_eax_ecx_edx
UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B1350 */
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledVerticalEdge
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,UiPixelCoordinate edgeX,
          UiPixelCoordinate tileStart,UiPixelCoordinate tileEnd,void *node);

/* 0x004B0640 */
void __thandor_void_preserve_eax_ecx_edx UiContainer_LayoutChildren(UiNodeBase *control);

/* 0x004B12F0 */
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileStart,UiPixelCoordinate edgeY,UiPixelCoordinate tileEnd,void *node);

/* 0x004B14D0 */
void __thandor_void_preserve_ecx_edx UiRootStack_InvalidateAll(void);


/* 0x004AF950 */
void __thandor_void_preserve_eax_ecx_edx UiFrame_RunUntilRootClosedAndPresentFinalFrame(void);

#endif /* THANDOR_UI_CONTROLS_LAYOUT_H */
