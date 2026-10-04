/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/class_updates.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_CLASS_UPDATES_H
#define THANDOR_GAMEPLAY_ARMY_CLASS_UPDATES_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView *modelRuntime);

void ArmyRuntime_ClassCommandHandlerGroupA(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView *modelRuntime);

ArmySegmentMeter ArmyRuntime_GetLinkedChildSlotMeter(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime);

int ArmyRuntime_GetAttachmentEffectVariantMask(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime);

void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView *modelRuntime);

Bool8 ArmyRuntime_TestModelAttachmentProximity(ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime);

Bool8 ArmyRuntime_TestPositionDistanceWithinCombinedRadius
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidateModelNode,
          void *sourceModelNode);

void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime);

void ArmyRuntime_UpdateAnimatedModelSubnodes(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_CLASS_UPDATES_H */
