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

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00576C30 */
void __thandor_preserve_eax_edx GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void);

/* 0x004168B0 */
CursorFrameResult __thandor_eax_cf_preserve_ecx_edx
GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex);

/* 0x004168E0 */
CursorEventResult __thandor_input_event_regs_cf GraphicsCursor_ConsumeNextInputEvent(void);

/* 0x00486430 */
GraphicsProjectedPointPair __thandor_eax_edx_cf_preserve_ecx
Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint);

/* 0x00486490 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX);

/* 0x004864D0 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX);

/* 0x00486640 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetProjectionViewport
          (GraphicsScreenCoordinate bottom,GraphicsScreenCoordinate right,
          GraphicsScreenCoordinate top,GraphicsScreenCoordinate left);

/* 0x004866C0 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetAuxiliaryOrientation(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00486730 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetSceneBounds
          (GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
          GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4,
          GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2,
          GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0);

/* 0x00486790 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue);

/* 0x004867B0 */
void __thandor_void_preserve_eax_ecx_edx Graphics_RebuildFrustumPlanes(void);

/* 0x004A9100 */
void GraphicsBackend_RefreshActiveAdapterNoOp(void);

/* 0x004BCFE0 */
FixedEulerAnglesEaxEcxEdx12
GraphicsObject_ExtractTransformEulerAnglesRegs(GraphicsObjectAddress32 graphicsObject);

/* 0x004BD000 */
FixedElevationAzimuth __thandor_preserve_eax
GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          GraphicsObjectAddress32 graphicsObject);

/* 0x004BD050 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_SetTranslationDirectionPackedAnglesAndScale
          (AngleTurn16Stored32 elevationAngle16,AngleTurn16Stored32 azimuthAngle16,
          FixedMathScale32 distance,GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x004BD080 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_SetRotationEulerAnglesPacked
          (AngleTurn32 azimuthAngle,AngleTurn16Stored32 rollAngle16,
          AngleTurn16Stored32 elevationAngle16,GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x004BD0B0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_RebuildTransformHierarchyRecursive(GraphicsObjectAddress32 graphicsObjectAddress);

/* 0x00578560 */
StatusResult __cdecl Graphics_Init(void);

/* 0x005794E0 */
void __thandor_void_preserve_eax_ecx_edx GraphicsBackend_RefreshActiveAdapterIfReady(void);

/* 0x00579520 */
void __thandor_void_preserve_eax_ecx_edx Graphics_Shutdown(void);

/* 0x0057A5C0 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetViewportAndClearDepth
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

/* 0x0057E6D0 */
void __thandor_void_preserve_eax_ecx_edx Graphics_BeginScene(void);

/* 0x0057E750 */
void __thandor_void_preserve_eax_ecx_edx Graphics_EndScene(void);

/* 0x0057E7A0 */
void __thandor_void_preserve_eax_ecx_edx
Graphics_DrawPrimitiveQueue
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

/* 0x0057A330 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface);

/* 0x0057A2C0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface);

/* 0x00579EC0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_SaveSurfaceBackground
          (SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface);

/* 0x0057A0C0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_RestoreSurfaceBackground
          (SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface);

#endif /* THANDOR_GRAPHICS_CORE_RUNTIME_H */
