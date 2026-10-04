/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/placement_contact.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_PLACEMENT_CONTACT_H
#define THANDOR_GAMEPLAY_ARMY_PLACEMENT_CONTACT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/placement_contact. */

/* Functions are grouped by semantic ownership. */

void ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_InitializeArticulatedSuspension
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

extern ArmyPlacementContactCallbackTable5 g_ArmyPlacementContactKindDispatchTable;

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_CONTACT_H */
