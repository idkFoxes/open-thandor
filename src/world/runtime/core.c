/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/core.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/runtime/core.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/runtime/core. */

/* Not a function of its own in the original: PUNPCKLBW mm,mm then PSRLW mm,shift, i.e. the four bytes b
   of value as the words ((b << 8) | b) >> shift (b * 0x101 widens a byte to the full 16-bit range). */
static __inline uint64_t WorldLighting_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
  }
  return lanes.q;
}

/* Not a function of its own in the original: the inlined MMX sequence that blends one colour pair of
   WorldLightingRuntime_UpdateInterpolatedTerrainLighting per byte, color * forward + alternateColor * inverse
   with unsigned saturation (PUNPCKLBW/PSRLW 6 of both colours, PMULHW by the factors, PADDW, PACKUSWB). */
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
  for (lane = 0; lane < 4; lane++) {
    sum = (short)(forwardTerm.sw[lane] + inverseTerm.sw[lane]);
    packed = packed | (uint32_t)(sum < 0 ? 0 : (0xff < sum ? 0xff : sum)) << (lane * 8);
  }
  return packed;
}

/* Address: 0x00532FA0.
   Periodic terrain lighting cycle (tick-wheel case 0, plus two session setup paths): when the level
   defines a cycle duration, the simulation tick's phase in the cycle picks a cosine blend between the
   level's primary and alternate terrain colour sets and between two packed 16-bit parameter pairs,
   installs the blended colours and recomputes the terrain normals and lighting with the blended pairs.
   At phase 0 the blend index is 256 if the cosine table holds exactly 1.0 there: one past the declared
   256-entry factor tables (as in the original).
*/
void WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void)

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
    /* phase in the cycle as a 16-bit angle; its cosine (Q28, -1..1) becomes a blend index 0..256 */
    cycleDurationOrPhase = (g_GameFactionRuntimeImage.tail.simulationTick % cycleDurationOrPhase << 16) / cycleDurationOrPhase;
    blendIndexOrPrimaryValue = g_FixedCosQ28[cycleDurationOrPhase] + 0x10000000U >> 21;
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
    /* Low 16 bits of the pairs: triangular blend over the phase byte (0x80 = half cycle); the value that
       would lie below the other one gets 0x10000 added, so the blend runs forward through the 16-bit wrap.
       High 16 bits: the same cosine weight as the colours. */
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
    alternateOriginOrBlendWeight = g_FixedCosQ28[cycleDurationOrPhase] + 0x10000000U >> 21;
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
}


/* Address: 0x0050D100.
   Moves the camera (motion.position, +0x60) to the given point and keeps its target point (+0x80): the
   target and committed distances become the new distance between the two.
*/
void WorldRuntime_SetPosition60AndDistanceFromPosition80
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
   Sets the camera's magnitude (at least 0x400 = 0.25 in Q12), heading (16-bit turn) and pitch and the
   motion value at +0x78. The pitch is clamped to the world's pitch limits (unless the camera is unlimited)
   and always to a quarter turn up or down (+-0x4000).
*/
void WorldRuntime_SetMotionParameters6CThrough78Clamped
          (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime)

{
  if ((runtime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA) == 0) {
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
      pitchAngle = 0xffffc000; /* -0x4000 */
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
   Points the camera at a target: stores the target point (motion.targetPosition, +0x80), pitch, heading and
   distance, and places the camera (motion.position, +0x60) that distance away from the target, looking at it
   along the given angles (the offset uses the reversed direction: negated pitch, heading + half a turn).
*/
void WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime)

{
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
   Restores the camera saved by WorldRuntime_CaptureMotionStateToSnapshot (position, magnitude, angles,
   distance) and recomputes its target point where the view ray meets the field.
*/
void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime)

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
   Attaches a field grid ('fld' asset) to the world and computes its triangle normals; any other asset is
   ignored.
*/
void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

{
  if ((asset->common).magic == ASSET_MAGIC_FLD) {
    world->fieldGrid = asset;
    FieldGrid_RecomputeInteriorTriangleNormalAngles(asset);
    WorldRuntime_ClearFieldGridDirtyFlag(world);
  }
  return;
}


/* Address: 0x00561E30.
   Keyboard command of the in-game root (called directly in a local game, in a networked one queued as
   command 0x2D00 from InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags): turns the
   auxiliary angle pair (stored in fieldRegion.regionHeight/regionWidth, see
   WorldRuntime_RecomputeFieldRegionNormalsAndLighting) by the given deltas, the elevation clamped to
   -0x4000..-0x1000 and the azimuth wrapped to 16 bits, and relights the field with the unchanged light
   direction. The name is historical: nothing here is a field origin.
*/
void WorldRuntime_AdjustFieldOriginWrappedClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaElevationAngle,Q12 deltaAzimuthAngle)

