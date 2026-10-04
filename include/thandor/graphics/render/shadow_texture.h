/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/shadow_texture.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_SHADOW_TEXTURE_H
#define THANDOR_GRAPHICS_RENDER_SHADOW_TEXTURE_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

/* Generated shadow texture (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): a shadow vertex fades
   out linearly with its ray distance to the caster and vanishes at 5.0 world units (Q12) */
#define GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 0x5000
/* 1/256 grid cell in Q20: the far texture coordinate sample of a g_GraphicsShadingGridStepQ20 step is pulled
   back by this so it stays inside the step (samples at 0, step / 2 and step - this) */
#define GRAPHICS_SHADING_SAMPLE_INSET_Q20 0x1000
/* Projected point pool of the generated shadow texture: 0x80-byte primitive blocks of 16 point pairs, three
   0x20-byte vertices (4 pairs each) and a 0x20-byte header at +0x60 (pairs 12..15). A shadow patch reserves 14
   consecutive blocks (GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks). */
#define GRAPHICS_PROJECTED_BLOCK_BYTES 0x80
#define GRAPHICS_PROJECTED_BLOCK_PAIRS 16
#define GRAPHICS_SHADOW_PATCH_BLOCK_COUNT 14
/* Index of point pair `pair` of block `block` in a GraphicsProjectedPointPair array of consecutive blocks */
#define GRAPHICS_PROJECTED_PAIR(block, pair) ((block) * GRAPHICS_PROJECTED_BLOCK_PAIRS + (pair))
/* Highest light level of g_ShadingIntensityScaleMmx (256 entries) */
#define GRAPHICS_SHADING_INTENSITY_MAX 255

void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext);

uint32_t GraphicsShadingRuntime_InitializeGeneratedTexture
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension);

void GraphicsShadingRuntime_Shutdown();

void GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();

void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();

void GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh(ModelMeshGroupAddress32 meshRecord);

void GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(ModelRuntimeNode *modelNode);

uintptr_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh(ModelMeshGroupAddress32 meshRecord);

uintptr_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(ModelRuntimeNode *modelNode);

void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(ModelMeshGroupAddress32 meshRecord);

void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(ModelRuntimeNode *modelNode);

void GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

void GraphicsShadingGeneratedTexture_AdvanceTileCursor();

void GraphicsShadingGeneratedTexture_FilterGridScratchMmx();

Bool8 GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode);

GraphicsProjectedPointPair *GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext);

void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext);

void GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

void GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform);

void GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC);

extern uint32_t g_TextureDownsampleShift;

extern uint32_t g_GraphicsShadingTextureDimension;
extern uint32_t g_GraphicsShadingGridHalfSize;
extern uint8_t *g_GraphicsShadingGeneratedTexturePixelCursor;
extern uint32_t g_GraphicsShadingGeneratedTextureTileX;
extern uint32_t g_GraphicsShadingGeneratedTextureTileY;
extern GraphicsSubresourceIndex g_GraphicsShadingGeneratedTextureSubresourceIndex;
extern uint32_t g_GraphicsShadingSubresourceCount;
extern uint32_t g_GraphicsShadingGeneratedTextureTileXQ20;
extern uint32_t g_GraphicsShadingGeneratedTextureTileYQ20;
extern uint32_t g_GraphicsShadingGridStepQ20;
extern int32_t g_GraphicsShadingGridStepQ20Current;
extern GraphicsTextureSourceAsset *g_GraphicsShadingGeneratedAsset;
extern void *g_GraphicsShadingGridScratch;
extern void *g_GraphicsShadingGridScratchInterior;
extern GraphicsTextureSet *g_GraphicsShadingTextureSet;
extern uint32_t g_GraphicsShadingGeneratedTextureCompletedTraversalCount;
extern int32_t g_GraphicsShadingPositiveGridOriginQ12;
extern int32_t g_GraphicsShadingNegativeGridOriginQ12;
extern GeneratedTextureScratchRuntime g_GeneratedTextureScratchRuntime;

#endif /* THANDOR_GRAPHICS_RENDER_SHADOW_TEXTURE_H */
