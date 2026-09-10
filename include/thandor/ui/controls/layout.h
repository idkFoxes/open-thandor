#ifndef THANDOR_UI_CONTROLS_LAYOUT_H
#define THANDOR_UI_CONTROLS_LAYOUT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/layout. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004B49A0 */
void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B4D40 */
void UiResizableWindowControl_DrawFrameTitleAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B3420 */
void UiTitledWindowControl_DrawFrameTitleAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004AF890 */
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void);

/* 0x004AF920 */
void __cdecl UiFrame_ProcessAndPresent(void);

/* 0x004B48D0 */
void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack);

/* 0x004B52D0 */
void __fastcall UiResizableWindowControl_EndMoveResizeAndHandleWindowActions(undefined4 param_1,undefined4 param_2);

/* 0x004B5770 */
void UiResizableWindowControl_HandleWindowHotkeysCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x004B1000 */
void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

/* 0x004B1110 */
void UiRootStack_PopCf(UiRootNode *root);

/* 0x004B2790 */
void UiWindowControl_RelocateWithFrameInset (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B36C0 */
void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiNodeBase *control);

/* 0x004B3C00 */
UiNodeBase * UiContainer_HitTestChildrenOrNoneA (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B3C80 */
undefined8 __fastcall UiHorizontalGaugeControl_DrawFrameFillAndLabel (undefined4 param_1,undefined4 param_2,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight, UiNodeBase *control);

/* 0x004B46A0 */
void UiLayoutContainerControl_RelocateChildren (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B4700 */
void UiLayoutContainerControl_LayoutChildren(UiNodeBase *control);

/* 0x004B4790 */
UiNodeBase * UiContainer_HitTestChildrenOrNoneB (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B47B0 */
void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control);

/* 0x004B4800 */
void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control);

/* 0x004B4D10 */
void UiResizableWindowControl_RelocateAndRefreshInteractionState (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B53E0 */
void UiResizableWindowControl_UpdateMoveOrResize (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004BC660 */
void UiImageControl_LayoutChildrenToParent(UiImageControl *control);

/* 0x004AF3B0 */
void __cdecl UiFrame_FlushInputAndResetPendingTicks(void);

/* 0x004AF9D0 */
void UiRootStack_BringToFront(UiRootNode *root);

/* 0x004B0F30 */
void UiWindowResources_Init(void);

/* 0x004B1240 */
void UiRootStack_Relayout(void);

/* 0x004B3EE0 */
GraphicsCursorFrameIndex UiContainer_PointerMoveReturnCode6 (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B4740 */
void UiLayoutContainerControl_ApplyFlagsRecursive (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control);

/* 0x004B5120 */
void UiResizableWindowControl_BeginMoveResizeOrWindowAction (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B5660 */
GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x00569A80 */
UiGridDimensionsEdxEax8 UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount);

/* 0x00569AE0 */
UiGridDimensionsEdxEax8 UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount);

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
undefined8 UiContainer_LayoutWithOptionalWindowHeaderOffset(UiNodeBase *control);

/* 0x004AF680 */
void UiFrame_Update(UiStopMessageCode stopMessageCode);

/* 0x004AF7E0 */
void UiFrame_Draw(void);

/* 0x004B0800 */
UiNodeBase * UiContainer_HitTestChildren (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B13B0 */
void UiWindow_BlitTiledInterior (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource, UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,UiPixelCoordinate tileBottom, UiPixelCoordinate tileRight,void *node);

/* 0x004B0510 */
void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B05B0 */
void UiContainer_DrawIntersectingChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B1350 */
void UiWindow_BlitTiledVerticalEdge (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource, UiPixelCoordinate edgeX,UiPixelCoordinate tileStart,UiPixelCoordinate tileEnd, void *node);

/* 0x004B0640 */
void UiContainer_LayoutChildren(UiNodeBase *control);

/* 0x004B12F0 */
void UiWindow_BlitTiledHorizontalEdge (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource, UiPixelCoordinate tileStart,UiPixelCoordinate edgeY,UiPixelCoordinate tileEnd, void *node);

/* 0x004B14D0 */
void UiRootStack_InvalidateAll(void);

#endif /* THANDOR_UI_CONTROLS_LAYOUT_H */
