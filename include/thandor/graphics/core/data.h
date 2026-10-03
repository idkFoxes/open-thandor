/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/data.h
 */

#ifndef THANDOR_GRAPHICS_CORE_DATA_H
#define THANDOR_GRAPHICS_CORE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsCursorInputEvent18 g_CursorInputEvents[256];

extern uint32_t g_CursorInputReadIndex;

extern uint32_t g_CursorInputClockValue;

extern uint32_t g_GraphicsCursorAnimationCountdown; /* uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */

extern uint32_t g_CursorButtonReleaseClock[3];

extern GraphicsTextureSourceAsset *g_CursorSourceAsset;

extern GraphicsCursorFrameRecord *g_CursorFrameRecords;

extern GraphicsCursorFrameIndex g_CursorFrameIndex;

extern GraphicsCursorFrameCount g_CursorFrameCount;

extern UiPixelCoordinate g_CursorLastClickX;

extern UiPixelCoordinate g_CursorLastClickY;

extern GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent;

extern GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth;

extern GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue;

extern GraphicsBeginSceneProc *g_GraphicsBeginScene;

extern GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures;

extern GraphicsViewAngle16 g_ViewAngle0;

extern GraphicsViewAngle16 g_ViewAngle1;

extern uint32_t g_ProjectionShift;

extern uint32_t g_ProjectionScaleProduct;

extern GraphicsWideFixed g_ProjectionNumerator;

extern GraphicsFixedVec2 g_ProjectionCenterFixed;

extern GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed;

extern GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed;

extern GraphicsWideFixed g_ProjectionAngleFactors[2];

extern GraphicsFixedVec2 g_AuxiliaryOrientation;

extern GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed;

extern GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4];

extern GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4];

extern GraphicsSceneBounds8 g_SceneBoundsFixed;

extern SoftwareFramebufferAccess *g_CursorSavedBackground;

extern SoftwareFramebufferAccess *g_CursorCompositeBuffer;

extern DirectDrawCreate *pDirectDrawCreate;

extern DirectDrawEnumerateA *pDirectDrawEnumerateA;

extern char sz_DDRAW[6];

extern char sz_DirectDrawCreate[17];

extern char sz_DirectDrawEnumerateA[21];

extern uint32_t g_MouseEventsProcessed;

extern DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc; /* scratch descriptor the cursor save/restore Lock fills (lPitch, lpSurface) */

extern int32_t g_CursorCurrentDrawX;

extern int32_t g_CursorCurrentDrawY;

#endif