{
  FieldGridDimensionCells auxiliaryElevationAngle;

  auxiliaryElevationAngle = deltaElevationAngle + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionHeight;
  if (-0x1000 < auxiliaryElevationAngle) {
    auxiliaryElevationAngle = -0x1000;
  }
  if (auxiliaryElevationAngle < -0x4000) {
    auxiliaryElevationAngle = -0x4000;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            (auxiliaryElevationAngle,
             deltaAzimuthAngle + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionWidth & 0xffff,
             g_InGameRuntimeRoot->fieldRegionOriginWorldYQ12_0BAC,
             g_InGameRuntimeRoot->fieldRegionOriginWorldXQ12_0BA8,
             &g_InGameRuntimeRoot->worldRuntime0A30);
  return;
}


/* Address: 0x004BE760.
   Returns the field grid's terrain height at a world point, or WORLD_HEIGHT_NO_FIELD_GRID when the world
   has no field grid. No caller found in src/ or the image tables.
*/
Q12 WorldRuntime_InterpolateTerrainHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 interpolatedHeightQ12;
  HeightSampleResult heightResult;

  interpolatedHeightQ12 = WORLD_HEIGHT_NO_FIELD_GRID;
  if (worldRuntime->fieldGrid != NULL) {
    heightResult = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    interpolatedHeightQ12 = heightResult.heightQ12;
  }
  return interpolatedHeightQ12;
}


/* Address: 0x004BE790.
   Returns the field grid's water surface height at a world point, or WORLD_HEIGHT_NO_FIELD_GRID when the
   world has no field grid. No caller found in src/ or the image tables.
*/
Q12 WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 waterSurfaceHeightQ12;
  HeightSampleResult heightResult;

  waterSurfaceHeightQ12 = WORLD_HEIGHT_NO_FIELD_GRID;
  if (worldRuntime->fieldGrid != NULL) {
    heightResult = FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    waterSurfaceHeightQ12 = heightResult.heightQ12;
  }
  return waterSurfaceHeightQ12;
}


/* Address: 0x004BE7C0.
   Returns the field grid's top surface height (terrain plus the water above it) at a world point, or
   WORLD_HEIGHT_NO_FIELD_GRID when the world has no field grid.
*/
uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  uint32_t topSurfaceHeightQ12;
  HeightSampleResult heightResult;

  topSurfaceHeightQ12 = WORLD_HEIGHT_NO_FIELD_GRID;
  if (worldRuntime->fieldGrid != NULL) {
    heightResult = FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    topSurfaceHeightQ12 = heightResult.heightQ12;
  }
  return topSurfaceHeightQ12;
}


/* Address: 0x0050A610.
   Drag selection test: projects the node's world position to the screen and returns CF set when that pixel
   lies inside the rectangle spanned by the two corners at +0x160/+0x164 and +0x168/+0x16C of boundsControl
   (inclusive, in either corner order).
*/
bool WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode100 *runtimeNode,WorldRuntimeExtendedMapControlView170 *boundsControl)

{
  int boundsSecondX;
  int boundsSecondY;
  int projectedScreenX;
  int boundsMaxX;
  int projectedScreenY;
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
  /* EAX = Q12 screen x, EDX = Q12 screen y */
  projectedScreenX = (int)projectedPositionPair >> 12;
  projectedScreenY = (int)((int64_t)projectedPositionPair >> (32 + 12));
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
  if ((((boundsMinX <= projectedScreenX) && (projectedScreenX <= boundsMaxX)) && (boundsMinY <= projectedScreenY)) &&
     (projectedScreenY <= boundsMaxY)) {
    return true;
  }
  return false;
}


