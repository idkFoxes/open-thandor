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

/* A node of the graphics-object transform hierarchy (GraphicsObject_* in graphics/core/runtime.c, which take
   it as a GraphicsObjectAddress32). Only the fields those functions touch are named; the size is not known. */
typedef struct GraphicsObject {
    uint8_t unknown00_0B[12];
    int childCount;                                /* +0x0C */
    GraphicsFixedMatrix3x4 worldTransform;         /* +0x10 */
    FixedMathScale32 translationDistance;          /* +0x40 */
    uint32_t translationAnglesPacked;              /* +0x44 azimuth (low word) | elevation << 16 */
    AngleTurn32 rotationAzimuth;                   /* +0x48 */
    uint32_t rotationAnglesPacked;                 /* +0x4C elevation (low word) | roll << 16 */
    uint8_t unknown50_63[20];
    GraphicsObjectAddress32 parentObject;          /* +0x64, 0 for a root object */
    uint8_t unknown68_77[16];
    GraphicsObjectAddress32 childObjects[1];       /* +0x78 */
} GraphicsObject;

/* GraphicsAdapterRecord.adapterGuid.Data1 of the 3dfx Glide adapter (Glide3_InitAndEnumerate); DirectDraw
   adapters carry their real GUID, the primary display driver an all-zero one (passed as NULL). */
#define GRAPHICS_ADAPTER_GUID_GLIDE 1
/* Capacity of g_GraphicsAdapters (Graphics_Init allocates 16 records of 0x80 bytes) and of g_GraphicsDisplayModes
   (the enumeration callbacks stop at 256 modes). */
#define GRAPHICS_ADAPTER_CAPACITY 16
#define GRAPHICS_DISPLAY_MODE_CAPACITY 256
/* g_ActiveGraphicsAdapterIndex before the first display mode is set (and while the backend is being recreated). */
#define GRAPHICS_ADAPTER_INDEX_NONE (-1)
/* GraphicsAdapterRecord.deviceGuid.Data1 selects the renderer of the adapter: no Direct3D device (the software
   rasterizer), the Glide adapter (set by Glide3_InitAndEnumerate), otherwise the GUID of a Direct3D device. */
#define GRAPHICS_DEVICE_GUID_SOFTWARE 0
#define GRAPHICS_DEVICE_GUID_GLIDE 1

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
/* 16-bit RGB565 (the Glide and most DirectDraw 16-bit modes) */
#define RGB565_RED_MASK 0xf800
#define RGB565_GREEN_MASK 0x7e0
#define RGB565_BLUE_MASK 0x1f
/* 16-bit ARGB4444 (Glide textures with alpha) */
#define ARGB4444_ALPHA_MASK 0xf000
#define ARGB4444_RGB_MASK 0xfff
/* x86 shifts use only the low 5 bits of the count. The original masks some counts explicitly; the C keeps the
   mask where dropping it changes the generated code. */
#ifndef SHIFT_COUNT_MASK
#define SHIFT_COUNT_MASK 0x1f
#endif
/* A dword count derived from its byte count ((count * 4) >> 2, the original's REP STOSD/MOVSD count): the
   top two bits drop */
#define DWORD_COUNT_MASK 0x3fffffff

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00576C30 */
void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void);

/* 0x004168B0 */
bool GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex);

/* 0x004168E0 */
bool GraphicsCursor_ConsumeNextInputEvent(CursorPointerEvent *outEvent);

/* 0x00486430 */
GraphicsProjectedPointPair Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint);

/* 0x00486490 */
void Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX);

/* 0x004864D0 */
void Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX);

/* 0x00486640 */
void Graphics_SetProjectionViewport(GraphicsScreenCoordinate bottom,GraphicsScreenCoordinate right,
          GraphicsScreenCoordinate top,GraphicsScreenCoordinate left);

/* 0x004866C0 */
void Graphics_SetAuxiliaryOrientation(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00486730 */
void Graphics_SetSceneBoundsAndColors(GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
          GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4,
          GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2,
          GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0);

/* 0x00486790 */
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue);

/* 0x004867B0 */
void Graphics_RebuildFrustumPlanes(void);

/* 0x004A9100 */
void GraphicsBackend_RefreshActiveAdapterNoOp(void);

/* 0x004BCFE0 */
FixedRollAzimuthElevation
GraphicsObject_ExtractTransformEulerAngles(GraphicsObjectAddress32 graphicsObject);

/* 0x004BD000 */
FixedElevationAzimuth GraphicsObject_ConvertWorldDirectionAnglesToLocalAngles
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          GraphicsObjectAddress32 graphicsObject);

/* 0x004BD050 */
void GraphicsObject_SetTranslationDirectionPackedAnglesAndScale
          (AngleTurn16Stored32 elevationAngle16,AngleTurn16Stored32 azimuthAngle16,
          FixedMathScale32 distance,GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x004BD080 */
void GraphicsObject_SetRotationEulerAnglesPacked(AngleTurn32 azimuthAngle,AngleTurn16Stored32 rollAngle16,
          AngleTurn16Stored32 elevationAngle16,GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x004BD0B0 */
void GraphicsObject_RebuildTransformHierarchyRecursive(GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x00578560 */
uint32_t __cdecl Graphics_Init(void);

/* 0x005794E0 */
void GraphicsBackend_ShutdownGlideOnDeactivate(void);

/* 0x00579520 */
void Graphics_Shutdown(void);

/* 0x0057A5C0 */
void Graphics_SetViewportAndClearDepth(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

/* 0x0057E6D0 */
void Graphics_BeginScene(void);

/* 0x0057E750 */
void Graphics_EndScene(void);

/* 0x0057E7A0 */
void Graphics_DrawPrimitiveQueue(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

/* 0x0057A330 */
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface);

/* 0x0057A2C0 */
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface);

/* 0x00579EC0 */
void GraphicsCursor_SaveSurfaceBackground(SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface);

/* 0x0057A0C0 */
void GraphicsCursor_RestoreSurfaceBackground(SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface);

#endif /* THANDOR_GRAPHICS_CORE_RUNTIME_H */
