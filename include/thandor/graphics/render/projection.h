/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/projection.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_PROJECTION_H
#define THANDOR_GRAPHICS_RENDER_PROJECTION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

extern GraphicsFixedVec3 g_ViewOriginFixed;

extern int32_t g_ProjectionScaleFixed;

extern GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed;

extern GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed;

extern GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed;

extern GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4];

extern GraphicsSceneBounds8 g_SceneBoundsFixed;

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

Bool8 GraphicsProjectedPoint_IsInsideTriangle(int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2);

extern GraphicsPrimitiveQueue *g_ActivePrimitiveQueue;

extern GraphicsFixedRect g_ProjectionClipRect;

#endif /* THANDOR_GRAPHICS_RENDER_PROJECTION_H */
