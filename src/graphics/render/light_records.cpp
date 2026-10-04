/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/light_records.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Dynamic light records: the record table and its compact copy, record allocation, nearby-record collection,
   the compact lighting at a point, and the intensity clamp, intensity scale and packed lighting lookup tables. */

#include <thandor/graphics/render/light_records.h>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>

/* Module data. */

/* filled at startup by GraphicsLighting_BuildPackedLookupTable */
THANDOR_ALIGN(16) uint64_t g_PackedLightingLookupTable[512] = {0};

THANDOR_ALIGN(16) GraphicsShadingRuntimeRecord g_GraphicsShadingRuntimeRecords[256] = {0};

uintptr_t g_GraphicsIntensityClampTableBase = 0;

GraphicsShadingRecordCount g_GraphicsShadingCompactRecordCount = 0;

/* PMULHW multipliers per light level 0..255: zero B/G/R lanes and (level * 0x1010) >> 8 (0..0x0FFF) in the alpha
   lane; built by GraphicsShading_BuildIntensityScaleTable. */
SoftwareBgraWordLanes g_ShadingIntensityScaleMmx[256];

static GraphicsShadingRuntimeRecord g_GraphicsShadingCompactRecords[256] = {0};

GraphicsShadingRuntimeRecord g_GraphicsShadingNearbyRecords[256] = {0};

/* GraphicsShadingRecordCount (4 bytes, 0 in the image): number of valid g_GraphicsShadingNearbyRecords, set by GraphicsShadingRuntime_CollectNearbyRecords, read by the model vertex lighting. Followed by 8 bytes of 0x90 filler and g_ModelLightingMmxMultiplierRows. */
GraphicsShadingRecordCount g_GraphicsShadingNearbyRecordCount = 0;

/* Builds the 64 KiB intensity clamp table used by the model tint fade
   (ModelNodeRuntime_UpdateStateTintRecursive): entry (previous << 8) | target holds target limited to previous
   +/- GRAPHICS_INTENSITY_CLAMP_MAX_STEP. The table is aligned to 64 KiB so the original can index it with a
   16-bit register pair. Returns 0, or the (non-zero) arena error code when the allocation fails.
*/
uint32_t GraphicsIntensityClampTable_Initialize()

{
  int rowsRemaining;
  char targetByte;
  uint32_t targetIntensity;
  int previousIntensity;
  char *tableCursor;
  uint32_t allocError;
  void *allocPayload;

  allocError = g_MemoryApi.alloc(GRAPHICS_INTENSITY_CLAMP_ALLOCATION_BYTES,&allocPayload);
  if (allocError == 0) {
    targetIntensity = 0;
    /* round up to the next 64 KiB boundary */
    tableCursor = (char *)((uintptr_t)allocPayload +(GRAPHICS_INTENSITY_CLAMP_TABLE_ALIGNMENT - 1) & ~(uintptr_t)(GRAPHICS_INTENSITY_CLAMP_TABLE_ALIGNMENT - 1u));
    rowsRemaining = 256;
    previousIntensity = 0;
    g_GraphicsIntensityClampTableBase = (uintptr_t)tableCursor;
    do {
      do {
        targetByte = (char)targetIntensity;
        if (previousIntensity < (int)targetIntensity) {
          if (previousIntensity + GRAPHICS_INTENSITY_CLAMP_MAX_STEP < (int)targetIntensity) {
            *tableCursor = (char)(previousIntensity + GRAPHICS_INTENSITY_CLAMP_MAX_STEP);
          }
          else {
            *tableCursor = targetByte;
          }
        }
        else if ((int)targetIntensity < previousIntensity - GRAPHICS_INTENSITY_CLAMP_MAX_STEP) {
          *tableCursor = (char)(previousIntensity - GRAPHICS_INTENSITY_CLAMP_MAX_STEP);
        }
        else {
          *tableCursor = targetByte;
        }
        tableCursor = tableCursor + 1;
        /* 8-bit wrap ends the row after target 255 (the original counts the target in a byte) */
        targetIntensity = (uint32_t)(uint8_t)(targetByte + 1U);
      } while ((uint8_t)(targetByte + 1U) != 0);
      previousIntensity++;
      rowsRemaining--;
    } while (rowsRemaining != 0);
    return 0;
  }
  return allocError;
}

/* Adds the light of every active compact light record (view space, see
   GraphicsShadingRuntime_RebuildCompactLightingRecords) whose sphere contains worldPointQ12 to the packed
   light accumulator (four 16-bit lanes), with unsigned saturation, and returns the new accumulator. The
   strength comes from g_PackedLightingLookupTable indexed by (radius^2 - distance^2) / radius^2, so it falls
   off towards the sphere edge. Used by the terrain vertex shading in src/graphics/terrain/terrain_render.cpp.
*/
MmxPackedValue64 GraphicsShadingRuntime_AccumulateCompactLightingAtPoint
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulator)

