/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/buttons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_BUTTONS_H
#define THANDOR_UI_CONTROLS_BUTTONS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* UiSpriteButtonControl stateFlags bits beyond UiSelectableStateFlags (UiSpriteButtonControl_* functions):
   ANIMATED cycles the frames of the normal/selected range (UiSpriteButtonControl_AdvanceAnimation);
   ACTION_AFTER_ANIMATION defers the action of an activation until the animation reaches its last frame;
   ACTION_PENDING marks such a deferred action. */
inline constexpr int32_t UI_SPRITE_BUTTON_ANIMATED = 0x80;
inline constexpr int32_t UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION = 0x800;
inline constexpr int32_t UI_SPRITE_BUTTON_ACTION_PENDING = 0x1000;
/* Further UiSpriteButtonControl stateFlags bits (UiSpriteButtonControl_DrawClipped, _HitTestOpaque,
   _NonRightPress/_Release/_Drag, _Relocate): RECT_HIT_TEST hit-tests the node rectangle instead of opaque
   sprite pixels; NORMAL_UNDER_SELECTED draws the normal frame below the selected one; SERIALIZED_DESCRIPTOR
   means normalSubresourceStartOrDescriptor still points at the 8-int layout/frame descriptor that
   _Relocate expands; ACTIVATION_SOUND plays activationSound on activation; SELECTED_ONLY draws the button
   only while selected and hit-tests its selected frame. Without ANIMATED, the ACTION_AFTER_ANIMATION bit
   (0x800) instead selects alternateTextureSource for the selected frame. */
inline constexpr int32_t UI_SPRITE_BUTTON_RECT_HIT_TEST = 0x20;
inline constexpr int32_t UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED = 0x40;
inline constexpr int32_t UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR = 0x100;
inline constexpr int32_t UI_SPRITE_BUTTON_ACTIVATION_SOUND = 0x200;
inline constexpr int32_t UI_SPRITE_BUTTON_SELECTED_ONLY = 0x400;
inline constexpr int32_t UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE = 0x800;
/* Colour the sprite shadow is drawn with (ARGB, half-transparent black; UiSpriteButtonControl_DrawClipped). */
inline constexpr int32_t UI_SPRITE_BUTTON_SHADOW_ARGB = 0x7F000000;
/* UiImageActionControl displayFlags (UiImageActionControl_DrawImageAndChildren/_HandleKeyboardActivation). */
inline constexpr int32_t UI_IMAGE_ACTION_STRETCH = 0x1;
inline constexpr int32_t UI_IMAGE_ACTION_KEY_ACTIVATES = 0x2; /* any key but Tab queues primaryActionId */
inline constexpr int32_t UI_IMAGE_ACTION_LETTERBOX = 0x4;
inline constexpr uint32_t UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB = 0xFF000000; /* opaque black */
/* Framed text box pieces in g_UiWindowTextureSource (UiConditionalActionControl_DrawClipped); a 416x58
   box instead uses the single unframed background UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND. */
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT = 0x72;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_TOP_RIGHT = 0x73;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_BOTTOM_LEFT = 0x74;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_BOTTOM_RIGHT = 0x75;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_TOP = 0x76;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_LEFT = 0x77;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_RIGHT = 0x78;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_BOTTOM = 0x79;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_INTERIOR = 0x7B;
inline constexpr int32_t UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND = 200;
/* Size of that unframed box; it shows only the last UI_TEXT_BOX_WIDE_MAX_LINES lines, newest first. */
inline constexpr int32_t UI_TEXT_BOX_WIDE_WIDTH = 416;
inline constexpr int32_t UI_TEXT_BOX_WIDE_HEIGHT = 58;
inline constexpr int32_t UI_TEXT_BOX_WIDE_MAX_LINES = 4;

void UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root);

void UiSpriteButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control);

void UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSpriteButtonControl *control);

void UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

void UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

void UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control);

UiNodeBase * UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control);

void UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageActionControl *control);

GraphicsCursorFrameIndex UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageActionControl *control);

void UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

void UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control);

Bool8 UiImageActionControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiImageActionControl *control);

void UiConditionalActionControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiConditionalActionControl *control);

GraphicsCursorFrameIndex UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

UiNodeBase * UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control);

void UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiConditionalActionControl *control);

void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control);

extern UiNodeVtable g_UiSpriteButtonControlVtable;
extern UiNodeVtable g_UiImageActionControlVtable;
extern UiNodeVtable g_UiConditionalActionControlVtable;

#endif /* THANDOR_UI_CONTROLS_BUTTONS_H */
