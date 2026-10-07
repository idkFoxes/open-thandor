/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/command_buttons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_COMMAND_BUTTONS_H
#define THANDOR_UI_INGAME_COMMAND_BUTTONS_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>


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

extern UiNodeVtable g_UiCommandVisibilityWrappedTextVtable;
extern UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable;

#endif /* THANDOR_UI_INGAME_COMMAND_BUTTONS_H */
