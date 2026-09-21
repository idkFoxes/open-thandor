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
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x005151F0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x00515210 */
void __thandor_preserve_eax
UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x00515230 */
void __thandor_preserve_eax
UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x00515250 */
bool __thandor_cf_preserve_eax_ecx_edx
UiImageActionControl_HandleKeyboardActivationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x005152E0 */
void __thandor_void_preserve_eax_ecx_edx
UiConditionalActionControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x005155A0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005155C0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005155F0 */
void __thandor_preserve_eax
UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B1C80 */
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_BUTTONS_H */