/* Address: 0x0050D260.
   Saves the camera (position, magnitude, heading, pitch and committed distance) into worldRuntime->snapshot,
   to be restored later by WorldRuntime_RestoreMotionStateFromSnapshot.
*/
void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime)

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
   Compares the camera with the one saved by WorldRuntime_CaptureMotionStateToSnapshot (position, magnitude,
   heading, pitch, and the target distance against the saved committed distance). The original returns CF
   clear when all of them match and CF set otherwise; this version returns nothing (no caller found in src/
   or the image tables).
*/
void WorldRuntime_MotionStateMatchesSnapshot(WorldRuntimeContext *worldRuntime)

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
   Commits the camera's target distance (+0x8C) as its committed distance (+0x7C), the base that later
   distance input is added to.
*/
void WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world)

{
  (world->motion).committedDistanceQ12 = (world->motion).targetDistanceQ12;
  return;
}


/* Address: 0x0050D510.
   Attaches the pool of 0x100-byte object records that WorldObjectArray_AllocateFreeRecord hands out (callers
   attach 0x100 or 0x4000 records).
*/
void WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world)

{
  world->objectArray = objectArray;
  world->objectCount = count;
  return;
}


/* Address: 0x0050D540.
   Replaces the world's secondary control flags (runtimeControlFlags, +0xCC). No caller found in src/ or the
   image tables.
*/
void WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = flags;
  return;
}


/* Address: 0x0050D560.
   Sets the given bits in the world's secondary control flags (runtimeControlFlags, +0xCC). No caller found in
   src/ or the image tables.
*/
void WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags | flags;
  return;
}


/* Address: 0x0050D580.
   Clears the given bits in the world's secondary control flags (runtimeControlFlags, +0xCC). No caller found
   in src/ or the image tables.
*/
void WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags & ~flags;
  return;
}


/* Address: 0x0050D5A0.
   Toggles the given bits in the world's secondary control flags (runtimeControlFlags, +0xCC). No caller found
   in src/ or the image tables.
*/
void WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags ^ flags;
  return;
}


/* Address: 0x0050D610.
   Returns the camera position (motion.positionX/Y/ZQ12, context +0x60..+0x68) in EAX, ECX and EDX.
*/
WorldVector0EaxEcxEdx12 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world)

{
  WorldVector0EaxEcxEdx12 positionVector;

  positionVector.xQ12= (world->motion).positionXQ12;
  positionVector.yQ12 = (world->motion).positionYQ12;
  positionVector.zQ12 = (world->motion).positionZQ12;
  return positionVector;
}


/* Address: 0x0050D630.
   Returns the camera orientation (motion.positionMagnitudeQ12, headingAngle, pitchAngle, context
   +0x6C..+0x74) in EAX, ECX and EDX.
*/
WorldVector1EaxEcxEdx12 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world)

{
  WorldVector1EaxEcxEdx12 motionVector;

  motionVector.magnitudeQ12= (world->motion).positionMagnitudeQ12;
  motionVector.headingAngle = (world->motion).headingAngle;
  motionVector.pitchAngle = (world->motion).pitchAngle;
  return motionVector;
}


/* Address: 0x0050D650.
   Returns the world's secondary control flags (runtimeControlFlags, +0xCC) in EAX with CF clear. No caller
   found in src/ or the image tables.
*/
WorldFlagsResult WorldRuntime_GetFlags(WorldRuntimeContext *world)

{
  WorldFlagsResult flagsResult;
  
  flagsResult.failed = false;
  flagsResult.flags = world->runtimeControlFlags;
  return flagsResult;
}


/* Address: 0x0050D6A0.
   Returns the field grid attached by WorldRuntime_AttachFieldGridAsset (+0x54). No caller found in src/ or
   the image tables.
*/
FieldGridAsset * WorldRuntime_GetFieldGridAsset(WorldRuntimeContext *world)

