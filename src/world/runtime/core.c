/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/core.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/runtime/core.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/runtime/core. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift. */
static __inline uint64_t WorldLighting_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
  }
  return lanes.q;
}

/* One lighting color pair: PUNPCKLBW/PSRLW 6 of both colors, PMULHW by the forward and inverse blend
   factors, PADDW, PACKUSWB (low dword). */
static __inline uint32_t WorldLighting_BlendColors
          (uint32_t color,uint32_t alternateColor,SoftwareBgraWordLanes forwardFactors,
          SoftwareBgraWordLanes inverseFactors)

{
  ThandorMmx forwardTerm;
  ThandorMmx inverseTerm;
  uint32_t packed;
  short sum;
  int lane;

  forwardTerm.q = pmulhw(WorldLighting_UnpackBytesShiftRight(color,6),forwardFactors);
  inverseTerm.q = pmulhw(WorldLighting_UnpackBytesShiftRight(alternateColor,6),inverseFactors);
  packed = 0;
  for (lane = 0; lane < 4; lane = lane + 1) {
    sum = (short)(forwardTerm.sw[lane] + inverseTerm.sw[lane]);
    packed = packed | (uint32_t)(sum < 0 ? 0 : (0xff < sum ? 0xff : sum)) << (lane * 8);
  }
  return packed;
}

/* Address: 0x00532FA0.
   Ownership: world/runtime/core.
   Purpose: Interpolates the level lighting color sets and angular parameters from the current runtime phase,
   applies the resulting terrain-lighting configuration, and refreshes field-region normals and lighting.
   Local calls: WorldRuntime_SetTerrainLightingConfiguration, WorldRuntime_RecomputeFieldRegionNormalsAndLighting.
*/

void __thandor_void_preserve_eax_ecx_edx
WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void)

