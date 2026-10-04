/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/command_buttons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_COMMAND_BUTTONS_H
#define THANDOR_UI_INGAME_COMMAND_BUTTONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/command_buttons. */

/* g_UiCommandRuntimeFlags bit hiding the world view status texts (UiCommandVisibility*Text_DrawWhenAllowed); no
   writer with a constant mask, so it can only come from command 0x310 */
#define UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS 0x200

/* Functions are grouped by semantic ownership. */

void UiCommandSpriteButtonControl_BeginPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

void UiCommandSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

void UiCommandSpriteButtonControl_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

void UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

void UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control);

extern UiNodeVtable g_UiCommandSpriteButtonWithDetailsVtable;
extern UiNodeVtable g_UiCommandSpriteButtonControlVtable;

#endif /* THANDOR_UI_INGAME_COMMAND_BUTTONS_H */
