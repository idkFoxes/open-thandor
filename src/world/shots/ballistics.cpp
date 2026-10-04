/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/ballistics.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/ballistics.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/shots/ballistics. */

/* Computes the heading and elevation (Angle16) at which a shot of this definition must leave launchPoint to reach
   targetPoint. Ballistic shots solve the projectile equation (gravity = ballisticDivisorQ12) and take the high arc,
   the low arc only when the height difference is below 1.0; fixed-range shots always go straight up (0, 0x4000);
   all others aim directly, raised by the definition's elevation offset and capped at straight up. The callers
   pass the points in Z, Y, X order (Z = height). Called directly by the weapon aiming code (gameplay/army/combat.c,
   movement.c, runtime.c) and the shot launch in world/shots/runtime.c.
*/
ShotLaunchAngles ShotDefinition_ComputeLaunchAngles
          (Q12 targetZ,Q12 targetY,Q12 targetX,Q12 launchZ,Q12 launchY,Q12 launchX,
          ShotDefinition *definition)

{
  int64_t discriminant;
  int rangeTimesDivisor;
  int launchSpeedSquared;
  int heightDelta;
  uint32_t computedElevation;
  uint32_t computedHeading;
  ShotLaunchAngles clampedAngles;
  ShotLaunchAngles fixedRangeAngles;
  FixedLengthAngle planarLengthAngle;
  ShotLaunchAngles launchAngles;
  FixedVectorAngles vectorAngles;

  heightDelta = targetZ - launchZ;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    planarLengthAngle = FixedMath_Vector2AngleAndLength(targetY - launchY,targetX - launchX);
    computedHeading = planarLengthAngle.angle & FIXED_ANGLE16_MASK;
    /* tan(elevation) = (v^2 +- sqrt(v^4 - 2*g*h*v^2 - (g*r)^2)) / (g*r) */
    launchSpeedSquared = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    rangeTimesDivisor = planarLengthAngle.length * definition->ballisticDivisorQ12;
    discriminant =
         (int64_t)(launchSpeedSquared + definition->ballisticDivisorQ12 * heightDelta * -2) *
         (int64_t)launchSpeedSquared -
         (int64_t)rangeTimesDivisor * (int64_t)rangeTimesDivisor;
    if (discriminant < 0) {
      discriminant = 0; /* target out of reach */
    }
    computedElevation =
         FIXED_UINT64_SQRT(discriminant);
    if ((-Q12_ONE < heightDelta) && (heightDelta < Q12_ONE)) {
      computedElevation = -computedElevation;
    }
    computedElevation = FixedMath_Atan2Angle16(launchSpeedSquared + computedElevation,rangeTimesDivisor);
  }
  else {
    if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
      fixedRangeAngles.headingAngle = 0;
      fixedRangeAngles.elevationAngle = FIXED_ANGLE16_QUARTER_TURN;
      return fixedRangeAngles;
    }
    vectorAngles = FixedMath_VectorToAngles(heightDelta,targetY - launchY,targetX - launchX);
    computedHeading = vectorAngles.azimuthAngle;
    computedElevation = vectorAngles.elevationAngle + definition->elevationOffsetAngle16;
    if (FIXED_ANGLE16_QUARTER_TURN < (int)computedElevation) {
      clampedAngles.elevationAngle = FIXED_ANGLE16_QUARTER_TURN;
      clampedAngles.headingAngle = computedHeading;
      return clampedAngles;
    }
  }
  launchAngles.elevationAngle = computedElevation;
  launchAngles.headingAngle = computedHeading;
  return launchAngles;
}

/* Returns how far a shot of this definition reaches, used when a model's weapons are summed up for target
   selection: ballistic shots 9/8 of speed^2 / divisor, fixed-range shots their stored range (mode2SelectionRangeQ12), all
   others speed times the flight time, where the ramp-up ticks count only one third.
*/
uint32_t ShotDefinition_ComputeSelectionRange(ShotDefinition *definition)

{
  uint32_t selectionRangeQ12;

  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    selectionRangeQ12 =
         (uint32_t)((int)(((int64_t)definition->launchSpeedQ12 * (int64_t)definition->launchSpeedQ12)
                     / (int64_t)definition->ballisticDivisorQ12) * 9) >> 3;
  }
  else if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
    selectionRangeQ12 = definition->mode2SelectionRangeQ12;
  }
  else {
    /* lifetime - 2/3 of the ramp duration (-2/3 in Q12 = -0xAAA) */
    selectionRangeQ12 =
         (((int)(definition->trajectoryRampDurationTicks * -(Q12_ONE * 2 / 3)) >> Q12_SHIFT) +
         definition->projectileLifetimeTicks) * definition->launchSpeedQ12;
  }
  return selectionRangeQ12;
}

/* Returns the shot speed used to lead a moving target: the launch speed (launchSpeedQ12) for unguided shots
   that do not fly a direct line, INT32_MAX (no lead) for direct-line or guided (guidanceTurnLimitAngle16
   non-zero) shots. Called directly by the target aim-point computation in gameplay/army/runtime.c.
*/
Q12 ShotDefinition_GetLeadSpeed(ShotDefinition *definition)

{
  uint32_t leadSpeedQ12;

  leadSpeedQ12 = INT32_MAX;
  if ((definition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    leadSpeedQ12 = definition->launchSpeedQ12;
  }
  return (Q12)leadSpeedQ12;
}

/* Returns the extra lead time for unguided lead-adjusted shots (trajectory mode 3, guidanceTurnLimitAngle16
   zero): about two thirds (0xAB / 256) of the ramp-up ticks (trajectoryRampDurationTicks), during which the shot is still accelerating; 0 for all
   other shots. The aim-point computation in gameplay/army/runtime.c multiplies it by the target's speed.
*/
uint32_t ShotDefinition_ComputeRampUpLeadTime(ShotDefinition *definition)

{
  uint32_t leadAdjustmentQ12;
  
  leadAdjustmentQ12 = 0;
  if ((definition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    leadAdjustmentQ12 = definition->trajectoryRampDurationTicks * 171 >> 8;
  }
  return leadAdjustmentQ12;
}