{
  SoftwareBgraWordLanes forwardFactors;
  SoftwareBgraWordLanes inverseFactors;
  PackedArgb32 primaryColorB;
  PackedArgb32 alternateColorA;
  PackedArgb32 alternateColorB;
  InGameLevelConditionStorageView800 *levelConditions;
  uint32_t mixedColor0A;
  uint32_t mixedColor0B;
  uint32_t mixedColor1A;
  uint32_t mixedColor1B;
  uint32_t mixedColor2A;
  uint32_t mixedColor2B;
  uint32_t mixedColor3A;
  uint32_t mixedColor3B;
  uint32_t cycleDurationOrPhase;
  int primaryOriginXWeighted;
  int primaryWidthWeighted;
  uint32_t phaseByteOrAlternateSize;
  uint32_t blendIndexOrPrimaryValue;
  uint32_t alternateOriginOrBlendWeight;
  int alternateOriginXWeighted;
  int alternateWidthWeighted;
  WorldRuntimeContext *worldRuntime;
  int inverseBlendWeight;
  PackedArgb32 primaryColorA;
  
  levelConditions = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  cycleDurationOrPhase = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
          terrainLightingCycleDurationTicks;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  if (cycleDurationOrPhase != 0) {
    cycleDurationOrPhase = (g_GameFactionRuntimeImage.tail.simulationTick % cycleDurationOrPhase << 0x10) / cycleDurationOrPhase;
    blendIndexOrPrimaryValue = g_FixedCosQ28[cycleDurationOrPhase] + 0x10000000U >> 0x15;
    primaryColorA =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainBaseColorArgb;
    primaryColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainRampColor124Argb;
    alternateColorA = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainBaseColorArgb;
    alternateColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainRampColor124Argb;
    forwardFactors = g_SoftwareBilinearForwardFactors[blendIndexOrPrimaryValue];
    inverseFactors = g_SoftwareBilinearInverseFactors[blendIndexOrPrimaryValue];
    mixedColor0A = WorldLighting_BlendColors(primaryColorA,alternateColorA,forwardFactors,inverseFactors);
    mixedColor0B = WorldLighting_BlendColors(primaryColorB,alternateColorB,forwardFactors,inverseFactors);
    primaryColorA =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor128Argb;
    primaryColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainRampColor12CArgb;
    alternateColorA = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor128Argb;
    alternateColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainRampColor12CArgb;
    mixedColor1A = WorldLighting_BlendColors(primaryColorA,alternateColorA,forwardFactors,inverseFactors);
    mixedColor1B = WorldLighting_BlendColors(primaryColorB,alternateColorB,forwardFactors,inverseFactors);
    primaryColorA =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor130Argb;
    primaryColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainLightingColor134Argb;
    alternateColorA = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor130Argb;
    alternateColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor134Argb;
    mixedColor2A = WorldLighting_BlendColors(primaryColorA,alternateColorA,forwardFactors,inverseFactors);
    mixedColor2B = WorldLighting_BlendColors(primaryColorB,alternateColorB,forwardFactors,inverseFactors);
    primaryColorA =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor138Argb;
    primaryColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainLightingColor13CArgb;
    alternateColorA = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor138Argb;
    alternateColorB = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor13CArgb;
    mixedColor3A = WorldLighting_BlendColors(primaryColorA,alternateColorA,forwardFactors,inverseFactors);
    mixedColor3B = WorldLighting_BlendColors(primaryColorB,alternateColorB,forwardFactors,inverseFactors);
    /* A colors without alpha; B colors opaque, except the second one keeps the ramp color's alpha */
    WorldRuntime_SetTerrainLightingConfiguration
              (mixedColor3B | 0xff000000,mixedColor3A & 0xffffff,mixedColor2B | 0xff000000,
               mixedColor2A & 0xffffff,
               mixedColor1B |
               ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
               terrainRampColor12CArgb & 0xff000000,
               mixedColor1A & 0xffffff,mixedColor0B | 0xff000000,mixedColor0A & 0xffffff,worldRuntime);
    phaseByteOrAlternateSize = cycleDurationOrPhase >> 8;
    blendIndexOrPrimaryValue = (uint32_t)(uint16_t)(levelConditions->levelImage).runtimeTail2E0.packedFieldRegionOriginYHigh16XLow16;
    alternateOriginOrBlendWeight = (uint32_t)(uint16_t)(levelConditions->levelImage).runtimeTail2E0.
                           alternatePackedFieldRegionOriginYHigh16XLow16;
    if (phaseByteOrAlternateSize < 0x80) {
      if (alternateOriginOrBlendWeight < blendIndexOrPrimaryValue) {
        alternateOriginOrBlendWeight = alternateOriginOrBlendWeight + 0x10000;
      }
      alternateOriginXWeighted = alternateOriginOrBlendWeight * phaseByteOrAlternateSize;
      primaryOriginXWeighted = blendIndexOrPrimaryValue * (0x80 - phaseByteOrAlternateSize);
    }
    else {
      if (blendIndexOrPrimaryValue < alternateOriginOrBlendWeight) {
        blendIndexOrPrimaryValue = blendIndexOrPrimaryValue + 0x10000;
      }
      primaryOriginXWeighted = blendIndexOrPrimaryValue * (phaseByteOrAlternateSize - 0x80);
      alternateOriginXWeighted = alternateOriginOrBlendWeight * (0x80 - (phaseByteOrAlternateSize - 0x80));
    }
    alternateOriginOrBlendWeight = g_FixedCosQ28[cycleDurationOrPhase] + 0x10000000U >> 0x15;
    inverseBlendWeight = 0x100 - alternateOriginOrBlendWeight;
    blendIndexOrPrimaryValue = (uint32_t)(uint16_t)(levelConditions->levelImage).runtimeTail2E0.
                           packedFieldRegionHeightHigh16WidthLow16;
    cycleDurationOrPhase = cycleDurationOrPhase >> 8;
    phaseByteOrAlternateSize = (uint32_t)(uint16_t)(levelConditions->levelImage).runtimeTail2E0.
                           alternatePackedFieldRegionHeightHigh16WidthLow16;
    if (cycleDurationOrPhase < 0x80) {
      if (phaseByteOrAlternateSize < blendIndexOrPrimaryValue) {
        phaseByteOrAlternateSize = phaseByteOrAlternateSize + 0x10000;
      }
      alternateWidthWeighted = phaseByteOrAlternateSize * cycleDurationOrPhase;
      primaryWidthWeighted = blendIndexOrPrimaryValue * (0x80 - cycleDurationOrPhase);
    }
    else {
      if (blendIndexOrPrimaryValue < phaseByteOrAlternateSize) {
        blendIndexOrPrimaryValue = blendIndexOrPrimaryValue + 0x10000;
      }
      primaryWidthWeighted = blendIndexOrPrimaryValue * (cycleDurationOrPhase - 0x80);
      alternateWidthWeighted = phaseByteOrAlternateSize * (0x80 - (cycleDurationOrPhase - 0x80));
    }
    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
              ((int)((uint32_t)*(uint16_t *)
                            ((int)&(levelConditions->levelImage).runtimeTail2E0.
                                   alternatePackedFieldRegionHeightHigh16WidthLow16 + 2) * inverseBlendWeight +
                    (uint32_t)*(uint16_t *)
                           ((int)&(levelConditions->levelImage).runtimeTail2E0.
                                  packedFieldRegionHeightHigh16WidthLow16 + 2) * (0x100 - inverseBlendWeight))
               >> 8,(uint32_t)(primaryWidthWeighted + alternateWidthWeighted) >> 7 & 0xffff,
               (int)((uint32_t)*(uint16_t *)
                            ((int)&(levelConditions->levelImage).runtimeTail2E0.
                                   alternatePackedFieldRegionOriginYHigh16XLow16 + 2) * inverseBlendWeight +
                    *(uint16_t *)
                     ((int)&(levelConditions->levelImage).runtimeTail2E0.packedFieldRegionOriginYHigh16XLow16
                     + 2) * alternateOriginOrBlendWeight) >> 8,(uint32_t)(primaryOriginXWeighted + alternateOriginXWeighted) >> 7 & 0xffff,worldRuntime);
  }
  return;
}


