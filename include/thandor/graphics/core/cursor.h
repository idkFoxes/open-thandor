/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/cursor.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_CORE_CURSOR_H
#define THANDOR_GRAPHICS_CORE_CURSOR_H

#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Frames for g_GraphicsCursorSetFrame (GraphicsCursor_SetFrameIndex). */
inline constexpr int GRAPHICS_CURSOR_FRAME_ARROW = 0;
inline constexpr int GRAPHICS_CURSOR_FRAME_BUSY = 6; /* shown while something loads (credits, session start, savegame list) */
/* Move and resize cursors of the resizable windows (UiResizableWindowControl_QueryResizeCursorCode, named
   after the Win32 IDC_SIZE* cursors they stand for) */
inline constexpr int GRAPHICS_CURSOR_FRAME_MOVE = 1;
inline constexpr int GRAPHICS_CURSOR_FRAME_SIZE_NWSE = 2; /* top-left and bottom-right corner */
inline constexpr int GRAPHICS_CURSOR_FRAME_SIZE_NESW = 3; /* top-right and bottom-left corner */
inline constexpr int GRAPHICS_CURSOR_FRAME_SIZE_NS = 4; /* top and bottom edge */
inline constexpr int GRAPHICS_CURSOR_FRAME_SIZE_WE = 5; /* left and right edge */
/* GraphicsCursor_ConsumeNextInputEvent: bit 31 of a press's returned button state marks a double click (the
   same bit as UI_POINTER_BUTTON_REPEAT_CLICK): the press comes less than 16 clock ticks after the release of
   the same button and within +-4 pixels of the previous press. */
inline constexpr uint32_t GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK = 0x80000000u;
inline constexpr uint32_t GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS = 16u;
inline constexpr int GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE = 4;
/* Entries of the g_CursorInputEvents ring (read by GraphicsCursor_ConsumeNextInputEvent). */
inline constexpr int GRAPHICS_CURSOR_INPUT_EVENT_CAPACITY = 256;

extern GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame;

extern int32_t g_CursorCurrentVisibilityToken;

extern SoftwareFramebufferAccess *g_CursorAlternateSavedBackground;

extern GraphicsCursorInputEvent18 g_CursorInputEvents[256];

extern uint32_t g_CursorInputReadIndex;

extern uint32_t g_CursorInputClockValue;

extern GraphicsTextureSourceAsset *g_CursorSourceAsset;

extern GraphicsCursorFrameRecord *g_CursorFrameRecords;

extern GraphicsCursorFrameCount g_CursorFrameCount;

extern GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent;

extern SoftwareFramebufferAccess *g_CursorSavedBackground;

extern SoftwareFramebufferAccess *g_CursorCompositeBuffer;

void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer();

Bool8 GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex);

GraphicsCursorFrameIndex GraphicsCursor_GetFrameIndex();

Bool8 GraphicsCursor_ConsumeNextInputEvent(CursorPointerEvent *outEvent);

#endif /* THANDOR_GRAPHICS_CORE_CURSOR_H */
