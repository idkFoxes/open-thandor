#ifndef THANDOR_GRAPHICS_RENDER_PRIMITIVES_H
#define THANDOR_GRAPHICS_RENDER_PRIMITIVES_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/primitives. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486080 */
void GraphicsPrimitiveQueue_RadixSortForRendering (GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue);

/* 0x004D0A10 */
void GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity);

/* 0x004D0A40 */
GraphicsPrimitiveQueue * __cdecl GraphicsPrimitiveQueue_ResetGlobal(void);

/* 0x004D0A70 */
void GraphicsPrimitiveQueue_Free(GraphicsPrimitiveQueue *queue);

/* 0x004D0A90 */
dword GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue);

/* 0x004D0AA0 */
GraphicsPrimitivePacket * GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue);

/* 0x004D0AE0 */
GraphicsPrimitivePacket * GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue);

/* 0x004D0B20 */
void GraphicsPrimitiveQueue_AppendTriangle (GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle, GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1, GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue);

/* 0x004D0C80 */
void GraphicsPrimitiveQueue_SetVertexColors (PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color, GraphicsPrimitiveQueue *queue);

/* 0x004D0D00 */
void GraphicsPrimitiveQueue_SetMaterial (PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry, GraphicsPrimitiveQueue *queue);

/* 0x004D0D50 */
void GraphicsPrimitiveQueue_OffsetTextureCoordinates (GraphicsPrimitiveTextureCoordinateFixed deltaV, GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue);

/* 0x004D0DA0 */
undefined8 GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle (undefined4 *param_1,uint param_2,uint param_3,uint param_4,int param_5,int param_6, int param_7,int param_8);

/* 0x004D0F20 */
undefined8 __fastcall GraphicsPrimitiveQueue_AppendTexturedTriangleRegs (undefined4 incomingEcxValue,undefined4 preservedEdxValue,dword *textureAndMaterialIndices ,PackedArgb32 vertex2DiffuseColor,PackedArgb32 vertex1DiffuseColor, PackedArgb32 vertex0DiffuseColor,GraphicsProjectedVertexSource *vertex2Projected, GraphicsProjectedVertexSource *vertex1Projected, GraphicsProjectedVertexSource *vertex0Projected,GraphicsPrimitiveQueue *primitiveQueue);

/* 0x004FFC10 */
DepthBinMaskEaxPreservedEdxCarrier64 DepthInterval_BuildBinMask(DepthIntervalRadius32 intervalRadius,DepthIntervalCenter32 centerDepth);

/* 0x004FFC50 */
void DepthBinMasks_OverlapCf (DepthBinMask32 firstMaskLow,DepthBinMask32 firstMaskHigh, DepthBinMask32 secondMaskLow,DepthBinMask32 secondMaskHigh);

#endif /* THANDOR_GRAPHICS_RENDER_PRIMITIVES_H */