/* Address: 0x0050D100.
   Ownership: world/runtime/core.
   Purpose: Stores a three-component position at +0x60 through +0x68, computes its exact fixed-point distance from
   +0x80 through +0x88, publishes the result at +0x7C and +0x8C, and clears the field-grid dirty state.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition60AndDistanceFromPosition80
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime)

{
  uint32_t targetDistanceQ12;
  
  (runtime->motion).positionXQ12 = positionX;
  (runtime->motion).positionYQ12 = positionY;
  (runtime->motion).positionZQ12 = positionZ;
  targetDistanceQ12 =
       FixedMath_Length3(positionZ - (runtime->motion).targetPositionZQ12,
                         positionY - (runtime->motion).targetPositionYQ12,
                         positionX - (runtime->motion).targetPositionXQ12);
  (runtime->motion).targetDistanceQ12 = targetDistanceQ12;
  (runtime->motion).committedDistanceQ12 = targetDistanceQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D150.
   Ownership: world/runtime/core.
   Purpose: Stores motion fields +0x6C through +0x78 after enforcing magnitude >=0x400, masking the heading to 16
   bits, and clamping the signed pitch to runtime limits and then to plus or minus 0x4000. Typed parameters: p0
   value78→WorldMotionValue78_V344. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetMotionParameters6CThrough78Clamped
          (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime)

{
  if ((runtime->runtimeFlags & 0x40000) == 0) {
    if ((int)(runtime->motion).maximumPitchAngle < (int)pitchAngle) {
      pitchAngle = (runtime->motion).maximumPitchAngle;
    }
    else if ((int)pitchAngle < (int)(runtime->motion).minimumPitchAngle) {
      pitchAngle = (runtime->motion).minimumPitchAngle;
    }
  }
  if ((int)magnitude < 0x400) {
    magnitude = 0x400;
  }
  if ((int)pitchAngle < 0x4001) {
    if ((int)pitchAngle < -0x4000) {
      pitchAngle = 0xffffc000;
    }
  }
  else {
    pitchAngle = 0x4000;
  }
  (runtime->motion).positionMagnitudeQ12 = magnitude;
  (runtime->motion).headingAngle = headingAngle & 0xffff;
  (runtime->motion).pitchAngle = pitchAngle;
  (runtime->motion).motionValue78 = value78;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D1E0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime set position80 and rebuild position60 from angles.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime)

{
  FixedDirectionXZEdxEax8 positionOffsetXZQ12;
  FixedDirection directionOffset;
  
  (runtime->motion).targetPositionXQ12 = originX;
  (runtime->motion).targetPositionYQ12 = originY;
  (runtime->motion).targetPositionZQ12 = originZ;
  (runtime->motion).pitchAngle = pitchAngle;
  (runtime->motion).headingAngle = headingAngle;
  (runtime->motion).targetDistanceQ12 = distance;
  (runtime->motion).committedDistanceQ12 = distance;
  directionOffset = FixedMath_DirectionFromAnglesScaledRegs(-pitchAngle,headingAngle ^ 0x8000,distance);
  (runtime->motion).positionXQ12 = directionOffset.x + (runtime->motion).targetPositionXQ12;
  (runtime->motion).positionYQ12 = directionOffset.y + (runtime->motion).targetPositionYQ12;
  (runtime->motion).positionZQ12 = directionOffset.z + (runtime->motion).targetPositionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D2C0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime restore motion state from snapshot.
   Local calls: WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 snapshotHeadingAngle;
  AngleTurn32 snapshotPitchAngle;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  UQ12 snapshotDistanceQ12;
  
  snapshotPositionYQ12 = (worldRuntime->snapshot).positionYQ12;
  snapshotPositionZQ12 = (worldRuntime->snapshot).positionZQ12;
  (worldRuntime->motion).positionXQ12 = (worldRuntime->snapshot).positionXQ12;
  (worldRuntime->motion).positionYQ12 = snapshotPositionYQ12;
  (worldRuntime->motion).positionZQ12 = snapshotPositionZQ12;
  snapshotHeadingAngle = (worldRuntime->snapshot).headingAngle;
  snapshotPitchAngle = (worldRuntime->snapshot).pitchAngle;
  snapshotDistanceQ12 = (worldRuntime->snapshot).distanceQ12;
  (worldRuntime->motion).positionMagnitudeQ12 = (worldRuntime->snapshot).magnitudeQ12;
  (worldRuntime->motion).headingAngle = snapshotHeadingAngle;
  (worldRuntime->motion).pitchAngle = snapshotPitchAngle;
  (worldRuntime->motion).targetDistanceQ12 = snapshotDistanceQ12;
  (worldRuntime->motion).committedDistanceQ12 = snapshotDistanceQ12;
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  return;
}


/* Address: 0x0050D670.
   Ownership: world/runtime/core.
   Purpose: Accepts only an asset whose first dword is the little-endian fld signature. A valid asset is stored at
   context offset 0x54, prepared through the existing field helper, and followed by clearing context flag
   0x00000800.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FieldGrid_RecomputeInteriorTriangleNormalAngles [world/terrain/grid].
*/
void __thandor_preserve_eax
WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

{
  if ((asset->common).magic == ASSET_MAGIC_FLD) {
    world->fieldGrid = asset;
    FieldGrid_RecomputeInteriorTriangleNormalAngles(asset);
    WorldRuntime_ClearFieldGridDirtyFlag(world);
  }
  return;
}


/* Address: 0x00561E30.
   Ownership: world/runtime/core.
   Purpose: Adjusts the active field origin by signed Y and X deltas, wraps X to sixteen bits, clamps Y to the
   verified range -0x4000 through -0x1000, and reapplies the world-runtime origin state. Typed parameters: p4
   deltaWorldY→Q12, p5 deltaWorldX→Q12. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: WorldRuntime_RecomputeFieldRegionNormalsAndLighting.
*/
void __thandor_preserve_eax_edx
WorldRuntime_AdjustFieldOriginWrappedClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaWorldY,Q12 deltaWorldX)

{
  FieldGridDimensionCells gridHeight;
  
  gridHeight = deltaWorldY + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionHeight;
  if (-0x1000 < gridHeight) {
    gridHeight = -0x1000;
  }
  if (gridHeight < -0x4000) {
    gridHeight = -0x4000;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            (gridHeight,
             deltaWorldX + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionWidth & 0xffff,
             g_InGameRuntimeRoot->fieldRegionOriginWorldYQ12_0BAC,
             g_InGameRuntimeRoot->fieldRegionOriginWorldXQ12_0BA8,
             &g_InGameRuntimeRoot->worldRuntime0A30);
  return;
}


/* Address: 0x004BE760.
   Ownership: world/runtime/core.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid].
*/
Q12 WorldRuntime_InterpolateTerrainHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 interpolatedHeightQ12;
  HeightSampleResult heightResult;
  
  interpolatedHeightQ12 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    heightResult = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    interpolatedHeightQ12 = heightResult.heightQ12;
  }
  return interpolatedHeightQ12;
}


