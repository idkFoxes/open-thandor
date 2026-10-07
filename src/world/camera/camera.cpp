/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/camera/camera.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Camera state of the world runtime: position, angles and distance, the target point on the field surface
   and snapshots of the motion state. */

#include <thandor/world/camera/camera.h>
#include <thandor/thandor.h>

/* Moves the camera (motion.position) to the given point and keeps its target point (motion.targetPosition): the
   target and committed distances become the new distance between the two.
*/
void WorldRuntime_SetCameraPositionKeepingTarget
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime)

{
  uint32_t targetDistanceQ12;
  
  runtime->motion.positionXQ12 = positionX;
  runtime->motion.positionYQ12 = positionY;
  runtime->motion.positionZQ12 = positionZ;
  targetDistanceQ12 =
       FixedMath_Length3(positionZ - runtime->motion.targetPositionZQ12,
                         positionY - runtime->motion.targetPositionYQ12,
                         positionX - runtime->motion.targetPositionXQ12);
  runtime->motion.targetDistanceQ12 = targetDistanceQ12;
  runtime->motion.committedDistanceQ12 = targetDistanceQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
}

/* Sets the camera's magnitude (at least 0x400 = 0.25 in Q12), heading (16-bit turn) and pitch and the
   projection shift (motion.projectionShift). The pitch is clamped to the world's pitch limits (unless the camera is unlimited)
   and always to a quarter turn up or down (+-0x4000).
*/
void WorldRuntime_SetCameraAnglesAndMagnitudeClamped
          (WorldMotionValue78 projectionShift,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime)

{
  if (!Any(runtime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA)) {
    if ((int)(runtime->motion).maximumPitchAngle < (int)pitchAngle) {
      pitchAngle = runtime->motion.maximumPitchAngle;
    }
    else if ((int)pitchAngle < (int)(runtime->motion).minimumPitchAngle) {
      pitchAngle = runtime->motion.minimumPitchAngle;
    }
  }
  if ((int)magnitude < WORLD_MOTION_MINIMUM_MAGNITUDE_Q12) {
    magnitude = WORLD_MOTION_MINIMUM_MAGNITUDE_Q12;
  }
  if ((int)pitchAngle < FIXED_ANGLE16_QUARTER_TURN + 1) {
    if ((int)pitchAngle < -FIXED_ANGLE16_QUARTER_TURN) {
      pitchAngle = -FIXED_ANGLE16_QUARTER_TURN;
    }
  }
  else {
    pitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  runtime->motion.positionMagnitudeQ12 = magnitude;
  runtime->motion.headingAngle = headingAngle & FIXED_ANGLE16_MASK;
  runtime->motion.pitchAngle = pitchAngle;
  runtime->motion.projectionShift = projectionShift;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
}

/* Points the camera at a target: stores the target point (motion.targetPosition), pitch, heading and
   distance, and places the camera (motion.position) that distance away from the target, looking at it
   along the given angles (the offset uses the reversed direction: negated pitch, heading + half a turn).
*/
void WorldRuntime_PointCameraAtTarget
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime)

{
  FixedDirection directionOffset;

  runtime->motion.targetPositionXQ12 = originX;
  runtime->motion.targetPositionYQ12 = originY;
  runtime->motion.targetPositionZQ12 = originZ;
  runtime->motion.pitchAngle = pitchAngle;
  runtime->motion.headingAngle = headingAngle;
  runtime->motion.targetDistanceQ12 = distance;
  runtime->motion.committedDistanceQ12 = distance;
  directionOffset =
       FixedMath_DirectionFromAnglesScaled(-pitchAngle,headingAngle ^ FIXED_ANGLE16_HALF_TURN,distance);
  runtime->motion.positionXQ12 = directionOffset.x + runtime->motion.targetPositionXQ12;
  runtime->motion.positionYQ12 = directionOffset.y + runtime->motion.targetPositionYQ12;
  runtime->motion.positionZQ12 = directionOffset.z + runtime->motion.targetPositionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
}

