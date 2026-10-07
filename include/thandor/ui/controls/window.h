/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/window.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_WINDOW_H
#define THANDOR_UI_CONTROLS_WINDOW_H

#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWindowControl *control);

extern UiNodeVtable g_UiWindowControlVtable;

/* The UiRootNode.rootFlags bits, UI_ROOT_*, are the enum class UiRootFlags in ui/controls/types.h. */

/* Smallest width and height a window can be resized to */
inline constexpr int32_t UI_WINDOW_MINIMUM_SIZE = 0x40;

/* Action 0 (page 0, handler 0 = UiRootStack_Pop, installed by UiWindowResources_Init): closes the window
   whose node is the source. */
inline constexpr int32_t UI_ACTION_CLOSE_ROOT = 0;

/* Pieces of g_UiWindowTextureSource (win.gfx) drawn by the windows, panels and gauges of this file. A frame is
   eight consecutive pieces (see UI_WINDOW_FRAME_PIECE_COUNT); UI_WINDOW_FRAME_* select one of them. */
inline constexpr int32_t UI_WINDOW_FRAME_TOP_LEFT = 0;
inline constexpr int32_t UI_WINDOW_FRAME_TOP_RIGHT = 1;
inline constexpr int32_t UI_WINDOW_FRAME_BOTTOM_LEFT = 2;
inline constexpr int32_t UI_WINDOW_FRAME_BOTTOM_RIGHT = 3;
inline constexpr int32_t UI_WINDOW_FRAME_TOP = 4;
inline constexpr int32_t UI_WINDOW_FRAME_LEFT = 5;
inline constexpr int32_t UI_WINDOW_FRAME_RIGHT = 6;
inline constexpr int32_t UI_WINDOW_FRAME_BOTTOM = 7;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR = 0;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON = 1;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_INACTIVE = 2; /* window not in the front root */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_ARMED = 3;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON = 4;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_INACTIVE = 5;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_ARMED = 6;
inline constexpr int32_t UI_WINDOW_RESTORE_BUTTON_OFFSET = 3; /* a maximized window shows the restore button (7..9) instead */

/* Title bar: left cap, then +UI_WINDOW_TITLE_BAR_MIDDLE the tiled middle and +UI_WINDOW_TITLE_BAR_RIGHT the
   right cap; the middle piece of the active bar gives the title bar height. */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_TITLE_BAR = 0xA;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_TITLE_BAR_INACTIVE = 0xB;
inline constexpr int32_t UI_WINDOW_TITLE_BAR_MIDDLE = 2;
inline constexpr int32_t UI_WINDOW_TITLE_BAR_RIGHT = 4;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_WINDOW_FRAME = 0x10;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME = 0x24; /* UI_ROOT_ALTERNATE_BACKGROUND */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ALTERNATE_INTERIOR = 0x2C;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME = 0x36; /* UiTitledWindowControl */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT = 0x3E; /* caps left and right of the title text */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT = 0x3F;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_LEFT = 0x7C; /* UiHorizontalGaugeControl: track caps and tiled middle */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_TRACK = 0x7D;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_RIGHT = 0x7E;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT = 0x7F; /* fill caps and tiled middle */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_FILL = 0x80;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_GAUGE_FILL_RIGHT = 0x81;

/* UiTitledWindowControl.titleFlags and UiHorizontalGaugeControl.gaugeFlags bits. */
inline constexpr int32_t UI_TITLED_WINDOW_CENTERED_TITLE = 0x1;
inline constexpr int32_t UI_HORIZONTAL_GAUGE_SHOW_PERCENT = 0x1;

void UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control);

void UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control);

void UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

Bool8 UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control);

void UiWindowControl_RelocateWithFrameInset(UiSerializedRelocationDelta relocationDelta,UiWindowControl *control);

void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control);

void UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control);

void UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

void UiWindowResources_Init();

void UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control);

GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control);

void UiWindow_BlitTiledInterior(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom
          ,UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node);

void UiWindow_BlitTiledVerticalEdge(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

void UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node);

extern UiNodeVtable g_UiTitledWindowControlVtable;
extern UiNodeVtable g_UiResizableWindowControlVtable;
extern GraphicsTextureSourceAsset *g_UiWindowTextureSource;
extern GraphicsTextureSourceAsset *g_UiWindowClassTextureSource;

#endif /* THANDOR_UI_CONTROLS_WINDOW_H */