/* Address: 0x004BE790.
   Ownership: world/runtime/core.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FieldGrid_InterpolateWaterSurfaceHeight [world/terrain/grid].
*/
Q12 WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 waterSurfaceHeightQ12;
  HeightSampleResult heightResult;
  
  waterSurfaceHeightQ12 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    heightResult = FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    waterSurfaceHeightQ12 = heightResult.heightQ12;
  }
  return waterSurfaceHeightQ12;
}


/* Address: 0x004BE7C0.
   Ownership: world/runtime/core.
   Purpose: Returns the interpolated field-grid top surface height or the 0x7FFFF000 sentinel when the runtime has
   no field grid.
   Cross-module calls: FieldGrid_InterpolateTopSurfaceHeight [world/terrain/grid].
*/
uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  uint32_t topSurfaceHeightQ12;
  HeightSampleResult heightResult;
  
  topSurfaceHeightQ12 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    heightResult = FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    topSurfaceHeightQ12 = heightResult.heightQ12;
  }
  return topSurfaceHeightQ12;
}


/* Address: 0x0050A610.
   Ownership: world/runtime/core.
   Purpose: Transforms one runtime-node position to integer grid coordinates and returns carry set only when both
   coordinates lie inside the four inclusive bounds stored at offsets 0x160-0x16C. Typed parameters: p3
   boundsControl→WorldRuntimeExtendedMapControlAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
WorldRuntimeNode_IsPositionInsideBoundsCf
          (WorldOwnerListNode100 *runtimeNode,WorldRuntimeExtendedMapControlView170 *boundsControl)

{
  int boundsSecondX;
  int boundsSecondY;
  int projectedGridX;
  int boundsMaxX;
  int projectedGridY;
  int boundsMinX;
  int boundsMaxY;
  int boundsMinY;
  GraphicsProjectedPointEdxEax8 projectedPositionPair;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsProjectionScratchVec3,(GraphicsFixedVec3 *)&runtimeNode->worldXQ12,
             &g_ViewProjectionMatrixFixed);
  projectedPositionPair =
       THANDOR_BITCAST(GraphicsProjectedPointPair, GraphicsProjectedPointEdxEax8, Graphics_ProjectViewPoint(&g_GraphicsProjectionScratchVec3));
  boundsMinX = boundsControl->extendedCoordinate160;
  boundsSecondX = boundsControl->extendedCoordinate168;
  boundsMinY = boundsControl->extendedCoordinate164;
  boundsSecondY = boundsControl->extendedCoordinate16C;
  projectedGridX = (int)projectedPositionPair >> 0xc;
  projectedGridY = (int)((int64_t)projectedPositionPair >> 0x2c);
  boundsMaxX = boundsSecondX;
  if (boundsSecondX < boundsMinX) {
    boundsMaxX = boundsMinX;
    boundsMinX = boundsSecondX;
  }
  boundsMaxY = boundsSecondY;
  if (boundsSecondY < boundsMinY) {
    boundsMaxY = boundsMinY;
    boundsMinY = boundsSecondY;
  }
  if ((((boundsMinX <= projectedGridX) && (projectedGridX <= boundsMaxX)) && (boundsMinY <= projectedGridY)) &&
     (projectedGridY <= boundsMaxY)) {
    return true;
  }
  return false;
}


/* Address: 0x0050D260.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime capture motion state to snapshot.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime)

{
  UQ12 snapshotDistanceQ12;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  AngleTurn32 snapshotHeadingAngle;
  AngleTurn32 snapshotPitchAngle;
  
  snapshotPositionYQ12 = (worldRuntime->motion).positionYQ12;
  snapshotPositionZQ12 = (worldRuntime->motion).positionZQ12;
  (worldRuntime->snapshot).positionXQ12 = (worldRuntime->motion).positionXQ12;
  (worldRuntime->snapshot).positionYQ12 = snapshotPositionYQ12;
  (worldRuntime->snapshot).positionZQ12 = snapshotPositionZQ12;
  snapshotHeadingAngle = (worldRuntime->motion).headingAngle;
  snapshotPitchAngle = (worldRuntime->motion).pitchAngle;
  snapshotDistanceQ12 = (worldRuntime->motion).committedDistanceQ12;
  (worldRuntime->snapshot).magnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
  (worldRuntime->snapshot).headingAngle = snapshotHeadingAngle;
  (worldRuntime->snapshot).pitchAngle = snapshotPitchAngle;
  (worldRuntime->snapshot).distanceQ12 = snapshotDistanceQ12;
  return;
}


/* Address: 0x0050D330.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime motion state matches snapshot carry-flag result.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_MotionStateMatchesSnapshotCf(WorldRuntimeContext *worldRuntime)

{
  if ((((worldRuntime->motion).positionXQ12 == (worldRuntime->snapshot).positionXQ12) &&
      ((worldRuntime->motion).positionYQ12 == (worldRuntime->snapshot).positionYQ12)) &&
     ((worldRuntime->motion).positionZQ12 == (worldRuntime->snapshot).positionZQ12)) {
    if ((((worldRuntime->motion).positionMagnitudeQ12 == (worldRuntime->snapshot).magnitudeQ12) &&
        ((worldRuntime->motion).headingAngle == (worldRuntime->snapshot).headingAngle)) &&
       (((worldRuntime->motion).pitchAngle == (worldRuntime->snapshot).pitchAngle &&
        ((worldRuntime->motion).targetDistanceQ12 == (worldRuntime->snapshot).distanceQ12)))) {
      return;
    }
  }
  return;
}


/* Address: 0x0050D4F0.
   Ownership: world/runtime/core.
   Purpose: Copies the dword at context offset 0x8C into offset 0x7C. The surrounding world-runtime layout remains
   opaque.
*/
void __thandor_preserve_eax WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world)

{
  (world->motion).committedDistanceQ12 = (world->motion).targetDistanceQ12;
  return;
}