{
  PackedRgb24 packedColor;
  int axisDelta;
  int64_t remainingQ24;
  uint32_t radiusScale;
  GraphicsShadingRecordCount recordsRemaining;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t scaledLight;

  shadingRecord = g_GraphicsShadingCompactRecords;
  for (recordsRemaining = g_GraphicsShadingCompactRecordCount; recordsRemaining != 0; recordsRemaining--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* 64-bit radius^2 - dx^2 - dy^2 - dz^2 (two's complement), giving up as soon as it turns negative */
      axisDelta = worldPointQ12->x - shadingRecord->worldXQ12;
      remainingQ24 = (int64_t)(shadingRecord->squaredRadiusQ24 - (uint64_t)((int64_t)axisDelta * (int64_t)axisDelta));
      if (remainingQ24 >= 0) {
        axisDelta = worldPointQ12->y - shadingRecord->worldYQ12;
        remainingQ24 = remainingQ24 - (int64_t)axisDelta * (int64_t)axisDelta;
        if (remainingQ24 >= 0) {
          axisDelta = worldPointQ12->z - shadingRecord->worldZQ12;
          remainingQ24 = remainingQ24 - (int64_t)axisDelta * (int64_t)axisDelta;
          if (remainingQ24 >= 0) {
            packedColor = shadingRecord->packedColorRgbActive;
            /* bits 12..43 of radius^2 */
            radiusScale = (uint32_t)((uint64_t)shadingRecord->squaredRadiusQ24 >> Q12_SHIFT);
            if (radiusScale != 0) {
              /* bits 5..36 of the remainder, divided unsigned */
              scaledLight = pmulhw(ColorLanes_UnpackBytesShiftRight(packedColor,2),
                              g_PackedLightingLookupTable[(int32_t)((uint32_t)(remainingQ24 >> 5) / radiusScale)]);
              packedLightAccumulator = paddusw(packedLightAccumulator,scaledLight);
            }
          }
        }
      }
    }
    shadingRecord++;
  }
  return packedLightAccumulator;
}

