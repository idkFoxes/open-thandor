/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/visuals.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_VISUALS_H
#define THANDOR_WORLD_TERRAIN_VISUALS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/visuals. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053D370 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx TerrainCompositeTexture_Create(void);

/* 0x00503B10 */
bool __thandor_void_preserve_ecx_edx TerrainByteClampLookup_Initialize(void);

/* 0x00503F30 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
TerrainVisualResources_LoadPrimary
          (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field);

/* 0x005041C0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TerrainVisualResources_LoadAndClearCellOverlayFlags
          (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field);

/* 0x00504470 */
void __thandor_void_preserve_eax_ecx TerrainVisualResources_Shutdown(void);

/* 0x00505780 */
void __thandor_void_preserve_eax_ecx_edx
TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          );

/* 0x0053D4D0 */
void __thandor_preserve_eax TerrainCompositeTexture_Destroy(void);

/* 0x00561EA0 */
void __thandor_preserve_eax_edx
TerrainLighting_AdjustDirectionAndRecomputeField
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x0053D560 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane1(void);

/* 0x0053D680 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane2(void);

/* 0x0053D840 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_RebuildPlane0(void);

#endif /* THANDOR_WORLD_TERRAIN_VISUALS_H */
