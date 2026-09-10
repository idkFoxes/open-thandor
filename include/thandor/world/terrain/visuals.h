#ifndef THANDOR_WORLD_TERRAIN_VISUALS_H
#define THANDOR_WORLD_TERRAIN_VISUALS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/visuals. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053D370 */
TerrainCompositeTextureRuntime * TerrainCompositeTexture_Create(void);

/* 0x00503B10 */
void TerrainByteClampLookup_Initialize(void);

/* 0x00503F30 */
undefined8 TerrainVisualResources_LoadPrimary (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field);

/* 0x005041C0 */
undefined8 TerrainVisualResources_LoadAndClearCellOverlayFlags (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field);

/* 0x00504470 */
GraphicsTextureSet * TerrainVisualResources_Shutdown(void);

/* 0x00505780 */
void TerrainLighting_BuildColorRampAndSetBaseColor (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb, PackedArgb32 rampStepColorArgb);

/* 0x0053D4D0 */
void TerrainCompositeTexture_Destroy(void);

/* 0x00561EA0 */
void TerrainLighting_AdjustDirectionAndRecomputeField (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x0053D560 */
void TerrainCompositeTexture_FillPlane1(void);

/* 0x0053D680 */
void TerrainCompositeTexture_FillPlane2(void);

/* 0x0053D840 */
void TerrainCompositeTexture_RebuildPlane0(void);

#endif /* THANDOR_WORLD_TERRAIN_VISUALS_H */