{
  return world->fieldGrid;
}

/* Address: 0x0050D6D0.
   Returns the world's pending token (+0x5C) without consuming it (see WorldRuntime_TakePendingToken). No
   caller found in src/ or the image tables.
*/
uint32_t WorldRuntime_GetPendingToken(WorldRuntimeContext *world)

{
  return world->pendingToken;
}

/* Address: 0x0050D6F0.
   Consumes the world's pending token (+0x5C): returns it and leaves 0 behind. The original swaps it out with
   XCHG, i.e. atomically; here the swap is spelled out. No caller found in src/ or the image tables.
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
   Attaches a caller-owned workspace of count dwords to the world runtime (see WorldRuntime_GetDwordArray) and
   zeroes it.
*/
void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uint32_t *array,WorldRuntimeContext *world)

{
  world->dwordArray = array;
  world->dwordArrayCount = count;
  for (; count != 0; count--) {
    *array = 0;
    array = array + 1;
  }
  return;
}


/* Address: 0x0050D740.
   Returns the dword workspace attached by WorldRuntime_AttachAndClearDwordArray (+0xC0). No caller found in
   src/ or the image tables.
*/
uint32_t * WorldRuntime_GetDwordArray(WorldRuntimeContext *world)

{
  return world->dwordArray;
}

/* Address: 0x0050D7D0.
   Takes the first free record of the world's object pool (WorldRuntime_AttachObjectArray): marks it allocated
   (which also resets its other flag bits) and stores the owning world. Returns FATAL_ERROR_GENERAL_FAILURE with
   CF set when the pool is exhausted.
*/
WorldObjectAllocResult WorldObjectArray_AllocateFreeRecord(WorldRuntimeContext *worldRuntime)

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
      exhaustedResult.recordOrError = (WorldObjectRecord *)FATAL_ERROR_GENERAL_FAILURE;
      return exhaustedResult;
    }
    if (((recordCursor->common).allocationFlags & WORLD_OBJECT_RECORD_ALLOCATED) == 0) break;
    recordCursor = recordCursor + 1;
    recordsRemaining--;
  }
  (recordCursor->common).allocationFlags = WORLD_OBJECT_RECORD_ALLOCATED;
  (recordCursor->common).ownerWorld = worldRuntime;
  allocatedResult.failed = false;
  allocatedResult.recordOrError = recordCursor;
  return allocatedResult;
}


/* Address: 0x0050D830.
   Marks node as linked and puts it at the head of its world's owner list (head at +0xD8; the head is
   swapped with XCHG, the neighbour links are then set without a lock).
*/
void WorldRuntime_LinkNodeIntoOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 **ownerListHeadLink;
  WorldOwnerListNode100 *previousHeadNode;
  WorldRuntimeContext *ownerWorld;

  ownerWorld = node->ownerWorld;
  node->runtimeFlags = node->runtimeFlags | WORLD_OWNER_NODE_LINKED;
  LOCK();
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  UNLOCK();
  node->previousNode = NULL;
  node->nextNode = previousHeadNode;
  if (previousHeadNode != NULL) {
    previousHeadNode->previousNode = node;
  }
  return;
}


/* Address: 0x0050D880.
   Takes a linked node out of its world's owner list (fixing the neighbours or the list head) and clears all
   of its runtime flags, the linked mark included.
*/
void WorldRuntime_UnlinkNodeFromOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 *previousNode;
  WorldOwnerListNode100 *nextNode;

  if ((node->runtimeFlags & WORLD_OWNER_NODE_LINKED) != 0) {
    previousNode = node->previousNode;
    nextNode = node->nextNode;
    if (previousNode == NULL) {
      node->ownerWorld->ownerListHead = nextNode;
    }
    else {
      previousNode->nextNode = nextNode;
    }
    if (nextNode != NULL) {
      nextNode->previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
  return;
}


/* Address: 0x0050D8F0.
   Calls callback(callbackContext, node) for every node of the world's owner list (head at +0xD8), from the most
   recently linked one on.
*/
void WorldRuntime_ForEachNodeInOwnerListD8(void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world)

{
  WorldOwnerListNode100 *node;

  for (node = world->ownerListHead; node != NULL; node = node->nextNode) {
    callback(callbackContext,node);
  }
  return;
}


/* Address: 0x0050EC80.
   Pre-serializer provider of the light.hex save segment (called by
   InGameUiAction1210_ResourceRegistrationHelper): returns the shading runtime records (EAX) and their byte
   size 0x4000 (EDX), and inverts serializationToggleDword of record 0 so the saved image carries the
   inverted value; RuntimeHexSegment_ToggleLightImageFlag inverts it back after saving.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl RuntimeHexSegment_GetLightImageAndToggleFlagRegs(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  /* EDX:EAX = byte size 0x4000, shading runtime records */
  return ((uint64_t)0x4000 << 32) | (uint32_t)(uintptr_t)g_GraphicsShadingRuntimeRecords;
}

