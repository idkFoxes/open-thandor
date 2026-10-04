/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/walker.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_WALKER_H
#define THANDOR_GAMEPLAY_ARMY_WALKER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/walker. */

/* ArmyRuntime_UpdateMovementAndWaypoints: the route end (fallbackPosition) counts as reached within
   +-0x40 exclusive, the final target (movementTarget*) within +-1.0 (Q12) inclusive on both axes */
#define ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 0x40
#define ARMY_MOVEMENT_TARGET_RADIUS_Q12 0x1000

/* Step state of the two-legged articulated walker (runtime-update slot 3), kept in
   ArmyArticulatedRuntimeSlotView.articulatedContact.fallbackPosition0Q12. The upper 16 bits hold half
   the heading change of a turn step (ArmyArticulatedRuntime_UpdateSelectedTerrainContact). */
#define ARMY_ARTICULATED_STEP_LEFT 0x1          /* the left foot is moving (progress runtimeStateA8) */
#define ARMY_ARTICULATED_STEP_RIGHT 0x2         /* the right foot is moving (progress terrainContactMode) */
#define ARMY_ARTICULATED_STEP_LEFT_LAST 0x4     /* the current/last step is by the left foot */
#define ARMY_ARTICULATED_STEP_RIGHT_LAST 0x8    /* the current/last step is by the right foot */
#define ARMY_ARTICULATED_STEP_TURN 0x10         /* turn on the spot; the other foot follows with the other half */
#define ARMY_ARTICULATED_STEP_WALK 0x20         /* walking step towards the route point */
#define ARMY_ARTICULATED_STEP_CLOSE 0x40        /* closing step: the feet are brought side by side again */
#define ARMY_ARTICULATED_STEP_OBSTRUCTED 0x80   /* the foot target was blocked; a second block cancels the step */
/* a new walking step keeps only WALK, OBSTRUCTED and bits 8-15; the foot, turn and close bits and the stored
   turn angle are cleared (0xffa0) */
#define ARMY_ARTICULATED_STEP_NEW_WALK_KEEP_MASK \
          (0xffff & ~(ARMY_ARTICULATED_STEP_LEFT | ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT_LAST | \
                      ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_CLOSE))
/* turn angle16 * this = half the turn angle in the upper 16 bits of the step state */
#define ARMY_ARTICULATED_STEP_HALF_TURN_SCALE 0x8000
/* step progress runs from 0 to 1.0 (Q12); the step ends once it exceeds 0xFFF */
#define ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 0x1000
/* Articulated walker step choice (ArmyRuntimeClass_UpdateArticulatedWalker), angles relative to the heading
   in angle16 units: a new step walks on while the route point lies within the eighth turn ahead (within
   ARMY_ARTICULATED_WALK_ON_ANGLE16 once walking), otherwise the walker turns on the spot when it is more than
   ARMY_ARTICULATED_TURN_ANGLE16 off. Standing, the feet are closed when their line is more than
   ARMY_ARTICULATED_FEET_SQUARE_ANGLE16 off square to the heading. */
#define ARMY_ARTICULATED_WALK_ON_ANGLE16 0x200
#define ARMY_ARTICULATED_TURN_ANGLE16 0x800
#define ARMY_ARTICULATED_FEET_SQUARE_ANGLE16 0x1000
/* Ground normals of the articulated walker as packed elevation << 16 | azimuth: straight up */
#define ARMY_ARTICULATED_NORMAL_UP (FIXED_ANGLE16_QUARTER_TURN << 16)

/* angle16 difference * this >> 16 sign-extends it to the shortest signed turn (-0x8000..0x7fff); the
   multiplication form is kept because a shift compiles differently */
#define ARMY_ANGLE16_SIGN_EXTEND_SCALE 0x10000

/* Functions are grouped by semantic ownership. */

void ArmyRuntimeClass_UpdateArticulatedMovement (WorldRuntimeContext *worldRuntime, ModelRuntimeArticulatedMovementDefinitionView *modelRuntime);

void ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_UpdateLeftTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_UpdateRightTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_UpdateContactChildAndEffects(ModelRuntimeNode *legNode,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot *modelRuntime);

void ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

void ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_WALKER_H */
