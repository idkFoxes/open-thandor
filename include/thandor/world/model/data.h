/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/data.h
 */

#ifndef THANDOR_WORLD_MODEL_DATA_H
#define THANDOR_WORLD_MODEL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsFixedVec3 g_ViewOriginFixed;

extern int32_t g_ModelLodDepthThresholdQ8;

extern GraphicsFixedVec3 g_GraphicsDirectionWorld;

extern GraphicsFixedVec3 g_ModelBoundsTransformedPoint;

extern GraphicsFixedVec3 g_ModelCullViewRelative;

extern GraphicsFixedMatrix3x4 g_ModelTransformScratchMatrix;

extern GraphicsFixedMatrix3x4 g_GraphicsTransformScratchMatrix3x4;

extern GraphicsProjectedPoint2i g_ModelProjectedBoundsCornerScratch8[8];

extern int32_t g_ModelRaycastMaximumDistance;

extern GraphicsFixedVec3 g_ModelRaycastOrigin; /* Q12 world-space ray origin */

extern GraphicsFixedVec3 g_ModelRaycastWorldDirectionQ28; /* Q28 world-space ray direction */

extern ModelRuntimeSlot *g_ModelRuntimeSlots;

extern int g_ModelRuntimeRebaseDelta;

#endif