/* Address: 0x0050D510.
   Ownership: world/runtime/core.
   Purpose: Stores arrayBase at context offset 0x58 and count at offset 0xAC. Verified callers attach arrays
   containing 0x100 or 0x4000 entries.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world)

{
  world->objectArray = objectArray;
  world->objectCount = count;
  return;
}


/* Address: 0x0050D540.
   Ownership: world/runtime/core.
   Purpose: Replaces the dword at context offset 0xCC with flags. Typed parameters: p0 flags→WorldRuntimeFlags.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = flags;
  return;
}


/* Address: 0x0050D560.
   Ownership: world/runtime/core.
   Purpose: ORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags | flags;
  return;
}


/* Address: 0x0050D580.
   Ownership: world/runtime/core.
   Purpose: Clears every bit selected by flags from the dword at context offset 0xCC. Typed parameters: p0
   flags→WorldRuntimeFlags. Nearby but non-identical semantic domains were explicitly deferred. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags & ~flags;
  return;
}


/* Address: 0x0050D5A0.
   Ownership: world/runtime/core.
   Purpose: XORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags ^ flags;
  return;
}


/* Address: 0x0050D610.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x60, 0x64, and 0x68 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
WorldVector0EaxEcxEdx12 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world)

{
  WorldVector0EaxEcxEdx12 positionVector;
  
  /* Ghidra split the ECX/EDX halves of the return into uVar2._4_4_ and register0x00000008. */
  positionVector.xQ12 = (world->motion).positionXQ12;
  positionVector.yQ12 = (world->motion).positionYQ12;
  positionVector.zQ12 = (world->motion).positionZQ12;
  return positionVector;
}


/* Address: 0x0050D630.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x6C, 0x70, and 0x74 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
WorldVector1EaxEcxEdx12 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world)

{
  WorldVector1EaxEcxEdx12 motionVector;
  
  /* Ghidra split the ECX/EDX halves of the return into uVar2._4_4_ and register0x00000008. */
  motionVector.magnitudeQ12 = (world->motion).positionMagnitudeQ12;
  motionVector.headingAngle = (world->motion).headingAngle;
  motionVector.pitchAngle = (world->motion).pitchAngle;
  return motionVector;
}


/* Address: 0x0050D650.
   Ownership: world/runtime/core.
   Purpose: Returns the dword at context offset 0xCC in EAX and explicitly clears CF.
*/
WorldFlagsResult __thandor_eax_cf_preserve_ecx_edx
WorldRuntime_GetFlagsCf(WorldRuntimeContext *world)

{
  WorldFlagsResult flagsResult;
  
  flagsResult.failed = false;
  flagsResult.flags = world->runtimeControlFlags;
  return flagsResult;
}


/* Address: 0x0050D6A0.
   Ownership: world/runtime/core.
   Purpose: Returns the FieldGridAsset pointer stored at context offset 0x54.
*/
FieldGridAsset * WorldRuntime_GetFieldGridAsset(WorldRuntimeContext *world)

{
  return world->fieldGrid;
}

/* Address: 0x0050D6D0.
   Ownership: world/runtime/core.
   Purpose: Returns the dword at context offset 0x5C without modifying it.
*/
uint32_t WorldRuntime_GetPendingToken(WorldRuntimeContext *world)

{
  return world->pendingToken;
}

/* Address: 0x0050D6F0.
   Ownership: world/runtime/core.
   Purpose: Atomically exchanges the dword at context offset 0x5C with zero and returns the previous value in EAX.
*/
uint32_t WorldRuntime_TakePendingToken(WorldRuntimeContext *world)

{
  uint32_t pendingToken;
  
  LOCK();
  pendingToken = world->pendingToken;
  world->pendingToken = 0;
  UNLOCK();
  return pendingToken;
}

/* Address: 0x0050D710.
   Ownership: world/runtime/core.
   Purpose: Stores array at context offset 0xC0 and count at 0xC4, then clears exactly count dwords beginning at
   array.
*/
void __thandor_void_preserve_eax_ecx
WorldRuntime_AttachAndClearDwordArray
          (WorldWorkspaceElementCount count,uint32_t *array,WorldRuntimeContext *world)

{
  world->dwordArray = array;
  world->dwordArrayCount = count;
  for (; count != 0; count = count - 1) {
    *array = 0;
    array = array + 1;
  }
  return;
}


/* Address: 0x0050D740.
   Ownership: world/runtime/core.
   Purpose: Returns the dword-array pointer stored at context offset 0xC0.
*/
uint32_t * WorldRuntime_GetDwordArray(WorldRuntimeContext *world)

{
  return world->dwordArray;
}

/* Address: 0x0050D7D0.
   Ownership: world/runtime/core.
   Purpose: Scans the attached fixed-size 0x100-byte object records for a slot without allocation bit 0x40000000,
   marks the selected slot, stores its owning world runtime, and reports exhaustion or success through carry.
*/
WorldObjectAllocResult __thandor_eax_cf_preserve_ecx_edx
WorldObjectArray_AllocateFreeRecordCf(WorldRuntimeContext *worldRuntime)

{
  WorldObjectRecordCount recordsRemaining;
  WorldObjectRecord *recordCursor;
  WorldObjectAllocResult exhaustedResult;
  WorldObjectAllocResult allocatedResult;
  
  recordsRemaining = worldRuntime->objectCount;
  recordCursor = worldRuntime->objectArray;
  while( true ) {
    if (recordsRemaining == 0) {
      exhaustedResult.failed = true;
      exhaustedResult.recordOrError = (WorldObjectRecord *)0x14;
      return exhaustedResult;
    }
    if (((recordCursor->common).allocationFlags & 0x40000000) == 0) break;
    recordCursor = recordCursor + 1;
    recordsRemaining = recordsRemaining - 1;
  }
  (recordCursor->common).allocationFlags = 0x40000000;
  (recordCursor->common).ownerWorld = worldRuntime;
  allocatedResult.failed = false;
  allocatedResult.recordOrError = recordCursor;
  return allocatedResult;
}


