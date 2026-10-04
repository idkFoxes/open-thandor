/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/class_dispatch.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_CLASS_DISPATCH_H
#define THANDOR_GAMEPLAY_ARMY_CLASS_DISPATCH_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/class_dispatch. */

/* Functions are grouped by semantic ownership. */

void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode);

void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode);

void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode *node);

void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *node);

Bool8 ArmyRuntimeNode_DispatchTypedCallback(Ptr32<ArmyRuntimeSlot> *armyRuntimeHolder,WorldRuntimeContext *worldRuntime);

void ArmyRuntime_DispatchClassCommand(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime);

void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

extern ArmyRuntimeOrderHandlerMatrix11x24 g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes;
extern RuntimeMaintenanceCallbackPhasesTyped g_RuntimeMaintenanceCallbackPhases;

#endif /* THANDOR_GAMEPLAY_ARMY_CLASS_DISPATCH_H */