/* Claims the first free runtime light record (colour 0) for a point light at the given world position and
   returns it. With transitionDurationTicks 0 the light starts at full radius, otherwise its squared radius
   starts at 0 and grows over that many ticks. Returns NULL when packedColorRgb is 0 or all
   GRAPHICS_SHADING_RUNTIME_RECORD_COUNT records are taken.
*/
GraphicsShadingRuntimeRecord * GraphicsShadingRuntime_AllocateRecord
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int recordsRemaining;
  GraphicsShadingRuntimeRecord *recordCursor;

  if (packedColorRgb != 0) {
    recordCursor = g_GraphicsShadingRuntimeRecords;
    recordsRemaining = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT;
    do {
      if (recordCursor->packedColorRgbActive == 0) {
        recordCursor->targetRadiusQ12 = radiusQ12;
        recordCursor->packedColorRgbActive = packedColorRgb & ARGB8888_RGB_MASK;
        if (transitionDurationTicks == 0) {
          recordCursor->squaredRadiusQ24 = (int64_t)radiusQ12 * (int64_t)radiusQ12;
          recordCursor->radiusTransitionDurationTicks = 0;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        else {
          recordCursor->squaredRadiusQ24 = 0;
          recordCursor->radiusTransitionDurationTicks = transitionDurationTicks;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        recordCursor->worldXQ12 = worldXQ12;
        recordCursor->worldYQ12 = worldYQ12;
        recordCursor->worldZQ12 = worldZQ12;
        return recordCursor;
      }
      recordCursor = recordCursor + 1;
      recordsRemaining--;
    } while (recordsRemaining != 0);
  }
  return nullptr;
}

/* Zeroes the 256 runtime light records (0x40 bytes each, 0x4000 bytes in total) so that no light source is
   active; GraphicsShadingRuntime_RebuildCompactLightingRecords only picks up records with a colour set.
*/
void GraphicsShadingRuntime_ClearRecordTable()

{
  int recordDwordsRemaining;
  GraphicsShadingRuntimeRecord *recordDwordCursor;

  recordDwordCursor = g_GraphicsShadingRuntimeRecords;
  for (recordDwordsRemaining = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT * sizeof(GraphicsShadingRuntimeRecord) / 4;
       recordDwordsRemaining != 0; recordDwordsRemaining--) {
    recordDwordCursor->worldXQ12 = 0;
    recordDwordCursor = (GraphicsShadingRuntimeRecord *)&recordDwordCursor->worldYQ12;
  }
  return;
}

/* Once per rendered world frame (frontend world render in src/ui/frontend/menu_room.cpp): copies every active
   runtime light record (colour set) into the compact table with its position transformed into view space,
   and publishes the count, so the per-vertex and per-model light queries only walk the live lights.
*/
void GraphicsShadingRuntime_RebuildCompactLightingRecords()

{
  GraphicsRadiusQ12 targetRadius;
  GraphicsShadingRecordCount compactCount;
  int recordsRemaining;
  GraphicsShadingRuntimeRecord *sourceRecord;
  GraphicsShadingRuntimeRecord *compactRecord;
  
  sourceRecord = g_GraphicsShadingRuntimeRecords;
  compactRecord = g_GraphicsShadingCompactRecords;
  recordsRemaining = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT;
  compactCount = 0;
  do {
    if (sourceRecord->packedColorRgbActive != 0) {
      FixedTransform_ApplyPoint
                ((GraphicsFixedVec3 *)compactRecord,(GraphicsFixedVec3 *)sourceRecord,
                 &g_ViewProjectionMatrixFixed);
      targetRadius = sourceRecord->targetRadiusQ12;
      compactRecord->packedColorRgbActive = sourceRecord->packedColorRgbActive;
      compactRecord->targetRadiusQ12 = targetRadius;
      compactCount++;
      compactRecord->squaredRadiusQ24 = sourceRecord->squaredRadiusQ24;
      compactRecord++;
    }
    sourceRecord++;
    recordsRemaining--;
  } while (recordsRemaining != 0);
  g_GraphicsShadingCompactRecordCount = compactCount;
  return;
}

/* Copies every compact light record whose sphere overlaps the query sphere (distance^2 <= (queryRadius +
   lightRadius)^2, compared in 64 bits) into g_GraphicsShadingNearbyRecords and publishes
   g_GraphicsShadingNearbyRecordCount, so model vertex lighting (src/graphics/render/model_lighting.cpp) only tests
   the lights near the model. Called per model node by the hierarchy renderers in src/graphics/render/model_draw.cpp.
*/
void GraphicsShadingRuntime_CollectNearbyRecords(GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int deltaXQ12;
  int deltaYQ12;
  int deltaZQ12;
  int combinedRadiusQ12;
  uint32_t sourceRecordsRemaining;
  GraphicsShadingRuntimeRecord *sourceRecordCursor;
  GraphicsShadingRuntimeRecord *destinationRecordCursor;
  uint64_t combinedRadiusSquaredQ24;
  uint64_t distanceSquaredQ24;

  sourceRecordCursor = g_GraphicsShadingCompactRecords;
  destinationRecordCursor = g_GraphicsShadingNearbyRecords;
  g_GraphicsShadingNearbyRecordCount = 0;
  for (sourceRecordsRemaining = g_GraphicsShadingCompactRecordCount; sourceRecordsRemaining != 0;
      sourceRecordsRemaining--) {
    deltaXQ12 = worldXQ12 - sourceRecordCursor->worldXQ12;
    deltaYQ12 = worldYQ12 - sourceRecordCursor->worldYQ12;
    deltaZQ12 = worldZQ12 - sourceRecordCursor->worldZQ12;
    /* sums wrap modulo 2^64 */
    distanceSquaredQ24 = (uint64_t)((int64_t)deltaYQ12 * (int64_t)deltaYQ12) +
                         (uint64_t)((int64_t)deltaXQ12 * (int64_t)deltaXQ12) +
                         (uint64_t)((int64_t)deltaZQ12 * (int64_t)deltaZQ12);
    combinedRadiusQ12 = queryRadiusQ12 + sourceRecordCursor->targetRadiusQ12;
    combinedRadiusSquaredQ24 = (uint64_t)((int64_t)combinedRadiusQ12 * (int64_t)combinedRadiusQ12);
    /* signed 64-bit test of combinedRadius^2 - distance^2 >= 0 */
    if ((int64_t)(combinedRadiusSquaredQ24 - distanceSquaredQ24) >= 0) {
      g_GraphicsShadingNearbyRecordCount++;
      *destinationRecordCursor = *sourceRecordCursor;
      destinationRecordCursor++;
    }
    sourceRecordCursor++;
  }
  return;
}

/* Not in the original (it carried the table precomputed): builds g_ShadingIntensityScaleMmx. Light level l gets
   (l * 0x1010) >> 8 = l * 16 + l / 16 (0..0x0FFF) in the alpha lane and zero in the B/G/R lanes. This reproduces
   every entry of the original table. Called once at startup. */
void GraphicsShading_BuildIntensityScaleTable()
{
  int level;

  for (level = 0; level < 256; level++) {
    g_ShadingIntensityScaleMmx[level].blue = 0;
    g_ShadingIntensityScaleMmx[level].green = 0;
    g_ShadingIntensityScaleMmx[level].red = 0;
    g_ShadingIntensityScaleMmx[level].alpha = (uint16_t)((level * 0x1010) >> 8);
  }
}

/* Not in the original: the original executable carries g_PackedLightingLookupTable precomputed
   (512 entries). Each entry holds one light level as four Q12 words for PMULHW: the
   same factor in the three colour lanes and 0x1000 (1.0) in the alpha lane. Levels 0..255 map to
   (level * 0x101) >> 4, i.e. 0..0x0FFF; levels 256..511 saturate at 0x1000. Called once at startup. */
void GraphicsLighting_BuildPackedLookupTable()
{
  int level;

  for (level = 0; level < 512; level++) {
    uint64_t factor = level < 256 ? (uint64_t)((level * COLOR_CHANNEL_TO_WORD_LANE) >> 4) : Q12_ONE;
    g_PackedLightingLookupTable[level] = factor | factor << 16 | factor << 32 | (uint64_t)Q12_ONE << 48;
  }
}
