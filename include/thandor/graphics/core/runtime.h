/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_CORE_RUNTIME_H
#define THANDOR_GRAPHICS_CORE_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/core/runtime. */

/* Capacity of g_GraphicsAdapters (Graphics_Init allocates 16 records of 0x80 bytes) and of g_GraphicsDisplayModes
   (the enumeration callbacks stop at 256 modes). */
#define GRAPHICS_ADAPTER_CAPACITY 16
#define GRAPHICS_DISPLAY_MODE_CAPACITY 256
/* g_ActiveGraphicsAdapterIndex before the first display mode is set (and while the backend is being recreated). */
#define GRAPHICS_ADAPTER_INDEX_NONE (-1)

/* Frames for g_GraphicsCursorSetFrame (GraphicsCursor_SetFrameIndex). */
#define GRAPHICS_CURSOR_FRAME_ARROW 0
#define GRAPHICS_CURSOR_FRAME_BUSY 6 /* shown while something loads (credits, session start, savegame list) */
/* Move and resize cursors of the resizable windows (UiResizableWindowControl_QueryResizeCursorCode, named
   after the Win32 IDC_SIZE* cursors they stand for) */
#define GRAPHICS_CURSOR_FRAME_MOVE 1
#define GRAPHICS_CURSOR_FRAME_SIZE_NWSE 2 /* top-left and bottom-right corner */
#define GRAPHICS_CURSOR_FRAME_SIZE_NESW 3 /* top-right and bottom-left corner */
#define GRAPHICS_CURSOR_FRAME_SIZE_NS 4 /* top and bottom edge */
#define GRAPHICS_CURSOR_FRAME_SIZE_WE 5 /* left and right edge */

/* GraphicsCursor_ConsumeNextInputEvent: bit 31 of a press's returned button state marks a double click (the
   same bit as UI_POINTER_BUTTON_REPEAT_CLICK): the press comes less than 16 clock ticks after the release of
   the same button and within +-4 pixels of the previous press. */
#define GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK 0x80000000u
#define GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS 16u
#define GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE 4
/* Entries of the g_CursorInputEvents ring (read by GraphicsCursor_ConsumeNextInputEvent). */
#define GRAPHICS_CURSOR_INPUT_EVENT_CAPACITY 256

/* Pixel formats shared by the graphics backends, textures and movies. ARGB8888 is the engine's 32-bit colour
   (gfx assets, palette entries, vertex colours): alpha in the top byte, blue in the low byte. */
#define ARGB8888_ALPHA_MASK 0xff000000
#define ARGB8888_RED_MASK 0xff0000
#define ARGB8888_GREEN_MASK 0xff00
#define ARGB8888_BLUE_MASK 0xff
#define ARGB8888_RGB_MASK 0xffffff
#define ARGB8888_OPAQUE_WHITE 0xffffffff
#define ARGB8888_CHANNEL_MASK 0xff /* one channel shifted down to bit 0 */
#define ARGB8888_CHANNEL_MAX 0xff /* one 8-bit channel at full intensity */
#define ARGB8888_CHANNEL_ONES 0x1010101 /* 1 in each of the four byte channels */
#define ARGB8888_ALPHA_ONE 0x1000000 /* alpha 1, the lowest alpha step */
/* c * 0x101 = c | c << 8: an 8-bit channel widened to a 16-bit lane (PUNPCKLBW of a value with itself) */
#define COLOR_CHANNEL_TO_WORD_LANE 0x101
/* x86 shifts use only the low 5 bits of the count. The original masks some counts explicitly; the C keeps the
   mask where dropping it changes the generated code. */
#ifndef SHIFT_COUNT_MASK
#define SHIFT_COUNT_MASK 0x1f
#endif
/* A dword count derived from its byte count ((count * 4) >> 2, as the original's dword fill/copy loops count): the
   top two bits drop */
#define DWORD_COUNT_MASK 0x3fffffff

/* Functions are grouped by semantic ownership. */

void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void);

bool GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex);

bool GraphicsCursor_ConsumeNextInputEvent(CursorPointerEvent *outEvent);

GraphicsProjectedPointPair Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint);

void Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX);

void Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX);

void Graphics_SetProjectionViewport(GraphicsScreenCoordinate bottom,GraphicsScreenCoordinate right,
          GraphicsScreenCoordinate top,GraphicsScreenCoordinate left);

void Graphics_SetAuxiliaryOrientation(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

void Graphics_SetSceneBoundsAndColors(GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
          GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4,
          GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2,
          GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0);

void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue);

void Graphics_RebuildFrustumPlanes(void);

uint32_t __cdecl Graphics_Init(void);

void Graphics_Shutdown(void);

void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface);

void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface);

void GraphicsCursor_SaveSurfaceBackground(SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface);

void GraphicsCursor_RestoreSurfaceBackground(SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface);

extern GraphicsCursorInputEvent18 g_CursorInputEvents[256];
extern uint32_t g_CursorInputReadIndex;
extern uint32_t g_CursorInputClockValue;
extern GraphicsTextureSourceAsset *g_CursorSourceAsset;
extern GraphicsCursorFrameRecord *g_CursorFrameRecords;
extern GraphicsCursorFrameCount g_CursorFrameCount;
extern GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent;
extern GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed;
extern GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4];
extern GraphicsSceneBounds8 g_SceneBoundsFixed;
extern SoftwareFramebufferAccess *g_CursorSavedBackground;
extern SoftwareFramebufferAccess *g_CursorCompositeBuffer;
extern DirectDrawCreate *pDirectDrawCreate;
extern uint32_t g_MouseEventsProcessed;

extern SoftwareFramebufferAccess *g_CursorAlternateSavedBackground;

extern SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize;
extern int32_t g_CursorCurrentVisibilityToken;
extern int32_t g_GraphicsBackendAccessState;

extern int32_t g_ProjectionScaleFixed;
extern GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed;
extern GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed;

extern GraphicsFixedVec3 g_ViewOriginFixed;

extern GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame;

#endif /* THANDOR_GRAPHICS_CORE_RUNTIME_H */