/* Address: 0x0050D830.
   Ownership: world/runtime/core.
   Purpose: Sets runtime flag 0x80000000 and atomically inserts the node at the head pointer stored at owner +0xD8,
   maintaining previous and next links at node +0x00 and +0x04.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_LinkNodeIntoOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 **ownerListHeadLink;
  WorldOwnerListNode100 *previousHeadNode;
  WorldRuntimeContext *ownerWorld;
  
  ownerWorld = node->ownerWorld;
  node->runtimeFlags = node->runtimeFlags | 0x80000000;
  LOCK();
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  UNLOCK();
  node->previousNode = (WorldOwnerListNode100 *)0x0;
  node->nextNode = previousHeadNode;
  if (previousHeadNode != (WorldOwnerListNode100 *)0x0) {
    previousHeadNode->previousNode = node;
  }
  return;
}


/* Address: 0x0050D880.
   Ownership: world/runtime/core.
   Purpose: When linked, removes the node from the owner +0xD8 intrusive list, repairs both neighbors or the head
   pointer, then clears the complete runtime flag dword at +0x4C.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_UnlinkNodeFromOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 *previousNode;
  WorldOwnerListNode100 *nextNode;
  
  if ((node->runtimeFlags & 0x80000000) != 0) {
    previousNode = node->previousNode;
    nextNode = node->nextNode;
    if (previousNode == (WorldOwnerListNode100 *)0x0) {
      node->ownerWorld->ownerListHead = nextNode;
    }
    else {
      previousNode->nextNode = nextNode;
    }
    if (nextNode != (WorldOwnerListNode100 *)0x0) {
      nextNode->previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
  return;
}


/* Address: 0x0050D8F0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime for each node in owner list d8.
*/
void __thandor_preserve_eax_edx
WorldRuntime_ForEachNodeInOwnerListD8
          (void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world)

{
  WorldOwnerListNode100 *node;
  
  for (node = world->ownerListHead; node != (WorldOwnerListNode100 *)0x0; node = node->nextNode) {
    (*callback)(callbackContext,node);
  }
  return;
}


/* Address: 0x0050EC80.
   Ownership: world/runtime/core.
   Purpose: Returns g_GraphicsShadingRuntimeRecords in EAX and byte count 0x4000 in EDX, then inverts record zero
   serializationToggleDword before light.hex serialization.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl RuntimeHexSegment_GetLightImageAndToggleFlagRegs(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  /* EDX:EAX = byte size 0x4000, shading runtime records */
  return ((uint64_t)0x4000 << 32) | (uint32_t)(uintptr_t)g_GraphicsShadingRuntimeRecords;
}

/* Address: 0x0050ECA0.
   Ownership: world/runtime/core.
   Purpose: Inverts record zero serializationToggleDword after light.hex serialization while the caller preserves
   serializer flags.
*/
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  return;
}

/* Address: 0x0050ECB0.
   Ownership: world/runtime/core.
   Purpose: Missed pre-serializer provider for field.hex. From context +0x54 it returns the attached field-image
   pointer in EAX and that image's complete allocation size at +0x04 in EDX.
*/
ResourceRegistrationImagePair
RuntimeHexSegment_GetFieldImageRegs(InGameFieldImageSaveContext58 *fieldImageContext)

{
  /* The pair is stored as {low: byte count (EDX), high: image (EAX)}, as the caller reads it. */
  return (uint64_t)(uintptr_t)fieldImageContext->fieldGridAsset << 0x20 |
         (uint64_t)(uint32_t)(fieldImageContext->fieldGridAsset->common).allocationSizeBytes;
}

/* Address: 0x0050ECD0.
   Ownership: world/runtime/core.
   Purpose: One-argument field.hex post-serializer hook. It is an exact no-op invoked while the caller preserves
   the serializer CF result with PUSHF/POPF.
*/
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext)

{
  return;
}

