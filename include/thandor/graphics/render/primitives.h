/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/primitives.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_PRIMITIVES_H
#define THANDOR_GRAPHICS_RENDER_PRIMITIVES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/primitives. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486080 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_RadixSortForRendering
          (GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue);

/* 0x004D0A10 */
void GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity);

/* 0x004D0A40 */
GraphicsPrimitiveQueueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_ResetGlobal(void);

/* 0x004D0A70 */
void __thandor_preserve_eax GraphicsPrimitiveQueue_Free(GraphicsPrimitiveQueue *queue);

/* 0x004D0A90 */
dword __thandor_eax_preserve_ecx_edx GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue);

/* 0x004D0AA0 */
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue);

/* 0x004D0AE0 */
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue);

/* 0x004D0B20 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_AppendTriangle
          (GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle,
          GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1,
          GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue);

/* 0x004D0C80 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_SetVertexColors
          (PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color,
          GraphicsPrimitiveQueue *queue);

/* 0x004D0D00 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_SetMaterial
          (PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry,
          GraphicsPrimitiveQueue *queue);

/* 0x004D0D50 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_OffsetTextureCoordinates
          (GraphicsPrimitiveTextureCoordinateFixed deltaV,
          GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue);

/* 0x004D0DA0 */
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf
          (dword *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x004D0F20 */
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
          (dword *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x004FFC10 */
DepthBinMask32 __thandor_eax_preserve_ecx_edx
DepthInterval_BuildBinMask(DepthIntervalRadius32 intervalRadius,DepthIntervalCenter32 centerDepth);

/* 0x004FFC50 */
bool __thandor_cf_preserve_eax_ecx_edx
DepthBinMasks_OverlapCf
          (DepthBinMask32 firstMaskLow,DepthBinMask32 firstMaskHigh,DepthBinMask32 secondMaskLow,
          DepthBinMask32 secondMaskHigh);

#endif /* THANDOR_GRAPHICS_RENDER_PRIMITIVES_H */