/* Restores the camera saved by WorldRuntime_CaptureMotionStateToSnapshot (position, magnitude, angles,
   distance) and recomputes its target point where the view ray meets the field.
*/
void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 snapshotHeadingAngle;
  AngleTurn32 snapshotPitchAngle;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  UQ12 snapshotDistanceQ12;
  
  snapshotPositionYQ12 = worldRuntime->snapshot.positionYQ12;
  snapshotPositionZQ12 = worldRuntime->snapshot.positionZQ12;
  worldRuntime->motion.positionXQ12 = worldRuntime->snapshot.positionXQ12;
  worldRuntime->motion.positionYQ12 = snapshotPositionYQ12;
  worldRuntime->motion.positionZQ12 = snapshotPositionZQ12;
  snapshotHeadingAngle = worldRuntime->snapshot.headingAngle;
  snapshotPitchAngle = worldRuntime->snapshot.pitchAngle;
  snapshotDistanceQ12 = worldRuntime->snapshot.distanceQ12;
  worldRuntime->motion.positionMagnitudeQ12 = worldRuntime->snapshot.magnitudeQ12;
  worldRuntime->motion.headingAngle = snapshotHeadingAngle;
  worldRuntime->motion.pitchAngle = snapshotPitchAngle;
  worldRuntime->motion.targetDistanceQ12 = snapshotDistanceQ12;
  worldRuntime->motion.committedDistanceQ12 = snapshotDistanceQ12;
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
}

/* Saves the camera (position, magnitude, heading, pitch and committed distance) into worldRuntime->snapshot,
   to be restored later by WorldRuntime_RestoreMotionStateFromSnapshot.
*/
void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime)

{
  UQ12 snapshotDistanceQ12;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  AngleTurn32 snapshotHeadingAngle;
  AngleTurn32 snapshotPitchAngle;
  
  snapshotPositionYQ12 = worldRuntime->motion.positionYQ12;
  snapshotPositionZQ12 = worldRuntime->motion.positionZQ12;
  worldRuntime->snapshot.positionXQ12 = worldRuntime->motion.positionXQ12;
  worldRuntime->snapshot.positionYQ12 = snapshotPositionYQ12;
  worldRuntime->snapshot.positionZQ12 = snapshotPositionZQ12;
  snapshotHeadingAngle = worldRuntime->motion.headingAngle;
  snapshotPitchAngle = worldRuntime->motion.pitchAngle;
  snapshotDistanceQ12 = worldRuntime->motion.committedDistanceQ12;
  worldRuntime->snapshot.magnitudeQ12 = worldRuntime->motion.positionMagnitudeQ12;
  worldRuntime->snapshot.headingAngle = snapshotHeadingAngle;
  worldRuntime->snapshot.pitchAngle = snapshotPitchAngle;
  worldRuntime->snapshot.distanceQ12 = snapshotDistanceQ12;
}

/* Commits the camera's target distance (motion.targetDistanceQ12) as its committed distance
   (motion.committedDistanceQ12), the base that later
   distance input is added to.
*/
void WorldRuntime_CommitCameraTargetDistance(WorldRuntimeContext *world)

{
  world->motion.committedDistanceQ12 = world->motion.targetDistanceQ12;
}

/* Returns the camera position (motion.positionX/Y/ZQ12).
*/
WorldCameraPosition WorldRuntime_GetCameraPosition(WorldRuntimeContext *world)

{
  WorldCameraPosition positionVector;

  positionVector.xQ12 = world->motion.positionXQ12;
  positionVector.yQ12 = world->motion.positionYQ12;
  positionVector.zQ12 = world->motion.positionZQ12;
  return positionVector;
}

/* Returns the camera orientation (motion.positionMagnitudeQ12, headingAngle, pitchAngle).
*/
WorldCameraOrientation WorldRuntime_GetCameraOrientation(WorldRuntimeContext *world)

{
  WorldCameraOrientation motionVector;

  motionVector.magnitudeQ12 = world->motion.positionMagnitudeQ12;
  motionVector.headingAngle = world->motion.headingAngle;
  motionVector.pitchAngle = world->motion.pitchAngle;
  return motionVector;
}