/* Address: 0x0051BFA0.
   Ownership: world/runtime/core.
   Purpose: Owner-list callback that clears runtime-node fields which still reference a model or object being
   released. The node representation is selected by its verified kind field at +0xA4.
   Cross-module calls: ModelRuntimeHierarchy_ClearMatchingTargetRecursive [world/model/hierarchy].
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode100 *node)

{
  int *modelRuntime;
  int linkedRuntimeStateAddress;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    modelRuntime = node->runtimePayload;
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive((RuntimeToken)releasedObject,modelRuntime);
    linkedRuntimeStateAddress = modelRuntime[2];
    if (releasedObject == *(void **)(linkedRuntimeStateAddress + 0x98)) {
      *(uint32_t *)(linkedRuntimeStateAddress + 0x98) = 0;
    }
    if (((*(uint32_t *)(linkedRuntimeStateAddress + 0x2c) & 1) != 0) &&
       (releasedObject == *(void **)(linkedRuntimeStateAddress + 0x1c))) {
      *(uint32_t *)(linkedRuntimeStateAddress + 0x1c) = 0;
      *(uint32_t *)(linkedRuntimeStateAddress + 0x2c) =
           *(uint32_t *)(linkedRuntimeStateAddress + 0x2c) & 0xfffffff2;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) &&
          (releasedObject == *(void **)((int)node->runtimePayload + 0x1c))) {
    *(uint32_t *)((int)node->runtimePayload + 0x1c) = 0;
  }
  return;
}


/* Address: 0x0051D500.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime emit model definition overlay for matching entries.
   Cross-module calls: ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
          (void *sourceRuntime,WorldRuntimeContext *worldRuntime)

{
  uint32_t overlayBaseOffset;
  TerrainClassOverlayCallback *overlayCallback;
  int modelOverlayBase;
  ModelDefinitionRecordPrefix *definitionRecord;
  WorldOwnerListNode100 *ownerNode;
  ModelDefinitionResult definitionLookup;
  uint32_t overlayExtent;
  
  if (sourceRuntime != (void *)0x0) {
    definitionLookup = ModelDefinitionRegistry_FindByIdWithErrorCf
                      (*(PckModelDefinitionIdCatalog *)(*(int *)((int)sourceRuntime + 0xc) + 0x20));
    definitionRecord = definitionLookup.modelDefinition;
    if (!definitionLookup.notFound) {
      overlayExtent = 0xffffffff;
      ownerNode = worldRuntime->ownerListHead;
      overlayBaseOffset = definitionRecord[0x23].flags;
      if (ownerNode != (WorldOwnerListNode100 *)0x0) {
        if (definitionRecord[6].flags == 0xe) {
          overlayExtent = 0x800 << ((uint8_t)definitionRecord[0x10].byteSize & 0x1f);
        }
        overlayCallback = g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
                 [definitionRecord[0x34].definitionId];
        do {
          if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
              (worldRuntime->activeFactionRuntimeIndex ==
               *(int *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc))) &&
             (modelOverlayBase = *(int *)(*(int *)ownerNode->runtimePayload + 0x19c), modelOverlayBase != 0)) {
            (*overlayCallback)(overlayExtent,-1,modelOverlayBase + overlayBaseOffset,ownerNode->worldYQ12,ownerNode->worldXQ12,
                      worldRuntime->fieldGrid);
          }
          ownerNode = ownerNode->nextNode;
        } while (ownerNode != (WorldOwnerListNode100 *)0x0);
      }
    }
  }
  return;
}


/* Address: 0x005233F0.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument no-op installed in unified runtime table method slot 5. It preserves the existing
   EAX and flags contract and returns with ret 0x08. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
*/
void UnifiedRuntimeTable_Method5_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00523400.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument no-op installed in unified runtime table method slot 6. It preserves the existing
   EAX and flags contract and returns with ret 0x08. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
*/
void UnifiedRuntimeTable_Method6_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00527B70.
   Ownership: world/runtime/core.
   Purpose: Third exact one-argument no-op reused across many unified runtime object method-table entries. It
   returns with ret 0x04. Model-unrebase partition slots 48-71 receive one ModelRuntimeSlot pointer.
*/
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

{
  return;
}


/* Address: 0x00527BA0.
   Ownership: world/runtime/core.
   Purpose: Second exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Model release partition slots 0-23 receive (modelDefinition, modelRuntime).
*/
void UnifiedRuntimeDefault_TwoArgNoOpB
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Address: 0x00527BB0.
   Ownership: world/runtime/core.
   Purpose: Exact one-argument default that clears EAX and returns zero with ret 0x04.
*/
uint32_t __thandor_eax_preserve_ecx_edx UnifiedRuntimeDefault_OneArgReturnZero(void *context)

{
  return 0;
}


