/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/placement_release.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_PLACEMENT_RELEASE_H
#define THANDOR_GAMEPLAY_ARMY_PLACEMENT_RELEASE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/placement_release. */

/* Functions are grouped by semantic ownership. */

void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

void ArmyPlacement_ReleaseFactionCapacity(ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

void ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_RELEASE_H */
