/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/light_records.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_LIGHT_RECORDS_H
#define THANDOR_GRAPHICS_RENDER_LIGHT_RECORDS_H

#include <thandor/audio/codec/types.h>
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/core/contracts.h>

/* Number of runtime light records in g_GraphicsShadingRuntimeRecords (0x40 bytes each) */
inline constexpr int GRAPHICS_SHADING_RUNTIME_RECORD_COUNT = 256;
/* Intensity clamp table (GraphicsIntensityClampTable_Initialize): entry (previous << 8) | target is target
   limited to previous +/- this step, so model tints fade by at most 21 per update */
inline constexpr int GRAPHICS_INTENSITY_CLAMP_MAX_STEP = 21;
/* 64 KiB table plus 64 KiB slack so it can be aligned to a 64 KiB boundary */
inline constexpr int GRAPHICS_INTENSITY_CLAMP_ALLOCATION_BYTES = 0x20000;
inline constexpr int GRAPHICS_INTENSITY_CLAMP_TABLE_ALIGNMENT = 0x10000;

extern uint64_t g_PackedLightingLookupTable[512];

extern GraphicsShadingRuntimeRecord g_GraphicsShadingRuntimeRecords[256];

extern uintptr_t g_GraphicsIntensityClampTableBase;

extern GraphicsShadingRecordCount g_GraphicsShadingCompactRecordCount;

uint32_t GraphicsIntensityClampTable_Initialize();

MmxPackedValue64 GraphicsShadingRuntime_AccumulateCompactLightingAtPoint
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulator);

GraphicsShadingRuntimeRecord * GraphicsShadingRuntime_AllocateRecord
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

void GraphicsShadingRuntime_ClearRecordTable();

void GraphicsShadingRuntime_RebuildCompactLightingRecords();

void GraphicsShadingRuntime_CollectNearbyRecords(GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* Not in the original: fills g_ShadingIntensityScaleMmx (the original shipped it precomputed). */
void GraphicsShading_BuildIntensityScaleTable();

/* Not in the original: fills g_PackedLightingLookupTable (the original shipped it precomputed). */
void GraphicsLighting_BuildPackedLookupTable();

extern SoftwareBgraWordLanes g_ShadingIntensityScaleMmx[256];

extern GraphicsShadingRuntimeRecord g_GraphicsShadingNearbyRecords[256];

extern GraphicsShadingRecordCount g_GraphicsShadingNearbyRecordCount; /* GraphicsShadingRecordCount (4 bytes, 0 in the image): number of valid g_GraphicsShadingNearbyRecords, set by GraphicsShadingRuntime_CollectNearbyRecords, read by the model vertex lighting. Followed by 8 bytes of 0x90 filler and g_ModelLightingMmxMultiplierRows. */

#endif /* THANDOR_GRAPHICS_RENDER_LIGHT_RECORDS_H */
