/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/graphics/core/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) GraphicsCursorInputEvent18 g_CursorInputEvents[256] = {0};

__declspec(align(4)) uint32_t g_CursorInputReadIndex = 0;

__declspec(align(8)) uint32_t g_CursorInputClockValue = 0;

/* uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */
__declspec(align(4)) uint32_t g_GraphicsCursorAnimationCountdown = 2;

__declspec(align(16)) uint32_t g_CursorButtonReleaseClock[3] = {0};

__declspec(align(4)) GraphicsTextureSourceAsset *g_CursorSourceAsset = 0;

__declspec(align(16)) GraphicsCursorFrameRecord *g_CursorFrameRecords = 0;

__declspec(align(4)) GraphicsCursorFrameIndex g_CursorFrameIndex = 0;

__declspec(align(4)) GraphicsCursorFrameCount g_CursorFrameCount = 0;

__declspec(align(4)) UiPixelCoordinate g_CursorLastClickX = 0;

__declspec(align(16)) UiPixelCoordinate g_CursorLastClickY = 0;

__declspec(align(16)) GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent = (void *)GraphicsCursor_ConsumeNextInputEvent;

__declspec(align(8)) GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth = (void *)SoftwareRenderer_ClearViewport;

__declspec(align(4)) GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue = (void *)SoftwareRenderer_DrawPrimitiveQueueBridge;

__declspec(align(16)) GraphicsBeginSceneProc *g_GraphicsBeginScene = (void *)SoftwareGraphicsDispatch_SuccessNoOp;

__declspec(align(8)) GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures = (void *)GraphicsTexture_RebuildNoOp;

__declspec(align(4)) GraphicsViewAngle16 g_ViewAngle0 = 0;

__declspec(align(16)) GraphicsViewAngle16 g_ViewAngle1 = 0;

__declspec(align(4)) uint32_t g_ProjectionShift = 0;

__declspec(align(8)) uint32_t g_ProjectionScaleProduct = 0;

__declspec(align(4)) GraphicsWideFixed g_ProjectionNumerator = {0};

__declspec(align(4)) GraphicsFixedVec2 g_ProjectionCenterFixed = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed = {0};

__declspec(align(16)) GraphicsWideFixed g_ProjectionAngleFactors[2] = {0};

__declspec(align(16)) GraphicsFixedVec2 g_AuxiliaryOrientation = {0};

__declspec(align(8)) GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed = {0};

__declspec(align(4)) GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4] = {0};

__declspec(align(4)) GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4] = {0};

__declspec(align(4)) GraphicsSceneBounds8 g_SceneBoundsFixed = {0};

__declspec(align(4)) SoftwareFramebufferAccess *g_CursorSavedBackground = 0;

__declspec(align(8)) SoftwareFramebufferAccess *g_CursorCompositeBuffer = 0;

__declspec(align(4)) DirectDrawCreate *pDirectDrawCreate = 0;

__declspec(align(16)) DirectDrawEnumerateA *pDirectDrawEnumerateA = 0;

__declspec(align(4)) char sz_DDRAW[6] = "DDRAW";

__declspec(align(4)) char sz_DirectDrawCreate[17] = "DirectDrawCreate";

__declspec(align(16)) char sz_DirectDrawEnumerateA[21] = "DirectDrawEnumerateA";

__declspec(align(4)) uint32_t g_MouseEventsProcessed = 0;

/* scratch descriptor the cursor save/restore Lock fills (lPitch at 00577D00, lpSurface at 00577D14) */
__declspec(align(16)) DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc = {0};

__declspec(align(4)) int32_t g_CursorCurrentDrawX = 0;

__declspec(align(16)) int32_t g_CursorCurrentDrawY = 0;
