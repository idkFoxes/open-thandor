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
/* Further UiSpriteButtonControl stateFlags bits (UiSpriteButtonControl_DrawClipped, _HitTestOpaque,
   _NonRightPress/_Release/_Drag, _Relocate): RECT_HIT_TEST hit-tests the node rectangle instead of opaque
   sprite pixels; NORMAL_UNDER_SELECTED draws the normal frame below the selected one; SERIALIZED_DESCRIPTOR
   means normalSubresourceStartOrDescriptor still points at the 8-int layout/frame descriptor that
   _Relocate expands; ACTIVATION_SOUND plays activationSoundId on activation; SELECTED_ONLY draws the button
   only while selected and hit-tests its selected frame. Without ANIMATED, the ACTION_AFTER_ANIMATION bit
   (0x800) instead selects alternateTextureSource for the selected frame. */
#define UI_SPRITE_BUTTON_RECT_HIT_TEST 0x20
#define UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED 0x40
#define UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR 0x100
#define UI_SPRITE_BUTTON_ACTIVATION_SOUND 0x200
#define UI_SPRITE_BUTTON_SELECTED_ONLY 0x400
#define UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE 0x800
/* Colour the sprite shadow is drawn with (ARGB, half-transparent black; UiSpriteButtonControl_DrawClipped). */
#define UI_SPRITE_BUTTON_SHADOW_ARGB 0x7F000000
/* UiImageActionControl displayFlags (UiImageActionControl_DrawImageAndChildren/_HandleKeyboardActivation). */
#define UI_IMAGE_ACTION_STRETCH 0x1
#define UI_IMAGE_ACTION_KEY_ACTIVATES 0x2 /* any key but Tab queues primaryActionId */
#define UI_IMAGE_ACTION_LETTERBOX 0x4
#define UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB 0xFF000000 /* opaque black */
/* Framed text box pieces in g_UiWindowTextureSource (UiConditionalActionControl_DrawClipped); a 416x58
   box instead uses the single unframed background UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND. */
#define UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT 0x72
#define UI_TEXT_BOX_SUBRESOURCE_TOP_RIGHT 0x73
#define UI_TEXT_BOX_SUBRESOURCE_BOTTOM_LEFT 0x74
#define UI_TEXT_BOX_SUBRESOURCE_BOTTOM_RIGHT 0x75
#define UI_TEXT_BOX_SUBRESOURCE_TOP 0x76
#define UI_TEXT_BOX_SUBRESOURCE_LEFT 0x77
#define UI_TEXT_BOX_SUBRESOURCE_RIGHT 0x78
#define UI_TEXT_BOX_SUBRESOURCE_BOTTOM 0x79
#define UI_TEXT_BOX_SUBRESOURCE_INTERIOR 0x7B
#define UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND 200
/* Size of that unframed box; it shows only the last UI_TEXT_BOX_WIDE_MAX_LINES lines, newest first. */
#define UI_TEXT_BOX_WIDE_WIDTH 416
#define UI_TEXT_BOX_WIDE_HEIGHT 58
#define UI_TEXT_BOX_WIDE_MAX_LINES 4

/* 0x004B1D20 */
void UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root);

/* 0x004B1620 */
void UiSpriteButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control);

/* 0x004B16E0 */
void UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSpriteButtonControl *control);

/* 0x004B1890 */
void UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1A30 */
void UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1AE0 */
void UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

/* 0x004B1BF0 */
UiNodeBase * UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control);

/* 0x00515010 */
void UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageActionControl *control);

/* 0x005151F0 */
GraphicsCursorFrameIndex UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageActionControl *control);

/* 0x00515210 */
void UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

/* 0x00515230 */
void UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

/* 0x00515250 */
bool UiImageActionControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiImageActionControl *control);

/* 0x005152E0 */
void UiConditionalActionControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiConditionalActionControl *control);

/* 0x005155A0 */
GraphicsCursorFrameIndex UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

/* 0x005155C0 */
UiNodeBase * UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

/* 0x005155F0 */
void UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiConditionalActionControl *control);

/* 0x004B1C80 */
void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_BUTTONS_H */
