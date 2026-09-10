#ifndef THANDOR_UI_CONTROLS_BUTTONS_H
#define THANDOR_UI_CONTROLS_BUTTONS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/buttons. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004B1D20 */
void UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root);

/* 0x004B1620 */
void UiSpriteButtonControl_Relocate (UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control);

/* 0x004B16E0 */
void UiSpriteButtonControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiSpriteButtonControl *control);

/* 0x004B1890 */
void UiSpriteButtonControl_NonRightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiSpriteButtonControl *control);

/* 0x004B1A30 */
void UiSpriteButtonControl_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiSpriteButtonControl *control);

/* 0x004B1AE0 */
void UiSpriteButtonControl_NonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiSpriteButtonControl *control);

/* 0x004B1BF0 */
UiNodeBase * UiSpriteButtonControl_HitTestOpaque (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control);

/* 0x00515010 */
void UiImageActionControl_DrawImageAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x005151F0 */
GraphicsCursorFrameIndex UiImageActionControl_QueryPointerCode (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x00515210 */
void UiImageActionControl_EnqueuePrimaryAction(void);

/* 0x00515230 */
void UiImageActionControl_EnqueueSecondaryAction(void);

/* 0x00515250 */
void UiImageActionControl_HandleKeyboardActivationCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x005152E0 */
void UiConditionalActionControl_DrawClipped (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x005155A0 */
GraphicsCursorFrameIndex UiConditionalActionControl_QueryPointerCode (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005155C0 */
UiNodeBase * UiConditionalActionControl_HitTestWhenEnabled (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005155F0 */
void UiConditionalActionControl_EnqueuePrimaryActionIfEnabled(void);

/* 0x004B1C80 */
void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_BUTTONS_H */
