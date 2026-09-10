#ifndef THANDOR_GRAPHICS_CORE_RUNTIME_H
#define THANDOR_GRAPHICS_CORE_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/core/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00576C30 */
void __cdecl GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void);

/* 0x004168B0 */
dword GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex);

/* 0x004168E0 */
void __cdecl GraphicsCursor_ConsumeNextInputEvent(void);

/* 0x00486430 */
GraphicsProjectedPointEdxEax8 Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint);

/* 0x00486490 */
void Graphics_SetProjectionClipRect (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX, GraphicsScreenCoordinate minY,GraphicsScreenCoordinate minX);

/* 0x004864D0 */
void Graphics_SetViewProjectionParameters (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewAngle1, GraphicsViewAngle16 viewAngle0,GraphicsProjectionScale projectionScale, GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY, GraphicsWorldCoordinateQ12 originX);

/* 0x00486640 */
void Graphics_SetProjectionViewport (GraphicsScreenCoordinate bound0,GraphicsScreenCoordinate bound1, GraphicsScreenCoordinate bound2,GraphicsScreenCoordinate bound3);

/* 0x004866C0 */
void Graphics_SetAuxiliaryOrientation(AngleTurn32 angle1,AngleTurn32 angle0);

/* 0x00486730 */
void Graphics_SetSceneBounds (GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6, GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4, GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2, GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0);

/* 0x00486790 */
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue);

/* 0x004867B0 */
void __cdecl Graphics_RebuildFrustumPlanes(void);

/* 0x004A9100 */
void GraphicsBackend_RefreshActiveAdapterNoOp(void);

/* 0x004BCFE0 */
FixedEulerPairEdxEax8 GraphicsObject_ExtractTransformEulerAnglesRegs(GraphicsObjectAddress32 graphicsObject);

/* 0x004BD000 */
undefined4 GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs (AngleTurn32 param_1,AngleTurn32 param_2,int param_3);

/* 0x004BD050 */
undefined8 GraphicsObject_SetTranslationDirectionPackedAnglesAndScale (int param_1,uint param_2,undefined4 param_3,int param_4);

/* 0x004BD080 */
undefined8 GraphicsObject_SetRotationEulerAnglesPacked(undefined4 param_1,int param_2,uint param_3,int param_4);

/* 0x004BD0B0 */
undefined8 GraphicsObject_RebuildTransformHierarchyRecursive(int param_1);

/* 0x00578560 */
dword __cdecl Graphics_Init(void);

/* 0x005794E0 */
void __cdecl GraphicsBackend_RefreshActiveAdapterIfReady(void);

/* 0x00579520 */
void __cdecl Graphics_Shutdown(void);

/* 0x0057A5C0 */
void Graphics_SetViewportAndClearDepth (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1, GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3);

/* 0x0057E6D0 */
void __cdecl Graphics_BeginScene(void);

/* 0x0057E750 */
void __cdecl Graphics_EndScene(void);

/* 0x0057E7A0 */
void Graphics_DrawPrimitiveQueue (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1, GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3, GraphicsPrimitiveQueue *queue);

/* 0x0057A330 */
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface);

/* 0x0057A2C0 */
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface);

/* 0x00579EC0 */
void GraphicsCursor_SaveSurfaceBackground (SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY, GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface);

/* 0x0057A0C0 */
void GraphicsCursor_RestoreSurfaceBackground (SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY, GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface);

#endif /* THANDOR_GRAPHICS_CORE_RUNTIME_H */