/* Recomputes the camera's target point: the first point where the view ray (from the camera along its
   pitch and heading, up to four times the maximum camera distance) meets the field, i.e. the terrain or a
   nearer secondary surface (only the secondary surface with WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY).
   Without a hit the ray is intersected with the ground plane z = 0. Also updates the target distance.
*/
void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 currentPitchAngle;
  UQ12 hitDistanceQ12;
  uint32_t endpointDistanceQ12;
  int rayLengthQ12;
  int groundOffsetY;
  FixedSinCos groundOffsetXY;
  Bool8 surfaceHit;
  Q12 rayDistanceQ12;
  Q12 secondaryDistanceQ12;
  FixedDirection endpointOffset;

  if (!Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY)) {
    rayLengthQ12 = worldRuntime->maximumCameraDistanceQ12 << 2;
    surfaceHit = FieldGrid_RaycastTerrainSurfaceDistance
                      (worldRuntime->motion.pitchAngle,worldRuntime->motion.headingAngle,rayLengthQ12,
                       worldRuntime->motion.positionZQ12,worldRuntime->motion.positionYQ12,
                       worldRuntime->motion.positionXQ12,worldRuntime->fieldGrid,&rayDistanceQ12,nullptr);
    hitDistanceQ12 = rayDistanceQ12;
    if (surfaceHit) {
      /* terrain hit: a nearer secondary-surface hit wins */
      if ((FieldGrid_RaycastSecondarySurfaceDistance
                        (worldRuntime->motion.pitchAngle,worldRuntime->motion.headingAngle,rayLengthQ12,
                         worldRuntime->motion.positionZQ12,worldRuntime->motion.positionYQ12,
                         worldRuntime->motion.positionXQ12,worldRuntime->fieldGrid,&secondaryDistanceQ12)) &&
          (secondaryDistanceQ12 < (int)hitDistanceQ12)) {
        hitDistanceQ12 = secondaryDistanceQ12;
      }
    }
  }
  else {
    surfaceHit = FieldGrid_RaycastSecondarySurfaceDistance
                      (worldRuntime->motion.pitchAngle,worldRuntime->motion.headingAngle,
                       worldRuntime->maximumCameraDistanceQ12 << 2,
                       worldRuntime->motion.positionZQ12,worldRuntime->motion.positionYQ12,
                       worldRuntime->motion.positionXQ12,worldRuntime->fieldGrid,&rayDistanceQ12);
    hitDistanceQ12 = rayDistanceQ12;
  }
  if (!surfaceHit) {
    /* no hit: intersect the view ray with the ground plane z = 0 */
    currentPitchAngle = worldRuntime->motion.pitchAngle;
    const int32_t pitchSineQ28 = g_FixedSineQ28[(int32_t)(FIXED_SINE_TABLE_SIN - currentPitchAngle)];
    if (pitchSineQ28 == 0) {
      /* The original divides by the sine of the pitch here and traps (integer divide by zero) when the
         pitch is exactly horizontal (reachable with the unlimited camera and in the menu room); bounded
         here because a horizontal ray never meets the ground plane: the previous target is kept. */
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
      return;
    }
    groundOffsetXY = FixedMath_SinCosScaled
                      (worldRuntime->motion.headingAngle,
                       (FixedMathScale32)
                       (((int64_t)(worldRuntime->motion).positionZQ12 *
                        (int64_t)g_FixedSineQ28[(int32_t)(FIXED_SINE_TABLE_COS - currentPitchAngle)]) /
                       (int64_t)pitchSineQ28));
    groundOffsetY = groundOffsetXY.sinValue;
    worldRuntime->motion.targetPositionXQ12 = groundOffsetXY.cosValue + worldRuntime->motion.positionXQ12;
    worldRuntime->motion.targetPositionYQ12 = groundOffsetY + worldRuntime->motion.positionYQ12;
    worldRuntime->motion.targetPositionZQ12 = 0;
    endpointDistanceQ12 =
         FixedMath_Length3(worldRuntime->motion.positionZQ12,groundOffsetY,groundOffsetXY.cosValue);
    worldRuntime->motion.targetDistanceQ12 = endpointDistanceQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
    return;
  }
  worldRuntime->motion.targetDistanceQ12 = hitDistanceQ12;
  endpointOffset = FixedMath_DirectionFromAnglesScaled
                    (worldRuntime->motion.pitchAngle,worldRuntime->motion.headingAngle,hitDistanceQ12);
  worldRuntime->motion.targetPositionXQ12 = endpointOffset.x + worldRuntime->motion.positionXQ12;
  worldRuntime->motion.targetPositionYQ12 = endpointOffset.y + worldRuntime->motion.positionYQ12;
  worldRuntime->motion.targetPositionZQ12 = endpointOffset.z + worldRuntime->motion.positionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Clears WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY; called after every change of the camera state and when a
   field grid is attached.
*/
void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world)

{
  world->runtimeFlags = world->runtimeFlags & ~WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY;
}
