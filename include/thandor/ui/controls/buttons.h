/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/buttons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_BUTTONS_H
#define THANDOR_UI_CONTROLS_BUTTONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/buttons. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* UiSpriteButtonControl stateFlags bits beyond UiSelectableStateFlags (UiSpriteButtonControl_* functions):
   ANIMATED cycles the frames of the normal/selected range (UiSpriteButtonControl_AdvanceAnimation);
   ACTION_AFTER_ANIMATION defers the action of an activation until the animation reaches its last frame;
   ACTION_PENDING marks such a deferred action. */
#define UI_SPRITE_BUTTON_ANIMATED 0x80
#define UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION 0x800
#define UI_SPRITE_BUTTON_ACTION_PENDING 0x1000

/* 0x004B1D20 */
void __thandor_void_preserve_eax_ecx_edx UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root);

/* 0x004B1620 */
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control);

/* 0x004B16E0 */
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSpriteButtonControl *control);

/* 0x004B1890 */
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1A30 */
void __thandor_preserve_eax
UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1AE0 */
void __thandor_preserve_eax_edx
UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1BF0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control);

/* 0x00515010 */
void __thandor_void_preserve_eax_ecx_edx
UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImageActionControl *control);

/* 0x005151F0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageActionControl *control);

/* 0x00515210 */
void __thandor_preserve_eax
UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

/* 0x00515230 */
void __thandor_preserve_eax
UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

/* 0x00515250 */
bool __thandor_cf_preserve_eax_ecx_edx
UiImageActionControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiImageActionControl *control);

/* 0x005152E0 */
void __thandor_void_preserve_eax_ecx_edx
UiConditionalActionControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiConditionalActionControl *control);

/* 0x005155A0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

/* 0x005155C0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

/* 0x005155F0 */
void __thandor_preserve_eax
UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiConditionalActionControl *control);

/* 0x004B1C80 */
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_BUTTONS_H */
