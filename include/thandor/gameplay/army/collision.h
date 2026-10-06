/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/collision.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_COLLISION_H
#define THANDOR_GAMEPLAY_ARMY_COLLISION_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

Bool8 ArmyCollision_TestPointAgainstRuntimeList
          (Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,WorldRuntimeContext *worldRuntime);

ModelRuntimeSlot *ArmyCollision_FindBlockingRuntimeForCurrentUnit
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime);

Bool8 ArmyPlacementCollision_TestPointAgainstRuntimeList
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime);

Bool8 ArmyPlacementCollision_TestCandidateAgainstRuntimeList
          (WorldOwnerListNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          ModelRuntimeSlot *candidateRuntime,Q12 radiusQ12,WorldRuntimeContext *worldRuntime);

Bool8 ArmyPlacementCollision_TestCurrentRuntime
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

Bool8 ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeSlot *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_COLLISION_H */