/* Address: 0x0050ECA0.
   Post-serializer hook of the light.hex save segment: inverts serializationToggleDword of shading record 0
   back (RuntimeHexSegment_GetLightImageAndToggleFlagRegs inverted it before), so the saved image carries the
   inverted value while the live one is unchanged. The caller keeps the serializer flags.
*/
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  return;
}

/* Address: 0x0050ECB0.
   Pre-serializer provider of the field.hex save segment (called by
   InGameUiAction1210_ResourceRegistrationHelper): returns the attached field grid (+0x54) and its whole
   allocation size (asset +0x04), so the field image is saved as one block. The original returns the image
   in EAX and the size in EDX.
*/
ResourceRegistrationImagePair
RuntimeHexSegment_GetFieldImageRegs(InGameFieldImageSaveContext58 *fieldImageContext)

{
  /* The pair is stored as {low: byte count (EDX), high: image (EAX)}, as the caller reads it. */
  return (uint64_t)(uintptr_t)fieldImageContext->fieldGridAsset << 0x20 |
         (uint64_t)(uint32_t)(fieldImageContext->fieldGridAsset->common).allocationSizeBytes;
}

/* Address: 0x0050ECD0.
   Post-serializer hook of the field.hex save segment (called by InGameUiAction1210_ResourceRegistrationHelper):
   does nothing; the field image needs no restoring after saving. The caller keeps the serializer flags.
*/
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext)

{
  return;
}

/* Address: 0x0051BFA0.
   Callback of WorldRuntime_ForEachNodeInOwnerListD8 from ArmyRuntime_DestroyInstanceAndRefreshUi: removes
   every reference to the destroyed object from one world node, so nothing keeps targeting it. For a model
   node: its hierarchy's targets and two fields of the entity linked at payload dword 2; for an effect node:
   its target at +0x1C.
*/
void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode100 *node)

{
  int *modelRuntime;
  int linkedRuntimeStateAddress;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    modelRuntime = node->runtimePayload;
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive((RuntimeToken)releasedObject,modelRuntime);
    /* the entity that owns the model (payload dword 2) */
    linkedRuntimeStateAddress = modelRuntime[2];
    if (releasedObject== *(void **)(linkedRuntimeStateAddress + 0x98)) {
      *(uint32_t *)(linkedRuntimeStateAddress + 0x98) = 0;
    }
    /* +0x1C is only a live reference while bit 0 of +0x2C is set; bits 0, 2 and 3 are cleared with it */
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
   Applies the terrain-class overlay of sourceRuntime's model definition at every model of the world's active
   faction: for each such owner-list node whose model has an overlay base (+0x19C of its first payload
   record), the overlay callback of the definition's terrain class runs at the node's position on the field
   grid. The extent is 0x800 << n for definitions of kind 0xE, else unlimited (-1).
   The definitionRecord[n] indexing below addresses fields of the definition record at fixed offsets.
*/
void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries(void *sourceRuntime,WorldRuntimeContext *worldRuntime)

{
  uint32_t overlayBaseOffset;
  TerrainClassOverlayCallback *overlayCallback;
  int modelOverlayBase;
  ModelDefinitionRecordPrefix *definitionRecord;
  WorldOwnerListNode100 *ownerNode;
  ModelDefinitionResult definitionLookup;
  uint32_t overlayExtent;
  
  if (sourceRuntime != NULL) {
    definitionLookup = ModelDefinitionRegistry_FindByIdWithError
                      (*(PckModelDefinitionIdCatalog *)(*(int *)((int)sourceRuntime + 0xc) + 0x20));
    definitionRecord = definitionLookup.modelDefinition;
    if (!definitionLookup.notFound) {
      overlayExtent = 0xffffffff;
      ownerNode = worldRuntime->ownerListHead;
      overlayBaseOffset = definitionRecord[0x23].flags;
      if (ownerNode != NULL) {
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
            overlayCallback(overlayExtent,-1,modelOverlayBase + overlayBaseOffset,ownerNode->worldYQ12,ownerNode->worldXQ12,
                      worldRuntime->fieldGrid);
          }
          ownerNode = ownerNode->nextNode;
        } while (ownerNode != NULL);
      }
    }
  }
  return;
}


