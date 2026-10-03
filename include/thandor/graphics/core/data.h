/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/data.h
 */

#ifndef THANDOR_GRAPHICS_CORE_DATA_H
#define THANDOR_GRAPHICS_CORE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsCursorInputEvent18 g_CursorInputEvents[256]; /* 004107F0 g_CursorInputEvents */

extern uint32_t g_CursorInputReadIndex; /* 004167F4 g_CursorInputReadIndex */

extern uint32_t g_CursorInputClockValue; /* 004167F8 g_CursorInputClockValue */

extern uint32_t g_GraphicsCursorAnimationCountdown; /* 004167FC g_GraphicsCursorAnimationCountdown: uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */

extern uint32_t g_CursorButtonReleaseClock[3]; /* 00416800 g_CursorButtonReleaseClock */

extern GraphicsTextureSourceAsset *g_CursorSourceAsset; /* 0041680C g_CursorSourceAsset */

extern GraphicsCursorFrameRecord *g_CursorFrameRecords; /* 00416810 g_CursorFrameRecords */

extern GraphicsCursorFrameIndex g_CursorFrameIndex; /* 00416814 g_CursorFrameIndex */

extern GraphicsCursorFrameCount g_CursorFrameCount; /* 0041681C g_CursorFrameCount */

extern UiPixelCoordinate g_CursorLastClickX; /* 0041683C g_CursorLastClickX */

extern UiPixelCoordinate g_CursorLastClickY; /* 00416840 g_CursorLastClickY */

extern GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent; /* 00416850 g_GraphicsCursorConsumeEvent */

extern GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth; /* 00485818 g_GraphicsSetViewportAndClearDepth */

extern GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue; /* 0048581C g_GraphicsDrawPrimitiveQueue */

extern GraphicsBeginSceneProc *g_GraphicsBeginScene; /* 00485820 g_GraphicsBeginScene */

extern GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures; /* 00485828 g_GraphicsRebuildAllStagingTextures */

extern GraphicsViewAngle16 g_ViewAngle0; /* 0048586C g_ViewAngle0 */

extern GraphicsViewAngle16 g_ViewAngle1; /* 00485870 g_ViewAngle1 */

extern uint32_t g_ProjectionShift; /* 00485874 g_ProjectionShift */

extern uint32_t g_ProjectionScaleProduct; /* 00485878 g_ProjectionScaleProduct */

extern GraphicsWideFixed g_ProjectionNumerator; /* 0048587C g_ProjectionNumerator */

extern GraphicsFixedVec2 g_ProjectionCenterFixed; /* 00485884 g_ProjectionCenterFixed */

extern GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed; /* 004858D0 g_ViewRotationMatrixFixed */

extern GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed; /* 00485900 g_CameraTransformMatrixFixed */

extern GraphicsWideFixed g_ProjectionAngleFactors[2]; /* 00485930 g_ProjectionAngleFactors */

extern GraphicsFixedVec2 g_AuxiliaryOrientation; /* 00485940 g_AuxiliaryOrientation */

extern GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed; /* 00485948 g_AuxiliaryForwardDirectionFixed */

extern GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4]; /* 00485984 g_FrustumPlaneNormalFixed_0 */

extern GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4]; /* 004859B4 g_FrustumCornerRayFixed_0 */

extern GraphicsSceneBounds8 g_SceneBoundsFixed; /* 004859E4 g_SceneBoundsFixed */

extern SoftwareFramebufferAccess *g_CursorSavedBackground; /* 004A8E74 g_CursorSavedBackground */

extern SoftwareFramebufferAccess *g_CursorCompositeBuffer; /* 004A8E78 g_CursorCompositeBuffer */

extern DirectDrawCreate *pDirectDrawCreate; /* 00573FBC pDirectDrawCreate */

extern DirectDrawEnumerateA *pDirectDrawEnumerateA; /* 00573FC0 pDirectDrawEnumerateA */

extern char sz_DDRAW[6]; /* 005744B4 sz_DDRAW */

extern char sz_DirectDrawCreate[17]; /* 0057455E sz_DirectDrawCreate */

extern char sz_DirectDrawEnumerateA[21]; /* 00574570 sz_DirectDrawEnumerateA */

extern uint32_t g_MouseEventsProcessed; /* 00576C1C g_MouseEventsProcessed */

extern DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc; /* 00577CF0 g_GraphicsCursorSurfaceDesc: scratch descriptor the cursor save/restore Lock fills (lPitch at 00577D00, lpSurface at 00577D14) */

extern int32_t g_CursorCurrentDrawX; /* 00577E3C g_CursorCurrentDrawX */

extern int32_t g_CursorCurrentDrawY; /* 00577E40 g_CursorCurrentDrawY */

#endif
