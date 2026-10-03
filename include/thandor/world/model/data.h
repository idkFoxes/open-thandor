/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/data.h
 */

#ifndef THANDOR_WORLD_MODEL_DATA_H
#define THANDOR_WORLD_MODEL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsFixedVec3 g_ViewOriginFixed; /* 0048585C g_ViewOriginFixed */

extern int32_t g_ModelLodDepthThresholdQ8; /* 00485A04 g_ModelLodDepthThresholdQ8 */

extern GraphicsFixedVec3 g_GraphicsDirectionWorld; /* 004BCF50 g_GraphicsDirectionWorld */

extern GraphicsFixedVec3 g_ModelBoundsTransformedPoint; /* 004BD2F8 g_ModelBoundsTransformedPoint */

extern GraphicsFixedVec3 g_ModelCullViewRelative; /* 004BD8B0 g_ModelCullViewRelative */

extern GraphicsFixedMatrix3x4 g_ModelTransformScratchMatrix; /* 004BEA70 g_ModelTransformScratchMatrix */

extern GraphicsFixedMatrix3x4 g_GraphicsTransformScratchMatrix3x4; /* 0050A350 g_GraphicsTransformScratchMatrix3x4 */

extern GraphicsProjectedPoint2i g_ModelProjectedBoundsCornerScratch8[8]; /* 0050A3F0 g_ModelProjectedBoundsCornerScratch8 */

extern int32_t g_ModelRaycastMaximumDistance; /* 0050AE60 g_ModelRaycastMaximumDistance */

extern GraphicsFixedVec3 g_ModelRaycastOrigin; /* 0050AE64 g_ModelRaycastOrigin: Q12 world-space ray origin */

extern GraphicsFixedVec3 g_ModelRaycastWorldDirectionQ28; /* 0050AE7C g_ModelRaycastWorldDirectionQ28: Q28 world-space ray direction */

extern ModelRuntimeSlot *g_ModelRuntimeSlots; /* 005200B8 g_ModelRuntimeSlots */

extern int g_ModelRuntimeRebaseDelta; /* 005200BC g_ModelRuntimeRebaseDelta */

#endif
