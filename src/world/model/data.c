/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/model/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0048585C g_ViewOriginFixed */
__declspec(align(4)) GraphicsFixedVec3 g_ViewOriginFixed = {0};

/* 00485A04 g_ModelLodDepthThresholdQ8 */
__declspec(align(4)) int32_t g_ModelLodDepthThresholdQ8 = 65536;

/* 004BCF50 g_GraphicsDirectionWorld */
__declspec(align(16)) GraphicsFixedVec3 g_GraphicsDirectionWorld = {0};

/* 004BD2F8 g_ModelBoundsTransformedPoint */
__declspec(align(8)) GraphicsFixedVec3 g_ModelBoundsTransformedPoint = {0};

/* 004BD8B0 g_ModelCullViewRelative */
__declspec(align(16)) GraphicsFixedVec3 g_ModelCullViewRelative = {0};

/* 004BEA70 g_ModelTransformScratchMatrix */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_ModelTransformScratchMatrix = {0};

/* 0050A350 g_GraphicsTransformScratchMatrix3x4 */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_GraphicsTransformScratchMatrix3x4 = {0};

/* 0050A3F0 g_ModelProjectedBoundsCornerScratch8 */
__declspec(align(16)) GraphicsProjectedPoint2i g_ModelProjectedBoundsCornerScratch8[8] = {0};

/* 0050AE60 g_ModelRaycastMaximumDistance */
__declspec(align(16)) int32_t g_ModelRaycastMaximumDistance = 0;

/* 0050AE64 g_ModelRaycastOrigin: Q12 world-space ray origin */
__declspec(align(4)) GraphicsFixedVec3 g_ModelRaycastOrigin = {0};

/* 0050AE7C g_ModelRaycastWorldDirectionQ28: Q28 world-space ray direction */
__declspec(align(4)) GraphicsFixedVec3 g_ModelRaycastWorldDirectionQ28 = {0};

/* 005200B8 g_ModelRuntimeSlots */
__declspec(align(8)) ModelRuntimeSlot *g_ModelRuntimeSlots = 0;

/* 005200BC g_ModelRuntimeRebaseDelta */
__declspec(align(4)) int g_ModelRuntimeRebaseDelta = 0;