/* Address: 0x005233F0.
   Per-tick update of army class 5 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[5]):
   that class has nothing to update, so this does nothing.
*/
void UnifiedRuntimeTable_Method5_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00523400.
   Per-tick update of army class 6 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[6]):
   that class has nothing to update, so this does nothing.
*/
void UnifiedRuntimeTable_Method6_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00527B70.
   Default model-unrebase handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase, every class
   except 13 and 21): those classes keep no pointers that need unrebasing, so this does nothing.
*/
void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

{
  return;
}


/* Address: 0x00527BA0.
   Default model release/commit handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit,
   every class except 14-16 and 21): those classes hold no faction capacity or placement reservation to release,
   so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpB
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Address: 0x00527BB0.
   One-argument default handler that returns 0. It sits next to the other defaults of
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, but no table slot or caller in src/ uses it.
*/
uint32_t UnifiedRuntimeDefault_OneArgReturnZero(void *context)

{
  return 0;
}


/* Address: 0x00527BE0.
   Default placement validation (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation, classes
   0, 5-9, 12 and 21): accepts every placement (CF clear).
*/
bool UnifiedRuntimeDefault_TwoArgSuccess
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  return false;
}


/* Address: 0x00527BF0.
   Default class method D (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD, classes 0, 9, 12,
   15, 20 and 23), the slot where the other classes update their looping and positioned sounds: these classes
   have none, so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Address: 0x00529430.
   WorldRuntime_ForEachNodeInOwnerListD8 callback run while a model runtime is destroyed
   (detachedObject = that model runtime): every effect (+0x1C), shot (+0x14) or entity (+0xF0, and +0x60 for
   definition class 0x15) that still points at it gets the pointer cleared, so nothing keeps a dangling reference.
*/
void WorldRuntimeNode_ClearDetachedEntityReferencesCallback(void *detachedObject,WorldOwnerListNode100 *node)

{
  int *entityRuntimeWords;

  if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    if (detachedObject == *(void **)((int)node->runtimePayload + 0x1c)) {
      *(uint32_t *)((int)node->runtimePayload + 0x1c) = 0;
    }
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    /* word 0 is the definition (class index at +0x4C), word 0x3C = +0xF0, word 0x18 = +0x60 */
    entityRuntimeWords = node->runtimePayload;
    if (detachedObject == (void *)entityRuntimeWords[0x3c]) {
      entityRuntimeWords[0x3c] = 0;
    }
    if ((*(int *)(*entityRuntimeWords + 0x4c) == 0x15) &&
       (detachedObject == (void *)entityRuntimeWords[0x18])) {
      entityRuntimeWords[0x18] = 0;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          (detachedObject == *(void **)((int)node->runtimePayload + 0x14))) {
    *(uint32_t *)((int)node->runtimePayload + 0x14) = 0;
  }
  return;
}