/* Address: 0x00527BE0.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument default that returns CF clear with ret 0x08 while preserving EAX. Placement-
   validation partition slots 24-47 receive (worldRuntime, armyRuntime), with CF carrying acceptance.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgSuccessCf
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  return false;
}


/* Address: 0x00527BF0.
   Ownership: world/runtime/core.
   Purpose: Fourth exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Address: 0x00529430.
   Ownership: world/runtime/core.
   Purpose: Owner-list callback that clears kind-specific entity/model references before the referenced runtime
   hierarchy is detached and released.
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearDetachedEntityReferencesCallback
          (void *detachedObject,WorldOwnerListNode100 *node)

{
  int *entityRuntimePayloadWords;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    if (detachedObject == *(void **)((int)node->runtimePayload + 0x1c)) {
      *(uint32_t *)((int)node->runtimePayload + 0x1c) = 0;
    }
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    entityRuntimePayloadWords = node->runtimePayload;
    if (detachedObject == (void *)entityRuntimePayloadWords[0x3c]) {
      entityRuntimePayloadWords[0x3c] = 0;
    }
    if ((*(int *)(*entityRuntimePayloadWords + 0x4c) == 0x15) &&
       (detachedObject == (void *)entityRuntimePayloadWords[0x18])) {
      entityRuntimePayloadWords[0x18] = 0;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          (detachedObject == *(void **)((int)node->runtimePayload + 0x14))) {
    *(uint32_t *)((int)node->runtimePayload + 0x14) = 0;
  }
  return;
}


/* Address: 0x00565110.
   Ownership: world/runtime/core.
   Purpose: In-game shutdown owner-list callback. It releases kind-0 bindings and clears the verified kind-1 and
   kind-2 back-reference fields before level resources are destroyed.
   Cross-module calls: ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ReleaseShutdownBindingsCallback
          (WorldRuntimeContext *shutdownContext,WorldOwnerListNode100 *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    ArmyRuntime_DestroyInstanceAndRefreshUi
              (shutdownContext,*(GameEntityRuntime **)((int)node->runtimePayload + 8));
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) {
    node->runtimeFlags = node->runtimeFlags & 0x3fffffff;
    *(uint32_t *)((int)node->runtimePayload + 0x10) = 0;
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    node->runtimeFlags = node->runtimeFlags & 0x3fffffff;
    *(uint32_t *)((int)node->runtimePayload + 4) = 0;
  }
  return;
}


/* Address: 0x0050D3B0.
   Ownership: world/runtime/core.
   Purpose: Raycasts the terrain and secondary field surfaces according to the runtime surface-selection flag,
   chooses the nearest accepted travel distance, rebuilds the endpoint coordinates and distance, and clears the
   field-grid dirty flag.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FieldGrid_RaycastTerrainSurfaceDistanceCf [world/terrain/grid], FixedMath_SinCosScaled
   [core/math/fixed], FixedMath_Length3 [core/math/fixed], FieldGrid_RaycastSecondarySurfaceDistanceCf
   [world/terrain/grid], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 currentPitchAngle;
  UQ12 scale;
  uint32_t endpointDistanceQ12;
  int rayLengthOrOffsetY;
  FixedSinCosEdxEax8 groundOffsetXY;
  TerrainRaycastResult raycastResult;
  TerrainRaycastResult secondaryRaycastResult;
  FixedDirection endpointOffset;
  
  if ((worldRuntime->runtimeFlags & 0x1000000) == 0) {
    rayLengthOrOffsetY = worldRuntime->maximumCameraDistanceQ12 << 2;
    raycastResult = FieldGrid_RaycastTerrainSurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,rayLengthOrOffsetY,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    scale = raycastResult.distanceQ12;
    if (raycastResult.hit) {
      /* terrain hit: a nearer secondary-surface hit wins */
      secondaryRaycastResult = FieldGrid_RaycastSecondarySurfaceDistanceCf
                        ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,rayLengthOrOffsetY,
                         (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                         (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
      if ((secondaryRaycastResult.hit) && (secondaryRaycastResult.distanceQ12 < (int)scale)) {
        scale = secondaryRaycastResult.distanceQ12;
      }
    }
  }
  else {
    raycastResult = FieldGrid_RaycastSecondarySurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       worldRuntime->maximumCameraDistanceQ12 << 2,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    scale = raycastResult.distanceQ12;
  }
  if (!raycastResult.hit) {
    /* no hit: intersect the view ray with the ground plane z = 0 */
    currentPitchAngle = (worldRuntime->motion).pitchAngle;
    groundOffsetXY = FixedMath_SinCosScaled
                      ((worldRuntime->motion).headingAngle,
                       (FixedMathScale32)
                       (((int64_t)(worldRuntime->motion).positionZQ12 *
                        (int64_t)g_FixedCosQ28[-currentPitchAngle]) / (int64_t)g_FixedSinQ28[-currentPitchAngle]));
    rayLengthOrOffsetY = (int)(groundOffsetXY >> 0x20);
    (worldRuntime->motion).targetPositionXQ12 = (int)groundOffsetXY + (worldRuntime->motion).positionXQ12;
    (worldRuntime->motion).targetPositionYQ12 = rayLengthOrOffsetY + (worldRuntime->motion).positionYQ12;
    (worldRuntime->motion).targetPositionZQ12 = 0;
    endpointDistanceQ12 = FixedMath_Length3((worldRuntime->motion).positionZQ12,rayLengthOrOffsetY,(int)groundOffsetXY);
    (worldRuntime->motion).targetDistanceQ12 = endpointDistanceQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
    return;
  }
  (worldRuntime->motion).targetDistanceQ12 = scale;
  endpointOffset = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,scale);
  (worldRuntime->motion).targetPositionXQ12 = endpointOffset.x + (worldRuntime->motion).positionXQ12;
  (worldRuntime->motion).targetPositionYQ12 = endpointOffset.y + (worldRuntime->motion).positionYQ12;
  (worldRuntime->motion).targetPositionZQ12 = endpointOffset.z + (worldRuntime->motion).positionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050D760.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime set terrain lighting configuration.
   Cross-module calls: TerrainLighting_BuildColorRampAndSetBaseColor [world/terrain/visuals].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetTerrainLightingConfiguration
          (PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 rampColor12CArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 rampColor124Argb,PackedArgb32 baseColorArgb,WorldRuntimeContext *worldRuntime
          )

{
  (worldRuntime->lighting).color130Argb = lightingColor130Argb;
  (worldRuntime->lighting).color134Argb = lightingColor134Argb;
  (worldRuntime->lighting).color128Argb = lightingColor128Argb;
  (worldRuntime->lighting).color138Argb = lightingColor138Argb;
  (worldRuntime->lighting).color13CArgb = lightingColor13CArgb;
  (worldRuntime->lighting).baseColorArgb = baseColorArgb;
  (worldRuntime->lighting).rampColorArgb = rampColor124Argb;
  (worldRuntime->lighting).color12CArgb = rampColor12CArgb;
  TerrainLighting_BuildColorRampAndSetBaseColor(rampColor12CArgb,rampColor124Argb,baseColorArgb);
  return;
}


/* Address: 0x0050D5C0.
   Ownership: world/runtime/core.
   Purpose: Typed parameters: p2 gridHeight→FieldGridDimensionCells_V343, p3
   gridWidth→FieldGridDimensionCells_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FieldGrid_RecomputeInteriorTriangleNormalAngles [world/terrain/grid],
   FieldGrid_RecomputeInteriorDirectionalLighting [world/terrain/grid].
*/
void __thandor_void_preserve_ecx_edx
WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells gridHeight,FieldGridDimensionCells gridWidth,Q12 originWorldYQ12,
          Q12 originWorldXQ12,WorldRuntimeContext *worldRuntime)

{
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x1c) = originWorldXQ12;
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x20) = originWorldYQ12;
  FieldGrid_RecomputeInteriorTriangleNormalAngles(worldRuntime->fieldGrid);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (originWorldYQ12,originWorldXQ12,worldRuntime->fieldGrid);
  (worldRuntime->fieldRegion).regionWidth = gridWidth;
  (worldRuntime->fieldRegion).regionHeight = gridHeight;
  return;
}


/* Address: 0x0050D6B0.
   Ownership: world/runtime/core.
   Purpose: Clears bit 0x00000800 in the dword at context offset 0x4C.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world)

{
  world->runtimeFlags = world->runtimeFlags & 0xfffff7ff;
  return;
}

