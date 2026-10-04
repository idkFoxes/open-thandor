/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/factory.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_FACTORY_H
#define THANDOR_GAMEPLAY_ARMY_FACTORY_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/factory. */

/* Production state in model runtime classState.behaviorState of the unit factory (class 13) and production
   class 11 (0/1 only) */
#define ARMY_FACTORY_STATE_IDLE 0
#define ARMY_FACTORY_STATE_BUILDING 1
#define ARMY_FACTORY_STATE_OPENING 2       /* door opens, then the new army is sent to the exit point */
#define ARMY_FACTORY_STATE_WAITING_EXIT 3  /* until the new army no longer links back to the factory */
#define ARMY_FACTORY_STATE_CLOSING 4

/* Camera heading of the "army built" notification relative to the new army's heading: unit factory (class 13)
   and production class 11 */
#define ARMY_FACTORY_NOTIFICATION_HEADING_OFFSET_ANGLE16 0x8800U
#define ARMY_PRODUCTION_NOTIFICATION_HEADING_OFFSET_ANGLE16 0x1800

/* Functions are grouped by semantic ownership. */

void ArmyRuntimeClass_UpdateUnitFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

void ArmyRuntimeClass_UpdateStructureFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

Bool8 ArmyRuntimeSpawner_CreateLinkedChildInstance
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

Bool8 ArmyRuntime_TestArmyNearFactoryExit
          (ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime);

uint32_t ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric(ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

void EffectLifecycle_SpawnArmyFromOwner(WorldRuntimeContext *worldRuntime,GameEntityRuntime *ownerEntity);

#endif /* THANDOR_GAMEPLAY_ARMY_FACTORY_H */