/* Address: 0x00565110.
   WorldRuntime_ForEachNodeInOwnerListD8 callback used when an in-game session shuts down, before the level
   resources are destroyed: destroys the army of every model node; for shot and effect nodes it clears flag bits
   31 (linked into the owner list) and 30 and zeroes one back-reference field of their runtime payload (+0x10 for
   shots, +4 for effects).
*/
void WorldRuntimeNode_ReleaseShutdownBindingsCallback(WorldRuntimeContext *shutdownContext,WorldOwnerListNode100 *node)

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
   Recomputes the camera's target point: the first point where the view ray (from the camera along its
   pitch and heading, up to four times the maximum camera distance) meets the field, i.e. the terrain or a
   nearer secondary surface (only the secondary surface with WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY).
   Without a hit the ray is intersected with the ground plane z = 0. Also updates the target distance.
*/
void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 currentPitchAngle;
  UQ12 hitDistanceQ12;
  uint32_t endpointDistanceQ12;
  int rayLengthOrOffsetY;
  FixedSinCosEdxEax8 groundOffsetXY;
  TerrainRaycastResult raycastResult;
  TerrainRaycastResult secondaryRaycastResult;
  FixedDirection endpointOffset;

  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY) == 0) {
    rayLengthOrOffsetY = worldRuntime->maximumCameraDistanceQ12 << 2;
    raycastResult = FieldGrid_RaycastTerrainSurfaceDistance
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,rayLengthOrOffsetY,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    hitDistanceQ12 = raycastResult.distanceQ12;
    if (raycastResult.hit) {
      /* terrain hit: a nearer secondary-surface hit wins */
      secondaryRaycastResult = FieldGrid_RaycastSecondarySurfaceDistance
                        ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,rayLengthOrOffsetY,
                         (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                         (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
      if ((secondaryRaycastResult.hit) && (secondaryRaycastResult.distanceQ12 < (int)hitDistanceQ12)) {
        hitDistanceQ12 = secondaryRaycastResult.distanceQ12;
      }
    }
  }
  else {
    raycastResult = FieldGrid_RaycastSecondarySurfaceDistance
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       worldRuntime->maximumCameraDistanceQ12 << 2,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    hitDistanceQ12 = raycastResult.distanceQ12;
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
  (worldRuntime->motion).targetDistanceQ12 = hitDistanceQ12;
  endpointOffset = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,hitDistanceQ12);
  (worldRuntime->motion).targetPositionXQ12 = endpointOffset.x + (worldRuntime->motion).positionXQ12;
  (worldRuntime->motion).targetPositionYQ12 = endpointOffset.y + (worldRuntime->motion).positionYQ12;
  (worldRuntime->motion).targetPositionZQ12 = endpointOffset.z + (worldRuntime->motion).positionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050D760.
   Stores the eight terrain lighting colours of the level (or of the current lighting-cycle blend) in the world
   runtime and rebuilds the terrain colour ramp from the two ramp colours and the base colour.
*/
void WorldRuntime_SetTerrainLightingConfiguration(PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
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
   Sets the terrain light direction (elevation, azimuth) and relights the field: recomputes the triangle normals
   and the directional lighting of the field grid. The auxiliary angle pair is only stored (in the fields named
   fieldRegion.regionHeight/regionWidth; callers clamp and wrap it like the light direction, elevation
   -0x4000..-0x1000, azimuth & 0xFFFF). Callers: level load, the periodic lighting cycle and the light-direction
   commands.
*/
void WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells auxiliaryElevationAngle,FieldGridDimensionCells auxiliaryAzimuthAngle,
          Q12 lightElevationAngle,Q12 lightAzimuthAngle,WorldRuntimeContext *worldRuntime)

{
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x1c) = lightAzimuthAngle;
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x20) = lightElevationAngle;
  FieldGrid_RecomputeInteriorTriangleNormalAngles(worldRuntime->fieldGrid);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (lightElevationAngle,lightAzimuthAngle,worldRuntime->fieldGrid);
  (worldRuntime->fieldRegion).regionWidth = auxiliaryAzimuthAngle;
  (worldRuntime->fieldRegion).regionHeight = auxiliaryElevationAngle;
  return;
}


/* Address: 0x0050D6B0.
   Clears WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY; called after every change of the camera state and when a
   field grid is attached.
*/
void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world)

{
  world->runtimeFlags = world->runtimeFlags & ~WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY;
  return;
}

