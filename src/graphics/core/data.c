/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/graphics/core/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 004107F0 g_CursorInputEvents */
__declspec(align(16)) GraphicsCursorInputEvent18 g_CursorInputEvents[256] = {0};

/* 004167F4 g_CursorInputReadIndex */
__declspec(align(4)) uint32_t g_CursorInputReadIndex = 0;

/* 004167F8 g_CursorInputClockValue */
__declspec(align(8)) uint32_t g_CursorInputClockValue = 0;

/* 004167FC g_GraphicsCursorAnimationCountdown: uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */
__declspec(align(4)) uint32_t g_GraphicsCursorAnimationCountdown = 2;

/* 00416800 g_CursorButtonReleaseClock */
__declspec(align(16)) uint32_t g_CursorButtonReleaseClock[3] = {0};

/* 0041680C g_CursorSourceAsset */
__declspec(align(4)) GraphicsTextureSourceAsset *g_CursorSourceAsset = 0;

/* 00416810 g_CursorFrameRecords */
__declspec(align(16)) GraphicsCursorFrameRecord *g_CursorFrameRecords = 0;

/* 00416814 g_CursorFrameIndex */
__declspec(align(4)) GraphicsCursorFrameIndex g_CursorFrameIndex = 0;

/* 0041681C g_CursorFrameCount */
__declspec(align(4)) GraphicsCursorFrameCount g_CursorFrameCount = 0;

/* 0041683C g_CursorLastClickX */
__declspec(align(4)) UiPixelCoordinate g_CursorLastClickX = 0;

/* 00416840 g_CursorLastClickY */
__declspec(align(16)) UiPixelCoordinate g_CursorLastClickY = 0;

/* 00416850 g_GraphicsCursorConsumeEvent */
__declspec(align(16)) GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent = (void *)GraphicsCursor_ConsumeNextInputEvent;

/* 00485818 g_GraphicsSetViewportAndClearDepth */
__declspec(align(8)) GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth = (void *)SoftwareRenderer_ClearViewport;

/* 0048581C g_GraphicsDrawPrimitiveQueue */
__declspec(align(4)) GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue = (void *)SoftwareRenderer_DrawPrimitiveQueueBridge;

/* 00485820 g_GraphicsBeginScene */
__declspec(align(16)) GraphicsBeginSceneProc *g_GraphicsBeginScene = (void *)SoftwareGraphicsDispatch_SuccessNoOp;

/* 00485828 g_GraphicsRebuildAllStagingTextures */
__declspec(align(8)) GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures = (void *)GraphicsTexture_RebuildNoOp;

/* 0048586C g_ViewAngle0 */
__declspec(align(4)) GraphicsViewAngle16 g_ViewAngle0 = 0;

/* 00485870 g_ViewAngle1 */
__declspec(align(16)) GraphicsViewAngle16 g_ViewAngle1 = 0;

/* 00485874 g_ProjectionShift */
__declspec(align(4)) uint32_t g_ProjectionShift = 0;

/* 00485878 g_ProjectionScaleProduct */
__declspec(align(8)) uint32_t g_ProjectionScaleProduct = 0;

/* 0048587C g_ProjectionNumerator */
__declspec(align(4)) GraphicsWideFixed g_ProjectionNumerator = {0};

/* 00485884 g_ProjectionCenterFixed */
__declspec(align(4)) GraphicsFixedVec2 g_ProjectionCenterFixed = {0};

/* 004858D0 g_ViewRotationMatrixFixed */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed = {0};

/* 00485900 g_CameraTransformMatrixFixed */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed = {0};

/* 00485930 g_ProjectionAngleFactors */
__declspec(align(16)) GraphicsWideFixed g_ProjectionAngleFactors[2] = {0};

/* 00485940 g_AuxiliaryOrientation */
__declspec(align(16)) GraphicsFixedVec2 g_AuxiliaryOrientation = {0};

/* 00485948 g_AuxiliaryForwardDirectionFixed */
__declspec(align(8)) GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed = {0};

/* 00485984 g_FrustumPlaneNormalFixed_0 */
__declspec(align(4)) GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4] = {0};

/* 004859B4 g_FrustumCornerRayFixed_0 */
__declspec(align(4)) GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4] = {0};

/* 004859E4 g_SceneBoundsFixed */
__declspec(align(4)) GraphicsSceneBounds8 g_SceneBoundsFixed = {0};

/* 004A8E74 g_CursorSavedBackground */
__declspec(align(4)) SoftwareFramebufferAccess *g_CursorSavedBackground = 0;

/* 004A8E78 g_CursorCompositeBuffer */
__declspec(align(8)) SoftwareFramebufferAccess *g_CursorCompositeBuffer = 0;

/* 004A8ED4 g_GraphicsBackendRefreshActiveAdapter */
__declspec(align(4)) GraphicsBackendRefreshActiveAdapterProc *g_GraphicsBackendRefreshActiveAdapter = (void *)GraphicsBackend_RefreshActiveAdapterNoOp;

/* 004BCF20 g_GraphicsDirectionInverseTransform */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_GraphicsDirectionInverseTransform = {0};

/* 004BCF5C g_GraphicsDirectionLocal */
__declspec(align(4)) GraphicsFixedVec3 g_GraphicsDirectionLocal = {0};

/* 00573FBC pDirectDrawCreate */
__declspec(align(4)) DirectDrawCreate *pDirectDrawCreate = 0;

/* 00573FC0 pDirectDrawEnumerateA */
__declspec(align(16)) DirectDrawEnumerateA *pDirectDrawEnumerateA = 0;

/* 005744B4 sz_DDRAW */
__declspec(align(4)) char sz_DDRAW[6] = "DDRAW";

/* 0057455E sz_DirectDrawCreate */
__declspec(align(4)) char sz_DirectDrawCreate[17] = "DirectDrawCreate";

/* 00574570 sz_DirectDrawEnumerateA */
__declspec(align(16)) char sz_DirectDrawEnumerateA[21] = "DirectDrawEnumerateA";

/* 00576C1C g_MouseEventsProcessed */
__declspec(align(4)) uint32_t g_MouseEventsProcessed = 0;

/* 00577C68 g_Direct3DViewport2 */
__declspec(align(8)) IDirect3DViewport2 *g_Direct3DViewport2 = 0;

/* 00577CF0 g_GraphicsCursorSurfaceDesc: scratch descriptor the cursor save/restore Lock fills (lPitch at 00577D00, lpSurface at 00577D14) */
__declspec(align(16)) DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc = {0};

/* 00577D60 g_Direct3DViewportState */
__declspec(align(16)) D3DVIEWPORT2 g_Direct3DViewportState = {
    .dvClipX = 0.0f,
    .dvClipY = 0.0f,
    .dvClipWidth = 0.0f,
    .dvClipHeight = 0.0f,
    .dvMinZ = 0.0f,
    .dvMaxZ = 0.0f};

/* 00577E3C g_CursorCurrentDrawX */
__declspec(align(4)) int32_t g_CursorCurrentDrawX = 0;

/* 00577E40 g_CursorCurrentDrawY */
__declspec(align(16)) int32_t g_CursorCurrentDrawY = 0;

/* 00578040 g_LastViewportRect */
__declspec(align(16)) D3DRECT_DX6 g_LastViewportRect = {0};

/* 00578078 g_CommandLineOptionD3dAll */
__declspec(align(8)) char g_CommandLineOptionD3dAll[7] = "D3DALL";

/* 0057EE84 g_CommandLineOptionGlide */
__declspec(align(4)) char g_CommandLineOptionGlide[6] = "GLIDE";
